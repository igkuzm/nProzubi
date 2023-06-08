/**
 * File              : error.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.05.2023
 * Last Modified Date: 01.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#include "error.h"
#include <cdk.h>
#include <stdio.h>

void error_callback(void *userdata, const char * error){
	CDKSCREEN *screen = userdata;
	char * err = (char *)error;
	char *buttons[] = {"OK"};
	if (error){
		CDKDIALOG *dialog = newCDKDialog(
				screen, 
				CENTER, 
				CENTER, 
				&err, 
				1, 
				buttons, 
				1, 
				A_REVERSE, 
				FALSE, 
				TRUE, 
				TRUE
				);
		
		setCDKDialogBackgroundColor(dialog, "</57>");
		activateCDKDialog(dialog, NULL);
		destroyCDKDialog(dialog);
		refreshCDKScreen(screen);
	}
};

void log_callback(void *userdata, const char * message){
	FILE *log = userdata;
	fputs(message, log);
	fputs("\n", log);
}
