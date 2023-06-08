/**
 * File              : TextUTF8Handler.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 29.05.2023
 * Last Modified Date: 08.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "TextUTF8Handler.h"
#include <curses.h>

int 
mentry_text_preHandler (EObjectType cdktype GCC_UNUSED, void *object,
		       void *clientData GCC_UNUSED,
		       chtype input GCC_UNUSED)
{
	CDKMENTRY *m = object;
	size_t *p = clientData;
	size_t len = strlen(m->info);
	switch (input){
		case KEY_ESC:
		case KEY_ENTER:
		case CDK_REFRESH:
		case KEY_MOUSE:
		case CTRL('q'):
			return 1; 

		case KEY_UP: case KEY_DOWN:
			{
				int row = m->currentRow;
				int col = m->currentCol;
				if (input == KEY_UP)
					row--;
				if (input == KEY_DOWN)
					row++;
				
				if (row < 0)
					row = 0;

				/* get cursor position */
				/*size_t pos = ((row + m->topRow) * m->fieldWidth) + col;*/
				size_t pos = ((row + m->topRow) * m->fieldWidth);
				*p = pos;
				
				size_t i;
				for (i=pos; i<=pos + col; i++){
					unsigned char c = m->info[*p];
					if (c >= 252) /* 6-bytes */
						p[0] += 6;
					else if (c >= 248) /* 5-bytes */
						p[0] += 5;
					else if (c >= 240) /* 4-bytes */
						p[0] += 4;
					else if (c >= 224) /* 3-bytes */
						p[0] += 3;
					else if (c >= 192) /* 2-bytes */
						p[0] += 2;
					else /* 1-byte */
						p[0]++;
				}

				if (*p > len)
					*p = len;
			}
			return 1;
		case KEY_RIGHT:
			{
				if (*p == len)
					return 0;

				unsigned char c = m->info[*p];
				if (c >= 252) /* 6-bytes */
					p[0] += 6;
				else if (c >= 248) /* 5-bytes */
					p[0] += 5;
				else if (c >= 240) /* 4-bytes */
					p[0] += 4;
				else if (c >= 224) /* 3-bytes */
					p[0] += 3;
				else if (c >= 192) /* 2-bytes */
					p[0] += 2;
				else /* 1-byte */
					p[0]++;
				
				if (*p <= len)
					return 1;
				
				return 0;
			}
		case KEY_LEFT: case KEY_BACKSPACE: case 127: case KEY_DC:
			{
				if (*p == 0)
					return 0;
				
				/* get previous possition */
				size_t pos;
				size_t i;
				for (i = 0; i < *p; ) {
					pos = i;
					unsigned char c = m->info[i];
					if (c >= 252) /* 6-bytes */
						i += 6;
					else if (c >= 248) /* 5-bytes */
						i += 5;
					else if (c >= 240) /* 4-bytes */
						i += 4;
					else if (c >= 224) /* 3-bytes */
						i += 3;
					else if (c >= 192) /* 2-bytes */
						i += 2;
					else /* 1-byte */
						i++;
				}

				if (input == KEY_BACKSPACE || input == 127 || input == KEY_DC){
					m->info[pos] = 0;
					strcat(m->info, &(m->info[*p]));
					*p = pos;
	
					m->currentCol--;
					if (m->currentCol < 0) {
						m->currentRow--;
						m->currentCol = m->fieldWidth;
					}
					eraseCDKMentry(m);
					drawCDKMentryField(m);
					drawCDKMentry(m, TRUE);
					return 0;
				}
				
				*p = pos;
				return 1;
			}
		
		default: {
			/* copy string after position*/
			char *str = "";
			if (*p < len){
				str = malloc(m->totalWidth - len);
				strncpy(str, &(m->info[*p]), m->totalWidth - len);
			}

			unsigned char c = input;
			m->info[p[0]++] = c; 
			m->info[*p] = 0;
			strcat(m->info, str);
			/* free string */
			if (*str)
				free(str);

			if (c < 192){
				m->currentCol++;
				if (m->currentCol > m->fieldWidth){
					m->currentCol = 0;
					m->currentRow++;
				}
			}
			
			eraseCDKMentry(m);
			drawCDKMentry(m, TRUE);
			
			return 0;
		}
	}
	return 1;
}

