#ifndef SITEWHO_H
#define SITEWHO_H

struct GROUP {
    char    *name;
    gid_t   id;
};

/* Force structure alignment to 4 bytes (for 64bit support). */
#pragma pack(push, 4)

/* 32-bit timeval data structure (for 64bit support). */
typedef struct {
    int32_t tv_sec;
    int32_t tv_usec;
} timeval32_t;

struct ONLINE_GL132 {
    char        tagline   [64];     /* The users tagline */
    char        username  [24];     /* The username of the user */
    char        status    [256];    /* The status of the user, idle, RETR, etc */
    char        host      [256];    /* The host the user is coming from (with ident) */
    char        currentdir[256];    /* The users current dir (fullpath) */
    int32_t     groupid;            /* The groupid of the users primary group */
    int32_t     login_time;         /* The login time since the epoch (man 2 time) */
    timeval32_t tstart;             /* Replacement for last_update. */
    uint32_t    bytes_xfer;         /* Bytes transferred this far. */
    int32_t     procid;             /* The processor id of the process */
};

struct ONLINE_GL200 {
    char        tagline   [64];     /* The users tagline */
    char        username  [24];     /* The username of the user */
    char        status    [256];    /* The status of the user, idle, RETR, etc */
    int16_t     ssl_flag;           /* 0 = no ssl, 1 = ssl on control, 2 = ssl on control and data */
    char        host      [256];    /* The host the user is coming from (with ident) */
    char        currentdir[256];    /* The users current dir (fullpath) */
    int32_t     groupid;            /* The groupid of the users primary group */
    int32_t     login_time;         /* The login time since the epoch (man 2 time) */
    timeval32_t tstart;             /* replacement for last_update. */
    timeval32_t txfer;              /* The time of the last succesfull transfer. */
    uint64_t    bytes_xfer;         /* bytes transferred so far. */
    int32_t     procid;             /* The processor id of the process */
};

struct ONLINE_GL201 {
    char        tagline   [64];     /* The users tagline */
    char        username  [24];     /* The username of the user */
    char        status    [256];    /* The status of the user, idle, RETR, etc */
    int16_t     ssl_flag;           /* 0 = no ssl, 1 = ssl on control, 2 = ssl on control and data */
    char        host      [256];    /* The host the user is coming from (with ident) */
    char        currentdir[256];    /* The users current dir (fullpath) */
    int32_t     groupid;            /* The groupid of the users primary group */
    int32_t     login_time;         /* The login time since the epoch (man 2 time) */
    timeval32_t tstart;             /* replacement for last_update. */
    timeval32_t txfer;              /* The time of the last succesfull transfer. */
    uint64_t    bytes_xfer;         /* bytes transferred so far. */
    uint64_t    bytes_txfer;        /* bytes transferred in the last loop (speed limiting). */
    int32_t     procid;             /* The processor id of the process */
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

/* Restore default structure alignment for non-critical structures. */
#pragma pack(pop)

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

