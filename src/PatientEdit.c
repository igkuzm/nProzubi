/**
 * File              : PatientEdit.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 22.05.2023
 * Last Modified Date: 08.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "PatientEdit.h"

#include "delegate.h"
#include "error.h"
#include "prozubilib/prozubilib.h"
#include "colors.h"
#include <cdk/button.h>
#include <cdk/curdefs.h>
#include <cdk/entry.h>
#include <cdk/mentry.h>
#include <cdk/template.h>
#include <time.h>
#include "InfoPannel.h"
#include "input.h"
#include "TextUTF8Handler.h"

static int 
_patient_edit_q_handler(
				EObjectType cdktype GCC_UNUSED, 
				void *object, 
				void *clientData GCC_UNUSED, 
				chtype key GCC_UNUSED
				)
{
	delegate_t *d = clientData;
	exitOKCDKScreen(d->patientEdit);
	return 1;
}

#define PATIENT_EDIT_TYPES\
	PATIENT_EDIT_TYPE_ENTRY(familiya,    "Фамилия:",           PASSPORTFAMILIYA)\
	PATIENT_EDIT_TYPE_ENTRY(imia,        "Имя:",               PASSPOTIMIA)\
	PATIENT_EDIT_TYPE_ENTRY(otchestvo,   "Отчество:",          PASSPORTOTCHESTVO)\
	PATIENT_EDIT_TYPE_EDATE(dateofbirth, "Дата рождения:",     PASSPORTDATEOFBIRTH)\
	PATIENT_EDIT_TYPE_ETELE(tel,         "Телефон:",           PASSPORTTEL)\
	PATIENT_EDIT_TYPE_ENTRY(email,       "Email:",             PASSPORTEMAIL)\
	PATIENT_EDIT_TYPE_TEXTV(address,     "Адрес проживания:",  PASSPORTADDRESS)\
	PATIENT_EDIT_TYPE_TEXTV(document,    "Паспортные данные:", PASSPORTDOCUMENT)\
	PATIENT_EDIT_TYPE_ENTRY(comment,     "Коментарий:",        PASSPORTCOMMENT)

struct patinet_edit_t {
	delegate_t *d;
	struct passport_t *patient;
	CDKSCREEN *screen;
#define PATIENT_EDIT_TYPE_ENTRY(name, title, key) CDKENTRY    * m_##name;
#define PATIENT_EDIT_TYPE_EDATE(name, title, key) CDKTEMPLATE * m_##name;
#define PATIENT_EDIT_TYPE_ETELE(name, title, key) CDKTEMPLATE * m_##name;
#define PATIENT_EDIT_TYPE_TEXTV(name, title, key) CDKMENTRY   * m_##name;
	PATIENT_EDIT_TYPES
#undef PATIENT_EDIT_TYPE_ENTRY 
#undef PATIENT_EDIT_TYPE_ETELE 
#undef PATIENT_EDIT_TYPE_EDATE 
#undef PATIENT_EDIT_TYPE_TEXTV 
};

static void 
_patient_edit_cancel_callback(CDKBUTTON *button){
	struct patinet_edit_t *p = button->callbackData;
	exitOKCDKScreen(p->screen);
}

static void 
_patient_edit_ok_callback(CDKBUTTON *button){
	struct patinet_edit_t *p = button->callbackData;
	/* update patient */
