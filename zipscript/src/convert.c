#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <ctype.h>
#include "objects.h"
#include "zsfunctions.h"
#include "../conf/zsconfig.h"
#include "zsconfig.defaults.h"

#include "convert.h"

char output[2048], output2[1024];

/*
 * char *hms(char *ttime, int secs)
 * 
 * Converts source integer to bolded time string.
 * 
 */
char *
hms(char *ttime, int secs)
{
	int		hours = 0,	mins = 0, tmp = 0;

	while (secs >= 3600) {
		hours++;
		secs -= 3600;
	}
	while (secs >= 60) {
		mins++;
		secs -= 60;
	}

	if (hours)
		tmp = sprintf(ttime, "%ih", (int)hours);
	if (mins)
		tmp += sprintf(ttime + tmp, "%im", (int)mins);
	if (secs || !tmp)
		tmp += sprintf(ttime + tmp, "%is", (int)secs);
	
	return ttime;
}



/*
 * Modified: 01.16.2002
 */
char           *
convert2(struct VARS *raceI, struct USERINFO *userI, struct GROUPINFO **groupI, char *instr, short int userpos)
{
	int		val1;
	int		val2;
	char           *out_p;
	char           *m;
	char		ctrl      [255];

	out_p = output2;
	bzero(out_p, (int)sizeof(out_p));
	bzero(ctrl, (int)sizeof(ctrl));

	if (instr) {
		for (; *instr; instr++) {
			if (*instr == '%') {
				instr++;
				m = instr;
				if (*instr == '-' && isdigit(*(instr + 1)))
					instr += 2;
				while (isdigit(*instr))
					instr++;
				if (m != instr && instr-m < (int)sizeof(ctrl)) {
					sprintf(ctrl, "%.*s", (int)(instr - m), m);
					val1 = strtol(ctrl, NULL, 10);
				} else
					val1 = 0;
				if (*instr == '.') {
					instr++;
					m = instr;
					if (*instr == '-' && isdigit(*(instr + 1)))
						instr += 2;
					while (isdigit(*instr))
						instr++;
					if (m != instr && instr-m < (int)sizeof(ctrl)) {
						sprintf(ctrl, "%.*s", (int)(instr - m), m);
						val2 = strtol(ctrl, NULL, 10);
					} else
						val2 = 0;
				} else {
					val2 = -1;
				}

				switch (*instr) {
//				case 'B':
//					out_p += sprintf(out_p, "\\002");
//					break;
				case 'K':
					out_p += sprintf(out_p, "%s", raceI->user.tagline);
					break;
				case 'F':
					out_p += sprintf(out_p, "%*i", val1, (unsigned int)raceI->misc.fastest_user[0]);
					break;
				case 'n':
					out_p += sprintf(out_p, "%*i", val1, (int)userpos + 1);
					break;
				case 'N':
					if ((int)userpos == 0) {
						out_p += sprintf(out_p, winner);
					} else {
						out_p += sprintf(out_p, loser);
					}
					break;
				case 'u':
					out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)userI->name);
					break;
				case 'g':
					out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)groupI[userI->group]->name);
					break;
				case 'U':
					sprintf(ctrl, "%s/%s", userI->name, groupI[userI->group]->name);
					out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)ctrl);
					break;
				case 'b':
					out_p += sprintf(out_p, "%*f", val1, (double)userI->bytes);
					break;
				case 'k':
					out_p += sprintf(out_p, "%*.*f", val1, val2, (double)(userI->bytes / 1024.));
					break;
				case 'm':
					out_p += sprintf(out_p, "%*.*f", val1, val2, (double)((userI->bytes >> 10) / 1024.));
					break;
				case 'p':
					out_p += sprintf(out_p, "%*.*f", val1, val2, (double)(userI->bytes * 100. / raceI->total.size));
					break;
				case 'f':
					out_p += sprintf(out_p, "%*i", val1, (int)userI->files);
					break;
				case 'S':
					out_p += sprintf(out_p, "%*i", val1, (unsigned int)raceI->misc.slowest_user[0]);
					break;
				case 's':
					out_p += sprintf(out_p, "%*.*f", val1, val2, (double)(userI->speed / 1024 / userI->bytes));
					break;

				case 'D':
					out_p += sprintf(out_p, "%*llu", val1, (unsigned long long)userI->dayup);
					break;
				case 'W':
					out_p += sprintf(out_p, "%*llu", val1, (unsigned long long)userI->wkup);
					break;
				case 'M':
					out_p += sprintf(out_p, "%*llu", val1, (unsigned long long)userI->monthup);
					break;
				case 'A':
					out_p += sprintf(out_p, "%*llu", val1, (unsigned long long)userI->allup);
					break;
				case '%':
					*out_p++ = *instr;
				}
			} else {
				*out_p++ = *instr;
			}
		}
	}
	*out_p = 0;
	return output2;
}




