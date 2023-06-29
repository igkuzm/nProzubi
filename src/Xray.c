/**
 * File              : Xray.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.06.2023
 * Last Modified Date: 10.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "Xray.h"
#include "colors.h"
#include "image2ascii.h"
#include "error.h"

#include <cdk/cdk_objs.h>
#include <cdk/cdkscreen.h>
#include <cdk/selection.h>
#include <cdk/traverse.h>
#include <cdk/viewer.h>
#include <curses.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <tiffio.h>
#include "input.h"


int 
xray_list_update_cb(void *user_data, struct image_t *c)
{
	delegate_t *d = user_data;

	char date[11];
	struct tm *tp = localtime(&c->date);
	strftime(date, 11, "%d.%m.%Y", tp);

	d->images[d->imagesCount] = c;

  char *title = MALLOC(256, error_callback(d->screen_main,
				"cant' allocate memory for pointer"), return 0);
	snprintf(title, 255, "%d. %s %s\t\t\t                       ", 
			d->imagesCount + 1,
			date, 
			c->title ? c->title : "снимок" 
	);
	d->imagesTitles[d->imagesCount] = title;

	d->imagesCount++;
	
	// realloc arrays
  d->imagesTitles = REALLOC(d->imagesTitles, d->imagesCount * 8 + 8, 
			error_callback(d->screen_xray,
			"cant' reallocate memory for pointer"), return 0);

  d->images = REALLOC(d->images, (d->imagesCount + 2) * 8, 
			error_callback(d->screen_xray,
			"cant' reallocate memory for pointer"), return 0);

	return 0;
}

void
xray_list_free(delegate_t *d)
{
	int i;
	for (i = 0; i < d->imagesCount; ++i) {
		/* free memory */
		prozubi_image_free(d->images[i]);
		free(d->imagesTitles[i]);
	}
	if (d->images)
		free(d->images);
	if (d->imagesTitles)
		free(d->imagesTitles);
	
	d->images = NULL;
	d->imagesTitles = NULL;
	d->imagesCount = 0;
}

void
xray_list_update(delegate_t *d, 
		struct case_list_node *node)
{
	xray_list_free(d);
	
	/* allocate and fill data */
	d->images = MALLOC(8, error_callback(d->screen_xray,
				"cant' allocate memory for pointer"), return);
	d->imagesTitles = MALLOC(8, error_callback(d->screen_xray,
				"cant' allocate memory for pounter"), return);
	prozubi_image_foreach(d->p, node->c->id, d, xray_list_update_cb);

	setCDKSelectionItems (d->imagesList, d->imagesTitles, d->imagesCount);
	refreshCDKScreen(d->screen_xray);
}

struct jpg_write_s {
	void *data;
	size_t len;
	delegate_t *d;
};

void jpg_write_func(void *context, void *data, int size){
	struct jpg_write_s *s = context; 
	// realloc data
	s->data = REALLOC(s->data, s->len + size,
			error_callback(s->d->screen_xray, "can't realloc"), return;);
	// copy
	memcpy(&(s->data[s->len]), data, size);
	s->len += size;
}

struct xray_list_preHandler_s {
	delegate_t *d;
	struct case_list_node *node;
};

void 
xray_remove_image_confirm(
		delegate_t *d, 
		struct case_list_node *node, 
		struct image_t *image)
{
	/* *INDENT-EQLS* */
	char *buttons[] =
	{
	  " Cancel ",
		COLOR_STR(COLOR_RED_ON_WHITE, " Удалить "), 
	};
  
	char *message[] = {"Вы уверены?"};

	int selection;

	CDKDIALOG *m = newCDKDialog (
	d->screen_xray	/* cdkscreen */,
	CENTER		/* xPos */,
	CENTER		/* yPos */,
	message	/* message */,
	1		/* Rows */,
	buttons	/* buttons */,
	2		/* buttonCount */,
	A_REVERSE		/* highlight */,
	TRUE		/* separator */,
	TRUE		/* Box */,
	TRUE		/* shadow */
			);

	bindCDKObject (vDIALOG, m, KEY_MOUSE, input_mouse_handler, d);\
	wbkgd(m->win, COLOR_PAIR(COLOR_BLACK_ON_WHITE));

	/* Activate the entry field. */
	activateCDKDialog(m, NULL);
	
	if (m->currentButton == 1)
		if (!prozubi_image_remove(d->p, image))
			xray_list_update(d, node);
	
