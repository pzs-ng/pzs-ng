/*
 * Copyright (C) 2000-2001  Cedric Tefft <cedric@earthling.net>
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 675
 * Mass Ave, Cambridge, MA 02139, USA.
 * 
**************************************************************************
 * 
 * 
 * This file is based in part on:
 * 
 * MP3Info 0.5 by Ricardo Cerqueira <rmc@rccn.net> MP3Stat 0.9 by Ed Sweetman
 * <safemode@voicenet.com> and Johannes Overmann <overmann@iname.com>
 * 
 */

#include "mp3info.h"
#include "objects.h"

int		layer_tab  [4] = {0, 3, 2, 1};

int		frequencies[3][4] = {
	{22050, 24000, 16000, 50000},	/* MPEG 2.0 */
	{44100, 48000, 32000, 50000},	/* MPEG 1.0 */
	{11025, 12000, 8000, 50000}	/* MPEG 2.5 */
};

int		bitrate    [2][3][14] = {
	{			/* MPEG 2.0 */
		{32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256},	/* layer 1 */
		{8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160},	/* layer 2 */
		{8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160}	/* layer 3 */
	},

	{			/* MPEG 1.0 */
		{32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448},	/* layer 1 */
		{32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384},	/* layer 2 */
		{32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320}	/* layer 3 */
	}
};

int		frame_size_index[] = {24000, 72000, 72000};

char           *mode_text[] = {
	"Stereo", "Joint Stereo", "Dual Channel", "Mono"
};

char           *emphasis_text[] = {
	"none", "50/15 microsecs", "reserved", "CCITT J 17"
};

int 
get_first_header(mp3info * mp3, int startpos)
{
	int		k         , l = 0, c;
	mp3header	h    , h2;
	int		valid_start = 0;

	fseek(mp3->file, startpos, SEEK_SET);
	while (1) {
		while ((c = fgetc(mp3->file)) != 255 && (c != EOF));
		if (c == 255) {
			ungetc(c, mp3->file);
			valid_start = ftell(mp3->file);
			if ((l = get_header(mp3->file, &h))) {
				fseek(mp3->file, l - FRAME_HEADER_SIZE, SEEK_CUR);
				for (k = 1; (k < MIN_CONSEC_GOOD_FRAMES) && (mp3->datasize - ftell(mp3->file) >= FRAME_HEADER_SIZE); k++) {
					if (!(l = get_header(mp3->file, &h2)))
						break;
					if (!sameConstant(&h, &h2))
						break;
					fseek(mp3->file, l - FRAME_HEADER_SIZE, SEEK_CUR);
				}
				if (k == MIN_CONSEC_GOOD_FRAMES) {
					fseek(mp3->file, valid_start, SEEK_SET);
					memcpy(&(mp3->header), &h2, sizeof(mp3header));
					mp3->header_isvalid = 1;
					return 1;
				}
			}
		} else {
			return 0;
		}
	}

	return 0;
}

/*
 * get_next_header() - read header at current position or look for the next
 * valid header if there isn't one at the current position
 */
int 
get_next_header(mp3info * mp3)
{
	int		l = 0,	c  , skip_bytes = 0;
	mp3header	h;

	while (1) {
		while ((c = fgetc(mp3->file)) != 255 && (ftell(mp3->file) < mp3->datasize))
			skip_bytes++;
		if (c == 255) {
			ungetc(c, mp3->file);
			if ((l = get_header(mp3->file, &h))) {
				if (skip_bytes)
					mp3->badframes++;
				fseek(mp3->file, l - FRAME_HEADER_SIZE, SEEK_CUR);
				return 15 - h.bitrate;
			} else {
				skip_bytes += FRAME_HEADER_SIZE;
			}
		} else {
			if (skip_bytes)
				mp3->badframes++;
			return 0;
		}
	}
}

/*
 * Get next MP3 frame header. Return codes: positive value = Frame Length of
 * this header 0 = No, we did not retrieve a valid frame header
 */
int 
get_header(FILE * file, mp3header * header)
{
	unsigned char	buffer[FRAME_HEADER_SIZE];
	int		fl;

	if (fread(&buffer, FRAME_HEADER_SIZE, 1, file) < 1) {
		header->sync = 0;
		return 0;
	}
	header->sync = (((int)buffer[0] << 4) | ((int)(buffer[1] & 0xE0) >> 4));
	if (buffer[1] & 0x10)
		header->version = (buffer[1] >> 3) & 1;
	else
		header->version = 2;
	header->layer = (buffer[1] >> 1) & 3;
	if ((header->sync != 0xFFE) || (header->layer != 1)) {
		header->sync = 0;
		return 0;
	}
	header->crc = buffer[1] & 1;
	header->bitrate = (buffer[2] >> 4) & 0x0F;
	header->freq = (buffer[2] >> 2) & 0x3;
	header->padding = (buffer[2] >> 1) & 0x1;
	header->extension = (buffer[2]) & 0x1;
	header->mode = (buffer[3] >> 6) & 0x3;
	header->mode_extension = (buffer[3] >> 4) & 0x3;
	header->copyright = (buffer[3] >> 3) & 0x1;
	header->original = (buffer[3] >> 2) & 0x1;
	header->emphasis = (buffer[3]) & 0x3;

	return ((fl = frame_length(header)) >= MIN_FRAME_SIZE ? fl : 0);
}

