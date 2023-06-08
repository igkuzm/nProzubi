/**
 * File              : InfoPannel.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 29.05.2023
 * Last Modified Date: 08.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "InfoPannel.h"

#include <cdk.h>
#include <cdk/label.h>
#include "delegate.h"
#include "error.h"
#include "input.h"

static char *
_info_pannel_fill_string_with_spaces(
		delegate_t *d,
		const char *str
		)
{
  char *s = MALLOC(COLS + 1, error_callback(d->screen,
				"cant' allocate memory for pointer"), return NULL);
	int len = 0;
	if (str != NULL){
		len = strlen(str);
		strcpy(s, str);
	}
	if (len < COLS)
		memset(&(s[len]), ' ', COLS - len);
	s[COLS] = 0;
	return s;
}

void
info_pannel_set_text(
		delegate_t *d,
		const char *text
		)
{
	char *str = 
		_info_pannel_fill_string_with_spaces(d, text);
	if (str){
		char *msg[] = {str};
		setCDKLabelMessage(d->infoPannel, msg, 1);
		free(str);
	}
}

CDKLABEL *
create_info_pannel(
		delegate_t *d,
		CDKSCREEN *screen
		)
{
	char *str = _info_pannel_fill_string_with_spaces(d, NULL);
	char *msg[] = {str};
	CDKLABEL *m = 
		newCDKLabel(
				screen, 
				0, 
				0, 
				msg, 
				1, 
				TRUE, 
				FALSE);

	setCDKLabelBackgroundColor(m, "</57>");
	d->infoPannel = m;
	free(str);
	
	bindCDKObject (vLABEL, m, KEY_MOUSE, input_mouse_handler, screen);\
	
	return m;
}
