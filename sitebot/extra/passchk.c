#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>
#include <stdlib.h>

extern char *crypt(const char *key, const char *salt);

#ifndef  __USE_SVID
struct passwd pwd;
struct passwd* fgetpwent(FILE *fp) {
 char *data[10],
       tmp;
 int   charcnt[10],
       fieldcnt = 0,
       varsize = 0;

 charcnt[0] = 0;

 while ( fread(&tmp, 1, 1, fp) > 0 ) {
  charcnt[fieldcnt]++;

  if ( varsize < charcnt[fieldcnt] ) {
   varsize += 20;
   if ( varsize == 20 ) data[fieldcnt] = malloc(varsize);
                   else data[fieldcnt] = realloc(data[fieldcnt], varsize);
  }

  if ( tmp == '\n' ) {
   data[fieldcnt][charcnt[fieldcnt] - 1] = 0;
   break;
  } else if ( tmp == ':' ) {
   data[fieldcnt][charcnt[fieldcnt] - 1] = 0;
   fieldcnt++;
   charcnt[fieldcnt] = varsize = 0;
  } else {
   data[fieldcnt][charcnt[fieldcnt] - 1] = tmp;
  }
 }

 if ( fieldcnt == 6 ) {
  pwd.pw_name = malloc(charcnt[0]);
  pwd.pw_passwd = malloc(charcnt[1]);
  strcpy(pwd.pw_name, data[0]);
  strcpy(pwd.pw_passwd, data[1]);
 } else return NULL;

 return &pwd;
}
#endif

int main(int argc, char *argv[]) {
	FILE *fp;
	struct passwd *buf;
	char *crypted;
	char salt[2];
	if (argc != 4) {
		printf("Usage: %s <user> <pass> <passwdfile>\n", argv[0]);
		return 1;
	}
	if ((fp=fopen(argv[3], "r")) == NULL) {
		printf("Ooops, couldn\'t open your passwd file.\n");
		printf("Looks like you didnt specify a correct path.\n");
		return 1;
	}
	while ((buf=fgetpwent(fp)) != NULL) {
		if (strcmp(buf->pw_name, argv[1]))
			continue;
		strncpy(salt, buf->pw_passwd, 2);
		if ((crypted=(char *)malloc(64)) == NULL) {
			fprintf(stderr, "Couldn't malloc!\n");
			return 1;
		}
		crypted=crypt(argv[2], salt);
		if (strcmp(buf->pw_passwd, crypted) == 0) {
			printf("MATCH\n");
			return 0;
		} else {
			printf("NOMATCH\n");
			return 0;
		}
	}
	printf("No such user in passwd file!\n");
	return 0;	
}
	
