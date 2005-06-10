#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

#include "handle_sfv.h"
#include "handling.h"

#include "zsfunctions.h"
#include "convert.h"
#include "race-file.h"
#include "errors.h"
#include "crc.h"
#include "multimedia.h"

#ifndef HAVE_STRLCPY
# include "strl/strl.h"
#endif

int
handle_sfv(HANDLER_ARGS *ha) {

	unsigned char	exit_value = EXIT_SUCCESS;
	struct dirent	*dp;
	int		cnt, cnt2;
	char		*sfv_type = 0;
	DIR		*dir;

	dir = opendir(".");

	d_log(1, "handle_sfv: File type is: SFV\n");
	if ((matchpath(sfv_dirs, ha->g->l.path)) || (matchpath(group_dirs, ha->g->l.path))  ) {
		d_log(1, "handle_sfv: Directory matched with sfv_dirs/group_dirs\n");
	} else {
		d_log(1, "handle_sfv: WARNING! Directory did not match with sfv_dirs/group_dirs\n");
		if (strict_path_match == TRUE) {
			d_log(1, "handle_sfv: Strict mode on - exiting\n");
			sprintf(ha->g->v.misc.error_msg, UNKNOWN_FILE, ha->fileext);
			mark_as_bad(ha->g->v.file.name);
			ha->msg->error = convert(&ha->g->v, ha->g->ui, ha->g->gi, bad_file_msg);
			if (exit_value < 2)
				writelog(ha->g, ha->msg->error, bad_file_wrongdir_type);
			exit_value = 2;
			closedir(dir);
			return exit_value;
		}
	}

	if (fileexists(ha->g->l.sfv)) {
		if (deny_double_sfv == TRUE && findfileextcount(".", ".sfv") > 1 && sfv_compare_size(".sfv", ha->g->v.file.size) > 0) {
			d_log(1, "handle_sfv: DEBUG: sfv_compare_size=%d\n", sfv_compare_size(".sfv", ha->g->v.file.size));
			d_log(1, "handle_sfv: No double sfv allowed\n");
			ha->msg->error = convert(&ha->g->v, ha->g->ui, ha->g->gi, deny_double_msg);
			writelog(ha->g, ha->msg->error, general_doublesfv_type);
			sprintf(ha->g->v.misc.error_msg, DOUBLE_SFV);
			mark_as_bad(ha->g->v.file.name);
			exit_value = 2;
			closedir(dir);
			return exit_value;
		} else if (findfileextcount(".", ".sfv") > 1 && sfv_compare_size(".sfv", ha->g->v.file.size) > 0) {
			d_log(1, "handle_sfv: DEBUG: sfv_compare_size=%d\n", sfv_compare_size(".sfv", ha->g->v.file.size));
			d_log(1, "handle_sfv: Reading remainders of old sfv\n");
			readsfv(ha->g->l.sfv, &ha->g->v, 1);
			cnt = ha->g->v.total.files - ha->g->v.total.files_missing;
			cnt2 = ha->g->v.total.files;
			ha->g->v.total.files_missing = ha->g->v.total.files = 0;
			readsfv_ffile(&ha->g->v);
			if ((ha->g->v.total.files <= cnt2) || (ha->g->v.total.files != (cnt + ha->g->v.total.files_missing))) {
				d_log(1, "handle_sfv: Old sfv seems to match with more files than current one\n");
				strlcpy(ha->g->v.misc.error_msg, "SFV does not match with files!", 80);
				ha->msg->error = convert(&ha->g->v, ha->g->ui, ha->g->gi, deny_double_msg);
				writelog(ha->g, ha->msg->error, general_doublesfv_type);
				sprintf(ha->g->v.misc.error_msg, DOUBLE_SFV);
				mark_as_bad(ha->g->v.file.name);
				exit_value = 2;
				closedir(dir);
				return exit_value;
			}
			ha->g->v.total.files = ha->g->v.total.files_missing = 0;
		} else {
			d_log(1, "handle_sfv: DEBUG: sfv_compare_size=%d\n", sfv_compare_size(".sfv", ha->g->v.file.size));
			d_log(1, "handle_sfv: Hmm.. Seems the old .sfv was deleted. Allowing new one.\n");
			unlink(ha->g->l.race);
			unlink(ha->g->l.sfv);
			rewinddir(dir);
			while ((dp = readdir(dir)))
				unlink_extra(dp->d_name, "-missing");
		}
	}
	d_log(1, "handle_sfv: Parsing sfv and creating sfv data\n");
	if (copysfv(ha->g->v.file.name, ha->g->l.sfv, ha->g->l.path, 0)) {
		d_log(1, "handle_sfv: Found invalid entries in SFV.\n");
		ha->msg->error = convert(&ha->g->v, ha->g->ui, ha->g->gi, bad_file_msg);
		if (exit_value < 2)
			writelog(ha->g, ha->msg->error, bad_file_sfv_type);
		mark_as_bad(ha->g->v.file.name);
		exit_value = 2;
		sprintf(ha->g->v.misc.error_msg, EMPTY_SFV);
		unlink(ha->g->l.race);
		unlink(ha->g->l.sfv);

		rewinddir(dir);
		while ((dp = readdir(dir)))
			unlink_extra(dp->d_name, "-missing");
		closedir(dir);
		return exit_value;
	}

#if (use_partial_on_noforce == TRUE)
	if ((force_sfv_first == FALSE) || matchpartialpath(noforce_sfv_first_dirs, ha->g->l.path))
#else
	if ((force_sfv_first == FALSE) || matchpath(noforce_sfv_first_dirs, ha->g->l.path))
#endif
	{
		if (fileexists(ha->g->l.race)) {
			d_log(1, "handle_sfv: Testing files marked as untested\n");
			testfiles(&ha->g->l, &ha->g->v, 0);
		}
	}
	d_log(1, "handle_sfv: Reading file count from SFV\n");
	readsfv(ha->g->l.sfv, &ha->g->v, 0);

#if (smart_sfv_write && sfv_cleanup)
	d_log(1, "handle_sfv: Rewriting sfv file according to smart_sfv_write\n");
	sfvdata_to_sfv(ha->g->l.sfv, findfileext_dir(dir, ".sfv"));
#endif
	
	if (ha->g->v.total.files == 0) {
		d_log(1, "handle_sfv: SFV seems to have no files of accepted types, or has errors.\n");
		sprintf(ha->g->v.misc.error_msg, EMPTY_SFV);
		mark_as_bad(ha->g->v.file.name);
		ha->msg->error = convert(&ha->g->v, ha->g->ui, ha->g->gi, bad_file_msg);
		if (exit_value < 2)
			writelog(ha->g, ha->msg->error, bad_file_sfv_type);
		exit_value = 2;
		closedir(dir);
		return exit_value;
	}
	printf(zipscript_sfv_ok);
	if (fileexists(ha->g->l.race)) {
		d_log(1, "handle_sfv: Reading race data from file to memory\n");
		readrace(ha->g->l.race, &ha->g->v, ha->g->ui, ha->g->gi);
	}
	if (del_completebar) {
		d_log(1, "handle_sfv: Making sure that release is not marked as complete\n");
		removecomplete();
	}
	d_log(1, "handle_sfv: Setting message pointers\n");
	sfv_type = general_announce_sfv_type;
	switch (ha->g->v.misc.release_type) {
	case RTYPE_RAR:
		ha->msg->sfv = rar_sfv;
		sfv_type = rar_announce_sfv_type;
		break;
	case RTYPE_OTHER:
		ha->msg->sfv = other_sfv;
		sfv_type = other_announce_sfv_type;
		break;
	case RTYPE_AUDIO:
		ha->msg->sfv = audio_sfv;
		sfv_type = audio_announce_sfv_type;
		break;
	case RTYPE_VIDEO:
		ha->msg->sfv = video_sfv;
		sfv_type = video_announce_sfv_type;
		break;
	default :
		ha->msg->sfv = rar_sfv;
		sfv_type = rar_announce_sfv_type;
		d_log(1, "handle_sfv: WARNING! Not a known release type - Contact the authors! (1:%d)\n", ha->g->v.misc.release_type);
		break;
	}

	if (!ha->msg->sfv)
		d_log(1, "handle_sfv: Something's messed up - ha->msg->sfv not set!\n");

	ha->msg->halfway = ha->msg->newleader = ha->msg->race = ha->msg->update = NULL;

	if (ha->g->v.total.files_missing > 0) {
		if (ha->msg->sfv != NULL) {
			d_log(1, "handle_sfv: Writing SFV message to %s\n", log);
			writelog(ha->g, convert(&ha->g->v, ha->g->ui, ha->g->gi, ha->msg->sfv), sfv_type);
		}
	} else {
		if (ha->g->v.misc.release_type == RTYPE_AUDIO) {
			d_log(1, "handle_sfv: Reading audio info for completebar\n");
			get_mpeg_audio_info(findfileext(".", ".mp3"), &ha->g->v.audio);
		}
	}

	closedir(dir);
	return exit_value;
}

