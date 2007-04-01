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

#include "complete.h"

//extern void	writelog(char *, char *);

/*
 * Modified     : 01.27.2002 Author       : Dark0n3
 * 
 * Description  : Writes .message file and moves toplists into buffer
 * 
 */
void 
complete(GLOBAL *g, int completetype)
{
	int		cnt       , pos;
	char           *user_p, *group_p;
	FILE           *msgfile;

	move_progress_bar(1, &g->v, g->ui, g->gi);
	unlink(g->l.incomplete);

#if ( write_complete_message == TRUE )
	if (matchpath(group_dirs, g->l.path) && (write_complete_message_in_group_dirs == FALSE))
		d_log("complete: No message File is written. Directory matched with group_dirs\n");
	else if (message_file_name == DISABLED || !message_file_name)
		d_log("complete: No message file is written. 'message_file_name' not set.\n");
	else {
		d_log("complete: Writing %s file ...\n", message_file_name);

		if (!(msgfile = fopen(message_file_name, "w"))) {
			d_log("complete: Couldn't fopen %s: %s\n", message_file_name, strerror(errno));
			exit(EXIT_FAILURE);
		}
		if ((matchpath(group_dirs, g->l.path)) && (custom_group_dirs_complete_message)) {
			if (custom_group_dirs_complete_message != DISABLED) {
				d_log("complete:   - Writing custom complete message for group dirs ...\n");
				fprintf(msgfile, "%s", convert(&g->v, g->ui, g->gi, custom_group_dirs_complete_message));
			}
		} else {
			if (message_header != DISABLED) {
				d_log("complete:   - Converting message_header ...\n");
				fprintf(msgfile, "%s", convert(&g->v, g->ui, g->gi, message_header));
			}
			if (message_user_header != DISABLED && max_users_in_top > 0) {
				d_log("complete:   - Converting message_user_header ...\n");
				fprintf(msgfile, "%s", convert(&g->v, g->ui, g->gi, message_user_header));
			}
			if (message_user_body != DISABLED && max_users_in_top > 0) {
				d_log("complete:   - Converting message_user_body ...\n");
				for (cnt = 0; cnt < g->v.total.users; cnt++) {
					pos = g->ui[cnt]->pos;
					fprintf(msgfile, "%s", convert2(&g->v, g->ui[pos], g->gi, message_user_body, cnt));
				}
			}
			if (message_user_footer != DISABLED && max_users_in_top > 0) {
				d_log("complete:   - Converting message_user_footer ...\n");
				fprintf(msgfile, "%s", convert(&g->v, g->ui, g->gi, message_user_footer));
			}
			if (message_group_header != DISABLED && max_groups_in_top > 0) {
				d_log("complete:   - Converting message_group_header ...\n");
				fprintf(msgfile, "%s", convert(&g->v, g->ui, g->gi, message_group_header));
			}
			if (message_group_body != DISABLED && max_groups_in_top > 0) {
				d_log("complete:   - Converting message_group_body ...\n");
				for (cnt = 0; cnt < g->v.total.groups; cnt++) {
					pos = g->gi[cnt]->pos;
					fprintf(msgfile, "%s", convert3(&g->v, g->gi[pos], message_group_body, cnt));
				}
			}
			if (message_group_footer != DISABLED && max_groups_in_top > 0) {
				d_log("complete:   - Converting message_group_footer ...\n");
				fprintf(msgfile, "%s", convert(&g->v, g->ui, g->gi, message_group_footer));
			}
			if (message_mp3 != DISABLED) {
				if (g->v.misc.release_type == RTYPE_AUDIO) {
					d_log("complete:   - Converting message_mp3 ...\n");
					fprintf(msgfile, convert(&g->v, g->ui, g->gi, message_mp3));
				}
			}
			if (message_footer != DISABLED) {
				d_log("complete:   - Converting message_footer ...\n");
				fprintf(msgfile, "%s", convert(&g->v, g->ui, g->gi, message_footer));
			}
		}
		d_log("complete:   - Converting complete.\n");
		fclose(msgfile);
	}
#endif

	user_p = g->v.misc.top_messages[0];
	group_p = g->v.misc.top_messages[1];

	if (g->v.misc.write_log && completetype == 0) {
		if (user_top != NULL && max_users_in_top > 0) {
#if ( show_stats_from_pos2_only )
			for (cnt = 1; cnt < max_users_in_top && cnt < g->v.total.users; cnt++)
#else
			for (cnt = 0; cnt < max_users_in_top && cnt < g->v.total.users; cnt++)
#endif
				user_p += sprintf(user_p, " %s", convert2(&g->v, g->ui[g->ui[cnt]->pos], g->gi, user_top, cnt));
		}
		if (group_top != NULL && max_groups_in_top > 0) {
#if ( show_stats_from_pos2_only )
			for (cnt = 1; cnt < max_groups_in_top && cnt < g->v.total.groups; cnt++)
#else
			for (cnt = 0; cnt < max_groups_in_top && cnt < g->v.total.groups; cnt++)
#endif
				group_p += sprintf(group_p, " %s", convert3(&g->v, g->gi[g->gi[cnt]->pos], group_top, cnt));
		}
	}
}

/*
 * Modified     : 01.27.2002 Author       : Dark0n3
 * 
 * Description  : Writes toplists to glftpd.log
 */
void 
writetop(GLOBAL *g, int completetype)
{
	int		cnt, mlen, mset, mtemp;
	char		templine [FILE_MAX];
	char	       *buffer = 0;
	char	       *pbuf = 0;

	if (completetype == 1) {
		if (user_top != NULL && max_users_in_top > 0) {
			mlen = 0;
			mset = 1;
			pbuf = buffer = ng_realloc(buffer, FILE_MAX, 1, 1, &g->v, 1);
			for (cnt = 0; cnt < max_users_in_top && cnt < g->v.total.users; cnt++) {
				snprintf(templine, FILE_MAX, "%s ", convert2(&g->v, g->ui[g->ui[cnt]->pos], g->gi, user_top, cnt));
				mlen = strlen(templine);
				if ((int)strlen(buffer) + mlen >= FILE_MAX * mset) {
					mset += 1;
					mtemp = pbuf - buffer;
					buffer = ng_realloc(buffer, FILE_MAX * mset, 0, 1, &g->v, 0);
					pbuf = buffer + mtemp;
				}
				memcpy(pbuf, templine, mlen);
				pbuf += mlen;
			}
			*pbuf -= '\0';
			writelog(g, buffer, stat_users_type);
			ng_free(buffer);
		}
		if (group_top != NULL && max_groups_in_top > 0) {
			mlen = 0;
			mset = 1;
			pbuf = buffer = ng_realloc(buffer, FILE_MAX, 1, 1, &g->v, 1);
			for (cnt = 0; cnt < max_groups_in_top && cnt < g->v.total.groups; cnt++) {
				snprintf(templine, FILE_MAX, "%s ", convert3(&g->v, g->gi[g->gi[cnt]->pos], group_top, cnt));
				mlen = strlen(templine);
				if ((int)strlen(buffer) + mlen >= FILE_MAX * mset) {
					mset += 1;
					buffer = ng_realloc(buffer, FILE_MAX * mset, 0, 1, &g->v, 0);
				}
				memcpy(pbuf, templine, mlen);
				pbuf += mlen;
			}
			*pbuf -= '\0';
			writelog(g, buffer, stat_groups_type);
			ng_free(buffer);
		}
		if (post_stats != NULL) {
			writelog(g, convert(&g->v, g->ui, g->gi, post_stats), stat_post_type);
		}
	}
}
