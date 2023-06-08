/**
 * File              : CaseEdit.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 02.06.2023
 * Last Modified Date: 02.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef CASE_EDIT_H
#define CASE_EDIT_H

#include <cdk.h>
#include "delegate.h"

void
case_edit_plan_lecheniya_create(
		struct case_list_node *node,
		delegate_t *d);

void
case_edit_xray_create(
		struct case_list_node *node,
		delegate_t *d);

void
case_edit_zformula_create(
		struct case_list_node *node,
		delegate_t *d);

void
case_edit_date_create(
		struct case_list_node *node,
		delegate_t *d);

void
case_edit_combobox_create(
		struct case_list_node *node,
		delegate_t *d);

void
case_edit_text_create(
		struct case_list_node *node,
		delegate_t *d);

#endif /* ifndef CASE_EDIT_H */
