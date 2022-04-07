#include "sdl_utility.h"
#include "widgets.h"

/* kulon fuggvenybe, hogy olvashatobb legyen */
void sdl_init(char const *felirat, int szeles, int magas, const char* tipus, SDL_Window **pwindow, SDL_Renderer **prenderer,  TTF_Font **pfont) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        SDL_Log("Nem indithato az SDL: %s", SDL_GetError());
        exit(1);
    }
    SDL_Window *window = SDL_CreateWindow(felirat, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, szeles, magas, 0);
    if (window == NULL) {
        SDL_Log("Nem hozhato letre az ablak: %s", SDL_GetError());
        exit(1);
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) {
        SDL_Log("Nem hozhato letre a megjelenito: %s", SDL_GetError());
        exit(1);
    }
    SDL_RenderClear(renderer);

    TTF_Init();
    TTF_Font *font = TTF_OpenFont(tipus, 32);
    if (font == NULL) {
        SDL_Log("Nem sikerult megnyitni a fontot! %s\n", TTF_GetError());
        exit(1);
    }

    *pwindow = window;
    *prenderer = renderer;
    *pfont = font;
}

void sdl_close(SDL_Window **pwindow, SDL_Renderer **prenderer, TTF_Font **pfont) {
    SDL_DestroyRenderer(*prenderer);
    *prenderer = NULL;

    SDL_DestroyWindow(*pwindow);
    *pwindow = NULL;

    TTF_CloseFont(*pfont);
    *pfont = NULL;

    SDL_Quit();
}

bool input_text(char *dest, size_t hossz, SDL_Rect teglalap, SDL_Color hatter, SDL_Color szoveg, TTF_Font *font, SDL_Renderer *renderer) {
    /* Ez tartalmazza az aktualis szerkesztest */
    char composition[SDL_TEXTEDITINGEVENT_TEXT_SIZE];
    composition[0] = '\0';
    /* Ezt a kirajzolas kozben hasznaljuk */
    char textandcomposition[hossz + SDL_TEXTEDITINGEVENT_TEXT_SIZE + 1];
    /* Max hasznalhato szelesseg */
    int maxw = teglalap.w - 2;
    int maxh = teglalap.h - 2;

    dest[0] = '\0';

    bool enter = false;
    bool kilep = false;

    SDL_StartTextInput();
    while (!kilep && !enter) {
        /* doboz kirajzolasa */
        boxRGBA(renderer, teglalap.x, teglalap.y, teglalap.x + teglalap.w - 1, teglalap.y + teglalap.h - 1, hatter.r, hatter.g, hatter.b, 255);
        rectangleRGBA(renderer, teglalap.x, teglalap.y, teglalap.x + teglalap.w - 1, teglalap.y + teglalap.h - 1, szoveg.r, szoveg.g, szoveg.b, 255);
        /* szoveg kirajzolasa */
        int w;
        strcpy(textandcomposition, dest);
        strcat(textandcomposition, composition);
        if (textandcomposition[0] != '\0') {
            SDL_Surface *felirat = TTF_RenderUTF8_Blended(font, textandcomposition, szoveg);
            SDL_Texture *felirat_t = SDL_CreateTextureFromSurface(renderer, felirat);
            SDL_Rect cel = { teglalap.x, teglalap.y, felirat->w < maxw ? felirat->w : maxw, felirat->h < maxh ? felirat->h : maxh };
            SDL_RenderCopy(renderer, felirat_t, NULL, &cel);
            SDL_FreeSurface(felirat);
            SDL_DestroyTexture(felirat_t);
            w = cel.w;
        } else {
            w = 0;
        }
        /* kurzor kirajzolasa */
        if (w < maxw) {
            vlineRGBA(renderer, teglalap.x + w + 2, teglalap.y + 2, teglalap.y + teglalap.h - 3, szoveg.r, szoveg.g, szoveg.b, 192);
        }
        /* megjeleniti a képernyon az eddig rajzoltakat */
        SDL_RenderPresent(renderer);

        SDL_Event event;
        SDL_WaitEvent(&event);
        switch (event.type) {
            /* Kulonleges karakter */
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_BACKSPACE) {
                    int textlen = strlen(dest);
                    do {
                        if (textlen == 0) {
                            break;
                        }
                        if ((dest[textlen-1] & 0x80) == 0x00)   {
                            /* Egy bajt */
                            dest[textlen-1] = 0x00;
                            break;
                        }
                        if ((dest[textlen-1] & 0xC0) == 0x80) {
                            /* Bajt, egy tobb-bajtos szekvenciabol */
                            dest[textlen-1] = 0x00;
                            textlen--;
                        }
                        if ((dest[textlen-1] & 0xC0) == 0xC0) {
                            /* Egy tobb-bajtos szekvencia elso bajtja */
                            dest[textlen-1] = 0x00;
                            break;
                        }
                    } while(true);
                }
                if (event.key.keysym.sym == SDLK_RETURN) {
                    enter = true;
                }
                break;

            /* A feldolgozott szoveg bemenete */
            case SDL_TEXTINPUT:
                if (strlen(dest) + strlen(event.text.text) < hossz) {
                    strcat(dest, event.text.text);
                }

                /* Az eddigi szerkesztes torolheto */
                composition[0] = '\0';
                break;

            /* Szoveg szerkesztese */
            case SDL_TEXTEDITING:
                strcpy(composition, event.edit.text);
                break;

            case SDL_QUIT:
                /* visszatesszuk a sorba ezt az eventet, mert
                 * sok mindent nem tudunk vele kezdeni */
                SDL_PushEvent(&event);
                kilep = true;
                break;
        }
    }

    /* igaz jelzi a helyes beolvasast; = ha enter miatt allt meg a ciklus */
    SDL_StopTextInput();
    return enter;
}


