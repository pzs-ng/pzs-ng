/*
 * taken from project-zs-ng, thx ripped back from glftp2 rc3 :)
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>
#include <stdlib.h>

/*#include <paths.h>*/
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>

extern char    *crypt(const char *key, const char *salt);

struct passwd *fgetpwent(FILE *);
int pbkdf2(const unsigned char *, unsigned int, unsigned int, unsigned char *, unsigned long long);
int pw_encrypt(const unsigned char *, char *);
int pw_encrypt_new(const unsigned char *, unsigned char *, char *);

#define SHA_SALT_LEN 4

#ifndef  __USE_SVID
struct passwd	pwd;

struct passwd  *
fgetpwent(FILE * fp)
{
	char           *data[10], tmp;
	int		charcnt    [10], fieldcnt = 0, varsize = 0;

	charcnt[0] = 0;

	while (fread(&tmp, 1, 1, fp) > 0) {
		charcnt[fieldcnt]++;

		if (varsize < charcnt[fieldcnt]) {
			varsize += 20;
			if (varsize == 20)
				data[fieldcnt] = malloc(varsize);
			else
				data[fieldcnt] = realloc(data[fieldcnt], varsize);
		}
		if (tmp == '\n') {
			data[fieldcnt][charcnt[fieldcnt] - 1] = 0;
			break;
		} else if (tmp == ':') {
			data[fieldcnt][charcnt[fieldcnt] - 1] = 0;
			fieldcnt++;
			charcnt[fieldcnt] = varsize = 0;
		} else {
			data[fieldcnt][charcnt[fieldcnt] - 1] = tmp;
		}
	}

	if (fieldcnt == 6) {
		pwd.pw_name = malloc(charcnt[0]);
		pwd.pw_passwd = malloc(charcnt[1]);
		pwd.pw_gecos = malloc(charcnt[4]);
		strcpy(pwd.pw_name, data[0]);
		strcpy(pwd.pw_passwd, data[1]);
		strcpy(pwd.pw_gecos, data[4]);
	} else
		return NULL;

	return &pwd;
}
#endif

/* see http://lists.jammed.com/secprog/2002/11/0008.html for info */
#define HLEN (20)		/* Using SHA-1 */

int
pbkdf2(const unsigned char *pw, unsigned int pwlen,
       unsigned int ic, unsigned char *dk, unsigned long long dklen)
{
	unsigned long	l, r, i, j;
	unsigned char	txt[4], hash[HLEN * 2], tmp[HLEN], *p =
	dk,            *lhix, *hix, *swap;
	unsigned short	k;
	unsigned int	outlen;

	if (dklen > ((((unsigned long long)1) << 32) - 1) * HLEN) {
		/* TODO: Call an error handler. */
		return 1;
	}
	l = dklen / HLEN;
	r = dklen % HLEN;

	for (i = 1; i <= l; i++) {
		sprintf((char *)txt, "%04u", (unsigned int)i);
		HMAC(EVP_sha1(), pw, pwlen, txt, 4, hash, &outlen);
		lhix = hash;
		hix = hash + HLEN;
		for (k = 0; k < HLEN; k++) {
			tmp[k] = hash[k];
		}
		for (j = 1; j < ic; j++) {
			HMAC(EVP_sha1(), pw, pwlen, lhix, HLEN, hix, &outlen);
			for (k = 0; k < HLEN; k++) {
				tmp[k] ^= hix[k];
			}
			swap = hix;
			hix = lhix;
			lhix = swap;
		}
		for (k = 0; k < HLEN; k++) {
			*p++ = tmp[k];
		}
	}
	if (r) {
		sprintf((char *)txt, "%04u", (unsigned int)i);
		HMAC(EVP_sha1(), pw, pwlen, txt, 4, hash, &outlen);
		lhix = hash;
		hix = hash + HLEN;
		for (k = 0; k < HLEN; k++) {
			tmp[k] = hash[k];
		}
		for (j = 1; j < ic; j++) {
			HMAC(EVP_sha1(), pw, pwlen, lhix, HLEN, hix, &outlen);
			for (k = 0; k < HLEN; k++) {
				tmp[k] ^= hix[k];
			}
			swap = hix;
			hix = lhix;
			lhix = swap;
		}
		for (k = 0; k < r; k++) {
			*p++ = tmp[k];
		}
	}
	return 0;
}

