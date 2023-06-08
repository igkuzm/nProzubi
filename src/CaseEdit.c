/**
 * File              : CaseEdit.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 15.05.2023
 * Last Modified Date: 08.06.2023
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
#include "InfoPannel.h"
#include "TextUTF8Handler.h"
#include "input.h"

void
case_edit_text_create(
		struct case_list_node *node,
		delegate_t *d)
{	
	/* init window and screen */
	screen_init_textEdit(d, LINES/3, COLS/3, LINES/3, COLS/3, COLOR_BLACK_ON_CYAN);

	char title[BUFSIZ];
	sprintf(title, "</B>%s:<!B>", node->title);
	CDKMENTRY *m =
		newCDKMentry (
		d->textEdit	/* cdkscreen */,
		0		/* xpos */,
	  0		/* ypos */,
	 	title	/* title */,
		"" /* label */,
		A_NORMAL|COLOR_PAIR(COLOR_BLACK_ON_CYAN)		/* fieldAttr */,
		//A_NORMAL		/* fieldAttr */,
		' '		/* filler */,
		vMIXED	/* disptype */,
		COLS/3	/* fieldWidth */,
		LINES/3		/* fieldrows */,
		LINES*2		/* logicalRows */,
		0		/* min */,
		FALSE		/* Box */,
		FALSE		/* shadow */
		);
	
	if (!m){
		error_callback(d->textEdit, "can't draw CDKMENTRY - the screen is too small");
		return;
	}
	
	setCDKMentryBackgroundColor(m, COLOR_N(COLOR_BLACK_ON_CYAN));
	bindCDKObject (vMENTRY, m, KEY_MOUSE, input_mouse_handler, NULL);\
	
	char *value = prozubi_case_get(node->c, node->key); 
	if (node->key == CASEDIAGNOZIS)
		if (!value || strlen(value) <  2)
			value = prozubi_diagnosis_get(d->p, node->c);
	if (!value)
		value = "";
	setCDKMentryValue(m, value);
	m->currentCol = 0;
	m->currentRow = 0;
	size_t text_position = 0;
	setCDKMentryPreProcess (m, mentry_text_preHandler, &text_position);

	info_pannel_set_text(d, 
			"ESC - отмена, ENTER - сохранить");
	/* activate */
	//traverseCDKScreen(screen);

	char * ret = activateCDKMentry(m, NULL);
	if (ret)
		prozubi_case_set_text(node->key, d->p, node->c, ret);
	
	/* destroy widgets */
	destroyCDKMentry(m);
	screen_destroy_textEdit(d);

	/* redraw CDK */
}

void
case_edit_combobox_create(
		struct case_list_node *node,
		delegate_t *d)
{
	char *value = prozubi_case_get(node->c, node->key); 
	if (!value)
		value = "";
	
	int selected = 0;
	int i = 0;
	while (node->array[i]){
		char *item = node->array[i++];
		if (strcmp(item, value) == 0)
			selected = i -1;
	}

	/* init window and screen */
	int w = 40, h = i + 2;
	int x = COLS/3 + COLS/6 - w/2, y = LINES/3 + LINES/6 - h/2;
	screen_init_combobox(d, h, w , y, x, COLOR_BLACK_ON_CYAN);
	CDKRADIO *m = newCDKRadio (
		d->combobox	/* cdkscreen */,
		0		/* xpos */,
		0		/* ypos */,
		0		/* spos */,
		i+2		/* height */,
		COLS/3		/* width */,
		node->title	/* title */,
		node->array	/* mesg */,
		i		/* items */,
		'*'		/* choiceChar */,
		selected		/* defItem */,
		A_REVERSE		/* highlight */,
		FALSE		/* Box */,
		FALSE		/* shadow */
		);

	if (!m){
		error_callback(d->cases, "can't draw CDKITEMLIST - the screen os too small");
		return;
	}

	setCDKItemlistBackgroundColor(m, COLOR_N(COLOR_BLACK_ON_CYAN));
	bindCDKObject (vRADIO, m, KEY_MOUSE, input_mouse_handler, NULL);\
	
	info_pannel_set_text(d, 
			"ESC - отмена, ENTER - сохранить");

	/* activate */
	activateCDKRadio(m, NULL);
	int ret = m->selectedItem;
	if (ret != -1)
		prozubi_case_set_text(node->key, d->p, node->c, node->array[ret]);
	
	/* destroy widgets */
	destroyCDKRadio(m);

	screen_destroy_combobox(d);
}

void
case_edit_date_create(
		struct case_list_node *node,
		delegate_t *d)
{

	/* init window and screen */
	int w = 24, h = 12;
	int x = COLS/3 + COLS/6 - w/2, y = LINES/3 + LINES/6 - h/2;
	screen_init_date(d, h, w, y, x, COLOR_BLACK_ON_CYAN);
	
	time_t *value = prozubi_case_get(node->c, node->key); 
	struct tm *tp = localtime(value);
	
	CDKCALENDAR *m = newCDKCalendar (
	d->date	/* screen */,
	0		/* xPos */,
	0		/* yPos */,
	node->title	/* title */,
	tp->tm_mday		/* day */,
	tp->tm_mon + 1		/* month */,
	tp->tm_year + 1900		/* year */,
	A_NORMAL    /* dayAttrib */,
	A_NORMAL    /* monthAttrib */,
	A_NORMAL    /* yearAttrib */,
	A_REVERSE /* highlight */,
	FALSE		/* Box */,
	FALSE		/* shadow */
	);

	if (!m){
		error_callback(d->cases, "can't draw CDKCALENDAR - the screen is too small");
		return;
	}
	wbkgd(m->win, COLOR_PAIR(COLOR_BLACK_ON_CYAN));
	wbkgd(m->fieldWin, COLOR_PAIR(COLOR_BLACK_ON_CYAN));
	wbkgd(m->labelWin, COLOR_PAIR(COLOR_BLACK_ON_CYAN));

	bindCDKObject (vCALENDAR, m, KEY_MOUSE, input_mouse_handler, NULL);\
	
	info_pannel_set_text(d, 
			"ESC - отмена, ENTER - сохранить");
	/* activate */
	time_t ret = activateCDKCalendar(m, NULL);
	if (ret != -1){
		//error_callback(d->date, STR("timezone: %ld", timezone));
		ret -= timezone; 
		prozubi_case_set_date(node->key, d->p, node->c, ret);
	}
	
	/* destroy widgets */
	destroyCDKCalendar(m);
	screen_destroy_date(d);
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

	screen_init_zformula(d, h, w, y, x, COLOR_BLACK_ON_CYAN);

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
	d->zformula	/* cdkscreen */,
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
		error_callback(d->cases, "can't draw CDKMATRIX - the screen os too small");
		return;
	}

	wbkgd(m->win, COLOR_PAIR(COLOR_BLACK_ON_CYAN));
	setCDKMatrixBackgroundColor(m, COLOR_N(COLOR_BLACK_ON_CYAN));
	
	bindCDKObject (vMATRIX, m, KEY_MOUSE, input_mouse_handler, NULL);\

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

	info_pannel_set_text(d, 
			"ESC - отмена, ENTER - сохранить, TAB, p, c, r, n, 0, l, k, t, i - заполнение формулы");
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

	screen_destroy_zformula(d);

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
	info_pannel_set_text(d, 
			"ESC - отмена, ENTER - открыть, а - добавить, d - удалить");
	xray_create(node, d);
}
