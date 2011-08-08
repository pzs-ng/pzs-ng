#!/bin/bash

# psxc-symlink-maker
# an example addon for psxc-imdb
# this addon first appeared in psxc-imdb v2.0j
#
# in order for this to work, you should copy this file to
# /glftpd/bin and change the variable EXTERNALSCRIPTNAME
# in psxc-imdb.conf to "$GLROOT/bin/psxc-symlink-maker.sh"
###########################################################

# Config.
# After you're done configging, run the script from shell. It will test a
# few variables/settings and report back to you.
# You can also put the config here in psxc-imdb.conf - the variables there
# will override the ones here. Good to know for future updates. :)
############################################################################

# version number. no need to change
VERSION=2.9i

# The location of psxc-imdb.conf. This is the full path.
IMDB_CONF=/glftpd/etc/psxc-imdb.conf

# Where should symlinks be put? This is relative to $GLROOT.
SYMLINK_PATH=/site/MOVIES_SORTED

# What should we sort after? 0 disables, 1 enables.
# Genre is *all* genres listed. Year is, well, the year. Score is based on
# the score without decimal (0 to 10). Title is based on the first letter/number
# in the title. Group is releasegroup, and date is date the release was uploaded.
SORT_BY_GENRE=1
SORT_BY_YEAR=1
SORT_BY_SCORE=1
SORT_BY_TITLE=0
SORT_BY_GROUP=0
SORT_BY_DATE=0
SORT_BY_TOP250=0
SORT_BY_KEYWORD=0
SORT_BY_LANGUAGE=1
SORT_BY_DIRECTOR=1
SORT_BY_CASTLEADNAME=0
SORT_BY_CASTING=0

# Here we specify the name of the dirs for the sorted releases.
SORT_BY_GENRE_NAME="Sorted.by.Genre"
SORT_BY_YEAR_NAME="Sorted.by.Year"
SORT_BY_SCORE_NAME="Sorted.by.Score"
SORT_BY_TITLE_NAME="Sorted.by.Title"
SORT_BY_GROUP_NAME="Sorted.by.Group"
SORT_BY_DATE_NAME="Sorted.by.Date"
SORT_BY_TOP250_NAME="Sorted.by.Top250"
SORT_BY_KEYWORD_NAME="Sorted.by.%KEYWORD%"
SORT_BY_LANGUAGE_NAME="Sorted.by.Language"
SORT_BY_DIRECTOR_NAME="Sorted.by.Director"
SORT_BY_CASTLEADNAME_NAME="Sorted.by.Leading.Actor"
SORT_BY_CASTING_NAME="Sorted.by.Actors"

# If you wish to use SORT_BY_DATE, enter here the format you wish to use.
# This will be prefixed the name of the release. Please take note that this
# is the date this script is run, not when the dir was created (In case
# you're thinking of doing a rescan, this date will definitely be wrong).
#SORT_BY_DATE_FORMAT="`date +"%Y.%m.%d-%H%M-.-"`"
SORT_BY_DATE_FORMAT=""

# If you use the auto-date feature, please take care to check this variable -
# 'ls' differ on platforms, so you need to put this right.
# for bsd the normal approach is to use 'ls'. on linux, 'ls' is too strange to
# use, so we use gnu's 'find' instead.
SORT_BY_DATE_LS="bsd"  # for the bsd's
#SORT_BY_DATE_LS="gnu"  # this works on linux (gentoo at least)

# Sorting by keywords require, eh, keywords. ;) You may have noticed the strange
# name in SORT_BY_KEYWORD_NAME - we will replace the %KEYWORD% with something of
# your choice here. Keywords are searched for in the dirname.
# Do not use dots (.), spaces ( ), dashes (-) or underscores (_) in your keywords.
# It goes like this - you enter a keyword to search for, followed by a '|'
# (pipe), followed by what you wish the %KEYWORD% to be replaced with. Spaces
# separate keywords. If you wish to search for several keywords and put them in
# the same dir, you can do that. The search keyword is case-insensitive.
SORT_BY_KEYWORD_LIST="german|German.Movies divx|DivX xvid|DivX"

# When sorting by groups, what should be used if no groupname is present.
# Do not leave empty or use /, . or ..
SORT_BY_GROUP_NONE="#NONE#"

# Groups having a dash (-) in their name would produce wrong results
# when sorting by groups. List here those groups.
SORT_BY_GROUP_SPECIAL="VH-PROD|DVD-R"

# When sorting by score, if there is no score yet (awaiting 5 votes),
# what should be used to sort under. Do not leave empty or use /, . or ..
SORT_BY_SCORE_NONE="NA"

# Dirs that are sorted under a single char and starting with a dot (.) will be
# stored under the first char being . which means the SORT_BY_TITLE_NAME rootdir.
# Give here a dirname to sort it properly under, not being /, ., .. or empty.
SORT_BY_CHAR_DOT="DOT"

# List a dir where dirs sorted under a single char should be stored when starting
# with a char not being "A-Za-z0-9_\-().". Do not use /, . or .., or
# leave this empty if you do want to use the special char.
SORT_BY_CHAR_OTHER=""

# Titles, directors, castnames, ... can have chars not being "A-Za-z0-9_\-()."
# in them, do you want to allow those, or have them substituted by a given char?
# Leave empty to allow special chars, else give a single char to replace with.
# Do not use / or %.
SPECIAL_CHAR_REPLACER=""

# What to replace spaces with in created links. Leave empty or put a space
# to keep spaces. Use a single char and do not use / or %.
SPACE_REPLACER="_"

# List here chars that can not be in linknames, ie those restricted by your os.
BADCHARS="/"

