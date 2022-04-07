#ifndef SNAKE_FV_H
#define SNAKE_FV_H
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <stdbool.h>

typedef enum palya_enum {empty_tile = 0, apple_tile = 1, snake_tile = 2} palya_enum;
typedef enum direction {left, right, up, down} direction;
typedef enum gamestate_enum {menu_state = 0, play_state = 1, end_state = 2, scoreboard_state = 4, enter_name_state = 3, color_state = 5} gamestate_enum;

typedef struct pont {
    int x, y;
}pont;

typedef struct node {
    pont snk;
    struct node * next;
}s_list;

typedef struct t_snake_color{
    unsigned char r, g, b;
}t_snake_color;

typedef struct game_struct {
    int width, heigth, rect_size, snake_length, base_s_length;
    palya_enum **palya;
    pont apple;
    direction dir;
    bool changed_dir, paused, quit;
    gamestate_enum gamestate;
    SDL_Texture *pic[10];
    Mix_Music *musics[10];
    Mix_Chunk *sounds[10];
    t_snake_color snake_color;
    t_snake_color eye;
}game_struct;

s_list *snake_add(s_list * head, pont snk);
void print_list(s_list * head);
void remove_last(s_list * head);
int snake_reset(s_list * head, game_struct *w);
int check_snake_death(s_list * head, game_struct w);
void snake_step(s_list * head, direction dir);
int snake_grow(s_list * head);
int snake_init(s_list **head, game_struct *w);
void free_snake(s_list * head);

#endif
