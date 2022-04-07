#include "snake_fv.h"
#include "draw_fv.h"
#include "game_board.h"
#include "debugmalloc.h"

//ez a fv rajzolja ki az egész pályát, egyesével végighalad az elemein, és azon értékek szerint rajzol
//fekete, zöld vagy piros négyzetet
void draw_map(SDL_Renderer *renderer, game_struct w)
{
    SDL_Rect src = {0, 0, w.width * w.rect_size, w.heigth * w.rect_size};
    SDL_Rect dest = { 0, 0, w.width * w.rect_size, w.heigth * w.rect_size};
    SDL_RenderCopy(renderer, w.pic[0], &src, &dest);
    for (int i = 0; i < w.heigth+4; ++i) //pálya kirajzolása
    {
        for (int j = 0; j < w.width+4; ++j)
        {
            if(w.palya[i][j] == empty_tile)
            {
                //boxRGBA(renderer, (j-2)*w.rect_size, (i-2)*w.rect_size, (j-1)*w.rect_size, (i-1)*w.rect_size, 0, 0, 0, 255);
            }
            else if (w.palya[i][j] == apple_tile)
            {
                SDL_Rect dest = {(j-2)*w.rect_size-5, (i-2)*w.rect_size-5, 12+10, 12+10};
                SDL_RenderCopy(renderer, w.pic[1], NULL, &dest);
                //boxRGBA(renderer, (j-2)*w.rect_size, (i-2)*w.rect_size, (j-1)*w.rect_size, (i-1)*w.rect_size, 255, 0, 0, 255);
            }
            else if (w.palya[i][j] == snake_tile)
            {
                boxRGBA(renderer, (j-2)*w.rect_size, (i-2)*w.rect_size, (j-1)*w.rect_size, (i-1)*w.rect_size, w.snake_color.r, w.snake_color.g, w.snake_color.b, 255);
            }
        }
    }
}

//törli a pálya elemeit, azaz mindent üresre állít
void clear_map(game_struct w)
{
    for (int i = 0; i < w.width+2; ++i)
    {
        for (int j = 0; j < w.heigth+2; ++j)
        {
            w.palya[j][i] = empty_tile;
        }
    }
}