/* handling of a file thats inside a zip dir */
int
handle_sfv32(HANDLER_ARGS *ha)
{
	
	unsigned int	crc, s_crc = 0;
	unsigned char	no_check = FALSE;
	char		*target = 0;
	unsigned char	exit_value = EXIT_SUCCESS;

	d_log(1, "handle_sfv32: File type is: ANY\n");

	target = ng_realloc(target, 1024, 1, 1, 1);

	if (check_rarfile(ha->g->v.file.name)) {
		d_log(1, "handle_sfv32: File is password protected.\n");
		sprintf(ha->g->v.misc.error_msg, PASSWORD_PROTECTED);
		ha->msg->error = convert(&ha->g->v, ha->g->ui, ha->g->gi, bad_file_msg);
		if (exit_value < 2)
			writelog(ha->g, ha->msg->error, bad_file_password_type);
		exit_value = 2;
		return exit_value;
	}

	d_log(1, "handle_sfv32: Converting crc (%s) from string to integer\n", ha->argv[3]);
	crc = hexstrtodec(ha->argv[3]);
	if (crc == 0) {
		d_log(1, "handle_sfv32: We did not get crc from ftp daemon, calculating crc for %s now.\n", ha->g->v.file.name);
		crc = calc_crc32(ha->g->v.file.name);
	}
	if (fileexists(ha->g->l.sfv)) {
		s_crc = readsfv(ha->g->l.sfv, &ha->g->v, 0);

#if (sfv_calc_single_fname == TRUE)
		if (s_crc == 0 && ha->g->v.misc.in_sfvfile == TRUE) {
			d_log(1, "handle_sfv32: CRC in SFV is 0 - trying to calculate it from the file\n");
			s_crc = calc_crc32(ha->g->v.file.name);
			update_sfvdata(ha->g->l.sfv, ha->g->v.file.name, s_crc);
			if (smart_sfv_write && sfv_cleanup)
				sfvdata_to_sfv(ha->g->l.sfv, findfileext(".", ".sfv"));
		}
#endif
		if (!ha->g->v.misc.sfv_match && ha->g->v.misc.in_sfvfile == TRUE) {
			update_sfvdata(ha->g->l.sfv, ha->g->v.file.name, s_crc);
#if (smart_sfv_write && sfv_cleanup)
			sfvdata_to_sfv(ha->g->l.sfv, findfileext(".", ".sfv"));
#endif
		}

		d_log(1, "handle_sfv32: DEBUG: crc: %X - s_crc: %X - match:%d\n", crc, s_crc, ha->g->v.misc.sfv_match);
		if (s_crc != crc && !(s_crc == 0 && ha->g->v.misc.sfv_match)) {
			if (s_crc == 0 && !ha->g->v.misc.sfv_match) {
				if (!strcomp(allowed_types, ha->fileext)) {
#if (allow_files_not_in_sfv == TRUE)
					d_log(1, "handle_sfv32: Filename was not found in the SFV, but allowing anyway\n");
					no_check = TRUE;
					return exit_value;
#endif
					d_log(1, "handle_sfv32: Filename was not found in the SFV\n");
					strlcpy(ha->g->v.misc.error_msg, NOT_IN_SFV, 80);
				} else {
					d_log(1, "handle_sfv32: filetype is part of allowed_types.\n");
					no_check = TRUE;
					return exit_value;
				}
			} else {
				d_log(1, "handle_sfv32: CRC-32 check failed\n");
				if (!hexstrtodec(ha->argv[3]) && allow_file_resume) {
					d_log(1, "handle_sfv32: Broken xfer detected - allowing file.\n");
					no_check = TRUE;
					ha->msg->error = convert(&ha->g->v, ha->g->ui, ha->g->gi, bad_file_msg);
					writelog(ha->g, ha->msg->error, bad_file_crc_type);
					if (enable_unduper_script == TRUE) {
						if (!fileexists(unduper_script)) {
							d_log(1, "handle_sfv32: Warning - undupe script (%s) does not exist.\n", unduper_script);
						}
						sprintf(target, unduper_script " \"%s\"", ha->g->v.file.name);
						if (execute(target) == 0)
							d_log(1, "handle_sfv32: undupe of %s successful.\n", ha->g->v.file.name);
						else
							d_log(1, "handle_sfv32: undupe of %s failed.\n", ha->g->v.file.name);
					}
					return exit_value;
				}
				strlcpy(ha->g->v.misc.error_msg, BAD_CRC, 80);
			}
			mark_as_bad(ha->g->v.file.name);
			ha->msg->error = convert(&ha->g->v, ha->g->ui, ha->g->gi, bad_file_msg);
			if (exit_value < 2)
				writelog(ha->g, ha->msg->error, bad_file_crc_type);
			exit_value = 2;
			return exit_value;
		}
		if (s_crc == 0 && ha->g->v.misc.sfv_match && ha->g->v.misc.in_sfvfile == TRUE) {
			d_log(1, "handle_sfv32: File was found in sfv, but had no crc.\n");
			s_crc = crc;
			update_sfvdata(ha->g->l.sfv, ha->g->v.file.name, s_crc);
#if (smart_sfv_write && sfv_cleanup)
			sfvdata_to_sfv(ha->g->l.sfv, findfileext(".", ".sfv"));
#endif

		}
#if (sfv_cleanup_lowercase == TRUE)
		if (check_dupefile(".", ha->g->v.file.name)) {
			d_log(1, "handle_sfv32: dupe detected - same file, different case already exists.\n");
			strlcpy(ha->g->v.misc.error_msg, DOUBLE_SFV, 80);
			mark_as_bad(ha->g->v.file.name);
			ha->msg->error = convert(&ha->g->v, ha->g->ui, ha->g->gi, bad_file_msg);
			if (exit_value < 2)
				writelog(ha->g, ha->msg->error, bad_file_crc_type);
			exit_value = 2;
			return exit_value;
		}
#endif
		printf(zipscript_SFV_ok);
		d_log(1, "handle_sfv32: Storing new race data\n");
		writerace(ha->g->l.race, &ha->g->v, crc, F_CHECKED);
	} else {
#if (force_sfv_first == TRUE )
# if (use_partial_on_noforce == TRUE)
		if (!matchpartialpath(noforce_sfv_first_dirs, ha->g->l.path) && !matchpath(zip_dirs, ha->g->l.path)) {
# else
		if (!matchpath(noforce_sfv_first_dirs, ha->g->l.path) && !matchpath(zip_dirs, ha->g->l.path)) {
# endif
			d_log(1, "handle_sfv32: SFV needs to be uploaded first\n");
			strlcpy(ha->g->v.misc.error_msg, SFV_FIRST, 80);
			mark_as_bad(ha->g->v.file.name);
			ha->msg->error = convert(&ha->g->v, ha->g->ui, ha->g->gi, bad_file_msg);
			if (exit_value < 2)
				writelog(ha->g, ha->msg->error, bad_file_nosfv_type);
			exit_value = 2;
			return exit_value;
		} else if (matchpath(zip_dirs, ha->g->l.path) && (!fileexists(ha->g->l.sfv))) {
			d_log(1, "handle_sfv32: This looks like a file uploaded the wrong place - Not allowing it.\n");
			strlcpy(ha->g->v.misc.error_msg, SFV_FIRST, 80);
			mark_as_bad(ha->g->v.file.name);
			ha->msg->error = convert(&ha->g->v, ha->g->ui, ha->g->gi, bad_file_msg);
			if (exit_value < 2)
				writelog(ha->g, ha->msg->error, bad_file_nosfv_type);
			exit_value = 2;
			return exit_value;
		} else {
			d_log(1, "handle_sfv32: path matched with noforce_sfv_first or zip_dirs - allowing file.\n");
			printf(zipscript_SFV_skip);
			d_log(1, "handle_sfv32: Storing new race data\n");
			writerace(ha->g->l.race, &ha->g->v, crc, F_NOTCHECKED);
		}
#else
		d_log(1, "handle_sfv32: Could not check file yet - SFV is not present\n");
		printf(zipscript_SFV_skip);
		d_log(1, "handle_sfv32: Storing new race data\n");
		writerace(ha->g->l.race, &ha->g->v, crc, F_NOTCHECKED);
#endif
	}

	d_log(1, "handle_sfv32: Reading race data from file to memory\n");
	readrace(ha->g->l.race, &ha->g->v, ha->g->ui, ha->g->gi);

	d_log(1, "handle_sfv32: Setting pointers\n");
	/* TODO: replace RTYPE_* with whatever is defined in handlers.h.
	 * replace is{rar,video} etc with get_filetype() */
	if (ha->g->v.misc.release_type == RTYPE_NULL) {
		if (israr(ha->fileext))
			ha->g->v.misc.release_type = RTYPE_RAR;	/* .RAR / .R?? */
		else if (isvideo(ha->fileext))
			ha->g->v.misc.release_type = RTYPE_VIDEO;	/* AVI/MPEG */
		else if (!memcmp(ha->fileext, "mp3", 4))
			ha->g->v.misc.release_type = RTYPE_AUDIO;	/* MP3 */
		else
			ha->g->v.misc.release_type = RTYPE_OTHER;	/* OTHER FILE */
	}
	switch (ha->g->v.misc.release_type) {
	/* TODO: set_rtype_msg() should replace a bit here */
	case RTYPE_RAR:
		get_rar_info(&ha->g->v);
		ha->msg->race = rar_race;
		ha->msg->update = rar_update;
		ha->msg->halfway = CHOOSE(ha->g->v.total.users, rar_halfway, rar_norace_halfway);
		ha->msg->newleader = rar_newleader;
		break;
	case RTYPE_OTHER:
		ha->msg->race = other_race;
		ha->msg->update = other_update;
		ha->msg->halfway = CHOOSE(ha->g->v.total.users, other_halfway, other_norace_halfway);
		ha->msg->newleader = other_newleader;
		break;
	case RTYPE_AUDIO:
		ha->msg->race = audio_race;
		ha->msg->update = audio_update;
		ha->msg->halfway = CHOOSE(ha->g->v.total.users, audio_halfway, audio_norace_halfway);
		ha->msg->newleader = audio_newleader;
		d_log(1, "handle_sfv32: Trying to read audio header and tags\n");
		get_mpeg_audio_info(ha->g->v.file.name, &ha->g->v.audio);
		write_bitrate_in_race(ha->g->l.race, &ha->g->v);
		sprintf(ha->g->v.audio.bitrate, "%i", read_bitrate_in_race(ha->g->l.race));
		if ((enable_mp3_script == TRUE) && (ha->g->ui[ha->g->v.user.pos].files == 1)) {
			if (!fileexists(mp3_script))
				d_log(1, "handle_sfv32: Warning -  mp3_script (%s) - file does not exists\n", mp3_script);
			sprintf(target, "%s %s", mp3_script, convert(&ha->g->v, ha->g->ui, ha->g->gi, mp3_script_cookies));
			d_log(1, "handle_sfv32: Executing mp3 script (%s %s)\n", mp3_script, convert(&ha->g->v, ha->g->ui, ha->g->gi, mp3_script_cookies));
			if (execute(target) != 0)
				d_log(1, "handle_sfv32: Failed to execute mp3_script: %s\n", strerror(errno));
		}
		/* TODO: GET RID OF THIS COPY/PASTE */
		if (!matchpath(audio_nocheck_dirs, ha->g->l.path)) {
#if ( audio_banned_genre_check )
			if (strcomp(banned_genres, ha->g->v.audio.id3_genre)) {
				d_log(1, "handle_sfv32: File is from banned genre\n");
				sprintf(ha->g->v.misc.error_msg, BANNED_GENRE, ha->g->v.audio.id3_genre);
				if (audio_genre_warn == TRUE) {
					if (ha->g->ui[ha->g->v.user.pos].files == 1) {
						d_log(1, "handle_sfv32: warn on - logging to logfile\n");
						ha->msg->error = convert(&ha->g->v, ha->g->ui, ha->g->gi, audio_genre_warn_msg);
						writelog(ha->g, ha->msg->error, general_badgenre_type);
					} else
						d_log(1, "handle_sfv32: warn on - have already logged to logfile\n");
				} else {
					mark_as_bad(ha->g->v.file.name);
					ha->msg->error = convert(&ha->g->v, ha->g->ui, ha->g->gi, bad_file_msg);
					if ((ha->g->ui[ha->g->v.user.pos].files == 1) && (exit_value < 2))
						writelog(ha->g, ha->msg->error, bad_file_genre_type);
					exit_value = 2;
				}
#if ( del_banned_release || enable_banned_script )
				*ha->deldir = 1;
				exit_value = 2;
#endif
				break;
			}
#elif ( audio_allowed_genre_check == TRUE )
			if (!strcomp(allowed_genres, ha->g->v.audio.id3_genre)) {
				d_log(1, "handle_sfv32: File is not in allowed genre\n");
				sprintf(ha->g->v.misc.error_msg, BANNED_GENRE, ha->g->v.audio.id3_genre);
				if (audio_genre_warn == TRUE) {
					if (ha->g->ui[ha->g->v.user.pos]->files == 1) {
						d_log(1, "handle_sfv32: warn on - logging to logfile\n");
						ha->msg->error = convert(&ha->g->v, ha->g->ui, ha->g->gi, audio_genre_warn_msg);
						writelog(ha->g, ha->msg->error, general_badgenre_type);
					} else
						d_log(1, "handle_sfv32: warn on - have already logged to logfile\n");
				} else {
					mark_as_bad(ha->g->v.file.name);
					ha->msg->error = convert(&ha->g->v, ha->g->ui, ha->g->gi, bad_file_msg);
					if ((ha->g->ui[ha->g->v.user.pos]->files == 1) && (exit_value < 2))
						writelog(ha->g, ha->msg->error, bad_file_genre_type);
					exit_value = 2;
				}
#if ( del_banned_release || enable_banned_script )
				*ha->deldir = 1;
				exit_value = 2;
#endif
				break;
			}
#endif
#if ( audio_year_check == TRUE )
			if (!strcomp(allowed_years, ha->g->v.audio.id3_year)) {
				d_log(1, "handle_sfv32: File is from banned year\n");
				sprintf(ha->g->v.misc.error_msg, BANNED_YEAR, ha->g->v.audio.id3_year);
				if (audio_year_warn == TRUE) {
					if (ha->g->ui[ha->g->v.user.pos].files == 1) {
						d_log(1, "handle_sfv32: warn on - logging to logfile\n");
						ha->msg->error = convert(&ha->g->v, ha->g->ui, ha->g->gi, audio_year_warn_msg);
						writelog(ha->g, ha->msg->error, general_badyear_type);
					} else
						d_log(1, "handle_sfv32: warn on - have already logged to logfile\n");
				} else {
					mark_as_bad(ha->g->v.file.name);
					ha->msg->error = convert(&ha->g->v, ha->g->ui, ha->g->gi, bad_file_msg);
					if ((ha->g->ui[ha->g->v.user.pos].files == 1) && (exit_value < 2))
						writelog(ha->g, ha->msg->error, bad_file_year_type);
					exit_value = 2;
				}
#if ( del_banned_release || enable_banned_script )
				*ha->deldir = 1;
				exit_value = 2;
#endif
				break;
			}
#endif
#if ( audio_cbr_check == TRUE )
			if (ha->g->v.audio.is_vbr == 0) {
				if (!strcomp(allowed_constant_bitrates, ha->g->v.audio.bitrate)) {
					d_log(1, "handle_sfv32: File is encoded using banned bitrate\n");
					sprintf(ha->g->v.misc.error_msg, BANNED_BITRATE, ha->g->v.audio.bitrate);
					if (audio_cbr_warn == TRUE) {
						if (ha->g->ui[ha->g->v.user.pos].files == 1) {
							d_log(1, "handle_sfv32: warn on - logging to logfile\n");
							ha->msg->error = convert(&ha->g->v, ha->g->ui, ha->g->gi, audio_cbr_warn_msg);
							writelog(ha->g, ha->msg->error, general_badbitrate_type);
						} else
							d_log(1, "handle_sfv32: warn on - have already logged to logfile\n");
					} else {
						mark_as_bad(ha->g->v.file.name);
						ha->msg->error = convert(&ha->g->v, ha->g->ui, ha->g->gi, bad_file_msg);
						if ((ha->g->ui[ha->g->v.user.pos].files == 1) && (exit_value < 2))
							writelog(ha->g, ha->msg->error, bad_file_bitrate_type);
						exit_value = 2;
					}
#if ( del_banned_release || enable_banned_script )
					*ha->deldir = 1;
					exit_value = 2;
#endif
					break;
				}
			}
#endif
#if ( audio_allowed_vbr_preset_check == TRUE )
			if (ha->g->v.audio.is_vbr && strlen(ha->g->v.audio.vbr_preset) && !strcomp(allowed_vbr_presets, ha->g->v.audio.vbr_preset)) {
				d_log(1, "handle_sfv32: File is not in allowed vbr preset list (%s)\n", ha->g->v.audio.vbr_preset);
				sprintf(ha->g->v.misc.error_msg, BANNED_PRESET, ha->g->v.audio.vbr_preset);
				if (audio_vbr_preset_warn == TRUE) {
					if (ha->g->ui[ha->g->v.user.pos].files == 1) {
						d_log(1, "handle_sfv32: warn on - logging to logfile\n");
						ha->msg->error = convert(&ha->g->v, ha->g->ui, ha->g->gi, audio_vbr_preset_warn_msg);
						writelog(ha->g, ha->msg->error, general_badpreset_type);
					} else
						d_log(1, "handle_sfv32: warn on - have already logged to logfile\n");
				} else {
					mark_as_bad(ha->g->v.file.name);
					ha->msg->error = convert(&ha->g->v, ha->g->ui, ha->g->gi, bad_file_msg);
					if ((ha->g->ui[ha->g->v.user.pos].files == 1) && (exit_value < 2))
						writelog(ha->g, ha->msg->error, bad_file_vbr_preset_type);
					exit_value = 2;
				}
#if ( del_banned_release || enable_banned_script )
				*ha->deldir = 1;
				exit_value = 2;
#endif
				break;
			}
#endif
		} else
			d_log(1, "handle_sfv32: user is in a no audio check dir - skipping checks.\n");

		if (realtime_mp3_info != DISABLED) {
			d_log(1, "handle_sfv32: Printing realtime_mp3_info.\n");
			printf("%s", convert(&ha->g->v, ha->g->ui, ha->g->gi, realtime_mp3_info));
		}
		break;
	case RTYPE_VIDEO:
		d_log(1, "handle_sfv32: Trying to read video header\n");
		if (!memcmp(ha->fileext, "avi", 3)) {
			bzero(&ha->g->m, sizeof(struct MULTIMEDIA));
			avinfo(ha->g->v.file.name, &ha->g->m);
			d_log(1, "handle_sfv32: avinfo: video - %dx%d * %.0f of %s (%s).\n", ha->g->m.height, ha->g->m.width, ha->g->m.fps, ha->g->m.vids, ha->g->m.audio);
			avi_video(ha->g->v.file.name, &ha->g->v.video);
		}
		else
			mpeg_video(ha->g->v.file.name, &ha->g->v.video);
		ha->msg->race = video_race;
		ha->msg->update = video_update;
		ha->msg->halfway = CHOOSE(ha->g->v.total.users, video_halfway, video_norace_halfway);
		ha->msg->newleader = video_newleader;
		break;
	default:
		get_rar_info(&ha->g->v);
		ha->msg->race = rar_race;
		ha->msg->update = rar_update;
		ha->msg->halfway = CHOOSE(ha->g->v.total.users, rar_halfway, rar_norace_halfway);
		ha->msg->newleader = rar_newleader;
		d_log(1, "handle_sfv32: WARNING! Not a known release type - Contact the authors! (2:%d)\n", ha->g->v.misc.release_type);
		break;
	}

	if (!ha->msg->race)
		d_log(1, "handle_sfv32: Something's messed up - ha->msg->race not set!\n");

	if (exit_value == EXIT_SUCCESS) {
		d_log(1, "handle_sfv32: Removing missing indicator\n");
		unlink_missing(ha->g->v.file.name);
		/*
		sprintf(target, "%s-missing", ha->g->v.file.name);
#if (sfv_cleanup_lowercase == TRUE)
		  strtolower(target);
#endif
		if (target)
			unlink(target);
		*/
		ng_free(target);
	}

	return exit_value;
}
