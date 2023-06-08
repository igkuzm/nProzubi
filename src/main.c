/**
 * File              : main.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 14.05.2023
 * Last Modified Date: 08.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#include <cdk.h>
#include <cdk/selection.h>
#include <cdk/traverse.h>
#include <curses.h>
#include <locale.h>
#include <stdio.h>
#include "colors.h"

#include "InfoPannel.h"
#include "switcher.h"
#include "PatientsList.h"
#include "prozubilib/prozubilib.h"
#include "delegate.h"
#include "error.h"


int main(int argc, char *argv[])
{
	 /* init CDK */
	CDKSCREEN *cdkscreen = initCDKScreen(NULL);

	/* init mouse */
	mouseinterval(0);
	mousemask(ALL_MOUSE_EVENTS, NULL);

	/* init Fn keys */
	keypad(stdscr, TRUE);

	/*set delegate */
	delegate_t d;
	delegate_init(&d);
	d.screen = cdkscreen;
	d.selectedScreen = cdkscreen;
	
	/* open log file */
	void *log_cb = NULL;
	FILE *log = fopen("nprozubi.log", "w+");
	if (log)
		log_cb = log_callback;
	d.log = log;

	/* init kdata */
	d.p = prozubi_init(
			"/home/kuzmich/gProZubi/ProZubi.sqlite", 
			"",
			d.selectedScreen,
			error_callback,
			log,
			log_cb);
 
	/* fix locale */
	setlocale(LC_ALL, "ru_RU.CP1251");
	
	/* Start CDK colors. */
	initCDKColor ();
	
	/* setup curses */
	cbreak();  //Не использовать буфер для функции getch()
	raw();
	nonl();
	noecho();

	/* set color for main window */
	wbkgd(cdkscreen->window, COLOR_PAIR(COLOR_WHITE_ON_BLUE));
	
	/* start interface */
	CDKLABEL *info = create_info_pannel(&d, d.screen);
	CDKSELECTION *switcher = create_switcher(&d);
	d.patients = patients_list_create(&d);

	/* Draw the screen. */
	refreshCDKScreen (cdkscreen);

	/* Traverse the screen */
	traverseCDKScreen(cdkscreen);
	//activateCDKSelection(switcher, NULL);

	/* finish */
	if (log)
		fclose(log);
	destroyCDKLabel(info);
	destroyCDKSelection(switcher);
	destroyCDKSelection(d.patients);
	destroyCDKScreen(cdkscreen);
	endCDK ();
	return 0;
}

