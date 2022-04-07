#include "widgets.h"

unsigned long const hatter = 0x303030FF;
unsigned long const keret = 0x000000FF;
unsigned long const alapszin = 0x101010FF;
unsigned long const keretvilagos = 0xFFFFFF20;
unsigned long const gombfeliratszin = 0x00C0F0FF;
unsigned long const csuszkaszin = 0x00FFFF50;
unsigned long const feliratszin = 0x00C0F0FF;

/* egy widget kirajzolasa, altalanossagban. egy keretet ad csak. */
/* a keret tulnyulik az aktiv (kattinthato) mereten. */
void widget_alap_rajzol(SDL_Renderer *renderer, Widget *widget) {
    roundedRectangleColor(renderer, widget->x - 1, widget->y - 1, widget->x + widget->szeles, widget->y + widget->magas, 2, keret);
    boxColor(renderer, widget->x, widget->y, widget->x + widget->szeles - 1, widget->y + widget->magas - 1, alapszin);
    for (int y = 0; y < 20; ++y) {
        boxRGBA(renderer, widget->x, widget->y + y * widget->magas / 20, widget->x + widget->szeles - 1, widget->y + (y + 1)*widget->magas / 20 - 1, 255, 255, 255, (19 - y) * 3);
    }
    rectangleColor(renderer, widget->x, widget->y, widget->x + widget->szeles - 1, widget->y + widget->magas - 1, keretvilagos);
}

/* letrehoz egy uj, egyelore ismeretlen tipusu widgetet. tulajdonkepp csak a helyet allitja be */
Widget *uj_widget(int x, int y, int szeles, int magas) {
    Widget *uj = (Widget *) malloc(sizeof(Widget));
    uj->x = x;
    uj->y = y;
    uj->szeles = szeles;
    uj->magas = magas;
    uj->rajzolo_fv = widget_alap_rajzol;
    uj->kattintas_fv = NULL;
    uj->felhasznaloi_cb = NULL;
    uj->felhasznaloi_cb_param = NULL;
    return uj;
}

/* kirajzolja a gombot. keret, alapszin, felirat. */
void gomb_rajzol(SDL_Renderer *renderer, Widget *gomb) {
    widget_alap_rajzol(renderer, gomb);
    stringColor(renderer, gomb->x + (gomb->szeles - strlen(gomb->adat.gomb.felirat) * 8) / 2, gomb->y + (gomb->magas - 8) / 2, gomb->adat.gomb.felirat, gombfeliratszin);
}

/* gombot hoz letre. */
Widget *uj_gomb(int x, int y, int szeles, int magas, char const *felirat) {
    Widget *uj = uj_widget(x, y, szeles, magas);
    uj->tipus = gomb;
    uj->rajzolo_fv = gomb_rajzol;
    strcpy(uj->adat.gomb.felirat, felirat);
    return uj;
}

/* kirajzol egy feliratot. nem hivodik az alap rajzolo, mert nem kell neki keret! */
void felirat_rajzol(SDL_Renderer *renderer, Widget *felirat) {
    stringColor(renderer, felirat->x, felirat->y, felirat->adat.felirat.szoveg, feliratszin);
}

/* feliratot hoz letre. */
Widget *uj_felirat(int x, int y, char const *szoveg) {
    Widget *uj = uj_widget(x, y, 0, 0);
    uj->tipus = felirat;
    uj->rajzolo_fv = felirat_rajzol;
    strcpy(uj->adat.felirat.szoveg, szoveg);
    return uj;
}

/* csuszkaot rajzol; a keretet meghagyja, de belul mindent
 * felulir a sajat szinevel */
void csuszka_rajzol(SDL_Renderer *renderer, Widget *csuszka) {
    widget_alap_rajzol(renderer, csuszka);
    boxColor(renderer, csuszka->x, csuszka->y, csuszka->x + csuszka->szeles * csuszka->adat.csuszka.jelenlegi, csuszka->y + csuszka->magas - 1, csuszkaszin);
}

/* ez a sajat adatfeldolgozoja; nem kulon hozzaadott, hanem elvalaszthatatlan */
void csuszka_kattintas(SDL_Renderer *renderer, Widget *csuszka, int x, int y) {
    csuszka->adat.csuszka.jelenlegi = (double) (x - 1) / (csuszka->szeles);
    csuszka_rajzol(renderer, csuszka);
}

/* csuszkat hoz letre */
Widget *uj_csuszka(int x, int y, int szeles, int magas, double kezdeti) {
    Widget *uj = uj_widget(x, y, szeles, magas);
    uj->tipus = csuszka;
    uj->rajzolo_fv = csuszka_rajzol;
    uj->kattintas_fv = csuszka_kattintas;
    uj->adat.csuszka.jelenlegi = kezdeti;
    return uj;
}

/* szines teglalapot rajzol ki; az eredeti tartalmat teljesen felulirja az adott szinnel */
void szinesteglalap_rajzol(SDL_Renderer *renderer, Widget *teglalap) {
    widget_alap_rajzol(renderer, teglalap);
    boxRGBA(renderer, teglalap->x, teglalap->y, teglalap->x + teglalap->szeles - 1, teglalap->y + teglalap->magas - 1,
            teglalap->adat.szinesteglalap.r, teglalap->adat.szinesteglalap.g, teglalap->adat.szinesteglalap.b, 255);
}

