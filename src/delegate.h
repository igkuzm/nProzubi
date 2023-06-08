/**
 * File              : delegate.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 15.05.2023
 * Last Modified Date: 08.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef DELEGATE_H
#define DELEGATE_H

#include <cdk.h>
#include <cdk/cdkscreen.h>
#include <cdk/draw.h>
#include <curses.h>
#include "prozubilib/prozubilib.h"

#define SCREENS\
	SCREEN(cases, screen)\
	SCREEN(patientEdit, screen)\
	SCREEN(priceList, screen)\
	SCREEN(xray, cases)\
	SCREEN(planLecheniya, cases)\
	SCREEN(planLecheniyaEdit, planLecheniya)\
	SCREEN(zformula, cases)\
	SCREEN(textEdit, cases)\
	SCREEN(combobox, cases)\
	SCREEN(date, cases)\
	SCREEN(nomenklatura, priceList)\
	SCREEN(imageselect, xray)\
	SCREEN(imageshow, xray)\

typedef struct delegate_t {
	prozubi_t *p;
	CDKSCREEN *screen;
	CDKSCREEN *selectedScreen;
	CDKLABEL *infoPannel;
	CDKSELECTION *patients;
	CDKSELECTION *casesList;
#define SCREEN(title, ...)\
	CDKSCREEN * title;\
	WINDOW * s_##title;\
	WINDOW * b_##title;\

	SCREENS
#undef SCREEN

	CDKSELECTION *imagesList;
	struct image_t **images;
	int imagesCount;
	char **imagesTitles;
	FILE *log;
}delegate_t;

static void delegate_init(
		delegate_t *d
		)
{
	d->p                  = NULL;
	d->screen             = NULL;
	d->infoPannel         = NULL;
#define SCREEN(title, ...)\
	d->title = NULL;\
	d->s_##title = NULL;\
	d->b_##title = NULL;\

	SCREENS
#undef SCREEN

	d->patients           = NULL;
	d->casesList          = NULL;
	d->imagesList         = NULL;
	d->images             = NULL;
	d->imagesCount        = 0;
	d->imagesTitles       = NULL;
	d->log                = NULL;
}

#define SCREEN(title, ...)\
static CDKSCREEN * screen_init_##title(delegate_t *d, int h, int w, int y, int x, int color)\
{\
	d->s_##title = newwin(h + 1, w + 1, y + 1, x + 2);\
	drawShadow(d->s_##title);\
	d->b_##title = newwin(h, w, y, x);\
	wbkgd(d->b_##title, COLOR_PAIR(color));\
	drawShadow(d->b_##title);\
	WINDOW *win = newwin(h, w, y, x);\
	d->title = initCDKScreen(win);\
	wbkgd(win, COLOR_PAIR(color));\
	return d->title;\
}

	SCREENS
#undef SCREEN

#define SCREEN(title, ...)\
static void screen_redraw_##title(delegate_t *d)\
{\
	if (d->s_##title)\
		drawShadow(d->s_##title);\
		redrawwin(d->s_##title);\
	if (d->b_##title)\
		drawShadow(d->b_##title);\
		redrawwin(d->b_##title);\
	refreshCDKScreen(d->title);\
}
	SCREENS
#undef SCREEN


#define SCREEN(title, parent)\
static void screen_destroy_##title(delegate_t *d)\
{\
	WINDOW *win = d->title->window;\
	destroyCDKScreen(d->title);\
	d->title = NULL;\
	werase(d->s_##title);\
	d->s_##title = NULL;\
	werase(d->b_##title);\
	d->b_##title = NULL;\
	werase(win);\
	refreshCDKScreen(d->parent);\
}

	SCREENS
#undef SCREEN

#endif /* ifndef DELEGATE_H */
