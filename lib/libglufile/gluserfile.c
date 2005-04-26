#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gluserfile.h"

/* glFTPd userfile reader

      The functions in this file reads and parses available
      information which finally populates a GLUSER structure.
	  
      To execute a proper read, the functions should be called
      in the order:

        gluser_init();
        gluser_read();
        gluser_read();
        .
        .
        .
		gluser_free();

*/

/*************************************************************/
/*************************************************************/

void gluser_init(GLUSER *glu)
{

	glu->read = 0;
	gluser_reset(glu);

}

/* gluser_read: Fills a GLUSER datatype with information
                from specified filename which is a valid
                glFTPd user file.

                returns 1 if the specified file couldn't
                be opened, else 0.
*/
int gluser_read(GLUSER *glu, char *filename)
{
	
	char *fbuf, *complete, *space, *hash;
	long pos;
	size_t bufsize;
	FILE *fp;

	if (!(fp = fopen(filename, "r"))) {
		perror(filename);
		return 1;
	}
	
	bufsize = 128;
	fbuf = malloc(bufsize);
	
	pos = ftell(fp);

	gluser_free(glu);
	gluser_reset(glu);
	
	while (fgets(fbuf, bufsize, fp)) {
		
		if (!(complete = strchr(fbuf, '\n'))) {
		
			/* didn't get a full line, read again
			   with larger buffer */
			bufsize *= 2;
			fbuf = realloc(fbuf, bufsize);
			fseek(fp, pos, SEEK_SET);

		} else {
			
			if ((hash = strchr(fbuf, '#')))
				*hash = '\0';
				
			/* do ugly checking of file values in here */
			if ((space = strchr(fbuf, ' '))) {

				/* perhaps we got a valid line */

				*space = '\0'; space++;

				space = strip_whitespaces(space);

				/* this shit makes me puke, it must be replacable
				   with some kind of loop */
				if (strcasecmp(fbuf, "USER") == 0)
					glu->user = strdup(space);
				else if (strcasecmp(fbuf, "HOMEDIR") == 0)
					glu->homedir = strdup(space);
				else if (strcasecmp(fbuf, "GENERAL") == 0)
					ufile_general(glu->general, space);
				else if (strcasecmp(fbuf, "LOGINS") == 0)
					ufile_logins(glu->logins, space);
				else if (strcasecmp(fbuf, "TIMEFRAME") == 0)
					ufile_timeframe(&glu->timeframe, space);
				else if (strcasecmp(fbuf, "FLAGS") == 0)
					ufile_flags(glu->flags, space);
				else if (strcasecmp(fbuf, "TAGLINE") == 0)
					glu->tagline = strdup(space);
				else if (strcasecmp(fbuf, "DIR") == 0)
					glu->dir = strdup(space);
				else if (strcasecmp(fbuf, "ADDED") == 0)
					ufile_added(&glu->added, space);
				else if (strcasecmp(fbuf, "EXPIRES") == 0)
					glu->expires = strtol(space, NULL, 10);
				else if (strcasecmp(fbuf, "RATIO") == 0)
					ufile_ratio(&glu->ratio, space);
				else if (strcasecmp(fbuf, "CREDITS") == 0)
					ufile_credits(&glu->credits, space);
				else if (strcasecmp(fbuf, "TIME") == 0)
					ufile_time(&glu->time, space);
				
				else if (strcasecmp(fbuf, "GROUP") == 0)
					ufile_group(&glu->groups, space);
				else if (strcasecmp(fbuf, "PRIVATE") == 0)
					ufile_private(&glu->private, space);
					
				else if (strcasecmp(fbuf, "ALLUP") == 0)
					ufile_section(&glu->sections[0], space);
				else if (strcasecmp(fbuf, "ALLDN") == 0)
					ufile_section(&glu->sections[1], space);
				else if (strcasecmp(fbuf, "WKUP") == 0)
					ufile_section(&glu->sections[2], space);
				else if (strcasecmp(fbuf, "WKDN") == 0)
					ufile_section(&glu->sections[3], space);
				else if (strcasecmp(fbuf, "DAYUP") == 0)
					ufile_section(&glu->sections[4], space);
				else if (strcasecmp(fbuf, "DAYDN") == 0)
					ufile_section(&glu->sections[5], space);
				else if (strcasecmp(fbuf, "MONTHUP") == 0)
					ufile_section(&glu->sections[6], space);
				else if (strcasecmp(fbuf, "MONTHDN") == 0)
					ufile_section(&glu->sections[7], space);
				else if (strcasecmp(fbuf, "NUKE") == 0)
					ufile_section(&glu->sections[8], space);
				
				else if (strcasecmp(fbuf, "IP") == 0)
					ufile_ip(&glu->ip, space);

			}
			
			pos = ftell(fp);

		}
		
	}

	glu->read = 1;
	
	free(fbuf);
	fclose(fp);

	return 0;

}

