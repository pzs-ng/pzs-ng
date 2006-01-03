#include <stdio.h>

#include "processfuncs.h"
#include "macros.h"
#include "complete.h"
#include "handlers.h"
#include "errors.h"
#include "race-file.h"
#include "helpfunctions.h"
#include "convert.h"

#ifndef HAVE_STRLCPY_H
# include "strlcpy.h"
#endif

void
read_envdata(GLOBAL *g, GDATA *gdata, UDATA *udata, struct stat *fileinfo)
{
	
	int		n;
	char		*temp_p = NULL, *temp_p_free = NULL;

	buffer_groups(gdata, GROUPFILE, 0);
	buffer_users(udata, PASSWDFILE, 0);
	
	if ((getenv("USER") == NULL) || (getenv("GROUP") == NULL) ||
	    (getenv("TAGLINE") == NULL) || (getenv("SPEED") ==NULL) ||
	    (getenv("SECTION") == NULL)) {
		d_log(1, "read_envdata: We are running from shell, falling back to default values for $USER, $GROUP, $TAGLINE, $SECTION and $SPEED\n");
		fileinfo->st_uid = geteuid();
		fileinfo->st_gid = getegid();
		strlcpy(g->v.user.name, get_u_name(udata, fileinfo->st_uid), 24);
		strlcpy(g->v.user.group, get_g_name(gdata, fileinfo->st_gid), 24);
		memcpy(g->v.user.tagline, "No Tagline Set", 15);
		g->v.file.speed = 2005;
		g->v.section = 0;
		sprintf(g->v.sectionname, "DEFAULT");
	} else {
		sprintf(g->v.user.name, getenv("USER"));
		sprintf(g->v.user.group, getenv("GROUP"));
		if (!(int)strlen(g->v.user.group))
			memcpy(g->v.user.group, "NoGroup", 8);
		sprintf(g->v.user.tagline, getenv("TAGLINE"));
		if (!(int)strlen(g->v.user.tagline))
			memcpy(g->v.user.tagline, "No Tagline Set", 15);
		g->v.file.speed = (unsigned int)strtol(getenv("SPEED"), NULL, 0);
		if (!g->v.file.speed)
			g->v.file.speed = 1;

#if ( debug_announce )
		printf("DEBUG: Speed: %dkb/s (%skb/s)\n",  g->v.file.speed, getenv("SPEED"));
#endif

		d_log(1, "read_envdata: Reading section from env (%s)\n", getenv("SECTION"));
		snprintf(g->v.sectionname, 127, getenv("SECTION"));
		g->v.section = 0;
		temp_p_free = temp_p = strdup((const char *)gl_sections);	/* temp_p_free is needed since temp_p is modified by strsep */
		if (temp_p == NULL) {
			d_log(1, "read_envdata: Can't allocate memory for sections\n");
		} else {
			n = 0;
			while (temp_p) {
				if (!strcmp(strsep(&temp_p, " "), getenv("SECTION"))) {
					g->v.section = (unsigned char)n;
					break;
				} else
					n++;
			}
			ng_free(temp_p_free);
		}
	}
}

void
check_filesize(GLOBAL *g, const char *filename, struct stat *fileinfo)
{

	if (stat(filename, fileinfo)) {
		d_log(1, "check_filesize: Failed to stat file: %s\n", strerror(errno));
		g->v.file.size = 0;
		g->v.total.stop_time = 0;
	} else {
		g->v.file.size = fileinfo->st_size;
		d_log(1, "check_filesize: File size was: %d\n", g->v.file.size);
		g->v.total.stop_time = fileinfo->st_mtime;
	}

}

void
set_uid_gid(void)
{

	if ( program_uid > 0 ) {
		d_log(1, "set_uid_gid: Trying to change effective uid/gid\n");
		setegid(program_gid);
		seteuid(program_uid);
	} else if (!geteuid()) {
		d_log(1, "set_uid_gid: +s mode detected - trying to change effective uid/gid to !root\n");
		if (setegid(getgid()) == -1)
			d_log(1, "set_uid_gid: failed to change gid: %s\n", strerror(errno));
		if (seteuid(getuid()) == -1)
			d_log(1, "set_uid_gid: failed to change uid: %s\n", strerror(errno));
	}

}

void
group_dir_users(GLOBAL *g)
{

	if (matchpath(group_dirs, g->l.path) && hide_group_uploaders) {
		d_log(1, "group_dir_users: Hiding user in group-dir:\n");
		if (strlen(hide_gname) > 0) {
			snprintf(g->v.user.group, 18, "%s", hide_gname);
			d_log(1, "group_dir_users:    Changing groupname\n");
		}
		if (strlen(hide_uname) > 0) {
			snprintf(g->v.user.name, 18, "%s", hide_uname);
			d_log(1, "group_dir_users:    Changing username\n");
		}
		if (strlen(hide_uname) == 0) {
			d_log(1, "group_dir_users:    Making username = groupname\n");
			snprintf(g->v.user.name, 18, "%s", g->v.user.group);
		}
	}

}

