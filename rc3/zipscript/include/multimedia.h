#ifndef _MULTIMEDIA_H_
#define _MULTIMEDIA_H_

#define WHITESPACE_STR  " \f\n\r\t\v"

char *trim(char *);
void mpeg_video(char *, struct video *);
void avi_video(char *, struct video *);
char *get_preset(char[4]);
void get_mpeg_audio_info(char *, struct audio *);

#endif