int 
frame_length(mp3header * header)
{
	return header->sync == 0xFFE ?
	(frame_size_index[3 - header->layer] * ((header->version & 1) + 1) *
	 header_bitrate(header) / header_frequency(header)) +
	header->padding : 1;
}

int 
header_layer(mp3header * h)
{
	return layer_tab[h->layer];
}

int 
header_bitrate(mp3header * h)
{
	return bitrate[h->version & 1][3 - h->layer][h->bitrate - 1];
}

int 
header_frequency(mp3header * h)
{
	return frequencies[h->version][h->freq];
}

char *
header_emphasis(mp3header * h)
{
	return emphasis_text[h->emphasis];
}

char *
header_mode(mp3header * h)
{
	return mode_text[h->mode];
}

int 
sameConstant(mp3header * h1, mp3header * h2)
{
	if ((*(uint *) h1) == (*(uint *) h2))
		return 1;

	if ((h1->version == h2->version) &&
	    (h1->layer == h2->layer) &&
	    (h1->crc == h2->crc) &&
	    (h1->freq == h2->freq) &&
	    (h1->mode == h2->mode) &&
	    (h1->copyright == h2->copyright) &&
	    (h1->original == h2->original) &&
	    (h1->emphasis == h2->emphasis))
		return 1;
	else
		return 0;
}

