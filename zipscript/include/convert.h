#ifndef _CONVERT_H_
#define _CONVERT_H_

//char* convert_obsolete(struct VARS *, char *); -- obsolete? :)
char* convert(struct VARS *, struct USERINFO **, struct GROUPINFO **, char *);
char* convert2(struct VARS *, struct USERINFO *, struct GROUPINFO **, char *, short);
char* convert3(struct VARS *, struct GROUPINFO *, char *, short);
char* convert4(struct VARS *, char *);
char* c_incomplete(char *, char **);

#endif

