#include "../conf/zsconfig.h"
#include "video_info.h"
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
  unsigned long int file_size;
  struct audio_info audio;
  struct video_info video;
  struct program_info program;
  char temp1[7], temp2[15], temp3[6], stream_param_log[100],
       stream_param_file[120];
  FILE *fp;
  #ifdef video_info_verbose
    clock_t start_clock = clock();
  #endif

  if (((fp = fopen(argv[1], "r")) == NULL) || (argc != 2))
    exit(EXIT_FAILURE);

  fseek(fp, 0L, SEEK_END);
  file_size = ftell(fp);

  reset_structs(&program, &video, &audio);

  if(get_program_info(&program, fp) != 0) /* MPEG-1, MPEG-2 or XviD? */
    exit(EXIT_FAILURE);

  if(program.program_type == 1 || program.program_type == 2) { /* MPEG-1/2. */
    if(get_mpeg_video_info(program.program_type, &video, fp) != 0)
      exit(EXIT_FAILURE);
    if(video.video_duration > 0)
      program.program_avg_bit_rate = file_size / video.video_duration * 8;
    else
      program.program_avg_bit_rate = 0;
  }
  else if(program.program_type == 100) { /* XviD. */
    if(get_xvid_video_info(&video, fp) != 0)
      exit(EXIT_FAILURE);
    if(get_avi_info(fp, &program, file_size) != 0)
      exit(EXIT_FAILURE);
  }

  if(get_audio_info(program.program_type, &audio, fp) != 0)
    exit(EXIT_FAILURE);

  /******** Below here is just output things. ********/
  switch(program.program_type) {
    case 1:
    case 2:   sprintf(temp1, "MPEG-%d", program.program_type); break;
    case 100: sprintf(temp1, "AVI"); break;
  }
  switch(audio.audio_id) {
    case 1:   sprintf(temp2, "MPEG-1 layer %d", audio.audio_layer); break;
    case 0:   sprintf(temp2, "MPEG-2 layer %d", audio.audio_layer); break;
    case 100: sprintf(temp2, "AC-3"); break;
  }
  if(program.program_avg_bit_rate == 0)
    sprintf(temp3, "n/a");
  else
    sprintf(temp3, "%d", program.program_avg_bit_rate / 1000);

  sprintf(stream_param_log, "\"%s\" \"%s\" \"%d\" \"%s\" \"%d\" \"%d\" \"%s\""
    " \"%s\" \"%d\" \"%d\" \"%s\"", temp1, temp3, video.video_mpeg_type,
    video.video_frame_rate, video.video_hor_size,
    video.video_ver_size, video.video_aspect_ratio, temp2,
    audio.audio_bitrate, audio.audio_sampl_freq, audio.audio_mode);

  sprintf(stream_param_file, "[STREAM:%s@%skbps][ViDEO:MPEG-%d_%sfps_%dx%dpx_%s"
    "ar][AUDiO:%s@%dkbps_%dHz_%s]", temp1, temp3, video.video_mpeg_type,
    video.video_frame_rate, video.video_hor_size,
    video.video_ver_size, video.video_aspect_ratio, temp2,
    audio.audio_bitrate, audio.audio_sampl_freq, audio.audio_mode);

  #ifdef video_info_verbose
    print_verbose_info(&program, &video, &audio, start_clock);
  #endif

  #ifdef video_info_make_dir
    make_file(stream_param_file, argv[1]);
  #endif

  #ifdef video_info_make_file
    make_file(stream_param_file, argv[1]);
  #endif

  #ifdef video_info_write_log
    write_log(stream_param_log, argv[1]);
  #endif

  #ifdef video_info_write_message
    write_message_file(&program, &video, &audio, argv[1]);
  #endif

  exit(EXIT_SUCCESS);
}




/* get_start_code() searched for a 24 bit long byte aligned pattern in a file.
 * param 1: A pointer to the file in which the byte is to be found.
 * param 2: The 24 bit long pattern to search for.
 * param 3: Maximum number of bytes to search (has to be at least 4), not
 *          including heading zeroes in the file.
 * returns: The byte following the 2nd parameter or '0' if EOF.
 */
