#ifndef KEY
  #define KEY		0x0000DEAD
#endif

#ifndef USE_GLFTPD2
struct ONLINE {
	char		tagline   [64];	/* The users tagline */
	char		username  [24];	/* The username of the user */
	char		status    [256];	/* The status of the user,
						 * idle, RETR, etc */
	char		host      [256];	/* The host the user is
						 * comming from (with ident) */
	char		currentdir[256];	/* The users current dir
						 * (fullpath) */
	long		groupid;/* The groupid of the users primary group */
	time_t		login_time;	/* The login time since the epoch
					 * (man 2 time) */
	struct timeval	tstart;	/* Replacement for last_update. */
	unsigned long	bytes_xfer;	/* Bytes transferred this far. */
	pid_t		procid;	/* The processor id of the process */
}		__attribute__((deprecated));
#else
struct ONLINE {
	char		tagline   [64];	/* The users tagline */
	char		username  [24];	/* The username of the user */
	char		status    [256];	/* The status of the user,
						 * idle, RETR, etc */
	short int	ssl_flag;	/* 0 = no ssl, 1 = ssl on control, 2
					 * = ssl on control and data */
	char		host      [256];	/* The host the user is
						 * comming from (with ident) */
	char		currentdir[256];	/* The users current dir
						 * (fullpath) */
	long		groupid;/* The groupid of the users primary group */
	time_t		login_time;	/* The login time since the epoch
					 * (man 2 time) */
	struct timeval	tstart;	/* replacement for last_update. */
	struct timeval	txfer;	/* The time of the last succesfull transfer. */
	unsigned long long bytes_xfer;	/* bytes transferred so far. */
	pid_t		procid;	/* The processor id of the process */
};
#endif