/* gluser_free: Free all allocated memory for a GLUSER structure
*/
void gluser_free(GLUSER *glu)
{

	int i;

	if (glu->read == 0)
		return;

	if (glu->user)
		free(glu->user);
	if (glu->homedir)
		free(glu->homedir);
	if (glu->tagline)
		free(glu->tagline);
	if (glu->dir)
		free(glu->dir);

	if (glu->ratio.val)
		free(glu->ratio.val);
	if (glu->credits.val)
		free(glu->credits.val);
	if (glu->time.val)
		free(glu->time.val);
	if (glu->timeframe.val)
		free(glu->timeframe.val);

	if (glu->groups.grps)
		free(glu->groups.grps);

	if (glu->private.grps)
		free(glu->private.grps);

	for (i = 0; i < 9; i++)
		if (glu->sections[i].s)
			free(glu->sections[i].s);

	if (glu->ip.ips) {
		for (i = 0; i < glu->ip.n; i++)
			if (glu->ip.ips[i].ident)
				free(glu->ip.ips[i].ident);
		free(glu->ip.ips);
	}

}

/* gluser_reset: Set all members of a GLUSER structure to 0.
*/
void _gluser_reset(GLUSER *glu)
{

	int i;

	glu->user = glu->homedir = glu->tagline = glu->dir = 0;
	
	glu->added.time = 0;
	
	glu->expires = 0;

	glu->ratio.n = glu->credits.n = glu->time.n = glu->ip.n = 
	               glu->groups.n = glu->private.n = 
				   glu->timeframe.n = 0;
				   
	glu->ratio.val = glu->credits.val = glu->time.val =
	                 glu->timeframe.val = 0;

	glu->groups.grps = glu->private.grps = 0;
	glu->ip.ips = 0;
	
	for (i = 0; i < 22; i++)
		glu->flags[i] = 0;

	for (i = 0; i < 5; i++)
		glu->general[i] = 0;
		
	for (i = 0; i < 4; i++)
		glu->logins[i] = 0;
		
	for (i = 0; i < 9; i++) {
		glu->sections[i].s = 0;
		glu->sections[i].n = 0;
	}
	
}

/* ufile_general: Store data from a GENERAL field
*/
void ufile_general(int *general, char *s)
{

	int i;
	char *comma, *space;

	if ((comma = strchr(s, ','))) {
		*comma = '\0'; space = comma+1;
		
		general[0] = strtol(s, NULL, 10);
		
		s = space;
		
		for (i = 1; i < 5; i++) {
			general[i] = strtol(s, NULL, 10);

			if ((space = strchr(s, ' '))) {
				*space = '\0';
				s = space+1;
			}
		}
	}

}

/* ufile_logins: Store data from a LOGINS field
*/
void ufile_logins(int *logins, char *s)
{

	int i;
	char *space;
	
	for (i = 0; i < 4; i++) {
		logins[i] = strtol(s, NULL, 10);
		
		if ((space = strchr(s, ' '))) {
			*space = '\0';
			s = space+1;
		}
	}

}

/* ufile_flags: Read FLAGS for a user and set eventual
                flags in the GLUSER structure to 1 if they
                are present. There are 22 different flags,
                and each flag can be accessed with

                  glu->flags[number]

                where number represents a flag in the numerical
                order
				
                  12345678ABCDEFGHIJKLMN

                Meaning if the user has flags "1A", the GLUSER
                data would have 

                  glu->flags[0]
                  glu->flags[9]

                set.
*/
void ufile_flags(short *flags, char *s)
{

	int i;
	char *flag;
	char glflags[] = { '1', '2', '3', '4', '5', '6', '7',
	                 '8', 'A', 'B', 'C', 'D', 'E', 'F',
					 'G', 'H', 'I', 'J', 'K', 'L', 'M',
					 'N' };

	for (i = 0; i < 22; i++)
		for (flag = s; *flag; flag++)
			if (*flag == glflags[i])
				flags[i] = 1;

}

