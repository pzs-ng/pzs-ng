#include<dirent.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>

/* This function is only required for SunOS, all other supported OS
   have this function in their system library */

int scandir(const char *, struct dirent ***,
            int (*select)(const struct dirent *),
            int (*compar)(const struct dirent **, const struct dirent **));

int alphasort(const struct dirent **, const struct dirent **);
