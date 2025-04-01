#include <cglm/struct.h> // vec2s, glms_vec2_add

#include "sprite.h"

Sprite sprite_create(vec2s pos, vec2s size, vec2s texOffset, vec2s texSize)
{
    Sprite s;

    sprite_setPos(&s, pos, size);

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

void sprite_setPos(Sprite* s, vec2s pos, vec2s size)
{
    float x1 = pos.x;
    float y1 = pos.y;
    float x2 = pos.x + size.s - 1;
    float y2 = pos.y + size.t - 1;

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
