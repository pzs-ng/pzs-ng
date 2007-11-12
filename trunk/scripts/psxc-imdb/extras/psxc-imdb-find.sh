#!/bin/bash


##################################################
# PSXC-IMDB-FIND
################
#
# An addon to psxc-imdb.
# Please read the README, then edit config below.
#
# History:
# v0.1  Initial Release
# v2.3  Added a bit more fuzzy search
#       Changed lookup url
#       Added timeout setting to wget
#       Did a jump in version number, since this
#        script is now a part of psxc-imdb.
#       Added better control on arguments passed.
# v2.3f Added support for iMDB IDs.
# v2.5  Fixed a bug with international versions
#        of lynx.
#       Now shows with local url by default.
#       Added support for listing hits. (-l)
#       Added support for disabling url catching.
#       Added support for private lookups.
# v2.5c Now logs data in debug-mode.
# v2.7  Added support for output destination
#        (user or channel)
# v2.7c imdb.com changed url-style. Changed script
#        to match.
# v2.8  imdb.com changed html layout. Changed
#	 script to match.
##################################################

########
# CONFIG

# Version. No need to change.
VERSION=2.8

# (full) path to psxc-imdb.conf
PSXC_IMDB_CONF=/glftpd/etc/psxc-imdb.conf

# max hits listed
MAXLIST=10

# default number of hits listed
DEFLIST=5

# bold char - set to "" to disable.
BOLD=""

# different (mirc) colors.
WHITE="00"
BLACK="01"
DARKBLUE="02"
GREEN="03"
RED="04"
DARKRED="05"
DARKPURPLE="06"
ORANGE="07"
YELLOW="08"
LIGHTGREEN="09"
BLUEGREEN="10"
CYAN="11"
BLUE="12"
PURPLE="13"
GREY="14"
LIGHTGREY="15"

# color off
COLOROFF=""

# Word before text output
PREWORD="$BOLD""IMDB:""$BOLD"
#PREWORD="${BOLD}${RED}[${BLUE}HUBBA-BUBBA${RED}]${COLOROFF} :${BOLD}"

# Verbose mode. Default is on ("").
VERBOSE=""
#VERBOSE="OFF"

# OBS! Please - turn off DEBUG! It should only be used if you have problems
#      Having DEBUG on may cause problems!

# temp dir - should exist inside and outside chroot and must be 777
TEMPDIR=/tmp

# END OF CONFIG
###############

DESTINATION=$1
PSXCFINDTEMPA=$TEMPDIR/PSXCFINDTEMPA.tmp$$
PSXCFINDTEMPB=$TEMPDIR/PSXCFINDTEMPB.tmp$$
PSXCFINDTEMPC=$TEMPDIR/PSXCFINDTEMPC.tmp$$
[[ ! -d $TEMPDIR || ! -w $TEMPDIR ]] && {
	echo "Error - could not find/write to tempdir"
	exit 1
}
shift
IMDBSEARCHORIGA=`echo -n "$@" | tr -cd 'A-Za-z0-9\-\,+\=\.\ '`
IMDBSEARCHORIG="`echo $IMDBSEARCHORIGA | tr ' \.' '\n' | grep -v "^-" | grep -v "^$" | tr '\n' ' '`"
if [ -z "$IMDBSEARCHORIG" ]; then
 echo "$PREWORD psxc-imdb channel trigger v$VERSION - argument(s) missing."
 echo "$PREWORD   use ${BOLD}-f${BOLD}   to make a ${BOLD}f${BOLD}uzzy (alternative) search."
 echo "$PREWORD   use ${BOLD}-lXX${BOLD} to ${BOLD}l${BOLD}ist ${BOLD}XX${BOLD} matches (max $MAXLIST, default $DEFLIST)."
 echo "$PREWORD   use ${BOLD}-n${BOLD}   to ${BOLD}n${BOLD}ot search for imdb ID's in the search-string."
 echo "$PREWORD   /msg <botname> <search> will give the results in private."
 echo "$PREWORD   search-words can be separated by spaces or dots."
 echo "$PREWORD   words starting with '-' are ignored."
 exit 0
fi
IMDBFUZZ=""
if [ ! -z "`echo $IMDBSEARCHORIGA | grep -e "\-[fF]"`" ]; then
 IMDBFUZZ="&type=fuzzy"
fi
IMDBNOURL=""
if [ ! -z "`echo $IMDBSEARCHORIGA | grep -e "\-[nN]"`" ]; then
 IMDBNOURL="ON"
