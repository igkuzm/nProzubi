/**
 * File              : PlanLecheniya.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.06.2023
 * Last Modified Date: 08.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#include "PlanLecheniya.h"

#include "colors.h"
#include "InfoPannel.h"
#include "error.h"
#include "PlanLecheniyaEdit.h"
#include "input.h"
#include "prozubilib/alloc.h"
#include "prozubilib/prozubilib.h"
#include "prozubilib/planlecheniya.h"
#include <cdk.h>
#include <cdk/binding.h>
#include <cdk/cdkscreen.h>
#include <cdk/curdefs.h>
#include <cdk/selection.h>
#include "TextUTF8Handler.h"
#include "PriceList.h"
#include <stdio.h>

#define MAX_TITLES 200

typedef struct plan_lecheniya_t{
	delegate_t *d;
	struct case_t *c;
	CDKSELECTION *s;
	char **titles;
	int count;
	struct planlecheniya_t **t;
	int stage_index;
} PlanLecheniya;

void * 
plan_lecheniya_update_callback(
			void * userdata, 
			void * parent, 
			struct planlecheniya_t *t)
{
	PlanLecheniya *pl = userdata;
	char *str =  MALLOC(BUFSIZ, 
			error_callback(pl->d->planLecheniya, STR("can't allocate memory: %d", BUFSIZ)), 
			return NULL);

	pl->t[pl->count] = t; 
	switch (t->type) {
		case PLANLECHENIYA_TYPE_STAGE:
			snprintf(str, BUFSIZ, "</I=10>%s<!I>", t->title);		
			break;
		case PLANLECHENIYA_TYPE_ITEM:
			snprintf(str, BUFSIZ, "%d. x%s - %s", 
					t->itemIndex+1, t->count, t->title);		
			break;
		case PLANLECHENIYA_TYPE_STAGE_PRICE:
			snprintf(str, BUFSIZ, "</B>%s %s руб.<!B>", t->title, t->total);		
			break;
		case PLANLECHENIYA_TYPE_STAGE_DURATION:
			snprintf(str, BUFSIZ, "</B>%s %s<!B>", t->title, t->count);		
			break;
		case PLANLECHENIYA_TYPE_TOTAL_PRICE:
			snprintf(str, BUFSIZ, "</U>%s %s руб.<!U>", t->title, t->total);		
			break;
		case PLANLECHENIYA_TYPE_TOTAL_DURATION:
			snprintf(str, BUFSIZ, "</U>%s %s<!U>", t->title, t->count);		
			break;
		
		default:
			break;	
	}

	pl->titles[pl->count++] = str;
	return str;
}

void
plan_lecheniya_update(
		PlanLecheniya *pl
		)
{
	// free titles
	int i;
	for (i = 0; i < pl->count; ++i) {
		if (pl->titles[i])
			free(pl->titles[i]);
		pl->titles[i] = NULL;
		pl->t[i] = NULL;
	}
	pl->count = 0; 

	prozubi_planlecheniya_foreach(
			pl->d->p, 
			pl->c->planlecheniya, 
			pl, 
			plan_lecheniya_update_callback);

	if (pl->count > 0)
		setCDKSelectionItems(pl->s, pl->titles, pl->count);	
}

void price_add_callback(void *userdata, const struct price_t *price){
	PlanLecheniya *pl = userdata; 
	prozubi_planlecheniya_add_item(
			pl->d->p, 
			pl->c->planlecheniya, 
			pl->stage_index, 
			price->title, 
			price->kod, 
			atoi(price->price), 
			1);
}

int 
plan_lecheniya_handler(
		EObjectType cdktype GCC_UNUSED, 
		void *object,
		void *clientData GCC_UNUSED,
		chtype input GCC_UNUSED)
{
	CDKSELECTION *s = object;
	PlanLecheniya *pl = clientData; 
	int selected = getCDKSelectionCurrent(s);
	
	switch (input){
		case KEY_ENTER:
			{
				if (pl->t[selected]->type == PLANLECHENIYA_TYPE_ITEM){
					plan_lecheniya_edit_create(
							pl->d, 
							pl->c,
							pl->t[selected]->stageIndex,
							pl->t[selected]->itemIndex,
							pl->t[selected]->title,
							pl->t[selected]->price,
							pl->t[selected]->count,
							pl->t[selected]->kod
							);
					
					plan_lecheniya_update(pl);
				}
			}
			return 0;
		
		case '+':
			{
				log_callback(pl->d->log, STR("STAGE: %d, ITEM: %d", 
						pl->t[selected]->stageIndex, pl->t[selected]->itemIndex));
				if (pl->t[selected]->type == PLANLECHENIYA_TYPE_ITEM){
					int count = atoi(pl->t[selected]->count);
					count++;
					prozubi_planlecheniya_set_item_count(
							pl->d->p,
							pl->c->planlecheniya, 
							pl->t[selected]->stageIndex, 
							pl->t[selected]->itemIndex, 
							count);	
					
					plan_lecheniya_update(pl);
					pl->s->currentItem = selected;
					refreshCDKScreen(pl->d->planLecheniya);
				}
				else if (pl->t[selected]->type == PLANLECHENIYA_TYPE_STAGE_DURATION){
					int count = atoi(pl->t[selected]->count);
					count++;
					prozubi_planlecheniya_set_stage_duration(
							pl->d->p,
							pl->c->planlecheniya, 
							pl->t[selected]->stageIndex, 
							count);	
					
					plan_lecheniya_update(pl);
					pl->s->currentItem = selected;
					refreshCDKScreen(pl->d->planLecheniya);
				}
				return 0;
			}
		case '-':
			{
				if (pl->t[selected]->type == PLANLECHENIYA_TYPE_ITEM){
					int count = atoi(pl->t[selected]->count);
					count--;
					if (count < 1)
						count = 1;
					prozubi_planlecheniya_set_item_count(
							pl->d->p,
							pl->c->planlecheniya, 
							pl->t[selected]->stageIndex, 
							pl->t[selected]->itemIndex, 
							count);	
					plan_lecheniya_update(pl);
					pl->s->currentItem = selected;
					refreshCDKScreen(pl->d->planLecheniya);
				}
				else if (pl->t[selected]->type == PLANLECHENIYA_TYPE_STAGE_DURATION){
					int count = atoi(pl->t[selected]->count);
					count--;
					if (count < 1)
						count = 1;
					prozubi_planlecheniya_set_stage_duration(
							pl->d->p,
							pl->c->planlecheniya, 
							pl->t[selected]->stageIndex, 
							count);	
					plan_lecheniya_update(pl);
					pl->s->currentItem = selected;
					refreshCDKScreen(pl->d->planLecheniya);
				}


				return 0;
			}

		case 'a':
			{
				if (!pl->c->planlecheniya)
					prozubi_planlecheniya_new(pl->c);
				prozubi_planlecheniya_add_stage(pl->d->p, pl->c->planlecheniya);
				plan_lecheniya_update(pl);
				pl->s->currentItem = selected;
				refreshCDKScreen(pl->d->planLecheniya);
				return 1;
			}
		
		case 'i':
			{
				pl->stage_index = pl->t[selected]->stageIndex, 
				price_list_create(pl->d, pl, price_add_callback);
				screen_redraw_cases(pl->d);
				screen_redraw_planLecheniya(pl->d);
				plan_lecheniya_update(pl);
				pl->s->currentItem = selected;
				refreshCDKScreen(pl->d->planLecheniya);
				return 1;
			}

		default:
			break;
	}		

	return 1;
}

void
plan_lecheniya_create(
		delegate_t *d,
	  struct case_t *c
		)
{

	/* init window and screen */
	screen_init_planLecheniya(d, LINES/3, COLS/3, LINES/3, COLS/3, COLOR_BLACK_ON_CYAN);
	
	PlanLecheniya pl = {d, c, NULL, NULL, 0, NULL, -1};
	
	/* allocate titles */
	pl.titles = MALLOC(8 * MAX_TITLES, 
			error_callback(d->planLecheniya, "can't allocate memory"), return);
	pl.t = MALLOC(8 * MAX_TITLES, 
			error_callback(d->planLecheniya, "can't allocate memory"), return);
	int i;
	for (i = 0; i < MAX_TITLES; ++i) {
		pl.titles[i] = NULL;
		pl.t[i] = NULL;
	}
	
	char * choises[] = 
	{""};
	
	CDKSELECTION 
		*s =
					newCDKSelection(
							d->planLecheniya, 
							0, 
							0, 
							1,
							LINES/3,
							COLS/3,
							"</B>План Лечения:<!B>", 
							NULL, 
							0, 
							choises,
							1,
							A_REVERSE,
							FALSE, 
							FALSE
							);

	pl.s = s;
	wbkgd(s->win, COLOR_PAIR(COLOR_BLACK_ON_CYAN));
	wbkgd(s->shadowWin, COLOR_PAIR(COLOR_BLACK_ON_CYAN));

	plan_lecheniya_update(&pl);
	
	bindCDKObject(vSELECTION, s, 'q', input_escape_handler, NULL);
	bindCDKObject(vSELECTION, s, CTRL('q'), input_escape_handler, NULL);
	bindCDKObject(vSELECTION, s, KEY_MOUSE, input_mouse_handler, NULL);\
	setCDKMentryPreProcess (s, plan_lecheniya_handler, &pl);
	setCDKSelectionPostProcess (s, screen_update_postHandler, NULL);
	
	info_pannel_set_text(d, 
			"ESC - закрыть, a - доб. этап, i - доб., d - удалить, +/- изменить кол-во");
	
	/* activate */
	int ret = activateCDKSelection(s, NULL);
	//if (ret != -1)
		//prozubi_case_set_text(node->key, d->p, node->c, node->array[ret]);
	
	/* destroy widgets */
	destroyCDKSelection(s);

	/* free memory */
	for (i = 0; i < pl.count; ++i) {
		if (pl.titles[i])
			free(pl.titles[i]);
	}
	free(pl.titles);

	/* destroy screen */
	screen_destroy_planLecheniya(d);
}		