Uint32 idozit(Uint32 ms, void *param) {
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    SDL_PushEvent(&ev);
    return ms;   /* ujabb varakozas */
}

void sdl_down_fv(highlight_s *hls, game_struct *sw)
{
    if (sw->dir != up && !(sw->changed_dir) && sw->gamestate == play_state)
    {
        sw->changed_dir = true;
        sw->dir = down;
    }

    if (hls->menu_highlights < 3 && sw->gamestate == menu_state)
    {
        hls->menu_highlights++;
    }

    if (hls->endmenu_highlights < 2 && sw->gamestate == end_state)
    {
        hls->endmenu_highlights++;
    }

    if (hls->scoreboard_highlights < 1 && sw->gamestate == scoreboard_state)
    {
        hls->scoreboard_highlights++;
    }
}

void sdl_up_fv(highlight_s *hls, game_struct *sw)
{
    if (sw->dir != down && !(sw->changed_dir) && sw->gamestate == play_state)
    {
        sw->changed_dir = true;
        sw->dir = up;
    }

    if (hls->menu_highlights > 0 && sw->gamestate == menu_state)
    {
        hls->menu_highlights--;
    }

    if (hls->endmenu_highlights > 0 && sw->gamestate == end_state)
    {
        hls->endmenu_highlights--;
    }
    if (hls->scoreboard_highlights > 0 && sw->gamestate == scoreboard_state)
    {
        hls->scoreboard_highlights--;
    }
}

bool sdl_return_fv(highlight_s *hls, s_list *head, game_struct *sw)
{
    if (sw->gamestate == menu_state)
    {
       switch (hls->menu_highlights)
        {
            case play_title:
                if (snake_reset(head, sw) == -1)
                {
                    return true;
                }
                spawn_apple(head, sw);
                sw->gamestate = play_state;
                hls->endmenu_highlights = retry_title;
                break;

            case scoreboard_title:
                sw->gamestate = scoreboard_state;
                break;

            case color_title:
                sw->gamestate = color_state;
                break;

            case quit_title:
                return true;
                break;
        }
    }
    else if(sw->gamestate == end_state)
    {
       switch (hls->endmenu_highlights)
        {
            case retry_title:
                if (snake_reset(head, sw) == -1)
                {
                    return true;
                }
                spawn_apple(head, sw);
                sw->gamestate = play_state;
                break;

            case menu_title:
                sw->gamestate = menu_state;
                break;

            case end_quit_title:
                return true;
                break;
        }
    }
    else if(sw->gamestate == scoreboard_state)
    {
       switch (hls->scoreboard_highlights)
        {
            case back_to_menu_title:
                hls->menu_highlights = play_title;
                sw->gamestate = menu_state;
                break;

            case scoreboard_quit_title:
                return true;
                break;
        }
    }
    return false;
}