/*
 * Modified: 01.16.2002
 */
char           *
convert3(struct VARS *raceI, struct GROUPINFO *groupI, char *instr, short int grouppos)
{
	int		val1;
	int		val2;
	char           *out_p;
	char           *m;
	char		ctrl      [15];

	out_p = output2;

	bzero(out_p, (int)sizeof(out_p));
	bzero(ctrl, (int)sizeof(ctrl));

	for (; *instr; instr++)
		if (*instr == '%') {
			instr++;
			m = instr;
			if (*instr == '-' && isdigit(*(instr + 1)))
				instr += 2;
			while (isdigit(*instr))
				instr++;
			if (m != instr && instr-m < (int)sizeof(ctrl)) {
				sprintf(ctrl, "%.*s", (int)(instr - m), m);
				val1 = strtol(ctrl, NULL, 10);
			} else {
				val1 = 0;
			}

			if (*instr == '.') {
				instr++;
				m = instr;
				if (*instr == '-' && isdigit(*(instr + 1)))
					instr += 2;
				while (isdigit(*instr))
					instr++;
				if (m != instr && instr-m < (int)sizeof(ctrl)) {
					sprintf(ctrl, "%.*s", (int)(instr - m), m);
					val2 = strtol(ctrl, NULL, 10);
				} else {
					val2 = 0;
				}
			} else {
				val2 = -1;
			}

			switch (*instr) {
//			case 'B':
//				out_p += sprintf(out_p, "\\002");
//				break;
			case 'K':
				out_p += sprintf(out_p, "%s", raceI->user.tagline);
				break;
			case 'n':
				out_p += sprintf(out_p, "%*i", val1, (int)grouppos + 1);
				break;
			case 'N':
				if ((int)grouppos == 0) {
					out_p += sprintf(out_p, winner);
				} else {
					out_p += sprintf(out_p, loser);
				}
				break;
			case 'g':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)groupI->name);
				break;
			case 'b':
				out_p += sprintf(out_p, "%*i", val1, (int)groupI->bytes);
				break;
			case 'k':
				out_p += sprintf(out_p, "%*.*f", val1, val2, (double)(groupI->bytes / 1024.));
				break;
			case 'm':
				out_p += sprintf(out_p, "%*.*f", val1, val2, (double)((groupI->bytes >> 10) / 1024.));
				break;
			case 'p':
				out_p += sprintf(out_p, "%*.*f", val1, val2, (double)(groupI->bytes * 100.0 / raceI->total.size));
				break;
			case 'f':
				out_p += sprintf(out_p, "%*i", val1, (int)groupI->files);
				break;
			case 's':
				out_p += sprintf(out_p, "%*.*f", val1, val2, (double)(groupI->speed / 1024 / groupI->bytes));
				break;
			case 'u':
				out_p += sprintf(out_p, "%*i", val1, (int)groupI->users);
				break;
			case '%':
				*out_p++ = *instr;
			}
		} else
			*out_p++ = *instr;
	*out_p = 0;
	return output2;
}

