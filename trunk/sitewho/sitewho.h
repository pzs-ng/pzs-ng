#ifndef SITEWHO_H
#define SITEWHO_H

struct GROUP {
	char           *name;
	gid_t		id;
};

struct ONLINE_GL132 {
	char		tagline   [64];		/* The users tagline */
	char		username  [24];		/* The username of the user */
	char		status    [256];	/* The status of the user, idle, RETR, etc */
	char		host      [256];	/* The host the user is coming from (with ident) */
	char		currentdir[256];	/* The users current dir (fullpath) */
	long		groupid;		/* The groupid of the users primary group */
	time_t		login_time;		/* The login time since the epoch (man 2 time) */
	struct timeval	tstart;			/* Replacement for last_update. */
	unsigned long	bytes_xfer;		/* Bytes transferred this far. */
	pid_t		procid;			/* The processor id of the process */
};

struct ONLINE_GL200 {
	char		tagline   [64];		/* The users tagline */
	char		username  [24];		/* The username of the user */
	char		status    [256];	/* The status of the user, idle, RETR, etc */
	short int	ssl_flag;		/* 0 = no ssl, 1 = ssl on control, 2 = ssl on control and data */
	char		host      [256];	/* The host the user is coming from (with ident) */
	char		currentdir[256];	/* The users current dir (fullpath) */
	long		groupid;		/* The groupid of the users primary group */
	time_t		login_time;		/* The login time since the epoch (man 2 time) */
	struct timeval	tstart;			/* replacement for last_update. */
	struct timeval	txfer;			/* The time of the last succesfull transfer. */
	unsigned long long bytes_xfer;		/* bytes transferred so far. */
	pid_t		procid;			/* The processor id of the process */
};

struct ONLINE_GL201 {
	char		tagline   [64];		/* The users tagline */
	char		username  [24];		/* The username of the user */
	char		status    [256];	/* The status of the user, idle, RETR, etc */
	short int	ssl_flag;		/* 0 = no ssl, 1 = ssl on control, 2 = ssl on control and data */
	char		host      [256];	/* The host the user is coming from (with ident) */
	char		currentdir[256];	/* The users current dir (fullpath) */
	long		groupid;		/* The groupid of the users primary group */
	time_t		login_time;		/* The login time since the epoch (man 2 time) */
	struct timeval	tstart;			/* replacement for last_update. */
	struct timeval	txfer;			/* The time of the last succesfull transfer. */
	unsigned long long bytes_xfer;		/* bytes transferred so far. */
	unsigned long long bytes_txfer;		/* bytes transferred in the last loop (speed limiting). */
	pid_t		procid;			/* The processor id of the process */
};

struct ONLINE {
	char			tagline   [64];	/* The users tagline */
	char			username  [24];	/* The username of the user */
	char			status    [256];/* The status of the user, idle, RETR, etc */
	char			currentdir[256];/* The users current dir (fullpath) */
	long			groupid;	/* The groupid of the users primary group */
	time_t			login_time;	/* The login time since the epoch (man 2 time) */
	struct timeval		tstart;		/* Replacement for last_update. */
	unsigned long long	bytes_xfer;	/* Bytes transferred this far. */
	pid_t			procid;		/* The processor id of the process */
};

int copystruct(int);
int check_path(char *);
unsigned long filesize(char *);
char *get_g_name(unsigned int);
int strplen(char *);
short matchpath(char *, char *);
short strcomp(char *, char *);
void showusers(int, int, char *, char);
short compareflags(char *, char *);
void readconfig(char *);
void show(char *);
void showtotals(char);
int buffer_groups(char *, int);

#endif

