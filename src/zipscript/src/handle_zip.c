#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

#include "handle_zip.h"
#include "zsfunctions.h"
#include "convert.h"
#include "race-file.h"
#include "errors.h"
#include "dizreader.h"

void bad_file(HANDLER_ARGS *, char *, char *);

int
handle_zip(HANDLER_ARGS *ha) {

	int	ret;
	char	target[strlen(unzip_bin) + 10 + NAME_MAX];
	long	loc;

	d_log(1, "handle_zip: File type is: ZIP - Testing file integrity with %s\n", unzip_bin);
	
	if (!fileexists(unzip_bin)) {
		
		d_log(1, "handle_zip: ERROR! Not able to check zip-files - %s does not exist!\n", unzip_bin);
		bad_file(ha, BAD_ZIP, bad_file_zip_type);
		return 2;
		
	} else {
		
#if (test_for_password || extract_nfo)
		if ((!findfileextcount(".", ".nfo") ||
		     findfileextcount(".", ".zip") == 1) &&
		     !mkdir(".unzipped", 0777))
			ret = execute(5, unzip_bin, "-qqjo", ha->g->v.file.name, "-d", ".unzipped");
		else
			ret = execute(3, unzip_bin, "-qqt", ha->g->v.file.name);
#else
		ret = execute(3, unzip_bin, "-qqt", ha->g->v.file.name);
#endif
		
		if (ret != 0) {
			d_log(1, "handle_zip: Integrity check failed (#%d): %s\n", errno, strerror(errno));
			bad_file(ha, BAD_ZIP, bad_file_zip_type);
			return 2;
		}
		
#if (test_for_password || extract_nfo || zip_clean)
			if (!findfileextcount(".", ".nfo") || findfileextcount(".", ".zip") == 1) {
				if (check_zipfile(".unzipped", ha->g->v.file.name)) {
					d_log(1, "handle_zip: File is password protected.\n");
					bad_file(ha, PASSWORD_PROTECTED, bad_file_password_type);
					return 2;
				}
			}
#endif
			
	}
	
	d_log(1, "handle_zip: Integrity ok\n");
	
	printf(zipscript_zip_ok);

	if ((matchpath(zip_dirs, ha->g->l.path)) || (matchpath(group_dirs, ha->g->l.path)))
		d_log(1, "handle_zip: Directory matched with zip_dirs/group_dirs\n");
	else {
		d_log(1, "handle_zip: WARNING! Directory did not match with zip_dirs/group_dirs\n");
		
		if (strict_path_match == TRUE) {
			d_log(1, "handle_zip: Strict mode on - exiting\n");
			bad_file(ha, UNKNOWN_FILE, bad_file_wrongdir_type);
			return 2;
		}
		
	}
	
	if (!fileexists("file_id.diz")) {
		
		d_log(1, "handle_zip: file_id.diz does not exist, trying to extract it from %s\n", ha->g->v.file.name);
		
		ret = execute(4, unzip_bin, "-qqjnCLL", ha->g->v.file.name, "file_id.diz");
		if (ret != 0)
			d_log(1, "handle_zip: No file_id.diz found (#%d): %s\n", errno, strerror(errno));
		else {
			if ((loc = findfile(".", "file_id.diz.bad")))
				remove_at_loc(".", loc);
			chmod("file_id.diz", 0666);
		}
		
	}
	
	ha->g->v.total.files = read_diz("file_id.diz");

	if (ha->g->v.total.files == 0) {
		d_log(1, "handle_zip: Could not get diskcount from diz.\n");
		ha->g->v.total.files = 1;
		unlink("file_id.diz");
	} else
		d_log(1, "handle_zip: diz count: Expecting %d files.\n", ha->g->v.total.files);
	
	ha->g->v.total.files_missing = ha->g->v.total.files;

	d_log(1, "handle_zip: Storing new race data\n");
	writerace(ha->g->l.race, &ha->g->v, 0, F_CHECKED);
	
	d_log(1, "handle_zip: Reading race data from file to memory\n");
	readrace(ha->g->l.race, &ha->g->v, ha->g->ui, ha->g->gi);
	
	if (ha->g->v.total.files_missing < 0) {
		d_log(1, "handle_zip: There seems to be more files in zip than we expected\n");
		ha->g->v.total.files -= ha->g->v.total.files_missing;
		ha->g->v.total.files_missing = 0;
		ha->g->v.misc.write_log = FALSE;
	}
	
	d_log(1, "handle_zip: Setting message pointers\n");
	ha->msg->race = zip_race;
	ha->msg->update = zip_update;
	ha->msg->halfway = CHOOSE(ha->g->v.total.users, zip_halfway, zip_norace_halfway);
	ha->msg->newleader = zip_newleader;

	return EXIT_SUCCESS;
	
}

void
bad_file(HANDLER_ARGS *ha, char *error_msg, char *error_type)
{

	sprintf(ha->g->v.misc.error_msg, error_msg);
	ha->msg->error = convert(&ha->g->v, ha->g->ui, ha->g->gi, bad_file_msg);
	writelog(ha->g, ha->msg->error, error_type);
	
}
