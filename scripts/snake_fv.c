#include "snake_fv.h"
#include "draw_fv.h"
#include "debugmalloc.h"

// hozzáad egy új elemet a snake lista végéhez
s_list *snake_add(s_list * head, pont snk)
{
    if (head == NULL)
    {
        head = (s_list *) malloc(sizeof(s_list));
        if (head == NULL)
        {
            return NULL;
        }
        head->next = NULL;
        head->snk = snk;
    }
    else
    {
        s_list * current = head;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = (s_list *) malloc(sizeof(s_list));
        if (current->next == NULL)
        {
            return NULL;
        }
        current->next->snk = snk;
        current->next->next = NULL;
    }
    return head;
}

//kiírja a snake lista összes elemét, azaz a snake darabjainak a mező koordinátáit
void print_list(s_list * head)
{
    s_list * current = head;
    while (current != NULL)
    {
        printf("%d %d\n", current->snk.x, current->snk.y);
        current = current->next;
    }
    printf("\n");
}

//törli a snake lista utolsó elemét
void remove_last(s_list * head)
{
    if (head->next == NULL)
    {
        free(head);
    }
    s_list * current = head;
    while (current->next->next != NULL)
    {
        current = current->next;
    }
    free(current->next);
    current->next = NULL;
}

//a kezdeti értékre állítja a snake lista elemeit és számát
//majdnem az összes elemet töröljük, és a megfelelő elemeket pedig hozzáadjuk
int snake_reset(s_list * head, game_struct *w)
{
    while(w->snake_length > 1)
    {
        remove_last(head);
        (w->snake_length)--;
    }
    head->snk.x = w->base_s_length+1;
    head->snk.y = w->heigth/2;

    pont snk_push;
    for (int i = 0; i < (w->base_s_length-1); ++i)
    {
        snk_push.x = w->base_s_length-i;
        snk_push.y = w->heigth/2;
        head = snake_add(head, snk_push);
        if (head == NULL)
        {
            return -1;
        }
        (w->snake_length)++;
    }
    w->dir = right;
    return 0;
}
//ellenőrzi, hogy meghaltunk-e a játékban, egyrészt hogy magunkba ütköztünk-e (megnézzi, hogy a kígyó feje egy helyen van-e egy másik darabjával)
//másrészt, hogy nem-e ütköztünk neki afalnak (azaz mentünk túl a pálya szélén)
int check_snake_death(s_list * head, game_struct w)
{
    s_list *current = head;  //magába ütközés ellenõrzése
    while (current->next != NULL)
    {
        current = current->next;
        if (current->snk.x == head->snk.x && current->snk.y == head->snk.y)
        {
            return 1;
        }
    }

    if(head->snk.y >= w.heigth+2 || head->snk.y < 2 || head->snk.x < 2 || head->snk.x >= w.width+2) // falnak ütközés ellenõrzése
    {
        return 1;
    }
    return 0;

}
//ez a fv végzi el a kígyó "mozgását", az első algoritmus a lista elemeit tolja egyel hátra,
//majd a következő az első elemnek ad új értéket az iránytól függően
void snake_step(s_list * head, direction dir)
{
    s_list *current = head; // léptetés
    pont tarol1 = head->snk;
    pont tarol2 = head->snk;
    while (current->next != NULL)
    {
        tarol2 = current->next->snk;
        current->next->snk = tarol1;
        tarol1 = tarol2;
        current = current->next;
    }

    switch(dir)  //léptetés értékadása
    {
        case left:
            head->snk.x--;
            print_list(head);
            break;

        case right:
            head->snk.x++;
            print_list(head);
            break;

        case up:
            head->snk.y--;
            print_list(head);
            break;

        case down:
            head->snk.y++;
            print_list(head);
            break;
    }
}

//alma evés esetén van meghívva, a végéhez hozzáfűzünk egy az utolsó  elem értékeivel egyenlő elemet, így a következő lépésnél már látszódik a növekedés
int snake_grow(s_list * head)
{
       //növelés
    s_list *current = head;
    while (current->next != NULL)
    {
        current = current->next;
    }
    pont snk_push;
    snk_push.x = current->snk.x;
    snk_push.y = current->snk.y;
    head = snake_add(head, snk_push);
    if (head == NULL)
    {
        return -1;
    }
    return 0;
}
//kezdetleges létrehozása a snake listának
int snake_init(s_list **head, game_struct *w)
{
    *head = (s_list *) malloc(sizeof(s_list));
    if (*head == NULL)
    {
        return -1;
    }
    (*head)->snk.x = 6;
    (*head)->snk.y = w->heigth/2;
    (*head)->next = NULL;

    if (snake_reset(*head, w) == -1)
    {
        return -1;
    }
    print_list(*head);
    return 0;
}

void free_snake(s_list * head)
{
    s_list *current = head;
    s_list *helper = current;
    while (current != NULL)
    {
        current = current->next;
        free(helper);
        helper = current;
    }
}
