/*
 *  (C) Copyright 2004 Wojtek Kaniewski <wojtekka@toxygen.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License Version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "avi.h"
#include "audio.h"
#include "video.h"
#include "objects.h"
#include "avinfo.h"

//struct avi vinfo;

const char *fourcc(FOURCC tag)
{
	static unsigned char buf[5];
	int i;

	buf[0] = tag & 255;
	buf[1] = (tag >> 8) & 255;
	buf[2] = (tag >> 16) & 255;
	buf[3] = (tag >> 24) & 255;
	buf[4] = 0;

	for (i = 0; i < 4; i++)
		if (buf[i] < 32 || buf[i] > 127)
			buf[i] = '?';

	return buf;
}

void error(const char *s)
{
	fprintf(stderr, "%s\n", s);
	exit(1);
}

DWORD get32(FILE *f)
{
	DWORD result;
	
	if (fread(&result, sizeof(DWORD), 1, f) != 1)
		error("Premature end of file");

	return result;
}

WORD get16(FILE *f)
{
	WORD result;
	
	if (fread(&result, sizeof(WORD), 1, f) != 1)
		error("Premature end of file");

	return result;
}

int avinfo(char *filename, char *vidinfo)
{
	FILE *f;
	FOURCC tag, list = 0, vids = 0, type = 0;
	DWORD size, hz = 0;
	WORD auds = 0, ch = 0;
	double fps = 0;
	int width = 0, height = 0, i;
	char fourcc_vids[5];
	const char *_vids = "Unknown codec", *_auds = "Unknown codec";
	char *vidnfo = 0;
	vidnfo = malloc(1024);

	f = fopen(filename, "rb");

	if (!f)
		return 1;
//		error("Unable to open file");

	tag = get32(f);
	size = get32(f);

	if (tag != MKTAG('R','I','F','F'))
		return 2;
//		error("Not a RIFF file");

	tag = get32(f);

	if (tag != MKTAG('A','V','I',' '))
		return 2;
//		error("Not an AVI file");

	while (!feof(f)) {
		tag = get32(f);
		size = get32(f);

		if (!tag)
			return 2;
//			error("Invalid file format");

		if (tag == MKTAG('L','I','S','T')) {
			if ((list = get32(f)) == MKTAG('m','o','v','i')) {
				fclose(f);
				break;
			}
			continue;
		}
		
		if (tag == MKTAG('a','v','i','h')) {
			AVIMAINHEADER avih;

			fread(&avih, sizeof(avih), 1, f);

			width = avih.dwWidth;
			height = avih.dwHeight;

			fseek(f, -sizeof(avih), SEEK_CUR);
		}

		if (tag == MKTAG('s','t','r','h')) {
			AVISTREAMHEADER strh;

			fread(&strh, sizeof(strh), 1, f);
			
			if ((type = strh.fccType) == MKTAG('v','i','d','s')) {
				vids = strh.fccHandler;
				fps = (double) strh.dwRate / (double) strh.dwScale;
			}

			fseek(f, -sizeof(strh), SEEK_CUR);
		}

		if (tag == MKTAG('s','t','r','f')) {
			if (type == MKTAG('a','u','d','s')) {
				WAVEFORMATEX wave;

				fread(&wave, sizeof(wave), 1, f);

				hz = wave.nSamplesPerSec;
				ch = wave.nChannels;
				auds = wave.wFormatTag;
				
				fseek(f, -sizeof(wave), SEEK_CUR);
			}
			
			if (type == MKTAG('v','i','d','s') && !vids) {
				BITMAPINFOHEADER bm;

				fread(&bm, sizeof(bm), 1, f);

				vids = bm.biCompression;
				
				fseek(f, -sizeof(bm), SEEK_CUR);
			}
		}

		fseek(f, size + (size & 1), SEEK_CUR);
	}

	for (i = 0; aviaudio_formats[i].tag; i++)
		if (aviaudio_formats[i].tag == auds)
			_auds = aviaudio_formats[i].descr;

	strcpy(fourcc_vids, fourcc(vids));

	for (i = 0; avideo_formats[i].tag; i++)
		if (!strcasecmp(avideo_formats[i].tag, fourcc_vids))
			_vids = avideo_formats[i].descr;

//	if (hz || ch || auds)
		sprintf(vidinfo,
			"{%dx%d} {%.2f} {%s} {%s} "
			"{%ld} {%d} {%s} {0x%.4x}",
			width, height, fps, _vids, fourcc(vids),
			hz, ch, _auds, auds);
/*	else
		sprintf(buf,
			"Video: %dx%d, %.2f fps, %s [%s] - "
			"No audio",
			width, height, fps, _vids, fourcc(vids));
*/
//	printf("%s\n", buf);
	return 0;
}

