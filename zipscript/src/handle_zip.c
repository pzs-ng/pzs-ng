#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

#include "handle_zip.h"
#include "zsfunctions.h"
#include "convert.h"
#include "race-file.h"
#include "errors.h"

int
handle_zip(GLOBAL *g, MSG *msg) {

	unsigned char	exit_value = EXIT_SUCCESS;
	char		target[strlen(unzip_bin) + 10 + NAME_MAX];
	char 		*fileext;
	long		loc;

	d_log(1, "handle_zip: File type is: ZIP\n");
	d_log(1, "handle_zip: Testing file integrity with %s\n", unzip_bin);
	if (!fileexists(unzip_bin)) {
		d_log(1, "handle_zip: ERROR! Not able to check zip-files - %s does not exists!\n", unzip_bin);
		sprintf(g->v.misc.error_msg, BAD_ZIP);
		mark_as_bad(g->v.file.name);
		msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
		if (exit_value < 2)
			writelog(g, msg->error, bad_file_zip_type);
		exit_value = 2;
		return exit_value;
	} else {
#if (test_for_password || extract_nfo)
		if ((!findfileextcount(".", ".nfo") || findfileextcount(".", ".zip") == 1) && !mkdir(".unzipped", 0777)) {
			sprintf(target, "%s -qqjo \"%s\" -d .unzipped", unzip_bin, g->v.file.name);
		} else
			sprintf(target, "%s -qqt \"%s\"", unzip_bin, g->v.file.name);
#else
		sprintf(target, "%s -qqt \"%s\"", unzip_bin, g->v.file.name);
#endif
		if (execute(target) != 0) {
			d_log(1, "handle_zip: Integrity check failed (#%d): %s\n", errno, strerror(errno));
			sprintf(g->v.misc.error_msg, BAD_ZIP);
			mark_as_bad(g->v.file.name);
			msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
			if (exit_value < 2)
				writelog(g, msg->error, bad_file_zip_type);
			exit_value = 2;
			return exit_value;
		}
#if (test_for_password || extract_nfo || zip_clean)
			if (!findfileextcount(".", ".nfo") || findfileextcount(".", ".zip") == 1) {
				if (check_zipfile(".unzipped", g->v.file.name)) {
					d_log(1, "handle_zip: File is password protected.\n");
					sprintf(g->v.misc.error_msg, PASSWORD_PROTECTED);
					msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
					if (exit_value < 2)
						writelog(g, msg->error, bad_file_password_type);
					exit_value = 2;
					return exit_value;
				}
			}
#endif
	}
	d_log(1, "handle_zip: Integrity ok\n");
	printf(zipscript_zip_ok);

	if ((matchpath(zip_dirs, g->l.path)) || (matchpath(group_dirs, g->l.path))  ) {
		d_log(1, "handle_zip: Directory matched with zip_dirs/group_dirs\n");
	} else {
		d_log(1, "handle_zip: WARNING! Directory did not match with zip_dirs/group_dirs\n");
		if (strict_path_match == TRUE) {
			d_log(1, "handle_zip: Strict mode on - exiting\n");
			sprintf(g->v.misc.error_msg, UNKNOWN_FILE, fileext);
			mark_as_bad(g->v.file.name);
			msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
			if (exit_value < 2)
				writelog(g, msg->error, bad_file_wrongdir_type);
			exit_value = 2;
			return exit_value;
		}
	}
	if (!fileexists("file_id.diz")) {
		d_log(1, "handle_zip: file_id.diz does not exist, trying to extract it from %s\n", g->v.file.name);
		sprintf(target, "%s -qqjnCLL \"%s\" file_id.diz 2>.delme", unzip_bin, g->v.file.name);
		if (execute(target) != 0)
			d_log(1, "handle_zip: No file_id.diz found (#%d): %s\n", errno, strerror(errno));
		else {
			if ((loc = findfile(".", "file_id.diz.bad")))
				remove_at_loc(".", loc);
			chmod("file_id.diz", 0666);
		}
	}
	d_log(1, "handle_zip: Reading diskcount from diz:\n");
	g->v.total.files = read_diz("file_id.diz");
	d_log(1, "handle_zip:    Expecting %d files.\n", g->v.total.files);

	if (g->v.total.files == 0) {
		d_log(1, "handle_zip:    Could not get diskcount from diz.\n");
		g->v.total.files = 1;
		unlink("file_id.diz");
	}
	g->v.total.files_missing = g->v.total.files;

	d_log(1, "handle_zip: Storing new race data\n");
	writerace(g->l.race, &g->v, 0, F_CHECKED);
	d_log(1, "handle_zip: Reading race data from file to memory\n");
	readrace(g->l.race, &g->v, g->ui, g->gi);
	if (g->v.total.files_missing < 0) {
		d_log(1, "handle_zip: There seems to be more files in zip than we expected\n");
		g->v.total.files -= g->v.total.files_missing;
		g->v.total.files_missing = 0;
		g->v.misc.write_log = FALSE;
	}
	d_log(1, "handle_zip: Setting message pointers\n");
	msg->race = zip_race;
	msg->update = zip_update;
	msg->halfway = CHOOSE2(g->v.total.users, zip_halfway, zip_norace_halfway);
	msg->newleader = zip_newleader;

	return exit_value;
}
