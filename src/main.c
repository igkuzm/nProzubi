/**
 * File              : main.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 14.05.2023
 * Last Modified Date: 01.07.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#include <cdk.h>
#include <cdk/cdkscreen.h>
#include <cdk/selection.h>
#include <cdk/traverse.h>
#include <curses.h>
#include <locale.h>
#include <stdio.h>
#include "colors.h"

#include "ncwidgets/src/types.h"
#include "switcher.h"
#include "PatientsList.h"
#include "prozubilib/prozubilib.h"
#include "delegate.h"
#include "error.h"

#include "ncwidgets/src/ncwidgets.h"
#include "ncwidgets/src/nclist.h"
#include "ncwidgets/src/ncscreen.h"

#include "MainScreen.h"

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
	//d.cdkscreen = cdkscreen;
	
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
			cdkscreen,
			error_callback,
			log,
			log_cb);
 
	main_screen_create(&d);
	
	/* fix locale */
	/*setlocale(LC_ALL, "ru_RU.CP1251");*/
	//setlocale(LC_ALL, "ru_RU.KOI8-R");
	
	/* Start CDK colors. */
	//initCDKColor ();
	
	/* setup curses */
	//cbreak();  //Не использовать буфер для функции getch()
	//raw();
	//nonl();
	//noecho();

	/* start interface */
	//screen_init_screen_main(&d, LINES, COLS, 0, 0, COLOR_WHITE_ON_BLUE);
	//CDKSELECTION *switcher = create_switcher(&d);
	//d.patients = patients_list_create(&d);

	/* refresh screens */
	//refreshCDKScreen(d.screen_main);

	/* Traverse the screen */
	//traverseCDKScreen(d.screen_main);
	
	/* finish */
	if (log)
		fclose(log);

	//destroyCDKSelection(switcher);
	//destroyCDKSelection(d.patients);

	endwin();
	
	//endCDK ();
	return 0;
}

