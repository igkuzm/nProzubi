/**
 * File              : CaseEdit.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 15.05.2023
 * Last Modified Date: 01.07.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "CaseEdit.h"

#include <cdk.h>
#include <cdk/curdefs.h>
#include <cdk/matrix.h>
#include <cdk/mentry.h>
#include <curses.h>
#include <stdio.h>
#include <time.h>
#include "delegate.h"
#include "colors.h"
#include "error.h"
#include "Xray.h"
#include "PlanLecheniya.h"
#include "TextUTF8Handler.h"
#include "ncwidgets/src/nclist.h"
#include "ncwidgets/src/keys.h"
#include "ncwidgets/src/utils.h"
#include "ncwidgets/src/ncentry.h"
#include "ncwidgets/src/ncselection.h"
#include "ncwidgets/src/nccalendar.h"
#include "input.h"

typedef struct {
	delegate_t *d;
	struct case_list_node *node;
	void *data;
} case_edit_t;

CBRET case_edit_text_callback(void *userdata, enum SCREEN type, void *object, chtype key)
{
	ncentry_t *s = object;
	case_edit_t *t = userdata;
	switch (key) {
		case CTRL('s'):
			{
				//save
				char *text = ucharstr2str(s->info);
				if (text){
					prozubi_case_set_text(t->node->key, t->d->p, t->node->c, text);
					free(text);
				}
				return CBBREAK;
			}
		
		case KEY_MOUSE:
			{
				MEVENT event;
				if (getmouse(&event) == OK) {
					if (!wenclose(s->ncwin->overlay, event.y, event.x)){
						//save
						char *text = ucharstr2str(s->info);
						if (text){
							prozubi_case_set_text(t->node->key, t->d->p, t->node->c, text);
							free(text);
						}
						return CBBREAK;
					}	
					
					ungetmouse(&event);
				}
				break;
			}

		case KEY_ESC:
			return CBBREAK;
	
		default:
			break;
	}

	return 0;
}

void
case_edit_text_create(
		struct case_list_node *node,
		delegate_t *d)
{	
	char title[BUFSIZ];
	sprintf(title, "</B>%s:<!B>", node->title);
		
	char *value = prozubi_case_get(node->c, node->key); 
	if (node->key == CASEDIAGNOZIS)
		if (!value || strlen(value) <  2)
			value = prozubi_diagnosis_get(d->p, node->c);
	if (!value)
		value = "";

	ncentry_t *s = 
			nc_entry_new(
					NULL, 
					title, 
					LINES/2, COLS/2, LINES/4, COLS/4, 
					COLOR_BLACK_ON_CYAN, 
					value, 
					TRUE, 
					TRUE, 
					TRUE
					);

	case_edit_t t = {d, node, NULL};
	nc_entry_activate(s, &t, case_edit_text_callback);
	nc_entry_destroy(s);
}

CBRET case_edit_combobox_callback(void *userdata, enum SCREEN type, void *object, chtype key)
{
	ncselection_t *s = object;
	case_edit_t *t = userdata;
	switch (key) {
		case CTRL('s'):
			{
				//save
				int i;
				for (i = 0; i < s->size; ++i) {
					if (s->selected[i] == 1)
						prozubi_case_set_text(t->node->key, t->d->p, t->node->c, t->node->array[i]);
				}
		
				return CBBREAK;
			}
		
		case KEY_MOUSE:
			{
				MEVENT event;
				if (getmouse(&event) == OK) {
					if (!wenclose(s->nclist->ncwin->overlay, event.y, event.x)){
						//save
						int index = 0;
						int i;
						for (i = 0; i < s->size; ++i) {
							if (s->selected[i] == 1)
								index = i;
						}
						prozubi_case_set_text(t->node->key, t->d->p, t->node->c, t->node->array[index]);
						return CBBREAK;
					}	
					
					ungetmouse(&event);
				}
				break;
			}

		case KEY_ESC:
			return CBBREAK;
	
		default:
			break;
	}

	return 0;
}

void
case_edit_combobox_create(
		struct case_list_node *node,
		delegate_t *d)
{
	char *value = prozubi_case_get(node->c, node->key); 
	if (!value)
		value = "";

	int selected[10] = {0};
	
	int i = 0;
	while (node->array[i]){
		char *item = node->array[i];
		if (strcmp(item, value) == 0)
			selected[i] = 1;
		else
			selected[i] = 0;
		i++;
	}

	int w = 40, h = i + 2;
	int x = COLS/3 + COLS/6 - w/2, y = LINES/3 + LINES/6 - h/2;
	
	char *selections[] = {"[ ] ", "[*] "};
	
	ncselection_t *s = 
			nc_selection_new(
					NULL, 
					node->title, 
					h, w, y, x, 
					COLOR_BLACK_ON_CYAN, 
					selections,
					2, 
					selected, 
					FALSE,
					node->array, 
					i, 
					TRUE, 
					TRUE
					);

	case_edit_t t = {d, node, s};
	nc_selection_activate(s, &t, case_edit_combobox_callback);
	nc_selection_destroy(s);
}

CBRET case_edit_date_callback(void *userdata, enum SCREEN type, void *object, chtype key)
{
	nccalendar_t *s = object;
	case_edit_t *t = userdata;
	switch (key) {
		case KEY_ENTER: case KEY_RETURN:
			{
				//save
				time_t value = mktime(s->tm);
				if (value != -1)
					prozubi_case_set_date(t->node->key, t->d->p, t->node->c, value);
				return CBBREAK;
			}
		
		case KEY_MOUSE:
			{
				MEVENT event;
				if (getmouse(&event) == OK) {
					if (!wenclose(s->ncwin->overlay, event.y, event.x))
						return CBBREAK;
					ungetmouse(&event);
				}
				break;
			}

		case KEY_ESC:
			return CBBREAK;
	
		default:
			break;
	}

	return 0;
}


void
case_edit_date_create(
		struct case_list_node *node,
		delegate_t *d)
{

	time_t *value = prozubi_case_get(node->c, node->key); 
	
	/* init window and screen */
	int w = 24, h = 12;
	int x = COLS/3 + COLS/6 - w/2, y = LINES/3 + LINES/6 - h/2;

	nccalendar_t *m = 
			nc_calendar_new(
					NULL, 
					node->title, 
					y, x, 
					COLOR_BLACK_ON_CYAN, 
					*value, 
					1, 
					TRUE, 
					TRUE);

	case_edit_t t = {d, node, NULL};
	nc_calendar_activate(m, &t, case_edit_date_callback);
	nc_calendar_destroy(m);

	//screen_init_screen_date(d, h, w, y, x, COLOR_BLACK_ON_CYAN);
	
	//struct tm *tp = localtime(value);
	
	//CDKCALENDAR *m = newCDKCalendar (
	//d->screen_date	[> screen <],
	//0		[> xPos <],
	//0		[> yPos <],
	//node->title	[> title <],
	//tp->tm_mday		[> day <],
	//tp->tm_mon + 1		[> month <],
	//tp->tm_year + 1900		[> year <],
	//A_NORMAL    [> dayAttrib <],
	//A_NORMAL    [> monthAttrib <],
	//A_NORMAL    [> yearAttrib <],
	//A_REVERSE [> highlight <],
	//FALSE		[> Box <],
	//FALSE		[> shadow <]
	//);

	//if (!m){
		//error_callback(d->screen_cases, "can't draw CDKCALENDAR - the screen is too small");
		//return;
	//}
	//wbkgd(m->win, COLOR_PAIR(COLOR_BLACK_ON_CYAN));
	//wbkgd(m->fieldWin, COLOR_PAIR(COLOR_BLACK_ON_CYAN));
	//wbkgd(m->labelWin, COLOR_PAIR(COLOR_BLACK_ON_CYAN));

	//bindCDKObject (vCALENDAR, m, KEY_MOUSE, input_mouse_handler, d);\
	
	//[>info_pannel_set_text(d, <]
			//[>"ESC - отмена, ENTER - сохранить");<]
	//[> activate <]
	//time_t ret = activateCDKCalendar(m, NULL);
	//if (ret != -1){
		////error_callback(d->date, STR("timezone: %ld", timezone));
		//ret -= timezone; 
		//prozubi_case_set_date(node->key, d->p, node->c, ret);
	//}
	
	//[> destroy widgets <]
	//destroyCDKCalendar(m);
	//screen_destroy_screen_date(d);
}

