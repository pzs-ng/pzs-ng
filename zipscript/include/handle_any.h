#ifndef __HANDLE_ANY_H
#define __HANDLE_ANY_H

#include "handling.h"

/* default handler for sfv releases */
#define def_sfv_handler handle_sfv32

int handle_any(HANDLER_ARGS *);

#endif

