#include "game_board.h"

//egyéd functions
//lerak egy új almát, mégpedig úgy, hogy ne a kígyó alá tegye, hanem egy üres mezõre
//ehhez végigmegy a kígyó darabjain, és ha a lerakott alma valamelyik alatt lenne, akkor lerak egy újat és addig folytatja amíg nem üres mezõre kerül az alma
void spawn_apple(s_list * head, game_struct *w)  /// mi van ha a kígyó beteríti az egészet? valószínûleg leáll a játék és nyertél...
{
    bool spawned = false;
    while (!spawned)
    {
        spawned = true;
        w->apple.x = (rand() % w->width)+2;
        w->apple.y = (rand() % w->heigth)+2;
        s_list *current = head;
        while (current->next != NULL && spawned)
        {
            if (current->snk.x == w->apple.x && current->snk.y == w->apple.y)
            {
                spawned = false;
            }
            current = current->next;
        }
    }
}
// lefoglalja a pályát dinamikus memória kezeléssel
int foglal(game_struct *w) {
    palya_enum **uj;

    uj = (palya_enum**) malloc((w->heigth+4) * sizeof(palya_enum*));
    if (uj == NULL)
    {
        return -1;
    }

    for (palya_enum y = 0; y < (w->heigth+4); ++y)
    {
       uj[y] = (palya_enum*) malloc((w->width+4) * sizeof(palya_enum));
       if (uj[y] == NULL)
       {
           return -1;
       }
    }
    w->palya = uj;
    return 0;
}
//felszabadítjuk a dinamikusan foglalt pályát
void free_all(game_struct *w)
{
    for (int y = 0; y < (w->heigth+4); ++y)
    {
        free(w->palya[y]);
    }
    free(w->palya);
}

int highscore_placment(char *new_name, int new_score)
{

    highscore_struct hs_struct;
    FILE *fp = fopen("resources/score.txt", "rt");
    if (fp == NULL)
    {
        perror("Fajl megnyitasa sikertelen");
        return -1;
    }

    int db = 0;
    int rank, score;
    char nev[50];
    while (fscanf(fp, "%d %s %d", &rank, nev, &score) == 3)
    {
        if (db > 9)
        {
            printf("Tul sok nev van a ranglistan.");
            return -1;
        }
        hs_struct.ranks_t[db] = rank;
        hs_struct.score_t[db] = score;
        strcpy(hs_struct.names[db], nev);
        printf("%d %s %d\n",hs_struct.ranks_t[db], hs_struct.names[db], hs_struct.score_t[db]);
        db++;
    }

    fclose(fp);
    //printf("############################ %d\n", db);
    int j = db-1;
    while(j > 0 && new_score > hs_struct.score_t[j])
    {
        j--;
    }
    if (j != 0 || (j == 0 && hs_struct.score_t[0] >= new_score))
    {
        j++;
    }

    int i = db-1;
    while (i > j)
    {
        hs_struct.score_t[i] = hs_struct.score_t[i-1];
        strcpy(hs_struct.names[i], hs_struct.names[i-1]);
        i--;
    }
    strcpy(hs_struct.names[j], new_name);
    hs_struct.score_t[j] = new_score;
    fp = fopen("resources/score.txt", "w");
    if (fp == NULL)
    {
        perror("Fajl megnyitasa sikertelen");
        return -1;
    }

    for (int i = 0; i < db; ++i)
    {
        fprintf(fp, "%d %s %d\n", i+1, hs_struct.names[i], hs_struct.score_t[i]);
    }
    fclose(fp);
    return 0;
}

gamestate_enum enter_name_state_fv(SDL_Renderer *renderer, game_struct sw, TTF_Font *font)
{
    draw_enter_name_state(renderer, sw);
    SDL_Color feher = {255, 255, 255}, fekete = { 0, 0, 0 };
    SDL_Rect r = { (sw.rect_size*sw.width/2)-100, (sw.rect_size*sw.heigth/2)-10, 200, 40};
    char new_name[50];
    if (input_text(new_name, 100, r, fekete, feher, font, renderer))
    {
        int new_score = (sw.snake_length-sw.base_s_length)*50;
        if (highscore_placment(new_name, new_score) == -1)
        {
            return -1;
        }
    }
    return scoreboard_state;
}

int check_highscore()
{
    FILE *fp = fopen("resources/score.txt", "rt");
    if (fp == NULL)
    {
        perror("Fajl megnyitasa sikertelen");
        return -1;
    }

    int rank, score;
    char nev[50];
    while (fscanf(fp, "%d %s %d", &rank, nev, &score) == 3)
    {
        printf("%d %s %d\n", rank, nev, score);
    }
    fclose(fp);
    return score;
}

void close_and_free(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font, game_struct *sw, s_list *head)
{
    sdl_close(&window, &renderer, &font); //ablak bezarasa

    destroy_images(sw);
    //Mix_FreeChunk(sw->sounds[0]);
    Mix_FreeMusic(sw->musics[0]);
    Mix_CloseAudio();

    free_all(sw);
    free_snake(head);

    printf("\nEND\n");
}
