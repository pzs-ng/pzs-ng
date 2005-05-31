#include <stdio.h>

#include "handling.h"
#include "handle_any.h"

#include "zsfunctions.h"
#include "convert.h"
#include "errors.h"

int
handle_any(HANDLER_ARGS *ha)
{

	d_log(1, "handle_any: Using default handler for extension %s\n", ha->fileext);
	
	if (strcomp(allowed_types, ha->fileext) && !matchpath(allowed_types_exemption_dirs, ha->g->l.path)) {
		d_log(1, "handle_any: File type: NO CHECK\n");

		printf(zipscript_any_ok);
		printf("%s", convert(&ha->g->v, ha->g->ui, ha->g->gi, zipscript_footer_skip));
		
		if (matchpath(speedtest_dirs, ha->g->l.path)) {
			d_log(1, "zipscript-c: writing speedtest to channel\n");
			writelog(ha->g, convert(&ha->g->v, ha->g->ui, ha->g->gi, speed_announce), speed_type);
			return 2;
		}
		
		return 0;
	}

	if (!strcomp(ignored_types, ha->fileext)) {
		return def_sfv_handler(ha);
	}

	d_log(1, "handle_any: File type: UNKNOWN [ignored in sfv]\n");
	sprintf(ha->g->v.misc.error_msg, UNKNOWN_FILE, ha->fileext);
	mark_as_bad(ha->g->v.file.name);
	ha->msg->error = convert(&ha->g->v, ha->g->ui, ha->g->gi, bad_file_msg);
	writelog(ha->g, ha->msg->error, bad_file_disallowed_type);
	
	return 2;

}

