#include <cglm/struct.h> // vec2s, glms_vec2_add
#include <math.h>        // INFINITY, fmaxf, fminf

#include "sprite.h"

// Function definitions

Sprite sprite_create(vec2s pos, vec2s size, vec2s texOff, vec2s texSize)
{
    Sprite s;

    s.size = size;
    sprite_setPos(&s, pos);

    // Normalise tex offset
    float u1 = texOff.x / texSize.s;
    float v1 = texOff.y / texSize.t;
    float u2 = (texOff.x + size.s - 1) / texSize.s;
    float v2 = (texOff.y + size.y - 1) / texSize.t;

    s.verts[0].texCoord = (vec2s) {{ u1, v1 }};
    s.verts[1].texCoord = (vec2s) {{ u2, v1 }};
    s.verts[2].texCoord = (vec2s) {{ u2, v2 }};
    s.verts[3].texCoord = (vec2s) {{ u1, v2 }};

    return s;
}

void sprite_setPos(Sprite* s, vec2s pos)
{
    float x1 = pos.x;
    float y1 = pos.y;
    float x2 = pos.x + s->size.s - 1;
    float y2 = pos.y + s->size.t - 1;

    s->verts[0].pos = (vec2s) {{ x1, y1 }};
    s->verts[1].pos = (vec2s) {{ x2, y1 }};
    s->verts[2].pos = (vec2s) {{ x2, y2 }};
    s->verts[3].pos = (vec2s) {{ x1, y2 }};
}

void sprite_posAdd(Sprite* s, vec2s v)
{
    for (size_t i = 0; i < VERT_COUNT; i++) {
        s->verts[i].pos = glms_vec2_add(s->verts[i].pos, v);
    }
}

void sprite_texOffAdd(Sprite* s, vec2s v)
{
    for (size_t i = 0; i < VERT_COUNT; i++) {
        s->verts[i].texCoord = glms_vec2_add(s->verts[i].texCoord, v);
    }
}

// Basic AABB (axis-aligned bounding box) collision check
bool sprite_checkCollision(Sprite a, Sprite b)
{
    bool collisionX = a.pos.x + a.size.x >= b.pos.x && b.pos.x + b.size.x >= a.pos.x;
    bool collisionY = a.pos.y + a.size.y >= b.pos.y && b.pos.y + b.size.y >= a.pos.y;
    return collisionX && collisionY;
}

// Returns true if a and b collide. If so, fills out `normal` with the collision direction.
bool sprite_checkCollisionEx(Sprite a, Sprite b, vec2s* normal)
{
    float ax1 = a.pos.x;
    float ay1 = a.pos.y;
    float ax2 = a.pos.x + a.size.x;
    float ay2 = a.pos.y + a.size.y;

    float bx1 = b.pos.x;
    float by1 = b.pos.y;
    float bx2 = b.pos.x + b.size.x;
    float by2 = b.pos.y + b.size.y;

    if (ax2 < bx1 || ax1 > bx2 || ay2 < by1 || ay1 > by2)
        return false;

    float overlapLeft   = ax2 - bx1;
    float overlapRight  = bx2 - ax1;
    float overlapTop    = ay2 - by1;
    float overlapBottom = by2 - ay1;

    float minOverlapX = overlapLeft < overlapRight  ? overlapLeft : overlapRight;
    float minOverlapY = overlapTop  < overlapBottom ? overlapTop  : overlapBottom;

    // Determine collision direction
    if (minOverlapX < minOverlapY)
    {
        if (overlapLeft < overlapRight)
            *normal = (vec2s) {{ -1.0f, 0.0f }}; // Hit from left
        else
            *normal = (vec2s) {{ 1.0f, 0.0f }};  // Hit from right
    }
    else
    {
        if (overlapTop < overlapBottom)
            *normal = (vec2s) {{ 0.0f, -1.0f }}; // Hit from top
        else
            *normal = (vec2s) {{ 0.0f, 1.0f }};  // Hit from bottom
    }

    return true;
}

// Swept AABB collision detection function. 'moving' is the ball, 'movement'
// is its full movement vector for the frame, 'target' is the static object
// (paddle, brick, or wall), and 'normal' will be set to the collision normal.
// Returns a collision time in [0.0, 1.0]. If 1.0 is returned, no collision
// occurred.
float sprite_sweptAABB(Sprite moving, vec2s movement, Sprite target, vec2s* normal)
{
    float xInvEntry, yInvEntry; float xInvExit, yInvExit;
    if (movement.x > 0.0f) {
        xInvEntry = target.pos.x - (moving.pos.x + moving.size.s);
        xInvExit  = (target.pos.x + target.size.s) - moving.pos.x;
    } else {
        xInvEntry = (target.pos.x + target.size.s) - moving.pos.x;
        xInvExit  = target.pos.x - (moving.pos.x + moving.size.s);
    }

    if (movement.y > 0.0f) {
        yInvEntry = target.pos.y - (moving.pos.y + moving.size.t);
        yInvExit  = (target.pos.y + target.size.t) - moving.pos.y;
    } else {
        yInvEntry = (target.pos.y + target.size.t) - moving.pos.y;
        yInvExit  = target.pos.y - (moving.pos.y + moving.size.t);
    }

    float xEntry, yEntry;
    float xExit,  yExit;

    if (movement.x == 0.0f) {
        xEntry = -INFINITY;
        xExit  = INFINITY;
    } else {
        xEntry = xInvEntry / movement.x;
        xExit  = xInvExit / movement.x;
    }

    if (movement.y == 0.0f) {
        yEntry = -INFINITY;
        yExit  = INFINITY;
    } else {
        yEntry = yInvEntry / movement.y;
        yExit  = yInvExit / movement.y;
    }

    float entryTime = fmaxf(xEntry, yEntry);
    float exitTime  = fminf(xExit, yExit);

    // No collision if there is no overlap during the movement
    if (entryTime > exitTime || (xEntry < 0.0f && yEntry < 0.0f) || entryTime > 1.0f) {
        normal->x = 0.0f;
        normal->y = 0.0f;
        return 1.0f;
    } else {
        // Determine the collision normal based on which axis had the later entry
        if (xEntry > yEntry) {
            if (xInvEntry < 0.0f) {
                normal->x = 1.0f;
                normal->y = 0.0f;
            } else {
                normal->x = -1.0f;
                normal->y = 0.0f;
            }
        } else {
            if (yInvEntry < 0.0f) {
                normal->x = 0.0f;
                normal->y = 1.0f;
            } else {
                normal->x = 0.0f;
                normal->y = -1.0f;
            }
        }
        return entryTime;
    }
}
