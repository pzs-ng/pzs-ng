#include "video_info.h"

int get_mpeg_video_info(int program_type, struct video_info *video, FILE *fp) {
  long int file_size;
  unsigned int temp2 = 0, horizontal_size_value, vertical_size_value,
               bit_rate_value, bit_rate_extension;
  unsigned char temp1, aspect_ratio_information = 0, frame_rate_code = 0,
                extension_start_code_identifier, profile_and_level_indication = 0,
                progressive_sequence, chroma_format = 0, hor_size_ext,
                ver_size_ext, drop_frame_flag;

  rewind(fp);
  /* MPEG-1 or MPEG-2 video? */
  while(!feof(fp))
    if(get_start_code(fp, 0x000001, 50000000) == 0xB3)
      break;
  if(feof(fp))
    return(-1);
  video->video_mpeg_type = 1;
  if(get_start_code(fp, 0x000001, 140) == 0xB5)
    video->video_mpeg_type = 2;

  /* Get the length of the video stream. Since some "mpeg-cutters" doesn't seem
   * to update all the time stamps in the file this code snippet subtracts the
   * first time stamps from the last.
   */
  fseek(fp, 0L, SEEK_END);
  file_size = ftell(fp);
  rewind(fp);

  while(ftell(fp) < 1000000) {
    if(get_start_code(fp, 0x000001, 10000000) == 0xB8) { /* group_start_code */
      data = getc(fp);
      /* This is an ugly fix for (to me) weird time stamps. */
      /*if(!(data & 0x80) && (((data & 0x7C) >> 2) < 4)) {*/
      if(((data & 0x7C) >> 2) < 4) {
        drop_frame_flag = (data & 0x80);
        video->video_duration = ((data & 0x7C) >> 2) * 3600;
        temp1 = (data & 0x03) << 4;
        data = getc(fp);
        video->video_duration += ((temp1 | (data >> 4)) * 60);
        temp1 = (data & 0x07) << 3;
        data = getc(fp);
        video->video_duration += (temp1 | (data >> 5));
        break;
      }
    }
  }
  if(file_size > 1000000) /* This is an elaborate value. */
    fseek(fp, -1000000L, SEEK_END);
  else
    rewind(fp);

  while(!feof(fp)) {
    if(get_start_code(fp, 0x000001, 50000000) == 0xB8) { /* group_start_code */
      data = getc(fp);
      /* This is an ugly fix for (to me) weird time stamps. */
      /*if(!(data & 0x80) && (((data & 0x7C) >> 2) < 4)) {*/
      if(((data & 0x7C) >> 2) < 4) {
        drop_frame_flag = (data & 0x80);
        temp2 = ((data & 0x7C) >> 2) * 3600;
        temp1 = (data & 0x03) << 4;
        data = getc(fp);
        temp2 += ((temp1 | (data >> 4)) * 60);
        temp1 = (data & 0x07) << 3;
        data = getc(fp);
        temp2 += (temp1 | (data >> 5));
      }
    }
  }
  if(!temp2) /* couldn't find two valid time stamps. */
    video->video_duration = 0;
  else
    video->video_duration = temp2 - video->video_duration;

  rewind(fp);
  while(1) {
    if(ftell(fp) > 1000000 || feof(fp))
      return(-1);
    if(get_start_code(fp, 0x000001, 500000) == 0xB3) /* sequence_header_code */
      break;
  }

  if(program_type == 1) {
    data = getc(fp);
    horizontal_size_value = data << 4;
    data = getc(fp);
    horizontal_size_value = horizontal_size_value | (data >> 4);
    vertical_size_value = (data & 0x0F) << 8;
    data = getc(fp);
    vertical_size_value = vertical_size_value | data;
    data = getc(fp);
    aspect_ratio_information = data >> 4;
    frame_rate_code = data & 0x0F;
    video->video_bit_rate = (getc(fp) << 10) | (getc(fp) << 2) | (getc(fp) >> 6);
    if(video->video_bit_rate == 0x3FFFF) /* intended for vbr operation. */
      video->video_bit_rate = 0;
    else
      video->video_bit_rate = video->video_bit_rate * 400;
    video->video_hor_size = horizontal_size_value;
    video->video_ver_size = vertical_size_value;
    chroma_format = 0x01;
    profile_and_level_indication = 0x00;
  }
  else if(program_type == 2) {
    data = getc(fp);
    horizontal_size_value = data << 4;
    data = getc(fp);
    horizontal_size_value = horizontal_size_value | (data >> 4);
    vertical_size_value = (data & 0x0F) << 8;
    data = getc(fp);
    vertical_size_value = vertical_size_value | data;
    data = getc(fp);
    aspect_ratio_information = data >> 4;
    frame_rate_code = data & 0x0F;
    bit_rate_value = (getc(fp) << 10) | (getc(fp) << 2) | (getc(fp) & 0xC0);

    /* Not much more useful information to read here so lets go on... */
    if(get_start_code(fp, 0x000001, 50000000) != 0xB5) /*extension_start_code*/
      return(-1);
    data = getc(fp);
    extension_start_code_identifier = data >> 4;
    profile_and_level_indication = data << 4;
    data = getc(fp);
    profile_and_level_indication = profile_and_level_indication | (data >> 4);
    progressive_sequence = (data & 0x08) >> 3;
    chroma_format = (data & 0x06) >> 1;
    hor_size_ext = (data & 0x01) << 1;
    data = getc(fp);
    hor_size_ext = hor_size_ext | ((data & 0x80) >> 7);
    ver_size_ext = (data & 0x60) >> 5;
    video->video_hor_size = horizontal_size_value | (hor_size_ext << 12);
    video->video_ver_size = vertical_size_value | (ver_size_ext << 12);
    bit_rate_extension = (data & 0x1F) << 7;
    data = getc(fp);
    bit_rate_extension = bit_rate_extension | ((data & 0xFE) >> 1);
    video->video_bit_rate = (bit_rate_value | (bit_rate_extension << 18)) * 400;
  /*  getc(fp);
   *  data = getc(fp);
   *  frame_rate_extension_n = (data & 0x60) >> 5;
   *  frame_rate_extension_d = data & 0x1F;
   */
  }
  switch((profile_and_level_indication & 0x70) >> 4) {
    case 0x01: sprintf(video->video_profile, "high"); break;
    case 0x02: sprintf(video->video_profile, "spatially_scalable"); break;
    case 0x03: sprintf(video->video_profile, "snr_scalable"); break;
    case 0x04: sprintf(video->video_profile, "main"); break;
    case 0x05: sprintf(video->video_profile, "simple"); break;
    default:   sprintf(video->video_profile, "n/a"); break;
  }
  switch(profile_and_level_indication & 0x0F) {
    case 0x04: sprintf(video->video_level, "high"); break;
    case 0x06: sprintf(video->video_level, "high_1440"); break;
    case 0x08: sprintf(video->video_level, "main"); break;
    case 0x0A: sprintf(video->video_level, "low"); break;
    default:   sprintf(video->video_level, "n/a"); break;
  }

  switch(aspect_ratio_information) {
    case 0x01: sprintf(video->video_aspect_ratio, "%.2f",
               (float)video->video_ver_size /
               video->video_hor_size); break;
    case 0x02: if(video->video_mpeg_type == 1) {
                 sprintf(video->video_aspect_ratio, "%.2f", 1/atof("0.6735"));
                 break;
               }
               else if(video->video_mpeg_type == 2) {
                 sprintf(video->video_aspect_ratio, "%.2f", 1/atof("0.75"));
                 break;
               }
    case 0x03: if(video->video_mpeg_type == 1){
                 sprintf(video->video_aspect_ratio, "%.2f", 1/atof("0.7031"));
                 break;
               }
               else if(video->video_mpeg_type == 2) {
                 sprintf(video->video_aspect_ratio, "%.2f", 1/atof("0.5625"));
                 break;
               }
    case 0x04: if(video->video_mpeg_type == 1) {
                 sprintf(video->video_aspect_ratio, "%.2f", 1/atof("0.7615"));
                 break;
               }
               else if(video->video_mpeg_type == 2) {
                 sprintf(video->video_aspect_ratio, "%.2f", 1/atof("0.4524"));
                 break;
               }
    case 0x05: sprintf(video->video_aspect_ratio, "%.2f", 1/atof("0.8055"));
      break;
    case 0x06: sprintf(video->video_aspect_ratio, "%.2f", 1/atof("0.8437"));
      break;
    case 0x07: sprintf(video->video_aspect_ratio, "%.2f", 1/atof("0.8935"));
      break;
    case 0x08: sprintf(video->video_aspect_ratio, "%.2f", 1/atof("0.9375"));
      break;
    case 0x09: sprintf(video->video_aspect_ratio, "%.2f", 1/atof("0.9815"));
      break;
    case 0x0A: sprintf(video->video_aspect_ratio, "%.2f", 1/atof("1.0255"));
      break;
    case 0x0B: sprintf(video->video_aspect_ratio, "%.2f", 1/atof("1.0695"));
      break;
    case 0x0C: sprintf(video->video_aspect_ratio, "%.2f", 1/atof("1.1250"));
      break;
    case 0x0D: sprintf(video->video_aspect_ratio, "%.2f", 1/atof("1.1575"));
      break;
    case 0x0E: sprintf(video->video_aspect_ratio, "%.2f", 1/atof("1.2015"));
      break;
    default:   sprintf(video->video_aspect_ratio, "n/a");
      break;
  }

  switch(frame_rate_code) {
    case 0x01: sprintf(video->video_frame_rate, "23.976"); break;
    case 0x02: sprintf(video->video_frame_rate, "24"); break;
    case 0x03: sprintf(video->video_frame_rate, "25"); break;
    case 0x04: sprintf(video->video_frame_rate, "29.97"); break;
    case 0x05: sprintf(video->video_frame_rate, "30"); break;
    case 0x06: sprintf(video->video_frame_rate, "50"); break;
    case 0x07: sprintf(video->video_frame_rate, "59.94"); break;
    case 0x08: sprintf(video->video_frame_rate, "60"); break;
    default:   sprintf(video->video_frame_rate, "n/a"); break;
  }

  switch(chroma_format) {
    case 0x01: sprintf(video->video_chroma_format, "4:2:0"); break;
    case 0x02: sprintf(video->video_chroma_format, "4:2:2"); break;
    case 0x03: sprintf(video->video_chroma_format, "4:4:4"); break;
    default:   sprintf(video->video_chroma_format, "n/a"); break;
  }

  return(0);
}




