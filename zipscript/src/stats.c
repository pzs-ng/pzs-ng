#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>

#include "objects.h"
#include "macros.h"
#include "convert.h"
#include "zsfunctions.h"
#include "race-file.h"

#ifdef _SunOS_
#include "scandir.h"
#endif

#include "../conf/zsconfig.h"
#include "../include/zsconfig.defaults.h"

#include "stats.h"

void
updatestats_free(GLOBAL *g)
{
	int n;

	for (n = 0; n < g->v.total.users; n++)
		ng_free(g->ui[n]);
	ng_free(g->ui);

	for (n = 0; n < g->v.total.groups; n++)
		ng_free(g->gi[n]);
	ng_free(g->gi);
}

/*
 * Modified   : 01.27.2002 Author     : Dark0n3
 * 
 * Description: Updates existing entries in userI and groupI or creates new, if
 * old doesnt exist
 */
void 
updatestats(struct VARS *raceI, struct USERINFO **userI, struct GROUPINFO **groupI, char *usern, char *group, off_t filesize, unsigned int speed, unsigned int start_time)
{
	int		u_no = -1;
	int		g_no = -1;
	int		n;
	double		speedD = filesize * speed;

	for (n = 0; n < raceI->total.users; n++) {
		if (strncmp(userI[n]->name, usern, 24) == 0) {
			u_no = n;
			g_no = userI[n]->group;
			break;
		}
	}

	if (u_no == -1) {
		if (!raceI->total.users) {
			raceI->total.start_time = start_time;
			/*
			 * to prevent a possible floating point exception in
			 * convert,
			 */
			/* if first entry in racefile is not the oldest one           */
			if ((int)(raceI->total.stop_time - raceI->total.start_time) < 1)
				raceI->total.stop_time = raceI->total.start_time + 1;
		}
		u_no = raceI->total.users++;
		ng_free(userI[u_no]);
		userI[u_no] = ng_realloc(userI[u_no], sizeof(struct USERINFO), 1, 1, raceI, 1);
		memcpy(userI[u_no]->name, usern, 24);
		userI[u_no]->files = 0;

		for (n = 0; n < raceI->total.groups; n++) {
			if (strncmp(groupI[n]->name, group, 24) == 0) {
				g_no = n;
				break;
			}
		}

		if (g_no == -1) {
			g_no = raceI->total.groups++;
			ng_free(groupI[g_no]);

			groupI[g_no] = ng_realloc(groupI[g_no], sizeof(struct GROUPINFO), 1, 1, raceI, 1);
			memcpy(groupI[g_no]->name, group, 24);
		}
		userI[u_no]->group = g_no;
	}
	userI[u_no]->bytes += filesize;
	groupI[g_no]->bytes += filesize;
	raceI->total.size += filesize;

	userI[u_no]->speed += speedD;
	groupI[g_no]->speed += speedD;
	raceI->total.speed += speedD;

	userI[u_no]->files++;
	groupI[g_no]->files++;
	raceI->total.files_missing--;

	speed >>= 10;

	if (speed > (unsigned int)raceI->misc.fastest_user[0]) {
		raceI->misc.fastest_user[1] = u_no;
		raceI->misc.fastest_user[0] = speed;
	}
	if (speed < (unsigned int)raceI->misc.slowest_user[0]) {
		raceI->misc.slowest_user[1] = u_no;
		raceI->misc.slowest_user[0] = speed;
	}
}

/*
 * Modified   : 01.17.2002 Author     : Dark0n3
 * 
 * Description: Sorts userI and groupI
 * Modified by psxc 09.25.2004 - nasty bug
 * Modified by psxc 11.22.2004 - added support for list of ppl against leader.
 * Modified by psxc 12.01.2004 - fixed this routine! :)
 */
