#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

#include "handle_nfo.h"
#include "zsfunctions.h"
#include "convert.h"
#include "race-file.h"
#include "errors.h"

int
handle_nfo(GLOBAL *g, MSG *msg) {
	
	//int	no_check = TRUE;
	int	exit_value = EXIT_SUCCESS;

	d_log(1, "handle_nfo: File type is: NFO\n");
#if ( deny_double_nfo )
	if (findfileextcount(".", ".nfo") > 1) {
		sprintf(g->v.misc.error_msg, DUPE_NFO);
		msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
		if (exit_value < 2)
			writelog(g, msg->error, bad_file_nfo_type);
		if (!deny_double_nfo_warn) {
			d_log(1, "handle_nfo: Looks like there already is a nfo uploaded. Denying this one.\n");
			exit_value = 2;
		} else
			d_log(1, "handle_nfo: Looks like there already is a nfo uploaded. Warn on -allowing anyway.\n");
	}
#endif
#if ( deny_nfo_upload_in_zip )
	if (matchpath(zip_dirs, g->l.path) && !matchpath(sfv_dirs, g->l.path) && !matchpath(group_dirs, g->l.path)) {
		d_log(1, "handle_nfo: nfo-files not allowed in zip_dirs.\n");
		sprintf(g->v.misc.error_msg, NFO_DENIED);
		msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
		if (exit_value < 2)
			writelog(g, msg->error, bad_file_nfodenied_type);
		exit_value = 2;
	}

#endif
	return exit_value;
}
