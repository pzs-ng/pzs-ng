#ifndef POSTDEL_H
#define POSTDEL_H

#include "objects.h"

/* COMMENT THESE */
void writelog(GLOBAL *, char *, char *);
char **buffer_paths(GLOBAL *, char **, int *, int);
/*void getrelname(GLOBAL *);*/
/*void remove_nfo_indicator(GLOBAL *);*/
unsigned char get_filetype_postdel(GLOBAL *, char *);

#endif
