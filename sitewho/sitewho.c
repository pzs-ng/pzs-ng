#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>
#include "structonline.h"

struct GROUP {
	char           *name;
	gid_t		id;
};

int		groups = 0, GROUPS = 0;

static struct ONLINE *user;
static struct GROUP **group;

long long	shmid;
struct shmid_ds	ipcbuf;
struct stat	filestat;

char           *header, *footer, *glpath, *mpaths, *husers, *ipckey, *glgroup,
               *def_ipckey = "0x0000DEAD", *def_glgroup = "/etc/group";
int		maxusers  , showall = 0, uploads = 0, downloads = 0, onlineusers = 0;
double		total_dn_speed = 0, total_up_speed = 0;

unsigned long 
filesize(char *filename)
{
	char           *file;

	file = malloc(strlen(glpath) + strlen(filename) + 2);
	sprintf(file, "%s/%s", glpath, filename);

	if (stat(file, &filestat) != 0) {
		if (!strcmp(filename, "")) {
			filestat.st_size = 1;
		} else {
			fprintf(stderr, "Could not stat file '%s', is glrootpath set correctly in sitewho.conf? (error: %s)\n", file, strerror(errno));
			free(file);
			exit(1);
		}
	}
	free(file);
	return filestat.st_size;
}

char           *
get_g_name(unsigned int gid)
{
	int		n;

	for (n = 0; n < groups; n++) {
		if (group[n]->id == gid)
			return group[n]->name;
	}

	return "NoGroup";
}

int 
strplen(char *strin)
{
	int		n = 0;

	while (isprint(strin[n]) && strin[n])
		n++;

	return n;
}

short 
matchpath(char *instr, char *path)
{
	int		cnt       , pos, k;

	k = strlen(instr) + 1;
	for (cnt = pos = 0; cnt < k; cnt++) {
		if (instr[cnt] == ' ' || instr[cnt] == 0) {
			if (!strncmp(instr + cnt - pos, path, pos - 1))
				return 1;
			pos = 0;
		} else
			pos++;
	}
	return 0;
}

short 
strcomp(char *instr, char *searchstr)
{
	int		cnt       , pos;
	int		k = strlen(searchstr);
	int		l = strlen(instr) + 1;

	for (cnt = pos = 0; cnt < l; cnt++) {
		if (instr[cnt] == ' ' || instr[cnt] == 0) {
			if (k == pos && !strncmp(instr + cnt - pos, searchstr, pos - 1)) {
				return 1;
			}
			pos = 0;
		} else {
			pos++;
		}
	}
	return 0;
}

