/**
 * File              : Xray.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 16.05.2023
 * Last Modified Date: 01.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef XRAY_H
#define XRAY_H

#include <cdk.h>
#include "delegate.h"

void
xray_create(
		struct case_list_node *node,
		delegate_t *d);

#endif /* ifndef XRAY_H */
