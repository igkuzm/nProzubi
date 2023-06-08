/**
 * File              : PriceList.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 05.06.2023
 * Last Modified Date: 08.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef PRICE_LIST_H
#define PRICE_LIST_H
#include "delegate.h"
#include "prozubilib/prices.h"

void
price_list_create(
		delegate_t *d,
		void * userdata,
		void (*callback)(void *userdata, const struct price_t *price)
		);
	
#endif /* ifndef PRICE_LIST_H */
