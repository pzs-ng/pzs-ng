#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

#include "handle_sfv.h"
#include "zsfunctions.h"
#include "convert.h"
#include "race-file.h"
#include "errors.h"
#include "crc.h"
#include "multimedia.h"

int
handle_sfv(GLOBAL *g, MSG *msg) {

	char		*fileext;
	unsigned char	exit_value = EXIT_SUCCESS;
	struct dirent	*dp;
	int		cnt, cnt2;
	char		*ext = 0;
	char		*sfv_type = 0;
	DIR		*dir;

	d_log(1, "handle_sfv: File type is: SFV\n");
	if ((matchpath(sfv_dirs, g->l.path)) || (matchpath(group_dirs, g->l.path))  ) {
		d_log(1, "handle_sfv: Directory matched with sfv_dirs/group_dirs\n");
	} else {
		d_log(1, "handle_sfv: WARNING! Directory did not match with sfv_dirs/group_dirs\n");
		if (strict_path_match == TRUE) {
			d_log(1, "handle_sfv: Strict mode on - exiting\n");
			sprintf(g->v.misc.error_msg, UNKNOWN_FILE, fileext);
			mark_as_bad(g->v.file.name);
			msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
			if (exit_value < 2)
				writelog(g, msg->error, bad_file_wrongdir_type);
			exit_value = 2;
			return exit_value;
		}
	}

	if (fileexists(g->l.sfv)) {
		if (deny_double_sfv == TRUE && findfileextcount(".", ".sfv") > 1 && sfv_compare_size(".sfv", g->v.file.size) > 0) {
			d_log(1, "handle_sfv: DEBUG: sfv_compare_size=%d\n", sfv_compare_size(".sfv", g->v.file.size));
			d_log(1, "handle_sfv: No double sfv allowed\n");
			msg->error = convert(&g->v, g->ui, g->gi, deny_double_msg);
			writelog(g, msg->error, general_doublesfv_type);
			sprintf(g->v.misc.error_msg, DOUBLE_SFV);
			mark_as_bad(g->v.file.name);
			exit_value = 2;
			return exit_value;
		} else if (findfileextcount(".", ".sfv") > 1 && sfv_compare_size(".sfv", g->v.file.size) > 0) {
			d_log(1, "handle_sfv: DEBUG: sfv_compare_size=%d\n", sfv_compare_size(".sfv", g->v.file.size));
			d_log(1, "handle_sfv: Reading remainders of old sfv\n");
			readsfv(g->l.sfv, &g->v, 1);
			cnt = g->v.total.files - g->v.total.files_missing;
			cnt2 = g->v.total.files;
			g->v.total.files_missing = g->v.total.files = 0;
			readsfv_ffile(&g->v);
			if ((g->v.total.files <= cnt2) || (g->v.total.files != (cnt + g->v.total.files_missing))) {
				d_log(1, "handle_sfv: Old sfv seems to match with more files than current one\n");
				strlcpy(g->v.misc.error_msg, "SFV does not match with files!", 80);
				msg->error = convert(&g->v, g->ui, g->gi, deny_double_msg);
				writelog(g, msg->error, general_doublesfv_type);
				sprintf(g->v.misc.error_msg, DOUBLE_SFV);
				mark_as_bad(g->v.file.name);
				exit_value = 2;
				return exit_value;
			}
			g->v.total.files = g->v.total.files_missing = 0;
		} else {
			d_log(1, "handle_sfv: DEBUG: sfv_compare_size=%d\n", sfv_compare_size(".sfv", g->v.file.size));
			d_log(1, "handle_sfv: Hmm.. Seems the old .sfv was deleted. Allowing new one.\n");
			unlink(g->l.race);
			unlink(g->l.sfv);
			dir = opendir(".");
			while ((dp = readdir(dir))) {
				cnt = cnt2 = (int)strlen(dp->d_name);
				ext = dp->d_name;
				while (ext[cnt] != '-' && cnt > 0)
					cnt--;
				if (ext[cnt] != '-')
					cnt = cnt2;
				else
					cnt++;
				ext += cnt;
				if (!strncmp(ext, "missing", 7))
					unlink(dp->d_name);
			}
			closedir(dir);
		}
	}
	d_log(1, "handle_sfv: Parsing sfv and creating sfv data\n");
	if (copysfv(g->v.file.name, g->l.sfv, &g->v, g->l.path, 0)) {
		d_log(1, "handle_sfv: Found invalid entries in SFV.\n");
		msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
		if (exit_value < 2)
			writelog(g, msg->error, bad_file_sfv_type);
		mark_as_bad(g->v.file.name);
		exit_value = 2;
		sprintf(g->v.misc.error_msg, EMPTY_SFV);
		unlink(g->l.race);
		unlink(g->l.sfv);

		dir = opendir(".");
		while ((dp = readdir(dir))) {
			cnt = cnt2 = (int)strlen(dp->d_name);
			ext = dp->d_name;
			while (ext[cnt] != '-' && cnt > 0)
				cnt--;
			if (ext[cnt] != '-')
				cnt = cnt2;
			else
				cnt++;
			ext += cnt;
			if (!strncmp(ext, "missing", 7))
				unlink(dp->d_name);
		}
		closedir(dir);

		return exit_value;
	}

#if (use_partial_on_noforce == TRUE)
	if ( (force_sfv_first == FALSE) || matchpartialpath(noforce_sfv_first_dirs, g->l.path))
#else
	if ( (force_sfv_first == FALSE) || matchpath(noforce_sfv_first_dirs, g->l.path))
#endif
	{
		if (fileexists(g->l.race)) {
			d_log(1, "handle_sfv: Testing files marked as untested\n");
			testfiles(&g->l, &g->v, 0);
		}
	}
	d_log(1, "handle_sfv: Reading file count from SFV\n");
	readsfv(g->l.sfv, &g->v, 0);

#if (smart_sfv_write && sfv_cleanup)
	d_log(1, "handle_sfv: Rewriting sfv file according to smart_sfv_write\n");
	sfvdata_to_sfv(g->l.sfv, findfileext(".", ".sfv"));
#endif
	
	if (g->v.total.files == 0) {
		d_log(1, "handle_sfv: SFV seems to have no files of accepted types, or has errors.\n");
		sprintf(g->v.misc.error_msg, EMPTY_SFV);
		mark_as_bad(g->v.file.name);
		msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
		if (exit_value < 2)
			writelog(g, msg->error, bad_file_sfv_type);
		exit_value = 2;
		return exit_value;
	}
	printf(zipscript_sfv_ok);
	if (fileexists(g->l.race)) {
		d_log(1, "handle_sfv: Reading race data from file to memory\n");
		readrace(g->l.race, &g->v, g->ui, g->gi);
	}
	if (del_completebar) {
		d_log(1, "handle_sfv: Making sure that release is not marked as complete\n");
		removecomplete();
	}
	d_log(1, "handle_sfv: Setting message pointers\n");
	sfv_type = general_announce_sfv_type;
	switch (g->v.misc.release_type) {
	case RTYPE_RAR:
		msg->sfv = rar_sfv;
		sfv_type = rar_announce_sfv_type;
		break;
	case RTYPE_OTHER:
		msg->sfv = other_sfv;
		sfv_type = other_announce_sfv_type;
		break;
	case RTYPE_AUDIO:
		msg->sfv = audio_sfv;
		sfv_type = audio_announce_sfv_type;
		break;
	case RTYPE_VIDEO:
		msg->sfv = video_sfv;
		sfv_type = video_announce_sfv_type;
		break;
	default :
		msg->sfv = rar_sfv;
		sfv_type = rar_announce_sfv_type;
		d_log(1, "handle_sfv: WARNING! Not a known release type - Contact the authors! (1:%d)\n", g->v.misc.release_type);
		break;
	}

	if (!msg->sfv)
		d_log(1, "handle_sfv: Something's messed up - msg->sfv not set!\n");

	msg->halfway = msg->newleader = msg->race = msg->update = NULL;

	if (g->v.total.files_missing > 0) {
		if (msg->sfv != NULL) {
			d_log(1, "handle_sfv: Writing SFV message to %s\n", log);
			writelog(g, convert(&g->v, g->ui, g->gi, msg->sfv), sfv_type);
		}
	} else {
		if (g->v.misc.release_type == RTYPE_AUDIO) {
			d_log(1, "handle_sfv: Reading audio info for completebar\n");
			get_mpeg_audio_info(findfileext(".", ".mp3"), &g->v.audio);
		}
	}

	return exit_value;
}

