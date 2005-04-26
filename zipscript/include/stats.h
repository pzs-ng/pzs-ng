#ifndef _STATS_H_
#define _STATS_H_

struct userdata {
	/* 0 - allup
	   1 - monthup
	   2 - wkup
	   3 - dayup */
	int			bytes[4];

	short		name;
};

void updatestats_free(GLOBAL *);
void updatestats(struct VARS *, struct USERINFO **, struct GROUPINFO **, char *, char *, char *, off_t, unsigned int, unsigned int);
void sortstats(struct VARS *, struct USERINFO **, struct GROUPINFO **);
void showstats(struct VARS *, struct USERINFO **, struct GROUPINFO **);
void get_stats(struct VARS *, struct USERINFO **);
char *get_statline(FILE *, const char *);

#endif