int 
get_id3(mp3info * mp3, struct audio *audio)
{
	int		retcode = 0;
	unsigned long	sizeval = 0, totsize = 0;
	char		fbuf[4];
	char		mbuf[255];

	if (mp3->datasize >= 128) {
		if (fseek(mp3->file, -128, SEEK_END)) {
			fprintf(stderr, "ERROR: Couldn't read last 128 bytes of %s!!\n", mp3->filename);
			retcode |= 4;
		} else {
			fread(fbuf, 1, 3, mp3->file);
			fbuf[3] = '\0';
			mp3->id3.genre[0] = 255;
			if (!strcmp((const char *)"ID3", (const char *)fbuf)) {
				mp3->id3_isvalid = 1;
				mp3->datasize -= 128;
				fseek(mp3->file, -125, SEEK_END);
				fread(mp3->id3.title, 1, 30, mp3->file);
				mp3->id3.title[30] = '\0';
				fread(mp3->id3.artist, 1, 30, mp3->file);
				mp3->id3.artist[30] = '\0';
				fread(mp3->id3.album, 1, 30, mp3->file);
				mp3->id3.album[30] = '\0';
				fread(mp3->id3.year, 1, 4, mp3->file);
				mp3->id3.year[4] = '\0';
				fread(mp3->id3.comment, 1, 30, mp3->file);
				mp3->id3.comment[30] = '\0';
				if (mp3->id3.comment[28] == '\0') {
					mp3->id3.track[0] = mp3->id3.comment[29];
				}
				fread(mp3->id3.genre, 1, 1, mp3->file);
				unpad(mp3->id3.title);
				unpad(mp3->id3.artist);
				unpad(mp3->id3.album);
				unpad(mp3->id3.year);
				unpad(mp3->id3.comment);

				memcpy(&(audio->id3_artist), &(mp3->id3.artist), sizeof(mp3->id3.artist));
				memcpy(&(audio->id3_title), &(mp3->id3.title), sizeof(mp3->id3.title));
				memcpy(&(audio->id3_album), &(mp3->id3.album), sizeof(mp3->id3.album));
				memcpy(&(audio->id3_year), &(mp3->id3.year), sizeof(mp3->id3.year));
			} else {
				/* ID3v2 ? */
				fseek(mp3->file, 0L, SEEK_SET);
				fread(fbuf, 1, 3, mp3->file);
				fbuf[3] = '\0';
				if (!strcmp((const char *)"ID3", (const char *)fbuf)) {
					mp3->id3_isvalid = 1;
					fread(mp3->id3v2.version, 1, 2, mp3->file);
					fread(mp3->id3v2.flags, 1, 1, mp3->file);
					fread(fbuf, 4, 1, mp3->file);
					totsize = fbuf[0];
					totsize <<= 8;
					totsize += fbuf[1];
					totsize <<= 8;
					totsize += fbuf[2];
					totsize <<= 8;
					totsize += fbuf[3];
					if ((unsigned int)mp3->id3v2.flags[0] >> 6 & ~(~0 << 1)) {	/* ID3v2 Extended Header indicated */
						fread(fbuf, 4, 1, mp3->file);
						sizeval = fbuf[0];
						sizeval <<= 8;
						sizeval += fbuf[1];
						sizeval <<= 8;
						sizeval += fbuf[2];
						sizeval <<= 8;
						sizeval += fbuf[3];
						fseek(mp3->file, sizeval - 4, SEEK_CUR);
						totsize -= (sizeval + 4);
					}
					while ((totsize) && (fread(mp3->id3v2.frame_id, 1, 4, mp3->file) == 4)) {
						if (mp3->id3v2.frame_id[0] < 0x30 || mp3->id3v2.frame_id[0] > 0x5A)
							break;
						mp3->id3v2.frame_id[4] = '\0';
						fread(fbuf, 4, 1, mp3->file);
						sizeval = fbuf[0];
						sizeval <<= 8;
						sizeval += fbuf[1];
						sizeval <<= 8;
						sizeval += fbuf[2];
						sizeval <<= 8;
						sizeval += fbuf[3];
						totsize -= sizeval;
						fseek(mp3->file, 2, SEEK_CUR);					/* Ignore flags */
						if (!strcmp("TALB", mp3->id3v2.frame_id)) {	/* Album name */
							bzero(mbuf, sizeof(mbuf));
							fread(mbuf, 1, sizeval < sizeof(mbuf) ? sizeval : sizeof(mbuf), mp3->file);
							unpadpre(mbuf);
							unpad(mbuf);
							memcpy(&(audio->id3_album), &(mbuf), sizeof(mp3->id3.album));
							mp3->id3.album[sizeof(mp3->id3.album)] = '\0';
							if (sizeval > sizeof(mbuf))
								fseek(mp3->file, sizeval - sizeof(mbuf), SEEK_CUR);
						} else if (!strcmp("TPE1", mp3->id3v2.frame_id)) {	/* Artist name */
							bzero(mbuf, sizeof(mbuf));
							fread(mbuf, 1, sizeval < sizeof(mbuf) ? sizeval : sizeof(mbuf), mp3->file);
							unpadpre(mbuf);
							unpad(mbuf);
							memcpy(&(audio->id3_artist), &(mbuf), sizeof(mp3->id3.artist));
							mp3->id3.artist[sizeof(mp3->id3.artist)] = '\0';
							if (sizeval > sizeof(mbuf))
								fseek(mp3->file, sizeval - sizeof(mbuf), SEEK_CUR);
						} else if (!strcmp("TIT2", mp3->id3v2.frame_id)) {	/* Title name */
							bzero(mbuf, sizeof(mbuf));
							fread(mbuf, 1, sizeval < sizeof(mbuf) ? sizeval : sizeof(mbuf), mp3->file);
							unpadpre(mbuf);
							unpad(mbuf);
							memcpy(&(audio->id3_title), &(mbuf), sizeof(mp3->id3.title));
							mp3->id3.title[sizeof(mp3->id3.title)] = '\0';
							if (sizeval > sizeof(mbuf))
								fseek(mp3->file, sizeval - sizeof(mbuf), SEEK_CUR);
						} else if (!strcmp("TYER", mp3->id3v2.frame_id)) {	/* Album Year */
							bzero(mbuf, sizeof(mbuf));
							fread(mbuf, 1, sizeval < sizeof(mbuf) ? sizeval : sizeof(mbuf), mp3->file);
							unpadpre(mbuf);
							unpad(mbuf);
							memcpy(&(audio->id3_year), &(mbuf), sizeof(mp3->id3.year));
							mp3->id3.year[sizeof(mp3->id3.year)] = '\0';
							if (sizeval > sizeof(mbuf))
								fseek(mp3->file, sizeval - sizeof(mbuf), SEEK_CUR);
						} else if (!strcmp("TCON", mp3->id3v2.frame_id)) {	/* Genre */
							bzero(mbuf, sizeof(mbuf));
							fread(mbuf, 1, sizeval < sizeof(mbuf) ? sizeval : sizeof(mbuf), mp3->file);
							unpadpre(mbuf);
							unpad(mbuf);
							getgenre(mbuf);
							audio->id3_genrenum = strtol(mbuf, NULL, 10);
							if (sizeval > sizeof(mbuf))
								fseek(mp3->file, sizeval - sizeof(mbuf), SEEK_CUR);

						} else
							fseek(mp3->file, sizeval, SEEK_CUR);
					}
				}
			}
		}
	}
	return retcode;
}