int
match_nocheck_dirs(GLOBAL *g)
{

	if (matchpath(nocheck_dirs, g->l.path) || matchpath(speedtest_dirs, g->l.path) ||
		(!matchpath(zip_dirs, g->l.path) && !matchpath(sfv_dirs, g->l.path) &&
		!matchpath(group_dirs, g->l.path))) {
		
		d_log(1, "match_nocheck_dirs: Directory matched with nocheck_dirs, or is not among sfv/zip/group dirs\n");
		d_log(1, "match_nocheck_dirs:   - nocheck_dirs  : '%s'\n", nocheck_dirs);
		d_log(1, "match_nocheck_dirs:   - speedtest_dirs: '%s'\n", speedtest_dirs);
		d_log(1, "match_nocheck_dirs:   - zip_dirs      : '%s'\n", zip_dirs);
		d_log(1, "match_nocheck_dirs:   - sfv_dirs      : '%s'\n", sfv_dirs);
		d_log(1, "match_nocheck_dirs:   - group_dirs    : '%s'\n", group_dirs);
		d_log(1, "match_nocheck_dirs:   - current path  : '%s'\n", g->l.path);
		
		return TRUE;

	} else
		return FALSE;

}

int
check_zerosize(GLOBAL *g, MSG *msg)
{
	
	if (g->v.file.size == 0) {
		d_log(1, "check_zerosize: File seems to be 0 bytes\n");
		sprintf(g->v.misc.error_msg, EMPTY_FILE);
		mark_as_bad(g->v.file.name);
		msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
		writelog(g, msg->error, bad_file_0size_type);
		return 2;
	} else
		return EXIT_SUCCESS;

}

int
check_banned_file(GLOBAL *g, MSG *msg)
{
	
	if (filebanned_match(g->v.file.name)) {
		d_log(1, "check_banned_file: Banned file detected (%s)\n", g->v.file.name);
		sprintf(g->v.misc.error_msg, BANNED_FILE);
		mark_as_bad(g->v.file.name);
		msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
		writelog(g, msg->error, bad_file_disallowed_type);
		return 2;
	} else
		return EXIT_SUCCESS;

}

/* fuck you, arguments */
int
process_file(GLOBAL *g, MSG *msg, char **argv, char *fileext, int *deldir)
{
	
	short type;
	handler_t *handler;
	HANDLER_ARGS ha;
	
	type = get_filetype(fileext);
	handler = get_handler(type);
		
	ha.g = g;
	ha.msg = msg;
	ha.argv = argv;
	ha.fileext = fileext;
	ha.deldir = deldir;
	
	return handler(&ha);
	
}

void
check_release_type(GLOBAL *g, MSG *msg, RACETYPE *rtype, char *_complete[2])
{

	switch (g->v.misc.release_type) {
		case RTYPE_RAR:
			set_rtype_msg(rtype, &__racetype_rar);
			_complete[0] = rar_completebar;
			_complete[1] = CHOOSE(g->v.total.users, rar_announce_race_complete_type, rar_announce_norace_complete_type);
			msg->complete = CHOOSE(g->v.total.users, rar_complete, rar_norace_complete);
			break;	/* rar */
		case RTYPE_OTHER:
			set_rtype_msg(rtype, &__racetype_other);
			_complete[0] = other_completebar;
			_complete[1] = CHOOSE(g->v.total.users, other_announce_race_complete_type, other_announce_norace_complete_type);
			msg->complete = CHOOSE(g->v.total.users, other_complete, other_norace_complete);
			break;	/* other */
		case RTYPE_AUDIO:
			_complete[0] = audio_completebar;
			if (!g->v.audio.is_vbr) {
				set_rtype_msg(rtype, &__racetype_audio_vbr);
				_complete[1] = CHOOSE(g->v.total.users, audio_cbr_announce_race_complete_type, audio_cbr_announce_norace_complete_type);
			} else {
				set_rtype_msg(rtype, &__racetype_audio_cbr);
				_complete[1] = CHOOSE(g->v.total.users, audio_vbr_announce_race_complete_type, audio_vbr_announce_norace_complete_type);
			}
			msg->complete = CHOOSE(g->v.total.users, audio_complete, audio_norace_complete);
			break;	/* audio */
		case RTYPE_VIDEO:
			set_rtype_msg(rtype, &__racetype_video);
			_complete[0] = video_completebar;
			_complete[1] = CHOOSE(g->v.total.users, video_announce_race_complete_type, video_announce_norace_complete_type);
			msg->complete = CHOOSE(g->v.total.users, video_complete, video_norace_complete);
			break;	/* video */
		case RTYPE_NULL:
			set_rtype_msg(rtype, &__racetype_zip);
			_complete[0] = zip_completebar;
			_complete[1] = CHOOSE(g->v.total.users, zip_announce_race_complete_type, zip_announce_norace_complete_type);
			msg->complete = CHOOSE(g->v.total.users, zip_complete, zip_norace_complete);
			break;	/* zip */
		default:
			set_rtype_msg(rtype, &__racetype_rar);
			_complete[0] = rar_completebar;
			_complete[1] = CHOOSE(g->v.total.users, rar_announce_race_complete_type, rar_announce_norace_complete_type);
			msg->complete = CHOOSE(g->v.total.users, rar_complete, rar_norace_complete);
			d_log(1, "check_release_type: WARNING! Not a known release type - Contact the authors! (3:%d)\n", g->v.misc.release_type);
			break;	/* rar */
	}

}