# What should BADCHARS be replaced with? Do not leave empty or use / or %.
BAD_CHAR_REPLACER="-"

# Clean up dead symlinks after each run? Usually, this is done pretty quick,
# but can take time, so use the trial and error method on this ;)
# Please note that only the used SORT_BY cathegories is being scanned for
# dead symlinks.
# It is reccomended to do this as a crontab job every 30 minutes or so,
# or to use a dedicated dead symlink remover instead of doing it after each
# imdb lookup.
# So why does it take so long? Because all symlinks are chrooted - testing
# for dead ones require a lot of tests and checks.
CLEANUP_SYMLINKS=0
#CLEANUP_SYMLINKS=1

# End of config
############################################################

# First, let's grab the variables found in psxc-imdb.conf.
# Yes, the dot (.) in front is correct - do not remove it.
# If you put the above variables in psxc-imdb.conf, they will override what is
# put in this file. Should be helpful if you want all variables to be in one
# place.
. $IMDB_CONF

# The following is the routine to grab variables from psxc-imdb. It's a copy
# of the code in psxc-imdb-parser.sh in the /extras dir.

IFSORIG=$IFS
IFS="^"

# Initialize variables. bash is a bit limited, so we gotta do a "hack"
c=1
for a in `echo $@ | sed "s/^\"//;s/\"$//;s|\" \"|^|g"`; do
b[c]=$a
let c=c+1
done

IFS=$IFSORIG

# Give the variables some sensible names
IMDBDATE=${b[1]}
IMDBDOTFILE=${b[2]}
IMDBRELPATH=${b[3]}
IMDBDIRNAME=${b[4]}
IMDBURL=${b[5]}
IMDBTITLE=${b[6]}
IMDBGENRE=${b[7]}
IMDBRATING=${b[8]}
IMDBCOUNTRY=${b[9]}
IMDBLANGUAGE=${b[10]}
IMDBCERTIFICATION=${b[11]}
IMDBRUNTIME=${b[12]}
IMDBDIRECTOR=${b[13]}
IMDBBUSINESSDATA=${b[14]}
IMDBPREMIERE=${b[15]}
IMDBLIMITED=${b[16]}
IMDBVOTES=${b[17]}
IMDBSCORE=${b[18]}
IMDBNAME=${b[19]}
IMDBYEAR=${b[20]}
IMDBNUMSCREENS=${b[21]}
IMDBISLIMITED=${b[22]}
IMDBCASTLEADNAME=${b[23]}
IMDBCASTLEADCHAR=${b[24]}
IMDBTAGLINE=${b[25]}
IMDBPLOT=${b[26]}
IMDBBAR=${b[27]}
IMDBCASTING=${b[28]}
IMDBCOMMENTSHORT=${b[29]}
IMDBCOMMENTFULL=${b[30]}

###### Let's start

# First, a couple of test to make sure we got the variables. These should be better.
[[ ! -z "`echo "$SYMLINK_PATH" | grep -v "/"`" ]] &&
 echo "config error. check variables." &&
 exit 0

# Let's check the if SYMLINK_PATH exist.
if [ ! -d "$GLROOT$SYMLINK_PATH" ]; then
 mkdir "$GLROOT$SYMLINK_PATH" >/dev/null 2>&1
 [[ $? -ne 0 ]] &&
  echo "Could not create $GLROOT$SYMLINK_PATH. Exiting." &&
  exit 0
 chmod 777 "$GLROOT$SYMLINK_PATH" >/dev/null 2>&1
 [[ $? -ne 0 ]] &&
  echo "Could not chmod 777 $GLROOT$SYMLINK_PATH. Exiting." &&
  exit 0
fi

# Make sure we are able to write in SYMLINK_PATH
[[ ! -w "$GLROOT$SYMLINK_PATH" ]] &&
 echo "Unable to write to $GLROOT$SYMLINK_PATH. Exiting." &&
 exit 0

# If SPACE_REPLACER is empty, we put in a space
[[ -z "$SPACE_REPLACER" ]] && SPACE_REPLACER=" "

if [ -z "$IMDBRELPATH" ]; then
 echo "Seems to me like you've run this script in standalone mode."
 echo "That's fine if you wish to clean up old symlinks. If this was not your"
 echo "intention, I suggest you read the docs on how to set this up."
 echo "I will now check for dead links, ignoring the CLEANUP_SYMLINKS flag."
 echo "You can take advantage of this if you like, by adding a crontab entry"
 echo "and setting CLEANUP_SYMLINKS=0. The addon will be faster..."
 echo "A crontab entry can look like this (running every 30 mins):"
 echo "7,37 * * * * /glftpd/bin/psxc-symlink-maker.sh >/dev/null 2>&1"
 echo ""
 echo "Please wait while I check for dead links..."
 echo ""
 CLEANUP_SYMLINKS=1
fi

# finally, let's see if the release still exists. Maybe it's already been
# moved by a different script?
[[ ! -e "$GLROOT/$IMDBRELPATH" ]] && exit 0

##### From here on I will assume the needed bins exists, and the correct permissions
##### are set. The above should've catched most of such errors.

# First, let's make sure we're not in an affil/pre/group/private dir
ISEXEMPT=0
if [ ! -z "$BOTEXEMPT" ]; then
 for EXEMPT in $BOTEXEMPT; do
  if [ ! -z "`echo "$IMDBRELPATH" | grep -e "$EXEMPT"`" ]; then
   ISEXEMPT=1
   break
  fi
 done
fi