char           *
convert4(struct VARS *raceI, char *instr)
{
	int		val1      , val2;
	char           *out_p;
	char           *m;
	char		ctrl      [15];

	out_p = output2;

	bzero(out_p, (int)sizeof(out_p));
	bzero(ctrl, (int)sizeof(ctrl));

	for (; *instr; instr++)
		if (*instr == '%') {
			instr++;
			m = instr;
			if (*instr == '-' && isdigit(*(instr + 1)))
				instr += 2;
			while (isdigit(*instr))
				instr++;
			if (m != instr && instr-m < (int)sizeof(ctrl)) {
				sprintf(ctrl, "%.*s", (int)(instr - m), m);
				val1 = strtol(ctrl, NULL, 10);
			} else {
				val1 = 0;
			}
			if (*instr == '.') {
				instr++;
				m = instr;
				if (*instr == '-' && isdigit(*(instr + 1)))
					instr += 2;
				while (isdigit(*instr))
					instr++;
				if (m != instr && instr-m < (int)sizeof(ctrl)) {
					sprintf(ctrl, "%.*s", (int)(instr - m), m);
					val2 = strtol(ctrl, NULL, 10);
				} else {
					val2 = 0;
				}
			} else {
				val2 = -1;
			}

			switch (*instr) {
			case 'w':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->audio.id3_genre);
				break;
			case 'W':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->audio.id3_album);
				break;
			case 'x':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->audio.id3_artist);
				break;
			case 'y':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->audio.id3_title);
				break;
			case 'Y':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->audio.id3_year);
				break;
			case 'X':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->audio.bitrate);
				break;
			case 'z':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->audio.samplingrate);
				break;
			case 'h':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->audio.codec);
				break;

			case 'q':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->audio.layer);
				break;
			case 'Q':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->audio.channelmode);
				break;
			case 'i':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->audio.vbr_version_string);
				break;
			case 'I':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->audio.vbr_preset);
				break;
			}
		} else
			*out_p++ = *instr;
	*out_p = 0;
	return output2;
}

char           *
convert5(char *instr)
{
	int		val1, val2;
	char           *out_p;
	char           *m;
	char		ctrl[15];

	out_p = output2;

	bzero(out_p, (int)sizeof(out_p));
	bzero(ctrl, (int)sizeof(ctrl));

	for (; *instr; instr++)
		if (*instr == '%') {
			instr++;
			m = instr;
			if (*instr == '-' && isdigit(*(instr + 1)))
				instr += 2;
			while (isdigit(*instr))
				instr++;
			if (m != instr && instr-m < (int)sizeof(ctrl)) {
				sprintf(ctrl, "%.*s", (int)(instr - m), m);
				val1 = strtol(ctrl, NULL, 10);
			} else {
				val1 = 0;
			}
			if (*instr == '.') {
				instr++;
				m = instr;
				if (*instr == '-' && isdigit(*(instr + 1)))
					instr += 2;
				while (isdigit(*instr))
					instr++;
				if (m != instr && instr-m < (int)sizeof(ctrl)) {
					sprintf(ctrl, "%.*s", (int)(instr - m), m);
					val2 = strtol(ctrl, NULL, 10);
				} else {
					val2 = 0;
				}
			} else {
				val2 = -1;
			}

			switch (*instr) {
			case 'Z':
				out_p += sprintf(out_p, "%*s", val1, short_sitename);
				break;
			case '%':
				*out_p++ = *instr;
			}
		} else
			*out_p++ = *instr;
	*out_p = 0;
	return output2;
}


/*
 * Modified: 01.23.2002
 */
