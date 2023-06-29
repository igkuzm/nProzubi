/**
 * File              : PatientsList.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 14.05.2023
 * Last Modified Date: 26.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#include "PatientsList.h"

#include "ncwidgets/src/nclist.h"

#include <cdk.h>
#include <cdk/cdkscreen.h>
#include <cdk/fselect.h>
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "prozubilib/date-time/time_local.h"
#include "input.h"

#include "colors.h"
#include "prozubilib/prozubilib.h"
#include "delegate.h"
#include "quit.h"

#include "CasesList.h"
#include "PatientEdit.h"
#include "error.h"

#include "utf8tocp1251/utf8_to_cp1251.h"

static int 
patients_list_calback(void *user_data, struct passport_t *c)
{
	patients_list_t *t = user_data;

	char dateOfBirth[16];
	struct tm tp;
	sec_to_tm(c->dateofbirth, &tp);
	ya_strftime(dateOfBirth, 16, "%d.%m.%Y", &tp);

	t->patients[t->count] = c;

  char *title = MALLOC(256, error_callback(t->d->screen_main,
				"cant' allocate memory for pointer"), return 0);
	snprintf(title, 255, "%d. %s %s %s %s %s \t\t\t                       ", 
			t->count + 1,
			c->familiya ? c->familiya : "", 
			c->imia ? c->imia : "", 
			c->otchestvo ? c->otchestvo : "", 
			dateOfBirth, 
			c->tel ? c->tel : "" 
	);
	t->titles[t->count] = title;

	t->count++;

	t->patients = REALLOC(t->patients, t->count * 8 + 8, 
			error_callback(t->d->screen_main, "can't allocate memory"), return 0);
	
	t->titles   = REALLOC(t->titles, t->count * 8 + 8, 
			error_callback(t->d->screen_main, "can't allocate memory"), return 0);

	return 0;
}
void
patinets_list_free(patients_list_t *t)
{
	int i;
	for (i = 0; i < t->count; ++i) {
		prozubi_passport_free(t->patients[i]);
		free(t->titles[i]);
	}
	t->count = 0;
	if (t->patients)
		free(t->patients);
	t->patients = NULL;
	if (t->titles)
		//free(t->titles);
	t->titles = NULL;
}
	
void
patinets_list_update(patients_list_t *t)
{
	patinets_list_free(t);

	t->patients = MALLOC(8, error_callback(t->d->screen_main, "can't allocate memory"), return);
	t->titles   = MALLOC(8, error_callback(t->d->screen_main, "can't allocate memory"), return);

	prozubi_passport_foreach(t->d->p, t, patients_list_calback);
	
	//setCDKSelectionItems(t->s, t->titles, t->count);
	//refreshCDKScreen(t->d->screen_main);
	nc_list_set_value(t->s, t->titles, t->count);
}

static void 
patient_list_remove_patient_confirm(patients_list_t *t)
{
	/* *INDENT-EQLS* */
	char *buttons[] =
	{
	  " Cancel ",
		COLOR_STR(COLOR_RED_ON_WHITE, " Удалить "), 
	};
  
	char *message[] = {"Вы уверены?"};

	int selection;

	CDKDIALOG *m = newCDKDialog (
	t->d->screen_main	/* cdkscreen */,
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

	bindCDKObject (vDIALOG, m, KEY_MOUSE, input_mouse_handler, t->d);\

	wbkgd(m->win, COLOR_PAIR(COLOR_BLACK_ON_WHITE));

	/* Activate the entry field. */
	activateCDKDialog(m, NULL);
	
	//if (m->currentButton == 1)
		//if (!prozubi_passport_remove(t->d->p, t->patients[t->s->currentItem]))
			//patinets_list_update(t);
	
	destroyCDKDialog(m);
	refreshCDKScreen(t->d->screen_main);
}