void 
showusers(int n, int mode, char *ucomp, char raw)
{
	char		status    [20];
	char		online    [20];
	char           *filename = 0;
	char		realfile  [512];
	char		bar       [20];
	struct timeval	tstop;
	double		mb_xfered = 0;
	double		speed  , pct = 0;
	double		my_filesize = 0;
	int		mask;
	int		noshow;
	int		maskchar;
	int		i         , x, m;
	unsigned	hours;
	unsigned char	minutes;
	unsigned	seconds;

	gettimeofday(&tstop, (struct timezone *)0);

	for (x = 0; x < n; x++) {
		if (!user[x].procid)
			continue;

		maskchar = ' ';
		mask = noshow = 0;

		if (strcomp(husers, user[x].username) != 0) {
			if (showall)
				maskchar = '*';
			else
				noshow++;
		}
		if (noshow == 0) {
			if (maskchar == ' ' && matchpath(mpaths, user[x].currentdir) != 0) {
				if (showall)
					maskchar = '*';
				else
					mask++;
			}
		}
		if (strplen(user[x].status) > 5)
			filename = malloc(strplen(user[x].status) - 5 + 1);
		else
			filename = malloc(1);

		if ((strncasecmp(user[x].status, "STOR ", 5) == 0 ||
		     strncasecmp(user[x].status, "APPE ", 5) == 0) &&
		    user[x].bytes_xfer != 0 && mask == 0) {

			pct = -1;
			m = strplen(user[x].status) - 5;
			if (m < 15 || raw)
				sprintf(filename, "%.*s", m, user[x].status + 5);
			else
				sprintf(filename, "%.15s", user[x].status + m - 10);

			strcpy(bar, "?->");
			speed = user[x].bytes_xfer / 1024. /
				((tstop.tv_sec - user[x].tstart.tv_sec) * 1. +
			(tstop.tv_usec - user[x].tstart.tv_usec) / 1000000.);

			total_up_speed += speed;
			uploads++;
			if (!raw)
				sprintf(status, "Up: %7.1fKBs", speed);
			else if (raw == 1)
				sprintf(status, "\"UP\" \"%.1f\"", speed);
			else
				sprintf(status, "upld|%.1f", speed);

			mb_xfered = user[x].bytes_xfer * 1.0 / 1024 / 1024;
		} else if ((!strncasecmp(user[x].status, "RETR ", 5) && user[x].bytes_xfer) && mask == 0) {
			mb_xfered = 0;
			m = strplen(user[x].status) - 5;

			sprintf(realfile, "%s", user[x].currentdir);

			if (m < 15 || raw)
				sprintf(filename, "%.*s", m, user[x].status + 5);
			else
				sprintf(filename, "%.15s", user[x].status + m - 10);

			/*
			 * Dirty way to get around the fact that the buffered
			 * reading will change user[x].currentdir to not
			 * include filename once it's done reading the entire
			 * file "to memory". This means user[x].currentdir in
			 * fact will be _currentdir_ and this cannot tell us
			 * a true filesize since it's calculated from
			 * filesize(/site/incoming/path) - w/o filename :(
			 */
			my_filesize = filesize(realfile);
			if (my_filesize < user[x].bytes_xfer) {
				my_filesize = user[x].bytes_xfer;
			}
			pct = (user[x].bytes_xfer * 1. / my_filesize) * 100;
			i = 15 * user[x].bytes_xfer * 1. / my_filesize;
			i = (i > 15 ? 15 : i);

			bar[i] = 0;

			for (m = 0; m < i; m++)
				bar[m] = 'x';
			speed = user[x].bytes_xfer / 1024. /
				((tstop.tv_sec - user[x].tstart.tv_sec) * 1. +
			(tstop.tv_usec - user[x].tstart.tv_usec) / 1000000.);

			total_dn_speed += speed;
			downloads++;
			if (!raw)
				sprintf(status, "Dn: %7.1fKBs", speed);
			else if (raw == 1)
				sprintf(status, "\"DN\" \"%.1f\"", speed);
			else
				sprintf(status, "dnld|%.1f", speed);
		} else {
			pct = *bar = *filename = hours = minutes = mb_xfered = 0;
			seconds = tstop.tv_sec - user[x].tstart.tv_sec;
			while (seconds >= 3600) {
				hours++;
				seconds -= 3600;
			}
			while (seconds >= 60) {
				minutes++;
				seconds -= 60;
			}
			if (!raw)
				sprintf(status, "Idle: %02d:%02d:%02d", hours, minutes, seconds);
			else if (raw == 1)
				sprintf(status, "\"ID\" \"%02d:%02d:%02d\"", hours, minutes, seconds);
			else
				sprintf(status, "idle|%02d:%02d:%02d", hours, minutes, seconds);
		}

		hours = minutes = 0;
		seconds = tstop.tv_sec - user[x].login_time;
		while (seconds >= 3600) {
			hours++;
			seconds -= 3600;
		}
		while (seconds >= 60) {
			minutes++;
			seconds -= 60;
		}
		sprintf(online, "%02d:%02d:%02d", hours, minutes, seconds);

		if (mode == 0) {
			if (!raw) {
				if (mb_xfered)
					printf("|%1c%-16.16s/%-10.10s | %-15s | XFER: %13.1fMB |\n", maskchar, user[x].username, get_g_name(user[x].groupid), status, mb_xfered);
				else
					printf("|%1c%-16.16s/%-10.10s | %-15s | %3.0f%%: %-15.15s |\n", maskchar, user[x].username, get_g_name(user[x].groupid), status, pct, bar);

				printf("| %-27.27s | since %8.8s  | file: %-15.15s |\n", user[x].tagline, online, filename);
				printf("+-----------------------------------------------------------------------+\n");
			} else if (raw == 1) {
				/*
				 * Maskeduser / Username / GroupName / Status
				 * / TagLine / Online / Filename / Part
				 * up/down-loaded / Current dir
				 */
				printf("\"USER\" \"%1c\" \"%s\" \"%s\" %s \"%s\" \"%s\" \"%s\" \"%.1f%s\" \"%s\"\n", maskchar, user[x].username, get_g_name(user[x].groupid), status, user[x].tagline, online, filename, (pct >= 0 ? pct : mb_xfered), (pct >= 0 ? "%" : "MB"), user[x].currentdir);
			} else {
				printf("%s|%s|%s|%s|%s\n", user[x].username, get_g_name(user[x].groupid), user[x].tagline, status, filename);
			}
			onlineusers++;
		} else if (strcasecmp(ucomp, user[x].username) == 0) {
#ifdef _WITH_ALTWHO
			if (!raw) {
				if (mb_xfered)
					printf("%s : %1c%s/%s has xfered %.1fMB of %s and has been online for %8.8s.\n", status, maskchar, user[x].username, get_g_name(user[x].groupid), mb_xfered, filename, online);
				else if (strcmp(filename, ""))
					printf("%s : %1c%s/%s has xfered %.0f%% of %s and has been online for %8.8s.\n", status, maskchar, user[x].username, get_g_name(user[x].groupid), pct, filename, online);
				else
					printf("%s : %1c%s/%s has been online for %8.8s.\n", status, maskchar, user[x].username, get_g_name(user[x].groupid), online);
			} else if (raw == 1) {
				printf("\"USER\" \"%1c\" \"%s\" \"%s\" %s \"%s\" \"%s\" \"%s\" \"%.1f%s\" \"%s\"\n", maskchar, user[x].username, get_g_name(user[x].groupid), status, user[x].tagline, online, filename, (pct >= 0 ? pct : mb_xfered), (pct >= 0 ? "%" : "MB"), user[x].currentdir);
			} else {
				printf("%s|%s|%s|%s|%s\n", user[x].username, get_g_name(user[x].groupid), user[x].tagline, status, filename);
			}
#else
			if (!onlineusers) {
				if (!raw)
					printf("\002%s\002 - %s", user[x].username, status);
				else if (raw == 1)
					printf("\"USER\" \"%s\" %s", user[x].username, status);
				else
					printf("\002%s\002 - %s", user[x].username, status);
			} else {
				if (!raw)
					printf(" - %s", status);
				else if (raw == 1)
					printf("\"USER\" \"\" %s", status);
				else
					printf(" - %s", status);
			}
#endif
			onlineusers++;
		}
		free(filename);
	}
}