	destroyCDKDialog(m);
}

void 
xray_remove_image_message_show(
		delegate_t *d, 
		struct case_list_node *node, 
		struct image_t *image,
		int number
		)
{
	/* *INDENT-EQLS* */
	char *buttons[] =
	{
	  " Cancel ",
		COLOR_STR(COLOR_RED_ON_WHITE, " Удалить "), 
	};
  
	char **message = malloc(8*3); 
	message[0] = "удалить изображение";
	char title[256];
	sprintf(title, "%s #%d?", 
			image->title? image->title: "снимок",
			number);
	message[1] = title;

	int selection;

	CDKDIALOG *m = newCDKDialog (
	d->screen_xray	/* cdkscreen */,
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

	free(message);

	bindCDKObject (vDIALOG, m, KEY_MOUSE, input_mouse_handler, d);\
	
	wbkgd(m->win, COLOR_PAIR(COLOR_BLACK_ON_WHITE));

	/* Activate the entry field. */
	activateCDKDialog(m, NULL);
	int ret = m->currentButton;
	
	destroyCDKDialog(m);
	refreshCDKScreen(d->screen_xray);
	
	if (ret == 1)
		xray_remove_image_confirm(d, node, image);
}

int 
xray_list_preHandler (EObjectType cdktype GCC_UNUSED, void *object,
		       void *clientData GCC_UNUSED,
		       chtype input GCC_UNUSED)
{
	CDKSELECTION *m = object;
	struct xray_list_preHandler_s *t = clientData;
	delegate_t *d = t->d;
	struct case_list_node *node = t->node;
	switch (input) {
		case 'd':
		{		
			int selected = getCDKSelectionCurrent(m);
			if (selected < 0)
				return 0;
			struct image_t *image = d->images[selected]; 
			xray_remove_image_message_show(d, node, image, selected + 1);
			return 0;
		}
		case KEY_ENTER:
			{
				int selected = getCDKSelectionCurrent(m);
				if (selected < 0)
					return 0;
				
				struct image_t *image = d->images[selected]; 
				char *ascii;
				size_t size = image2ascii(
						image->data, 
						image->len_data, 
						COLS/2, 0, 
						&ascii,
						d->screen_xray,
						error_callback
						);

				if (size == -1)
					return 0;
				
				//alloc info
				char **info = MALLOC(8, 
					error_callback(d->screen_xray, "error allocate memory for CDKVIEWER info"), return 0);
				int lines = 0;

				size_t i;
				info[lines++] = &(ascii[i]);
				for (i = 0; i < size; ++i) {
					if (ascii[i] == '\n'){
						info = REALLOC(info, 8 * lines + 8 + 8, 
							error_callback(d->screen_xray, "error reallocate memory for CDKVIEWER info"), return 0);
						ascii[i] = 0;
						info[lines++] = &(ascii[++i]);
					}
				}
				
				char * buttons[] = { "OK"};

				screen_init_screen_imageshow(d, LINES, COLS, 0, 0, 0);

				CDKVIEWER *m = newCDKViewer (
				d->screen_imageshow	/* cdkscreen */,
				0		/* xpos */,
				0		  /* ypos */,
				LINES		/* height */,
				COLS		/* width */,
				buttons	/* buttons */,
				1		/* buttonCount */,
				A_REVERSE		/* buttonHighlight */,
				TRUE		/* Box */,
				FALSE		/* shadow */
				);

				if (!m){
					error_callback(d->screen_xray, "can't draw CDKVIEWER - the screen is too small");
					return 0;
				}
				setCDKViewerInfoLine(m, FALSE);
				
				bindCDKObject (vVIEWER, m, KEY_MOUSE, input_mouse_handler, d);\
				
				setCDKViewerInfo(m, info, lines, TRUE);
				
				activateCDKViewer(m, NULL);

				destroyCDKViewer(m);
				free(info);
			
				/* redraw CDK */
				screen_destroy_screen_imageshow(d);
				return 0;
			}
		case 'a':
			{
				screen_init_screen_imageselect(d, LINES/2, COLS/2, LINES/4, COLS/4, COLOR_BLACK_ON_WHITE);
				
				CDKFSELECT *s = newCDKFselect (            
				d->screen_imageselect /* cdkscreen */,       
				0   /* xpos */,                  
				0   /* ypos */,                  
				LINES/2   /* height */,                
				COLS/2  /* width */,                 
				node->title  /* title */,         
				""  /* label */,         
				A_NORMAL    /* fieldAttribute */,    
				' '    /* fillerChar */,        
				A_REVERSE    /* highlight */,         
				"</33>"  /* dirAttributes */, 
				"</N>"  /* fileAttributes */,
				"</17>"  /* linkAttribute */, 
				"</49>"  /* sockAttribute */, 
				FALSE   /* Box */,               
				TRUE   /* shadow */
				);           
				
				if (!s){
					error_callback(d->screen_imageselect, "can't draw CDKFSELECT - the screen os too small");
					return 0;
				}
				wbkgd(s->win, COLOR_PAIR(COLOR_BLACK_ON_WHITE));
				wbkgd(s->entryField->fieldWin, COLOR_PAIR(COLOR_BLACK_ON_WHITE));
				setCDKFselectBackgroundColor(s, "</57>");
	
				bindCDKObject (vFSELECT, s, KEY_MOUSE, input_mouse_handler, d);\

				/* activate */
				char * ret = 
								activateCDKFselect(s, NULL);
				while (ret != NULL){
						// try to load image (png, bmp, jpeg) 
						int H, W, C;
						stbi_uc *image = 
								stbi_load(ret, &W, &H, &C, 0);
						
						// try to load tiff
						if (!image){
							TIFF *tif =
									TIFFOpen(ret, "r");
							if (tif){
								C = 4;
								TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &W);
								TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &H);
								if (W > 1 && H > 1){
									image = _TIFFmalloc(W * H * C);
									if (image){
										TIFFReadRGBAImage(tif, W, H, (uint32_t *)image, 0);
									}
								}
							}
						}
						if (image){
							//add image to base, update viewer and close fselect
							
							struct jpg_write_s s;
							s.data = MALLOC(1, error_callback(d->screen_imageselect, 
									"can't allocate struct jpg_write_s"), continue;);
							s.len = 0;
							s.d = d;

							stbi_write_jpg_to_func(jpg_write_func, &s, W, H, C, image, 100);	
							prozubi_image_new(
									d->p, 
									time(NULL), 
									"снимок", 
									node->c->id, 
									s.data, 
									s.len, 
									NULL
									);
							
							xray_list_update(d, node);
							free(s.data);
							break;
						}

						ret = 
								activateCDKFselect(s, NULL);
				}
				
				/* destroy widgets */
				destroyCDKFselect(s);
				screen_destroy_screen_imageselect(d);
				return 0;;
		}
		case 'q': case CTRL('q'):
			{
				InjectObj((CDKOBJS *)object, KEY_ESC);
				return 1;
			}

		default:
			break;
	}

	return 1;
}