int get_xvid_video_info(struct video_info *video, FILE *fp) {
  unsigned int time_inc = 0, time_inc_res, temp, par_width, par_height, n = 0;
  unsigned char dec_time_inc_bits, vo_type_ind, chroma_format, remaining_bits;

  rewind(fp);
  while(1) {
    if(ftell(fp) > 1000000 || feof(fp))
      return(-1);
    if((get_start_code(fp, 0x000001, 5000000) == 0x00)
       && (get_start_code(fp, 0x000001, 4) & 0xF0) == 0x20)
      break;
  }
  video->video_mpeg_type = 4;

  data = getc(fp);
  vo_type_ind = data << 1;
  data = getc(fp);
  vo_type_ind |= data >> 7;
  if(vo_type_ind > 4 || vo_type_ind == 2)
    return(-1); /* printf("video_object_type_indication is invalid\n"); */

  if((data >> 6) & 0x01) /* is_object_layer_identifier */
    return(-1); /* printf("is_object_layer_identifier is set\n"); */

  if((data & 0x3C) >> 2 == 1)
    sprintf(video->video_aspect_ratio, "%d", 1);
  else
    sprintf(video->video_aspect_ratio, "n/a");

  if((data & 0x3C) >> 2 == 15) { /* aspect_ratio_info */
    par_width = (data << 6);
    data = getc(fp);
    par_width |= (data >> 2);
    par_height = (data << 6);
    data = getc(fp);
    par_height |= (data >> 2);
  }

  if((data >> 1) & 0x01) { /* vol_control_parameters = 1 */
    chroma_format = (data & 0x01) << 1;
    data = getc(fp);
    chroma_format |= data >> 7;
    switch(chroma_format) {
      case 0x01: sprintf(video->video_chroma_format, "4:2:0"); break;
      default:   sprintf(video->video_chroma_format, "n/a"); break;
    }

    if(data & 0x20) /* vbv_parameters */
      return(-1); /* printf("vbv_parameters is set\n"); */
  }
  else
    return(-1);

  if((data & 0x18) >> 3 != 0) /* NOT VIDOBJLAY_SHAPE_RECTANGULAR */
    return(-1);

  if(!(data & 0x04) >> 2) /* Marker bit. */
    return(-1);

  time_inc_res = (data & 0x03) << 14;
  data = getc(fp);
  time_inc_res |= data << 6;
  data = getc(fp);
  time_inc_res |= data >> 2;
  if(time_inc_res > 0) {
    temp = time_inc_res - 1;
    while(temp) { /* log2bin (from xvidcore). */
      temp >>= 1;
      n++;
    }
    dec_time_inc_bits = n;
  }
  else
    dec_time_inc_bits = 1; /* For "old" xvid compatibility */

  if(!(data & 0x02) >> 1) /* Marker bit. */
    return(-1);

  if(!(data & 0x01)) /* NOT fixed_vop_rate */
    return(-1);

  data = getc(fp);
  remaining_bits = 0;
  if(dec_time_inc_bits < 8) {
    time_inc = data >> (8 - dec_time_inc_bits % 8);
    remaining_bits = 8 - dec_time_inc_bits % 8;
  }
  else if(dec_time_inc_bits == 8)
    time_inc = data;
  else if(dec_time_inc_bits < 16) {
    time_inc = data << (dec_time_inc_bits % 8);
    data = getc(fp);
    time_inc |= (data >> (16 - dec_time_inc_bits));
    remaining_bits = 16 - dec_time_inc_bits;
  }
  else if(dec_time_inc_bits == 16) {
    time_inc = data << 8;
    data = getc(fp);
    time_inc |= data;
  }
  sprintf(video->video_frame_rate, "%.2f", (float)time_inc_res / time_inc);

  if(!remaining_bits) {
    data = getc(fp);
    remaining_bits = 8;
  }
  temp = data << (20 + 8 - remaining_bits);
  data = getc(fp);
  temp |= data << (20 - remaining_bits);
  data = getc(fp);
  temp |= data << (20 - 8 - remaining_bits);
  if(remaining_bits < 4) {
    data = getc(fp);
    temp |= data << (20 - 8 - 8 - remaining_bits);
  }
  if(!((temp >> 27) & 0x01) || !((temp >> 13) & 0x01)) /* Marker bit. */
    return(-1);

  video->video_hor_size = (temp >> 14) & 0x1FFF;
  video->video_ver_size = temp & 0x1FFF;
  sprintf(video->video_profile, "n/a");
  sprintf(video->video_level, "n/a");
  video->video_bit_rate = video->video_duration = 0;

  return(0);
}