/* COMPARE USERFLAGS ON CHECKFLAGS */
short 
compareflags(char *flags, char *checkflags)
{
	unsigned int	n1 = 0, n2 = 0;
	char           *userflags;

	userflags = (flags != NULL ? flags : "1234ABCDEFGHI");

	for (n1 = 0; n1 < strlen(userflags); n1++) {
		for (n2 = 0; n2 < strlen(checkflags); n2++) {
			if (*(userflags + n1) == *(checkflags + n2))
				return 1;
		}
	}
	return 0;
}

/* READ CONFIGURATION FILE */
void 
readconfig(char *arg)
{
	char           *buf, *tmp;
	FILE           *cfgfile;
	int		n         , b_w = 0,	/* Beginning of second word */
			e_c = 0,/* Position of equal char */
			e_w = 0,/* End of first word */
			l_b = 0;/* Beginning of the line */

	n = strlen(arg);
	while (arg[n] != '/' && n > 0)
		n--;

	if (n == 0) {
		tmp = malloc(18);
		sprintf(tmp, "/bin/sitewho.conf");
	} else {
		tmp = malloc(n + 14);
		sprintf(tmp, "%.*s/sitewho.conf", n, arg);
	}

	if (stat(tmp, &filestat) == -1 || filestat.st_size <= 0) {
		printf("Config file does not exist (%s)\n", tmp);
		exit(0);
	}
	cfgfile = fopen(tmp, "r");
	free(tmp);
	buf = malloc(filestat.st_size);
	fread(buf, 1, filestat.st_size, cfgfile);
	fclose(cfgfile);

	for (n = 0; n < filestat.st_size; n++) {
		switch (*(buf + n)) {
		case '\n':
			if (b_w > l_b && e_w > l_b) {
				tmp = malloc(n - b_w + 1);
				memcpy(tmp, buf + b_w, n - b_w);
				*(tmp + n - b_w) = 0;

				if (!memcmp(buf + l_b, "headerfile", 10))
					header = tmp;
				else if (!memcmp(buf + l_b, "footerfile", 10))
					footer = tmp;
				else if (!memcmp(buf + l_b, "maskeddirectories", 17))
					mpaths = tmp;
				else if (!memcmp(buf + l_b, "hiddenusers", 11))
					husers = tmp;
				else if (!memcmp(buf + l_b, "glrootpath", 10))
					glpath = tmp;
				else if (!memcmp(buf + l_b, "ipc_key", 7))
					ipckey = tmp;
				else if (!memcmp(buf + l_b, "grp_path", 8))
					glgroup = tmp;
				else {
					if (!memcmp(buf + l_b, "seeallflags", 11))
						showall = compareflags(getenv("FLAGS"), tmp);
					else if (!memcmp(buf + l_b, "maxusers", 8))
						maxusers = atoi(tmp);
					free(tmp);
				}
			}
			l_b = n + 1;
			b_w = 0;
			break;
		case '=':
			if (!e_c && !b_w)
				e_c = n;	/* Only one '=' char per line
						 * counts */
			b_w = n + 1;
			break;
		case '\t':
		case ' ':
			if (l_b == ' ' || l_b == '\t')
				l_b++;	/* Remove spaces from beginning of
					 * the line */
			else if (e_w <= l_b)
				e_w = n;	/* End of first word */
			else if (e_c)
				b_w = n + 1;	/* Beginning of second word */
			break;
		default:
			e_c = 0;
		}
	}
	free(buf);

	if (filesize("") == 1)
		*glpath = 0;
}

