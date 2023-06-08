/**
 * File              : CasesList.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 15.05.2023
 * Last Modified Date: 08.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "CasesList.h"

#include <cdk.h>
#include <cdk/cdkscreen.h>
#include <cdk/selection.h>
#include <curses.h>
#include <time.h>
#include "delegate.h"
#include "colors.h"
#include "CaseEdit.h"
#include "InfoPannel.h"
#include "error.h"
#include "input.h"
#include "TextUTF8Handler.h"

struct cases_list_t {
	delegate_t *d;
	CDKSELECTION *s;
	struct passport_t *patient;
	char **list;
	int count;
	struct case_list_node **nodes;
};

static void cases_list_update(struct cases_list_t  *t);

static void * 
cases_list_callback(
			void *user_data,
			void * parent,
			bool has_children,
			struct case_list_node *n
		)
{
	struct cases_list_t *t = user_data;

	char *str = MALLOC(BUFSIZ,
			error_callback(t->d->cases, "cant't allocate memory for string"),
			return NULL);

	if (parent == NULL) {
		sprintf(str, "%s                                      ", n->title ? n->title : ""); 
	} else {
		sprintf(str, "--- %s                                  ", n->title ? n->title : ""); 
	}

	t->list [t->count] = str;
	t->nodes[t->count] = n;
	
	t->count++;
	
	t->list = REALLOC(t->list, (t->count + 2) * 8,
			error_callback(t->d->cases, "cant't reallocate memory for s->list"),
			return NULL);
	
	t->nodes = REALLOC(t->nodes, (t->count + 2) * 8,
			error_callback(t->d->cases, "cant't reallocate memory for s->nodes"),
			return NULL);

	return str;
}

static int 
cases_list_get_list(void *userdata, struct case_t * c)
{
	
	struct cases_list_t *t = userdata;

		prozubi_cases_list_foreach(
			t->d->p,
			c, 
			userdata, 
			cases_list_callback);

	return 0;
}

static void 
cases_list_free(struct cases_list_t *t)
{
	int i;
	for (i = 0; i < t->count; ++i) {
		free(t->list[i]);
		prozubi_case_list_node_free_with_case(t->d->p,
				t->nodes[i]);	
	}
	if (t->list)
		free(t->list);
	t->list = NULL;

	if(t->nodes)
		free(t->nodes);
	t->nodes = NULL;
}

static void 
cases_list_remove_case_confirm(struct cases_list_t *t, struct case_list_node *node)
{
	delegate_t *d = t->d;
	
	/* *INDENT-EQLS* */
	char *buttons[] =
	{
	  " Cancel ",
		COLOR_STR(COLOR_RED_ON_WHITE, " Удалить "), 
	};
  
	char *message[] = {"Вы уверены?"};

	int selection;

	CDKDIALOG *m = newCDKDialog (
	d->cases	/* cdkscreen */,
	CENTER		/* xPos */,
	CENTER		/* yPos */,
	message	/* message */,
	1		/* Rows */,
	buttons	/* buttons */,
	2		/* buttonCount */,
	A_REVERSE		/* highlight */,
	TRUE		/* separator */,
	TRUE		/* Box */,
	TRUE		/* shadow */
			);

	wbkgd(m->win, COLOR_PAIR(COLOR_BLACK_ON_WHITE));
	bindCDKObject (vDIALOG, m, KEY_MOUSE, input_mouse_handler, NULL);\

	/* Activate the entry field. */
	activateCDKDialog(m, NULL);
	
	if (m->currentButton == 1)
		if (!prozubi_case_remove(t->d->p, node->c))
		{
				cases_list_update(t);
		}
	
	destroyCDKDialog(m);
	refreshCDKScreen(d->cases);
}

static void 
cases_list_remove_case_message_show(struct cases_list_t *t, struct case_list_node *node)
{
	delegate_t *d = t->d;
	/*get parent */

	/* *INDENT-EQLS* */
	char *buttons[] =
	{
	  " Cancel ",
		COLOR_STR(COLOR_RED_ON_WHITE, " Удалить "), 
	};

	struct tm *tm = localtime(&node->c->date);
	char date[11];
	strftime(date, 11, "%d.%m.%Y", tm);
  
	char **message = malloc(8*3); 
	message[0] = "удалить данные приёма";
	char title[256];
	sprintf(title, "%s %s?", 
			date,
			node->c->name);
	message[1] = title;

	int selection;

	CDKDIALOG *m = newCDKDialog (
	d->cases	/* cdkscreen */,
	CENTER		/* xPos */,
	CENTER		/* yPos */,
	message	/* message */,
	2		/* Rows */,
	buttons	/* buttons */,
	2		/* buttonCount */,
	A_REVERSE		/* highlight */,
	TRUE		/* separator */,
	TRUE		/* Box */,
	TRUE		/* shadow */
			);

	free(message);

	wbkgd(m->win, COLOR_PAIR(COLOR_BLACK_ON_WHITE));
	bindCDKObject (vDIALOG, m, KEY_MOUSE, input_mouse_handler, NULL);\

	/* Activate the entry field. */
	activateCDKDialog(m, NULL);
	int ret = m->currentButton; 
	
	destroyCDKDialog(m);
	refreshCDKScreen(d->cases);
	
	if (ret == 1)
		cases_list_remove_case_confirm(t, node);
}

