/**
 * File              : quit.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 15.05.2023
 * Last Modified Date: 05.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef QUIT_H
#define QUIT_H

#include <cdk.h>
#include <cdk/cdkscreen.h>
#include <curses.h>
#include "delegate.h"
#include "colors.h"
#include "input.h"


static void 
quit_message_show(delegate_t *d, CDKSCREEN *screen)
{
	/* *INDENT-EQLS* */
	char *buttons[] =
	{
	  " Cancel ",
		COLOR_STR(COLOR_RED_ON_WHITE, " Quit "), 
	};
  
	char *message[] = 
	{
		"Завершить программу",
		"и закрыть все окна?"
	};

	int selection;

	CDKDIALOG *m = newCDKDialog (
	screen	/* cdkscreen */,
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

	bindCDKObject (vDIALOG, m, KEY_MOUSE, input_mouse_handler, NULL);\
	
	wbkgd(m->win, COLOR_PAIR(COLOR_BLACK_ON_WHITE));

	/* Activate the entry field. */
	activateCDKDialog(m, NULL);
	
	if (m->currentButton == 1){
		exitOKCDKScreen(d->screen);
		return;
	}
	
	destroyCDKDialog(m);
	refreshCDKScreen(d->screen);
	refreshCDKScreen(screen);
}

#endif /* ifndef QUIT_H */