int
handle_sfv32(GLOBAL *g, MSG *msg, char **argv, char *fileext, int *deldir)
{
	
	unsigned int	crc, s_crc = 0;
	unsigned char	no_check = FALSE;
	char		*target = 0;
	unsigned char	exit_value = EXIT_SUCCESS;

	d_log(1, "handle_sfv32: File type is: ANY\n");

	if (check_rarfile(g->v.file.name)) {
		d_log(1, "handle_sfv32: File is password protected.\n");
		sprintf(g->v.misc.error_msg, PASSWORD_PROTECTED);
		msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
		if (exit_value < 2)
			writelog(g, msg->error, bad_file_password_type);
		exit_value = 2;
		return exit_value;
	}

	d_log(1, "handle_sfv32: Converting crc (%s) from string to integer\n", argv[3]);
	crc = hexstrtodec(argv[3]);
	if (crc == 0) {
		d_log(1, "handle_sfv32: We did not get crc from ftp daemon, calculating crc for %s now.\n", g->v.file.name);
		crc = calc_crc32(g->v.file.name);
	}
	if (fileexists(g->l.sfv)) {
		s_crc = readsfv(g->l.sfv, &g->v, 0);

#if (sfv_calc_single_fname == TRUE)
		if (s_crc == 0 && g->v.misc.in_sfvfile == TRUE) {
			d_log(1, "handle_sfv32: CRC in SFV is 0 - trying to calculate it from the file\n");
			s_crc = calc_crc32(g->v.file.name);
			update_sfvdata(g->l.sfv, g->v.file.name, s_crc);
			if (smart_sfv_write && sfv_cleanup)
				sfvdata_to_sfv(g->l.sfv, findfileext(".", ".sfv"));
		}
#endif
		if (!g->v.misc.sfv_match && g->v.misc.in_sfvfile == TRUE) {
			update_sfvdata(g->l.sfv, g->v.file.name, s_crc);
#if (smart_sfv_write && sfv_cleanup)
			sfvdata_to_sfv(g->l.sfv, findfileext(".", ".sfv"));
#endif
		}

		d_log(1, "handle_sfv32: DEBUG: crc: %X - s_crc: %X - match:%d\n", crc, s_crc, g->v.misc.sfv_match);
		if (s_crc != crc && !(s_crc == 0 && g->v.misc.sfv_match)) {
			if (s_crc == 0 && !g->v.misc.sfv_match) {
				if (!strcomp(allowed_types, fileext)) {
#if (allow_files_not_in_sfv == TRUE)
					d_log(1, "handle_sfv32: Filename was not found in the SFV, but allowing anyway\n");
					no_check = TRUE;
					return exit_value;
#endif
					d_log(1, "handle_sfv32: Filename was not found in the SFV\n");
					strlcpy(g->v.misc.error_msg, NOT_IN_SFV, 80);
				} else {
					d_log(1, "handle_sfv32: filetype is part of allowed_types.\n");
					no_check = TRUE;
					return exit_value;
				}
			} else {
				d_log(1, "handle_sfv32: CRC-32 check failed\n");
				if (!hexstrtodec(argv[3]) && allow_file_resume) {
					d_log(1, "handle_sfv32: Broken xfer detected - allowing file.\n");
					no_check = TRUE;
					msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
					writelog(g, msg->error, bad_file_crc_type);
					if (enable_unduper_script == TRUE) {
						if (!fileexists(unduper_script)) {
							d_log(1, "handle_sfv32: Warning - undupe script (%s) does not exist.\n", unduper_script);
						}
						sprintf(target, unduper_script " \"%s\"", g->v.file.name);
						if (execute(target) == 0)
							d_log(1, "handle_sfv32: undupe of %s successful.\n", g->v.file.name);
						else
							d_log(1, "handle_sfv32: undupe of %s failed.\n", g->v.file.name);
					}
					return exit_value;
				}
				strlcpy(g->v.misc.error_msg, BAD_CRC, 80);
			}
			mark_as_bad(g->v.file.name);
			msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
			if (exit_value < 2)
				writelog(g, msg->error, bad_file_crc_type);
			exit_value = 2;
			return exit_value;
		}
		if (s_crc == 0 && g->v.misc.sfv_match && g->v.misc.in_sfvfile == TRUE) {
			d_log(1, "handle_sfv32: File was found in sfv, but had no crc.\n");
			s_crc = crc;
			update_sfvdata(g->l.sfv, g->v.file.name, s_crc);
#if (smart_sfv_write && sfv_cleanup)
			sfvdata_to_sfv(g->l.sfv, findfileext(".", ".sfv"));
#endif

		}
#if (sfv_cleanup_lowercase == TRUE)
		if (check_dupefile(".", g->v.file.name)) {
			d_log(1, "handle_sfv32: dupe detected - same file, different case already exists.\n");
			strlcpy(g->v.misc.error_msg, DOUBLE_SFV, 80);
			mark_as_bad(g->v.file.name);
			msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
			if (exit_value < 2)
				writelog(g, msg->error, bad_file_crc_type);
			exit_value = 2;
			return exit_value;
		}
#endif
		printf(zipscript_SFV_ok);
		d_log(1, "handle_sfv32: Storing new race data\n");
		writerace(g->l.race, &g->v, crc, F_CHECKED);
	} else {
#if (force_sfv_first == TRUE )
# if (use_partial_on_noforce == TRUE)
		if (!matchpartialpath(noforce_sfv_first_dirs, g->l.path) && !matchpath(zip_dirs, g->l.path)) {
# else
		if (!matchpath(noforce_sfv_first_dirs, g->l.path) && !matchpath(zip_dirs, g->l.path)) {
# endif
			d_log(1, "handle_sfv32: SFV needs to be uploaded first\n");
			strlcpy(g->v.misc.error_msg, SFV_FIRST, 80);
			mark_as_bad(g->v.file.name);
			msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
			if (exit_value < 2)
				writelog(g, msg->error, bad_file_nosfv_type);
			exit_value = 2;
			return exit_value;
		} else if (matchpath(zip_dirs, g->l.path) && (!fileexists(g->l.sfv))) {
			d_log(1, "handle_sfv32: This looks like a file uploaded the wrong place - Not allowing it.\n");
			strlcpy(g->v.misc.error_msg, SFV_FIRST, 80);
			mark_as_bad(g->v.file.name);
			msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
			if (exit_value < 2)
				writelog(g, msg->error, bad_file_nosfv_type);
			exit_value = 2;
			return exit_value;
		} else {
			d_log(1, "handle_sfv32: path matched with noforce_sfv_first or zip_dirs - allowing file.\n");
			printf(zipscript_SFV_skip);
			d_log(1, "handle_sfv32: Storing new race data\n");
			writerace(g->l.race, &g->v, crc, F_NOTCHECKED);
		}
#else
		d_log(1, "handle_sfv32: Could not check file yet - SFV is not present\n");
		printf(zipscript_SFV_skip);
		d_log(1, "handle_sfv32: Storing new race data\n");
		writerace(g->l.race, &g->v, crc, F_NOTCHECKED);
#endif
	}

	d_log(1, "handle_sfv32: Reading race data from file to memory\n");
	readrace(g->l.race, &g->v, g->ui, g->gi);

	d_log(1, "handle_sfv32: Setting pointers\n");
	if (g->v.misc.release_type == RTYPE_NULL) {
		if (israr(fileext))
			g->v.misc.release_type = RTYPE_RAR;	/* .RAR / .R?? */
		else if (isvideo(fileext))
			g->v.misc.release_type = RTYPE_VIDEO;	/* AVI/MPEG */
		else if (!memcmp(fileext, "mp3", 4))
			g->v.misc.release_type = RTYPE_AUDIO;	/* MP3 */
		else
			g->v.misc.release_type = RTYPE_OTHER;	/* OTHER FILE */
	}
	switch (g->v.misc.release_type) {
	case RTYPE_RAR:
		get_rar_info(&g->v);
		msg->race = rar_race;
		msg->update = rar_update;
		msg->halfway = CHOOSE2(g->v.total.users, rar_halfway, rar_norace_halfway);
		msg->newleader = rar_newleader;
		break;
	case RTYPE_OTHER:
		msg->race = other_race;
		msg->update = other_update;
		msg->halfway = CHOOSE2(g->v.total.users, other_halfway, other_norace_halfway);
		msg->newleader = other_newleader;
		break;
	case RTYPE_AUDIO:
		msg->race = audio_race;
		msg->update = audio_update;
		msg->halfway = CHOOSE2(g->v.total.users, audio_halfway, audio_norace_halfway);
		msg->newleader = audio_newleader;
		d_log(1, "handle_sfv32: Trying to read audio header and tags\n");
		get_mpeg_audio_info(g->v.file.name, &g->v.audio);
		write_bitrate_in_race(g->l.race, &g->v);
		sprintf(g->v.audio.bitrate, "%i", read_bitrate_in_race(g->l.race, &g->v));
			if ((enable_mp3_script == TRUE) && (g->ui[g->v.user.pos]->files == 1)) {
				if (!fileexists(mp3_script)) {
					d_log(1, "handle_sfv32: Warning -  mp3_script (%s) - file does not exists\n", mp3_script);
				}
				d_log(1, "handle_sfv32: Executing mp3 script (%s %s)\n", mp3_script, convert(&g->v, g->ui, g->gi, mp3_script_cookies));
				sprintf(target, "%s %s", mp3_script, convert(&g->v, g->ui, g->gi, mp3_script_cookies));
				if (execute(target) != 0)
					d_log(1, "handle_sfv32: Failed to execute mp3_script: %s\n", strerror(errno));
			}
			if (!matchpath(audio_nocheck_dirs, g->l.path)) {
#if ( audio_banned_genre_check )
				if (strcomp(banned_genres, g->v.audio.id3_genre)) {
					d_log(1, "handle_sfv32: File is from banned genre\n");
					sprintf(g->v.misc.error_msg, BANNED_GENRE, g->v.audio.id3_genre);
					if (audio_genre_warn == TRUE) {
						if (g->ui[g->v.user.pos]->files == 1) {
							d_log(1, "handle_sfv32: warn on - logging to logfile\n");
							msg->error = convert(&g->v, g->ui, g->gi, audio_genre_warn_msg);
							writelog(g, msg->error, general_badgenre_type);
						} else
							d_log(1, "handle_sfv32: warn on - have already logged to logfile\n");
					} else {
						mark_as_bad(g->v.file.name);
						msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
						if ((g->ui[g->v.user.pos]->files == 1) && (exit_value < 2))
							writelog(g, msg->error, bad_file_genre_type);
						exit_value = 2;
					}
#if ( del_banned_release || enable_banned_script )
					*deldir = 1;
					exit_value = 2;
#endif
					break;
				}
#elif ( audio_allowed_genre_check == TRUE )
				if (!strcomp(allowed_genres, g->v.audio.id3_genre)) {
					d_log(1, "handle_sfv32: File is not in allowed genre\n");
					sprintf(g->v.misc.error_msg, BANNED_GENRE, g->v.audio.id3_genre);
					if (audio_genre_warn == TRUE) {
						if (g->ui[g->v.user.pos]->files == 1) {
							d_log(1, "handle_sfv32: warn on - logging to logfile\n");
							msg->error = convert(&g->v, g->ui, g->gi, audio_genre_warn_msg);
							writelog(g, msg->error, general_badgenre_type);
						} else
							d_log(1, "handle_sfv32: warn on - have already logged to logfile\n");
					} else {
						mark_as_bad(g->v.file.name);
						msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
						if ((g->ui[g->v.user.pos]->files == 1) && (exit_value < 2))
							writelog(g, msg->error, bad_file_genre_type);
						exit_value = 2;
					}
#if ( del_banned_release || enable_banned_script )
					*deldir = 1;
					exit_value = 2;
#endif
					break;
				}
#endif
#if ( audio_year_check == TRUE )
				if (!strcomp(allowed_years, g->v.audio.id3_year)) {
					d_log(1, "handle_sfv32: File is from banned year\n");
					sprintf(g->v.misc.error_msg, BANNED_YEAR, g->v.audio.id3_year);
					if (audio_year_warn == TRUE) {
						if (g->ui[g->v.user.pos]->files == 1) {
							d_log(1, "handle_sfv32: warn on - logging to logfile\n");
							msg->error = convert(&g->v, g->ui, g->gi, audio_year_warn_msg);
							writelog(g, msg->error, general_badyear_type);
						} else
							d_log(1, "handle_sfv32: warn on - have already logged to logfile\n");
					} else {
						mark_as_bad(g->v.file.name);
						msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
						if ((g->ui[g->v.user.pos]->files == 1) && (exit_value < 2))
							writelog(g, msg->error, bad_file_year_type);
						exit_value = 2;
					}
#if ( del_banned_release || enable_banned_script )
					*deldir = 1;
					exit_value = 2;
#endif
					break;
				}
#endif
#if ( audio_cbr_check == TRUE )
				if (g->v.audio.is_vbr == 0) {
					if (!strcomp(allowed_constant_bitrates, g->v.audio.bitrate)) {
						d_log(1, "handle_sfv32: File is encoded using banned bitrate\n");
						sprintf(g->v.misc.error_msg, BANNED_BITRATE, g->v.audio.bitrate);
						if (audio_cbr_warn == TRUE) {
							if (g->ui[g->v.user.pos]->files == 1) {
								d_log(1, "handle_sfv32: warn on - logging to logfile\n");
								msg->error = convert(&g->v, g->ui, g->gi, audio_cbr_warn_msg);
								writelog(g, msg->error, general_badbitrate_type);
							} else
								d_log(1, "handle_sfv32: warn on - have already logged to logfile\n");
						} else {
							mark_as_bad(g->v.file.name);
							msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
							if ((g->ui[g->v.user.pos]->files == 1) && (exit_value < 2))
								writelog(g, msg->error, bad_file_bitrate_type);
							exit_value = 2;
						}
#if ( del_banned_release || enable_banned_script )
						*deldir = 1;
						exit_value = 2;
#endif
						break;
					}
				}
#endif
#if ( audio_allowed_vbr_preset_check == TRUE )
				if (g->v.audio.is_vbr && strlen(g->v.audio.vbr_preset) && !strcomp(allowed_vbr_presets, g->v.audio.vbr_preset)) {
					d_log(1, "handle_sfv32: File is not in allowed vbr preset list (%s)\n", g->v.audio.vbr_preset);
					sprintf(g->v.misc.error_msg, BANNED_PRESET, g->v.audio.vbr_preset);
					if (audio_vbr_preset_warn == TRUE) {
						if (g->ui[g->v.user.pos]->files == 1) {
							d_log(1, "handle_sfv32: warn on - logging to logfile\n");
							msg->error = convert(&g->v, g->ui, g->gi, audio_vbr_preset_warn_msg);
							writelog(g, msg->error, general_badpreset_type);
						} else
							d_log(1, "handle_sfv32: warn on - have already logged to logfile\n");
					} else {
						mark_as_bad(g->v.file.name);
						msg->error = convert(&g->v, g->ui, g->gi, bad_file_msg);
						if ((g->ui[g->v.user.pos]->files == 1) && (exit_value < 2))
							writelog(g, msg->error, bad_file_vbr_preset_type);
						exit_value = 2;
					}
#if ( del_banned_release || enable_banned_script )
					*deldir = 1;
					exit_value = 2;
#endif
					break;
				}
#endif
			} else
				d_log(1, "handle_sfv32: user is in a no audio check dir - skipping checks.\n");

		if (realtime_mp3_info != DISABLED) {
			d_log(1, "handle_sfv32: Printing realtime_mp3_info.\n");
			printf("%s", convert(&g->v, g->ui, g->gi, realtime_mp3_info));
		}
		break;
	case RTYPE_VIDEO:
		d_log(1, "handle_sfv32: Trying to read video header\n");
		if (!memcmp(fileext, "avi", 3)) {
			bzero(&g->m, sizeof(struct MULTIMEDIA));
			avinfo(g->v.file.name, &g->m);
			d_log(1, "handle_sfv32: avinfo: video - %dx%d * %.0f of %s (%s).\n", g->m.height, g->m.width, g->m.fps, g->m.vids, g->m.audio);
			avi_video(g->v.file.name, &g->v.video);
		}
		else
			mpeg_video(g->v.file.name, &g->v.video);
		msg->race = video_race;
		msg->update = video_update;
		msg->halfway = CHOOSE2(g->v.total.users, video_halfway, video_norace_halfway);
		msg->newleader = video_newleader;
		break;
	default:
		get_rar_info(&g->v);
		msg->race = rar_race;
		msg->update = rar_update;
		msg->halfway = CHOOSE2(g->v.total.users, rar_halfway, rar_norace_halfway);
		msg->newleader = rar_newleader;
		d_log(1, "handle_sfv32: WARNING! Not a known release type - Contact the authors! (2:%d)\n", g->v.misc.release_type);
		break;
	}

	if (!msg->race)
		d_log(1, "handle_sfv32: Something's messed up - msg->race not set!\n");

	if (exit_value == EXIT_SUCCESS) {
		d_log(1, "handle_sfv32: Removing missing indicator\n");
		unlink_missing(g->v.file.name);
		/*
		sprintf(target, "%s-missing", g->v.file.name);
#if (sfv_cleanup_lowercase == TRUE)
		  strtolower(target);
#endif
		if (target)
			unlink(target);
		*/
	}

	return exit_value;
}
