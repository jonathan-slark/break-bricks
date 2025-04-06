#include <cglm/struct.h> // vec2s, glms_vec2_add

#include "sprite.h"

Sprite sprite_create(vec2s pos, vec2s size, vec2s texOffset, vec2s texSize)
{
    Sprite s;

    s.size = size;
    sprite_setPos(&s, pos);

    // Normalise tex offset
    float u1 = texOffset.x / texSize.s;
    float v1 = texOffset.y / texSize.t;
    float u2 = (texOffset.x + size.s - 1) / texSize.s;
    float v2 = (texOffset.y + size.y - 1) / texSize.t;

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

void sprite_addVec(Sprite* s, vec2s v)
{
    for (size_t i = 0; i < VERT_COUNT; i++)
    {
        s->verts[i].pos = glms_vec2_add(s->verts[i].pos, v);
    }
}

// Baisc AABB (axis-aligned bounding box) collision check
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
