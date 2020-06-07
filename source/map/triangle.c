#include "triangle.h"

triangle *create_triangle(float height, int texture, vec va, vec vb, vec vc, bool floor, float scale) {
    triangle *td = safe_malloc(sizeof(triangle));
    td->height = height;
    td->texture = texture;
    td->va = va;
    td->vb = vb;
    td->vc = vc;
    td->u1 = va.x * scale;
    td->v1 = va.y * scale;
    td->u2 = vb.x * scale;
    td->v2 = vb.y * scale;
    td->u3 = vc.x * scale;
    td->v3 = vc.y * scale;
    td->normal = floor ? 1.0 : -1.0;
    return td;
}
