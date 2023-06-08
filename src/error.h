/**
 * File              : error.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.05.2023
 * Last Modified Date: 01.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef ERROR_H
#define ERROR_H

void error_callback(void *userdata, const char * error);
void log_callback(void *userdata, const char * message);

#endif /* ifndef ERROR_H */