static int 
zformula_preHandler (EObjectType cdktype GCC_UNUSED, void *object,
		       void *clientData GCC_UNUSED,
		       chtype input GCC_UNUSED)
{
	CDKMATRIX *m = object;
	int col = getCDKMatrixCol(m);
	int row = getCDKMatrixRow(m);
	char *value = getCDKMatrixCell(m, row, col);

	switch (input) {
		case KEY_ENTER:
				return 1;
		case KEY_ESC:
				return 1;
		case KEY_LEFT:
				return 1;
		case KEY_RIGHT:
				return 1;
		case KEY_UP:
				return 1;
		case KEY_DOWN:
				return 1;
		case KEY_BACKSPACE:
			return 1;
		case 'p':
			{
				setCDKMatrixCell(m, row, col, "P");
				eraseCDKMatrix(m);
				drawCDKMatrix(m, TRUE);

				return 0;
			}
		case 't':
			{
				setCDKMatrixCell(m, row, col, "Pt");
				eraseCDKMatrix(m);
				drawCDKMatrix(m, TRUE);

				return 0;
			}
		case 'c':
			{
				setCDKMatrixCell(m, row, col, "C");
				eraseCDKMatrix(m);
				drawCDKMatrix(m, TRUE);

				return 0;
			}
		case 'k':
			{
				setCDKMatrixCell(m, row, col, "К");
				eraseCDKMatrix(m);
				drawCDKMatrix(m, TRUE);

				return 0;
			}
		case '0':
			{
				setCDKMatrixCell(m, row, col, "0");
				eraseCDKMatrix(m);
				drawCDKMatrix(m, TRUE);

				return 0;
			}
		case 'n':
			{
				setCDKMatrixCell(m, row, col, "");
				eraseCDKMatrix(m);
				drawCDKMatrix(m, TRUE);

				return 0;
			}
		case 'r':
			{
				setCDKMatrixCell(m, row, col, "R");
				eraseCDKMatrix(m);
				drawCDKMatrix(m, TRUE);

				return 0;
			}
		case 'i':
			{
				setCDKMatrixCell(m, row, col, "И");
				eraseCDKMatrix(m);
				drawCDKMatrix(m, TRUE);

				return 0;
			}
		case 'l':
			{
				setCDKMatrixCell(m, row, col, "П");
				eraseCDKMatrix(m);
				drawCDKMatrix(m, TRUE);

				return 0;
			}
		case ' ': case KEY_TAB:
			{
				char **ptr = clientData;
				char *newValue;
				int i = 0;
				while (ptr[i]){
					newValue = ptr[i++];
					if (strcmp(value, newValue) == 0)
						break;
				}
				newValue = ptr[i++];
				if (newValue == NULL)
					newValue = ptr[0];

				setCDKMatrixCell(m, row, col, newValue);
				eraseCDKMatrix(m);
				drawCDKMatrix(m, TRUE);

				return 0;
			}
		case 'q':
			injectCDKMatrix(m, KEY_ESC);	
			break;

		default:
			break;
	}

	return 0;
}

