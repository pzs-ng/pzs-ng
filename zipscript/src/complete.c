#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "zsfunctions.h"
#include "objects.h"
#include "convert.h"

#include "../conf/zsconfig.h"
#include "../include/zsconfig.defaults.h"

extern void	writelog(char *, char *);

/*
 * Modified     : 01.27.2002 Author       : Dark0n3
 * 
 * Description  : Writes .message file and moves toplists into buffer
 * 
 */
void 
complete(struct LOCATIONS *locations, struct VARS *raceI, struct USERINFO **userI, struct GROUPINFO **groupI, int completetype)
{
	int		cnt       , pos;
	char           *user_p, *group_p;
	FILE           *msgfile;

	move_progress_bar(1, raceI);
	unlink(locations->incomplete);

#if ( write_complete_message == TRUE )
	if (matchpath(group_dirs, locations->path) && (write_complete_message_in_group_dirs == FALSE)) {
		d_log("No message File is written. Directory matched with group_dirs\n");
	} else {
		d_log("Writing %s file\n", message_file_name);

		if (!(msgfile = fopen(message_file_name, "w"))) {
			d_log("Couldn't fopen %s: %s\n", message_file_name, strerror(errno));
			exit(EXIT_FAILURE);
		}
		if ((matchpath(group_dirs, locations->path)) && (custom_group_dirs_complete_message))
			fprintf(msgfile, "%s", convert(raceI, userI, groupI, custom_group_dirs_complete_message));
		else {
			fprintf(msgfile, "%s", convert(raceI, userI, groupI, message_header));
			fprintf(msgfile, "%s", convert(raceI, userI, groupI, message_user_header));
			for (cnt = 0; cnt < raceI->total.users; cnt++) {
				pos = userI[cnt]->pos;
				fprintf(msgfile, "%s", convert2(raceI, userI[pos], groupI, message_user_body, cnt));
			}
			fprintf(msgfile, "%s", convert(raceI, userI, groupI, message_user_footer));
			fprintf(msgfile, "%s", convert(raceI, userI, groupI, message_group_header));
			for (cnt = 0; cnt < raceI->total.groups; cnt++) {
				pos = groupI[cnt]->pos;
				fprintf(msgfile, "%s", convert3(raceI, groupI[pos], message_group_body, cnt));
			}
			fprintf(msgfile, "%s", convert(raceI, userI, groupI, message_group_footer));

			if (raceI->misc.release_type == RTYPE_AUDIO)
				fprintf(msgfile, convert(raceI, userI, groupI, message_mp3));

			fprintf(msgfile, "%s", convert(raceI, userI, groupI, message_footer));
		}
		fclose(msgfile);
	}
#endif

	user_p = raceI->misc.top_messages[0];
	group_p = raceI->misc.top_messages[1];

	if (raceI->misc.write_log && completetype == 0) {
		if (user_top != NULL) {
			for (cnt = 0; cnt < max_users_in_top && cnt < raceI->total.users; cnt++)
				user_p += sprintf(user_p, " %s", convert2(raceI, userI[userI[cnt]->pos], groupI, user_top, cnt));
		}
		if (group_top != NULL) {
			for (cnt = 0; cnt < max_groups_in_top && cnt < raceI->total.groups; cnt++)
				group_p += sprintf(group_p, " %s", convert3(raceI, groupI[groupI[cnt]->pos], group_top, cnt));
		}
	}
}

/*
 * Modified     : 01.27.2002 Author       : Dark0n3
 * 
 * Description  : Writes toplists to glftpd.log
 */
void 
writetop(struct VARS *raceI, struct USERINFO **userI, struct GROUPINFO **groupI, int completetype)
{
	int		cnt;

	if (completetype == 1) {
		if (pre_stats != NULL) {
			writelog(convert(raceI, userI, groupI, pre_stats), stat_users_head_type);
		}
		if (user_top != NULL) {
			for (cnt = 0; cnt < max_users_in_top && cnt < raceI->total.users; cnt++) {
				writelog(convert2(raceI, userI[userI[cnt]->pos], groupI, user_top, cnt), stat_users_type);
			}
		}
		if (between_stats != NULL) {
			writelog(convert(raceI, userI, groupI, between_stats), stat_groups_head_type);
		}
		if (group_top != NULL) {
			for (cnt = 0; cnt < max_groups_in_top && cnt < raceI->total.groups; cnt++) {
				writelog(convert3(raceI, groupI[groupI[cnt]->pos], group_top, cnt), stat_groups_type);
			}
		}
		if (post_stats != NULL) {
			writelog(convert(raceI, userI, groupI, post_stats), stat_post_type);
		}
	}
}