#define PATIENT_EDIT_TYPE_ENTRY(name, title, key)\
	{\
		char *value = getCDKEntryValue(p->m_##name);\
		prozubi_passport_set_text(key, p->d->p, p->patient, value, true);\
	}

#define PATIENT_EDIT_TYPE_EDATE(name, title, key)\
	{\
		char *value = getCDKTemplateValue(p->m_##name);\
		struct tm tm;\
		ya_strptime(value, "%d%m%Y", &tm);\
		prozubi_passport_set_date(key, p->d->p, p->patient, ya_mktime(&tm), true);\
	}

#define PATIENT_EDIT_TYPE_ETELE(name, title, key)\
	{\
		char *value = getCDKTemplateValue(p->m_##name);\
		prozubi_passport_set_text(key, p->d->p, p->patient, value, true);\
	}

#define PATIENT_EDIT_TYPE_TEXTV(name, title, key)\
	{\
		char *value = getCDKMentryValue(p->m_##name);\
		prozubi_passport_set_text(key, p->d->p, p->patient, value, true);\
	}
	
	PATIENT_EDIT_TYPES
#undef PATIENT_EDIT_TYPE_ENTRY 
#undef PATIENT_EDIT_TYPE_ETELE 
#undef PATIENT_EDIT_TYPE_EDATE 
#undef PATIENT_EDIT_TYPE_TEXTV 
	
	exitOKCDKScreen(p->screen);
}


void
patient_edit_create(
		delegate_t *d,
		struct passport_t *patient
		)
{
	
	struct patinet_edit_t p;
	p.d = d;
	p.patient = patient;
	
	/* init window and screen */
	p.screen = screen_init_patientEdit(d, 35, 40, LINES/4, COLS/4, COLOR_BLACK_ON_WHITE);

	info_pannel_set_text(d, 
			"CTRL-q - закрыть, TAB - далее");
	
	int x = COLS/4, y = LINES/4;
	/* init widgets */

#define PATIENT_EDIT_TYPE_ENTRY(name, title, key)\
	CDKENTRY *m_##name = newCDKEntry(\
			p.screen,\
			x,\
			y,\
			"</B>" title "<!B>",\
			"",\
			A_NORMAL|COLOR_PAIR(COLOR_BLACK_ON_WHITE),\
			' ',\
			vMIXED,\
			40,\
			0,\
			256,\
			FALSE,\
			FALSE);\
	p.m_##name = m_##name;\
	y += 3;\
	setCDKEntryBackgroundColor(m_##name, "</57>");\
	bindCDKObject (vENTRY, m_##name, CTRL('q'), _patient_edit_q_handler, d);\
	size_t text_position_##name = 0;\
	setCDKMentryPreProcess (m_##name, entry_text_preHandler, &text_position_##name);\
	bindCDKObject (vENTRY, m_##name, KEY_MOUSE, input_mouse_handler, p.screen);\
	if (patient->name)\
		setCDKEntryValue(m_##name, patient->name);\
	m_##name->leftChar = 0;\
	m_##name->screenCol = 0;

const char *overlay = "";
const char *plate = ""; 	

#define PATIENT_EDIT_TYPE_ETELE(name, title, key)\
	overlay = COLOR_STR(COLOR_BLACK_ON_WHITE, "_ (___) ___-____");\
	plate = "# (###) ###-####";\
	CDKTEMPLATE *m_##name = newCDKTemplate(\
			p.screen,\
			x,\
			y,\
			"</B>" title "<!B>",\
			"",\
			plate,\
			overlay,\
			FALSE,\
			FALSE);\
	p.m_##name = m_##name;\
	y += 3;\
	setCDKTemplateBackgroundColor(m_##name, "</57>");\
	bindCDKObject (vTEMPLATE, m_##name, CTRL('q'), _patient_edit_q_handler, d);\
	bindCDKObject (vTEMPLATE, m_##name, KEY_MOUSE, input_mouse_handler, p.screen);\
	if (patient->name)\
		setCDKTemplateValue(m_##name, patient->name);
	
#define PATIENT_EDIT_TYPE_EDATE(name, title, key)\
	overlay = COLOR_STR(COLOR_BLACK_ON_WHITE, "__.__.____");\
	plate = "##.##.####";\
	CDKTEMPLATE *m_##name = newCDKTemplate(\
			p.screen,\
			x,\
			y,\
			"</B>" title "<!B>",\
			"",\
			plate,\
			overlay,\
			FALSE,\
			FALSE);\
	p.m_##name = m_##name;\
	y += 3;\
	setCDKTemplateBackgroundColor(m_##name, "</57>");\
	bindCDKObject (vTEMPLATE, m_##name, CTRL('q'), _patient_edit_q_handler, d);\
	bindCDKObject (vTEMPLATE, m_##name, KEY_MOUSE, input_mouse_handler, p.screen);\
	{\
		struct tm tm;\
		sec_to_tm(patient->name, &tm);\
		char date[11];\
		strftime(date, 11, "%d%m%Y", &tm);\
		setCDKTemplateValue(m_##name, date);\
	}
	
#define PATIENT_EDIT_TYPE_TEXTV(name, title, key)\
	CDKMENTRY *m_##name = newCDKMentry (\
		p.screen,\
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
		FALSE,\
		FALSE);\
	p.m_##name = m_##name;\
  y += 4;\
	setCDKMentryBackgroundColor(m_##name, "</57>");\
	bindCDKObject (vMENTRY, m_##name, CTRL('q'), _patient_edit_q_handler, d);\
	size_t text_position_##name = 0;\
	setCDKMentryPreProcess (m_##name, mentry_text_preHandler, &text_position_##name);\
	bindCDKObject (vMENTRY, m_##name, KEY_MOUSE, input_mouse_handler, p.screen);\
	if (patient->name)\
		setCDKMentryValue(m_##name, patient->name);\
	m_##name->currentCol = 0;
	
PATIENT_EDIT_TYPES
#undef PATIENT_EDIT_TYPE_ENTRY 
#undef PATIENT_EDIT_TYPE_ETELE 
#undef PATIENT_EDIT_TYPE_EDATE 
#undef PATIENT_EDIT_TYPE_TEXTV 

	y += 1;

	CDKBUTTON *ok = newCDKButton(
			p.screen, 
			x, 
			y, 
			"Сохран", 
			NULL, 
			TRUE, 
			FALSE);
	setCDKButtonBackgroundColor(ok, "</57>");
	ok->callback = _patient_edit_ok_callback;
	ok->callbackData = &p;
	bindCDKObject (vBUTTON, ok, CTRL('q'), _patient_edit_q_handler, d);\
	bindCDKObject (vBUTTON, ok, KEY_MOUSE, input_mouse_handler, p.screen);\

CDKBUTTON *cancel = newCDKButton(
			p.screen, 
			x + 22, 
			y, 
			"Отмена", 
			NULL, 
			TRUE, 
			FALSE);
	setCDKButtonBackgroundColor(cancel, "</57>");
	cancel->callback = _patient_edit_cancel_callback;
	cancel->callbackData = &p;
	bindCDKObject (vBUTTON, cancel, CTRL('q'), _patient_edit_q_handler, d);\
	bindCDKObject (vBUTTON, cancel, KEY_MOUSE, input_mouse_handler, p.screen);\

	/* start traverse */
	refreshCDKScreen(p.screen);
	traverseCDKScreen(p.screen);

	screen_destroy_patientEdit(d);
}
