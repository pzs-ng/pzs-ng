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
	if (n<2) exit(2);
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
char	st[PATH_MAX];

/*
	if (geteuid() != 0) {
		printf("%s: you can only run this program under effective UID 0.\n", argv[0]);
		printf("%s: (read README.datacleaner for information on how to change effective UID)\n", argv[0]);
		return 1;
	} else {
		zd_length = strlen(storage);
		chroot(site_root);
		check_dir_loop(storage);
	}
	return 0;
*/

	zd_length = strlen(storage);

	if ( argc == 1 ) {
		check_dir_loop(storage);
	} else {
		if ((zd_length + 1 + strlen(argv[1]))< PATH_MAX)
			sprintf(st, storage "/%s", argv[1]);

		/* check subdirs */
		check_dir_loop(st);

		/* check current dir */
		if (! opendir(st + zd_length)) {
			remove_dir_loop(st);
			rmdir(st);
		}
	}
	return 0;
}
