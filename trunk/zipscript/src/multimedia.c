#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "mp3info.h"
#include "objects.h"
#include "multimedia.h"

char *genre_s[] = {
	"Blues", "Classic Rock", "Country", "Dance",
	"Disco", "Funk", "Grunge", "Hip-Hop",
	"Jazz", "Metal", "New Age", "Oldies",
	"Other", "Pop", "R&B", "Rap",
	"Reggae", "Rock", "Techno", "Industrial",
	"Alternative", "Ska", "Death Metal", "Pranks",
	"Soundtrack", "Euro-Techno", "Ambient", "Trip-Hop",
	"Vocal", "Jazz+Funk", "Fusion", "Trance",
	"Classical", "Instrumental", "Acid", "House",
	"Game", "Sound Clip", "Gospel", "Noise",
	"Alt. Rock", "Bass", "Soul", "Punk",
	"Space", "Meditative", "Instrumental Pop", "Instrumental Rock",
	"Ethnic", "Gothic", "Darkwave", "Techno-Industrial",
	"Electronic", "Pop-Folk", "Eurodance", "Dream",
	"Southern Rock", "Comedy", "Cult", "Gangsta Rap",
	"Top 40", "Christian Rap", "Pop Funk", "Jungle",
	"Native American", "Cabaret", "New Wave", "Psychedelic",
	"Rave", "Showtunes", "Trailer", "Lo-Fi",
	"Tribal", "Acid Punk", "Acid Jazz", "Polka",
	"Retro", "Musical", "Rock & Roll", "Hard Rock",
	"Folk", "Folk Rock", "National Folk", "Swing",
	"Fast-Fusion", "Bebob", "Latin", "Revival",
	"Celtic", "Bluegrass", "Avantgarde", "Gothic Rock",
	"Progressive Rock", "Psychedelic Rock", "Symphonic Rock", "Slow Rock",
	"Big Band", "Chorus", "Easy Listening", "Acoustic",
	"Humour", "Speech", "Chanson", "Opera",
	"Chamber Music", "Sonata", "Symphony", "Booty Bass",
	"Primus", "Porn Groove", "Satire", "Slow Jam",
	"Club", "Tango", "Samba", "Folklore",
	"Ballad", "Power Ballad", "Rhythmic Soul", "Freestyle",
	"Duet", "Punk Rock", "Drum Solo", "A Cappella",
	"Euro-House", "Dance Hall", "Goa", "Drum & Bass",
	"Club-House", "Hardcore", "Terror", "Indie",
	"BritPop", "Negerpunk", "Polsk Punk", "Beat",
	"Christian Gangsta Rap", "Heavy Metal", "Black Metal", "Crossover",
	"Contemporary Christian", "Christian Rock", "Merengue", "Salsa",
	"Thrash Metal", "Anime", "JPop", "Synthpop",
	"Unknown"
};

char *fps_s[] = {"Unknown", "23.976", "24", "25", "29.97", "30", "50", "59.94", "60"};
char *layer_s[] = {"Unknown", "Layer III", "Layer II", "Layer I"};
char *codec_s[] = {"Mpeg 2.5", "Unknown", "Mpeg 2", "Mpeg 1"};
char *chanmode_s[] = {"Stereo", "Joint Stereo", "Dual Channel", "Single Channel", "Unknown"};

/*
 * Remove whitespace characters from both ends of a copy of '\0' terminated
 * STRING and return the result.
 */
char           *
trim(char *string)
{
	char           *result = 0;

	/* Ignore NULL pointers. */
	if (string) {
		char           *ptr = string;

		/* Skip leading whitespace. */
		while (strchr(WHITESPACE_STR, *ptr))
			++ptr;

		/* Make a copy of the remainder. */
		result = strdup(ptr);

		/* Move to the last character of the copy. */
		for (ptr = result; *ptr; ++ptr)
			 /* NOWORK */ ;
		--ptr;

		/* Remove trailing whitespace.  */
		for (--ptr; strchr(WHITESPACE_STR, *ptr); --ptr)
			*ptr = '\0';
	}
	return result;
}

