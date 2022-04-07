#ifndef SDL_UTILITY_H
#define SDL_UTILITY_H

#include "snake_fv.h"
#include "draw_fv.h"
#include "game_board.h"

void sdl_init(char const *felirat, int szeles, int magas, const char* tipus, SDL_Window **pwindow, SDL_Renderer **prenderer,  TTF_Font **pfont);
void sdl_close(SDL_Window **pwindow, SDL_Renderer **prenderer, TTF_Font **pfont);
Uint32 idozit(Uint32 ms, void *param);

bool sdl_return_fv(highlight_s *hls, s_list *head, game_struct *sw);
void sdl_up_fv(highlight_s *hls, game_struct *sw);
void sdl_down_fv(highlight_s *hls, game_struct *sw);
void sdl_keydown_fv(SDL_Event event, game_struct *sw, highlight_s *hls, s_list *head);
int sdl_userevent_fv(SDL_Renderer *renderer, highlight_s *hls, game_struct *sw, TTF_Font *font, s_list *head);
bool input_text(char *dest, size_t hossz, SDL_Rect teglalap, SDL_Color hatter, SDL_Color szoveg, TTF_Font *font, SDL_Renderer *renderer);
int open_images(SDL_Renderer *renderer, game_struct *sw);
int destroy_images(game_struct *sw);

#endif
