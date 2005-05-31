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
handle_nfo(HANDLER_ARGS *ha) {
	
	//int	no_check = TRUE;
	int	exit_value = EXIT_SUCCESS;

	d_log(1, "handle_nfo: File type is: NFO\n");
#if ( deny_double_nfo )
	if (findfileextcount(".", ".nfo") > 1) {
		sprintf(ha->g->v.misc.error_msg, DUPE_NFO);
		ha->msg->error = convert(&ha->g->v, ha->g->ui, ha->g->gi, bad_file_msg);
		if (exit_value < 2)
			writelog(ha->g, ha->msg->error, bad_file_nfo_type);
		if (!deny_double_nfo_warn) {
			d_log(1, "handle_nfo: Looks like there already is a nfo uploaded. Denying this one.\n");
			exit_value = 2;
		} else
			d_log(1, "handle_nfo: Looks like there already is a nfo uploaded. Warn on -allowing anyway.\n");
	}
#endif
#if ( deny_nfo_upload_in_zip )
	if (matchpath(zip_dirs, ha->g->l.path) && !matchpath(sfv_dirs, ha->g->l.path) && !matchpath(group_dirs, ha->g->l.path)) {
		d_log(1, "handle_nfo: nfo-files not allowed in zip_dirs.\n");
		sprintf(ha->g->v.misc.error_msg, NFO_DENIED);
		ha->msg->error = convert(&ha->g->v, ha->g->ui, ha->g->gi, bad_file_msg);
		if (exit_value < 2)
			writelog(ha->g, ha->msg->error, bad_file_nfodenied_type);
		exit_value = 2;
	}

#endif
	return exit_value;
}
