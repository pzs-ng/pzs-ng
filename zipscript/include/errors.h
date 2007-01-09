#include "../conf/zsconfig.h"

#ifndef SFV_FIRST
#define SFV_FIRST	"CRC-Check: SFV first!"
#endif
#ifndef ZERO_CRC
#define ZERO_CRC	"CRC-Check: ERROR!"
#endif
#ifndef BAD_CRC
#define BAD_CRC		"CRC-Check: BAD!"
#endif
#ifndef NOT_IN_SFV
#define NOT_IN_SFV	"CRC-Check: Not in sfv!"
#endif
#ifndef EMPTY_SFV
#define EMPTY_SFV	"SFV-file: BAD!"
#endif
#ifndef DOUBLE_SFV
#define DOUBLE_SFV	"SFV-file: DUPE!"
#endif
#ifndef BAD_ZIP
#define BAD_ZIP		"ZiP-Integrity: BAD!"
#endif
#ifndef EMPTY_FILE
#define EMPTY_FILE	"0byte-file: Not allowed!"
#endif
#ifndef BANNED_FILE
#define BANNED_FILE	"Banned file: Not allowed!"
#endif
#ifndef UNKNOWN_FILE
#define UNKNOWN_FILE	"%s-file: Not allowed!"				/* %s = file extension	 */
#endif
#ifndef BANNED_GENRE
#define BANNED_GENRE	"%s is banned here!"				/* %s = genre		 */
#endif
#ifndef BANNED_YEAR
#define BANNED_YEAR	"Releases from %s are not allowed here!"	/* %s = year		 */
#endif
#ifndef BANNED_BITRATE
#define BANNED_BITRATE	"%s kbit codec is not allowed here!"		/* %s = bitrate		 */
#endif
#ifndef DUPE_NFO
#define DUPE_NFO	"NFO-File: DUPE!"
#endif
#ifndef ZIP_NFO
#define ZIP_NFO		"NFO-File: Not allowed here!"
#endif
#ifndef SPEEDTEST
#define SPEEDTEST	"BW: %.1fMiB (%.1fMB) @ %.2fMbps (%.2fMB/s)."
#endif 