## Cleanup proc for structures like SORT_BY_*_NAME/LINK
proc_cleanup() {
 for LINK in `ls -AF "$1" | tr ' ' '%'`; do
  LINK="`echo "$LINK" | sed "s/@$//" | tr '%' ' '`"
  LINK_DST="`ls -Al "$1/$LINK" | cut -d'>' -f2 | sed "s/^ *//"`"
  [[ ! -e "$GLROOT$LINK_DST" ]] &&
   rm -f "$1`basename "$LINK"`"
 done
}

## Cleanup proc for structures like SORT_BY_*_NAME/DIR/LINK
proc_cleanup_single() {
 for SDIR in `ls -AF "$1" | tr ' ' '%'`; do
  SDIR="`echo $SDIR | tr '%' ' '`"
  proc_cleanup "$1/$SDIR"
  rmdir "$1/$SDIR" >/dev/null 2>&1
 done
}

## Cleanup proc for structures like SORT_BY_*_NAME/DIR/DIR/LINK
proc_cleanup_double() {
 for DIR in `ls -AF "$1" | tr ' ' '%'`; do
  DIR="`echo $DIR | tr '%' ' '`"
  proc_cleanup_single "$1/$DIR"
  rmdir "$1/$DIR" >/dev/null 2>&1
 done
}

#################
# Genre Section #
#################