/* szines teglalapot hoz letre, kezdeti ertekkent a megadott szinekkel */
Widget *uj_szinesteglalap(int x, int y, int szeles, int magas, unsigned char r, unsigned char g, unsigned char b) {
    Widget *uj = uj_widget(x, y, szeles, magas);
    uj->tipus = szinesteglalap;
    uj->rajzolo_fv = szinesteglalap_rajzol;
    uj->adat.szinesteglalap.r = r;
    uj->adat.szinesteglalap.g = g;
    uj->adat.szinesteglalap.b = b;
    return uj;
}

/* az esemenyhurok kirajzolja a widgeteket, es feldolgozza a felhasznalotol erkezo
 * esemenyeket (eger kattintasok es ablak bezarasa.) */
void esemenyvezerelt_main(SDL_Renderer *renderer, Widget *widgetek[], game_struct *sw) {

    /* azzal kezdjuk, hogy mindent kirajzolunk */
    SDL_SetRenderDrawColor(renderer, (hatter & 0xFF000000) >> 6, (hatter & 0x00FF0000) >> 4, (hatter & 0x0000FF00) >> 2, (hatter & 0x000000FF));
    SDL_RenderClear(renderer);
    ///--------------------------------------------------------------------------------------------
    SDL_Rect src = {0, 0, sw->width * sw->rect_size, sw->heigth * sw->rect_size};
    SDL_Rect dest = { 0, 0, sw->width * sw->rect_size, sw->heigth * sw->rect_size};
    SDL_RenderCopy(renderer, sw->pic[0], &src, &dest);
    ///--------------------------------------------------------------------------------------------
    for (int i = 0; widgetek[i] != NULL; i++)
        widgetek[i]->rajzolo_fv(renderer, widgetek[i]);
    SDL_RenderPresent(renderer);

    /* esemenyhurok */
    bool vege = false;
    SDL_Event ev;
    while (!vege && SDL_WaitEvent(&ev)) {
        switch (ev.type) {
        case SDL_QUIT:
            vege = true;
            break;

        case SDL_MOUSEBUTTONUP:
            for (int i = 0; widgetek[i] != NULL; i++) {
                /* ennek a widgetnek a teruletere kattintott? */
                if (ev.button.x >= widgetek[i]->x && ev.button.y >= widgetek[i]->y
                        && ev.button.x < widgetek[i]->x + widgetek[i]->szeles
                        && ev.button.y < widgetek[i]->y + widgetek[i]->magas)
                {
                    /* widget-relativ kattintas koordinatak */
                    int bx = ev.button.x - widgetek[i]->x;
                    int by = ev.button.y - widgetek[i]->y;
                    /* widgetek belso mukodese */
                    if (widgetek[i]->kattintas_fv != NULL)
                        widgetek[i]->kattintas_fv(renderer, widgetek[i], bx, bx);
                    /* esetleg tarsitott callback */
                    if (widgetek[i]->felhasznaloi_cb != NULL)
                        widgetek[i]->felhasznaloi_cb(renderer, widgetek[i], bx, by,
                                                     widgetek[i]->felhasznaloi_cb_param, sw);
                }
            }
            SDL_RenderPresent(renderer);       /* tortenhettek valtozasok, rajzolas */
            break;
        }
    }
}

/* a gomb rakattintva general egy szintetikus SDL_QUIT esemenyt.
 * barmely alkalmazas hasznalhatja kilepes gombnak */
void kilep_gomb_cb(Widget *widget, int x, int y, void *param, game_struct *sw) {
    SDL_Event ev = {SDL_QUIT};
    SDL_PushEvent(&ev);
}

/*
 * INNENTOL JON AZ ALKALMAZASUNK
 */

/* egy ilyen strukturara mutato pointer lesz az apply gomb void* parametere.
 * tartalmazza a widgeteket, hogy r,g,b csuszkaokbol a teglalap szinezheto legyen. */

void mehet_gomb_cb(SDL_Renderer *renderer, Widget *widget, int x, int y, void *param, game_struct *sw) {
    UIAdat *widgetek = (UIAdat *) param;
    unsigned char r = widgetek->r->adat.csuszka.jelenlegi * 255.0;
    unsigned char g = widgetek->g->adat.csuszka.jelenlegi * 255.0;
    unsigned char b = widgetek->b->adat.csuszka.jelenlegi * 255.0;
    widgetek->teglalap->adat.szinesteglalap.r = r;
    widgetek->teglalap->adat.szinesteglalap.g = g;
    widgetek->teglalap->adat.szinesteglalap.b = b;
    widgetek->teglalap->rajzolo_fv(renderer, widgetek->teglalap);
    sw->snake_color.r = r;
    sw->snake_color.g = g;
    sw->snake_color.b = b;
    sw->eye.r = abs(200 - b);
    sw->eye.g = abs(200 - g);
    sw->eye.b = abs(200 - r);
}