void
case_edit_zformula_create(
		struct case_list_node *node,
		delegate_t *d)
{

#define TEETH\
	TOOTH(Z11, 1, 8)\
	TOOTH(Z12, 1, 7)\
	TOOTH(Z13, 1, 6)\
	TOOTH(Z14, 1, 5)\
	TOOTH(Z15, 1, 4)\
	TOOTH(Z16, 1, 3)\
	TOOTH(Z17, 1, 2)\
	TOOTH(Z18, 1, 1)\
	TOOTH(Z21, 1, 9)\
	TOOTH(Z22, 1, 10)\
	TOOTH(Z23, 1, 11)\
	TOOTH(Z24, 1, 12)\
	TOOTH(Z25, 1, 13)\
	TOOTH(Z26, 1, 14)\
	TOOTH(Z27, 1, 15)\
	TOOTH(Z28, 1, 16)\
	TOOTH(Z31, 2, 16)\
	TOOTH(Z32, 2, 15)\
	TOOTH(Z33, 2, 14)\
	TOOTH(Z34, 2, 13)\
	TOOTH(Z35, 2, 12)\
	TOOTH(Z36, 2, 11)\
	TOOTH(Z37, 2, 10)\
	TOOTH(Z38, 2, 9)\
	TOOTH(Z41, 2, 8)\
	TOOTH(Z42, 2, 7)\
	TOOTH(Z43, 2, 6)\
	TOOTH(Z44, 2, 5)\
	TOOTH(Z45, 2, 6)\
	TOOTH(Z46, 2, 3)\
	TOOTH(Z47, 2, 2)\
	TOOTH(Z48, 2, 1)

	/* init window and screen */

	int w = 100, h = 10;
	int x = COLS/3 + COLS/6 - w/2, y = LINES/3 + LINES/6 - h/2;

	screen_init_screen_zformula(d, h, w, y, x, COLOR_BLACK_ON_CYAN);

	int colwidth[16] = {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3};
	int coltypes[16] = {
		vMIXED, vMIXED, vMIXED, vMIXED, vMIXED, vMIXED, vMIXED, vMIXED, 
		vMIXED, vMIXED, vMIXED, vMIXED, vMIXED, vMIXED, vMIXED, vMIXED 
	};
	char *coltitles[] = 
	{
		NULL,
		"8", "7", "6", "5", "4", "3", "2", "1",
		"1", "2", "3", "4", "5", "6", "7", "8"
	};

	char *rowtitles[] = 
	{
		NULL,
		//"верхняя челюсть",
		//"нижняя челюсть",
		" upper  ",
		" lower  "
	};


	CDKMATRIX *m = newCDKMatrix (
	d->screen_zformula	/* cdkscreen */,
	0		/* xpos */,
	0		/* ypos */,
	2		/* rows */,
	16		/* cols */,
	2		/* vrows */,
	16		/* vcols */,
	node->title	/* title */,
	rowtitles	/* rowtitles */,
	coltitles	/* coltitles */,
	colwidth /* colwidths */,
	coltypes /* coltypes */,
	0		/* rowspace */,
	0		/* colspace */,
	' '		/* filler */,
	ROW		/* dominantAttrib */,
	FALSE		/* boxMatrix */,
	TRUE		/* boxCell */,
	FALSE		/* shadow */
			);

	if (!m){
		error_callback(d->screen_cases, "can't draw CDKMATRIX - the screen is too small");
		return;
	}

	wbkgd(m->win, COLOR_PAIR(COLOR_BLACK_ON_CYAN));
	setCDKMatrixBackgroundColor(m, COLOR_N(COLOR_BLACK_ON_CYAN));
	
	bindCDKObject (vMATRIX, m, KEY_MOUSE, input_mouse_handler, d);\

#define TOOTH(n, row, col)\
	{\
		char *value = prozubi_case_get_##n(node->c);\
		if (!value)\
			value = "";\
		setCDKMatrixCell(m, row, col, value);\
	}
	TEETH
#undef TOOTH
  
		char *values[] = {
			"P", "Pt", "0", "R", "К", "П", "C", "И", "", NULL 
		};

		setCDKMatrixPreProcess (m, zformula_preHandler, values);

	/*info_pannel_set_text(d, */
			/*"ESC - отмена, ENTER - сохранить, TAB, p, c, r, n, 0, l, k, t, i - заполнение формулы");*/
	/* activate */
	int ret = activateCDKMatrix(m, NULL);
	if (ret == 1){
#define TOOTH(n, row, col)\
	{\
		char *value = getCDKMatrixCell(m, row, col);\
		if (value)\
			prozubi_case_set_##n(d->p, node->c, value);\
	}
	TEETH
#undef TOOTH
 
	}
	
	/* destroy widgets */
	destroyCDKMatrix(m);

	screen_destroy_screen_zformula(d);

}

void
case_edit_plan_lecheniya_create(
		struct case_list_node *node,
		delegate_t *d)
{
	plan_lecheniya_create(d, node->c);
}

void
case_edit_xray_create(
		struct case_list_node *node,
		delegate_t *d)
{
	/*info_pannel_set_text(d, */
			/*"ESC - отмена, ENTER - открыть, а - добавить, d - удалить");*/
	xray_create(node, d);
}
