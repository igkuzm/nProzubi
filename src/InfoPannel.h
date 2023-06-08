/**
 * File              : InfoPannel.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 29.05.2023
 * Last Modified Date: 02.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef INPO_PANNEL_H
#define INPO_PANNEL_H

#include <cdk.h>
#include "delegate.h"

void
info_pannel_set_text(
		delegate_t *d,
		const char *text
		);

CDKLABEL *
create_info_pannel(
		delegate_t *d,
		CDKSCREEN *screen
		);

#endif /* ifndef INPO_PANNEL_H */	
