#ifndef __HANDLING_H
#define __HANDLING_H

/* all handler functions should take the same arguments
 * and their calls should be determined by a file extension
 */

#include "objects.h"

#define def_sfv_handler handle_sfv32

typedef struct _handler_args {

	GLOBAL *g;
	MSG *msg;
	char **argv;
	char *fileext;
	int *deldir;
	
} HANDLER_ARGS;

typedef int handler_t(HANDLER_ARGS *);

typedef struct _fext {
	char *ext;
	short type;
} FEXT;

typedef struct _handler {
	short type;
	handler_t *handler;
} HANDLER;

/* regular expressions for file types */

extern FEXT fexts[];
extern HANDLER handlers[];

extern short get_filetype(char *);
extern handler_t *get_handler(short);

#include "handlers.h"

#endif /* __HANDLING_H */