static int 
cases_list_preHandler (EObjectType cdktype GCC_UNUSED, void *object,
		       void *clientData GCC_UNUSED,
		       chtype input GCC_UNUSED)
{
	CDKSELECTION *sel = object;
	struct cases_list_t *t = clientData;
	
	info_pannel_set_text(t->d, 
			"CTRL-q - закрыть, ENTER - редактировать");

	switch (input) {
		case KEY_ENTER:
			{ 
		    int selected = getCDKSelectionCurrent(sel);
				struct case_list_node *node = t->nodes[selected];
				switch (node->type) {
					case CASES_LIST_TYPE_TEXT: 
						{
							case_edit_text_create(node, t->d);
							break;
						}
					case CASES_LIST_TYPE_COMBOBOX: 
						{
							case_edit_combobox_create(node, t->d);
							break;
						}
					case CASES_LIST_TYPE_DATE: 
						{
							case_edit_date_create(node, t->d);
							break;
						}
					case CASES_LIST_TYPE_ZFORMULA: 
						{
							case_edit_zformula_create(node, t->d);
							break;
						}
					case CASES_LIST_TYPE_XRAY: 
						{
							case_edit_xray_create(node, t->d);
							break;
						}
					case CASES_LIST_TYPE_PLANLECHENIYA: 
						{
							case_edit_plan_lecheniya_create(node, t->d);
							break;
						}

					default:
						break;
				}
				break;
			}
		case 'a':
			{
				// add new case
				prozubi_case_new_for_patient(t->d->p, t->patient->id);
				cases_list_update(t);
				return 0;
			}
		case 'r':
			{
				// refresh list
				cases_list_update(t);
				return 0;
			}
		case 'd':
			{
		    int selected = getCDKSelectionCurrent(sel);
				cases_list_remove_case_message_show(t, t->nodes[selected]);
				return 0;
			}
		case 'q': case CTRL('q'):
			{
				exitOKCDKScreen(t->d->cases);
				cases_list_free(t);
				break;
			}
			
		default:
			break;
	}

	return 1;
}

void 
cases_list_update(struct cases_list_t *t)
{
	cases_list_free(t);
	
	t->list = MALLOC(8, 
			error_callback(t->d->cases, "cant't allocate memory for pointer"),
			return);
	t->nodes = MALLOC(8,
			error_callback(t->d->cases, "cant't allocate memory for pointer"),
			return);
	
	prozubi_cases_foreach(
			t->d->p, 
			t->patient->id, 
			t, 
			cases_list_get_list
			);
	
	setCDKSelectionItems(t->s, t->list, t->count);
	refreshCDKScreen(t->d->cases);
}

void
cases_list_create(delegate_t *d, struct passport_t *patient)
{
	/* init window and screen */
	screen_init_cases(d, LINES/2, COLS/2, LINES/4, COLS/4, COLOR_BLACK_ON_WHITE);
	
	char * choises[] = 
	{""};
	
	CDKSELECTION 
		*s =
					newCDKSelection(
							d->cases, 
							0, 
							0, 
							0,
							LINES/2,
							COLS/2,
							"</B>Посещения:<!B>", 
							NULL, 
							0, 
							choises,
							1,
							A_REVERSE,
							FALSE, 
							FALSE
							);

	wbkgd(s->shadowWin, COLOR_PAIR(COLOR_BLACK_ON_WHITE));
	setCDKSelectionBackgroundColor(s, "</57>");
	bindCDKObject (vSELECTION, s, KEY_MOUSE, input_mouse_handler, NULL);\

	struct cases_list_t t;
	t.d = d;
	t.count = 0;
	t.list = NULL;
	t.nodes = NULL;
	t.patient = patient;
	t.s = s;
	
	cases_list_update(&t);
	
	setCDKSelectionPreProcess (s, cases_list_preHandler, &t);
	setCDKSelectionPostProcess (s, screen_update_postHandler, &t);
	
	info_pannel_set_text(d, 
			"CTRL-q - закрыть, ENTER - редактировать");
	
	/* start traverse */
	traverseCDKScreen(d->cases);

	/* destroy widgets */
	destroyCDKSelection(s);

	screen_destroy_cases(d);
	
}
