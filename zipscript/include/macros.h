#ifndef _MACROS_H_
#define _MACROS_H_

/* FreeBSD */
#ifdef __FreeBSD__
#define D_NAMLEN(d)	((d)->d_namlen)
#endif

/* OpenBSD */
#ifdef __OpenBSD__
#define D_NAMLEN(d)	((d)->d_namlen)
#endif

/* Linux */
#ifdef __linux__
#define D_NAMLEN(d)	_D_EXACT_NAMLEN(d)
#endif


/* absolute symlinks - obsolete*/
/*symlink(locations.path, locations.incomplete)*/

/* General */
#if ( incompleteislink == 1 )
# define create_incomplete() symlink(raceI.misc.release_name, locations.incomplete);
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

