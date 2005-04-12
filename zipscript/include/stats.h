#ifndef _STATS_H_
#define _STATS_H_

struct userdata {
	int		allup_bytes;
	int		monthup_bytes;
	int		wkup_bytes;
	int		dayup_bytes;
	short		name;
};

void updatestats_free(GLOBAL *);
void updatestats(struct VARS *, struct USERINFO **, struct GROUPINFO **, char *, char *, char *, off_t, unsigned int, unsigned int);
void sortstats(struct VARS *, struct USERINFO **, struct GROUPINFO **);
void showstats(struct VARS *, struct USERINFO **, struct GROUPINFO **);
void get_stats(struct VARS *, struct USERINFO **);

#endif