int
pw_encrypt(const unsigned char *pw_pwd, char *digest)
{
	unsigned char	md[SHA_DIGEST_LENGTH];
	int		mdlen = SHA_DIGEST_LENGTH, i;

	/* see pbe.c for info, 100x multihash */
	pbkdf2(pw_pwd, strlen((char *)pw_pwd), 100, md, SHA_DIGEST_LENGTH);

	for (i = 0; i < mdlen; i++) {
		sprintf(digest, "%02x", md[i]);
		digest += 2;
	}
	/* fix the last /0 !!! */
	*digest = '\0';
	return 1;
}

int
pw_encrypt_new(const unsigned char *pw_pwd, unsigned char *encryp, char *digest)
{
	unsigned char	hexconvert[3];
	unsigned char  *salt;
	int		i;
	unsigned char	md[SHA_DIGEST_LENGTH];
	int		mdlen = SHA_DIGEST_LENGTH;

	unsigned char	real_salt[SHA_SALT_LEN + 1];

	bzero(hexconvert, sizeof(hexconvert));

	salt = encryp;
	salt++;
	for (i = 0; i < SHA_SALT_LEN; i++) {
		hexconvert[0] = (*salt);
		salt++;
		hexconvert[1] = (*salt);
		salt++;
		real_salt[i] = strtol(hexconvert, NULL, 16);
	}

	PKCS5_PBKDF2_HMAC_SHA1(pw_pwd, strlen(pw_pwd), real_salt, SHA_SALT_LEN, 100,
			       mdlen, md);

	*digest = '$';
	digest++;
	for (i = 0; i < SHA_SALT_LEN; i++) {
		sprintf(digest, "%02x", real_salt[i]);
		digest += 2;
	}
	*digest = '$';
	digest++;
	for (i = 0; i < mdlen; i++) {
		sprintf(digest, "%02x", md[i]);
		digest += 2;
	}
	/* fix the last /0 !!! */
	*digest = '\0';

	return 0;
}

int
main(int argc, char *argv[])
{
	FILE           *fp;
	struct passwd  *buf;

	char           *crypted;
	char		salt      [2];

	if (argc != 4) {
		printf("Usage: %s <user> <pass> <passwdfile>\n", argv[0]);
		return 1;
	}
	if ((fp = fopen(argv[3], "r")) == NULL) {
		printf("Ooops, couldn\'t open your passwd file.\n");
		printf("Looks like you didnt specify a correct path.\n");
		return 1;
	}
	while ((buf = fgetpwent(fp)) != NULL) {
		if (strcmp(buf->pw_name, argv[1]))
			continue;

		if (strlen(buf->pw_passwd) == 13) {
			strncpy(salt, buf->pw_passwd, 2);
			crypted = crypt(argv[2], salt);
		} else if (strlen(buf->pw_passwd) == SHA_DIGEST_LENGTH * 2) {
			crypted = malloc(SHA_DIGEST_LENGTH * 2 + 1);
			if (!crypted) {
				printf
					("Ooops, couldn't allocate %d bytes of memory for hash.\n",
					 (SHA_DIGEST_LENGTH * 2 + 1));
				return 1;
			}
			pw_encrypt((unsigned char *)argv[2], crypted);
		} else if (strlen(buf->pw_passwd) == 50) {
			crypted = malloc(51);
			if (!crypted) {
				printf
					("Ooops, couldn't allocate %d bytes of memory for hash.\n",
					 (SHA_DIGEST_LENGTH * 2 + 1));
				return 1;
			}
			pw_encrypt_new((unsigned char *)argv[2], buf->pw_passwd,
				       crypted);
		} else {
			printf
				("Ooops, password is of invalid length! (not gl1 nor gl2).\n");
			return 1;
		}

		/* printf("%s %s\n",buf->pw_passwd, crypted); */

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