/* PRINT FILE */
void 
show(char *filename)
{
	int		fd        , n;
	char		buf       [128];
	char           *fname;

	fname = malloc(strlen(glpath) + strlen(filename) + 2);
	sprintf(fname, "%s/%s", glpath, filename);

	if ((fd = open(fname, O_RDONLY)) != -1) {
		while ((n = read(fd, buf, sizeof(buf))) > 0)
			printf("%.*s", n, buf);
		close(fd);
	}
	free(fname);
}

/* SHOW TOTALS */
void 
showtotals(char raw)
{
	if (!raw) {
		printf("| Up: %3i / %7.1fkbs | Dn: %3i / %7.1fkbs | Total: %3i / %7.1fkbs |\n", uploads, total_up_speed, downloads, total_dn_speed, uploads + downloads, total_up_speed + total_dn_speed);
		printf("| Currently %2i of %2i users are online...                                |\n", onlineusers, maxusers);
	} else if (raw == 1) {
		/*
		 * UpUsers / UpSpeed / DnUsers / DnSpeed / TotalUsers /
		 * TotalSpeed
		 */
		printf("\"STATS\" \"%i\" \"%.1f\" \"%i\" \"%.1f\" \"%i\" \"%.1f\"\n", uploads, total_up_speed, downloads, total_dn_speed, uploads + downloads, total_up_speed + total_dn_speed);
		/* } else { */
		/*
		 * upld | UpUsers | UpSpeed | dnld | DnUsers |DnSpeed | total
		 * | TotalUsers | TotalSpeed | online | OnlineUsers |
		 * MaxUsers
		 */
		/*
		 * printf("upld|%3i|%7.1f|dnld|%3i|%7.1f|total%3i|%7.1f|online
		 * |%2i|%2i\n", uploads, total_up_speed, downloads,
		 * total_dn_speed, uploads + downloads, total_up_speed +
		 * total_dn_speed, onlineusers, maxusers);
		 */
	}
}

