/*
 * Experimental module for tcl that will get useful information
 * from glftpd's shared memory. The purpose of this module is to replace
 * the use of ng-bw and sitewho inside dZSbot.
 *
 * Compile with:
 *
 * gcc -O2 -W -Wall -fPIC -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE \
 *  -I/usr/local/include/tcl8.4/ -I../../zipscript/include/ \
 *  -c -o glwho.o glwho.c
 * ld -o glwho.so glwho.o -G
 *
 * Edit the path to where tcl.h is if needed. Load the module in a tcl
 * shell with "load glwho.so"
 *
 */

/* static defines for these things isn't superb */
#define GLGROUPFILE "/glftpd/etc/group"
#define GLROOT "/glftpd"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <ctype.h>
#include <tcl.h>

#include "structonline.h"
#include "../../zipscript/conf/zsconfig.h"
#include "zsconfig.defaults.h"

struct GROUP {
	char g[24];
	long gid;
};

int GlCmd(ClientData, Tcl_Interp *, int, char **); /* handle all commands */
struct ONLINE *getShmem(struct shmid_ds *); /* fetch the shared memory */

int gl_bw(); /* print bandwidth like ng-bw */
int gl_raw(); /* print information in cookie format like ./sitewho --raw */
int gl_user(char *); /* perhaps print a lot of info about a connected user? */

double calcTime(int);
void getgroups(struct GROUP **, char *, off_t); /* buffer groups */
int strplen(char *);

int Glwho_Init(Tcl_Interp *interp)
{

	/* create the basic "gl" command */
	Tcl_CreateCommand(interp, "gl", GlCmd, (ClientData)0, (Tcl_CmdDeleteProc*)NULL);
	return TCL_OK;

}

int GlCmd(ClientData gl_word, Tcl_Interp *interp, int argc, char **argv)
{
	
	/* check the argument(s) and execute their functions */
	if (argc <= 1) {
		Tcl_SetResult(interp, "Usage: gl bw|raw|user <uname>|version", TCL_STATIC);
		return TCL_ERROR;
	} else if (strcmp(argv[1], "bw") == 0) {
		return gl_bw();
	} else if (strcmp(argv[1], "raw") == 0) {
		return gl_raw();
	} else if ((strcmp(argv[1], "user") == 0) && argc == 3) {
		return gl_user(argv[2]);
	} else if (strcmp(argv[1], "version") == 0) {
		Tcl_SetResult(interp, "glftpd tcl module version 0.1", TCL_STATIC);
		return TCL_OK;
	} else {
		Tcl_SetResult(interp, "Usage: gl bw|raw|user <uname>|version", TCL_STATIC);
		return TCL_ERROR;
	}

}

/* this might not be the best way to do things */
struct ONLINE *getShmem(struct shmid_ds *shminfo)
{
	
	int id;
	struct ONLINE *shm;
	
	if ((id = shmget(KEY, 0, 0)) < 0) return NULL;
	
	if (shmctl(id, IPC_STAT, shminfo) < 0) {
		perror("shmctl");
		return NULL;
	}

	if ((shm = shmat(id, NULL, SHM_RDONLY)) == (struct ONLINE *)-1) {
		perror("shmat");
		return NULL;
	}

	return shm;

}

