/**
 * File              : CasesList.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 02.06.2023
 * Last Modified Date: 06.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef CASES_LIST_H
#define CASES_LIST_H

#include <cdk.h>
#include "delegate.h"
#include "prozubilib/prozubilib.h"

void
cases_list_create(delegate_t *d, struct passport_t *patient);

#endif /* ifndef CASES_LIST_H */
