/**
 * File              : PatientsList.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 02.06.2023
 * Last Modified Date: 27.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef PATIENTS_LIST_H
#define PATIENTS_LIST_H

#include <cdk.h>
#include "delegate.h"
#include "ncwidgets/src/nclist.h"

//CDKSELECTION *
//patients_list_create(
		//delegate_t *d
		//);

typedef struct patients_list {
	delegate_t *d;
	//CDKSELECTION *s;
	nclist_t *s;
	struct passport_t **patients;
	char **titles;
	int count;
} patients_list_t;

patients_list_t *
patients_list_create(
		delegate_t *d
		);

void patinets_list_update(patients_list_t *t);

#endif /* ifndef PATIENTSLIST_H */
