#ifndef _STATS_H_
#define _STATS_H_

struct userdata {
	unsigned long long		allup_bytes;
	unsigned long long		monthup_bytes;
	unsigned long long		wkup_bytes;
	unsigned long long		dayup_bytes;
	short		name;
};

void updatestats_free(GLOBAL *);
void updatestats(struct VARS *, struct USERINFO **, struct GROUPINFO **, char *, char *, off_t, unsigned int, unsigned int);
void sortstats(struct VARS *, struct USERINFO **, struct GROUPINFO **);
void showstats(struct VARS *, struct USERINFO **, struct GROUPINFO **);
void get_stats(struct VARS *, struct USERINFO **);

#endif
