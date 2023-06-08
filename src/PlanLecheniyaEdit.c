/**
 * File              : PlanLecheniyaEdit.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 02.06.2023
 * Last Modified Date: 06.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#include "PlanLecheniyaEdit.h"
#include "colors.h"
#include "input.h"
#include "InfoPannel.h"
#include "TextUTF8Handler.h"
#include "prozubilib/planlecheniya.h"

#define PLAN_LECHENIYA_EDIT_TYPES\
	PLAN_LECHENIYA_EDIT_TYPE_TEXT(title, "Наименование:")\
	PLAN_LECHENIYA_EDIT_TYPE_NUMB(price, "Цена:")\
	PLAN_LECHENIYA_EDIT_TYPE_NUMB(count, "Количество:")\
	PLAN_LECHENIYA_EDIT_TYPE_TEXT(kod,   "Код:")

struct plan_lecheniya_edit_t {
	delegate_t *d;
	struct case_t *c;
	int stageIndex;
	int itemIndex;
	CDKSCREEN *screen;
#define PLAN_LECHENIYA_EDIT_TYPE_TEXT(name, title) CDKMENTRY   * m_##name;
#define PLAN_LECHENIYA_EDIT_TYPE_NUMB(name, title) CDKTEMPLATE * m_##name;
	PLAN_LECHENIYA_EDIT_TYPES
#undef PLAN_LECHENIYA_EDIT_TYPE_NUMB 
#undef PLAN_LECHENIYA_EDIT_TYPE_TEXT 
};

static int 
_plan_lecheniya_edit_q_handler(
				EObjectType cdktype GCC_UNUSED, 
				void *object, 
				void *clientData GCC_UNUSED, 
				chtype key GCC_UNUSED
				)
{
	delegate_t *d = clientData;
	exitOKCDKScreen(d->planLecheniyaEdit);

	return 1;
}

static void 
_plan_lecheniya_edit_ok_callback(CDKBUTTON *button){
	struct plan_lecheniya_edit_t *p = button->callbackData;
	/* update patient */
#define PLAN_LECHENIYA_EDIT_TYPE_TEXT(name, title)\
	{\
		char *value = getCDKMentryValue(p->m_##name);\
		prozubi_planlecheniya_set_item_##name(p->d->p, p->c->planlecheniya,\
				p->stageIndex, p->itemIndex, value);\
	}

#define PLAN_LECHENIYA_EDIT_TYPE_NUMB(name, title)\
	{\
		char *value = getCDKTemplateValue(p->m_##name);\
		int i = atoi(value);\
		prozubi_planlecheniya_set_item_##name(p->d->p, p->c->planlecheniya,\
				p->stageIndex, p->itemIndex, i);\
	}

	PLAN_LECHENIYA_EDIT_TYPES
#undef PLAN_LECHENIYA_EDIT_TYPE_NUMB 
#undef PLAN_LECHENIYA_EDIT_TYPE_TEXT 
	
	exitOKCDKScreen(p->screen);
}


static void 
_plan_lecheniya_edit_cancel_callback(CDKBUTTON *button){
	CDKSCREEN *screen = button->callbackData;
	exitOKCDKScreen(screen);
}

