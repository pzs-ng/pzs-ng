#ifndef _STATS_H_
#define _STATS_H_

void updatestats(struct VARS *, struct USERINFO **, struct GROUPINFO **, char *, char *, unsigned long, long, long, long);
void sortstats(struct VARS *, struct USERINFO **, struct GROUPINFO **);
void showstats(struct VARS *, struct USERINFO **, struct GROUPINFO **);
void get_stats(struct VARS *, struct USERINFO **);

#endif