void ufile_added(ADDED *added, char *s)
{
	
	char *space;

	if ((space = strchr(s, ' '))) {
		
		*space = 0; space++;
		strncpy(added->uname, space, 24);

	}

	added->time = strtol(s, NULL, 10);

}

/* ufile_timeframe: Parses a TIMEFRAME field from a userfile
*/
void ufile_timeframe(AINT *timeframe, char *s)
{
	
	strtointarray(timeframe, s);

}

/* ufile_ratio: Reads a RATIO field and stores values in an AINT
*/
void ufile_ratio(AINT *ratio, char *s)
{

	strtointarray(ratio, s);

}

/* ufile_credits: Reads a CREDITS field and stores values in an AINT
*/
void ufile_credits(AINT *credits, char *s)
{
	
	strtointarray(credits, s);

}

/* ufile_time: Reads a TIME field and stores values in an AINT
*/
void ufile_time(AINT *time, char *s)
{

	strtointarray(time, s);
	
}

/* ufile_ip: Stored an IP and ident in an array of IP
*/
int ufile_ip(AIP *ip, char *s)
{

	char *at;

	if ((at = strchr(s, '@'))) {
	
		*at = '\0'; at++;
		
		ip->ips = realloc(ip->ips, sizeof(IP)*(ip->n+1));
		
		ip->ips[ip->n].ident = strdup(s);
		strncpy(ip->ips[ip->n].ip, at, 16);

		ip->n++;

		return 0;

	}

	return 1;

}

/* ufile_section: Parses all available sections for the specified line
*/
void ufile_section(ASECTION *sect, char *s)
{

	int i, u;
	AINT temp;

	temp.val = 0;
	strtointarray(&temp, s);

	sect->s = malloc(sizeof(SECTION)*(temp.n/3));
	
	for (i = u = 0; i < temp.n; u++) {
		sect->s[u].files = temp.val[i];
		sect->s[u].kilobytes = temp.val[i+1];
		sect->s[u].seconds = temp.val[i+2];
		i += 3;
	}
	
	/* number of sections stored */
	sect->n = temp.n/3;

	free(temp.val);

}

/* ufile_group: Parses a GROUP line and stores it in a GLUSER structure.
*/
int ufile_group(AGROUP *groups, char *s)
{

	char *space;

	groups->grps = realloc(groups->grps, sizeof(GROUP)*(groups->n+1));
	groups->grps[groups->n].gadmin = -1;
		
	if ((space = strchr(s, ' '))) {

		*space = '\0'; space++;
		groups->grps[groups->n].gadmin = strtol(space, NULL, 10);

	}
	
	strncpy(groups->grps[groups->n].gname, s, 16);
	groups->n++;

	return 0;

}

/* ufile_private: Parses a PRIVATE line and stores it in a
                  GLUSER structure
*/
void ufile_private(AGROUP *priv, char *s)
{

	priv->grps = realloc(priv->grps, sizeof(GROUP)*(priv->n+1));
	
	strncpy(priv->grps[priv->n].gname, s, 16);

	priv->n++;
	
}

/* strtointarray: Parses a line in the format "number number number"
                  and stores each number individually in an AINT
*/
AINT *strtointarray(AINT *target, char *s)
{

	int i, count = 10;
	char *space;
	
	target->val = realloc(target->val, sizeof(int)*count);
	
	for (i = 0; ; i++) {

		if (i == count) {
			count *= 2;
			target->val = realloc(target->val, sizeof(int)*count);
		}
		
		target->val[i] = strtol(s, NULL, 10);
		
		if ((space = strchr(s, ' '))) {
			*space = '\0';
			s = space+1;
		} else
			break;

	}
	
	/* contract */
	target->n = i+1;
	target->val = realloc(target->val, sizeof(int)*(target->n));
	
	return target;

}

/* strip_whitespaces: Removes trailing whitespaces and returns
                      a pointer to a place in the string without
                      leading whitespaces
*/
char *strip_whitespaces(char *s)
{

	size_t len = strlen(s)-1;
	
	while (s[len] == ' '  ||
	       s[len] == '\t' ||
	       s[len] == '\n' ||
	       s[len] == '\r') {
		s[len] = '\0';
		len--;
	}

	while (*s == ' ' || *s == '\t')
		s++;

	return s;

}