void
xray_create(
		struct case_list_node *node,
		delegate_t *d)
{
	/* init window and screen */
	screen_init_screen_xray(d, LINES/3, COLS/3, LINES/3, COLS/3, COLOR_BLACK_ON_CYAN);

	char * choises[] = 
	{""};
	
	CDKSELECTION *imagesList =
		newCDKSelection(
				d->screen_xray, 
				0, 
				0, 
				0,
				LINES/3, 
				COLS/3, 
				"Прикреплённые изображения:", 
				NULL, 
				0, 
				choises, 
				1, 
				A_REVERSE, 
				FALSE, 
				TRUE);
	if (!imagesList){
		error_callback(d->screen_xray, "can't draw CDKSELECTION - the screen os too small");
		return;
	}

	d->imagesList = imagesList;
	
	bindCDKObject (vSELECTION, imagesList, KEY_MOUSE, input_mouse_handler, d);\
	
	wbkgd(imagesList->shadowWin, COLOR_PAIR(COLOR_BLACK_ON_CYAN));
	
	setCDKMentryBackgroundColor(imagesList, COLOR_N(COLOR_BLACK_ON_CYAN));
	setCDKMentryBackgroundAttrib(imagesList, COLOR_PAIR(COLOR_BLACK_ON_CYAN));
	
	xray_list_update(d, node);
	
	struct xray_list_preHandler_s t = {d, node};
	setCDKSelectionPreProcess(imagesList, xray_list_preHandler, &t);
		
	int ret = 
		activateCDKSelection(imagesList, NULL);
	
	destroyCDKSelection(imagesList);
	xray_list_free(d);
	
	screen_destroy_screen_xray(d);
}
