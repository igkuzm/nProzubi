/**
 * File              : input.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 02.06.2023
 * Last Modified Date: 03.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef INPUT_H
#define INPUT_H
#include <cdk.h>

CDKOBJS *input_switch_focus(CDKOBJS *new, CDKOBJS *old);

int input_escape_handler(EObjectType cdktype GCC_UNUSED, void *object, 
		void *clientData GCC_UNUSED, chtype input GCC_UNUSED);

int input_mouse_handler(EObjectType cdktype GCC_UNUSED, void *object, 
		void *clientData GCC_UNUSED, chtype input GCC_UNUSED);
#endif /* ifndef INPUT_H */

