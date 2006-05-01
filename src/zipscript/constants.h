#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#ifdef HAVE_LIMITS_H
# ifndef _LIMITS_H_
#  define _LIMITS_H_
#  include <limits.h>
# endif
#endif

#ifdef HAVE_SYS_LIMITS_H
# include <sys/limits.h>
#endif

#ifdef HAVE_SYSLIMITS_H
# include <syslimits.h>
#endif

#ifndef PATH_MAX
# define PATH_MAX 1024
# define _ALT_MAX
#endif

#ifndef NAME_MAX
# define NAME_MAX 255
# ifndef _ALT_MAX
#  define _ALT_MAX
# endif
#endif

#if NAME_MAX%4
# define NAMEMAX NAME_MAX+4-NAME_MAX%4
#else
# define NAMEMAX NAME_MAX
#endif

#endif /* _CONSTANTS_H_ */
