#ifndef WIDGETS_H
#define WIDGETS_H
#include "snake_fv.h"
#include "draw_fv.h"

/* mindenki hasznalja */


/* egy widget, az altalanos es a specialis adatokkal */
typedef struct Widget Widget;
struct Widget {
    int x, y, szeles, magas;            /* pozicio es meret */

    /* kirajzolashoz hasznalt fv */
    void (*rajzolo_fv)(SDL_Renderer *renderer, Widget *widget);
    void (*kattintas_fv)(SDL_Renderer *renderer, Widget *widget, int x, int y);
    /* kattintaskor ez hivodik, ha nem NULL */
    void (*felhasznaloi_cb)(SDL_Renderer *renderer, Widget *widget, int x, int y, void *param, game_struct *sw);
    void *felhasznaloi_cb_param;           /* ezt a parametert megkapja a param valtozoban */

    enum WidgetTipus {                  /* ilyen típusú lehet */
        gomb, csuszka, szinesteglalap, felirat
    } tipus;
    union {
        struct {
            char felirat[20];           /* a gomb szövege */
        } gomb;
        struct {
            double jelenlegi;           /* erteke; 0.0-1.0 */
        } csuszka;
        struct {
            unsigned char r, g, b;      /* szin */
        } szinesteglalap;
        struct {
            char szoveg[20];
        } felirat;
    } adat;
};

typedef struct UIAdat {
    Widget *r, *g, *b, *teglalap;
} UIAdat;

void widget_alap_rajzol(SDL_Renderer *renderer, Widget *widget);
Widget *uj_widget(int x, int y, int szeles, int magas);
void gomb_rajzol(SDL_Renderer *renderer, Widget *gomb);
Widget *uj_gomb(int x, int y, int szeles, int magas, char const *felirat);
void felirat_rajzol(SDL_Renderer *renderer, Widget *felirat);
Widget *uj_felirat(int x, int y, char const *szoveg);
void csuszka_rajzol(SDL_Renderer *renderer, Widget *csuszka);
void csuszka_kattintas(SDL_Renderer *renderer, Widget *csuszka, int x, int y);
Widget *uj_csuszka(int x, int y, int szeles, int magas, double kezdeti);
void szinesteglalap_rajzol(SDL_Renderer *renderer, Widget *teglalap);
Widget *uj_szinesteglalap(int x, int y, int szeles, int magas, unsigned char r, unsigned char g, unsigned char b);
void esemenyvezerelt_main(SDL_Renderer *renderer, Widget *widgetek[], game_struct *sw);
void kilep_gomb_cb(Widget *widget, int x, int y, void *param, game_struct *sw);

void mehet_gomb_cb(SDL_Renderer *renderer, Widget *widget, int x, int y, void *param, game_struct *sw);

#endif
