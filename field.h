#ifndef FIELD_H
#define FIELD_H

#define MAX_WIDTH 100
#define MAX_HEIGHT 100

typedef struct {
    char object[MAX_HEIGHT][MAX_WIDTH]; // #, %, ^, &, @, _
    char color[MAX_HEIGHT][MAX_WIDTH];  // a-z или '_'
    int width, height;
    int dino_x, dino_y;
    int has_dino;
} Field;

void init_field(Field* f);
void set_size(Field* f, int w, int h);
void place_dino(Field* f, int x, int y);
int move_dino(Field* f, const char* direction);
void paint_cell(Field* f, char letter);
int dig(Field* f, const char* direction);
int mound(Field* f, const char* direction);
int jump_dino(Field* f, const char* direction, int n);
int grow_tree(Field* f, const char* direction);
int cut_tree(Field* f, const char* direction);
int make_rock(Field* f, const char* direction);
int push_rock(Field* f, const char* direction);
void print_field(Field* f);

#endif