/*
 * Updated     : 01.22.2002 Author      : Dark0n3
 * 
 * Description : Reads height, width and fps from mpeg file.
 */
void 
mpeg_video(char *f, struct video *video)
{
	int		fd;
	unsigned char	header[] = {0, 0, 1, 179};
	unsigned char	buf[8];
	short int	width = 0;
	short int	height = 0;
	unsigned char	aspect_ratio;
	unsigned char	fps = 0;
	short int	t = 0;

	fd = open(f, O_RDONLY);

	while (read(fd, buf, 1) == 1) {
		if (*buf == *(header + t)) {
			t++;
			if (t == sizeof(header)) {
				read(fd, buf, 8);
				memcpy(&t, buf, 2);

				t = *(buf + 1) >> 4;
				width = (*buf << 4) + t;
				height = ((*(buf + 1) - (t << 4)) << 4) + *(buf + 2);

				aspect_ratio = *(buf + 3) >> 4;
				fps = *(buf + 3) - (aspect_ratio << 4);
				break;
			}
		} else if (*buf == 0) {
			t = (t == 2 ? 2 : 1);
		} else {
			t = 0;
		}
	}

	video->height = height;
	video->width = width;
	video->fps = fps_s[fps > 8 ? 0 : fps];

	close(fd);
}

/*
 * Updated     : 01.22.2002 Author      : Dark0n3
 * 
 * Description : Reads height, width and fps from avi file.
 */
char		fps_t     [10];
void 
avi_video(char *f, struct video *video)
{
	int		fd;
	unsigned char	buf[56];
	int		fps;

	fd = open(f, O_RDONLY);
	if (lseek(fd, 32, 0) != -1 &&
	    read(fd, buf, 56) == 56) {
		memcpy(&fps, buf, 4);
		if (fps > 0) {
			memcpy(&video->width, buf + 32, 4);
			memcpy(&video->height, buf + 36, 4);
			sprintf(fps_t, "%i", 1000000 / fps);
			video->fps = fps_t;
		} else {
			video->height = 0;
			video->width = 0;
			video->fps = fps_s[0];
		}
	}
	close(fd);
}

char           *
get_preset(char vbr_header[4])
{
	int		preset;
	static char	returnval[10];
	memset(returnval, 0, 10);

	preset = ((unsigned char)vbr_header[0] & 7) * 256 + (unsigned char)vbr_header[1];

	strcpy(returnval, "NA");
	switch (preset) {
	case 1000:
		strcpy(returnval, "APR");
		break;		/* r3mix         */
	case 1001:
		strcpy(returnval, "APS");
		break;		/* standard      */
	case 1002:
		strcpy(returnval, "APE");
		break;		/* extreme       */
	case 1003:
		strcpy(returnval, "API");
		break;		/* insane        */
	case 1004:
		strcpy(returnval, "FAPS");
		break;		/* fast standard */
	case 1005:
		strcpy(returnval, "FAPE");
		break;		/* fast extreme  */
	case 1006:
		strcpy(returnval, "APM");
		break;		/* medium        */
	case 1007:
		strcpy(returnval, "FAPM");
		break;		/* fast medium   */
	}
	return returnval;
}


/*
 * Updated     : 01.22.2002 Author      : Dark0n3
 * 
 * Description : Reads MPEG header from file and stores info to 'audio'.
 */
