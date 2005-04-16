#include <sys/stat.h>
#include "avi.h"
#include "audio.h"
#include "video.h"
#include "objects.h"

const char *fourcc(FOURCC);
void error(const char *);
DWORD get32(FILE *);
WORD get16(FILE *);
extern int avinfo(char *, struct MULTIMEDIA *);