/* Remove trailing whitespace from the end of a string */
char *
unpad(char *string)
{
	char           *pos = string + (int)strlen(string) - 1;
	while (isspace(pos[0]))
		(pos--)[0] = 0;
	return string;
}

/* Remove preceding \0 from the start of a string */
char *
unpadpre(char *string)
{
	char	buf[255];
	char   *pos = string;
	int	strsize = (int)sizeof(string) -1;

	bzero(buf, sizeof(buf));
	while (pos[0] == '\0' && pos < string + strsize)
		pos++;
	sprintf(buf, "%s",  pos);
	sprintf(string, "%s", buf);
	return string;
}

/* Special genre thingy for id3v2*/
char *
getgenre(char *string)
{
	char	buf[255];
	char   *pos = string, *pos2 = buf;
	int	strsize = (int)sizeof(string);

	bzero(buf, sizeof(buf));
	while (((pos[0] < 0x30) || (pos[0] > 0x39)) && (pos < string + strsize))
		pos++;
	while (((pos[0] >= 0x30) && (pos[0] <= 0x39)) && (pos < string + strsize)) {
		pos2[0] = pos[0];
		pos++;
		pos2++;
	}
	sprintf(string, "%s", buf);
	return string;
}

void
get_mp3_info(char *f, struct audio *audio)
{
	int		fullscan = 1;

	FILE           *fp;
	mp3info		mp3;

	int		frame_type [15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	float		seconds = 0, total_rate = 0;
	int		frames = 0, frame_types = 0, frames_so_far = 0;
	int		l         , vbr_median = -1;
	int		_bitrate   , lastrate;
	int		counter = 0;
	mp3header	header;
	struct stat	filestat;
	off_t		sample_pos, data_start = 0;

	if (!(fp = fopen(f, "r"))) {
		return;
	}
	memset(&mp3, 0, sizeof(mp3info));

	mp3.filename = f;
	mp3.file = fp;

	stat(mp3.filename, &filestat);
	mp3.datasize = filestat.st_size;
	get_id3(&mp3, audio);

	if (fullscan == 0) {
		if (get_first_header(&mp3, 0L)) {
			data_start = ftell(mp3.file);
			lastrate = 15 - mp3.header.bitrate;
			while ((counter < NUM_SAMPLES) && lastrate) {
				sample_pos = (counter * (mp3.datasize / NUM_SAMPLES + 1)) + data_start;
				if (get_first_header(&mp3, sample_pos)) {
					_bitrate = 15 - mp3.header.bitrate;
				} else {
					_bitrate = -1;
				}
				if (_bitrate != lastrate) {
					mp3.vbr = 1;
					if (fullscan == 0) {
						counter = NUM_SAMPLES;
						fullscan = 1;
					}
				}
				lastrate = _bitrate;
				counter++;
			}
			if (!(fullscan == 1)) {
				mp3.frames = (mp3.datasize - data_start) / (l = frame_length(&mp3.header));
				mp3.seconds = (int)((float)(frame_length(&mp3.header) * mp3.frames) /
						    (float)(header_bitrate(&mp3.header) * 125) + 0.5);
				mp3.vbr_average = (float)header_bitrate(&mp3.header);
			}
		}
	}
	if (fullscan == 1) {
		if (get_first_header(&mp3, 0L)) {
			data_start = ftell(mp3.file);
			while ((_bitrate = get_next_header(&mp3))) {
				frame_type[15 - _bitrate]++;
				frames++;
			}
			memcpy(&header, &(mp3.header), sizeof(mp3header));
			for (counter = 0; counter < 15; counter++) {
				if (frame_type[counter]) {
					frame_types++;
					header.bitrate = counter;
					frames_so_far += frame_type[counter];
					seconds += (float)(frame_length(&header) * frame_type[counter]) /
						(float)(header_bitrate(&header) * 125);
					total_rate += (float)((header_bitrate(&header)) * frame_type[counter]);
					if ((vbr_median == -1) && (frames_so_far >= frames / 2))
						vbr_median = counter;
				}
			}
			mp3.seconds = (int)(seconds + 0.5);
			mp3.header.bitrate = vbr_median;
			mp3.frames = frames - 1;
			mp3.vbr_average = total_rate / mp3.frames;
			if (frame_types > 1) {
				mp3.vbr = 1;
			}
		}
	}
	fclose(mp3.file);
	if (mp3.vbr || *audio->bitrate == '0')
		sprintf(audio->bitrate, "%.0f", (mp3.vbr_average));
//	audio->is_vbr = mp3.vbr;
//	return mp3.vbr_average;
}

/*
 * int main(void) { FILE *fp; char b[32]; sprintf(b, "VBR %.0f",
 * get_mp3_info("test.mp3")); printf("%s\n",b); return 0; }
 */
