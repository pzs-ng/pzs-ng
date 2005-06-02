/* This is where all available handler functions should
 * be included. 
 *
 * TODO: including a header for a handler function here
 * should be enough to enable it
 */
#ifndef __HANDLERS_H
#define __HANDLERS_H

/* file types, just add one at the bottom to be able to
 * use it */
enum {
	DEFAULT = 0,
	RAR,
	ZIP,
	SFV,
	NFO
	/*...*/
};

#include "handling.h"

#include "handle_zip.h"
#include "handle_sfv.h"
#include "handle_nfo.h"
#include "handle_any.h"
/* ... */

/* regular expressions for file types */
FEXT fexts[] = {	{ RAR, "[Rr][Aa][Rr]$" },
			{ RAR, "[Rr][0-9][0-9]$" },
			{ RAR, "[0-9][0-9][0-9]$" },
			{ ZIP, "[Zz][Ii][Pp]$" },
			{ SFV, "[Ss][Ff][Vv]$" },
			{ NFO, "[Nn][Ff][Oo]$" },
			{ DEFAULT, "" },
			/*...*/
			{ 0, 0 }
};

/* handlers and their types are defined here */
HANDLER handlers[] = {	{ RAR, handle_sfv32 }, 
			{ ZIP, handle_zip },
			{ SFV, handle_sfv },
			{ NFO, handle_nfo },
			{ DEFAULT, handle_any },
			/*...*/
			{ 0, 0 }
};

#endif /* __HANDLERS_H */

