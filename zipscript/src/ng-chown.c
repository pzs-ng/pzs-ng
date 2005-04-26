/****************************************************************************************************************/
/*                                                                                                              */
/* NG-CHOWN v0.1 (c) 2004 Team pzs-ng                                                                           */
/*                                                                                                              */
/* This util will cange the uid or gid of the dir/file passed as argument, or                                   */
/* dir and all files inside, or just the files inside the dir. Uhm.. anyway,                                    */
/* here's how you use it:                                                                                       */
/*                                                                                                              */
/* ng-chown <uid> <gid> <uid_flag> <gid_flag> <files_flag> <dir/file_flag> <u_name> <g_name> <name of dir/file> */
/*                                                                                                              */
/* <uid> == user id (number between 1 and 65535)                                                                */
/* <gid> == group id (number between 1 and 65535)                                                               */
/* <uid_flag> == if set to 1, the uid of the file/dir will be modified                                          */
/* <gid_flag> == if set to 1, the gid of the file/dir will be modified                                          */
/* <files_flag> == if set to 1, the files in the dir <name of dir/file> will be modified                        */
/* <dir/file_flag> == if set to 1, the dir/file <name of dir/file> will be modified                             */
/* <u_name> == name of user - only used if uid=0 *and* gid=0 - use '-' to disable                               */
/* <g_name> == name of group - only used if uid=0 *and* gid=0 - use '-' to disable                              */
/* <name of dir/file> == name of the file dir to which the changes is to be applied                             */
/*                                                                                                              */
/****************************************************************************************************************/

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

#ifndef PATH_MAX
# ifndef _SunOS_
#  define _LIMITS_H_
#  include <sys/syslimits.h>
# endif
#endif

#ifndef PATH_MAX
# define PATH_MAX 1024
# define _ALT_MAX
#endif

#ifndef NAME_MAX
# define NAME_MAX 255
# ifndef _ALT_MAX
#  define _ALT_MAX
# endif
#endif

#ifdef _SunOS_
#include "scandir.h"
#endif

#include "objects.h"
#include "../conf/zsconfig.h"
#include "zsconfig.defaults.h"

#include "ng-chown.h"

struct dirent	**dirlist;
unsigned int	direntries, n = 0;

int 
main(int argc, char *argv[])
{
	int	my_result = 1, new_user = 0,
		new_group = 0, user_flag = 0,
		group_flag = 0, files_flag = 0,
		dir_flag = 0;

	char	my_path[PATH_MAX];

	if (argc != 10) {
		printf("%s - Error: Wrong number of args given. (%d)\n", argv[0], argc);
		printf("%s - Syntax: %s <uid> <gid> <uid_flag> <gid_flag> <files_flag> <dir/file_flag> <user name> <group name> <name of dir/file>\n", argv[0], argv[0]);
		return 1;
	}
	
	new_user = strtol(argv[1], NULL, 10);	/* uid of user */
	new_group = strtol(argv[2], NULL, 10);	/* gid of user */
	
	if (allow_uid_change_in_ng_chown)
		user_flag = strtol(argv[3], NULL, 10);	/* change the uid of the files/dir if > 0      */

	if (allow_gid_change_in_ng_chown)
		group_flag = strtol(argv[4], NULL, 10);	/* change the gid of the files/dir if > 0      */

	if (allow_files_chown_in_ng_chown)
		files_flag = strtol(argv[5], NULL, 10);	/* make changes on the files in the dir if > 0 */
		
	if (allow_dir_chown_in_ng_chown)
		dir_flag = strtol(argv[6], NULL, 10);	/* make changes on the dir itself if > 0       */

	if ((new_user == 0) && (new_group == 0)) {
		if ((int)strlen(argv[7]) < 25)
			new_user = (int)get_gluid(PASSWDFILE, argv[7]);
		if ((int)strlen(argv[8]) < 25)
			new_group = (int)get_glgid(GROUPFILE, argv[8]);
	}
	
	if (((int)strlen(argv[9]) < PATH_MAX) && (new_user < 65536) && (new_group < 65535)) {
		sprintf(my_path, "%s", argv[9]);
	} else {
		printf("%s - Error in argument(s)\n", argv[0]);
		return 1;
	}

	my_result = myscan(my_result, new_user, new_group, user_flag, group_flag, files_flag, dir_flag, my_path, argv[0]);
	
	if (my_result == 0) {
		if ((new_user > 0) || (new_group > 0)) {
			if (setuid(0) == -1) {
				printf("%s - Error: Failed to change uid : %s\n", argv[0], strerror(errno));
				return 1;
			}
			if (setgid(0) == -1) {
				printf("%s - Error: Failed to change gid : %s\n", argv[0], strerror(errno));
				return 1;
			}
			my_result = myscan(my_result, new_user, new_group, user_flag, group_flag, files_flag, dir_flag, my_path, argv[0]);
		}
	} else {
		printf("\n%s: Could not change ownership of '%s':\n", argv[0], argv[9]);
		printf("\t- user  id set to: %5d (was: %5d / %s)\n", new_user, (int)strtol(argv[1], NULL, 10), argv[7]);
		printf("\t- group id set to: %5d (was: %5d / %s)\n", new_group, (int)strtol(argv[2], NULL, 10), argv[8]);
		printf("\t- allow_uid_change_in_ng_chown : %s - flag set to %d (was %d)\n", !allow_uid_change_in_ng_chown ? "FALSE" : "TRUE ", user_flag, (int)strtol(argv[3], NULL, 10));
		printf("\t- allow_gid_change_in_ng_chown : %s - flag set to %d (was %d)\n", !allow_gid_change_in_ng_chown ? "FALSE" : "TRUE ", group_flag, (int)strtol(argv[4], NULL, 10));
		printf("\t- allow_files_chown_in_ng_chown: %s - flag set to %d (was %d)\n", !allow_files_chown_in_ng_chown ? "FALSE" : "TRUE ", files_flag, (int)strtol(argv[5], NULL, 10));
		printf("\t- allow_dir_chown_in_ng_chown  : %s - flag set to %d (was %d)\n", !allow_dir_chown_in_ng_chown ? "FALSE" : "TRUE ", dir_flag, (int)strtol(argv[6], NULL, 10));
		
	}
	
	return my_result;
}