void sdl_keydown_fv(SDL_Event event, game_struct *sw, highlight_s *hls, s_list *head)
{
    switch (event.key.keysym.sym) {
        case SDLK_LEFT:
            if (sw->dir != right && !(sw->changed_dir) && sw->gamestate == play_state)
            {
                sw->changed_dir = true;
                sw->dir = left;
            }
            break;

        case SDLK_RIGHT:
            if (sw->dir != left && !(sw->changed_dir) && sw->gamestate == play_state)
            {
                sw->changed_dir = true;
                sw->dir = right;
            }
            break;

        case SDLK_UP:
            sdl_up_fv(hls, sw);
            break;

        case SDLK_DOWN:
            sdl_down_fv(hls, sw);
            break;

        case SDLK_RETURN:
            sw->quit = sdl_return_fv(hls, head, sw);
            break;

        case SDLK_m:
            if (Mix_PausedMusic() == 1)
            {
                Mix_ResumeMusic();
            }
            else
            {
                 Mix_PauseMusic();
            }
            break;

        case SDLK_ESCAPE: sw->quit = true; break;
        case SDLK_p: sw->paused = !(sw->paused); break;
    }
}

int sdl_userevent_fv(SDL_Renderer *renderer, highlight_s *hls, game_struct *sw, TTF_Font *font, s_list *head)
{
    Widget *widgetek[9 + 1];
    switch (sw->gamestate)
    {
        case menu_state:
            draw_menu_state(renderer, *hls, *sw);
            break;

        case end_state:
            draw_end_state(renderer, *hls, *sw);
            break;

        case color_state:
            //---------------------------------------
            SDL_RenderClear(renderer);
            int w = sw->width * sw->rect_size;
            int h = sw->heigth * sw->rect_size;
            /* widgetek létrehozása */
            widgetek[0] = uj_gomb(w/2 - 25, h/2 + 140, 50, 32, "MENU");
            widgetek[1] = uj_felirat(w/4, h/2 - 62, "v\x94r\x94s");
            widgetek[2] = uj_csuszka(w/4, h/2 - 50, w/2, 20, 0);
            widgetek[3] = uj_felirat(w/4, h/2 - 12, "z\x94ld");
            widgetek[4] = uj_csuszka(w/4, h/2, w/2, 20, 0.0);
            widgetek[5] = uj_felirat(w/4, h/2 + 38, "k\x82k");
            widgetek[6] = uj_csuszka(w/4, h/2 + 50, w/2 , 20, 0.0);
            widgetek[7] = uj_gomb(w/2 - 25, h/2 + 85, 50, 32, "mehet");
            widgetek[8] = uj_szinesteglalap(w/4, h/2 - 120, w/2, 32, 128, 0, 0);
            widgetek[9] = NULL;

            /* a 0. gomb kattintasa - kilepes */
            widgetek[0]->felhasznaloi_cb = kilep_gomb_cb;

            /* a mehet gomb kattintasa + parametere */
            UIAdat klikk_adat;
            klikk_adat.r = widgetek[2];
            klikk_adat.g = widgetek[4];
            klikk_adat.b = widgetek[6];
            klikk_adat.teglalap = widgetek[8];
            widgetek[7]->felhasznaloi_cb = mehet_gomb_cb;
            widgetek[7]->felhasznaloi_cb_param = &klikk_adat;

            esemenyvezerelt_main(renderer, widgetek, sw);

            for (int i = 0; widgetek[i] != NULL; ++i)
                free(widgetek[i]);
            //---------------------------------------
            sw->gamestate = menu_state;
            break;

        case scoreboard_state:
            if (draw_scoreboard_state(renderer, *hls, *sw) == -1)
            {
                return -1;
            }
            break;

        case enter_name_state:
            sw->gamestate = enter_name_state_fv(renderer, *sw, font);
            if (sw->gamestate == -1)
            {
                return -1;
            }
            break;

        case play_state:
            if (!(sw->paused))
            {
                if (draw_play_state(renderer, head, sw) == -1)
                {
                    return -1;
                }
                sw->changed_dir = false;
            }
            else
            {
                draw_pause_menu(renderer, *sw);
            }
            break;
    }
    SDL_RenderPresent(renderer);
    return 0;
}

int open_images(SDL_Renderer *renderer, game_struct *sw)
{
    sw->pic[0] = IMG_LoadTexture(renderer, "resources/bg.png");
    sw->pic[1] = IMG_LoadTexture(renderer, "resources/apple.png");

    if (sw->pic[1] == NULL || sw->pic[0] == NULL) {
        SDL_Log("Nem nyithato meg a kepfajl: %s", IMG_GetError());
        exit(1);
    }
    return 0;
}

int destroy_images(game_struct *sw)
{
    SDL_DestroyTexture(sw->pic[0]);
    SDL_DestroyTexture(sw->pic[0]);
    return 0;
}
