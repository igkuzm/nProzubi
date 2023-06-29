/**
 * File              : input.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 02.06.2023
 * Last Modified Date: 26.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "input.h"
#include <cdk.h>
#include <cdk/cdk_objs.h>
#include <cdk/dialog.h>
#include <cdk/traverse.h>
#include <curses.h>
#include "delegate.h"


int
input_escape_handler(EObjectType cdktype, void *object, void *clientData, chtype input)
{
	InjectObj((CDKOBJS *)object, KEY_ESC);
	return 1;
}

int 
input_mouse_check_selected(CDKOBJS *object, int x, int y){
	WINDOW *win = object->inputWindow;
	int winx, winy, winh, winw;
	getbegyx(win, winy, winx);
	getmaxyx(win, winh, winw);
	if (y>=winy && x>=winx && y <= winy + winh && x <= winx + winw)
		return 1;
	return 0;
}

CDKOBJS *
input_mouse_get_object(CDKSCREEN *screen, int x, int y)
{
	CDKOBJS **objs = screen->object;
	CDKOBJS  *obj  = objs[0];
	int i;
	for (i = 0; i < screen->objectCount; ++i) {
		if (input_mouse_check_selected(objs[i], x, y)){
				return objs[i];
		}
	}

	return obj;
}

void
input_unset_focus(CDKOBJS *obj)
{
	curs_set(0);
	if (obj != 0){
		HasFocusObj(obj) = FALSE;
		UnfocusObj(obj);
	}
}

void
input_set_focus(CDKOBJS *obj)
{
	if (obj != 0){
		HasFocusObj(obj) = TRUE;
		FocusObj(obj);
	}
	curs_set(1);
}

CDKOBJS *
input_switch_focus(CDKOBJS *new, CDKOBJS *old)
{
	if (old != new){
		input_unset_focus(old);
		input_set_focus(new);
	}
	return new;
}

int
input_mouse_handler(EObjectType cdktype, void *object, void *clientData, chtype input)
{
	CDKSCREEN *screen = ((CDKOBJS *)object)->screen;
	delegate_t *d = clientData;
	MEVENT event;
	if (getmouse(&event) == OK) {
		// mouse is OK!

		CDKOBJS *obj = input_mouse_get_object(screen, event.x, event.y);
		input_switch_focus(obj, getCDKFocusCurrent(screen));	
		setCDKFocusCurrent(screen, obj);
		
		if (event.bstate & BUTTON1_PRESSED){
			//left button
			switch (ObjTypeOf(obj)) {
				case vLABEL:
					{
						WINDOW *win = obj->inputWindow;
						if (wenclose(win, event.y, event.x)){
							InjectObj(obj, KEY_TAB);
						} else {
							InjectObj(obj, KEY_ESC);
						}

						return 1;
					}
				case vMENTRY:
					{
						WINDOW *win = obj->inputWindow;
						if (wenclose(win, event.y, event.x)){

						} else {
							InjectObj(obj, KEY_ESC);
						}
						return 1;
					}
				case vRADIO:
					{
						WINDOW *win = obj->inputWindow;
						if (wenclose(win, event.y, event.x)){
							int x, y, i;
							getbegyx(win, y, x);
							int selectedRow = event.y - y - 1;
							int selectedColumn = event.x - x;
							if (selectedColumn > 0 && selectedColumn < 3)
								((CDKRADIO *)obj)->selectedItem = selectedRow;
							return 1;
						} else {
							InjectObj(obj, KEY_ENTER);
						}
						return 1;
					}
				case vMATRIX:
					{
						WINDOW *win = ((CDKMATRIX *)obj)->win;
						if (wenclose(win, event.y, event.x)){

						} else {
							InjectObj(obj, KEY_ESC);
						}
						return 1;
					}
				case vCALENDAR:
					{
						WINDOW *win = obj->inputWindow;
						if (wenclose(win, event.y, event.x)){

						} else {
							InjectObj(obj, KEY_ESC);
						}
						return 1;
					}
				case vDIALOG:
				{
					if (event.bstate & BUTTON1_PRESSED){
						WINDOW *win = obj->inputWindow;
						int x, y, h, w, i;
						getbegyx(win, y, x);
						getmaxyx(win, h, w);
						int center = x + w/2;
						int selectedRow = event.y - y - 2;
						int selectedColumn = event.x;
						if (selectedRow == 2){
							if (selectedColumn < center && selectedColumn > center - 10)
							{ 
								//no button pressed
								if (((CDKDIALOG *)obj)->currentButton == 0)
									InjectObj(obj, KEY_ENTER);
								else
									((CDKDIALOG *)obj)->currentButton = 0;
								return 1;
							} 
							if (selectedColumn > center && selectedColumn < center + 10)
							{
								if (((CDKDIALOG *)obj)->currentButton == 1)
									InjectObj(obj, KEY_ENTER);
								else
									((CDKDIALOG *)obj)->currentButton = 1;
								return 1;
							}
						}
					}
						return 0;
				}
				case vVIEWER:
				{
					if (event.bstate & BUTTON1_PRESSED){
						WINDOW *win = obj->inputWindow;
						int x, y, h, w, i;
						getbegyx(win, y, x);
						getmaxyx(win, h, w);
						int center = x + w/2;
						int selectedRow = event.y;
						int selectedColumn = event.x;
						if (selectedRow == h - 1){
							if (selectedColumn > center - 5 && selectedColumn < center + 5)
							{
									InjectObj(obj, KEY_ENTER);
									return 1;
							}
						}
					}
						return 0;
				}


				case vBUTTON:
					InjectObj(obj, ' ');
					return 1;
				
				case vSELECTION:
					{
						WINDOW *win = obj->inputWindow;
						if (!wenclose(win, event.y, event.x)){
							// exit when out of window
							InjectObj(obj, 'q');
							return 1;
						}
						int x, y, i;
						getbegyx(win, y, x);
						int currentItem = getCDKSelectionCurrent((CDKSELECTION *)obj);
						int selectedRow = event.y - y - 1;
						if (currentItem < selectedRow){
							for (i = currentItem; i < selectedRow; ++i) {
								InjectObj(obj, KEY_DOWN);
							}
						} else if (currentItem > selectedRow){
							for (i = selectedRow; i < currentItem; ++i) {
								InjectObj(obj, KEY_UP);
							}
						} else {
								InjectObj(obj, KEY_ENTER);
						}
					}
					return 0;

				default:
					return 0;
			}
		}
		else if (event.bstate & BUTTON2_PRESSED){
			//right button
			switch (ObjTypeOf(obj)) {
				
				default:
					return 0;
			}
		}
		else if (event.bstate & BUTTON3_PRESSED){
			//central button
			switch (ObjTypeOf(obj)) {
				
				default:
					return 0;
			}
		}
		else if (event.bstate & BUTTON4_PRESSED){
			//scroll up
			switch (ObjTypeOf(obj)) {
				case vSELECTION:
					InjectObj(obj, KEY_UP);
					return 1;
				
				default:
					return 0;
			}
		}
		else if (event.bstate & BUTTON5_PRESSED){
			//scroll down
			switch (ObjTypeOf(obj)) {
				case vSELECTION:
					InjectObj(obj, KEY_DOWN);
					return 1;
				
				default:
					return 0;
			}
		}

	}

	return 0;
}

