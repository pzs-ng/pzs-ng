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

# set this to 1 if you wish to announce sections no incompletes are found.
verbose=0


#############################
# END OF CONFIG             #
# ^^^^^^^^^^^^^             #
#############################

for section in $sections; do
  secname="`echo $section | cut -d ':' -f 1`"
  secpath="`echo $section | cut -d ':' -f 2`"

  results="`$cleanup /glftpd 2>/dev/null | grep -e "^Incomplete" | tr '\"' '\n' | grep -e "$secpath"`"
  if [ -z "$results" ]; then
    if [ $verbose -eq 1 ]; then
      echo "$secname: No incomplete releases found."
    fi
  else
    for result in $results; do
      secrel="`echo $result | sed "s|$secpath||"`"
      echo "$secname: $secrel is incomplete."
    done
  fi
done

