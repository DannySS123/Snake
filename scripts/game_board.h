#ifndef GAME_BOARD_H
#define GAME_BOARD_H

#include "snake_fv.h"
#include "draw_fv.h"
#include "sdl_utility.h"

typedef struct highscore_struct {
    int score_t[10];
    int ranks_t[10];
    char names[10][50];
}highscore_struct;

gamestate_enum enter_name_state_fv(SDL_Renderer *renderer, game_struct sw, TTF_Font *font);
void spawn_apple(s_list * head, game_struct *w);
int foglal(game_struct *w);
void free_all(game_struct *w);
int highscore_placment(char *szoveg, int new_score);
int check_highscore();
void close_and_free(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font, game_struct *sw, s_list *head);

#endif