if [ $SORT_BY_GENRE -eq 1 ]; then
 [[ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_GENRE_NAME" ]] &&
  mkdir -pm777 "$GLROOT$SYMLINK_PATH/$SORT_BY_GENRE_NAME"

 # Cleanup Genre-dirs
 [[ $CLEANUP_SYMLINKS -eq 1 ]] &&
  proc_cleanup_single "$GLROOT$SYMLINK_PATH/$SORT_BY_GENRE_NAME/"

 # Make link if needed
 if [ ! -z "$IMDBGENRE" ] && [ ! $ISEXEMPT -eq 1 ]; then
  IMDBGENRES="`echo "$IMDBGENRE" | tr -s '/ ' ' '`"
  for GENRE in $IMDBGENRES; do
   GENRE=$(echo "$GENRE" | tr "$BADCHARS" "$BAD_CHAR_REPLACER")
   [[ ! -d  "$GLROOT$SYMLINK_PATH/$SORT_BY_GENRE_NAME/$GENRE" ]] &&
    mkdir -pm777 "$GLROOT$SYMLINK_PATH/$SORT_BY_GENRE_NAME/$GENRE"
   [[ ! -L "$GLROOT$SYMLINK_PATH/$SORT_BY_GENRE_NAME/$GENRE/$IMDBDIRNAME" ]] &&
    ln -s "$IMDBRELPATH" "$GLROOT$SYMLINK_PATH/$SORT_BY_GENRE_NAME/$GENRE/$IMDBDIRNAME"
  done
 fi
fi

####################
# Language Section #
####################

if [ $SORT_BY_LANGUAGE -eq 1 ]; then
 [[ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_LANGUAGE_NAME" ]] &&
  mkdir -pm777 "$GLROOT$SYMLINK_PATH/$SORT_BY_LANGUAGE_NAME"

 # Cleanup Language-dirs
 [[ $CLEANUP_SYMLINKS -eq 1 ]] &&
  proc_cleanup_single "$GLROOT$SYMLINK_PATH/$SORT_BY_LANGUAGE_NAME/"

 # Make link if needed
 if [ ! -z "$IMDBLANGUAGE" ] && [ ! $ISEXEMPT -eq 1 ]; then
  IMDBLANGUAGES="`echo "$IMDBLANGUAGE" | tr -s ' ' '_' | sed s/'_|_'/' '/g`"
  for LANGUAGE in $IMDBLANGUAGES; do
   LANGUAGE="$(echo "$LANGUAGE" | tr -s '_' "$SPACE_REPLACER" | tr "$BADCHARS" "$BAD_CHAR_REPLACER")"
   [[ ! -z "$SPECIAL_CHAR_REPLACER" ]] && LANGUAGE=$(echo "$LANGUAGE" | tr -sc 'A-Za-z0-9_\-(). \n' "$SPECIAL_CHAR_REPLACER")
   [[ ! -d  "$GLROOT$SYMLINK_PATH/$SORT_BY_LANGUAGE_NAME/$LANGUAGE" ]] &&
    mkdir -pm777 "$GLROOT$SYMLINK_PATH/$SORT_BY_LANGUAGE_NAME/$LANGUAGE"
   [[ ! -L "$GLROOT$SYMLINK_PATH/$SORT_BY_LANGUAGE_NAME/$LANGUAGE/$IMDBDIRNAME" ]] &&
    ln -s "$IMDBRELPATH" "$GLROOT$SYMLINK_PATH/$SORT_BY_LANGUAGE_NAME/$LANGUAGE/$IMDBDIRNAME"
  done
 fi
fi

####################
# Director Section #
####################

if [ $SORT_BY_DIRECTOR -eq 1 ]; then
 [[ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_DIRECTOR_NAME" ]] &&
  mkdir -pm777 "$GLROOT$SYMLINK_PATH/$SORT_BY_DIRECTOR_NAME"

 # Cleanup Director-dirs
 [[ $CLEANUP_SYMLINKS -eq 1 ]] &&
  proc_cleanup_double "$GLROOT$SYMLINK_PATH/$SORT_BY_DIRECTOR_NAME/"

 # Make link if needed
 if [ ! -z "$IMDBDIRECTOR" ] && [ ! $ISEXEMPT -eq 1 ]; then
  IMDBDIRECTORS="`echo "$IMDBDIRECTOR" | tr -s ' ' '_' | sed s/'_|_'/' '/g`"
  for DIRECTOR in $IMDBDIRECTORS; do
   if [ "${DIRECTOR:0:1}" == "." ]; then
    DIRECTORCHAR="$SORT_BY_CHAR_DOT"
   elif [ ! -z "`echo "${DIRECTOR:0:1}" | tr -d "A-Za-z0-9_\-()"`" ] && [ ! -z "$SORT_BY_CHAR_OTHER" ]; then
    DIRECTORCHAR="$SORT_BY_CHAR_OTHER"
   else
    DIRECTORCHAR=${DIRECTOR:0:1}
   fi
   DIRECTOR="$(echo "$DIRECTOR" | tr -s '_' "$SPACE_REPLACER" | tr "$BADCHARS" "$BAD_CHAR_REPLACER")"
   [[ ! -z "$SPECIAL_CHAR_REPLACER" ]] && DIRECTOR=$(echo "$DIRECTOR" | tr -sc 'A-Za-z0-9_\-(). \n' "$SPECIAL_CHAR_REPLACER")
   [[ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_DIRECTOR_NAME/$DIRECTORCHAR" ]] &&
    mkdir -pm777 "$GLROOT$SYMLINK_PATH/$SORT_BY_DIRECTOR_NAME/$DIRECTORCHAR"
   [[ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_DIRECTOR_NAME/$DIRECTORCHAR/$DIRECTOR" ]] &&
    mkdir -pm777 "$GLROOT$SYMLINK_PATH/$SORT_BY_DIRECTOR_NAME/$DIRECTORCHAR/$DIRECTOR"
   [[ ! -L "$GLROOT$SYMLINK_PATH/$SORT_BY_DIRECTOR_NAME/$DIRECTORCHAR/$DIRECTOR/$IMDBDIRNAME" ]] &&
    ln -s "$IMDBRELPATH" "$GLROOT$SYMLINK_PATH/$SORT_BY_DIRECTOR_NAME/$DIRECTORCHAR/$DIRECTOR/$IMDBDIRNAME"
  done
 fi
fi

#########################
# Leading Actor Section #
#########################

if [ $SORT_BY_CASTLEADNAME -eq 1 ]; then
 [[ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_CASTLEADNAME_NAME" ]] &&
  mkdir -pm777 "$GLROOT$SYMLINK_PATH/$SORT_BY_CASTLEADNAME_NAME"

 # Cleanup Castleadname-dirs
 [[ $CLEANUP_SYMLINKS -eq 1 ]] &&
  proc_cleanup_double "$GLROOT$SYMLINK_PATH/$SORT_BY_CASTLEADNAME_NAME/"

 # Make link if needed
 if [ ! -z "$IMDBCASTLEADNAME" ] && [ ! $ISEXEMPT -eq 1 ]; then
  if [ "${IMDBCASTLEADNAME:0:1}" == "." ]; then
   CASTLEADNAMECHAR="$SORT_BY_CHAR_DOT"
  elif [ ! -z "`echo "${IMDBCASTLEADNAME:0:1}" | tr -d "A-Za-z0-9_\-()"`" ] && [ ! -z "$SORT_BY_CHAR_OTHER" ]; then
   CASTLEADNAMECHAR="$SORT_BY_CHAR_OTHER"
  else
   CASTLEADNAMECHAR=${CASTLEADNAME:0:1}
  fi
  CASTLEADNAME="$(echo "$IMDBCASTLEADNAME" | tr -s ' ' "$SPACE_REPLACER" | tr "$BADCHARS" "$BAD_CHAR_REPLACER")"
  [[ ! -z "$SPECIAL_CHAR_REPLACER" ]] && CASTLEADNAME=$(echo "$CASTLEADNAME" | tr -sc 'A-Za-z0-9_\-(). \n' "$SPECIAL_CHAR_REPLACER")
  [[ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_CASTLEADNAME_NAME/$CASTLEADNAMECHAR" ]] &&
   mkdir -pm777 "$GLROOT$SYMLINK_PATH/$SORT_BY_CASTLEADNAME_NAME/$CASTLEADNAMECHAR"
  [[ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_CASTLEADNAME_NAME/$CASTLEADNAMECHAR/$CASTLEADNAME" ]] &&
   mkdir -pm777 "$GLROOT$SYMLINK_PATH/$SORT_BY_CASTLEADNAME_NAME/$CASTLEADNAMECHAR/$CASTLEADNAME"
  [[ ! -L "$GLROOT$SYMLINK_PATH/$SORT_BY_CASTLEADNAME_NAME/$CASTLEADNAMECHAR/$CASTLEADNAME/$IMDBDIRNAME" ]] &&
   ln -s "$IMDBRELPATH" "$GLROOT$SYMLINK_PATH/$SORT_BY_CASTLEADNAME_NAME/$CASTLEADNAMECHAR/$CASTLEADNAME/$IMDBDIRNAME"
 fi
fi

###################
# Casting Section #
###################

if [ $SORT_BY_CASTING -eq 1 ]; then
 [[ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_CASTING_NAME" ]] &&
  mkdir -pm777 "$GLROOT$SYMLINK_PATH/$SORT_BY_CASTING_NAME"

 # Cleanup Casting-dirs
 [[ $CLEANUP_SYMLINKS -eq 1 ]] &&
  proc_cleanup_double "$GLROOT$SYMLINK_PATH/$SORT_BY_CASTING_NAME/"

 # Make link if needed
 if [ ! -z "$IMDBCASTING" ] && [ ! $ISEXEMPT -eq 1 ]; then
  IMDBCASTINGS="`echo "$IMDBCASTING" | tr -s ' ' '_' | sed 's/\,_/ /g'`"
  for CASTING in $IMDBCASTINGS; do
   if [ "${CASTING:0:1}" == "." ]; then
    CASTINGCHAR="$SORT_BY_CHAR_DOT"
   elif [ ! -z "`echo "${CASTING:0:1}" | tr -d "A-Za-z0-9_\-()"`" ] && [ ! -z "$SORT_BY_CHAR_OTHER" ]; then
    CASTINGCHAR="$SORT_BY_CHAR_OTHER"
   else
    CASTINGCHAR=${CASTING:0:1}
   fi
   CASTING="$(echo "$CASTING" | tr -s '_' "$SPACE_REPLACER" | tr "$BADCHARS" "$BAD_CHAR_REPLACER")"
   [[ ! -z "$SPECIAL_CHAR_REPLACER" ]] && CASTING=$(echo "$CASTING" | tr -sc 'A-Za-z0-9_\-(). \n' "$SPECIAL_CHAR_REPLACER")
   [[ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_CASTING_NAME/$CASTINGCHAR" ]] &&
    mkdir -pm777 "$GLROOT$SYMLINK_PATH/$SORT_BY_CASTING_NAME/$CASTINGCHAR"
   [[ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_CASTING_NAME/$CASTINGCHAR/$CASTING" ]] &&
    mkdir -pm777 "$GLROOT$SYMLINK_PATH/$SORT_BY_CASTING_NAME/$CASTINGCHAR/$CASTING"
   [[ ! -L "$GLROOT$SYMLINK_PATH/$SORT_BY_CASTING_NAME/$CASTINGCHAR/$CASTING/$IMDBDIRNAME" ]] &&
    ln -s "$IMDBRELPATH" "$GLROOT$SYMLINK_PATH/$SORT_BY_CASTING_NAME/$CASTINGCHAR/$CASTING/$IMDBDIRNAME"
  done
 fi
fi

################
# Year Section #
################

if [ $SORT_BY_YEAR -eq 1 ]; then
 [[ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_YEAR_NAME" ]] &&
  mkdir -pm777 "$GLROOT$SYMLINK_PATH/$SORT_BY_YEAR_NAME"

 # Cleanup Year-dirs
 [[ $CLEANUP_SYMLINKS -eq 1 ]] &&
  proc_cleanup_single "$GLROOT$SYMLINK_PATH/$SORT_BY_YEAR_NAME/"

 # Make link if needed
 if [ ! -z "$IMDBYEAR" ] && [ ! $ISEXEMPT -eq 1 ]; then
  MYYEAR="`echo "$IMDBYEAR" | tr -cd '0-9'`"
  [[ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_YEAR_NAME/$MYYEAR" ]] &&
   mkdir -pm777 "$GLROOT$SYMLINK_PATH/$SORT_BY_YEAR_NAME/$MYYEAR"
  [[ ! -L "$GLROOT$SYMLINK_PATH/$SORT_BY_YEAR_NAME/$MYYEAR/$IMDBDIRNAME" ]] &&
   ln -s "$IMDBRELPATH" "$GLROOT$SYMLINK_PATH/$SORT_BY_YEAR_NAME/$MYYEAR/$IMDBDIRNAME"
 fi
fi

#################
# Score Section #
#################

if [ $SORT_BY_SCORE -eq 1 ]; then
 [[ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_SCORE_NAME" ]] &&
  mkdir -pm777 "$GLROOT$SYMLINK_PATH/$SORT_BY_SCORE_NAME"

 # Cleanup Score-dirs
 [[ $CLEANUP_SYMLINKS -eq 1 ]] &&
  proc_cleanup_single "$GLROOT$SYMLINK_PATH/$SORT_BY_SCORE_NAME/"

 # Make link if needed
 if [ ! -z "$IMDBSCORE" ] && [ ! $ISEXEMPT -eq 1 ]; then
  SCORE="`echo "$IMDBSCORE" | cut -d '.' -f 1`"
  [[ -z "$SCORE" ]] && SCORE="$SORT_BY_SCORE_NONE"
  [[ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_SCORE_NAME/$SCORE" ]] &&
   mkdir -pm777 "$GLROOT$SYMLINK_PATH/$SORT_BY_SCORE_NAME/$SCORE"
  [[ ! -L "$GLROOT$SYMLINK_PATH/$SORT_BY_SCORE_NAME/$SCORE/$IMDBDIRNAME" ]] &&
   ln -s "$IMDBRELPATH" "$GLROOT$SYMLINK_PATH/$SORT_BY_SCORE_NAME/$SCORE/$IMDBDIRNAME"
 fi
fi

#################
# Title Section #
#################

if [ $SORT_BY_TITLE -eq 1 ]; then
 [[ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_TITLE_NAME" ]] &&
  mkdir -pm777 "$GLROOT$SYMLINK_PATH/$SORT_BY_TITLE_NAME"

 # Cleanup Title-dirs
 [[ $CLEANUP_SYMLINKS -eq 1 ]] &&
  proc_cleanup_single "$GLROOT$SYMLINK_PATH/$SORT_BY_TITLE_NAME/"

 # Make link if needed
 if [ ! -z "$IMDBNAME" ] && [ ! $ISEXEMPT -eq 1 ]; then
  SECTION="`echo "$IMDBRELPATH" | tr ' ' '_' | tr '/' ' ' | wc -w | tr -d ' '`"
  SECTIONNAME="`echo "$IMDBRELPATH" | cut -d '/' -f $SECTION`"
  if [ "${IMDBNAME:0:1}" == "$QUOTECHAR" ] && [ "${IMDBNAME: -1:1}" == "$QUOTECHAR" ]; then
   # TV-serienames are between ", which is replaced by QUOTECHAR, which isn't part of the real name
   if [ "${IMDBNAME:1:1}" == "." ]; then
    TITLECHAR="$SORT_BY_CHAR_DOT"
   elif [ ! -z "`echo "${IMDBNAME:1:1}" | tr -d "A-Za-z0-9_\-()"`" ] && [ ! -z "$SORT_BY_CHAR_OTHER" ]; then
    TITLECHAR="$SORT_BY_CHAR_OTHER"
   else
   TITLECHAR=${IMDBNAME:1:1}
   fi
  elif [ "${IMDBNAME:0:1}" == "." ]; then
   TITLECHAR="$SORT_BY_CHAR_DOT"
  elif [ ! -z "`echo "${IMDBNAME:0:1}" | tr -d "A-Za-z0-9_\-()"`" ] && [ ! -z "$SORT_BY_CHAR_OTHER" ]; then
   TITLECHAR="$SORT_BY_CHAR_OTHER"
  else
   TITLECHAR=${IMDBNAME:0:1}
  fi
  TITLE="$(echo "$IMDBNAME-.$SECTIONNAME" | tr -s ' ' "$SPACE_REPLACER" | tr "$BADCHARS" "$BAD_CHAR_REPLACER")"
  [[ ! -z "$SPECIAL_CHAR_REPLACER" ]] && TITLE=$(echo "$TITLE" | tr -sc 'A-Za-z0-9_\-(). \n' "$SPECIAL_CHAR_REPLACER")
  [[ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_TITLE_NAME/$TITLECHAR" ]] &&
   mkdir -pm777 "$GLROOT$SYMLINK_PATH/$SORT_BY_TITLE_NAME/$TITLECHAR"
  CNTR=""
  while [ -L "$GLROOT$SYMLINK_PATH/$SORT_BY_TITLE_NAME/$TITLECHAR/$TITLE$CNTR" ]; do
   CNTR=$((CNTR+1))
  done
  ln -s "$IMDBRELPATH" "$GLROOT$SYMLINK_PATH/$SORT_BY_TITLE_NAME/$TITLECHAR/$TITLE$CNTR"
 fi
fi

#################
# Group Section #
#################

if [ $SORT_BY_GROUP -eq 1 ]; then
 [[ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_GROUP_NAME" ]] &&
  mkdir -pm777 "$GLROOT$SYMLINK_PATH/$SORT_BY_GROUP_NAME"

 # Cleanup Group-dirs
 [[ $CLEANUP_SYMLINKS -eq 1 ]] &&
  proc_cleanup_single "$GLROOT$SYMLINK_PATH/$SORT_BY_GROUP_NAME/"

 # Make link if needed
 if [ ! -z "$IMDBRELPATH" ]; then
  if [ "`basename "$IMDBRELPATH" | tr '-' '\n' | grep -v "^$" | wc -l`" -eq 1 ]; then
    GROUP="$SORT_BY_GROUP_NONE"
  elif [ ! -z "`echo $IMDBRELPATH | egrep "$SORT_BY_GROUP_SPECIAL"`" ]; then
    GROUP="`echo "$IMDBRELPATH" | egrep -o "*($SORT_BY_GROUP_SPECIAL)$"`"
  else
    GROUP="`basename "$IMDBRELPATH" | tr '-' '\n' | grep -v "^$" | tail -n 1`"
  fi
  if [ ! -z "$GROUP" ] && [ ! $ISEXEMPT -eq 1 ]; then
   [[ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_GROUP_NAME/$GROUP" ]] &&
    mkdir -pm777 "$GLROOT$SYMLINK_PATH/$SORT_BY_GROUP_NAME/$GROUP"
   [[ ! -L "$GLROOT$SYMLINK_PATH/$SORT_BY_GROUP_NAME/$GROUP/$IMDBDIRNAME" ]] &&
    ln -s "$IMDBRELPATH" "$GLROOT$SYMLINK_PATH/$SORT_BY_GROUP_NAME/$GROUP/$IMDBDIRNAME"
  fi
 fi
fi

################
# Date Section #
################

if [ $SORT_BY_DATE -eq 1 ]; then
 [[ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_DATE_NAME" ]] &&
  mkdir -pm777 "$GLROOT$SYMLINK_PATH/$SORT_BY_DATE_NAME"

 # Cleanup Date-dirs
 [[ $CLEANUP_SYMLINKS -eq 1 ]] &&
  proc_cleanup "$GLROOT$SYMLINK_PATH/$SORT_BY_DATE_NAME/"

 # Make link if needed
 if [ ! $ISEXEMPT -eq 1 ] && [ ! -z "$IMDBDIRNAME" ]; then
  if [ -e "$GLROOT$IMDBRELPATH/.message" ]; then
   MYDATEGRAB_PATH="$GLROOT$IMDBRELPATH/.message"
  else
   MYDATEGRAB_CD=`ls -1F "$GLROOT$IMDBRELPATH" | grep -e "[cC][dD][1aA]" | grep -e "/" | head -n 1 | tr -d '/'`
   MYDATEGRAB_DVD=`ls -1F "$GLROOT$IMDBRELPATH" | grep -e "[dD][vV][dD][1aA]" | grep -e "/" | head -n 1 | tr -d '/'`
   MYDATEGRAB_DISC=`ls -1F "$GLROOT$IMDBRELPATH" | grep -e "[dD][iI][sS][cC][1aA]" | grep -e "/" | head -n 1 | tr -d '/'`
   MYDATEGRAB_NFO=`ls -1Ftr "$GLROOT$IMDBRELPATH" | grep -e "[.][nN][fF][oO]" | grep -v "/" | grep -v "@" | head -n 1 | tr -d '/'`
   if [ ! -z "$MYDATEGRAB_CD" ] && [ -e "$GLROOT$IMDBRELPATH/$MYDATEGRAB_CD/.message" ]; then
    MYDATEGRAB_PATH="$GLROOT$IMDBRELPATH/$MYDATEGRAB_CD/.message"
   elif  [ ! -z "$MYDATEGRAB_DVD" ] && [ -e "$GLROOT$IMDBRELPATH/$MYDATEGRAB_DVD/.message" ]; then
    MYDATEGRAB_PATH="$GLROOT$IMDBRELPATH/$MYDATEGRAB_DVD/.message"
   elif  [ ! -z "$MYDATEGRAB_DISC" ] && [ -e "$GLROOT$IMDBRELPATH/$MYDATEGRAB_DISC/.message" ]; then
    MYDATEGRAB_PATH="$GLROOT$IMDBRELPATH/$MYDATEGRAB_DVD/.message"
   elif  [ ! -z "$MYDATEGRAB_NFO" ] && [ -e "$GLROOT$IMDBRELPATH/$MYDATEGRAB_NFO" ]; then
    MYDATEGRAB_PATH="$GLROOT$IMDBRELPATH/$MYDATEGRAB_NFO"
   else
    MYDATEGRAB_PATH="$GLROOT$IMDBRELPATH"
   fi
  fi
  if [ "$SORT_BY_DATE_LS" = "bsd" ]; then
   if [ ! "$MYDATEGRAB_PATH" = "$GLROOT$IMDBRELPATH" ]; then
    MYDATEGRAB_YEAR=`ls -lanTt "$MYDATEGRAB_PATH" | awk '{print $9}'`
    MYDATEGRAB_MONT=`ls -lanTt "$MYDATEGRAB_PATH" | awk '{print $6}'`
    MYDATEGRAB_DATE=`ls -lanTt "$MYDATEGRAB_PATH" | awk '{print $7}'`
    MYDATEGRAB_TIME=`ls -lanTt "$MYDATEGRAB_PATH" | awk '{print $8}' | tr ':' ' ' | awk '{print $1$2}'`
   else
    MYDATEGRAB_YEAR=`ls -lanTt "$MYDATEGRAB_PATH" | grep -e "\ .$" | awk '{print $9}'`
    MYDATEGRAB_MONT=`ls -lanTt "$MYDATEGRAB_PATH" | grep -e "\ .$" | awk '{print $6}'`
    MYDATEGRAB_DATE=`ls -lanTt "$MYDATEGRAB_PATH" | grep -e "\ .$" | awk '{print $7}'`
    MYDATEGRAB_TIME=`ls -lanTt "$MYDATEGRAB_PATH" | grep -e "\ .$" | awk '{print $8}' | tr ':' ' ' | awk '{print $1$2}'`
   fi
  else
   if [ ! "$MYDATEGRAB_PATH" = "$GLROOT$IMDBRELPATH" ]; then
    MYDATEGRAB_YEAR=`find "$MYDATEGRAB_PATH" -printf "%Ta %Tb %Td %TT %TY" | awk '{print $5}'`
    MYDATEGRAB_MONT=`find "$MYDATEGRAB_PATH" -printf "%Ta %Tb %Td %TT %TY" | awk '{print $2}'`
    MYDATEGRAB_DATE=`find "$MYDATEGRAB_PATH" -printf "%Ta %Tb %Td %TT %TY" | awk '{print $3}'`
    MYDATEGRAB_TIME=`find "$MYDATEGRAB_PATH" -printf "%Ta %Tb %Td %TT %TY" | awk '{print $4}' | tr ':' ' ' | awk '{print $1$2}'`
   else
    MYDATEGRAB_YEAR=`find "$MYDATEGRAB_PATH" -type d -printf "%Ta %Tb %Td %TT %TY" | awk '{print $5}'`
    MYDATEGRAB_MONT=`find "$MYDATEGRAB_PATH" -type d -printf "%Ta %Tb %Td %TT %TY" | awk '{print $2}'`
    MYDATEGRAB_DATE=`find "$MYDATEGRAB_PATH" -type d -printf "%Ta %Tb %Td %TT %TY" | awk '{print $3}'`
    MYDATEGRAB_TIME=`find "$MYDATEGRAB_PATH" -type d -printf "%Ta %Tb %Td %TT %TY" | awk '{print $4}' | tr ':' ' ' | awk '{print $1$2}'`
   fi
  fi
  [[ $MYDATEGRAB_DATE -le 9 ]] &&
   MYDATEGRAB_DATE="0$MYDATEGRAB_DATE"
  if [ ! -z "$SORT_BY_DATE_FORMAT" ]; then
   MYDATE="$SORT_BY_DATE_FORMAT$IMDBDIRNAME"
  else
   MYDATE="`echo "$MYDATEGRAB_YEAR"".""$MYDATEGRAB_MONT"".""$MYDATEGRAB_DATE""-""$MYDATEGRAB_TIME"".-.""$IMDBDIRNAME" | tr -cs '0-9a-zA-Z_\-()\n' '.'`"
  fi
  MYIMDBDIRNAME="$(echo $IMDBDIRNAME | tr -s ' ' "$SPACE_REPLACER")"
  [[ ! -z "$SPECIAL_CHAR_REPLACER" ]] && MYIMDBDIRNAME=$(echo "$IMDBNAME" | tr -sc 'A-Za-z0-9_\-(). \n' "$SPECIAL_CHAR_REPLACER")
  MYDELETE="`ls -1F "$GLROOT$SYMLINK_PATH/$SORT_BY_DATE_NAME" | grep -e "@" | grep -e "$MYIMDBDIRNAME" | tr -d '@' | grep -v "^$MYDATE$" | head -n 1`"
  [[ ! -z "$MYDELETE" ]] &&
   rm "$GLROOT$SYMLINK_PATH/$SORT_BY_DATE_NAME/$MYDELETE"
  if [ ! -L "$GLROOT$SYMLINK_PATH/$SORT_BY_DATE_NAME/$MYDATE" ]; then
   ln -s "$IMDBRELPATH" "$GLROOT$SYMLINK_PATH/$SORT_BY_DATE_NAME/$MYDATE"
   [[ -e "$GLROOT$IMDBRELPATH/.message" ]] &&
    touch -acmr "$GLROOT$IMDBRELPATH/.message" "$GLROOT$SYMLINK_PATH/$SORT_BY_DATE_NAME/$MYDATE"
  fi
 fi
fi

##################
# Top250 Section #
##################

if [ $SORT_BY_TOP250 -eq 1 ]; then
 [[ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_TOP250_NAME" ]] &&
  mkdir -pm777 "$GLROOT$SYMLINK_PATH/$SORT_BY_TOP250_NAME"

 # Cleanup TOP250-dirs
 [[ $CLEANUP_SYMLINKS -eq 1 ]] &&
  proc_cleanup_single "$GLROOT$SYMLINK_PATH/$SORT_BY_TOP250_NAME/"

 # Make link if needed
 TOP250_RATING=`echo "$IMDBRATING" | grep -e "250:" | grep -e "#" | cut -d "#" -f 2 | tr -cd '0-9'`
 if [ ! -z "$TOP250_RATING" ] && [ ! $ISEXEMPT -eq 1 ]; then
  if [ $TOP250_RATING -lt 10 ]; then
   TOP250R="00""$TOP250_RATING"
  elif [ $TOP250_RATING -lt 100 ]; then
   TOP250R="0""$TOP250_RATING"
  else
   TOP250R="$TOP250_RATING"
  fi
  IMDBNAMENEW="$(echo $IMDBNAME | tr -s ' ' "$SPACE_REPLACER" | tr "$BADCHARS" "$BAD_CHAR_REPLACER")"
  [[ ! -z "$SPECIAL_CHAR_REPLACER" ]] && IMDBNAMENEW=$(echo "$IMDBNAME" | tr -sc 'A-Za-z0-9_\-(). \n' "$SPECIAL_CHAR_REPLACER")
  TOP250="$TOP250R.-.$IMDBNAMENEW.($IMDBYEAR)"
  if [ -z "`ls -1F "$GLROOT$SYMLINK_PATH/$SORT_BY_TOP250_NAME" | grep -e "^$TOP250R.-.$IMDBNAMENEW"`" ]; then
   rm -fr "$GLROOT$SYMLINK_PATH/$SORT_BY_TOP250_NAME/"$TOP250R.-.*
   rm -fr "$GLROOT$SYMLINK_PATH/$SORT_BY_TOP250_NAME/"*.-.$IMDBNAMENEW.*
  fi
  [[ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_TOP250_NAME/$TOP250" ]] &&
   mkdir -pm777 "$GLROOT$SYMLINK_PATH/$SORT_BY_TOP250_NAME/$TOP250"
  [[ ! -L "$GLROOT$SYMLINK_PATH/$SORT_BY_TOP250_NAME/$TOP250/$IMDBDIRNAME" ]] &&
   ln -s "$IMDBRELPATH" "$GLROOT$SYMLINK_PATH/$SORT_BY_TOP250_NAME/$TOP250/$IMDBDIRNAME"
 fi
fi

###################
# Keyword Section #
###################

if [ ! -z "$SORT_BY_KEYWORD_LIST" ]; then
 if [ $SORT_BY_KEYWORD -eq 1 ]; then
  for KEYWORD_PAIR in $SORT_BY_KEYWORD_LIST; do
   KEYWORD_SEARCH=`echo $KEYWORD_PAIR | cut -d '|' -f 1 | tr 'A-Z' 'a-z'`
   KEYWORD_REPLACE=`echo $KEYWORD_PAIR | cut -d '|' -f 2 | tr -d '\\/\"&'`
   SORT_BY_KEYWORD_REPLACED=`echo $SORT_BY_KEYWORD_NAME | sed "s/%KEYWORD%/$KEYWORD_REPLACE/g"`
   [[ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_KEYWORD_REPLACED" ]] &&
    mkdir -pm777 "$GLROOT$SYMLINK_PATH/$SORT_BY_KEYWORD_REPLACED"

   # Cleanup Keyword-dirs
   [[ $CLEANUP_SYMLINKS -eq 1 ]] &&
    proc_cleanup "$GLROOT$SYMLINK_PATH/$SORT_BY_KEYWORD_REPLACED/"

   # Make link if needed
   if [ ! $ISEXEMPT -eq 1 ]; then
    if [ ! -z "`echo $IMDBDIRNAME | tr '\.\ \-_' '\n' | tr 'A-Z' 'a-z' | grep -e "^$KEYWORD_SEARCH$"`" ]; then
     [[ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_KEYWORD_REPLACED/" ]] &&
      mkdir -pm777 "$GLROOT$SYMLINK_PATH/$SORT_BY_KEYWORD_REPLACED/"
     [[ ! -L "$GLROOT$SYMLINK_PATH/$SORT_BY_KEYWORD_REPLACED/$IMDBDIRNAME" ]] &&
      ln -s "$IMDBRELPATH" "$GLROOT$SYMLINK_PATH/$SORT_BY_KEYWORD_REPLACED/$IMDBDIRNAME"
    fi
   fi
  done
 fi
fi

# You should always exit with a 0 - the parent won't give a crap anyway,
# but just to make sure...
exit 0