void 
get_mpeg_audio_info(char *f, struct audio *audio)
{
	int		fd;
	int		t_genre;
	int		n;
	int		tag_ok = 0;
	unsigned char	header[4];
	unsigned char	vbr_header[4];
	unsigned char	xing_header1[4], xing_header2[4], xing_header3[4];
	unsigned char	fraunhofer_header[4];
	unsigned char	id3v2_header[10];
	unsigned char	version;
	unsigned char	layer;
	unsigned char	protected = 1;
	unsigned char	t_bitrate;
	unsigned char	t_samplingrate;
	unsigned char	channelmode;
	short int	bitrate = 0;
	short int	br_v1_l3[] = {0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 0};
	short int	br_v1_l2[] = {0, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384, 0};
	short int	br_v1_l1[] = {0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 0};
	short int	br_v2_l1[] = {0, 32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256, 0};
	short int	br_v2_l23[] = {0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, 0};
	unsigned int	samplingrate = 0;
	unsigned int	sr_v1[] = {44100, 48000, 32000, 0};
	unsigned int	sr_v2[] = {22050, 24000, 16000, 0};
	unsigned int	sr_v25[] = {11025, 12000, 8000, 0};
	int		vbr_offset = 0;
	int		t1;

	fd = open(f, O_RDONLY);

	n = 2;
	while (read(fd, header + 2 - n, n) == n) {
		if (*header == 255) {
			n = 2;
			if (*(header + 1) >= 224) {
				n = 0;
				break;
			} else {
				n = 2;
			}
		} else {
			if (*(header + 1) == 255) {
				*header = *(header + 1);
				n = 1;
			} else {
				n = 2;
			}
		}
	}

	/*
	 * mp3 header: AAAAAAAA AAABBCCD EEEEFFGH IIJJKLMM A - Frame sync B -
	 * MPEG audio version (version) C - Layer (layer) D - Protected by
	 * CRC (protected) E - Bitrate (t_bitrate) F - Sampling rate
	 * (t_samplingrate) G - Padding H - Private bit I - Channel mode
	 * (channelmode) J - Mode extension, K - Copyright L - Original, M -
	 * Emphasis
	 */
	if (n == 0) {
		*(header + 1) -= 224;

		read(fd, header + 2, 2);

		version = (*(header + 1)) >> 3;
		layer = (*(header + 1) >> 1) & ((1 << 2) - 1);	/* Nasty code, keeps CC
								 * in 'layer'. (layer =
								 * (*(header + 1) -
								 * (version << 3)) >> 1) */
		protected = (*(header + 1)) & 1;
		t_bitrate = (*(header + 2)) >> 4;
		t_samplingrate = (*(header + 2) >> 2) & ((1 << 2) - 1);	/* Nasty code, keeps FF
									 * in 't_samplingrate'.
									 * (t_samplingrate =
									 * *(header + 2) -
									 * (t_bitrate << 4) >>
									 * 2) */

		switch (version) {
		case 0:
			samplingrate = sr_v25[t_samplingrate];
		case 2:
			if (!samplingrate)
				samplingrate = sr_v2[t_samplingrate];
			switch (layer) {
			case 3:
				bitrate = br_v2_l1[t_bitrate];
				break;
			case 1:
			case 2:
				bitrate = br_v2_l23[t_bitrate];
				break;
			}
			break;
		case 3:
			samplingrate = sr_v1[t_samplingrate];
			switch (layer) {
			case 1:
				bitrate = br_v1_l3[t_bitrate];
				break;
			case 2:
				bitrate = br_v1_l2[t_bitrate];
				break;
			case 3:
				bitrate = br_v1_l1[t_bitrate];
				break;
			}
			break;
		}
		channelmode = (*(header + 3)) >> 6;

		sprintf(audio->samplingrate, "%i", samplingrate);
		sprintf(audio->bitrate, "%i", bitrate);
		audio->codec = codec_s[version];
		audio->layer = layer_s[layer];
		audio->channelmode = chanmode_s[channelmode];

		/* LAME VBR TAG */
		lseek(fd, 0, SEEK_SET);
		read(fd, id3v2_header, 10);

		if (memcmp(id3v2_header, "ID3", 3) == 0) {
			/*
			 * The ID3V2 tag is prepended to the mp3file, so we
			 * must adjust the vbr_offset accordingly. ID3V2 uses
			 * synchsafe integers hence this bitmanipulation.
			 * Reference :
			 * http://www.id3.org/id3v2.4.0-structure.txt
			 */
			vbr_offset = (id3v2_header[8] >> 1) * 256 + ((id3v2_header[8] & 1) * 128) + id3v2_header[9] + 10;
		}
		lseek(fd, 13 + vbr_offset, SEEK_SET);
		read(fd, xing_header1, 4);
		lseek(fd, 21 + vbr_offset, SEEK_SET);
		read(fd, xing_header2, 4);
		lseek(fd, 36 + vbr_offset, SEEK_SET);
		read(fd, xing_header3, 4);
		lseek(fd, 36 + vbr_offset, SEEK_SET);
		read(fd, fraunhofer_header, 4);

		if (memcmp(xing_header1, "Xing", 4) == 0 ||
		    memcmp(xing_header2, "Xing", 4) == 0 ||
		    memcmp(xing_header3, "Xing", 4) == 0 ||
		    memcmp(fraunhofer_header, "VBRI", 4) == 0) {

			lseek(fd, 156 + vbr_offset, SEEK_SET);
			read(fd, audio->vbr_version_string, 9);
			audio->vbr_version_string[9] = 0;
			for (t1 = 9; t1 > 0; t1--) {
				if (audio->vbr_version_string[t1] > 32) {
					break;
				}
				audio->vbr_version_string[t1] = 0;
			}
			audio->is_vbr = 1;
			if (memcmp(audio->vbr_version_string, "LAME", 4) == 0) {
				lseek(fd, 182 + vbr_offset, SEEK_SET);
				read(fd, vbr_header, 2);
				sprintf(audio->vbr_preset, "%s", get_preset((char *)vbr_header));

				if (audio->vbr_version_string[4] == 32)
					audio->vbr_version_string[4] = 0;

				/* strcpy(audio->bitrate, "VBR"); */
			} else {
				strcpy(audio->vbr_version_string, "Not LAME");
				strcpy(audio->vbr_preset, "NA");
			}

		} else {
			audio->is_vbr = 0;
			strcpy(audio->vbr_version_string, "NA");
			strcpy(audio->vbr_preset, "NA");
		}

		if (memcmp(fraunhofer_header, "VBRI", 4) == 0) {
			strcpy(audio->vbr_version_string, "FHG");
		}
		/* ID3 TAG */
		lseek(fd, -128, SEEK_END);
		read(fd, header, 3);
		if (memcmp(header, "TAG", 3) == 0) {	/* id3 tag */
			tag_ok = 1;
			read(fd, audio->id3_title, 30);
			read(fd, audio->id3_artist, 30);
			read(fd, audio->id3_album, 30);

			lseek(fd, -35, SEEK_END);
			read(fd, audio->id3_year, 4);
			if (tolower(audio->id3_year[1]) == 'k') {
				memcpy(header, audio->id3_year, 3);
				sprintf(audio->id3_year, "%c00%c", *header, *(header + 2));
			}
			lseek(fd, -1, SEEK_END);
			read(fd, header, 1);
			t_genre = (int)*header;
			if (t_genre < 0)
				t_genre += 256;
			if (t_genre > 148)
				t_genre = 148;

			audio->id3_genre = genre_s[t_genre];
			audio->id3_year[4] =
				audio->id3_artist[30] =
				audio->id3_title[30] =
				audio->id3_album[30] = 0;
		}
	} else {		/* header is broken, shouldnt crc fail? */
		strcpy(audio->samplingrate, "0");
		strcpy(audio->bitrate, "0");
		audio->codec = codec_s[1];
		audio->layer = layer_s[0];
		audio->channelmode = chanmode_s[4];
	}

	if (tag_ok == 0) {
		strcpy(audio->id3_year, "0000");
		strcpy(audio->id3_title, "Unknown");
		strcpy(audio->id3_artist, "Unknown");
		strcpy(audio->id3_album, "Unknown");
		audio->id3_genre = genre_s[148];
	}
	close(fd);

	get_mp3_info(f, audio);
//	sprintf(audio->bitrate, "%.0f", get_mp3_info(f));
}