//kirajzolja azt a halál utáni állapotot, amikor nem sikerült elérnünk annyi pontot, hogy bekerüljünk a top 10 be
///a menüpontok közötti lépkedés úgy mûködik, hogy minden menüpont feketén meg van rajzolva és az aktuálisan kiválasztottra rárajzolunk egy pirosat is
///ez az összes többi menüpont választásnál így történik
void draw_end_state(SDL_Renderer *renderer, highlight_s hls, game_struct w)
{
    int w_width = w.width * w.rect_size;
    int w_heigth = w.heigth * w.rect_size;
    char sztring[1000];
    sprintf(sztring, "%d", (w.snake_length-w.base_s_length)*50);
    char score_string[100] = "Your score: ";
    strcat(score_string, sztring);
    //boxRGBA(renderer, 0, 0, w_width, w_heigth, 255, 255, 255, 255);
    ///----------------------------------------------------
    SDL_Rect src = {0, 0, w.width * w.rect_size, w.heigth * w.rect_size};
    SDL_Rect dest = { 0, 0, w.width * w.rect_size, w.heigth * w.rect_size};
    SDL_RenderCopy(renderer, w.pic[0], &src, &dest);
    ///----------------------------------------------------
    stringRGBA(renderer, (w_width/2)-30, (w_heigth/2)-10, "YOU DIED", 255, 0, 0, 255);
    stringRGBA(renderer, (w_width/2)-50, (w_heigth/2), score_string, 255, 255, 255, 255);

    stringRGBA(renderer, (w_width/2)-20, (w_heigth/2)+30, "RETRY", 255, 255, 255, 255);
    stringRGBA(renderer, (w_width/2)-20, (w_heigth/2)+50, "MENU", 255, 255, 255, 255);
    stringRGBA(renderer, (w_width/2)-20, (w_heigth/2)+70, "QUIT", 255, 255, 255, 255);
    switch (hls.endmenu_highlights)
    {
        case retry_title:
            stringRGBA(renderer, (w_width/2)-20, (w_heigth/2)+30, "RETRY", 255, 0, 100, 255);
            break;

        case menu_title:
            stringRGBA(renderer, (w_width/2)-20, (w_heigth/2)+50, "MENU", 255, 0, 100, 255);
            break;

        case end_quit_title:
            stringRGBA(renderer, (w_width/2)-20, (w_heigth/2)+70, "QUIT", 255, 0, 100, 255);
            break;
    }
}
//kirajzoljuk a menüt
void draw_menu_state(SDL_Renderer *renderer, highlight_s hls, game_struct w)
{

    int w_width = w.width * w.rect_size;
    int w_heigth = w.heigth * w.rect_size;
    //boxRGBA(renderer, 0, 0, w_width, w_heigth, 255, 255, 255, 255);
    ///----------------------------------------------------
    SDL_Rect src = {0, 0, w.width * w.rect_size, w.heigth * w.rect_size};
    SDL_Rect dest = { 0, 0, w.width * w.rect_size, w.heigth * w.rect_size};
    SDL_RenderCopy(renderer, w.pic[0], &src, &dest);
    ///----------------------------------------------------
    stringRGBA(renderer, (w_width/2)-40, (w_heigth/2)-60, "SNAKE", 0, 150, 0, 255);
    stringRGBA(renderer, (w_width/2)-40, (w_heigth/2)-30, "PLAY", 255, 255, 255, 255);
    stringRGBA(renderer, (w_width/2)-40, (w_heigth/2)-10, "HIGHSCORES", 255, 255, 255, 255);
    stringRGBA(renderer, (w_width/2)-40, (w_heigth/2)+10, "CHANGE COLOR", 255, 255, 255, 255);
    stringRGBA(renderer, (w_width/2)-40, (w_heigth/2)+30, "QUIT", 255, 255, 255, 255);
    switch (hls.menu_highlights)
    {
        case play_title:
            stringRGBA(renderer, (w_width/2)-40, (w_heigth/2)-30, "PLAY", 255, 25, 70, 255);
            break;

        case scoreboard_title:
            stringRGBA(renderer, (w_width/2)-40, (w_heigth/2)-10, "HIGHSCORES", 255, 25, 70, 255);
            break;

        case color_title:
            stringRGBA(renderer, (w_width/2)-40, (w_heigth/2)+10, "CHANGE COLOR", 255, 25, 70, 255);
            break;

        case quit_title:
            stringRGBA(renderer, (w_width/2)-40, (w_heigth/2)+30, "QUIT", 255, 25, 70, 255);
            break;
    }
}
// játék közben, ha szüneteltetjük, akkor a pálya fölé rajzol egy szünet menüt
void draw_pause_menu(SDL_Renderer *renderer, game_struct w)
{
    stringRGBA(renderer, (w.width * w.rect_size/2)-10, (w.heigth * w.rect_size/2)+10, "PAUSED", 255, 255, 255, 255);
    stringRGBA(renderer, (w.width * w.rect_size/2)-60, (w.heigth * w.rect_size/2)+20, "Press P to continue", 255, 255, 255, 255);
}

