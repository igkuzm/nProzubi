/**
 * File              : PriceList.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 05.06.2023
 * Last Modified Date: 10.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#include "PriceList.h"

#include <cdk.h>
#include <cdk/cdk_objs.h>
#include <cdk/cdkscreen.h>
#include <cdk/traverse.h>
#include <stdio.h>
#include "colors.h"
#include "input.h"
#include "error.h"
#include "prozubilib/prozubilib.h"
#include "prozubilib/alloc.h"
#include "NomenklaruraList.h"
#include "TextUTF8Handler.h"


struct price_list_t {
	delegate_t *d;
	CDKSELECTION *s;
	void * userdata;
	void (*callback)(void *userdata, const struct price_t *price);
	struct price_t **prices;
	char **titles;
	int count;
};

int 
price_list_update_callback(void *userdata, struct price_t *price){
	struct price_list_t *t = userdata;
	t->prices[t->count] = price;
	t->titles[t->count] = MALLOC(256, 
			error_callback(t->d->screen_price_list, "can't allocate memory"), return 0);
	snprintf(t->titles[t->count], 255, "%d. %s %s %s", 
			t->count + 1, price->title, price->price, price->kod);

	t->count++;

	t->prices = REALLOC(t->prices, t->count * 8 + 8, 
			error_callback(t->d->screen_price_list, "can't allocate memory"), return 0);
	
	t->titles = REALLOC(t->titles, t->count * 8 + 8, 
			error_callback(t->d->screen_price_list, "can't allocate memory"), return 0);

	return 0;
}

void 
price_list_update(struct price_list_t *t) 
{
	int i;
	for (i = 0; i < t->count; ++i) {
		prozubi_prices_free(t->prices[i]);
		free(t->titles[i]);
	}
	t->count = 0;
	if (t->prices)
		free(t->prices);
	if (t->titles)
		free(t->titles);
	
	t->prices = MALLOC(8, error_callback(t->d->screen_price_list, "can't allocate memory"), return);
	t->titles = MALLOC(8, error_callback(t->d->screen_price_list, "can't allocate memory"), return);

	prozubi_price_foreach(t->d->p, t, price_list_update_callback);
	
	setCDKSelectionItems(t->s, t->titles, t->count);
	refreshCDKScreen(t->d->screen_price_list);
}

static int 
prices_list_preHandler (EObjectType cdktype GCC_UNUSED, void *object,
		       void *clientData GCC_UNUSED,
		       chtype input GCC_UNUSED)
{
	CDKSELECTION *s = object;
	struct price_list_t *t = clientData;
	int selected = getCDKSelectionCurrent(s);
	
	switch (input) {
		case KEY_ENTER:
			{
				if (t->callback)
					t->callback(t->userdata, t->prices[selected]);
				break;
			}
		case 'q': case CTRL('q'):
			{
				exitOKCDKScreen(t->d->screen_price_list);
				return 0;
			}
		case 'a':
			{
				nomenklatura_list_create(t->d, NULL, NULL);
				price_list_update(t);
				return 0;
			}

		default:
			break;
	}

	return 1;
}


void
price_list_create(
		delegate_t *d,
		void * userdata,
		void (*callback)(void *userdata, const struct price_t *price)
		)
{
	/* init window and screen */
	screen_init_screen_price_list(d, LINES/2, COLS/2, LINES/4, COLS/4, COLOR_BLACK_ON_YELLOW);
	char * choises[] = 
	{""};
	
	CDKSELECTION 
		*s =
					newCDKSelection(
							d->screen_price_list, 
							COLS/4, 
							LINES/4, 
							0,
							LINES/2,
							COLS/2,
							"</B>Список услуг (прайс):<!B>", 
							NULL, 
							0, 
							choises,
							1,
							A_REVERSE,
							FALSE, 
							FALSE
							);

	d->casesList = s;
	setCDKSelectionBackgroundColor(s, COLOR_N(COLOR_BLACK_ON_YELLOW));
	bindCDKObject (vSELECTION, s, KEY_MOUSE, input_mouse_handler, d);\

	struct price_list_t t;
	t.d = d;
	t.s = s;
	t.count = 0;
	t.titles = NULL;
	t.prices = NULL;
	t.userdata = userdata;
	t.callback = callback;

	price_list_update(&t);
	
	setCDKSelectionPreProcess (s, prices_list_preHandler, &t);
	setCDKSelectionPostProcess (s, screen_update_postHandler, NULL);
	
	/*info_pannel_set_text(d, */
			/*"CTRL-q - закрыть, ENTER - редактировать");*/
	
	/* start traverse */
	traverseCDKScreen(d->screen_price_list);

	/* destroy widgets */
	destroyCDKSelection(s);

	screen_destroy_screen_price_list(d);
}