int 
#if defined(__linux__)
selector3(const struct dirent *d)
#elif defined(__NetBSD__)
selector3(const struct dirent *d)
#else
selector3(struct dirent *d)
#endif
{
	struct stat st;
	if ((stat(d->d_name, &st) < 0) ||
	    S_ISDIR(st.st_mode))
		return 0;
	return 1;
}

short int 
matchpath2(char *instr, char *path)
{
	int		pos = 0;

	do {
		switch (*instr) {
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

int 
myscandir(char *my_path, char *my_name)
{
	if (chdir(my_path) != -1) {
		if (matchpath2(group_dirs, my_path) || matchpath2(zip_dirs, my_path) || matchpath2(sfv_dirs, my_path)) {
			if (direntries > 0) {
				while (direntries--) {
					ng_free3(dirlist[direntries]);
				}
				ng_free3(dirlist);
			}
			direntries = scandir(".", &dirlist, selector3, alphasort);
			return 0;
		} else {
			printf("%s - Error: %s is not in allowed paths (group/zip/sfv-dirs).\n", my_name, my_path);
			return 1;
		}
	} else {
		printf("%s - Error: Unable to chdir to %s : %s\n", my_name, my_path, strerror(errno));
		return 1;
	}
}

int 
myscan(int my_result, int new_user, int new_group, int user_flag, int group_flag, int files_flag, int dir_flag, char *my_path, char *my_name)
{
	int		my_total = 0;

	/* change the uid or gid depending on the flag */
	if (user_flag == 0) {
		new_user = -1;
	}
	if (group_flag == 0) {
		new_group = -1;
	}
	if (files_flag == 1) {
		if (!myscandir(my_path, my_name)) {
			n = direntries;
			while (n--) {
				my_total += close(open(dirlist[n]->d_name, O_NONBLOCK));
				if (new_user != 0 && new_group != 0) {
					chown(dirlist[n]->d_name, new_user, new_group);
				}
			}
			if ((my_result == 0) && (dir_flag == 1)) {
				if (new_user != 0 && new_group != 0) {
					chown(my_path, new_user, new_group);
				}
			}
			return my_total;
		} else {
			return 1;
		}
	} else {
		if (dir_flag == 1) {
			my_result = close(open(my_path, O_NONBLOCK));
			if (my_result == 0) {
				if (new_user != 0 && new_group != 0) {
					chown(my_path, new_user, new_group);
				}
			} else {
				printf("%s - Error: Unable to read file %s\n", my_name, my_path);
			}
		}
		return my_result;
	}
}

uid_t
get_gluid(char *passwdfile, char *user_name)
{
	char	       *f_buf = 0;
	char	       *u_name = 0;
	uid_t		u_id = 0;
	off_t		f_size;
	int		f, i, m, l, l_start = 0;
	struct stat	fileinfo;

#if (change_spaces_to_underscore_in_ng_chown)
	char	       *u_modname = 0;

	u_modname = ng_realloc3(u_modname, (int)strlen(user_name) * sizeof(char) + 1);
	for (i = 0; i < ((int)strlen(user_name) + 1); i++) {
		if (user_name[i] == ' ')
			sprintf(u_modname + i, "_");
		else
			memcpy(u_modname + i, user_name + i, 1);
	}
#endif

	f = open(passwdfile, O_NONBLOCK);
	fstat(f, &fileinfo);
	f_size = fileinfo.st_size;
	f_buf = ng_realloc3(f_buf, f_size);
	read(f, f_buf, f_size);

	for (i = 0; i < f_size; i++) {
		if (f_buf[i] == '\n' || i == f_size) {
			f_buf[i] = 0;
			m = l_start;
			while (f_buf[m] != ':' && m < i)
				m++;
			if (m != l_start) {
				f_buf[m] = 0;
				u_name = f_buf + l_start;
				m = i;
				for (l = 0; l < 4; l++) {
					while (f_buf[m] != ':' && m > l_start)
						m--;
					f_buf[m] = 0;
				}
				while (f_buf[m] != ':' && m > l_start)
					m--;
#if (change_spaces_to_underscore_in_ng_chown)
				if ((m != i) && ((int)strlen(u_name) == (int)strlen(user_name)) && !strcmp(u_name, u_modname)){
#else
				if ((m != i) && ((int)strlen(u_name) == (int)strlen(user_name)) && !strcmp(u_name, user_name)){
#endif
					u_id = strtol(f_buf + m + 1, NULL, 10);
					break;
				}
			}
			l_start = i + 1;
		}
	}
	close(f);
	ng_free3(f_buf);
#if (change_spaces_to_underscore_in_ng_chown)
	ng_free3(u_modname);
#endif
	return u_id;
}

gid_t
get_glgid(char *groupfile, char *group_name)
{
	char	       *f_buf = 0;
	char	       *g_name = 0;
	gid_t		g_id = 0;
	off_t		f_size;
	int		f, i, m, l_start = 0;
	struct stat	fileinfo;

#if (change_spaces_to_underscore_in_ng_chown)
	char	       *g_modname = 0;

	g_modname = ng_realloc3(g_modname, (int)strlen(group_name) * sizeof(char) + 1);
	for (i = 0; i < ((int)strlen(group_name) + 1); i++) {
		if (group_name[i] == ' ')
			sprintf(g_modname + i, "_");
		else
			memcpy(g_modname + i, group_name + i, 1);
	}
#endif

	f = open(groupfile, O_NONBLOCK);
	fstat(f, &fileinfo);
	f_size = fileinfo.st_size;
	f_buf = ng_realloc3(f_buf, f_size);
	read(f, f_buf, f_size);

	for (i = 0; i < f_size; i++) {
		if (f_buf[i] == '\n' || i == f_size) {
			f_buf[i] = 0;
			m = l_start;
			while (f_buf[m] != ':' && m < i)
				m++;
			if (m != l_start) {
				f_buf[m] = 0;
				g_name = f_buf + l_start;
				m = i;
				while (f_buf[m] != ':' && m > l_start)
					m--;
				f_buf[m] = 0;
				while (f_buf[m] != ':' && m > l_start)
					m--;
#if (change_spaces_to_underscore_in_ng_chown)
				if ((m != i) && ((int)strlen(g_name) == (int)strlen(group_name)) && !strcmp(g_name, g_modname)){
#else
				if ((m != i) && ((int)strlen(g_name) == (int)strlen(group_name)) && !strcmp(g_name, group_name)){
#endif
					g_id = strtol(f_buf + m + 1, NULL, 10);
					break;
				}
			}
			l_start = i + 1;
		}
	}

	close(f);
	ng_free3(f_buf);
#if (change_spaces_to_underscore_in_ng_chown)
	ng_free3(g_modname);
#endif
	return g_id;
}

void *
ng_realloc3(void *mempointer, int memsize)
{
	mempointer = realloc(mempointer, memsize);
	if (mempointer == NULL) {
		printf("ng_realloc3: realloc failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	bzero(mempointer, memsize);
	return mempointer;
}

void *
ng_free3(void *mempointer)
{
	if (mempointer)
		free(mempointer);
	return 0;
}