//a játék állapotot rajzolja ki
int draw_play_state(SDL_Renderer *renderer, s_list * head, game_struct *w)
{
    clear_map(*w);

    w->palya[w->apple.y][w->apple.x] = apple_tile;
    s_list * current = head->next;  // pálya feltöltése a kígyó darabjaival
    while (current->next->next->next != NULL) {
        w->palya[current->snk.y][current->snk.x] = snake_tile;
        current = current->next;
    }
    draw_map(renderer, *w);
    filledCircleRGBA(renderer, (head->snk.x-2) * w->rect_size + (w->rect_size/2), (head->snk.y-2) * w->rect_size + (w->rect_size/2), 6, w->snake_color.r, w->snake_color.g, w->snake_color.b, 255);
    filledCircleRGBA(renderer, (current->snk.x-2) * w->rect_size + (w->rect_size/2), (current->snk.y-2) * w->rect_size + (w->rect_size/2), 6, w->snake_color.r, w->snake_color.g, w->snake_color.b, 255);
    filledCircleRGBA(renderer, (current->next->snk.x-2) * w->rect_size + (w->rect_size/2), (current->next->snk.y-2) * w->rect_size + (w->rect_size/2), 5, w->snake_color.r, w->snake_color.g, w->snake_color.b, 255);
    filledCircleRGBA(renderer, (current->next->next->snk.x-2) * w->rect_size + (w->rect_size/2), (current->next->next->snk.y-2) * w->rect_size + (w->rect_size/2), 4, w->snake_color.r, w->snake_color.g, w->snake_color.b, 255);
    switch (w->dir)
    {
        case left:
            boxRGBA(renderer, (head->snk.x-2) * w->rect_size + (w->rect_size/2), (head->snk.y-2) * w->rect_size,  ((head->snk.x-1) * w->rect_size), ((head->snk.y-1) * w->rect_size), w->snake_color.r, w->snake_color.g, w->snake_color.b, 255);
            //boxRGBA(renderer, (current->snk.x-2) * w->rect_size, (current->snk.y-2) * w->rect_size,  ((current->snk.x-1) * w->rect_size)-(w->rect_size/2), ((current->snk.y-1) * w->rect_size), 0, 255, 0, 255);

            break;

        case right:
            boxRGBA(renderer, (head->snk.x-2) * w->rect_size, (head->snk.y-2) * w->rect_size,  ((head->snk.x-1) * w->rect_size)-  (w->rect_size/2), ((head->snk.y-1) * w->rect_size), w->snake_color.r, w->snake_color.g, w->snake_color.b, 255);
            //boxRGBA(renderer, (current->snk.x-2) * w->rect_size + (w->rect_size/2), (current->snk.y-2) * w->rect_size,  ((current->snk.x-1) * w->rect_size), ((current->snk.y-1) * w->rect_size), 0, 255, 0, 255);
            break;

        case up:
            boxRGBA(renderer, (head->snk.x-2) * w->rect_size, ((head->snk.y-2) * w->rect_size)+(w->rect_size/2),  ((head->snk.x-1) * w->rect_size), ((head->snk.y-1) * w->rect_size), w->snake_color.r, w->snake_color.g, w->snake_color.b, 255);
            //boxRGBA(renderer, (current->snk.x-2) * w->rect_size, (current->snk.y-2) * w->rect_size,  ((current->snk.x-1) * w->rect_size), ((current->snk.y-1) * w->rect_size) - (w->rect_size/2), 0, 255, 0, 255);
            break;

        case down:
            boxRGBA(renderer, (head->snk.x-2) * w->rect_size, (head->snk.y-2) * w->rect_size,  ((head->snk.x-1) * w->rect_size), ((head->snk.y-1) * w->rect_size)-(w->rect_size/2), w->snake_color.r, w->snake_color.g, w->snake_color.b, 255);
            //boxRGBA(renderer, (current->snk.x-2) * w->rect_size, (current->snk.y-2) * w->rect_size + (w->rect_size/2),  ((current->snk.x-1) * w->rect_size), ((current->snk.y-1) * w->rect_size), 0, 255, 0, 255);
            break;
    }


    filledCircleRGBA(renderer, (head->snk.x-2) * w->rect_size + (w->rect_size/2), (head->snk.y-2) * w->rect_size + (w->rect_size/2), 3, w->eye.r, w->eye.g, w->eye.b, 255);


    if (check_snake_death(head, *w) == 1)
    {
        int last_score = check_highscore();
        if (last_score == -1)
        {
            return -1;
        }
        int new_score = (w->snake_length-w->base_s_length)*50;
        if (new_score > last_score)
        {
            w->gamestate = enter_name_state;
        }
        else
        {
            w->gamestate = end_state;
        }
    }
    snake_step(head, w->dir);

    if (w->palya[head->snk.y][head->snk.x] == apple_tile) //alma evés
    {
        spawn_apple(head, w);
        if (snake_grow(head) == -1)
        {
            return -1;
        }
        w->snake_length++;
    }
    return 0;
}

