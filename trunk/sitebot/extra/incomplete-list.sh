#!/bin/bash


##############################
# CONFIG                     #
# ^^^^^^                     #
##############################

# enter path glftpd is installed in.
glroot=/glftpd

# enter path to the cleanup binary.
cleanup=$glroot/bin/cleanup

# enter sections in the following format:
# <announce name of section>:<path to section, including a terminating slash ``/''
# spaces and newline separates.
sections="
0DAY:/site/incoming/0day/
GAMES:/site/incoming/games/
APPS:/site/incoming/apps/
MV:/site/incoming/musicvideos/
"

# alternative, set the following variable to point to your dZSbot.conf and
# uncomment the ''botconf='' directive below.
#botconf=/path/to/sitebot/pzs-ng/dZSbot.conf

# Set this to your complete line (non-dynamic part)
releaseComplete="Complete -"

# set this to 1 if you wish to announce sections where no incompletes are found.
verbose=0

#bold char
bold=""

# set this to one if you have sections in subdirs of one another - ie,
# if you have defined in $sections "A:/site/DIR" and "B:/site/DIR/SUBDIR"
no_strict=0

#############################
# END OF CONFIG             #
# ^^^^^^^^^^^^^             #
#############################

# grab sections from the sitebot's conf instead
if [ ! -z "$botconf" ] && [ -e "$botconf" ]; then
 sections="`grep "^set paths(" $botconf | sed 's/^set paths(\(.*\))[[:space:]]\{1,\}\"\(.*\)\*\"/\1:\2/'`"
fi;

IFSORIG="$IFS"
IFS="
"
for section in $sections; do
  secname="`echo "$section" | cut -d ':' -f 1`"
  secpaths="`echo "$section" | cut -d ':' -f 2- | tr ' ' '\n'`"

  for secpath in $secpaths; do
    results="`$cleanup $glroot 2>/dev/null | grep -e "^Incomplete" | tr '\"' '\n' | grep -e "$secpath" | tr -s '/'`"
    if [ -z "$results" ]; then
      if [ $verbose -eq 1 ]; then
        echo "$secname: No incomplete releases found."
      fi
    else
      for result in $results; do
        secrel="`echo $result/ | sed "s|$glroot$path||" | tr -s '/'`"
        comp="`ls -1 $result/ | grep "$releaseComplete"`"
        percent="`echo $comp | cut -d ' ' -f 4` complete"

        if [ $percent == " complete" ]; then
           percent="incomplete"
        fi

	if [ $no_strict ] || [ "`dirname $secrel`/" = "`echo $secpath/ | tr -s '/'`" ]; then 
          echo "$secname: ${bold}${secrel}${bold} is $percent."
	fi
      done
    fi
 done
done
echo "No more incompletes found."
IFS="$IFSORIG"
