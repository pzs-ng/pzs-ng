#ifndef _MACROS_H_
#define _MACROS_H_

#if HAVE_DIRENT_H
# include <dirent.h>
# define NAMLEN(dirent) strlen((dirent)->d_name)
#else
# define dirent direct
# define NAMLEN(dirent) (dirent)->d_namlen
# if HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# if HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# if HAVE_NDIR_H
#  include <ndir.h>
# else
#  error "No dirent header, quitting."
# endif
#endif

/* General */
#if ( incompleteislink == 1 )
# if ( userellink == 1 )
#  define create_incomplete() symlink(raceI.misc.release_name, locations.incomplete)
# else
#  define create_incomplete() symlink(locations.path, locations.incomplete)
# endif
#else
# define create_incomplete() createzerofile(locations.incomplete)
#endif


/* No race */
#if ( announce_norace == 1 )
# define CHOOSE(a,b,c)	raceI.total.users > 1 ? b : c;
#else
# define CHOOSE(a,b,c)	b;
#endif

#ifndef alloca
# define m_alloc(x) malloc(x)
# define m_free(p) free(p)
#else
# define m_alloc(x) alloca(x)
# define m_free(p)
#endif

#endif

