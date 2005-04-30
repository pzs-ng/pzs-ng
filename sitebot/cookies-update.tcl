#!/usr/bin/tclsh

source dZSbot.conf.defaults
source dZSbot.conf.dist
source dZSbot.vars

set handle [open "COOKIES" w]
fconfigure $handle -translation {auto lf}

puts $handle "###############################################################
# dZSbot Cookie Reference                                     #
###############################################################

# Notes
###############################################################

Prefixes:
- %u_ for user-based cookies.
- %g_ for group-based cookies.
- %t_ for totals.

Path Parsing:

- When dZSbot recieves the %pf cookie, its value is parsed into four seperate
  cookies: %reldir, %relfull, %relname and %relpath.

  %reldir  = Base directory.
  %relfull = Path to base directory.
  %relname = Release name and subdirectory.
  %relpath = Path to release name.

  For example:

- /APPS/2005/Software.Release-NG/CD1/

  %reldir  = CD1
  %relfull = APPS/2005/Software.Release-NG
  %relname = Software.Release-NG (CD1)
  %relpath = APPS/2005

- /MP3/0101/Music.Release-NG/

  %reldir  = Music.Release-NG
  %relfull = MP3/0101
  %relname = Music.Release-NG
  %relpath = MP3/0101

- /MOVIES/Movie.Release-NG/CD1

  %reldir  = CD1
  %relfull = MOVIES/Movie.Release-NG
  %relname = Music.Release-NG (CD1)
  %relpath = MOVIES

- As you can see, these four cookies are best used in pairs, %reldir
  and %relfull or %relname and %relpath.

# Cookies
###############################################################
"

foreach var [lsort -ascii [array names "variables"]] {
	if {[lindex $variables($var) 0] == "%pf"} {
		set variables($var) [lreplace $variables($var) 0 0]
	}
	puts $handle "- $var:"
	set vars [split $variables($var)]
	regsub -all {[{}]} $vars "" vars
	while {[llength $vars]} {
		set str [lindex $vars 0]
		set vars [lreplace $vars 0 0]
		while {[string length $str] < 80 && [llength $vars]} {
			set str [concat "$str," [lindex $vars 0]]
			set vars [lreplace $vars 0 0]
		}
		puts $handle $str
	}
	puts $handle ""
}
