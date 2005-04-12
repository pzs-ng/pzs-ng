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

#ifndef __AUDIO_H
#define __AUDIO_H

struct aviaudio_format {
	WORD tag;
	const char *descr;
} aviaudio_formats[] = {
	{ 0x1, "PCM" },
	{ 0x2, "MS ADPCM" },
	{ 0x6, "aLaw PCM" },
	{ 0x6, "uLaw PCM" },
	{ 0x11, "IMA ADPCM" },
	{ 0x31, "MS GSM" },
	{ 0x32, "MSN AUDIO" },
	{ 0x50, "MPEG Layer 1/2 Audio" },
	{ 0x55, "MPEG Layer 3 Audio" },
	{ 0x160, "Windows Media Audio" },
	{ 0x161, "Windows Media Audio" },
	{ 0x162, "Windows Media Audio" },
	{ 0x163, "Windows Media Audio" },
	{ 0x2000, "AC3" },
	{ 0, NULL },
};

#endif /* __AUDIO_H */
