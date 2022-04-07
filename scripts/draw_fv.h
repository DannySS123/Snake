#ifndef DRAW_FV_H
#define DRAW_FV_H

#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

typedef enum menu_enum {play_title = 0, scoreboard_title = 1, color_title = 2, quit_title = 3} menu_enum;
typedef enum endmenu_enum {retry_title = 0, menu_title = 1, end_quit_title = 2} endmenu_enum;
typedef enum scoreboard_enum {back_to_menu_title = 0, scoreboard_quit_title = 1} scoreboard_enum;

typedef struct highlight_s{  //kijelölt menüpontok
    menu_enum menu_highlights;
    endmenu_enum endmenu_highlights;
    scoreboard_enum scoreboard_highlights;
}highlight_s;

void clear_map(game_struct w);
void draw_map(SDL_Renderer *renderer, game_struct w);
void draw_end_state(SDL_Renderer *renderer, highlight_s hls, game_struct w);
void draw_menu_state(SDL_Renderer *renderer, highlight_s hls, game_struct w);
void draw_pause_menu(SDL_Renderer *renderer, game_struct w);
int draw_scoreboard_state(SDL_Renderer *renderer, highlight_s hls, game_struct w);
int draw_play_state(SDL_Renderer *renderer, s_list * head, game_struct *w);
void draw_enter_name_state(SDL_Renderer *renderer, game_struct w);
#endif
