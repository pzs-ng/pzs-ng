#ifndef HANDLE_SFV
#define HANDLE_SFV

#include "objects.h"
#include "handling.h"

extern int handle_sfv(HANDLER_ARGS *);
extern int handle_sfv32(HANDLER_ARGS *);
extern int __sfv_rar(HANDLER_ARGS *);
extern int __sfv_other(HANDLER_ARGS *);
extern int __sfv_audio(HANDLER_ARGS *);
extern int __sfv_video(HANDLER_ARGS *);
extern int __sfv_default(HANDLER_ARGS *);

#endif
