#ifndef SITEWHO_H
#define SITEWHO_H

struct GROUP {
    char    *name;
    gid_t   id;
};

struct ONLINE {
    char        tagline   [64];     /* The users tagline */
    char        username  [24];     /* The username of the user */
    char        status    [256];    /* The status of the user, idle, RETR, etc */
    char        currentdir[256];    /* The users current dir (fullpath) */
    int32_t     groupid;            /* The groupid of the users primary group */
    int32_t     login_time;         /* The login time since the epoch (man 2 time) */
    timeval32_t tstart;             /* Replacement for last_update. */
    uint64_t    bytes_xfer;         /* Bytes transferred this far. */
    pid_t       procid;             /* The processor id of the process */
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

