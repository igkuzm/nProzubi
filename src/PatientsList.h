/**
 * File              : PatientsList.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 02.06.2023
 * Last Modified Date: 02.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef PATIENTS_LIST_H
#define PATIENTS_LIST_H

#include <cdk.h>
#include "delegate.h"

CDKSELECTION *
patients_list_create(
		delegate_t *d
		);

#endif /* ifndef PATIENTSLIST_H */
