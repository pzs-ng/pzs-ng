#ifndef _CONVERT_H_
#define _CONVERT_H_

char *hms(char *, int);
char *convert(struct VARS *, struct USERINFO **, struct GROUPINFO **, char *);
char *convert2(struct VARS *, struct USERINFO *, struct GROUPINFO **, char *, short);
char *convert3(struct VARS *, struct GROUPINFO *, char *, short);
char *convert4(struct VARS *, char *);
char *convert5(char *);
char *c_incomplete(char *, char [2][PATH_MAX], struct VARS *);
char *i_incomplete(char *, char [2][PATH_MAX], struct VARS *);
char *s_incomplete(char *, char [2][PATH_MAX], struct VARS *);

#endif
