#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include "config.h"

#include "objects.h"
#include "macros.h"
#include "../conf/zsconfig.h"
#include "../include/zsconfig.defaults.h"

struct stat	entry_stat;
int		zd_length;

#ifndef PATH_MAX
 #define _LIMITS_H_
 #ifdef _SunOS_
  #include <syslimits.h>
 #else
  #include <sys/syslimits.h>
 #endif
#endif

void 
remove_dir_loop(char *path)
{
	struct dirent **list;
	int		n;
	char		target    [PATH_MAX];

	chdir(path);
	n = scandir(path, &list, 0, 0);
	while (n--)
		if (list[n]->d_name[0] != '.') {
			stat(list[n]->d_name, &entry_stat);
			if (S_ISDIR(entry_stat.st_mode)) {
				sprintf(target, "%s/%s", path, list[n]->d_name);
				remove_dir_loop(target);
				rmdir(target);
				chdir(path);
			} else
				unlink(list[n]->d_name);
			free(list[n]);
		}
	free(list);
}

void 
check_dir_loop(char *path)
{
	struct dirent **list;
	int		n;
	char		target    [PATH_MAX];
	DIR            *dirp;

	chdir(path);
	n = scandir(path, &list, 0, 0);
	if (n < 2)
		exit(2);
	while (n--)
		if (list[n]->d_name[0] != '.') {
			stat(list[n]->d_name, &entry_stat);
			if (S_ISDIR(entry_stat.st_mode)) {
				sprintf(target, "%s/%s", path, list[n]->d_name);
				if ((dirp = opendir(target + zd_length))) {
					closedir(dirp);
					check_dir_loop(target);
					chdir(path);
				} else {
					remove_dir_loop(target);
					rmdir(target);
					chdir(path);
				}
			}
			free(list[n]);
		}
	free(list);
}

int 
main(int argc, char **argv)
{
	char		st        [PATH_MAX];
	char		*wd;
	DIR		*od;

	zd_length = strlen(storage);

	if (argc == 1) {
		check_dir_loop(storage);
	} else {
		if ((zd_length + 1 + strlen(argv[1])) < PATH_MAX) {
			if ( !strncmp(argv[1], "RMD ", 4)) {
				/* script is called as a cscript for RMD */
				if (!strncmp(argv[1] + 4, "/", 1)) {
					/* client uses full path to dir */
					sprintf(st, storage "/%s/%s", sitepath_dir, argv[1] + 4);
				} else {
					/* client give only name of dir */
					if (( wd = getcwd(NULL, PATH_MAX)) == NULL) {
						exit (2);
					} else {
						sprintf(st, storage "/%s/%s", wd, argv[1] + 4);
						free(wd);
					}
				}
			} else if ( !strncmp(argv[1], "/", 1)) {
				/* script is called with an argument from (chroot) shell */
				sprintf(st, storage "/%s", argv[1]);
				printf("Checking dir: %s\n", st);
			} else {
				/* script is called with bad args - scanning current dirs */
				if (( wd = getcwd(NULL, PATH_MAX)) == NULL) {
						exit (2);
				} else {
					sprintf(st, storage "/%s", wd);
					free(wd);
				}
			}
		}
		/* check subdirs */
		check_dir_loop(st);

		/* check current dir */
		if (( od = opendir(st + zd_length)) == NULL) {
			remove_dir_loop(st);
			rmdir(st);
		} else {
			closedir(od);
		}
	}
	return 0;
}
