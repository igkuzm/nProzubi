/**
 * File              : NomenklaruraList.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 06.06.2023
 * Last Modified Date: 26.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "NomenklaruraList.h"

#include "prozubilib/prozubilib.h"
#include "colors.h"
#include "input.h"
#include "error.h"
#include "TextUTF8Handler.h"
#include <cdk/cdkscreen.h>
#include <curses.h>

struct nomenklatura_list_t {
	delegate_t *d;
	CDKSELECTION *s;
	nomenklatura_t **nomenklaturas;
	char **titles;
	int count;
};

static void 
nomenklatura_list_add_price(struct nomenklatura_list_t *t, int index)
{
	delegate_t *d = t->d;
	
	/* *INDENT-EQLS* */
	char *buttons[] =
	{
		COLOR_STR(COLOR_BLUE_ON_YELLOW, "Добавить"), 
	  " Cancel ",
	};
  
	char *message[3] = {"Добавить к прайсу?"};
	message[1] = t->nomenklaturas[index]->name;
	message[2] = t->nomenklaturas[index]->kod;

	int selection;

	CDKDIALOG *m = newCDKDialog (
	d->screen_nomenklatura	/* cdkscreen */,
	CENTER		/* xPos */,
	CENTER		/* yPos */,
	message	/* message */,
	3		/* Rows */,
	buttons	/* buttons */,
	2		/* buttonCount */,
	A_REVERSE		/* highlight */,
	TRUE		/* separator */,
	TRUE		/* Box */,
	TRUE		/* shadow */
			);

	wbkgd(m->win, COLOR_PAIR(COLOR_BLACK_ON_MAGENTA));
	bindCDKObject (vDIALOG, m, KEY_MOUSE, input_mouse_handler, d);\

	/* Activate the entry field. */
	activateCDKDialog(m, NULL);
	
	if (m->currentButton == 0){
		struct price_t *price = NULL;
		price =
			prozubi_price_new(
				t->d->p, 
				strdup(t->nomenklaturas[index]->name), 
				strdup(t->nomenklaturas[index]->headName), 
				strdup(t->nomenklaturas[index]->kod), 
				strdup("5000"),
				strdup(t->nomenklaturas[index]->name), 
				NULL);
		//if (price)
			//prozubi_prices_free(price);
	}
	
	destroyCDKDialog(m);
	refreshCDKScreen(d->screen_nomenklatura);
}


void * 
nomenklatura_list_update_callback(void *userdata, void *parent, nomenklatura_t *nomenklatura){
	struct nomenklatura_list_t *t = userdata;
	t->nomenklaturas[t->count] = nomenklatura;
	
	t->titles[t->count] = MALLOC(256, 
			error_callback(t->d->screen_nomenklatura, "can't allocate memory"), return 0);
	
	if (parent == NULL)
		snprintf(t->titles[t->count], 255, "</U>%s<!U>", 
				nomenklatura->name);
	else
		snprintf(t->titles[t->count], 255, "\t%s %s", 
				nomenklatura->name, nomenklatura->kod);

	t->count++;

	t->nomenklaturas = REALLOC(t->nomenklaturas, t->count * 8 + 8, 
			error_callback(t->d->screen_nomenklatura, "can't allocate memory"), return 0);
	
	t->titles = REALLOC(t->titles, t->count * 8 + 8, 
			error_callback(t->d->screen_nomenklatura, "can't allocate memory"), return 0);

	return t->titles;
}
void
nomenklatura_list_free(struct nomenklatura_list_t *t)
{
	int i;
	for (i = 0; i < t->count; ++i) {
		prozubi_nomenklatura_free(t->nomenklaturas[i]);
		free(t->titles[i]);
	}
	t->count = 0;
	if (t->nomenklaturas)
		free(t->nomenklaturas);
	if (t->titles)
		free(t->titles);
}

void 
nomenklatura_list_update(struct nomenklatura_list_t *t) 
{
	nomenklatura_list_free(t);
	t->nomenklaturas = MALLOC(8, error_callback(t->d->screen_nomenklatura, "can't allocate memory"), return);
	t->titles = MALLOC(8, error_callback(t->d->screen_nomenklatura, "can't allocate memory"), return);

	prozubi_nomenklatura_foreach(t->d->p, t, nomenklatura_list_update_callback);
	
	setCDKSelectionItems(t->s, t->titles, t->count);
	refreshCDKScreen(t->d->screen_nomenklatura);
}

static int 
nomenklatura_list_preHandler (EObjectType cdktype GCC_UNUSED, void *object,
		       void *clientData GCC_UNUSED,
		       chtype input GCC_UNUSED)
{
	CDKSELECTION *s = object;
	struct nomenklatura_list_t *t = clientData;
	int selected = getCDKSelectionCurrent(s);
	
	switch (input) {
		case KEY_ENTER:
			{
				nomenklatura_list_add_price(t, selected);
				break;
			}
		case KEY_LEFT: case KEY_RIGHT: case KEY_UP: case KEY_DOWN:
			refreshCDKScreen(((CDKOBJS *)object)->screen);
			return 1;

		case 'q': case CTRL('q'):
			{
				exitOKCDKScreen(t->d->screen_nomenklatura);
				return 0;
			}

		default:
			break;
	}

	return 1;
}



void
nomenklatura_list_create(
		delegate_t *d,
		void *userdata,
		void callback(
			void *userdata,
			const char * title,
			const char *kod))
{
	/* init window and screen */
	screen_init_screen_nomenklatura(d, LINES/3, COLS/3, LINES/3, COLS/3, COLOR_BLACK_ON_CYAN);
	char * choises[] = 
	{""};
	
	CDKSELECTION 
		*s =
					newCDKSelection(
							d->screen_nomenklatura, 
							0, 
							0, 
							0,
							LINES/3,
							COLS/3,
							"</B>Номенклатура:<!B>", 
							NULL, 
							0, 
							choises,
							1,
							A_REVERSE,
							FALSE, 
							FALSE
							);

	d->casesList = s;
	setCDKSelectionBackgroundColor(s, COLOR_N(COLOR_BLACK_ON_CYAN));
	bindCDKObject (vSELECTION, s, KEY_MOUSE, input_mouse_handler, d);\

	struct nomenklatura_list_t t;
	t.d = d;
	t.s = s;
	t.count = 0;
	t.titles = NULL;
	t.nomenklaturas = NULL;

	nomenklatura_list_update(&t);
	setCDKSelectionPreProcess (s, nomenklatura_list_preHandler, &t);
	setCDKSelectionPostProcess (s, screen_update_postHandler, NULL);
	
	/*info_pannel_set_text(d, */
			/*"CTRL-q - закрыть, ENTER - редактировать");*/
	
	/* start traverse */
	traverseCDKScreen(d->screen_nomenklatura);

	/* destroy widgets */
	destroyCDKSelection(s);
	nomenklatura_list_free(&t);
	screen_destroy_screen_nomenklatura(d);
}
	