unsigned char get_start_code(FILE *fp, int pattern, unsigned int search_limit) {
  #define BUFFER_LENGTH 1000
  long int fp_start_pos;
  unsigned int bytes_read, i = 0, j;
  int k = -4;
  unsigned char buffer[BUFFER_LENGTH];

  fp_start_pos = ftell(fp);
  bytes_read = fread(buffer, 1, BUFFER_LENGTH, fp);

  /* Skip heading zeroes (in multiples of 4) at the start of the file. */
  if((unsigned int) ftell(fp) == bytes_read) {
    while(!buffer[i] && !buffer[i+1] && !buffer[i+2] && !buffer[i+3]) {
      i += 4;
      if(i > (bytes_read - 4))
        return(0);
    }
  }

  if(search_limit < bytes_read)
    bytes_read = search_limit;

  /* This loop, if nescessary, reads more data from fp to the buffer. */
  for(j = 0; j <= (search_limit / BUFFER_LENGTH); j++) {
    /* This loop goes through the read buffer. */
    for(i = 0; i < (bytes_read - 3); i++) {
      if(buffer[i] == (pattern >> 16 & 0xFF) && buffer[i+1] == (pattern >> 8 & 0xFF) && buffer[i+2] == (pattern & 0xFF)) {
        fseek(fp, fp_start_pos + j * BUFFER_LENGTH + i - k, SEEK_SET);
        return(buffer[i+3]);
      }
    }
    if(feof(fp))
      return(0); /* Couldn't find the pattern. */
    fseek(fp, -3L, SEEK_CUR);
    search_limit -= 3;
    k += 3;
    bytes_read = fread(buffer, 1, BUFFER_LENGTH, fp);
  }
  fseek(fp, fp_start_pos, SEEK_SET);

  return(0); /* Couldn't find the pattern. */
}




/* get_program_info() searches for known bit patterns to identify the type of
 * file. Types it can identify are AVI files containing an XviD video stream,
 * and any files containing an MPEG-1/2 program stream.
 * param 1: A pointer to the struct in which we will add the properties found.
 * param 2: A pointer to the file in which the byte is to be found.
 * returns: '0' if succesful, '-1' otherwise.
 */
int get_program_info(struct program_info *program, FILE *fp) {
  rewind(fp);
  if(get_start_code(fp, 0x766964, 112) == 0x73) { /* "vids", indicates AVI stream. */
    if(((getc(fp) << 24) | (getc(fp) << 16) | (getc(fp) << 8) | getc(fp)) != 0x78766964) /* ! "xvid" */
      return(-1);
    program->program_type = 100;
    sprintf(program->program_bit_rate_type, "n/a");
    return(0);
  }
  else { /* MPEG-1/2 program stream or a .VOB file. */
    rewind(fp);
    while(1) {
      if(ftell(fp) > 1000000)
        return(-1);
      else if(get_start_code(fp, 0x000001, 1000000) == 0xBA) /* pack_start_code */
        break;
    }
    data = getc(fp);
    if((data & 0xF0) == 0x20)
      program->program_type = 1;
    else if((data & 0xC0) == 0x40)
      program->program_type = 2;
    else
      return(-1);
    ungetc(data, fp);
    while(1) {
      if(ftell(fp) > 1000000)
        return(-1);
      else if(get_start_code(fp, 0x000001, 1000000) == 0xBB) /* system_header_start_code. */
        break;
    }
    SKIP_BYTES(5);
    if(getc(fp) & 0x02)
      sprintf(program->program_bit_rate_type, "cbr");
    else
      sprintf(program->program_bit_rate_type, "vbr");

    return(0);
  }
}



/* reset_structs() initializes the structs that hold the gathered information.
 * I'ts here only as some kind of precaution.
 * param 1: A pointer to the struct that holds the program properties.
 * param 2: A pointer to the struct that holds the video properties.
 * param 3: A pointer to the struct that holds the sudio properties.
 * returns: void.
 */
void reset_structs(struct program_info *program, struct video_info *video, struct audio_info *audio) {
  program->program_type = 0;
  sprintf(program->program_bit_rate_type, "xxx");
  program->program_avg_bit_rate = 0;
  sprintf(video->video_profile, "xxxxxxxxxxxxxxxxxx");
  sprintf(video->video_level, "xxxxxxxxx");
  sprintf(video->video_aspect_ratio, "xxxx");
  video->video_bit_rate = 0;
  video->video_ver_size = 0;
  video->video_hor_size = 0;
  video->video_mpeg_type = 0;
  sprintf(video->video_frame_rate, "xxxxxx");
  sprintf(video->video_chroma_format, "xxxxxx");
  video->video_duration = 0;
  audio->audio_id = 0;
  audio->audio_layer = 0;
  audio->audio_protection_bit = 0;
  audio->audio_bitrate = 0;
  audio->audio_sampl_freq = 0;
  sprintf(audio->audio_mode, "xxxxxxxxxxxxxx");
  audio->audio_mode_extension = 0;
  audio->audio_copyright = 0;
  audio->audio_original = 0;
  audio->audio_emphasis = 0;

  return;
}




