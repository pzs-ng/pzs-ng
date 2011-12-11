#include "avi.h"

#ifndef _MULTIMEDIA_H_
#define _MULTIMEDIA_H_

#define WHITESPACE_STR  " \f\n\r\t\v"

char *trim(char *);
//void mpeg_video(char *, struct video *);
//void avi_video(char *, struct video *);
char *get_preset(char[4]);
char *safe_genre(char *);

void get_audio_info(char *, struct audio *);
void get_mpeg_audio_info(char *, struct audio *);
#ifdef HAVE_FLAC_HEADERS
void get_flac_audio_info(char *, struct audio *);
#endif

const unsigned char *fourcc(FOURCC);
void avierror(const char *);
DWORD get32(FILE *);
WORD get16(FILE *);
int avinfo(char *, struct VIDEO *);

#endif
