#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifndef PATH_MAX
 #define _LIMITS_H_
 #include <sys/syslimits.h>
#endif
#include "../conf/zsconfig.h"

struct dirent   **dirlist;
unsigned int direntries,n = 0;


#if defined(__linux__)
int selector3 (const struct dirent *d) {
#elif defined(__NetBSD__)
int selector3 (const struct dirent *d) {
#else
int selector3 (struct dirent *d) {
#endif
        struct stat st;
        if ((stat(d->d_name, &st) < 0) || S_ISDIR(st.st_mode)) return 0;
        return 1;
}

short int matchpath2(char *instr, char *path) {
  int pos = 0;
 
  do {
    switch ( *instr ) {
      case 0:
      case ' ':
        if (strncmp(instr - pos, path, pos - 1) == 0) {
            return 1;
        }
        pos = 0;
        break;
      default:
        pos++;
        break;
    }
  } while (*instr++);
  return 0;
}

int myscandir(char *my_path) {
  if ( ( chdir(my_path) != -1 ) && ( ( matchpath2(group_dirs, my_path) ) || ( matchpath2(zip_dirs, my_path ) ) || ( matchpath2(sfv_dirs, my_path) ) ) ) {
    if ( direntries > 0 ) {
      while ( direntries-- ) {
        free(dirlist[direntries]);
      }
      free(dirlist);
    }
    direntries = scandir(".", &dirlist, selector3, alphasort);
    return 0;
  } else {
    printf("Error: Unable to chdir to %s\n",my_path);
    return 1;
  }
}

int myscan(int flag, int new_user, int new_group, char *my_path) {
  int my_total = 0;

  if ( myscandir(my_path) == 0 ) {
    n = direntries;
    while ( n-- ) {
      my_total += access(dirlist[n]->d_name, R_OK);
      if (flag == 0)
        chown(dirlist[n]->d_name, new_user, new_group);
    }
    if (flag == 0)
      chown(my_path, new_user, new_group);
    return my_total;
  } else {
    return 1;
  }
}

int main(int argc, char *argv[]) {

  int my_result = 1;
  int new_user;
  int new_group;
  char my_path[PATH_MAX];

  if ( argc != 4 ) {
    printf("%s : Error - Wrong number of args given.\n", argv[0]);
    return 1;
  }

  new_user = atoi(argv[1]);
  new_group = atoi(argv[2]);
  if ( ( strlen(argv[3]) < PATH_MAX ) && ( new_user < 65536 ) && ( new_group < 65535 ) ) { 
    sprintf(my_path, "%s", argv[3]);
  } else {
    printf("%s : Error in argument(s)\n", argv[0]);
    return 1;
  }
  my_result = myscan(my_result, new_user, new_group, my_path);
  if ( my_result == 0 ) {
    if ( (new_user > 0) || (new_group > 0) ) {
      setuid(0);
      setgid(0);
      my_result = myscan(my_result, new_user, new_group, my_path);
    }
  }
  return my_result;
}

