#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifndef HAVE_STRLCPY
# include "strlcpy.h"
#endif

#ifndef HAVE_STRSEP
# include "strsep.h"
#endif

#include "helpfunctions.h"
#include "zsfunctions.h"
#include "race-file.h"
#include "objects.h"
#include "macros.h"

#include "constants.h"

#include "errors.h"
#include "multimedia.h"
#include "convert.h"
#include "dizreader.h"
#include "stats.h"
#include "complete.h"
#include "crc.h"
#include "ng-version.h"
#include "processfuncs.h"
//#include "avi.h"
//#include "audio.h"
//#include "video.h"
//#include "avinfo.h"

//#include "racetypes.h"

//#include "handlers.h"
/*#include "handle_sfv.h"
#include "handle_nfo.h"
#include "handle_zip.h"*/

#include "zsconfig.h"
#include "zsconfig.defaults.h"

/*#ifdef _SunOS_
# include "scandir.h"
# include "strsep.h"
#endif*/

int 
main(int argc, char **argv)
{
	GLOBAL		g; /* this motherfucker owns */
	MSG		msg;
	GDATA		gdata;
	UDATA		udata;
	RACETYPE	rtype;
	
	char		*fileext = NULL, *name_p, *temp_p = NULL;
	char		*_complete[2] = { 0 }; /* 0 = bar, 1 = announce */
	int		exit_value = EXIT_SUCCESS;
	int		no_check = FALSE;
	int		n = 0;
	int		deldir = 0;
	struct stat	fileinfo;

#if ( benchmark_mode )
	struct timeval	bstart, bstop;
	d_log(1, "zipscript-c: Reading time for benchmark\n");
	gettimeofday(&bstart, (struct timezone *)0);
#endif

	if (argc != 4) {
		printf(" - - PZS-NG ZipScript-C v%s - -\n\nUsage: %s <filename> <path> <crc>\n\n", ng_version(), argv[0]);
		return EXIT_FAILURE;
	}

	if (!fileexists(argv[1])) {
		d_log(1, "zipscript-c: File %s does not exist. exiting.\n", argv[1]);
		return 1;
	}

#if ( debug_mode && debug_announce )
	printf("PZS-NG: Running in debug mode.\n");
#endif

	d_log(1, "zipscript-c: Project-ZS Next Generation (pzs-ng) v%s debug log.\n", ng_version());

#ifdef _ALT_MAX
	d_log(1, "zipscript-c: PATH_MAX not found - using predefined settings! Please report to the devs!\n");
#endif

	umask(0666 & 000);

	set_uid_gid();

	bzero(&g, sizeof(GLOBAL));

	g.v.misc.write_log = TRUE;
	g.v.misc.in_sfvfile = FALSE;

	strlcpy(g.v.file.name, argv[1], NAME_MAX);
	strlcpy(g.l.path, argv[2], PATH_MAX);
	strlcpy(g.v.misc.current_path, g.l.path, PATH_MAX);
	d_log(1, "zipscript-c: Changing directory to %s\n", g.l.path);
	chdir(g.l.path);

	d_log(1, "zipscript-c: Reading data from environment variables\n");
	read_envdata(&g, &gdata, &udata, &fileinfo);
	g.v.file.speed *= 1024;

	d_log(1, "zipscript-c: Checking the file size of %s\n", g.v.file.name);
	check_filesize(&g, g.v.file.name, &fileinfo);

	d_log(1, "zipscript-c: Setting race times\n");
	if (g.v.file.size != 0)
		g.v.total.start_time = g.v.total.stop_time - (g.v.file.size / g.v.file.speed);
	else
		g.v.total.start_time = g.v.total.stop_time > (g.v.total.stop_time - 1) ? g.v.total.stop_time : (g.v.total.stop_time -1);
		
	if ((g.v.total.stop_time - g.v.total.start_time) < 1)
		g.v.total.stop_time = g.v.total.start_time + 1;

	n = (g.l.length_path = strlen(g.l.path)) + 1;

	d_log(1, "zipscript-c: Allocating memory for variables\n");
	g.l.race = ng_realloc(g.l.race, n += 10 + (g.l.length_zipdatadir = sizeof(storage) - 1), 1, 1, 1);
	g.l.sfv = ng_realloc(g.l.sfv, n, 1, 1, 1);
	g.l.leader = ng_realloc(g.l.leader, n, 1, 1, 1);

	d_log(1, "zipscript-c: Copying data g.l into memory\n");
	sprintf(g.l.sfv, storage "/%s/sfvdata", g.l.path);
	sprintf(g.l.leader, storage "/%s/leader", g.l.path);
	sprintf(g.l.race, storage "/%s/racedata", g.l.path);
	g.v.user.pos = 0;
	snprintf(g.v.misc.old_leader, 24, "none");
	g.v.file.compression_method = '5';

	/* Get file extension */
	d_log(1, "zipscript-c: Parsing file extension from filename... (%s)\n", argv[1]);
	name_p = temp_p = find_last_of(argv[1], ".");
	
	if (*temp_p != '.') {
		d_log(1, "zipscript-c: Got: no extension\n");
		temp_p = name_p;
	} else {
		d_log(1, "zipscript-c: Got: %s\n", temp_p);
		temp_p++;
	}
	name_p++;

	fileext = ng_realloc(fileext, sizeof(name_p), 1, 1, 1);
	memcpy(fileext, name_p, sizeof(name_p));
	
#if ( sfv_cleanup_lowercase )
	d_log(1, "zipscript-c: Copying (lowercased version of) extension to memory\n");
	strtolower(fileext);
#else
	d_log(1, "zipscript-c: Copying (unchanged version of) extension to memory\n");
#endif

	d_log(1, "zipscript-c: Caching release name\n");
	getrelname(&g);

	d_log(1, "zipscript-c: Creating directory to store racedata in\n");
	maketempdir(g.l.path);

	printf(zipscript_header);

	g.v.misc.nfofound = (int)findfileext(".", ".nfo");

	/* Hide users in group_dirs */
	group_dir_users(&g);
	
	/* No check directories */
	no_check = match_nocheck_dirs(&g);

	if (!no_check) {

#if ( !ignore_zero_size )
		exit_value = check_zerosize(&g, &msg);
#endif

#if ( check_for_banned_files )
		exit_value = check_banned_file(&g, &msg);
#endif

		if (exit_value < 2) {
			d_log(1, "process_file: Verifying old racedata\n");
			if (!verify_racedata(g.l.race))
				d_log(1, "process_file:   Failed to open racedata - assuming this is a new race.\n");
			exit_value = process_file(&g, &msg, argv, fileext, &deldir);
		}
	}
	if (exit_value == EXIT_SUCCESS) { /* File was checked */
		check_release_type(&g, &msg, &rtype, _complete);
		if (g.v.total.users > 0) {
			d_log(1, "zipscript-c: Sorting race stats\n");
			sortstats(&g.v, g.ui, g.gi);

#if ( get_user_stats )
			d_log(1, "zipscript-c: Reading day/week/month/all stats for racers\n");
			d_log(1, "zipscript-c: stat section: %i\n", g.v.section);
			get_stats(&g.v, g.ui);
#endif
			showstats(&g.v, g.ui, g.gi);

			if (!enable_files_ahead || ((g.v.total.users > 1 && g.ui[g.ui[0].pos].files >= (g.ui[g.ui[1].pos].files + newleader_files_ahead)) || g.v.total.users == 1)) {
				d_log(1, "zipscript-c: Writing current leader to file\n");
				read_write_leader(g.l.leader, &g.v, &g.ui[g.ui[0].pos]);
			}
			if (g.v.total.users > 1) {
				if (g.ui[g.v.user.pos].files == 1 && msg.race != NULL) {
					d_log(1, "zipscript-c: Writing RACE to %s\n", log);
					writelog(&g, convert(&g.v, g.ui, g.gi, msg.race), rtype.race);
				}
				if (g.v.total.files >= min_newleader_files && ((g.v.total.size * g.v.total.files) >= (min_newleader_size * 1024 * 1024)) && strcmp(g.v.misc.old_leader, g.ui[g.ui[0].pos].name) && msg.newleader != NULL && g.ui[g.ui[0].pos].files >= (g.ui[g.ui[1].pos].files + newleader_files_ahead) && g.v.total.files_missing) {
					d_log(1, "zipscript-c: Writing NEWLEADER to %s\n", log);
					writelog(&g, convert(&g.v, g.ui, g.gi, msg.newleader), rtype.newleader);
				}
			} else {
				if (g.ui[g.v.user.pos].files == 1 && g.v.total.files >= min_update_files && ((g.v.total.size * g.v.total.files) >= (min_update_size * 1024 * 1024)) && msg.update) {
					d_log(1, "zipscript-c: Writing UPDATE to %s\n", log);
					writelog(&g, convert(&g.v, g.ui, g.gi, msg.update), rtype.update);
				}
			}
		}
		
		if (g.v.total.files_missing > 0)
			release_incomplete(&g, &msg, &rtype);
		else if ((g.v.total.files_missing == 0) && (g.v.total.files > 0))
			release_complete(&g, &msg, _complete);
		else {

			/* Release is at unknown state */
			g.v.total.files = -g.v.total.files_missing;
			g.v.total.files_missing = 0;
			printf("%s", convert(&g.v, g.ui, g.gi, zipscript_footer_unknown));

		}
	} else {
		/* File is marked to be deleted */
		d_log(1, "zipscript-c: Logging file as bad\n");
		remove_from_race(g.l.race, g.v.file.name);
		printf("%s", convert(&g.v, g.ui, g.gi, zipscript_footer_error));
	}

#if ( enable_accept_script )
	if (exit_value == EXIT_SUCCESS)
		execute_script(accept_script, g.v.file.name, "accept");
#endif

#if ( enable_nfo_script )
	 if ( (!g.v.misc.nfofound || !strncasecmp("nfo", fileext, 3)) && findfileext(".", ".nfo"))
		execute_script(nfo_script, g.v.file.name, "nfo");
#endif

	if ((findfileext(".", ".nfo") || (findfileext("..", ".nfo"))) && (g.l.nfo_incomplete)) {
		d_log(1, "zipscript-c: Removing missing-nfo indicator (if any)\n");
		remove_nfo_indicator(&g);
	}

#if ( del_banned_release )
	if (deldir) {
		d_log(1, "zipscript-c: del_banned_release is set - removing entire dir.\n");
		move_progress_bar(1, &g.v, g.ui, g.gi);
		if (g.l.incomplete)
			unlink(g.l.incomplete);
		del_releasedir(".", g.l.path);
	}
#endif

#if ( enable_banned_script )
	if (deldir)
		execute_script(banned_script, g.v.file.name, "banned");
#endif

	if (fileexists(".delme"))
		unlink(".delme");

	buffer_groups(&gdata, GROUPFILE, 1);
	buffer_users(&udata, PASSWDFILE, 1);
	ng_free(fileext);
	ng_free(g.l.race);
	ng_free(g.l.sfv);
	ng_free(g.l.leader);

#if ( benchmark_mode )
	gettimeofday(&bstop, (struct timezone *)0);
	printf("Checks completed in %0.6f seconds\n", ((bstop.tv_sec - bstart.tv_sec) + (bstop.tv_usec - bstart.tv_usec) / 1000000.));
	d_log(1, "zipscript-c: Checks completed in %0.6f seconds\n", ((bstop.tv_sec - bstart.tv_sec) + (bstop.tv_usec - bstart.tv_usec) / 1000000.));
#endif

#if ( sleep_on_bad > 0 && sleep_on_bad < 1001 )
	if (exit_value == 2) {
		if (sleep_on_bad == 117)
			exit_value = 126;
		else
			exit_value = sleep_on_bad + 10;
		d_log(1, "zipscript-c: Sleeping for %d seconds.\n", sleep_on_bad);
	}
#endif
	d_log(1, "zipscript-c: Exit %d\n", exit_value);
	return exit_value;
}

