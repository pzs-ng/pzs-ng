#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>

#include "objects.h"
#include "macros.h"
#include "convert.h"

#include "../conf/zsconfig.h"

struct	userdata	{
	long	allup_bytes;
	long	monthup_bytes;
	long	wkup_bytes;
	long	dayup_bytes;
	short	name;
};



/*
 * Modified   : 01.27.2002
 * Author     : Dark0n3
 *
 * Description: Updates existing entries in userI and groupI or creates new, if old doesnt exist
 *
 */
void updatestats(struct VARS *raceI, struct USERINFO **userI, struct GROUPINFO **groupI, char *usern, char *group, unsigned long filesize, long speed, long startsec, long startusec) {
 int		u_no = -1;
 int		g_no = -1;
 int		n;
 double		speedD = filesize / speed;

 for (n = 0; n < raceI->total.users; n++) {
	if (strncmp(userI[n]->name, usern, 24) == 0) {
		u_no = n;
		g_no = userI[n]->group;
		break;
		}
	}

 if ( u_no == -1 ) {
//	if ( ! raceI->total.users ) {
//		raceI->transfer_start.tv_sec = startsec;
//		raceI->transfer_start.tv_usec = startusec;
//		}
	u_no = raceI->total.users++;
	userI[u_no] = malloc(sizeof(struct USERINFO));
	bzero(userI[u_no], sizeof(struct USERINFO));
	memcpy(userI[u_no]->name, usern, 24);

	for (n = 0; n < raceI->total.groups; n++ ) {
		if (strncmp(groupI[n]->name, group, 24) == 0) {
			g_no = n;
			break;
			}
		}

	if ( g_no == -1 ) {
		g_no = raceI->total.groups++;
		groupI[g_no] = malloc(sizeof(struct GROUPINFO));
		bzero(groupI[g_no], sizeof(struct GROUPINFO));
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

 if ( speed > raceI->misc.fastest_user[0] ) {
	raceI->misc.fastest_user[1] = u_no;
	raceI->misc.fastest_user[0] = speed;
	}

 if ( speed < raceI->misc.slowest_user[0] ) {
	raceI->misc.slowest_user[1] = u_no;
	raceI->misc.slowest_user[0] = speed;
	}
}





/*
 * Modified   : 01.17.2002
 * Author     : Dark0n3
 *
 * Description: Sorts userI and groupI
 *
 */
void sortstats(struct VARS *raceI, struct USERINFO **userI, struct GROUPINFO **groupI) {
 int	n;
 int	n2;
 int	t;
 int	*p_array;
 char	*r_list;

 p_array = (int *)m_alloc(raceI->total.users * sizeof(int));
 bzero(p_array, raceI->total.users * sizeof(int));
 r_list = raceI->misc.racer_list;

 for ( n = 0 ; n < raceI->total.users ; n++ ) {
	t = p_array[n];
	for ( n2 = n + 1; n2 < raceI->total.users; n2++ ) {
		if (userI[n]->bytes >= userI[n2]->bytes) {
			p_array[n2]++;
			} else {
			t++;
			}
		}
	userI[t]->pos = n;
#if ( get_competitor_list == TRUE )
	if ( strcmp(raceI->user.name, userI[n]->name) ) {
		r_list += sprintf(r_list, " %s", convert2(raceI, userI[n], groupI, racersmsg, t));
		} else {
		raceI->user.pos = n;
		}
#else
	if ( ! strcmp(raceI->user.name, userI[n]->name) ) raceI->user.pos = n;
#endif
	}
 bzero(p_array, raceI->total.groups * sizeof(int));

 for ( n = 0 ; n < raceI->total.groups ; n++ ) {
	t = p_array[n];
	for ( n2 = n + 1; n2 < raceI->total.groups ; n2++ ) {
		if (groupI[n]->bytes >= groupI[n2]->bytes) {
			p_array[n2]++;
			} else {
			t++;
			}
		}
	groupI[t]->pos = n;
	}
 m_free(p_array);
}




void showstats(struct VARS *raceI, struct USERINFO **userI, struct GROUPINFO **groupI) {
 int	cnt;

#if ( show_user_info == TRUE )
 printf(convert(raceI, userI, groupI, realtime_user_header));

 for ( cnt = 0 ; cnt < raceI->total.users ; cnt++ ) {
	printf(convert2(raceI, userI[userI[cnt]->pos], groupI, realtime_user_body, cnt));
	}
 printf(convert(raceI, userI, groupI, realtime_user_footer));
#endif
#if ( show_group_info == TRUE )
 printf(convert(raceI, userI, groupI, realtime_group_header));

 for ( cnt = 0 ; cnt < raceI->total.groups ; cnt++ ) {
	printf(convert3(raceI, groupI[groupI[cnt]->pos], realtime_group_body, cnt));
	}
 printf( convert(raceI, userI, groupI, realtime_group_footer) );
#endif
}






/* Created	: 01.15.2002
 * Modified	: 01.17.2002
 * Author	: Dark0n3
 * 
 * Description	: Reads transfer stats for all users from userfiles and creates
 *		  which is used to set dayup/weekup/monthup/allup variables for
 *		  every user in race.
 */
void get_stats(struct VARS *raceI, struct USERINFO **userI) {
 int			users;
 int			u1;
 int			n, m;
 int			fd;
 unsigned char		space;
 unsigned char		args;
 unsigned char		shift;
 char			*buf, *p_buf, *eof;
 char			*arg[31]; /* Enough to hold 10 sections (noone has more?) */
 struct dirent		**userlist;
 struct userdata	**user;
 struct stat		fileinfo;

 users = scandir(gl_userfiles, &userlist, 0, 0);
 user = m_alloc(users * sizeof(int));
 shift = 0;

	/* User stats reader */

 for ( n = 0 ; n < users ; n++ ) {
	buf = m_alloc(sizeof(gl_userfiles) + strlen(userlist[n + shift]->d_name) + 2);
	sprintf(buf, "%s/%s", gl_userfiles, userlist[n + shift]->d_name);
	fd = open(buf, O_RDONLY);
	m_free(buf);

	fileinfo.st_mode = 0;
	fstat(fd, &fileinfo);
	if ( S_ISDIR(fileinfo.st_mode) == 0 ) {

		eof = buf = m_alloc(fileinfo.st_size);
		eof += fileinfo.st_size;

		user[n] = malloc(sizeof(struct userdata));
		bzero(user[n], sizeof(struct userdata));

		read(fd, buf, fileinfo.st_size);

		args = 0;
		space = 1;

		for ( p_buf = buf ; p_buf < eof ; p_buf++ ) switch ( *p_buf ) {
			case '\n':
				*p_buf = 0;
				if ( !memcmp(arg[0], "DAYUP", 5)) user[n]->dayup_bytes = atol(arg[2]);
				 else if ( !memcmp(arg[0], "WKUP", 4)) user[n]->wkup_bytes = atol(arg[2]);
				 else if ( !memcmp(arg[0], "MONTHUP", 7)) user[n]->monthup_bytes = atol(arg[2]);
				 else if ( !memcmp(arg[0], "ALLUP", 5)) user[n]->allup_bytes = atol(arg[2]);
				args = 0;
				space = 1;
				break;
			case '\t':
			case ' ':
				*p_buf = 0;
				space = 1;
				break;
			default :
				if ( space && args < 30 ) {
					space = 0;
					arg[args] = p_buf;
					args++;
					}
				break;
		}

		m_free(buf);

		user[n]->name = -1;

		for ( m = 0; m < raceI->total.users ; m++ ) {
			if ( strcmp(userlist[n + shift]->d_name, userI[m]->name) != 0 ) continue;
			if ( ! strcmp(raceI->user.name, userI[m]->name)) {
				user[n]->dayup_bytes += raceI->file.size >> 10;
				user[n]->wkup_bytes += raceI->file.size >> 10;
				user[n]->monthup_bytes += raceI->file.size >> 10;
				user[n]->allup_bytes += raceI->file.size >> 10;
				}
			user[n]->name = m;
			break;
			}
		} else {
		shift++;
		users--;
		n--;
		}

	free(userlist[n + shift]);
	close(fd);
	}
 free(userlist);

 for ( m = 0 ; m < raceI->total.users ; m++ ) {
	userI[m]->dayup =
	userI[m]->wkup =
	userI[m]->monthup =
	userI[m]->allup = users;
	}

	/* Sort */

 for ( n = 0 ; n < users ; n++ ) {
	if ( (u1 = user[n]->name) == -1 ) continue;
	for ( m = 0 ; m < users ; m++ ) if ( m != n ) {
		if (user[n]->wkup_bytes >= user[m]->wkup_bytes ) {
			userI[u1]->wkup--;
			}
		if (user[n]->monthup_bytes >= user[m]->monthup_bytes ) {
			userI[u1]->monthup--;
			}
		if (user[n]->allup_bytes >= user[m]->allup_bytes ) {
			userI[u1]->allup--;
			}
		if (user[n]->dayup_bytes >= user[m]->dayup_bytes ) {
			userI[u1]->dayup--;
			}
		}
	}
 for ( n = 0 ; n < users ; n++ ) m_free(user[n]);
 m_free(user);
}
