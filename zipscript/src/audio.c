#include "video_info.h"

int get_audio_info(int program_type, struct audio_info *audio, FILE *fp) {
  int i, pes_packet_length;

  rewind(fp);

  /* Depending on stream type, find the first audio header. */
  if(program_type == 1 || program_type == 2) {    /* MPEG program stream. */
    while(1) {
      if((get_start_code(fp, 0x000001, 10000000) & 0xE0) == 0xC0) { /*PES hdr*/
        if(program_type == 1) {    /* MPEG-1 program stream. */
          SKIP_BYTES(2); /* packet_length. */
          do { /* skip stuffing bytes. */
            data = getc(fp);
          } while((data & 0x80) == 0x80);
          if((data & 0xC0) == 0x40)
            SKIP_BYTE;
          data = getc(fp);
          if((data & 0xF0) == 0x20)
            SKIP_BYTES(4);
          else if((data & 0xF0) == 0x30)
            SKIP_BYTES(9);
          else
            SKIP_BYTE;
        }
        else if(program_type == 2) {    /* MPEG-2 program stream. */
          SKIP_BYTES(4);
          data = getc(fp);
          SKIP_BYTES(data);
        }
        if(getc(fp) == 0xFF) {
          data = getc(fp);
          if((data & 0xF0) == 0xF0)
            break;
          else
            return(-1); /* No header found. */
        }
        else { /* AC-3 inside a private_stream_1? */
          rewind(fp);
          while(!feof(fp)) {
            while(1) {
              if(ftell(fp) > 5000000 || feof(fp))
                return(-1);
              if(get_start_code(fp, 0x000001, 50000000) == 0xBD)
                break;
            }
            pes_packet_length = getc(fp) << 8;
            pes_packet_length += getc(fp);
            SKIP_BYTES(2);
            SKIP_BYTES(getc(fp));
            for(i = 0; i < pes_packet_length; i++) {
              if(getc(fp) == 0x0B) {
                if(getc(fp) == 0x77) {
                  if(get_ac3_info(audio, fp) != 0)
                    return(-1);
                  else
                    return(0);
                }
              }
            }
          }
        }
      }
      if(ftell(fp) > 5000000 || feof(fp))
        return(-1);
    }
  }
  else if(program_type == 100) {    /* AVI stream. */
    rewind(fp);

    while(1) {
      if(get_start_code(fp, 0x4C4953, 100000) == 0x54) {    /* "LIST" */
        SKIP_BYTES(4);
        if(get_start_code(fp, 0x6D6F76, 4) == 0x69) {    /* "movi" */
          SKIP_BYTES(8);
          data = getc(fp);
          if(data == 0xFF) {
            data = getc(fp);
            if((data & 0xF0) == 0xF0)
              break;
            else
              return(-1); /* No header found. */
          }
          else if(data == 0x0B) { /* AC-3 audio? */
            if(getc(fp) == 0x77) {
              if(get_ac3_info(audio, fp) != 0)
                return(-1);
              else
                return(0);
            }
          }
          else
            return(-1);
        }
      }
      if(ftell(fp) > 1000000 || feof(fp))
        return(-1);
    }
  }

  /* Now extract the info from the audio header to the audio_info struct. */
  audio->audio_id = (data & 0x08) >> 3;
  switch((data & 0x06) >> 1) {
    case 0x03: audio->audio_layer = 1; break;
    case 0x02: audio->audio_layer = 2; break;
    case 0x01: audio->audio_layer = 3; break;
    default:   audio->audio_layer = 0; break;
  }
  audio->audio_protection_bit = data & 0x01;
  data = getc(fp);
  audio->audio_bitrate = 0;
  switch((data & 0xF0) >> 4) {
    case 0x01: if(audio->audio_id) {
                 audio->audio_bitrate = 32;
               }
               else {
                 if(audio->audio_layer == 1) audio->audio_bitrate = 32;
                 else if(audio->audio_layer == 2 || audio->audio_layer == 3)
                   audio->audio_bitrate = 8;
               }

    case 0x02: if(audio->audio_id) {
                 if(audio->audio_layer == 1) audio->audio_bitrate = 64;
                 else if(audio->audio_layer == 2) audio->audio_bitrate = 48;
                 else if(audio->audio_layer == 3) audio->audio_bitrate = 40;
               }
               else {
                 if(audio->audio_layer == 1) audio->audio_bitrate = 48;
                 else if(audio->audio_layer == 2 || audio->audio_layer == 3)
                   audio->audio_bitrate = 16;
               } break;

    case 0x03: if(audio->audio_id) {
                 if(audio->audio_layer == 1) audio->audio_bitrate = 96;
                 else if(audio->audio_layer == 2) audio->audio_bitrate = 56;
                 else if(audio->audio_layer == 3) audio->audio_bitrate = 48;
               }
               else {
                 if(audio->audio_layer == 1) audio->audio_bitrate = 56;
                 else if(audio->audio_layer == 2 || audio->audio_layer == 3)
                   audio->audio_bitrate = 24;
               } break;

    case 0x04: if(audio->audio_id) {
                 if(audio->audio_layer == 1) audio->audio_bitrate = 128;
                 else if(audio->audio_layer == 2) audio->audio_bitrate = 64;
                 else if(audio->audio_layer == 3) audio->audio_bitrate = 56;
               }
               else {
                 if(audio->audio_layer == 1) audio->audio_bitrate = 64;
                 else if(audio->audio_layer == 2 || audio->audio_layer == 3)
                   audio->audio_bitrate = 32;
               } break;

    case 0x05: if(audio->audio_id) {
                 if(audio->audio_layer == 1) audio->audio_bitrate = 160;
                 else if(audio->audio_layer == 2) audio->audio_bitrate = 80;
                 else if(audio->audio_layer == 3) audio->audio_bitrate = 64;
               }
               else {
                 if(audio->audio_layer == 1) audio->audio_bitrate = 80;
                 else if(audio->audio_layer == 2 || audio->audio_layer == 3)
                   audio->audio_bitrate = 40;
               } break;

    case 0x06: if(audio->audio_id) {
                 if(audio->audio_layer == 1) audio->audio_bitrate = 192;
                 else if(audio->audio_layer == 2) audio->audio_bitrate = 96;
                 else if(audio->audio_layer == 3) audio->audio_bitrate = 80;
               }
               else {
                 if(audio->audio_layer == 1) audio->audio_bitrate = 96;
                 else if(audio->audio_layer == 2 || audio->audio_layer == 3)
                   audio->audio_bitrate = 48;
               } break;

    case 0x07: if(audio->audio_id) {
                 if(audio->audio_layer == 1) audio->audio_bitrate = 224;
                 else if(audio->audio_layer == 2) audio->audio_bitrate = 112;
                 else if(audio->audio_layer == 3) audio->audio_bitrate = 96;
               }
               else {
                 if(audio->audio_layer == 1) audio->audio_bitrate = 112;
                 else if(audio->audio_layer == 2 || audio->audio_layer == 3)
                   audio->audio_bitrate = 56;
               } break;

    case 0x08: if(audio->audio_id) {
                 if(audio->audio_layer == 1) audio->audio_bitrate = 256;
                 else if(audio->audio_layer == 2) audio->audio_bitrate = 128;
                 else if(audio->audio_layer == 3) audio->audio_bitrate = 112;
               }
               else {
                 if(audio->audio_layer == 1) audio->audio_bitrate = 128;
                 else if(audio->audio_layer == 2 || audio->audio_layer == 3)
                   audio->audio_bitrate = 64;
               } break;

    case 0x09: if(audio->audio_id) {
                 if(audio->audio_layer == 1) audio->audio_bitrate = 288;
                 else if(audio->audio_layer == 2) audio->audio_bitrate = 160;
                 else if(audio->audio_layer == 3) audio->audio_bitrate = 128;
               }
               else {
                 if(audio->audio_layer == 1) audio->audio_bitrate = 144;
                 else if(audio->audio_layer == 2 || audio->audio_layer == 3)
                   audio->audio_bitrate = 80;
               } break;

    case 0x0A: if(audio->audio_id) {
                 if(audio->audio_layer == 1) audio->audio_bitrate = 320;
                 else if(audio->audio_layer == 2) audio->audio_bitrate = 192;
                 else if(audio->audio_layer == 3) audio->audio_bitrate = 160;
               }
               else {
                 if(audio->audio_layer == 1) audio->audio_bitrate = 160;
                 else if(audio->audio_layer == 2 || audio->audio_layer == 3)
                   audio->audio_bitrate = 96;
               } break;

    case 0x0B: if(audio->audio_id) {
                 if(audio->audio_layer == 1) audio->audio_bitrate = 352;
                 else if(audio->audio_layer == 2) audio->audio_bitrate = 224;
                 else if(audio->audio_layer == 3) audio->audio_bitrate = 192;
               }
               else {
                 if(audio->audio_layer == 1) audio->audio_bitrate = 176;
                 else if(audio->audio_layer == 2 || audio->audio_layer == 3)
                   audio->audio_bitrate = 112;
               } break;

    case 0x0C: if(audio->audio_id) {
                 if(audio->audio_layer == 1) audio->audio_bitrate = 384;
                 else if(audio->audio_layer == 2) audio->audio_bitrate = 256;
                 else if(audio->audio_layer == 3) audio->audio_bitrate = 224;
               }
               else {
                 if(audio->audio_layer == 1) audio->audio_bitrate = 192;
                 else if(audio->audio_layer == 2 || audio->audio_layer == 3)
                   audio->audio_bitrate = 128;
               } break;

    case 0x0D: if(audio->audio_id) {
                 if(audio->audio_layer == 1) audio->audio_bitrate = 416;
                 else if(audio->audio_layer == 2) audio->audio_bitrate = 320;
                 else if(audio->audio_layer == 3) audio->audio_bitrate = 256;
               }
               else {
                 if(audio->audio_layer == 1) audio->audio_bitrate = 224;
                 else if(audio->audio_layer == 2 || audio->audio_layer == 3)
                   audio->audio_bitrate = 144;
               } break;

    case 0x0E: if(audio->audio_id) {
                 if(audio->audio_layer == 1) audio->audio_bitrate = 448;
                 else if(audio->audio_layer == 2) audio->audio_bitrate = 384;
                 else if(audio->audio_layer == 3) audio->audio_bitrate = 320;
               }
               else {
                 if(audio->audio_layer == 1) audio->audio_bitrate = 256;
                 else if(audio->audio_layer == 2 || audio->audio_layer == 3)
                   audio->audio_bitrate = 160;
               } break;
    default:   audio->audio_bitrate = 0; break; /* free format or forbidden. */
  }

  switch((data & 0x0C) >> 2) {
    case 0x00: if(audio->audio_id)
                 audio->audio_sampl_freq = 44100;
               else
                 audio->audio_sampl_freq = 22050; break;
    case 0x01: if(audio->audio_id)
                 audio->audio_sampl_freq = 48000;
               else
                 audio->audio_sampl_freq = 24000; break;
    case 0x02: if(audio->audio_id)
                 audio->audio_sampl_freq = 32000;
               else
                 audio->audio_sampl_freq = 16000; break;
    default:   audio->audio_sampl_freq = 0; break; /* reserved. */
  }

  data = getc(fp);
  switch((data & 0xC0) >> 6) {
    case 0x00: sprintf(audio->audio_mode, "stereo"); break;
    case 0x01: sprintf(audio->audio_mode, "joint_stereo"); break;
    case 0x02: sprintf(audio->audio_mode, "dual_channel"); break;
    case 0x03: sprintf(audio->audio_mode, "single_channel"); break;
  }

  audio->audio_mode_extension = (data & 0x30) >> 4;
  audio->audio_copyright = (data & 0x08) >> 3;
  audio->audio_original = (data & 0x06) >> 2;
  audio->audio_emphasis = data & 0x03;

  return(0);
}