void 
sortstats(struct VARS *raceI, struct USERINFO **userI, struct GROUPINFO **groupI)
{
	int		n;
	int		n2;
	int		t;
	int		t2 = 0, t3 = 0;
	int            *p_array = NULL;
	char           *r_list;
	char           *t_list;

	p_array = (int *)ng_realloc(p_array, raceI->total.users * sizeof(int), 1, 1, raceI, 1);
	r_list = raceI->misc.racer_list;
	t_list = raceI->misc.total_racer_list;

	for (n = 0; n < raceI->total.users; n++) {
		t = p_array[n];
		for (n2 = n + 1; n2 < raceI->total.users; n2++) {
			if (userI[n]->bytes > userI[n2]->bytes) {
				p_array[n2]++;
			} else {
				t++;
			}
		}
		userI[t]->pos = n;
#if ( get_competitor_list == TRUE )
		if ( (strncmp(raceI->user.name, userI[n]->name, (int)strlen(raceI->user.name) < (int)strlen(userI[n]->name) ? (int)strlen(raceI->user.name) : (int)strlen(userI[n]->name))) || (((int)strlen(raceI->user.name) != (int)strlen(userI[n]->name)) && (!strncmp(raceI->user.name, userI[n]->name, (int)strlen(raceI->user.name) < (int)strlen(userI[n]->name) ? (int)strlen(raceI->user.name) : (int)strlen(userI[n]->name))))) {
			if (t2 > 0) {
				if ((int)strlen(racersplit)) {
					r_list += sprintf(r_list, " %s %s", racersplit, convert2(raceI, userI[n], groupI, racersmsg, t));
				} else {
					r_list += sprintf(r_list, " %s", convert2(raceI, userI[n], groupI, racersmsg, t));
				}
			} else if ((int)strlen(racersplit_prior)) {
				t2 = 1;
				r_list += sprintf(r_list, "%s %s", racersplit_prior, convert2(raceI, userI[n], groupI, racersmsg, t));
			} else {
				t2 = 1;
				r_list += sprintf(r_list, "%s", convert2(raceI, userI[n], groupI, racersmsg, t));
			}
		} else {
			raceI->user.pos = n;
		}
#else
		if (!strcmp(raceI->user.name, userI[n]->name))
			raceI->user.pos = n;
#endif
	}
	for (n = 1; n < raceI->total.users; n++) {
		if (t3 > 0) {
			if ((int)strlen(racersplit)) {
				t_list += sprintf(t_list, " %s %s", racersplit, convert2(raceI, userI[userI[n]->pos], groupI, racersmsg, n));
			} else {
				t_list += sprintf(t_list, " %s", convert2(raceI, userI[userI[n]->pos], groupI, racersmsg, n));
			}
		} else if ((int)strlen(racersplit_prior)) {
			t3 = 1;
			t_list += sprintf(t_list, "%s %s", racersplit_prior, convert2(raceI, userI[userI[n]->pos], groupI, racersmsg, n));
		} else {
			t3 = 1;
			t_list += sprintf(t_list, "%s", convert2(raceI, userI[userI[n]->pos], groupI, racersmsg, n));
		}
	}
	bzero(p_array, raceI->total.groups * sizeof(int));

	for (n = 0; n < raceI->total.groups; n++) {
		t = p_array[n];
		for (n2 = n + 1; n2 < raceI->total.groups; n2++) {
			if (groupI[n]->bytes >= groupI[n2]->bytes) {
				p_array[n2]++;
			} else {
				t++;
			}
		}
		groupI[t]->pos = n;
	}
	ng_free(p_array);
}

void 
showstats(struct VARS *raceI, struct USERINFO **userI, struct GROUPINFO **groupI)
{
	int		cnt;

#if ( show_user_info == TRUE )
	d_log("showstats: Showing realtime user stats ...\n");
	if (realtime_user_header != DISABLED) {
		d_log("showstats:   - printing realtime_user_header ...\n");
		printf("%s", convert(raceI, userI, groupI, realtime_user_header));
	}
	if (realtime_user_body != DISABLED) {
		d_log("showstats:   - printing realtime_user_body ...\n");
		for (cnt = 0; cnt < raceI->total.users; cnt++) {
			printf("%s", convert2(raceI, userI[userI[cnt]->pos], groupI, realtime_user_body, cnt));
		}
	}
	if (realtime_user_footer != DISABLED) {
		d_log("showstats:   - printing realtime_user_footer ...\n");
		printf("%s", convert(raceI, userI, groupI, realtime_user_footer));
	}
#endif
#if ( show_group_info == TRUE )
	d_log("showstats: Showing realtime user stats ...\n");
	if (realtime_group_header != DISABLED) {
		d_log("showstats:   - printing realtime_group_header ...\n");
		printf("%s", convert(raceI, userI, groupI, realtime_group_header));
	}
	if (realtime_group_body != DISABLED) {
		d_log("showstats:   - printing realtime_group_body ...\n");
		for (cnt = 0; cnt < raceI->total.groups; cnt++) {
			printf("%s", convert3(raceI, groupI[groupI[cnt]->pos], realtime_group_body, cnt));
		}
	}
	if (realtime_group_footer != DISABLED) {
		d_log("showstats:   - printing realtime_group_footer ...\n");
		printf("%s", convert(raceI, userI, groupI, realtime_group_footer));
	}
#endif
}

/*
 * Created	: 01.15.2002 Modified	: 01.17.2002 Author	: Dark0n3
 * 
 * Description	: Reads transfer stats for all users from userfiles and
 * creates which is used to set dayup/weekup/monthup/allup variables for
 * every user in race.
 */
