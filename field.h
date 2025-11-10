#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "field/field.h"

int wrap(int val, int max) {
    val %= max;
    if (val < 0) val += max;
    return val;
}

void init_field(Field* f) {
    f->width = f->height = 0;
    f->has_dino = 0;
}

void set_size(Field* f, int w, int h) {
    if (w < 10 || w > 100 || h < 10 || h > 100) return;
    f->width = w;
    f->height = h;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            f->object[y][x] = '_';
            f->color[y][x] = '_';
        }
    }
}

void place_dino(Field* f, int x, int y) {
    if (!f->width || x < 0 || x >= f->width || y < 0 || y >= f->height) return;
    f->dino_x = x;
    f->dino_y = y;
    f->has_dino = 1;
    f->object[y][x] = '#';
}

void get_neighbor(int x, int y, const char* dir, int width, int height, int* out_x, int* out_y) {
    *out_x = x;
    *out_y = y;
    if (strcmp(dir, "UP") == 0) (*out_y)--;
    else if (strcmp(dir, "DOWN") == 0) (*out_y)++;
    else if (strcmp(dir, "LEFT") == 0) (*out_x)--;
    else if (strcmp(dir, "RIGHT") == 0) (*out_x)++;
    *out_x = wrap(*out_x, width);
    *out_y = wrap(*out_y, height);
}

int move_dino(Field* f, const char* direction) {
    if (!f->has_dino) return 0;
    int nx = f->dino_x, ny = f->dino_y;
    if (strcmp(direction, "UP") == 0) ny--;
    else if (strcmp(direction, "DOWN") == 0) ny++;
    else if (strcmp(direction, "LEFT") == 0) nx--;
    else if (strcmp(direction, "RIGHT") == 0) nx++;
    else return 0;
    nx = wrap(nx, f->width);
    ny = wrap(ny, f->height);
    char target = f->object[ny][nx];
    if (target == '%') {
        fprintf(stderr, "Error: Dino fell into a pit!\n");
        exit(1);
    }
    if (target == '^' || target == '&' || target == '@') {
        printf("Warning: Blocked by obstacle, move ignored.\n");
        return 1;
    }
    f->object[f->dino_y][f->dino_x] = '_';
    f->dino_x = nx;
    f->dino_y = ny;
    f->object[f->dino_y][f->dino_x] = '#';
    return 1;
}

void paint_cell(Field* f, char letter) {
    if (!f->has_dino) return;
    if (letter >= 'a' && letter <= 'z') {
        f->color[f->dino_y][f->dino_x] = letter;
    }
}

int dig(Field* f, const char* direction) {
    if (!f->has_dino) return 0;
    int tx, ty;
    get_neighbor(f->dino_x, f->dino_y, direction, f->width, f->height, &tx, &ty);
    if (f->object[ty][tx] == '_') {
        f->object[ty][tx] = '%';
        return 1;
    }
    return 0;
}

int mound(Field* f, const char* direction) {
    if (!f->has_dino) return 0;
    int tx, ty;
    get_neighbor(f->dino_x, f->dino_y, direction, f->width, f->height, &tx, &ty);
    if (f->object[ty][tx] == '%') {
        f->object[ty][tx] = '_';
    } else if (f->object[ty][tx] == '_') {
        f->object[ty][tx] = '^';
    }
    return 1;
}

int jump_dino(Field* f, const char* direction, int n) {
    if (!f->has_dino || n <= 0) return 0;
    int dx = 0, dy = 0;
    if (strcmp(direction, "UP") == 0) dy = -1;
    else if (strcmp(direction, "DOWN") == 0) dy = 1;
    else if (strcmp(direction, "LEFT") == 0) dx = -1;
    else if (strcmp(direction, "RIGHT") == 0) dx = 1;
    else return 0;
    int nx = f->dino_x, ny = f->dino_y;
    int final_x = nx, final_y = ny;
    for (int i = 1; i <= n; i++) {
        nx = wrap(nx + dx, f->width);
        ny = wrap(ny + dy, f->height);
        char cell = f->object[ny][nx];
        if (cell == '^' || cell == '&' || cell == '@') {
            printf("Warning: Blocked by obstacle during jump, landed at (%d,%d)\n", final_x, final_y);
            break;
        }
        final_x = nx;
        final_y = ny;
    }
    char landing = f->object[final_y][final_x];
    if (landing == '%') {
        fprintf(stderr, "Error: Dino fell into a pit on landing!\n");
        exit(1);
    }
    f->object[f->dino_y][f->dino_x] = '_';
    f->dino_x = final_x;
    f->dino_y = final_y;
    f->object[f->dino_y][f->dino_x] = '#';
    return 1;
}

int grow_tree(Field* f, const char* direction) {
    if (!f->has_dino) return 0;
    int tx, ty;
    get_neighbor(f->dino_x, f->dino_y, direction, f->width, f->height, &tx, &ty);
    if (f->object[ty][tx] == '_') {
        f->object[ty][tx] = '&';
        return 1;
    }
    return 0;
}

int cut_tree(Field* f, const char* direction) {
    if (!f->has_dino) return 0;
    int tx, ty;
    get_neighbor(f->dino_x, f->dino_y, direction, f->width, f->height, &tx, &ty);
    if (f->object[ty][tx] == '&') {
        f->object[ty][tx] = '_';
        return 1;
    }
    return 0;
}

int make_rock(Field* f, const char* direction) {
    if (!f->has_dino) return 0;
    int tx, ty;
    get_neighbor(f->dino_x, f->dino_y, direction, f->width, f->height, &tx, &ty);
    if (f->object[ty][tx] == '_') {
        f->object[ty][tx] = '@';
        return 1;
    }
    return 0;
}

int push_rock(Field* f, const char* direction) {
    if (!f->has_dino) return 0;
    int tx, ty;
    get_neighbor(f->dino_x, f->dino_y, direction, f->width, f->height, &tx, &ty);
    if (f->object[ty][tx] != '@') return 0;
    int push_x = tx, push_y = ty;
    get_neighbor(tx, ty, direction, f->width, f->height, &push_x, &push_y);
    if (f->object[push_y][push_x] == '_') {
        char saved_color = f->color[push_y][push_x];
        f->object[ty][tx] = '_';
        f->object[push_y][push_x] = '@';
        f->color[push_y][push_x] = saved_color;
        return 1;
    }
    if (f->object[push_y][push_x] == '%') {
        f->object[push_y][push_x] = '_';
        f->object[ty][tx] = '_';
        return 1;
    }
    return 0;
}

void print_field(Field* f) {
    for (int y = 0; y < f->height; y++) {
        for (int x = 0; x < f->width; x++) {
            if (f->object[y][x] != '_' || f->color[y][x] == '_') {
                putchar(f->object[y][x]);
            } else {
                putchar(f->color[y][x]);
            }
        }
        putchar('\n');
    }
}
