#include "scripts/snake_fv.h"
#include "scripts/draw_fv.h"
#include "scripts/sdl_utility.h"
#include "scripts/game_board.h"
#include "scripts/debugmalloc.h"
#define FONT "resources/LiberationSerif-Regular.ttf"

//SDL_Renderer *renderer;

int main(int argc, char *argv[]) {
    srand(time(0));

    game_struct sw = {50, 50, 12, 0, 5};  //w, h, r, s.l, s.b.l
    sw.dir = right;
    sw.quit = (foglal(&sw) == -1);
    sw.snake_color.r = 255; sw.snake_color.g = 255; sw.snake_color.b = 0;
    SDL_Window *window;   // ablak létrehozása
    SDL_Renderer *renderer;
    TTF_Font *font;
    sdl_init("SNAKE", sw.width*sw.rect_size, sw.heigth*sw.rect_size, FONT,  &window, &renderer, &font);
    SDL_TimerID id = SDL_AddTimer(50, idozit, NULL); // idozito hozzaadasa: 20 ms; 1000 ms / 20 ms -> 50 fps

    ///--------------------------------------------------------------------
    open_images(renderer, &sw);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    sw.musics[0] = Mix_LoadMUS("resources/Netherplace.mp3");
    //sw.sounds[0] = Mix_LoadMUS("resources/Apple_eat.mp3");
    if(sw.musics[0] == NULL /*|| sw.sounds[0] == NULL*/)
    {
        printf("Zene nem nyithato meg");
        exit(0);
    }
    Mix_PlayMusic(sw.musics[0], -1);
    ///--------------------------------------------------------------------
    highlight_s hls = {play_title, retry_title, back_to_menu_title};
    s_list * head = NULL;
    if (snake_init(&head, &sw) == -1)
    {
        sw.quit = true;
    }

    while (!(sw.quit)) {
        SDL_Event event;
        SDL_WaitEvent(&event);
        switch (event.type) {
            case SDL_USEREVENT:
                if (sdl_userevent_fv(renderer, &hls, &sw, font, head) == -1)
                {
                    sw.quit = true;
                }
                break;

            case SDL_KEYDOWN:
                sdl_keydown_fv(event, &sw, &hls, head);
                break;

            case SDL_QUIT:
                sw.quit = true;
                break;
        }
    }
    close_and_free(window, renderer, font, &sw, head);
    SDL_RemoveTimer(id);
    return 0;
}