/* Print bandwidth usage, if any. */
int gl_bw()
{
	
	int i, max_users = 0, online_users = 0, u_xfers = 0, d_xfers = 0, 
	    a_users = 0, i_users = 0, idletime;
	double u_speed = 0, d_speed = 0;
	    
	struct ONLINE *shm;
	struct shmid_ds shminfo;
	
	if ((shm = getShmem(&shminfo)) != NULL) {
		max_users = shminfo.shm_segsz/sizeof(struct ONLINE);
		
		for (i=0; i < max_users; i++) {
			if ((shm->procid != 0) && (strcmp(shm->username, "-NEW-"))) {
				
				/* lots from ng-bw */
				idletime = time(NULL) - shm->tstart.tv_sec;
			
				if ((!strncasecmp(shm->status, "APPE", 4)) || (!strncasecmp(shm->status, "STOR", 4))) {
					/* uploading */
					u_xfers++;
					u_speed += calcTime(i);
				} else if (!strncasecmp(shm->status, "RETR", 4)) {
					/* downloading */
					d_xfers++;
					d_speed += calcTime(i);
				} else if (idletime <= 8) {
					/* active but no xfer */
					a_users++;
				} else {
					/* idle */
					i_users++;
				}
				
				online_users++;
			}
			shm++;
		}
		/*
		 * UploadThreads IncomingTraffic DownloadThreads OutgoingTraffic
		 * TotalThreads TotalTraffic TotalBrowsing TotalIdle TotalLoggedIn
		 */
		printf("%d %.1f %d %.1f %d %.1f %d %d %d\n",	u_xfers, u_speed, d_xfers, d_speed, 
								u_xfers + d_xfers, u_speed + d_speed,
								a_users, i_users,
								u_xfers + d_xfers + a_users + i_users);

		return TCL_OK;
	} else {
		printf("0 0.0 0 0.0 0 0.0 0 0 0\n");
		return TCL_OK;
	}

}

int gl_raw()
{

	int i, n, max_users=0, num_groups=0, seconds=0, hours=0, minutes=0;
	double speed=0, percent=0;
	char *buf, g[24], status[52], realfile[256], *filename=0, online_time[10];

	struct ONLINE *shm;
	struct shmid_ds shminfo;

	struct timeval time;
	struct stat sb;
	struct GROUP **groups;
	
	FILE *grpfile;

	/* this chunk of shit should be put somewhere else perhaps */
	if ((grpfile = fopen(GLGROUPFILE, "r")) == NULL) {
		perror("fopen");
		return NULL;
	}

	if (stat(GROUPFILE, &sb) == -1) {
		perror("stat");
		return NULL;
	}

	buf = malloc(sb.st_size);
	fread(buf, 1, sb.st_size, grpfile);
	fclose(grpfile);
	
	for (i=0; i < sb.st_size; i++)
		if (buf[i] == '\n') num_groups++;
	
	groups = malloc(num_groups*sizeof(struct GROUP*));
	getgroups(groups, buf, sb.st_size);
	free(buf);

	if ((shm = getShmem(&shminfo)) != NULL) {
		max_users = shminfo.shm_segsz/sizeof(struct ONLINE);

		for (i=0; i < max_users; i++) {

			if (shm->procid) {
				
				for (n=0; n < num_groups; n++)
					if (groups[n]->gid == shm->groupid)
						strcpy(g, groups[n]->g);
				
				/* a lot from sitewho is in here */
				
				gettimeofday(&time, NULL);
				
				seconds = time.tv_sec - shm->login_time;
				while (seconds >= 3600) { hours++; seconds -= 3600; }
				while (seconds >= 60) { minutes++; seconds -= 60; }
				sprintf(online_time, "%02d:%02d:%02d", hours, minutes, seconds);

				if (strplen(shm->status) > 5)
					filename = malloc(strplen(shm->status) - 5 + 1);
				else
					filename = malloc(1);
					
				/* this shit is kind of ugly, really */
				if ((!strncasecmp(shm->status, "STOR ", 5) ||
				    !strncasecmp(shm->status, "APPE ", 5)) &&
				    shm->bytes_xfer) {
					
					sprintf(filename, "%.*s", (strplen(shm->status)-5), shm->status + 5);
					
					while (filename[(int)strlen(filename) - 1] == '\r' || 
					       filename[(int)strlen(filename) - 1] == '\n')
						filename[(int)strlen(filename) - 1] = '\0';				
				
					speed =	shm->bytes_xfer/1024./(time.tv_sec+(time.tv_usec/1000000.)-
						shm->tstart.tv_sec+(shm->tstart.tv_usec/1000000.));

					percent = -1;
					sprintf(status, "\"UP\" \"%.1f\"", speed); 
					
				} else if ((!strncasecmp(shm->status, "RETR ", 5) && shm->bytes_xfer)) {
					
					sprintf(realfile, "%s/%s", GLROOT, shm->currentdir);
					sprintf(filename, "%.*s", (strplen(shm->status)-5), shm->status + 5);
					sprintf(filename, "%s", shm->status + 5);

					while (filename[(int)strlen(filename) - 1] == '\r' || 
					       filename[(int)strlen(filename) - 1] == '\n')
						filename[(int)strlen(filename) - 1] = '\0';				
					
					speed =	shm->bytes_xfer/1024./(time.tv_sec+(time.tv_usec/1000000.)-
						shm->tstart.tv_sec+(shm->tstart.tv_usec/1000000.));
					
					sprintf(status, "\"DN\" \"%.1f\"", speed);
					if (stat(realfile, &sb) != -1) percent = ((shm->bytes_xfer*1./sb.st_size)*100.);
					
				} else {
					
					seconds = time.tv_sec - shm->tstart.tv_sec;
					hours = minutes = 0;
					
					while (seconds >= 3600) { hours++; seconds -= 3600; }
					while (seconds >= 60) { minutes++; seconds -= 60; }
					
					sprintf(status, "\"ID\" \"%02d:%02d:%02d\"", hours, minutes, seconds);
					sprintf(filename, " ");

				}
				
				/*
				 * Maskeduser / Username / GroupName / Status
				 * / TagLine / Online / Filename / Part
				 * up/down-loaded / Current dir
				 */
				/* mask is not implemented yet */
				printf("\"USER\" \"mask\" \"%s\" \"%s\" %s \"%s\" \"%s\" \"%.1f%s\" \"%s\" \"%s\"\n",
					shm->username, g, status, online_time, shm->tagline,
					(percent >= 0 ? percent : (shm->bytes_xfer/1024/1024)),
					(percent >= 0 ? "%" : "MB"), filename, shm->currentdir);
			}

			shm++;
		}
		
		return TCL_OK;
	} else
		return TCL_ERROR;
	
	return TCL_OK;
}

