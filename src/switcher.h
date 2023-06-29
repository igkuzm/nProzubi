/**
 * File              : switcher.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 14.05.2023
 * Last Modified Date: 26.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef SWITCHER_H
#define SWITCHER_H

#include <cdk.h>
#include <cdk/cdk_objs.h>
#include <curses.h>

//#include "colors.h"
#include "delegate.h"
#include "ncwidgets/src/nclist.h"
#include "ncwidgets/src/colors.h"
//#include "quit.h"
#include "input.h"
#include "PriceList.h"


static int 
switcher_preHandler (EObjectType cdktype GCC_UNUSED, void *object,
		       void *clientData GCC_UNUSED,
		       chtype input GCC_UNUSED)
{
	delegate_t *d = clientData;
	int selected = getCDKSelectionCurrent((CDKSELECTION *)object);
	/*info_pannel_set_text(d, "CTRL-q - выход, r - обновить, TAB - список");*/

	switch (input) {
		case KEY_ENTER:
			{
				if (selected == 0){
					setCDKFocusCurrent(d->screen_main, ObjOf(d->patients));
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
				//quit_message_show(d, d->screen_main);
				break;
			}
			
		default:
			break;
	}

	return 1;
}



//static CDKSELECTION *
static nclist_t *
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

	//CDKSELECTION 
		//*s =
					//newCDKSelection(
							//d->screen_main, 
							//0, 
							//0, 
							//1,
							//LINES - 1,
							//40,
							//[>COLOR_STR(COLOR_GREEN_ON_BLUE, "ProZubi"), <]
							//"",
							//items, 
							//3, 
							//choises,
							//1,
							//A_REVERSE,
							//FALSE, 
							//TRUE
							//);

	//wbkgd(s->win, COLOR_PAIR(COLOR_WHITE_ON_BLUE));
	//wbkgd(s->shadowWin, COLOR_PAIR(COLOR_WHITE_ON_BLUE));
  
	//bindCDKObject(vSELECTION, s, KEY_MOUSE, input_mouse_handler, d);\
	//setCDKSelectionPreProcess(s, switcher_preHandler, d);

	/*info_pannel_set_text(d, "CTRL-q - выход, r - обновить, TAB - список");*/
	
	nclist_t *s = 
			nc_list_new(
					NULL, 
					NULL, 
					LINES, 40, 0, 0, 
					5, 
					items, 
					3, 
					TRUE, 
					FALSE);

	return s;
}

#endif /* ifndef SWITCHER_H */