char           *
convert(struct VARS *raceI, struct USERINFO **userI, struct GROUPINFO **groupI, char *instr)
{
	int		val1, val2, n;
	int		from, to, reverse;
	char		*out_p;
	char		*m;
	char		ttime[40], ctrl[15];

	out_p = output;

	bzero(out_p, (int)sizeof(out_p));
	bzero(ctrl, (int)sizeof(ctrl));

	for (; *instr; instr++)
		if (*instr == '%') {
			instr++;
			m = instr;
			if (*instr == '-' && isdigit(*(instr + 1)))
				instr += 2;
			while (isdigit(*instr))
				instr++;
			if (m != instr && instr-m < (int)sizeof(ctrl)) {
				sprintf(ctrl, "%.*s", (int)(instr - m), m);
				val1 = strtol(ctrl, NULL, 10);
			} else {
				val1 = 0;
			}

			if (*instr == '.') {
				instr++;
				m = instr;
				if (*instr == '-' && isdigit(*(instr + 1)))
					instr += 2;
				while (isdigit(*instr))
					instr++;
				if (m != instr && instr-m < (int)sizeof(ctrl)) {
					sprintf(ctrl, "%.*s", (int)(instr - m), m);
					val2 = strtol(ctrl, NULL, 10);
				} else {
					val2 = 0;
				}
			} else {
				val2 = -1;
			}

			switch (*instr) {
			case 'a':
				out_p += sprintf(out_p, "%*.*f", val1, val2, (double)(raceI->total.speed / 1024 / raceI->total.size));
				break;
			case 'A':
				out_p += sprintf(out_p, "%*.*f", val1, val2,
						 (double)((raceI->total.size / (raceI->total.stop_time - raceI->total.start_time)) / 1024));
				break;
			case 'b':
				out_p += sprintf(out_p, "%*u", val1, (unsigned int)raceI->total.size);
				break;	/* what about files bigger than 4gb? */
//			case 'B':
//				out_p += sprintf(out_p, "\\002");
//				break;
			case 'K':
				out_p += sprintf(out_p, "%s", raceI->user.tagline);
				break;
			case 'c':
				from = to = reverse = 0;
				instr++;
				m = instr;
				if (*instr == '-') {
					reverse = 1;
					instr++;
				}
				for (; isdigit(*instr); instr++) {
					from *= 10;
					from += *instr - 48;
				}

				if (*instr == '-') {
					instr++;
					for (; isdigit(*instr); instr++) {
						to *= 10;
						to += *instr - 48;
					}
					if (to == 0 || to >= raceI->total.groups) {
						to = raceI->total.groups - 1;
					}
				}
				if (to < from) {
					to = from;
				}
				if (reverse == 1) {
					n = from;
					from = raceI->total.groups - 1 - to;
					to = raceI->total.groups - 1 - n;
				}
				if (from >= raceI->total.groups) {
					to = -1;
				}
				for (n = from; n <= to; n++) {
					out_p += sprintf(out_p, "%*.*s", val1, val2, convert3(raceI, groupI[groupI[n]->pos], group_info, n));
				}
				instr--;
				break;
			case 'C':
				from = to = reverse = 0;
				instr++;
				m = instr;
				if (*instr == '-') {
					reverse = 1;
					instr++;
				}
				for (; isdigit(*instr); instr++) {
					from *= 10;
					from += *instr - 48;
				}

				if (*instr == '-') {
					instr++;
					for (; isdigit(*instr); instr++) {
						to *= 10;
						to += *instr - 48;
					}
					if (to == 0 || to >= raceI->total.users) {
						to = raceI->total.users - 1;
					}
				}
				if (to < from) {
					to = from;
				}
				if (reverse == 1) {
					n = from;
					from = raceI->total.users - 1 - to;
					to = raceI->total.users - 1 - n;
				}
				if (from >= raceI->total.users) {
					to = -1;
				}
				for (n = from; n <= to; n++) {
					out_p += sprintf(out_p, "%*.*s", val1, val2, convert2(raceI, userI[userI[n]->pos], groupI, user_info, n));
					break;
				}
				instr--;
				break;
			case 'd':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)hms(ttime, raceI->total.stop_time - raceI->total.start_time));
				break;
			case '$':
				out_p += sprintf(out_p, "%*.*s", val1, val2,
						 (char *)hms(ttime, ((((raceI->total.stop_time - raceI->total.start_time) + (raceI->total.files - raceI->total.files_missing)) / (raceI->total.files - raceI->total.files_missing)) * raceI->total.files) - (raceI->total.stop_time - raceI->total.start_time)));
				break;
			case 'e':
				out_p += sprintf(out_p, "%*.*f", val1, val2, (double)((raceI->file.size * raceI->total.files >> 10) / 1024.));
				break;
			case 'f':
				out_p += sprintf(out_p, "%*i", val1, (int)raceI->total.files);
				break;
			case 'F':
				out_p += sprintf(out_p, "%*i", val1, (int)raceI->total.files - (int)raceI->total.files_missing);
				break;
			case 'g':
				out_p += sprintf(out_p, "%*i", val1, (int)raceI->total.groups);
				break;
			case 'G':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->user.group);
				break;
			case 'k':
				out_p += sprintf(out_p, "%*.*f", val1, val2, (double)(raceI->total.size / 1024.));
				break;
			case 'l':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)convert2(raceI, userI[raceI->misc.slowest_user[1]], groupI, slowestfile, 0));
				break;
			case 'L':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)convert2(raceI, userI[raceI->misc.fastest_user[1]], groupI, fastestfile, 0));
				break;
			case 'm':
				out_p += sprintf(out_p, "%*.*f", val1, val2, (double)((raceI->total.size >> 10) / 1024.));
				break;
			case 'M':
				out_p += sprintf(out_p, "%*i", val1, (int)raceI->total.files_missing);
				break;
			case 'n':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->file.name);
				break;
			case 'o':
				out_p += sprintf(out_p, "%*i", val1, (int)raceI->total.files_bad);
				break;
			case 'O':
				out_p += sprintf(out_p, "%*.*f", val1, val2, (double)((raceI->total.bad_size >> 10) / 1024.));
				break;
			case 'p':
				out_p += sprintf(out_p, "%*.*f", val1, val2, (double)((raceI->total.files - raceI->total.files_missing) * 100. / raceI->total.files));
				break;
			case 'P':
				out_p += sprintf(out_p, "%*.*f", val1, val2, (double)(raceI->total.bad_size / 1024.));
				break;
			case 'S':
				out_p += sprintf(out_p, "%*.*f", val1, val2, (double)(raceI->file.speed / 1024));
				break;	/* kbps */
			case 'r':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->misc.release_name);
				break;
			case 'R':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->misc.racer_list);
				break;
			case 'B':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->misc.total_racer_list);
				break;
			case 't':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->misc.top_messages[1] + 1);
				break;
			case 'T':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->misc.top_messages[0] + 1);
				break;
			case 'u':
				out_p += sprintf(out_p, "%*i", val1, (int)raceI->total.users);
				break;
			case 'U':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->user.name);
				break;
			case 'v':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->misc.error_msg);
				break;
			case 'V':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->misc.progress_bar);
				break;

				/* Audio */

			case 'w':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->audio.id3_genre);
				break;
			case 'W':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->audio.id3_album);
				break;
			case 'x':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->audio.id3_artist);
				break;
			case 'y':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->audio.id3_title);
				break;
			case 'Y':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->audio.id3_year);
				break;
			case 'X':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->audio.bitrate);
				break;
			case 'z':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->audio.samplingrate);
				break;
			case 'h':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->audio.codec);
				break;
			case 'q':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->audio.layer);
				break;
			case 'Q':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->audio.channelmode);
				break;
			case 'j':
				if (raceI->audio.is_vbr == 1)
					out_p += sprintf(out_p, "%*.*s", val1, val2, convert4(raceI, audio_vbr));
				else
					out_p += sprintf(out_p, "%*.*s", val1, val2, convert4(raceI, audio_cbr));
				break;
			case 'i':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->audio.vbr_version_string);
				break;
			case 'I':
				out_p += sprintf(out_p, "%*.*s", val1, val2, (char *)raceI->audio.vbr_preset);
				break;

				/* Video */

			case 'D':
				out_p += sprintf(out_p, "%*i", val1, raceI->video.width);
				break;
			case 'E':
				out_p += sprintf(out_p, "%*i", val1, raceI->video.height);
				break;
			case 'H':
				out_p += sprintf(out_p, "%*s", val1, raceI->video.fps);
				break;

				/* Other */

			case 'J':
				*out_p++ = raceI->file.compression_method;
				break;
			case 'Z':
				out_p += sprintf(out_p, "%*s", val1, short_sitename);
				break;
			case '%':
				*out_p++ = *instr;
				break;
			case '?':
				out_p += sprintf(out_p, "%*s", val1, raceI->misc.current_path);
				break;
			}
		} else
			*out_p++ = *instr;
	*out_p = 0;
	return output;
}