static void 
patinet_list_remove_patient_message_show(patients_list_t *t)
{
	/* *INDENT-EQLS* */
	char *buttons[] =
	{
	  " Cancel ",
		COLOR_STR(COLOR_RED_ON_WHITE, " Удалить "), 
	};

	//struct passport_t *selectedPatient = t->patients[t->s->currentItem];
  
	char **message = malloc(8*3); 
	message[0] = "удалить данные пациента";
	//char fio[256];
	//sprintf(fio, "%s %s %s?", 
			//selectedPatient->familiya,
			//selectedPatient->imia,
			//selectedPatient->otchestvo);
	//message[1] = fio;

	int selection;

	CDKDIALOG *m = newCDKDialog (
	t->d->screen_main	/* cdkscreen */,
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

	bindCDKObject (vDIALOG, m, KEY_MOUSE, input_mouse_handler, t->d);\
	
	wbkgd(m->win, COLOR_PAIR(COLOR_BLACK_ON_WHITE));

	/* Activate the entry field. */
	activateCDKDialog(m, NULL);
	int ret = m->currentButton;
	
	destroyCDKDialog(m);
	refreshCDKScreen(t->d->screen_main);
	
	if (ret == 1)
		patient_list_remove_patient_confirm(t);
}


static int 
petients_list_preHandler (EObjectType cdktype GCC_UNUSED, void *object,
		       void *clientData GCC_UNUSED,
		       chtype input GCC_UNUSED)
{
	CDKSELECTION *s = object;
	patients_list_t *t = clientData;
	//info_pannel_set_text(t->d, 
			//"CTRL-q - выход, r - обновить, a - добавить, e - редактировать, d - удалить, ENTER - визиты");
	int selected = getCDKSelectionCurrent(s);
	
	switch (input) {
		case KEY_ENTER:
			{
				cases_list_create(t->d, t->patients[selected]);
				break;
			}
		case 'q': case CTRL('q'):
			{
				quit_message_show(t->d, t->d->screen_main);
				break;
			}
		case 'r':
			{
				patinets_list_update(t);
				break;
			}
	  case 'd':
			{
				patinet_list_remove_patient_message_show(t);
				break;
			}
	  case 'e':
			{
				patient_edit_create(t->d, t->patients[selected]);
				patinets_list_update(t);
				break;
			}
		case 'a':
			{
				struct passport_t *c = prozubi_passport_new(
					t->d->p, 
					"Новый", 
					"пациент", 
					"", 
					"", 
					"", 
					"", 
					"", 
					"", 
					0, 
					NULL
					);
				if (c){
					patient_edit_create(t->d, c);
					patinets_list_update(t);
					//free(c);
				}

				break;
			}

		default:
			break;
	}

	return 1;
}

//CDKSELECTION *
patients_list_t *
patients_list_create(
		delegate_t *d
		)
{  
	//char * choises[] = 
	//{""};
	//CDKSELECTION *s =
					//newCDKSelection(
							//d->screen_main, 
							//41, 
							//0, 
							//1,
							//LINES - 1,
							//COLS-40,
							//"</B>Список пациентов:<!B>", 
							//NULL, 
							//0, 
							//choises,
							//1,
							//A_REVERSE,
							//FALSE, 
							//TRUE
							//);

	//struct patients_list_t t;
	//t.count = 0;
	//t.d = d;
	//t.s = s;
	//t.patients = NULL;
	//t.titles = NULL;

	//patinets_list_update(&t);

	//wbkgd(s->win, COLOR_PAIR(COLOR_WHITE_ON_BLUE));
	//wbkgd(s->shadowWin, COLOR_PAIR(COLOR_WHITE_ON_BLUE));
	//bindCDKObject (vSELECTION, s, KEY_MOUSE, input_mouse_handler, d);
	//setCDKSelectionPreProcess (s, petients_list_preHandler, &t);

	//return s;
	
	nclist_t *s = nc_list_new(
			NULL, 
			"</B>Список пациентов:<!B>", 
			LINES, COLS - 40, 0, 40, 
			COLOR_WHITE_ON_BLUE, 
			NULL, 
			0, 
			TRUE, 
			FALSE
			);
	
	patients_list_t *t = malloc(sizeof(patients_list_t));
	if (!t)
		return NULL;
	t->count = 0;
	t->d = d;
	t->s = s;
	t->patients = NULL;
	t->titles = NULL;

	patinets_list_update(t);

	return t;
}
