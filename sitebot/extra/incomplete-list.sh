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
# <announce name of section>:<chrooted path to section, space separates paths>
# newline separates sections.
sections="
0DAY:/site/incoming/0day/
GAMES:/site/incoming/games/
APPS:/site/incoming/apps/
MV:/site/incoming/musicvideos/
"

# alternative, set the following variable to point to your ngBot.conf or
# other file where you have your section paths defined and uncomment the
# "botconf=" directive below.
#botconf=/path/to/sitebot/pzs-ng/ngBot.conf

# set this to 1 if you wish to announce sections where no incompletes are found.
verbose=0

#bold char
bold=""

# set this to zero if you have sections in subdirs of one another - ie,
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
cleanupresults="`$cleanup $glroot 2>/dev/null | grep -e "^Incomplete" | tr '\"' '\n' | tr -s '/'`"
for section in $sections; do
  secname="`echo "$section" | cut -d ':' -f 1`"
  secpaths="`echo "$section" | cut -d ':' -f 2- | tr ' ' '\n'`"

  for secpath in $secpaths; do
    results="`echo "$cleanupresults" | grep -e "$glroot$secpath"`"
    if [ -z "$results" ]; then
      if [ $verbose -eq 1 ]; then
        echo "$secname: No incomplete releases found."
      fi
    else
      for result in $results; do
        percent="`ls -1 "$result/" | sed -n 's/[^0-9]*\([0-9][0-9]*%\).*/\1/p'` complete"

        if [ $percent == " complete" ]; then
           percent="incomplete"
        fi

        # check for overlapping section paths
        # so we don't announce twice
        pass=1
        if [ $no_strict -eq 0 ]; then
          for sctn in $sections; do
            scpths="`echo "$sctn" | cut -d ':' -f 2- | tr ' ' '\n'`"
            for scpth in $scpths; do
              if [ "$secpath" != "$scpth" ] && [ "`echo "$scpth" | grep "^$secpath/"`" ]; then
                pass=0
                break
              fi
            done
            if [ $pass -eq 0 ]; then
              break
            fi
          done
        fi

        if [ $no_strict -eq 1 ] || [ $pass -eq 1 ]; then
          sedpath=`echo "$glroot$secpath/" | tr -s '/'`
          secrel="`echo $result | sed "s|$sedpath||" | tr -s '/'`"
          echo "$secname: ${bold}${secrel}${bold} is $percent."
        fi
      done
    fi
 done
done
echo "No more incompletes found."
IFS="$IFSORIG"
