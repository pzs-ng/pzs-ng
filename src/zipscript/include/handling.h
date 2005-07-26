/* "handlers" are functions which are executed for a
 * given file extension
 *
 * TODO: the extension and its handler should be defined
 * the same place/file, to make plugin writing easy
 */

#ifndef __HANDLING_H
#define __HANDLING_H

#include "objects.h"

/* this is what is passed to a handler */
typedef struct _handler_args {

	GLOBAL *g;
	MSG *msg;
	char **argv;
	char *fileext;
	int *deldir;
	
} HANDLER_ARGS;

/* the handler function type */
typedef int handler_t(HANDLER_ARGS *);

/* how file extensions are defined. each 'type' has a matching
 * 'ext' which is a regular expression used for matching aa
 * file extension.
 */
typedef struct _fext {
	short type;
	char *ext;
} FEXT;

/* handlers are defined with a type and matching handler function
 */
typedef struct _handler {
	short type;
	handler_t *handler;
} HANDLER;

extern FEXT fexts[];
extern HANDLER handlers[];

/* get the type of a file extension */
extern short get_filetype(char *);
/* return the handler for a specified type */
extern handler_t *get_handler(short);

#endif /* __HANDLING_H */