void
execute_script(char *script, char *arg, char *type)
{

	if (!fileexists(script))
		d_log(1, "execute_script: Warning - %s_script (%s) - file does not exist\n", type, script);

	d_log(1, "execute_script: Executing %s script\n", type);
	
	if (execute(2, script, arg) != 0)
		d_log(1, "execute_script: Failed to execute %s_script: %s\n", type, strerror(errno));
		
}

int
get_nfo_filetype(unsigned int type)
{

	switch (type) {
		case RTYPE_RAR:
			if (strcomp(check_for_missing_nfo_filetypes, "rar"))
				return 1;
		case RTYPE_OTHER:
			if (strcomp(check_for_missing_nfo_filetypes, "other"))
				return 1;
		case RTYPE_AUDIO:
			if (strcomp(check_for_missing_nfo_filetypes, "audio"))
				return 1;
		case RTYPE_VIDEO:
			if (strcomp(check_for_missing_nfo_filetypes, "video"))
				return 1;
		case RTYPE_NULL:
			if (strcomp(check_for_missing_nfo_filetypes, "zip"))
				return 1;
	}

	return 0;
}

void
create_missing_missing_nfo_indicator(GLOBAL *g, char *inc_point[2])
{

	if (!g->l.in_cd_dir) {
		d_log(1, "create_missing_missing_nfo_ind: Creating missing-nfo indicator %s\n", g->l.nfo_incomplete);
		create_incomplete_nfo2();
	} else if (!findfileext("..", ".nfo")) {
		d_log(1, "create_missing_missing_nfo_ind: Creating missing-nfo indicator (base) %s\n", g->l.nfo_incomplete);
		/* This is not pretty, but should be functional. */
		if ((inc_point[0] = find_last_of(g->l.path, "/")) != g->l.path)
			*inc_point[0] = '\0';
		if ((inc_point[1] = find_last_of(g->v.misc.release_name, "/")) != g->v.misc.release_name)
			*inc_point[1] = '\0';
		create_incomplete_nfo2();
		if (*inc_point[0] == '\0')
			*inc_point[0] = '/';
		if (*inc_point[1] == '\0')
			*inc_point[1] = '/';
	}
}