fi
IMDBPRIVATE=""
#if [ ! -z "`echo $IMDBSEARCHORIGA | grep -e "\-[pP]"`" ]; then
# IMDBPRIVATE="ON"
#fi
 IMDBLIST=""
if [ ! -z "`echo $IMDBSEARCHORIGA | grep -e "\-[lL]"`" ]; then
 IMDBLIST="`echo $IMDBSEARCHORIGA | tr ' ' '\n' | grep -e "\-[lL]" | head -n 1 | tr -cd '0-9'`"
 if [ -z "$IMDBLIST" ]; then
  IMDBLIST=$DEFLIST
 else
  if [ $IMDBLIST -eq 0 ]; then
   IMDBLIST=$DEFLIST
  elif [ $IMDBLIST -gt $MAXLIST ]; then
   IMDBLIST=$MAXLIST
  fi
 fi
fi

IMDBSEARCHORIG="`echo $IMDBSEARCHORIGA | tr ' \.' '\n' | grep -v "^-" | grep -v "^$" | tr '\n' ' '`"
if [ -z "$IMDBSEARCHORIG" ]; then
 echo "$PREWORD psxc-imdb channel trigger v$VERSION - please add something to search for."
 exit 0
fi
URLTOUSE=""
IMDBSEARCHWORDS="`echo $IMDBSEARCHORIG`"
IMDBSEARCHCNT=`echo $IMDBSEARCHORIG | tr '-' '\n' | wc | awk '{print $1}'`
let IMDBSEARCHCNT=IMDBSEARCHCNT-1
if [ $IMDBSEARCHCNT -lt 1 ]; then
 IMDBSEARCHCNT=1
fi
IMDBSEARCHTITLA=`echo "$IMDBSEARCHORIG" | cut -d '-' -f 1-$IMDBSEARCHCNT | tr ' ' '\n' | grep -iv "^custom$" | grep -iv "^dvd" | grep -iv "^screener" | grep -iv "vcd" | grep -iv "divx" | grep -iv "xvid" | grep -iv "^ts$" | grep -iv "telesync" | grep -iv "^tc$" | grep -iv "telecine" | grep -iv "^proper$" | grep -iv "limited" | grep -iv "^subbed$" | grep -iv "^read$" | grep -iv "^nfo$" | grep -iv "internal" | grep -iv "workprint" | grep -iv "^wp$" | tr '\n' ' '`
IMDBSEARCHTITLE="`echo $IMDBSEARCHTITLA | tr ' -' '+'`""$IMDBFUZZ"
IMDBSEARCHTITLB=`echo $IMDBSEARCHTITLA`

. $PSXC_IMDB_CONF

IMDBLOCAL="$LOCALURL"
if [ -z "$IMDBLOCAL" ]; then
 IMDBLOCAL="www"
fi

if [ -z "$IMDBNOURL" ]; then
 for SEARCHWORD in $IMDBSEARCHWORDS; do
  IMDBSEARCHID="`echo -n "$SEARCHWORD" | tr -cd '0-9'`"
  if [ ! -z "$IMDBSEARCHID" ]; then
   if [ `echo -n "$IMDBSEARCHID" | wc -c` -ge 6 ] && [ `echo -n "$IMDBSEARCHID" | wc -c` -le 8 ]; then
    URLTOUSE="http://""$IMDBLOCAL"".imdb.com/title/tt""$IMDBSEARCHID"
    break
   fi
  fi
 done
fi

MYLYNXFLAGS=`echo $LYNXFLAGS | sed "s| -nolist||"`
if [ -z "$URLTOUSE" ]; then
 CONTENT=`lynx $MYLYNXFLAGS http://former.imdb.com/Tsearch?title=$IMDBSEARCHTITLE 2>/dev/null`
 if [ $? -gt 0 ]; then
  echo "$PREWORD Internal Error. www.imdb.com may be down, or not answering. Try again later."
  exit 0
 fi

:>$PSXCFINDTEMPA
:>$PSXCFINDTEMPB
:>$PSXCFINDTEMPC

echo "$CONTENT" >$PSXCFINDTEMPA

