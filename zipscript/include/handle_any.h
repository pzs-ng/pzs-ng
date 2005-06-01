#ifndef __HANDLE_ANY_H
#define __HANDLE_ANY_H

#include "handling.h"

/* default handler for sfv releases */
#ifndef def_sfv_handler
# define def_sfv_handler handle_sfv32
# include "handle_sfv.h"
#endif

int handle_any(HANDLER_ARGS *);

#endif

