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

#include "objects.h"
#include "../conf/zsconfig.h"
#include "zsconfig.defaults.h"

struct dirent **dirlist;
unsigned int	direntries, n = 0;


#if defined(__linux__)
int 
selector3(const struct dirent *d)
#elif defined(__NetBSD__)
	int		selector3  (const struct dirent *d)
#else
int 
selector3(struct dirent *d)
#endif
{
	struct stat	st;
	if ((stat(d->d_name, &st) < 0) || S_ISDIR(st.st_mode))
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
	if ((chdir(my_path) != -1) && ((matchpath2(group_dirs, my_path)) || (matchpath2(zip_dirs, my_path)) || (matchpath2(sfv_dirs, my_path)))) {
		if (direntries > 0) {
			while (direntries--) {
				free(dirlist[direntries]);
			}
			free(dirlist);
		}
		direntries = scandir(".", &dirlist, selector3, alphasort);
		return 0;
	} else {
		printf("%s - Error: Unable to chdir to %s\n", my_name, my_path);
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
	int		f, n, m, l, l_start = 0;
	struct stat	fileinfo;

#if (change_spaces_to_underscore_in_ng_chown)
	char	       *u_modname = 0;

	u_modname = malloc(strlen(user_name) * sizeof(char) + 1);
	for (n = 0; n < ((int)strlen(user_name) + 1); n++) {
		if (user_name[n] == ' ')
			sprintf(u_modname + n, "_");
		else
			memcpy(u_modname + n, user_name + n, 1);
	}
#endif

	f = open(passwdfile, O_NONBLOCK);
	fstat(f, &fileinfo);
	f_size = fileinfo.st_size;
	f_buf = malloc(f_size);
	read(f, f_buf, f_size);

	for (n = 0; n < f_size; n++) {
		if (f_buf[n] == '\n' || n == f_size) {
			f_buf[n] = 0;
			m = l_start;
			while (f_buf[m] != ':' && m < n)
				m++;
			if (m != l_start) {
				f_buf[m] = 0;
				u_name = f_buf + l_start;
				m = n;
				for (l = 0; l < 4; l++) {
					while (f_buf[m] != ':' && m > l_start)
						m--;
					f_buf[m] = 0;
				}
				while (f_buf[m] != ':' && m > l_start)
					m--;
#if (change_spaces_to_underscore_in_ng_chown)
				if ((m != n) && (strlen(u_name) == strlen(user_name)) && !strcmp(u_name, u_modname)){
#else
				if ((m != n) && (strlen(u_name) == strlen(user_name)) && !strcmp(u_name, user_name)){
#endif
					u_id = atoi(f_buf + m + 1);
					break;
				}
			}
			l_start = n + 1;
		}
	}
	close(f);
	free(f_buf);
#if (change_spaces_to_underscore_in_ng_chown)
	free(u_modname);
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
	int		f, n, m, l_start = 0;
	struct stat	fileinfo;

#if (change_spaces_to_underscore_in_ng_chown)
	char	       *g_modname = 0;

	g_modname = malloc(strlen(group_name) * sizeof(char) + 1);
	for (n = 0; n < ((int)strlen(group_name) + 1); n++) {
		if (group_name[n] == ' ')
			sprintf(g_modname + n, "_");
		else
			memcpy(g_modname + n, group_name + n, 1);
	}
#endif

	f = open(groupfile, O_NONBLOCK);
	fstat(f, &fileinfo);
	f_size = fileinfo.st_size;
	f_buf = malloc(f_size);
	read(f, f_buf, f_size);

	for (n = 0; n < f_size; n++) {
		if (f_buf[n] == '\n' || n == f_size) {
			f_buf[n] = 0;
			m = l_start;
			while (f_buf[m] != ':' && m < n)
				m++;
			if (m != l_start) {
				f_buf[m] = 0;
				g_name = f_buf + l_start;
				m = n;
				while (f_buf[m] != ':' && m > l_start)
					m--;
				f_buf[m] = 0;
				while (f_buf[m] != ':' && m > l_start)
					m--;
#if (change_spaces_to_underscore_in_ng_chown)
				if ((m != n) && (strlen(g_name) == strlen(group_name)) && !strcmp(g_name, g_modname)){
#else
				if ((m != n) && (strlen(g_name) == strlen(group_name)) && !strcmp(g_name, group_name)){
#endif
					g_id = atoi(f_buf + m + 1);
					break;
				}
			}
			l_start = n + 1;
		}
	}

	close(f);
	free(f_buf);
#if (change_spaces_to_underscore_in_ng_chown)
	free(g_modname);
#endif
	return g_id;
}


int 
main(int argc, char *argv[])
{
	int		my_result = 1;
	int		new_user = 0;
	int		new_group = 0;
	int		user_flag = 0;
	int		group_flag = 0;
	int		files_flag = 0;
	int		dir_flag = 0;
	char		my_path   [PATH_MAX];

	if (argc != 10) {
		printf("%s - Error: Wrong number of args given. (%d)\n", argv[0], argc);
		printf("%s - Syntax: %s <uid> <gid> <uid_flag> <gid_flag> <files_flag> <dir/file_flag> <user name> <group name> <name of dir/file>\n", argv[0], argv[0]);
		return 1;
	}
	new_user = atoi(argv[1]);	/* uid of user */
	new_group = atoi(argv[2]);	/* gid of user */
	if (allow_uid_change_in_ng_chown) {
		user_flag = atoi(argv[3]);	/* change the uid of the files/dir if > 0      */
	}
	if (allow_gid_change_in_ng_chown) {
		group_flag = atoi(argv[4]);	/* change the gid of the files/dir if > 0      */
	}
	if (allow_files_chown_in_ng_chown) {
		files_flag = atoi(argv[5]);	/* make changes on the files in the dir if > 0 */
	}
	if (allow_dir_chown_in_ng_chown) {
		dir_flag = atoi(argv[6]);	/* make changes on the dir itself if > 0       */
	}
	if ((new_user == 0) && (new_group == 0)) {
		if (strlen(argv[7]) < 25) {
			new_user = (int)get_gluid(PASSWDFILE, argv[7]);
		}
		if (strlen(argv[8]) < 25) {
			new_group = (int)get_glgid(GROUPFILE, argv[8]);
		}
	}
	if ((strlen(argv[9]) < PATH_MAX) && (new_user < 65536) && (new_group < 65535)) {
		sprintf(my_path, "%s", argv[9]);
	} else {
		printf("%s - Error in argument(s)\n", argv[0]);
		return 1;
	}

	my_result = myscan(my_result, new_user, new_group, user_flag, group_flag, files_flag, dir_flag, my_path, argv[0]);
	if (my_result == 0) {
		if ((new_user > 0) || (new_group > 0)) {
			setuid(0);
			setgid(0);
			my_result = myscan(my_result, new_user, new_group, user_flag, group_flag, files_flag, dir_flag, my_path, argv[0]);
		}
	}
	return my_result;
}

