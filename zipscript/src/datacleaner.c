/* ADD MORE ERROR CHECKING IN THIE FILE */

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include "objects.h"
#include "macros.h"
#include "../conf/zsconfig.h"
#include "../include/zsconfig.defaults.h"

#ifndef PATH_MAX
 #define _LIMITS_H_
 #ifdef _SunOS_
  #include <syslimits.h>
 #else
  #include <sys/syslimits.h>
 #endif
#endif

#ifdef _SunOS_
#include "scandir.h"
#endif

#include "datacleaner.h"

int 
main(int argc, char **argv)
{
	int		zd_length;
	char		st[PATH_MAX];
	char		*wd;
	DIR		*od;

	zd_length = (int)strlen(storage);

	if (argc == 1) {
		check_dir_loop(storage, zd_length);
	} else {
		if ((zd_length + 1 + (int)strlen(argv[1])) < PATH_MAX) {
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
		check_dir_loop(st, zd_length);

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

void 
remove_dir_loop(char *path)
{
	DIR 		*dir;
	struct dirent	*dp;
	char		target[PATH_MAX];
	struct stat	sb;

	if ((chdir(path)) == -1) {
		perror(path);
		exit(EXIT_FAILURE);
	}
	
	dir = opendir(path);
	while ((dp = readdir(dir))) {
		if (dp->d_name[0] != '.' || (dp->d_name[0] == '.' && strlen(dp->d_name) > 2)) {
			stat(dp->d_name, &sb);
			if (S_ISDIR(sb.st_mode)) {
				sprintf(target, "%s/%s", path, dp->d_name);
				remove_dir_loop(target);
				rmdir(target);
				if (chdir(path) == -1) {
					perror(path);
				}
			} else
				unlink(dp->d_name);
		}
	}
	closedir(dir);
}

void 
check_dir_loop(char *path, int zd_length)
{
	DIR 		*dir1, *dir2;
	struct dirent	*dp;
	char		target    [PATH_MAX];
	struct stat	sb;

	if (chdir(path) == -1) {
		perror(path);
		exit(EXIT_FAILURE);
	}

	dir1 = opendir(path);
	while ((dp = readdir(dir1))) {
		if (dp->d_name[0] != '.') {
			stat(dp->d_name, &sb);
			if (S_ISDIR(sb.st_mode)) {
				sprintf(target, "%s/%s", path, dp->d_name);
				if ((dir2 = opendir(target + zd_length))) {
					closedir(dir2);
					check_dir_loop(target, zd_length);
					if (chdir(path) == -1) {
						perror(path);
					}
				} else {
					remove_dir_loop(target);
					rmdir(target);
					if (chdir(path) == -1) {
						perror(path);
					}
				}
			}
		}
	}
	closedir(dir1);
}

