#include "sprite.h"

void simple_sprite(float *out, float left, float top, float width, float height, float atlas_inverse_width, float atlas_inverse_height) {
    out[0] = left * atlas_inverse_width;
    out[1] = top * atlas_inverse_height;
    out[2] = (left + width) * atlas_inverse_width;
    out[3] = (top + height) * atlas_inverse_height;
}

sprite *create_sprite(int left, int top, int width, int height, int offset_x, int offset_y, float atlas_inverse_width, float atlas_inverse_height, float scale) {
    sprite *s = safe_malloc(sizeof(sprite));

    s->left = (float)left * atlas_inverse_width;
    s->top = (float)top * atlas_inverse_height;
    s->right = (float)(left + width) * atlas_inverse_width;
    s->bottom = (float)(top + height) * atlas_inverse_height;

    s->width = (float)width * scale;
    s->height = (float)height * scale;
    s->half_width = s->width * 0.5;

    s->offset_x = (float)offset_x * scale;
    s->offset_y = (float)offset_y * scale;

    return s;
}
