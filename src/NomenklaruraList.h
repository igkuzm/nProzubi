/**
 * File              : NomenklaruraList.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 06.06.2023
 * Last Modified Date: 06.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef NOMENKLATURA_LIST_H
#define NOMENKLATURA_LIST_H

#include "cdk.h"
#include "delegate.h"

void
nomenklatura_list_create(
		delegate_t *d,
		void *userdata,
		void callback(
			void *userdata,
			const char * title,
			const char *kod));
#endif /* ifndef NOMENKLATURA_LIST_H */
