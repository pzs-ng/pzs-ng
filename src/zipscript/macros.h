#ifndef _MACROS_H_
#define _MACROS_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <dirent.h>
#define NAMLEN(dirent) strlen((dirent)->d_name)
/*#else
#define dirent direct
#define NAMLEN(dirent) (dirent)->d_namlen
#if HAVE_SYS_NDIR_H
#include <sys/ndir.h>
#endif
#if HAVE_SYS_DIR_H
#include <sys/dir.h>
#endif
#if HAVE_NDIR_H
#include <ndir.h>
#else
#endif*/

/* General */
#if ( incompleteislink == 1 )
#if ( userellink == 1 )
#define create_incomplete() symlink(g.v.misc.release_name, g.l.incomplete)
#define create_incomplete2() symlink(g->v.misc.release_name, g->l.incomplete)
#define create_incomplete_nfo() symlink(g.v.misc.release_name, g.l.nfo_incomplete)
#define create_incomplete_nfo2() symlink(g->v.misc.release_name, g->l.nfo_incomplete)
#else
#define create_incomplete() symlink(g.l.path, g.l.incomplete)
#define create_incomplete2() symlink(g->l.path, g->l.incomplete)
#define create_incomplete_nfo() symlink(g.l.path, g.l.nfo_incomplete)
#define create_incomplete_nfo2() symlink(g->l.path, g->l.nfo_incomplete)
#endif
#else
#define create_incomplete() createzerofile(g.l.incomplete)
#define create_incomplete2() createzerofile(g->l.incomplete)
#define create_incomplete_nfo() createzerofile(g.l.nfo_incomplete)
#define create_incomplete_nfo2() createzerofile(g->l.nfo_incomplete)
#endif

/* No race */
#if ( announce_norace == 1 )
#define CHOOSE(a, b, c)	a > 1 ? b : c
#else
#define CHOOSE(a,b,c)	b;
#endif

#endif
