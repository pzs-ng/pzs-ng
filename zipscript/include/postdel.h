#ifndef POSTDEL_H
#define POSTDEL_H

#include "objects.h"

void writelog(struct LOCATIONS *, char *, char *);
char **buffer_paths(struct LOCATIONS *, char **, int *, int);
void getrelname(struct LOCATIONS *);
void remove_nfo_indicator(struct LOCATIONS *);
unsigned char get_filetype(struct LOCATIONS *, char *);

#endif
