#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "zsfunctions.h"
#include "objects.h"
#include "convert.h"

#include "../conf/zsconfig.h"

extern void writelog(char *, char *);

/*
 * Modified     : 01.27.2002
 * Author       : Dark0n3
 *
 * Description  : Writes .message file and moves toplists into buffer
 *
 */
void complete(struct LOCATIONS *locations, struct VARS *raceI, struct USERINFO **userI, struct GROUPINFO **groupI, int completetype) {

 int    cnt, pos;
 char	*user_p,
	*group_p;
 FILE	*msgfile;
 
 move_progress_bar(1, raceI);
 unlink(locations->incomplete);
 
#if ( write_complete_message == TRUE )
 if ((msgfile = fopen(".message", "w")) == NULL) { d_log("Couldn't fopen .message\n"); exit(EXIT_FAILURE); }

 fprintf(msgfile, "%s", convert( raceI, userI, groupI, message_header));
 fprintf(msgfile, "%s", convert( raceI, userI, groupI, message_user_header));
 for ( cnt = 0 ; cnt < raceI->total.users ; cnt++ ) {
	pos = userI[cnt]->pos;
	fprintf(msgfile, "%s", convert2(raceI, userI[pos], groupI, message_user_body, cnt));
	}
 fprintf(msgfile, "%s", convert(raceI, userI, groupI, message_user_footer));
 fprintf(msgfile, "%s", convert(raceI, userI, groupI, message_group_header));
 for ( cnt = 0 ; cnt < raceI->total.groups ; cnt++ ) {
	 pos = groupI[cnt]->pos;
	 fprintf(msgfile, "%s", convert3(raceI, groupI[pos], message_group_body, cnt));
	}
 fprintf(msgfile, "%s", convert( raceI, userI, groupI, message_group_footer));
  
 if ( raceI->misc.release_type == 3 ) fprintf(msgfile, convert( raceI, userI, groupI, message_mp3));

 fprintf(msgfile, "%s", convert( raceI, userI, groupI, message_footer));
 fclose(msgfile);
#endif

 user_p = raceI->misc.top_messages[0];
 group_p = raceI->misc.top_messages[1];

 if ( raceI->misc.write_log && completetype == 0 ) {
	if ( user_top != NULL ) {
		for ( cnt = 0 ; cnt < max_users_in_top && cnt < raceI->total.users ; cnt++ ) {
			user_p += sprintf(user_p, " %s", convert2(raceI, userI[userI[cnt]->pos], groupI, user_top, cnt));
			}
		}
	if ( group_top != NULL ) {
		for ( cnt = 0 ; cnt < max_groups_in_top && cnt < raceI->total.groups ; cnt++ ) {
			group_p += sprintf(group_p, " %s", convert3(raceI, groupI[groupI[cnt]->pos], group_top, cnt));
			}
		}
 	}
}





/*
 * Modified     : 01.27.2002
 * Author       : Dark0n3
 *
 * Description  : Writes toplists to glftpd.log
 *
 */
void writetop(struct LOCATIONS *locations, struct VARS *raceI, struct USERINFO **userI, struct GROUPINFO **groupI, int completetype ) {
/* char   *output, *out_p; */
 int    cnt;
   
 if ( completetype == 1 ) {
	if ( pre_stats != NULL ) {
		writelog(convert(raceI, userI, groupI, pre_stats), "STATS");
		}
	if ( user_top != NULL ) {
		for ( cnt = 0 ; cnt < max_users_in_top && cnt < raceI->total.users; cnt++ ) {
			writelog(convert2(raceI, userI[userI[cnt]->pos], groupI, user_top, cnt), "STATS");
			}
		}
	if ( between_stats != NULL ) {
		writelog(convert(raceI, userI, groupI, between_stats), "STATS");
		}
	if ( group_top != NULL ) {
		for ( cnt = 0 ; cnt < max_groups_in_top && cnt < raceI->total.groups; cnt++ ) {
			writelog(convert3(raceI, groupI[groupI[cnt]->pos], group_top, cnt), "STATS");
			}
		}
	if ( post_stats != NULL ) {
		writelog(convert(raceI, userI, groupI, post_stats), "STATS");
		}
	}
}