/* Converts cookies in incomplete indicators */
char		normal_buf	[FILE_MAX];
char		nfo_buf		[FILE_MAX];
char		sfv_buf		[FILE_MAX];
char           *
incomplete(char *instr, char path[2][PATH_MAX], struct VARS *raceI, int l_type)
{
	char           *buf_p;

	if (l_type == INCOMPLETE_NORMAL)
		buf_p = normal_buf;
	else if (l_type == INCOMPLETE_NFO)
		buf_p = nfo_buf;
	else if (l_type == INCOMPLETE_SFV)
		buf_p = sfv_buf;
	else
		return NULL;

	bzero(buf_p, FILE_MAX);

	for (; *instr; instr++)
		if (*instr == '%') {
			instr++;
			switch (*instr) {
			case '2':
				buf_p += sprintf(buf_p, "%s", raceI->sectionname);
				break;
			case '1':
				buf_p += sprintf(buf_p, "%s", path[0]);
				break;
			case '0':
				buf_p += sprintf(buf_p, "%s", path[1]);
				break;
			case '%':
				*buf_p++ = '%';
				break;
			}
		} else {
			*buf_p++ = *instr;
		}
	*buf_p = 0;
	if (l_type == INCOMPLETE_NORMAL)
		return normal_buf;
	else if (l_type == INCOMPLETE_NFO)
		return nfo_buf;
	else if (l_type == INCOMPLETE_SFV)
		return sfv_buf;
	else
		return NULL;
}