void
release_complete(GLOBAL *g, MSG *msg, char *_complete[2])
{

	int	n = 0, cnt;
	char	*inc_point[2];
	char	target[NAME_MAX];
	char	*temp_p = 0;

	d_log(1, "release_complete: Caching progress bar\n");
	buffer_progress_bar(&g->v);
	
	printf("%s", convert(&g->v, g->ui, g->gi, zipscript_footer_ok));
	
	d_log(1, "release_complete: Setting complete pointers\n");
	if ( g->v.misc.release_type == RTYPE_AUDIO ) {
		
		d_log(1, "release_complete: Symlinking audio\n");
		if (!strncasecmp(g->l.link_target, "VA", 2) && (g->l.link_target[2] == '-' || g->l.link_target[2] == '_'))
			memcpy(g->v.audio.id3_artist, "VA", 3);

		if (!matchpath(group_dirs, g->l.path)) {

#if ( audio_genre_sort )
			d_log(1, "release_complete:   Sorting mp3 by genre (%s)\n", g->v.audio.id3_genre);
			if (g->v.audio.id3_genre)
				createlink(audio_genre_path, g->v.audio.id3_genre, g->l.link_source, g->l.link_target);
#endif

#if ( audio_artist_sort )
			d_log(1, "release_complete:   Sorting mp3 by artist\n");
			if (g->v.audio.id3_artist) {
				d_log(1, "release_complete:     - artist: %s\n", g->v.audio.id3_artist);
				if (memcmp(g->v.audio.id3_artist, "VA", 3)) {
					temp_p = ng_realloc(temp_p, 2, 1, 1, 1);
					snprintf(temp_p, 2, "%c", toupper(*g->v.audio.id3_artist));
					createlink(audio_artist_path, temp_p, g->l.link_source, g->l.link_target);
					ng_free(temp_p);
				} else
					createlink(audio_artist_path, "VA", g->l.link_source, g->l.link_target);
			}
#endif

#if ( audio_year_sort )
			d_log(1, "release_complete:   Sorting mp3 by year (%s)\n", g->v.audio.id3_year);
			if (g->v.audio.id3_year)
				createlink(audio_year_path, g->v.audio.id3_year, g->l.link_source, g->l.link_target);
#endif

#if ( audio_group_sort )
			d_log(1, "release_complete:   Sorting mp3 by group\n");
			temp_p = remove_pattern(g->l.link_target, "*-", RP_LONG_LEFT);
			temp_p = remove_pattern(temp_p, "_", RP_SHORT_LEFT);
			n = strlen(temp_p);
			if (n > 0 && n < 15) {
				d_log(1, "release_complete:   - Valid groupname found: %s (%i)\n", temp_p, n);
				createlink(audio_group_path, temp_p, g->l.link_source, g->l.link_target);
			}
#endif

		}
		
#if ( create_m3u )
		if (findfileext(".", ".sfv")) {
			d_log(1, "release_complete: Creating m3u\n");
			cnt = sprintf(target, findfileext(".", ".sfv"));
			strlcpy(target + cnt - 3, "m3u", 4);
			create_indexfile(g->l.race, &g->v, target);
		} else
			d_log(1, "release_complete: Cannot create m3u, sfv is missing\n");
#endif

	}
	
	if (_complete[0]) {
		d_log(1, "release_complete: Removing old complete bar, if any\n");
		removecomplete();
	}

	d_log(1, "release_complete: Removing incomplete indicator (%s)\n", g->l.incomplete);
	complete(g);

	if (msg->complete != NULL) {
		d_log(1, "release_complete: Writing COMPLETE and STATS to %s\n", log);
		writelog(g, convert(&g->v, g->ui, g->gi, msg->complete), _complete[1]);
	}
	
	if (_complete[0]) {
		d_log(1, "release_complete: Creating complete bar\n");
		createstatusbar(convert(&g->v, g->ui, g->gi, _complete[0]));
		
#if (chmod_completebar)
		if (!matchpath(group_dirs, g->l.path))
			chmod(convert(&g->v, g->ui, g->gi, _complete[0]), 0222);
		else
			d_log(1, "release_complete: we are in a group_dir - will not chmod the complete bar.\n");
#endif

	}

#if ( enable_complete_script )
	execute_script(complete_script, g->v.file.name, "complete");
#endif

	if (!matchpath(group_dirs, g->l.path) || create_incomplete_links_in_group_dirs) {
		/* Creating no-nfo link if needed. */
		n = 0;
		
		if (check_for_missing_nfo_filetypes)
			n = get_nfo_filetype(g->v.misc.release_type);

		if ((g->l.nfo_incomplete) && (!findfileext(".", ".nfo")) &&
			(matchpath(check_for_missing_nfo_dirs, g->l.path) || n)) {
			
			create_missing_missing_nfo_indicator(g, inc_point);
		}

	}

}

void
release_incomplete(GLOBAL *g, MSG *msg, RACETYPE *rtype)
{

	if (g->v.total.files_missing == g->v.total.files >> 1 && g->v.total.files >= min_halfway_files &&
	   ((g->v.total.size * g->v.total.files) >= (min_halfway_size * 1024 * 1024)) && msg->halfway != NULL) {
		d_log(1, "release_incomplete: Writing HALFWAY to %s\n", log);
		writelog(g, convert(&g->v, g->ui, g->gi, msg->halfway), (g->v.total.users > 1 ? rtype->race_halfway : rtype->norace_halfway));
	}
	
	d_log(1, "release_incomplete: Caching progress bar\n");
	buffer_progress_bar(&g->v);

	if (!matchpath(group_dirs, g->l.path) || create_incomplete_links_in_group_dirs) {
		d_log(1, "release_incomplete: Creating incomplete indicator:\n", g->l.incomplete);
		d_log(1, "release_incomplete:    name: '%s', incomplete: '%s', path: '%s'\n", g->v.misc.release_name, g->l.incomplete, g->l.path);
		create_incomplete2();
	}

	d_log(1, "release_incomplete: Creating/moving progress bar\n");
	move_progress_bar(0, &g->v, g->ui, g->gi);
	printf("%s", convert(&g->v, g->ui, g->gi, zipscript_footer_ok));

}