/* Buffer groups file */
void 
buffer_groups(char *groupfile)
{
	char           *f_buf, *g_name, *f_name;
	long		f        , n, m, f_size, g_id, g_n_size, l_start = 0;

	f_name = malloc(strlen(glpath) + strlen(groupfile) + 2);
	sprintf(f_name, "%s/%s", glpath, groupfile);

	f = open(f_name, O_NONBLOCK);
	fstat(f, &filestat);
	f_size = filestat.st_size;
	f_buf = malloc(f_size);
	read(f, f_buf, f_size);

	for (n = 0; n < f_size; n++)
		if (f_buf[n] == '\n')
			GROUPS++;
	group = malloc(GROUPS * sizeof(struct GROUP *));

	for (n = 0; n < f_size; n++) {
		if (f_buf[n] == '\n' || n == f_size) {
			f_buf[n] = 0;
			m = l_start;
			while (f_buf[m] != ':' && m < n)
				m++;
			if (m != l_start) {
				f_buf[m] = 0;
				g_name = f_buf + l_start;
				g_n_size = m - l_start;
				m = n;
				while (f_buf[m] != ':' && m > l_start)
					m--;
				f_buf[m] = 0;
				while (f_buf[m] != ':' && m > l_start)
					m--;
				if (m != n) {
					g_id = atoi(f_buf + m + 1);
					group[groups] = malloc(sizeof(struct GROUP));
					group[groups]->name = malloc(g_n_size + 1);
					strcpy(group[groups]->name, g_name);
					group[groups]->id = g_id;
					groups++;
				}
			}
			l_start = n + 1;
		}
	}

	close(f);
	free(f_buf);
	free(f_name);
}

/* CORE CODE */
int 
main(int argc, char **argv)
{

#ifndef _WITH_SS5
	char		raw_output = 0;
	int		user_idx = 1;
#else
	char		raw_output = 2;
	int		user_idx = 2;
#endif

	readconfig(argv[0]);
	if (!ipckey)
		ipckey = def_ipckey;
	if (!glgroup)
		glgroup = def_glgroup;

	buffer_groups(glgroup);

	if (argc > 1 && strlen(argv[1]) == 5) {
		if (!strcasecmp(argv[1], "--raw")) {
			user_idx = 2;
			raw_output = 1;
		} else if (!strcasecmp(argv[1], "--ss5")) {
			user_idx = 2;
			raw_output = 2;
		}
	}
	if ((shmid = shmget((key_t) strtoll(ipckey, NULL, 16), 0, 0)) == -1) {
		if (argc == 1 || (raw_output)) {
			if (!raw_output)
				show(header);
			showtotals(raw_output);
			if (!raw_output)
				show(footer);
		} else {
			if (!raw_output)
				printf("\002%s\002 is not online\n", argv[user_idx]);
			else
				printf("\"ERROR\" \"User %s not online.\"\n", argv[user_idx]);
		}
		exit(0);
	}
	if ((user = (struct ONLINE *)shmat(shmid, NULL, SHM_RDONLY)) == (struct ONLINE *)-1) {
		if (!raw_output)
			printf("Error!: (SHMAT) failed...");
		else
			printf("\"ERROR\" \"SHMAT Failed.\"\n");
		exit(1);
	}
	shmctl(shmid, IPC_STAT, &ipcbuf);

	if (argc == 1 && (!raw_output))
		show(header);

	if (raw_output < 2)
		showusers(ipcbuf.shm_segsz / sizeof(struct ONLINE), argc - raw_output - 1, argv[user_idx], raw_output);
	else if (argc == 1)
		showusers(ipcbuf.shm_segsz / sizeof(struct ONLINE), argc - 1, argv[user_idx], raw_output);
	else
		showusers(ipcbuf.shm_segsz / sizeof(struct ONLINE), argc - 2, argv[user_idx], raw_output);


	if (argc == 1) {
		showtotals(raw_output);
		if (!raw_output)
			show(footer);
	} else {
		if (!onlineusers) {
			if (!raw_output)
				printf("\002%s\002 is not online\n", argv[user_idx]);
			else
				printf("\"ERROR\" \"User %s not online.\"\n", argv[user_idx]);
		}
#ifndef _WITH_ALTWHO
		else if (!raw_output) {
			printf("\n");
		}
#endif
	}

	return 0;
}