found=0
linknumber=0
while read a; do
	b="$(echo "$a" | tr 'a-zA-Z' ' ' | cut -d ' ' -f 1 | grep -v "^\[" | grep "\.")"
	[[ ! -z "$b" && found -eq 0 ]] && {
		echo "$a" >$PSXCFINDTEMPB
		found=1
		let linknumber=linknumber+1
		continue
	}
	[[ ! -z "$b" && found -eq 1 ]] && {
		while read c; do
			[[ "$(echo "$c" | grep -v "\[.*gif\]" | grep "\[")" ]] && {
				echo "$linknumber. $c" >>$PSXCFINDTEMPC
			}
		done <$PSXCFINDTEMPB
		echo "$a" >$PSXCFINDTEMPB
		found=1
		let linknumber=linknumber+1
		continue
	}	
	echo "$a" >>$PSXCFINDTEMPB
done <$PSXCFINDTEMPA


 if [ -z "$IMDBLIST" ]; then
  LINKNO=`cat "$PSXCFINDTEMPC" | head -n 1 | cut -d "[" -f 2 | cut -d "]" -f 1`
  if [ ! -z "$LINKNO" ]; then

#echo "$LINKNO"

   URLTOUSE=`cat "$PSXCFINDTEMPA" | grep -e "\ $LINKNO\.\ " | tail -n 3 | tr ' ' '\n' | grep -e "imdb" | grep -e "title/tt" | tr '\?' '\n' | head -n 1`

#echo "$URLTOUSE"

  else
   URLTOUSE=""
  fi
 else
  a=1
  b=1
  URLS=""
  while [ $a -le $IMDBLIST ]; do
   LINKNAME=`cat "$PSXCFINDTEMPC" | grep -e "^$a\.\ " | head -n 1 | cut -d "[" -f 2 | cut -d "]" -f 2`
   LINKNO=`cat "$PSXCFINDTEMPC" | grep -e "^$a\.\ " | head -n 1 | cut -d "[" -f 2 | cut -d "]" -f 1`
   if [ ! -z "$(echo $LINKNO | tr -d ' ')" ]; then
    URLTOUSE=`cat "$PSXCFINDTEMPA" | grep -e "\ $LINKNO\.\ " | tail -n 1 | tr ' ' '\n' | grep -e "imdb" | grep -e "title/tt" | tr '\?' '\n' | head -n 1`
    if [ ! -z "$URLTOUSE" ] && [ -z "`echo "$URLS" | grep -e "$URLTOUSE"`" ]; then
     if [ $a -eq 1 ]; then
      echo "$PREWORD Listing up to $IMDBLIST hits (duplicates removed)..."
      URLORIG="$URLTOUSE"
     fi
     echo "$PREWORD $b"". ( ""$URLTOUSE"" ) $LINKNAME" | sed "s|/former.|/$IMDBLOCAL.|"
     let b=b+1
    fi
   URLS="$URLS $URLTOUSE"
   fi
   let a=a+1
  done
  if [ $b -gt 2 ]; then
   exit 0
  fi
  URLTOUSE="$URLORIG"
 fi

# Just in case there's only one hit, imdb redirects us to the page. this will check to see if this is the case.
 if [ -z "$URLTOUSE" ]; then
  WGETOUT=`wget -U "Internet Explorer" -O /dev/null --timeout=10 http://former.imdb.com/Tsearch?title=$IMDBSEARCHTITLE 2>&1`
  URLTOUSE=`echo "$WGETOUT" | tr ' ' '\n' | grep -e "imdb" | tr '><&' '\n' | grep -i -e "\/title\/" | tr '\?' '\n' | head -n 1`
 fi
fi
rm -f $PSXCFINDTEMPA
rm -f $PSXCFINDTEMPB
rm -f $PSXCFINDTEMPC
if [ ! -z "$URLTOUSE" ]; then
 URLTOSHOW=`echo $URLTOUSE | sed "s|/former.|/$IMDBLOCAL.|"`
 if [ -z "$VERBOSE" ] && [ -z "$IMDBPRIVATE" ]; then
  if [ -z "$IMDBLIST" ]; then
   echo -n "$PREWORD '$IMDBSEARCHTITLB' found @ ""$BOLD""$URLTOSHOW""$BOLD"". "
  else
   echo -n "$PREWORD Only one hit found - "
  fi
  echo "Please wait while gathering details.."
 fi
 if [ ! -z "$DEBUG" ]; then
  echo "$URLTOUSE|/dev/null|$DESTINATION" | sed "s%/|%|%"
 fi
 if [ -z "$IMDBPRIVATE" ]; then
  echo "$URLTOUSE|/dev/null|$DESTINATION" | sed "s%/|%|%" >>$IMDBLOG
 fi
else
 echo "$PREWORD Sorry, nothing found on '""$BOLD""$IMDBSEARCHWORDS""$BOLD""'."
fi
exit 0

