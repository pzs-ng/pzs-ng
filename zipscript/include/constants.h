#ifndef __CONSTANTS_H_
#define __CONSTANTS_H_

#define VERSION		"0.7.3 BETA3 (PRE)"

#define F_IGNORED		254
#define F_BAD			255
#define F_NFO			253
#define F_DELETED		0
#define F_CHECKED		1
#define F_NOTCHECKED	2

#define TRUE			1
#define FALSE			0

#define DISABLED		NULL

#define FILE_MAX		256

#ifndef O_SYNC
# define O_SYNC O_FSYNC
#endif

#endif