int get_ac3_info(struct audio_info *audio, FILE *fp) {
  unsigned char lfe = 0;
  int i;

  audio->audio_id = 100;
  SKIP_BYTES(2); /* CRC */
  data = getc(fp);
  switch(data >> 6) {
    case 0x00: audio->audio_sampl_freq = 48000; break;
    case 0x01: audio->audio_sampl_freq = 44100; break;
    case 0x02: audio->audio_sampl_freq = 32000; break;
    default:   audio->audio_sampl_freq = 0; break;
  }
  switch(data & 0x3F) {
    case 0x00:
    case 0x01: audio->audio_bitrate = 32; break;
    case 0x02:
    case 0x03: audio->audio_bitrate = 40; break;
    case 0x04:
    case 0x05: audio->audio_bitrate = 48; break;
    case 0x06:
    case 0x07: audio->audio_bitrate = 56; break;
    case 0x08:
    case 0x09: audio->audio_bitrate = 64; break;
    case 0x0A:
    case 0x0B: audio->audio_bitrate = 80; break;
    case 0x0C:
    case 0x0D: audio->audio_bitrate = 96; break;
    case 0x0E:
    case 0x0F: audio->audio_bitrate = 112; break;
    case 0x10:
    case 0x11: audio->audio_bitrate = 128; break;
    case 0x12:
    case 0x13: audio->audio_bitrate = 160; break;
    case 0x14:
    case 0x15: audio->audio_bitrate = 192; break;
    case 0x16:
    case 0x17: audio->audio_bitrate = 224; break;
    case 0x18:
    case 0x19: audio->audio_bitrate = 256; break;
    case 0x1A:
    case 0x1B: audio->audio_bitrate = 320; break;
    case 0x1C:
    case 0x1D: audio->audio_bitrate = 384; break;
    case 0x1E:
    case 0x1F: audio->audio_bitrate = 448; break;
    case 0x20:
    case 0x21: audio->audio_bitrate = 512; break;
    case 0x22:
    case 0x23: audio->audio_bitrate = 576; break;
    case 0x24:
    case 0x25: audio->audio_bitrate = 640; break;
    default: audio->audio_bitrate = 0; break;
  }
  SKIP_BYTE;
  data = getc(fp);
  i = 0;
  if(((data >> 5) & 0x01) && ((data >> 5) != 0x01))
    i += 2;
  if((data >> 5) & 0x04)
    i += 2;
  if((data >> 5) == 0x02)
    i += 2;
  switch(i) {
    case 0: lfe = (data & 0x10) >> 4; break;
    case 2: lfe = (data & 0x04) >> 2; break;
    case 4: lfe = data & 0x01; break;
    case 6: data = getc(fp); lfe = (data & 0x40) >> 6;
            ungetc(data, fp); break;
  }
  switch(data >> 5) {
    case 0x00: if(lfe) sprintf(audio->audio_mode, "1+1+lfe");
               else sprintf(audio->audio_mode, "1+1"); break;
    case 0x01: if(lfe) sprintf(audio->audio_mode, "1/0+lfe");
               else sprintf(audio->audio_mode, "1/0"); break;
    case 0x02: if(lfe) sprintf(audio->audio_mode, "2/0+lfe");
               else sprintf(audio->audio_mode, "2/0"); break;
    case 0x03: if(lfe) sprintf(audio->audio_mode, "3/0+lfe");
               else sprintf(audio->audio_mode, "3/0"); break;
    case 0x04: if(lfe) sprintf(audio->audio_mode, "2/1+lfe");
               else sprintf(audio->audio_mode, "2/1"); break;
    case 0x05: if(lfe) sprintf(audio->audio_mode, "3/1+lfe");
               else sprintf(audio->audio_mode, "3/1"); break;
    case 0x06: if(lfe) sprintf(audio->audio_mode, "2/2+lfe");
               else sprintf(audio->audio_mode, "2/2"); break;
    case 0x07: if(lfe) sprintf(audio->audio_mode, "3/2+lfe");
               else sprintf(audio->audio_mode, "3/2"); break;
  }

  return(0);
}

