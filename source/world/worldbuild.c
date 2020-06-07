#include "worldbuild.h"

static void build_cell_lines(world *self, line *ld) {

    double dx = fabs(ld->vb.x - ld->va.x);
    double dy = fabs(ld->vb.y - ld->va.y);

    int x = (int)floor(ld->va.x);
    int y = (int)floor(ld->va.y);

    int n = 1;
    double error;
    int x_inc;
    int y_inc;

    if (dx == 0) {
        x_inc = 0;
        error = DBL_MAX;
    } else if (ld->vb.x > ld->va.x) {
        x_inc = 1;
        n += (int)floor(ld->vb.x) - x;
        error = (floor(ld->va.x) + 1 - ld->va.x) * dy;
    } else {
        x_inc = -1;
        n += x - (int)(floor(ld->vb.x));
        error = (ld->va.x - floor(ld->va.x)) * dy;
    }

    if (dy == 0) {
        y_inc = 0;
        error = DBL_MIN;
    } else if (ld->vb.y > ld->va.y) {
        y_inc = 1;
        n += (int)floor(ld->vb.y) - y;
        error -= (floor(ld->va.y) + 1 - ld->va.y) * dx;
    } else {
        y_inc = -1;
        n += y - (int)floor(ld->vb.y);
        error -= (ld->va.y - floor(ld->va.y)) * dx;
    }

    while (n > 0) {
        cell *c = &self->cells[(x >> WORLD_CELL_SHIFT) + (y >> WORLD_CELL_SHIFT) * self->cell_columns];
        cell_add_line(c, ld);

        if (error > 0) {
            y += y_inc;
            error -= dx;
        } else {
            x += x_inc;
            error += dy;
        }

        n--;
    }
}

static void build_lines(world *self, sector *sec) {
    int line_count = sec->line_count;

    if (line_count == 0) {
        return;
    }

    float bottom = sec->bottom;
    float floor = sec->floor;
    float ceil = sec->ceiling;
    float top = sec->top;

    sector *plus;
    sector *minus;

    if (sec->outside == NULL) {
        plus = NULL;
        minus = sec;
    } else {
        plus = sec;
        minus = sec->outside;
    }

    line **lines = sec->lines;

    float u = 0.0;

    for (int i = 0; i < line_count; i++) {

        line *ld = lines[i];

        build_cell_lines(self, ld);

        line_set_sectors(ld, plus, minus);

        float x = ld->va.x - ld->vb.x;
        float y = ld->va.y - ld->vb.y;
        float s = u + sqrtf(x * x + y * y) * WORLD_SCALE;

        if (ld->bottom != NULL) {
            wall_set(ld->bottom, bottom, floor, u, bottom * WORLD_SCALE, s, floor * WORLD_SCALE);
        }

        if (ld->middle != NULL) {
            wall_set(ld->middle, floor, ceil, u, floor * WORLD_SCALE, s, ceil * WORLD_SCALE);
        }

        if (ld->top != NULL) {
            wall_set(ld->top, ceil, top, u, ceil * WORLD_SCALE, s, top * WORLD_SCALE);
        }

        u = s;
    }
}

void world_build_map(world *self) {

    const int cell_size = 1 << WORLD_CELL_SHIFT;

    float top = 0;
    float left = 0;

    sector **sectors = self->sectors;
    int sector_count = self->sector_count;

    for (int i = 0; i < sector_count; i++) {
        sector *s = sectors[i];

        int vector_count = s->vec_count;
        vec **vecs = s->vecs;

        for (int k = 0; k < vector_count; k++) {
            vec *v = vecs[k];

            if (v->x > left) {
                left = v->x;
            }

            if (v->y > top) {
                top = v->y;
            }
        }
    }

    array *sector_inside_lists = safe_calloc(sector_count, sizeof(array));

    for (int i = 0; i < sector_count; i++) {
        sector *s = sectors[i];

        vec **s_vecs = s->vecs;
        int s_vec_count = s->vec_count;

        array *s_temp_inside_list = &sector_inside_lists[i];
        array_init(s_temp_inside_list, 0);

        for (int k = 0; k < sector_count; k++) {
            if (k == i) {
                continue;
            }

            sector *o = sectors[k];

            vec **o_vecs = o->vecs;
            int o_vec_count = o->vec_count;

            bool contained = true;

            for (int w = 0; w < o_vec_count; w++) {

                for (int c = 0; c < s_vec_count; c++) {
                    if (s_vecs[c] == o_vecs[w]) {
                        contained = false;
                        goto label_contained;
                    }
                }

                if (!sector_contains(s, o_vecs[w]->x, o_vecs[w]->y)) {
                    contained = false;
                    goto label_contained;
                }
            }

        label_contained:
            if (contained) {
                array_push(s_temp_inside_list, o);
            }
        }
    }

    for (int i = 0; i < sector_count; i++) {
        sector *s = sectors[i];
        array *s_temp_inside_list = &sector_inside_lists[i];
        unsigned int s_inside_count = s_temp_inside_list->length;

        array *dead = create_array(0);

        for (unsigned int k = 0; k < s_inside_count; k++) {
            sector *o = s_temp_inside_list->items[k];
            int index_of_o;
            for (index_of_o = 0; index_of_o < sector_count; index_of_o++) {
                if (sectors[index_of_o] == o) {
                    break;
                }
            }
            array *o_inside_list = &sector_inside_lists[index_of_o];
            unsigned int o_inside_count = o_inside_list->length;
            for (unsigned int w = 0; w < o_inside_count; w++) {
                array_push(dead, o_inside_list->items[w]);
            }
        }

        for (unsigned int k = 0; k < array_size(dead); k++) {
            array_remove(s_temp_inside_list, dead->items[k]);
        }

        for (unsigned int k = 0; k < s_inside_count; k++) {
            ((sector *)s_temp_inside_list->items[k])->outside = s;
        }

        delete_array(dead);

        s->inside = (sector **)array_copy_items(s_temp_inside_list);
        s->inside_count = s_temp_inside_list->length;
    }

    for (int i = 0; i < sector_count; i++) {
        release_array(&sector_inside_lists[i]);
    }

    free(sector_inside_lists);

    self->cell_rows = (int)ceil(top / cell_size);
    self->cell_columns = (int)ceil(left / cell_size);
    self->cell_count = self->cell_rows * self->cell_columns;
    self->cells = safe_calloc(self->cell_count, sizeof(cell));

    for (int i = 0; i < sector_count; i++) {
        triangulate_sector(sectors[i], WORLD_SCALE);
    }

    for (int i = 0; i < sector_count; i++) {
        build_lines(self, sectors[i]);
    }
}
