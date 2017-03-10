#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "zsfunctions.h"
#include "objects.h"
#include "convert.h"
#include "race-file.h"

#include "../conf/zsconfig.h"
#include "../include/zsconfig.defaults.h"

#include "complete.h"

//extern void	writelog(char *, char *);

/*
 * Modified     : 01.27.2002 Author       : Dark0n3
 * Modified	: 06.24.2012 by Skeddie: fix out of bounds for userstop
 *
 * Description  : Writes .message file and moves toplists into buffer
 *
 */
void 
complete(GLOBAL *g, int completetype)
{
	int cnt;
	char *user_p, *group_p;
#if ( write_complete_message == TRUE )
	int pos;
	FILE *msgfile;
#endif
#if message_store_in_mirror
	char message_mirror_name[PATH_MAX];
#endif

	move_progress_bar(1, &g->v, g->ui, g->gi);
	unlink(g->l.incomplete);

#if ( write_complete_message == TRUE )
	if (matchpath(group_dirs, g->l.path) && (write_complete_message_in_group_dirs == FALSE))
		d_log("complete: No message File is written. Directory matched with group_dirs\n");
	else if (message_file_name == DISABLED || !message_file_name)
		d_log("complete: No message file is written. 'message_file_name' not set.\n");
	else {
		d_log("complete: Writing %s file ...\n", message_file_name);
#if message_store_in_mirror
		sprintf(message_mirror_name, storage "/%s/%s", g->l.path, message_file_name);
		if (symlink(message_mirror_name, message_file_name) != 0)
			d_log("complete: Couldn't create symlink %s in %s: %s\n", message_file_name, strerror(errno));
#endif
		if (!(msgfile = fopen(message_file_name, "w"))) {
			d_log("complete: Couldn't fopen %s: %s\n", message_file_name, strerror(errno));
			remove_lock(&g->v);
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
					fprintf(msgfile, "%s", convert_user(&g->v, g->ui[pos], g->gi, message_user_body, cnt));
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
					fprintf(msgfile, "%s", convert_group(&g->v, g->gi[pos], message_group_body, cnt));
				}
			}
			if (message_group_footer != DISABLED && max_groups_in_top > 0) {
				d_log("complete:   - Converting message_group_footer ...\n");
				fprintf(msgfile, "%s", convert(&g->v, g->ui, g->gi, message_group_footer));
			}
			if (message_audio != DISABLED) {
				if (g->v.misc.release_type == RTYPE_AUDIO) {
					d_log("complete:   - Converting message_audio ...\n");
					fprintf(msgfile, "%s", convert(&g->v, g->ui, g->gi, message_audio));
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

	if (g->v.misc.write_log && completetype == 0) {
		int topsize = 0;
		char topbuf[256];

		user_p = g->v.misc.top_messages[0];
		group_p = g->v.misc.top_messages[1];

#if ( show_stats_from_pos2_only )
                const int first_entry = 1;
#else
                const int first_entry = 0;
#endif

		if (user_top != NULL && max_users_in_top > 0) {
                        user_p += sprintf(user_p, "%s", racersplit_prefix);
			for (cnt = first_entry; cnt < max_users_in_top && cnt < g->v.total.users; ++cnt) {
				topsize = sprintf(topbuf, "%s", convert_user(&g->v, g->ui[g->ui[cnt]->pos], g->gi, user_top, cnt));

				/* only add next pos if we're not going out of bounds,
				 * otherwise the sprintf outside the for-loop takes
				 * care of writing the \0 on the correct spot
				 */
				if (user_p + topsize + sizeof(racersplit_postfix) < g->v.misc.top_messages[0] + sizeof(g->v.misc.top_messages[0])) {
	                                if (cnt != first_entry) {
        	                            user_p += sprintf(user_p, "%s", racersplit);
					}
					user_p += sprintf(user_p, "%s", topbuf);
				} else {
					cnt = max_users_in_top;
	                        }
                        }
                        user_p += sprintf(user_p, "%s", racersplit_postfix);
		}

		if (group_top != NULL && max_groups_in_top > 0) {
			topsize = 0;
                        group_p += sprintf(group_p, "%s", racersplit_prefix);
			for (cnt = first_entry; cnt < max_groups_in_top && cnt < g->v.total.groups; ++cnt) {
				topsize = sprintf(topbuf, "%s", convert_group(&g->v, g->gi[g->gi[cnt]->pos], group_top, cnt));

				/* only add next pos if we're not going out of bounds,
				 * otherwise the sprintf outside the for-loop takes
				 * care of writing the \0 on the correct spot
				 */
				if (group_p + topsize + sizeof(racersplit_postfix) < g->v.misc.top_messages[1] + sizeof(g->v.misc.top_messages[1])) {
                	                if (cnt != first_entry) {
        	                            group_p += sprintf(group_p, "%s", racersplit);
					}
					group_p += sprintf(group_p, "%s", topbuf);

				} else {
					cnt = max_groups_in_top;
				}
			}
			group_p += sprintf(group_p, "%s", racersplit_postfix);
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
				snprintf(templine, FILE_MAX, "%s ", convert_user(&g->v, g->ui[g->ui[cnt]->pos], g->gi, user_top, cnt));
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
				snprintf(templine, FILE_MAX, "%s ", convert_group(&g->v, g->gi[g->gi[cnt]->pos], group_top, cnt));
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
