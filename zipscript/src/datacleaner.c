#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>
#include "objects.h"
#include "../conf/zsconfig.h"

struct stat	entry_stat;
int		zd_length;

#ifndef PATH_MAX
# define _LIMITS_H_
# include <sys/syslimits.h>
#endif

void remove_dir_loop(char *path) {
	struct dirent	**list;
	int		n;
	char		target[PATH_MAX];

	chdir(path);
	n = scandir(path, &list, 0, 0);
	while ( n-- ) if ( list[n]->d_name[0] != '.' ) {
		stat(list[n]->d_name, &entry_stat);
		if ( S_ISDIR(entry_stat.st_mode) ) {
			sprintf(target, "%s/%s", path, list[n]->d_name);
			remove_dir_loop(target);
			rmdir(target);
			chdir(path);
		} else unlink(list[n]->d_name);
		free(list[n]);
	}
	free(list);
}


void check_dir_loop(char *path) {
	struct dirent	**list;
	int		n;
	char		target[PATH_MAX];
 
	chdir(path);
	n = scandir(path, &list, 0, 0);
	while ( n-- ) if ( list[n]->d_name[0] != '.' ) {
		stat(list[n]->d_name, &entry_stat);
		if ( S_ISDIR(entry_stat.st_mode) ) {
			sprintf(target, "%s/%s", path, list[n]->d_name );
			if (opendir(target + zd_length)) {
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


int main (int argc, char **argv) {
	if (geteuid() != 0) {
		printf("%s: you can only run this program under effective UID 0.\n", argv[0]);
		printf("\t(read README.datacleaner for information on how to change effective UID)\n");
		return 1;
	} else {
		zd_length = strlen(storage);
		chroot(site_root);
		check_dir_loop(storage);
	}
	return 0;
}
