/**
 * File              : switcher.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 14.05.2023
 * Last Modified Date: 08.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef SWITCHER_H
#define SWITCHER_H

#include <cdk.h>
#include <cdk/cdk_objs.h>
#include <curses.h>

#include "colors.h"
#include "delegate.h"
#include "quit.h"
#include "input.h"
#include "InfoPannel.h"
#include "PriceList.h"


static int 
switcher_preHandler (EObjectType cdktype GCC_UNUSED, void *object,
		       void *clientData GCC_UNUSED,
		       chtype input GCC_UNUSED)
{
	delegate_t *d = clientData;
	int selected = getCDKSelectionCurrent((CDKSELECTION *)object);
	info_pannel_set_text(d, "CTRL-q - выход, r - обновить, TAB - список");

	switch (input) {
		case KEY_ENTER:
			{
				if (selected == 0){
					setCDKFocusCurrent(d->screen, ObjOf(d->patients));
					input_switch_focus(ObjOf(d->patients), (CDKOBJS *)object);
					return 1;
				}
				if (selected == 2){
					price_list_create(d, NULL, NULL);
					return 1;
				}
				break;
			}
		case 'q': case CTRL('q'):
			{
				quit_message_show(d, d->screen);
				break;
			}
			
		default:
			break;
	}

	return 1;
}



static CDKSELECTION *
create_switcher(delegate_t *d)
{  
	char * choises[] = 
	{""};
	
	char * items[] = 
	{
		"</B>Список пациентов                     <!B>",
		"</B>Список сотрудников                   <!B>",
		"</B>Список услуг (прайс)                 <!B>"
	};

	CDKSELECTION 
		*switcher =
					newCDKSelection(
							d->screen, 
							0, 
							3, 
							1,
							LINES - 3,
							40,
							COLOR_STR(COLOR_GREEN_ON_BLUE, "ProZubi"), 
							items, 
							3, 
							choises,
							1,
							A_REVERSE,
							FALSE, 
							FALSE
							);

	wbkgd(switcher->win, COLOR_PAIR(COLOR_WHITE_ON_BLUE));
  
	bindCDKObject (vSELECTION, switcher, KEY_MOUSE, input_mouse_handler, d->screen);\
	setCDKSelectionPreProcess (switcher, switcher_preHandler, d);

	info_pannel_set_text(d, "CTRL-q - выход, r - обновить, TAB - список");
	return switcher;
}

#endif /* ifndef SWITCHER_H */
