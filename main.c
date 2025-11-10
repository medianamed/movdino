#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "field/field.h"
#include <stdbool.h>

int main(int argc, char* argv[]) {
    FILE* file = fopen("input.txt", "r");
    if (!file) {
        fprintf(stderr, "Error: cannot open input.txt\n");
        return 1;
    }

    Field f;
    init_field(&f);

    bool size_done = false;
    bool start_done = false;
    int commands_executed = 0;

    char line[256];
    int line_num = 0;

    while (fgets(line, sizeof(line), file)) {
        line_num++;
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') line[--len] = '\0';
        if (len > 0 && line[len-1] == '\r') line[--len] = '\0';

        // Проверка: левые пробелы запрещены!
        if (len > 0 && (line[0] == ' ' || line[0] == '\t')) {
            fprintf(stderr, "Error on line %d: leading whitespace not allowed\n", line_num);
            fclose(file);
            return 1;
        }

        // Пропуск пустых строк и комментариев
        if (len == 0 || (line[0] == '/' && line[1] == '/')) {
            continue;
        }

        // Разбор команды
        char* tokens[10];
        int token_count = 0;
        char* token = strtok(line, " \t\r\n");
        while (token != NULL && token_count < 10) {
            tokens[token_count++] = token;
            token = strtok(NULL, " \t\r\n");
        }
        if (token_count == 0) continue;
        char* cmd = tokens[0];

        // Проверка: первая команда должна быть SIZE
        if (commands_executed == 0 && strcmp(cmd, "SIZE") != 0) {
            fprintf(stderr, "Error on line %d: first command must be SIZE\n", line_num);
            fclose(file);
            return 1;
        }

        if (strcmp(cmd, "SIZE") == 0) {
            if (size_done) {
                fprintf(stderr, "Error on line %d: SIZE already set\n", line_num);
                fclose(file);
                return 1;
            }
            if (token_count != 3) {
                fprintf(stderr, "Error on line %d: SIZE requires 2 arguments\n", line_num);
                fclose(file);
                return 1;
            }
            int w = atoi(tokens[1]);
            int h = atoi(tokens[2]);
            set_size(&f, w, h);
            size_done = true;
            commands_executed++;
        }
        else if (strcmp(cmd, "START") == 0) {
            if (!size_done) {
                fprintf(stderr, "Error on line %d: SIZE must come before START\n", line_num);
                fclose(file);
                return 1;
            }
            if (start_done) {
                fprintf(stderr, "Error on line %d: START already set\n", line_num);
                fclose(file);
                return 1;
            }
            if (token_count != 3) {
                fprintf(stderr, "Error on line %d: START requires 2 arguments\n", line_num);
                fclose(file);
                return 1;
            }
            int x = atoi(tokens[1]);
            int y = atoi(tokens[2]);
            place_dino(&f, x, y);
            start_done = true;
            commands_executed++;
        }
        else {
            // Все остальные команды требуют START
            if (!start_done) {
                fprintf(stderr, "Error on line %d: START must be called before other commands\n", line_num);
                fclose(file);
                return 1;
            }

            if (strcmp(cmd, "MOVE") == 0) {
                if (token_count != 2) {
                    fprintf(stderr, "Error on line %d: MOVE requires 1 direction\n", line_num);
                    fclose(file); return 1;
                }
                if (!move_dino(&f, tokens[1])) {
                    fprintf(stderr, "Error on line %d: invalid direction '%s'\n", line_num, tokens[1]);
                    fclose(file); return 1;
                }
                commands_executed++;
            }
            else if (strcmp(cmd, "PAINT") == 0) {
                if (token_count != 2) {
                    fprintf(stderr, "Error on line %d: PAINT requires 1 letter\n", line_num);
                    fclose(file); return 1;
                }
                if (strlen(tokens[1]) != 1 || tokens[1][0] < 'a' || tokens[1][0] > 'z') {
                    fprintf(stderr, "Error on line %d: PAINT argument must be a lowercase letter\n", line_num);
                    fclose(file); return 1;
                }
                paint_cell(&f, tokens[1][0]);
                commands_executed++;
            }
            else if (strcmp(cmd, "DIG") == 0) {
                if (token_count != 2) {
                    fprintf(stderr, "Error on line %d: DIG requires 1 direction\n", line_num);
                    fclose(file); return 1;
                }
                if (!dig(&f, tokens[1])) {
                    fprintf(stderr, "Error on line %d: cannot DIG in that direction\n", line_num);
                    fclose(file); return 1;
                }
                commands_executed++;
            }
            else if (strcmp(cmd, "MOUND") == 0) {
                if (token_count != 2) {
                    fprintf(stderr, "Error on line %d: MOUND requires 1 direction\n", line_num);
                    fclose(file); return 1;
                }
                if (!mound(&f, tokens[1])) {
                    fprintf(stderr, "Error on line %d: cannot MOUND in that direction\n", line_num);
                    fclose(file); return 1;
                }
                commands_executed++;
            }
            else if (strcmp(cmd, "JUMP") == 0) {
                if (token_count != 3) {
                    fprintf(stderr, "Error on line %d: JUMP requires direction and number\n", line_num);
                    fclose(file); return 1;
                }
                int n = atoi(tokens[2]);
                if (n <= 0) {
                    fprintf(stderr, "Error on line %d: jump distance must be > 0\n", line_num);
                    fclose(file); return 1;
                }
                if (!jump_dino(&f, tokens[1], n)) {
                    fprintf(stderr, "Error on line %d: invalid JUMP command\n", line_num);
                    fclose(file); return 1;
                }
                commands_executed++;
            }
            else if (strcmp(cmd, "GROW") == 0) {
                if (token_count != 2) {
                    fprintf(stderr, "Error on line %d: GROW requires 1 direction\n", line_num);
                    fclose(file); return 1;
                }
                if (!grow_tree(&f, tokens[1])) {
                    fprintf(stderr, "Error on line %d: cannot GROW tree in that direction\n", line_num);
                    fclose(file); return 1;
                }
                commands_executed++;
            }
            else if (strcmp(cmd, "CUT") == 0) {
                if (token_count != 2) {
                    fprintf(stderr, "Error on line %d: CUT requires 1 direction\n", line_num);
                    fclose(file); return 1;
                }
                if (!cut_tree(&f, tokens[1])) {
                    fprintf(stderr, "Error on line %d: no tree to CUT in that direction\n", line_num);
                    fclose(file); return 1;
                }
                commands_executed++;
            }
            else if (strcmp(cmd, "MAKE") == 0) {
                if (token_count != 2) {
                    fprintf(stderr, "Error on line %d: MAKE requires 1 direction\n", line_num);
                    fclose(file); return 1;
                }
                if (!make_rock(&f, tokens[1])) {
                    fprintf(stderr, "Error on line %d: cannot MAKE rock in that direction\n", line_num);
                    fclose(file); return 1;
                }
                commands_executed++;
            }
            else if (strcmp(cmd, "PUSH") == 0) {
                if (token_count != 2) {
                    fprintf(stderr, "Error on line %d: PUSH requires 1 direction\n", line_num);
                    fclose(file); return 1;
                }
                if (!push_rock(&f, tokens[1])) {
                    fprintf(stderr, "Error on line %d: no rock to PUSH in that direction\n", line_num);
                    fclose(file); return 1;
                }
                commands_executed++;
            }
            else {
                fprintf(stderr, "Error on line %d: unknown command '%s'\n", line_num, cmd);
                fclose(file);
                return 1;
            }
        }
    }

    fclose(file);
    print_field(&f);
    return 0;
}
