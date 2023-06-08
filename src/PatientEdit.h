/**
 * File              : PatientEdit.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 22.05.2023
 * Last Modified Date: 02.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef PATIENT_EDIT_H
#define PATIENT_EDIT_H

#include "delegate.h"
#include "prozubilib/prozubilib.h"

void
patient_edit_create(
		delegate_t *d,
		struct passport_t *patient
		);
#endif /* ifndef PATIENT_EDIT_H */
