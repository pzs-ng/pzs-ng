/*
   Displays currnet bandwidth over glftpd by iono (with some code coming from the original sitewho)

   Uses the format:
   "UploadThreads IncomingTraffic DownloadThreads OutgoingTraffic TotalThreads TotalTraffic TotalBrowsing TotalIdle TotalLoggedIn"

   To Compile:
   gcc -O2 -Wall -pedantic -static -o ng-bw ng-bw.c

*/

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <time.h>
#include <sys/time.h>
#include <sys/types.h> 
#include <sys/ipc.h>
#include <sys/shm.h>
#include "structonline.h"

/* You shouldn't need to modify below here */


static int numUsers = 0; static struct ONLINE *users; static struct shmid_ds ipcbuf;

/* Implicit declarations are *BAD*! */
int OutputData(void); 

int main() {

  int retv;
  int shmid;

  if ((shmid = shmget(KEY, 0,0)) == -1) {
    printf("0 0.0 0 0.0 0 0.0 0 0 0\n");
    exit(0);
  }
  users = (struct ONLINE *)shmat(shmid, 0, SHM_RDONLY);
  if (users == (struct ONLINE *)(-1)) {
    perror("3-SHMAT Failed");
    exit(3);
  }

  shmctl( shmid, IPC_STAT, &ipcbuf);
  numUsers = ipcbuf.shm_segsz / sizeof(struct ONLINE);
  retv = OutputData();

  if (shmdt(users) == -1) {
    perror("4-SHMDT Failed");
    exit(4);
  }

  return retv;

}

/* from original ftpwho source */
static double calcTime(int uid) {

  struct timeval tstop;
  double delta, rate;
  if (users[uid].bytes_xfer < 1) {
    return 0;
  }
  gettimeofday(&tstop, (struct timezone *)0 );
  delta = ((tstop.tv_sec*10.)+(tstop.tv_usec/100000.)) -
    ((users[uid].tstart.tv_sec*10.)+(users[uid].tstart.tv_usec/100000.));
  delta = delta/10.;
  rate = ((users[uid].bytes_xfer / 1024.0) / (delta));
  if (!rate) rate++;
  return (double)(rate);

}

int OutputData() {

  int i;
  char cmd[255];
  int u_xfers = 0, d_xfers = 0, a_users = 0, i_users = 0;
  double u_speed = 0, d_speed = 0;

  for (i = 0; i < numUsers; i++) {
    /* Ignore connections which have not finished logging in yet */
    if ((users[i].procid != 0) && (strcmp(users[i].username, "-NEW-"))) {

      /* set idletime var */
      int idletime = time(NULL) - users[i].tstart.tv_sec;

      /* copy command from array to var */
      strcpy(cmd, users[i].status);

      if ((!strncasecmp(cmd, "APPE", 4)) || (!strncasecmp(cmd, "STOR", 4))) {
	/* User Is Uploading */

	u_xfers++;
	u_speed += calcTime(i);

      } else if (!strncasecmp(cmd, "RETR", 4)) {
	/* User Is Downloading */

	d_xfers++;
	d_speed += calcTime(i);				

      } else if (idletime <= 8) {
	/* Active with no xfer */

	a_users++;				

      } else {
	/* User is Idle */

	i_users++;

      }

    }
  }
  /* UploadThreads IncomingTraffic DownloadThreads OutgoingTraffic TotalThreads TotalTraffic TotalBrowsing TotalIdle TotalLoggedIn
  */
  printf("%d %.1f %d %.1f %d %.1f %d %d %d\n",
      u_xfers, u_speed,
      d_xfers, d_speed,

      u_xfers + d_xfers,
      u_speed + d_speed,

      a_users, i_users,

      u_xfers + d_xfers + a_users + i_users
      );

  return 0;
}