void 
get_stats(struct VARS *raceI, struct USERINFO **userI)
{
	int		u1;
	int		n = 0, m, users = 0;
	int		fd;
	unsigned char	space;
	unsigned char	args;
	char		*p_buf = 0, *eof = 0;
	char		t_buf[PATH_MAX], *f_buf = 0; /* target buf, file buf */
	char		*arg[45]; /* Enough to hold 10 sections (noone has more?) */
	struct userdata	*user = 0;
	struct stat	fileinfo;

	DIR		*dir;
	struct dirent	*dp;

	if (!(dir = opendir(gl_userfiles))) {
		d_log("get_stats: opendir(%s): %s\n", gl_userfiles, strerror(errno));
		return; 
	}
	
	/* User stats reader */
	while ((dp = readdir(dir))) {
		
		sprintf(t_buf, "%s/%s", gl_userfiles, dp->d_name);

		if ((fd = open(t_buf, O_RDONLY)) == -1) {
			d_log("get_stats: open(%s): %s\n", t_buf, strerror(errno));
			continue;
		}
		
		fileinfo.st_mode = 0;
		if (fstat(fd, &fileinfo) == -1) {
			d_log("get_stats: fstat(%i): %s\n", fd, strerror(errno));
			continue;
		}
		
		if (S_ISDIR(fileinfo.st_mode) == 0) {

			if (!update_lock(raceI, 1, 0)) {
				d_log("get_stats: Lock is suggested removed. Will comply and exit\n");
				remove_lock(raceI);
				exit(EXIT_FAILURE);
			}

			user = ng_realloc(user, sizeof(struct userdata)*(n+1), 0, 1, raceI, 0);
			bzero(&user[n], (sizeof(struct userdata)));

			eof = f_buf = ng_realloc(f_buf, fileinfo.st_size, 1, 1, raceI, 0);
			eof += fileinfo.st_size;

			if (read(fd, f_buf, fileinfo.st_size) == -1) {
				d_log("get_stats: failed to read stats: %s\n", strerror(errno));
				remove_lock(raceI);
				exit(EXIT_FAILURE);
			}
			close(fd);

			args = 0;
			space = 1;

			for (p_buf = f_buf; p_buf < eof; p_buf++)
				switch (*p_buf) {
					case '\n':
						*p_buf = 0;
						if (!memcmp(arg[0], "DAYUP", 5))
							user[n].dayup_bytes = atoi(arg[raceI->section * 3 + 2]);
						else if (!memcmp(arg[0], "WKUP", 4))
							user[n].wkup_bytes = atoi(arg[raceI->section * 3 + 2]);
						else if (!memcmp(arg[0], "MONTHUP", 7))
							user[n].monthup_bytes = atoi(arg[raceI->section * 3 + 2]);
						else if (!memcmp(arg[0], "ALLUP", 5))
							user[n].allup_bytes = atoi(arg[raceI->section * 3 + 2]);
						args = 0;
						space = 1;
						break;
					case '\t':
					case ' ':
						*p_buf = 0;
						space = 1;
						break;
					default:
						if (space && args < 30) {
							space = 0;
							arg[args] = p_buf;
							args++;
						}
						break;
				}

			user[n].name = -1;

			for (m = 0; m < raceI->total.users; m++) {
				if (strcmp(dp->d_name, userI[m]->name) != 0)
					continue;
				if (!strcmp(raceI->user.name, userI[m]->name)) {
					user[n].dayup_bytes += raceI->file.size >> 10;
					user[n].wkup_bytes += raceI->file.size >> 10;
					user[n].monthup_bytes += raceI->file.size >> 10;
					user[n].allup_bytes += raceI->file.size >> 10;
				}
				user[n].name = m;
				break;
			}
			n++;
		}
	}
	closedir(dir);

	users = n;
	for (m = 0; m < raceI->total.users; m++) {
		userI[m]->dayup =
			userI[m]->wkup =
			userI[m]->monthup =
			userI[m]->allup = users;
	}

	/* Sort */
	for (n = 0; n < users; n++) {
		if ((u1 = user[n].name) == -1)
			continue;
		for (m = 0; m < users; m++)
			if (m != n) {
				if (user[n].wkup_bytes >= user[m].wkup_bytes) {
					userI[u1]->wkup--;
				}
				if (user[n].monthup_bytes >= user[m].monthup_bytes) {
					userI[u1]->monthup--;
				}
				if (user[n].allup_bytes >= user[m].allup_bytes) {
					userI[u1]->allup--;
				}
				if (user[n].dayup_bytes >= user[m].dayup_bytes) {
					userI[u1]->dayup--;
				}
			}
	}
	ng_free(f_buf);
	ng_free(user);
}