int 
entry_text_preHandler (EObjectType cdktype GCC_UNUSED, void *object,
		       void *clientData GCC_UNUSED,
		       chtype input GCC_UNUSED)
{
	CDKENTRY *m = object;
	size_t *p = clientData;
	size_t len = strlen(m->info);
	switch (input){
		case KEY_ESC:
		case KEY_ENTER:
		case CDK_REFRESH:
		case KEY_MOUSE:
		case CTRL('q'):
			return 1; 
		
		case KEY_UP: case KEY_DOWN:
			return 0;
			
		case KEY_RIGHT:
			{
				if (*p == len)
					return 0;

				unsigned char c = m->info[*p];
				if (c >= 252) /* 6-bytes */
					p[0] += 6;
				else if (c >= 248) /* 5-bytes */
					p[0] += 5;
				else if (c >= 240) /* 4-bytes */
					p[0] += 4;
				else if (c >= 224) /* 3-bytes */
					p[0] += 3;
				else if (c >= 192) /* 2-bytes */
					p[0] += 2;
				else /* 1-byte */
					p[0]++;
				
				if (*p <= len)
					return 1;
				
				return 0;
			}
		case KEY_LEFT: case KEY_BACKSPACE: case 127: case KEY_DC:
			{
				if (*p == 0)
					return 0;
				
				/* get previous possition */
				size_t pos;
				size_t i;
				for (i = 0; i < *p; ) {
					pos = i;
					unsigned char c = m->info[i];
					if (c >= 252) /* 6-bytes */
						i += 6;
					else if (c >= 248) /* 5-bytes */
						i += 5;
					else if (c >= 240) /* 4-bytes */
						i += 4;
					else if (c >= 224) /* 3-bytes */
						i += 3;
					else if (c >= 192) /* 2-bytes */
						i += 2;
					else /* 1-byte */
						i++;
				}

				if (input == KEY_BACKSPACE || input == 127 || input == KEY_DC){
					m->info[pos] = 0;
					strcat(m->info, &(m->info[*p]));
					*p = pos;

					int col = 0;
					//get column
					for (i = 0; i < pos;){
						unsigned char c = m->info[i];
						if (c >= 252) /* 6-bytes */
							i += 6;
						else if (c >= 248) /* 5-bytes */
							i += 5;
						else if (c >= 240) /* 4-bytes */
							i += 4;
						else if (c >= 224) /* 3-bytes */
							i += 3;
						else if (c >= 192) /* 2-bytes */
							i += 2;
						else /* 1-byte */
							i++;
						col++;
					}

					eraseCDKEntry(m);
					m->screenCol = col;
					drawCDKEntry(m, TRUE);
					return 0;
				}
				
				*p = pos;
				return 1;
			}
		
		default: {
			/* copy string after position*/
			char *str = "";
			if (*p < len){
				str = malloc(m->fieldWidth - len);
				strncpy(str, &(m->info[*p]), m->fieldWidth - len);
			}

			unsigned char c = input;
			m->info[p[0]++] = c; 
			m->info[*p] = 0;
			strcat(m->info, str);
			/* free string */
			if (*str)
				free(str);

			if (c < 192){
				m->screenCol++;
			}
			
			eraseCDKEntry(m);
			drawCDKEntry(m, TRUE);
			
			return 0;
		}
	}
	return 1;
}

int 
screen_update_postHandler (EObjectType cdktype GCC_UNUSED, void *object,
		       void *clientData GCC_UNUSED,
		       chtype input GCC_UNUSED)
{
	CDKSCREEN *screen = ((CDKOBJS *)object)->screen;
	switch (input) {
		case KEY_DOWN: case KEY_UP: case KEY_NPAGE: case KEY_PPAGE:
			refreshCDKScreen(screen);
			break;
	
		default:
			break;
	}
	return 1;

}


