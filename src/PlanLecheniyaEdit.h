/**
 * File              : PlanLecheniyaEdit.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 02.06.2023
 * Last Modified Date: 03.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef PLAN_LECHENIYA_EDIT
#define PLAN_LECHENIYA_EDIT

#include "delegate.h"
#include "prozubilib/prozubilib.h"

void 
plan_lecheniya_edit_create(
			delegate_t *d,
			struct case_t *c,
			int stageIndex,
			int itemIndex,
			const char *title,
			const char *price,
			const char *count,
			const char *kod
		);
#endif /* ifndef PLAN_LECHENIYA_EDIT */
