#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "dizreader.h"

/*
 * ? = any char; & = character is not ... (ie &/ = character is not /)
 * # = total disk count; ! = chars 0-9, o & x
 * &! = character is not 0-9, o or x
 *
 * !!! USE LOWERCASE !!!
 */
char	*search[] = {
		"[?!/##]",
		"(?!/##)",
		"[?!!/###]",
		"(?!!/###)",
		"[?/#]",
		"[?/##]",
		"(?/#)",
		"[disk:!!/##]",
		"[disk:?!/##]",
		"o?/o#",
		"disks[!!/##",
		" !/# ",
		" !!/##&/&!",
		"&/!!/## ",
		"[!!/#]",
		": ?!/##&/",
		"xx/##",
		"<!!/##>",
		"x/##",
		"! of #",
		"? of #",
		"x of #",
		"ox of o#",
		"!! of ##",
		"?! of ##",
		"xx of ##"};

int		strings = 25;

/* REMOVE SPACES FROM STRING */
void 
removespaces(char *instr, int l)
{
	int		spaces = 0, cnt2 = 0, cnt;

	for (cnt = 0; cnt < l; cnt++)
		switch (instr[cnt]) {
		case '\0':
		case ' ':
		case '\n':
			if (!spaces)
				instr[cnt2++] = ' ';
			spaces++;
			break;
		default:
			instr[cnt2++] = tolower(instr[cnt]);
			spaces = 0;
			break;
		}
	instr[cnt2] = 0;
}

int 
//read_diz(char *filename)
read_diz(void)
{
	int		pos       , fd, diskc, control, tgt, cnt, cnt2;
	unsigned int	cnt3, matches;
	char		data      [4096];
	char		disks     [4];

//	fd = open(filename, O_NONBLOCK);
	fd = open("file_id.diz", O_NONBLOCK);
	while ((tgt = read(fd, data, 4096)) > 0) {
		removespaces(data, tgt);
		for (cnt = 0; cnt < tgt; cnt++)
			for (cnt2 = 0; cnt2 < strings; cnt2++) {
				pos = matches = control = 0;
				disks[0] = disks[1] = disks[2] = disks[3] = '\0';
				for (cnt3 = 0; (unsigned int)cnt3 <= ((unsigned int)strlen(search[cnt2]) - control); cnt3++)
					switch (search[cnt2][cnt3 + control]) {
					case '#':
						if (isdigit(data[cnt + cnt3]) || data[cnt + cnt3] == ' ' || data[cnt + cnt3] == 'o') {
							if (data[cnt + cnt3] == 'o')
								data[cnt + cnt3] = '0';
							matches++;
							pos += sprintf(disks + pos, "%c", data[cnt + cnt3]);
						} break;
					case '?':
						matches++;
						break;
					case '!':
						if (isdigit(data[cnt + cnt3]) || data[cnt + cnt3] == 'o' || data[cnt + cnt3] == 'x')
							matches++;
						break;
					case '&':
						control++;
						if (!(search[cnt2][cnt3 + control] == '!' && (isdigit(data[cnt + cnt3]) || data[cnt + cnt3] == 'o' || data[cnt + cnt3] == 'x')) && data[cnt + cnt3] != search[cnt2][cnt3 + control])
							matches++;
						break;
					default:
						if (search[cnt2][cnt3 + control] == data[cnt + cnt3])
							matches++;
						break;
					}
				if ((unsigned int)matches == (unsigned int)strlen(search[cnt2]) - control && (diskc = strtol(disks, NULL, 10)))
					return diskc;
			}
	}
	close(fd);
	return 0;
}
