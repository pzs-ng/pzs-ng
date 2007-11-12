#ifndef _CONVERT_H_
#define _CONVERT_H_

char *hms(char *, int);
char *convert(struct VARS *, struct USERINFO **, struct GROUPINFO **, char *);
char *convert_user(struct VARS *, struct USERINFO *, struct GROUPINFO **, char *, short);
char *convert_group(struct VARS *, struct GROUPINFO *, char *, short);
char *convert_audio(struct VARS *, char *);
char *convert_sitename(char *);
char *incomplete(char *, char [2][PATH_MAX], struct VARS *, int);

#endif
