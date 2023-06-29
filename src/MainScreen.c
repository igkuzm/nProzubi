/**
 * File              : MainScreen.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 26.06.2023
 * Last Modified Date: 27.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "MainScreen.h"
#include "ncwidgets/src/keys.h"
#include "ncwidgets/src/nclib.h"
#include "ncwidgets/src/nclist.h"
#include "ncwidgets/src/ncscreen.h"

#include "ncwidgets/src/colors.h"

#include "PatientsList.h"
#include "CasesList.h"

#include "switcher.h"
#include <curses.h>
#include <panel.h>

typedef struct main_screen {
	delegate_t *d;
	patients_list_t *patients;
	nclist_t *switcher;
} main_screen_t;

CBRET main_screen_callback(void *userdata, enum SCREEN type, void *object, chtype key)
{
	main_screen_t *t = userdata;
	if (object == t->patients->s){
		switch (key) {
			case KEY_ENTER: case KEY_RETURN: case '\r': 
				{
					int selected = nc_list_get_selected(t->patients->s);
					cases_list_create(t->d, t->patients->patients[selected]);	
					break;
				}
			case 'r':
				{
					patinets_list_update(t->patients);
					break;
				}

			default:
				break;
		}
	}

	return 0;
};

void
main_screen_create(delegate_t *d)
{
	nc_init("", WHITE_ON_BLUE);	
	patients_list_t *patients = patients_list_create(d);
	nclist_t *switcher = create_switcher(d);

	main_screen_t t = {d, patients, switcher};
	
	ncscreen_node_t *screen = _nc_screen_node_new(SCREEN_nclist, patients->s);
	nc_screen_add(&screen, SCREEN_nclist, switcher);
	nc_screen_activate(screen, patients->s, &t, main_screen_callback);

	free(patients);
}
