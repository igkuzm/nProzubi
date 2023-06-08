/**
 * File              : TextUTF8Handler.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 29.05.2023
 * Last Modified Date: 08.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef TEXT_UTF8_HANDLER
#define TEXT_UTF8_HANDLER

#include <cdk.h>

int 
mentry_text_preHandler (EObjectType cdktype GCC_UNUSED, void *object,
		       void *clientData GCC_UNUSED,
		       chtype input GCC_UNUSED);

int 
entry_text_preHandler (EObjectType cdktype GCC_UNUSED, void *object,
		       void *clientData GCC_UNUSED,
		       chtype input GCC_UNUSED);
int 
screen_update_postHandler (EObjectType cdktype GCC_UNUSED, void *object,
		       void *clientData GCC_UNUSED,
		       chtype input GCC_UNUSED);


#endif /* ifndef TEXT_UTF8_HANDLER */