void 
plan_lecheniya_edit_create(
		delegate_t *d, 
		struct case_t *c,
		int stageIndex,
		int itemIndex,
		const char *title,
		const char *price,
		const char *count,
		const char *kod
		)
{	

	struct plan_lecheniya_edit_t p;
	p.d = d;
	p.c = c;
	p.stageIndex = stageIndex;
	p.itemIndex = itemIndex;
	
	int h = 18, w = 34;
	int x = COLS/3 + COLS/6 - w/2, y = LINES/3 + LINES/6 - h/2;
	
	screen_init_planLecheniyaEdit(d, h, w, y, x, COLOR_BLACK_ON_WHITE);
	CDKSCREEN *cdkscreen = d->planLecheniyaEdit;
	p.screen = cdkscreen;
	
	info_pannel_set_text(d, 
			"CTRL-q - закрыть, TAB - далее");
	
	/* init widgets */

#define PLAN_LECHENIYA_EDIT_TYPE_TEXT(name, title)\
	CDKMENTRY *m_##name = newCDKMentry (\
		cdkscreen,\
		x,\
		y,\
		"</B>" title "<!B>",\
		"",\
		A_NORMAL|COLOR_PAIR(COLOR_BLACK_ON_WHITE),\
		' ',\
		vMIXED,\
		40,\
		2,\
		1024,\
		0,\
		TRUE,\
		FALSE);\
	p.m_##name = m_##name;\
  y += 5;\
	setCDKMentryBackgroundColor(m_##name, "</57>");\
	bindCDKObject (vMENTRY, m_##name, CTRL('q'), _plan_lecheniya_edit_q_handler, d);\
	if (name)\
		setCDKMentryValue(m_##name, name);\
	size_t text_position_##name = 0;\
	bindCDKObject (vMENTRY, m_##name, KEY_MOUSE, input_mouse_handler, cdkscreen);\
	setCDKMentryPreProcess (m_##name, mentry_text_preHandler, &text_position_##name);\
	m_##name->currentCol = 0;
	
	const char *overlay = "";
	const char *plate = ""; 	

#define PLAN_LECHENIYA_EDIT_TYPE_NUMB(name, title)\
	overlay = COLOR_STR(COLOR_BLACK_ON_WHITE, "___________");\
	plate = "############";\
	CDKTEMPLATE *m_##name = newCDKTemplate(\
			cdkscreen,\
			x,\
			y,\
			"</B>" title "<!B>",\
			"",\
			plate,\
			overlay,\
			FALSE,\
			FALSE);\
	p.m_##name = m_##name;\
	if (name)\
		setCDKTemplateValue(m_##name, name);\
	y += 3;\
	setCDKTemplateBackgroundColor(m_##name, "</57>");\
	bindCDKObject (vTEMPLATE, m_##name, KEY_MOUSE, input_mouse_handler, cdkscreen);\
	bindCDKObject (vTEMPLATE, m_##name, CTRL('q'), _plan_lecheniya_edit_q_handler, d);\
	

PLAN_LECHENIYA_EDIT_TYPES
#undef PLAN_LECHENIYA_EDIT_TYPE_TEXT 
#undef PLAN_LECHENIYA_EDIT_TYPE_NUMB 

	CDKBUTTON *ok = newCDKButton(
			cdkscreen, 
			x, 
			y, 
			"Сохран", 
			NULL, 
			TRUE, 
			FALSE);
	setCDKButtonBackgroundColor(ok, "</57>");
	ok->callback = _plan_lecheniya_edit_ok_callback;
	ok->callbackData = &p;
	bindCDKObject (vBUTTON, ok, CTRL('q'), _plan_lecheniya_edit_q_handler, d);\
	bindCDKObject (vBUTTON, ok, KEY_MOUSE, input_mouse_handler, cdkscreen);\

CDKBUTTON *cancel = newCDKButton(
			cdkscreen, 
			x + 22, 
			y, 
			"Отмена", 
			NULL, 
			TRUE, 
			FALSE);
	setCDKButtonBackgroundColor(cancel, "</57>");
	cancel->callback = _plan_lecheniya_edit_cancel_callback;
	cancel->callbackData = cdkscreen;
	bindCDKObject (vBUTTON, cancel, CTRL('q'), _plan_lecheniya_edit_q_handler, d);\
	bindCDKObject (vBUTTON, cancel, KEY_MOUSE, input_mouse_handler, cdkscreen);\


	/* start traverse */
	refreshCDKScreen(cdkscreen);
	traverseCDKScreen(cdkscreen);

	/* destroy widgets */
	screen_destroy_planLecheniyaEdit(d);
}
