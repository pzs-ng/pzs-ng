#ifndef _MULTIMEDIA_H_
#define _MULTIMEDIA_H_

char *trim (char *);
void mpeg_video(char *, struct video *);
void avi_video(char *, struct video *);
/*char* get_preset(char [4]); -- this is mostly for internal use, afaict*/
void get_mpeg_audio_info(char *, struct audio *);

#endif