int gl_user(char *uname)
{
	
	int i, max_users=0;
	
	struct ONLINE *shm;
	struct shmid_ds shminfo;

	if ((shm = getShmem(&shminfo)) != NULL) {
		max_users = shminfo.shm_segsz/sizeof(struct ONLINE);

		for (i=0; i < max_users; i++) {
			if (shm->procid) {
				if (strcmp(shm->username, uname) == 0) {
					printf("%s is online and here is some information about him/her: blablabla\n", uname);
				}
			}
			shm++;
		}
		
		return TCL_OK;
	} else
		return TCL_ERROR;
	
}

/* from original ftpwho source */
double calcTime(int uid)
{
	
	struct ONLINE *users;
	struct shmid_ds shminfo;

	struct timeval	tstop;
	double		delta  , rate;

	users = getShmem(&shminfo);
	
	if (users[uid].bytes_xfer < 1) {
		return 0;
	}
	gettimeofday(&tstop, (struct timezone *)0);
	delta = ((tstop.tv_sec * 10.) + (tstop.tv_usec / 100000.)) -
		((users[uid].tstart.tv_sec * 10.) + (users[uid].tstart.tv_usec / 100000.));
	delta = delta / 10.;
	rate = ((users[uid].bytes_xfer / 1024.0) / (delta));
	if (!rate)
		rate++;
	return rate;

}

void getgroups(struct GROUP **group, char *buf, off_t filesize)
{

	int i, u=0, n=0, num_groups=0;
	long gid=1;
	char g[24];
	char *tmp;

	for (i=0; i < filesize; i++) {

		if (buf[i] == ':') {
		
			switch(n) {
				case 0:
					strncpy(g, (buf+(i-u)), u);
					g[u] = '\0';
					n++;
				case 1:
					n++; break;
				case 2:
					n++; break;
				case 3:
					tmp = malloc(u);
					strncpy(tmp, (buf+(i-u+1)), u-1);
					tmp[u] = '\0';
					gid = atol(tmp);
					free(tmp);
					n++;
				default:
					n=0;
					break;
			}
			u=0;
			
		} else if (buf[i] == '\n') {
			group[num_groups] = malloc(sizeof(struct GROUP));
			group[num_groups]->gid=gid;
			strcpy(group[num_groups]->g, g);
			num_groups++;
			u=-1;
		}
		u++;
	}
}

/* from sitewho */
int strplen(char *string)
{
	int i = 0;

	while (isprint(string[i]) && string[i])
		i++;

	return i;
}
