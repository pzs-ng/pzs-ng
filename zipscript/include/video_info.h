#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


/* Specify which file that WRITE_LOG should use. */
#define LOG_FILE log


/**** No more to edit below here. ****/
#define SKIP_BYTE getc(fp)
#define SKIP_BYTES(n) for(k = 0; k < n; k++) getc(fp)

int k; /* Used for the SKIP_BYTES() macro */
unsigned char data;

struct program_info {
  int program_type;             /* 1 = MPEG-1, 2 = MPEG-2, 100-105 = AVI */
  char program_bit_rate_type[4];/* CBR/VBR, only for MPEG-1/2. */
  int program_avg_bit_rate;     /* In kb/s. */
};

struct video_info {
  char video_profile[19];       /* Only for MPEG-1/2. */
  char video_level[10];         /* Only for MPEG-1/2. */
  char video_aspect_ratio[5];   /* MPEG-4 limited to aspect ratio of 1. */
  int video_bit_rate;           /* Only for MPEG-1/2. This only indicates the
                                   max rate of operation of the vbv. In b/s. */
  int video_ver_size;           /* In pixels. */
  int video_hor_size;           /* In pixels. */
  int video_mpeg_type;          /* 1 = MPEG-1, 2 = MPEG-2, 4 = MPEG-4. */
  char video_frame_rate[7];     /* In Hz. */
  char video_chroma_format[7];  /* MPEG-4 limited to chroma 4:2:0. */
  int video_duration;           /* Only for MPEG-1/2. In seconds. */
};

struct audio_info {
  int audio_id;                 /* 1 = MPEG-1, 2 = MPEG-2, 100 = AC-3. */
  int audio_layer;              /* Only for MPEG audio. */
  int audio_protection_bit;     /* Only for MPEG audio. */
  int audio_bitrate;            /* In kb/s. */
  int audio_sampl_freq; /* In Hz. */
  char audio_mode[15];          /* Number of channels (sort of). */
  int audio_mode_extension;     /* Only for MPEG audio joint_stereo mode. */
  int audio_copyright;          /* Only for MPEG audio. */
  int audio_original;           /* Only for MPEG audio. */
  int audio_emphasis;           /* Only for MPEG audio. */
};

unsigned char get_start_code(FILE *, int, unsigned int);
int get_audio_info(int, struct audio_info *, FILE *);
int get_ac3_info(struct audio_info *, FILE *);
int get_mpeg_video_info(int, struct video_info *, FILE *);
int get_xvid_video_info(struct video_info *, FILE *);
int get_program_info(struct program_info *, FILE *);
int get_avi_info(FILE*, struct program_info *, unsigned long int);
void reset_structs(struct program_info *, struct video_info *, struct audio_info *);
int print_verbose_info(struct program_info *, struct video_info *, struct audio_info *, clock_t);
int make_file(char *, char *);
int write_log(char *, char *);
int write_message_file(struct program_info *, struct video_info *, struct audio_info *, char *);
void write_message_header(char *, FILE *);
void write_message_footer(char *, FILE *);
int get_base_dir(char *);
char *pad_int(int, int, char *);
char *pad_str(char *, int, char *);

