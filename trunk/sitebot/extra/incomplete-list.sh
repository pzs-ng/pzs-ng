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

# set this to 1 if you wish to announce sections where no incompletes are found.
verbose=0

#bold char
bold=""


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
   for path in `compgen -G $secpath`; do
    results="`$cleanup $glroot 2>/dev/null | grep -e "^Incomplete" | tr '\"' '\n' | grep -e "$path" | tr -s '/'`"
    if [ -z "$results" ]; then
      if [ $verbose -eq 1 ]; then
        echo "$secname: No incomplete releases found."
      fi
    else
      for result in $results; do
        secrel="`echo $result | sed "s|$glroot$path||" | tr -s '/'`"
        echo "$secname: ${bold}${secrel}${bold} is incomplete."
      done
    fi
   done
 done
done
IFS="$IFSORIG"
