#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

char *search[] = { 
 "[?!/##]",
 "(?!/##)",
 "[?!!/###]",
 "(?!!/###)",
 "[?/#]",
 "(?/#)",
 "[disk:!!/##]",
 "o?/o#",
 "disks[!!/##",
 " !/# ",
 " !!/##&/",
 "&/!!/## ",
 "[!!/#]",
 ": ?!/##&/",
 "xx/##", 
 "<!!/##>" };
int strings = 16;
/*
 
 ? = any char
 & = character is not (&/ = character is not /)
 # = total disk count
 ! = chars 0-9, o & x

 !!! USE LOWERCASE !!!

*/
/* DO NOT EDIT BEYOND THIS POINT */


/* REMOVE SPACES FROM STRING */
void removespaces(char *instr, int l ) {
 int	spaces = 0,
	cnt2 = 0, 
	cnt;

 for (cnt = 0 ; cnt < l ; cnt++ ) switch( instr[cnt] ) {
	case '\0':
	case  ' ':
	case '\n':
		if ( ! spaces ) instr[cnt2++] = ' ';
		spaces++;
		break;
	default :
		instr[cnt2++] = tolower(instr[cnt]);
		spaces = 0;
		break;
	}
 instr[cnt2] = 0;
}


int read_diz(char *filename) {
 int pos, fd, diskc, control, tgt, cnt, cnt2;
 unsigned int cnt3, matches;
 char data[4096];
 char disks[4];

 fd = open(filename, O_NONBLOCK);
 while ( (tgt = read(fd, data, 4096)) > 0 ) {
  removespaces(data, tgt);
  for ( cnt = 0 ; cnt < tgt ; cnt++ )  
   for ( cnt2 = 0 ; cnt2 < strings ; cnt2++ ) {
    pos = matches = control = 0; disks[0] = disks[1] = disks[2] = disks[3] = '\0';
    for ( cnt3 = 0; (unsigned int)cnt3 <= (strlen(search[cnt2]) - control) ; cnt3++ )
     switch ( search[cnt2][cnt3 + control] ) {
      case '#': if ( isdigit(data[cnt + cnt3]) || data[cnt + cnt3] == ' ' ) { matches++; pos += sprintf(disks + pos, "%c", data[cnt + cnt3]); } break;
      case '?': matches++; break;
      case '!': if ( isdigit(data[cnt + cnt3]) || data[cnt + cnt3] == 'o' || data[cnt + cnt3] == 'x') matches++; break;
      case '&': control++; if ( data[cnt + cnt3] != search[cnt2][cnt3 + control] ) matches++; break;
      default : if ( search[cnt2][cnt3 + control] == data[cnt + cnt3] ) matches++; break;
     }
    if ( (unsigned int)matches == strlen(search[cnt2]) - control && (diskc = atoi(disks))) return diskc;
   }
 }
 close(fd);
 return 0;
}