//kirajzolja a ranglistát
//a score.txt fájlból beolvassa az adatokat és kirajzolja õket
int draw_scoreboard_state(SDL_Renderer *renderer, highlight_s hls, game_struct w)
{
    int w_width = w.width * w.rect_size;
    int w_heigth = w.heigth * w.rect_size;
    //boxRGBA(renderer, 0, 0, w_width, w_heigth, 255, 255, 255, 255);
    ///----------------------------------------------------
    SDL_Rect src = {0, 0, w.width * w.rect_size, w.heigth * w.rect_size};
    SDL_Rect dest = { 0, 0, w.width * w.rect_size, w.heigth * w.rect_size};
    SDL_RenderCopy(renderer, w.pic[0], &src, &dest);
    ///----------------------------------------------------
    stringRGBA(renderer, (w_width/2)-40, (w_heigth/2)+70, "BACK TO MENU", 255, 255, 255, 255);
    stringRGBA(renderer, (w_width/2)-40, (w_heigth/2)+90, "QUIT", 255, 255, 255, 255);
    switch (hls.scoreboard_highlights)
    {
        case back_to_menu_title:
            stringRGBA(renderer, (w_width/2)-40, (w_heigth/2)+70, "BACK TO MENU", 255, 25, 70, 255);
            break;

        case scoreboard_quit_title:
            stringRGBA(renderer, (w_width/2)-40, (w_heigth/2)+90, "QUIT", 255, 25, 70, 255);
            break;
    }

    FILE *fp = fopen("resources/score.txt", "rt");
    if (fp == NULL)
    {
        perror("Fajl megnyitasa sikertelen");
        return -1;
    }

    char helyezes[2+1];
    char score_in[100+1];
    char nev[50+1];
    int i = 0;
    while (fscanf(fp, "%s %s %s", helyezes, nev, score_in) == 3)
    {
        stringRGBA(renderer, (w_width/2)-110, 30 + (20 * i), helyezes, 255, 255, 255, 255);
        stringRGBA(renderer, (w_width/2)-60, 30 + (20 * i), nev, 255, 255, 255, 255);
        stringRGBA(renderer, (w_width/2)+100, 30 + (20 * i), score_in, 255, 255, 255, 255);
        i++;
    }
    fclose(fp);
    return 0;
}

//a név beírós állapotot rajzolja ki a szöveg bekérõ dobozon kívül
void draw_enter_name_state(SDL_Renderer *renderer, game_struct w)
{
    int w_width = w.width * w.rect_size;
    int w_heigth = w.heigth * w.rect_size;
    //boxRGBA(renderer, 0, 0, w_width, w_heigth, 255, 255, 255, 255);
    ///----------------------------------------------------
    SDL_Rect src = {0, 0, w.width * w.rect_size, w.heigth * w.rect_size};
    SDL_Rect dest = { 0, 0, w.width * w.rect_size, w.heigth * w.rect_size};
    SDL_RenderCopy(renderer, w.pic[0], &src, &dest);
    ///----------------------------------------------------
    stringRGBA(renderer, (w_width/2)-100, (w_heigth/2)-50, "You've got a new highscore!", 255, 255, 255, 255);
    stringRGBA(renderer, (w_width/2)-100, (w_heigth/2)-30, "Please enter your name:", 255, 255, 255, 255);
}