int get_base_dir(char *file_name) {
  int i = strlen(file_name);

  while(file_name[i] != '/') /* Strip the file name. */
    i--;
  i--;
  while(file_name[i] != '/') /* Strip the sample dir name. */
    i--;

  if(i < 0)
    return(-1);

  return(i);
}




int make_file(char *stream_param, char *file_name) {
  int i, l = 0;
  unsigned int j;
  char name[500];

  if((i = get_base_dir(file_name)) < 0)
    return(-1);

  /* Remove any chars containing '"' or '/'. */
  for(j = 0; j < strlen(stream_param); j++) {
    if((stream_param[j+l] == '"') || (stream_param[j+l] == '/'))
      l++;
    stream_param[j] = stream_param[j+l];
  }

  if((unsigned int) i > (497 - strlen(stream_param)))
    return(-1);

  strncpy(name, file_name, i+1);
  strncpy(name+i+1, stream_param, strlen(stream_param));
  name[i+1+strlen(stream_param)] = '\0';

  #ifdef video_info_make_dir
    mkdir(name, 0700);
  #endif

  #ifdef video_info_make_file
    creat(name, 0600);
  #endif

  return(0);
}




int write_log(char *stream_param, char *file_name) {
  FILE *log_file;
  time_t cur_time;
  int i;
  char path[400];

  if((log_file = fopen(LOG_FILE, "a+")) == NULL)
    return(-1);

  if((i = get_base_dir(file_name)) < 1)
    return(-1);

  if(i > 398)
    return(-1);
  strncpy(path, file_name, i);
  path[i] = '\0';

  cur_time = time(NULL);
  fprintf(log_file, "%.24s VIDEO_INFO: \"%s\" %s\n", ctime(&cur_time), path, stream_param);
  fclose(log_file);

  return(0);
}




int write_message_file(struct program_info *program, struct video_info *video, struct audio_info *audio, char *file_name) {
  FILE *message_file;
  int i;
  char temp1[7], temp2[15], temp3[6];
  char pad_string[video_info_padding];
  char file[400];

  if((i = get_base_dir(file_name)) < 0)
    return(-1);

  if(i > 389)
    return(-1);
  strncpy(file, file_name, i+1);
  strncpy(file+i+1, ".message", strlen(".message"));
  file[i+1+strlen(".message")] = '\0';

  if((message_file = fopen(file, "a+")) == NULL)
    return(-1);

  switch(program->program_type) {
    case 1:   sprintf(temp1, "MPEG-1"); break;
    case 2:   sprintf(temp1, "MPEG-2"); break;
    case 100: sprintf(temp1, "AVI"); break;
  }
  switch(audio->audio_id) {
    case 1:   sprintf(temp2, "MPEG-1 layer %d", audio->audio_layer); break;
    case 0:   sprintf(temp2, "MPEG-2 layer %d", audio->audio_layer); break;
    case 100: sprintf(temp2, "AC-3"); break;
  }
  if(program->program_avg_bit_rate == 0)
    sprintf(temp3, "n/a");
  else
    sprintf(temp3, "%d", program->program_avg_bit_rate / 1000);

  write_message_header(" video_info: ", message_file);
  fprintf(message_file, "| Program stream type:       %s%s|\n", temp1, pad_str(temp1, 0, pad_string));
  fprintf(message_file, "| Program average bit rate:  %s kbps%s|\n", temp3, pad_str(temp3, 5, pad_string));
  fprintf(message_file, "| Video stream type:         MPEG-%d%s|\n", video->video_mpeg_type, pad_int(video->video_mpeg_type, 5, pad_string));
  fprintf(message_file, "| Video frame rate:          %s fps%s|\n", video->video_frame_rate, pad_str(video->video_frame_rate, 4, pad_string));
  fprintf(message_file, "| Video width:               %d pixels%s|\n", video->video_hor_size, pad_int(video->video_hor_size, 7, pad_string));
  fprintf(message_file, "| Video height:              %d pixels%s|\n", video->video_ver_size, pad_int(video->video_ver_size, 7, pad_string));
  fprintf(message_file, "| Video aspect ratio:        %s%s|\n", video->video_aspect_ratio, pad_str(video->video_aspect_ratio, 0, pad_string));
  fprintf(message_file, "| Audio stream type:         %s%s|\n", temp2, pad_str(temp2, 0, pad_string));
  fprintf(message_file, "| Audio bit rate:            %d kbps%s|\n", audio->audio_bitrate, pad_int(audio->audio_bitrate, 5, pad_string));
  fprintf(message_file, "| Audio sampling frequency:  %d Hz%s|\n", audio->audio_sampl_freq, pad_int(audio->audio_sampl_freq, 3, pad_string));
  fprintf(message_file, "| Audio mode:                %s%s|\n", audio->audio_mode, pad_str(audio->audio_mode, 0, pad_string));
  write_message_footer(" generated by video_info v0.2beta ", message_file);

  fclose(message_file);

  return(0);
}




void write_message_header(char *title, FILE *message_file) {
  unsigned int i;

  fprintf(message_file, "+");
  for(i = 0; i < (28 + video_info_padding - strlen(title)) / 2; i++)
    fprintf(message_file, "-");
  fprintf(message_file, title);
  for(i = 0; i < (28 + video_info_padding - strlen(title)) / 2; i++)
    fprintf(message_file, "-");
  if((28 + video_info_padding - strlen(title)) % 2)
    fprintf(message_file, "-");
  fprintf(message_file, "+\n");

  return;
}




void write_message_footer(char *title, FILE *message_file) {
  unsigned int i;

  fprintf(message_file, "+");
  for(i = 0; i < (27 + video_info_padding - strlen(title)); i++)
    fprintf(message_file, "-");
  fprintf(message_file, title);
  fprintf(message_file, "-+\n");

  return;
}




char *pad_int(int in, int extra, char *pad_string) {
  char temp[video_info_padding];
  int i, length;

  if((length = sprintf(temp, "%d", in) + extra) > video_info_padding)
    length = 1;

  for(i = 0; i < video_info_padding - length; i++)
    temp[i] = ' ';
  temp[i] = '\0';

  sprintf(pad_string, "%s", temp);

  return(pad_string);
}




char *pad_str(char *in, int extra, char *pad_string) {
  char temp[video_info_padding];
  int i, length;

  if((length = sprintf(temp, "%s", in) + extra) > video_info_padding)
    length = 1;

  for(i = 0; i < video_info_padding - length; i++)
    temp[i] = ' ';
  temp[i] = '\0';

  sprintf(pad_string, "%s", temp);

  return(pad_string);
}




int print_verbose_info(struct program_info *program, struct video_info *video, struct audio_info *audio, clock_t start_clock) {
  printf("CPU time used:         %d ms\n", (int)(1000 * (clock() - start_clock) / (double)CLOCKS_PER_SEC));
  printf("program_type:          %d\n", program->program_type);
  printf("program_bit_rate_type: %s\n", program->program_bit_rate_type);
  printf("program_avg_bit_rate:  %d\n", program->program_avg_bit_rate / 1000);
  printf("video_profile:         %s\n", video->video_profile);
  printf("video_level:           %s\n", video->video_level);
  printf("video_aspect_ratio:    %s\n", video->video_aspect_ratio);
  printf("video_bit_rate:        %d\n", video->video_bit_rate);
  printf("video_ver_size:        %d\n", video->video_ver_size);
  printf("video_hor_size:        %d\n", video->video_hor_size);
  printf("video_mpeg_type:       %d\n", video->video_mpeg_type);
  printf("video_frame_rate:      %s\n", video->video_frame_rate);
  printf("video_chroma_format:   %s\n", video->video_chroma_format);
  printf("video_duration:        %d (%dmin %dsec)\n", video->video_duration, video->video_duration / 60, video->video_duration - (video->video_duration / 60) * 60);
  printf("audio_id:              %d\n", audio->audio_id);
  printf("audio_layer:           %d\n", audio->audio_layer);
  printf("audio_protection_bit:  %d\n", audio->audio_protection_bit);
  printf("audio_bitrate:         %d\n", audio->audio_bitrate);
  printf("audio_sampling_freq.:  %d\n", audio->audio_sampl_freq);
  printf("audio_mode:            %s\n", audio->audio_mode);
  printf("audio_mode_extension:  %d\n", audio->audio_mode_extension);
  printf("audio_copyright:       %d\n", audio->audio_copyright);
  printf("audio_original:        %d\n", audio->audio_original);
  printf("audio_emphasis:        %d\n", audio->audio_emphasis);

  return(0);
}

