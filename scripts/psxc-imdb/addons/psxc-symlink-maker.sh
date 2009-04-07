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
VERSION=2.9h

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
# It goes like this - you enter a keyword to search for, followed by a '|'
# (pipe), followed by what you wish the %KEYWORD% to be replaced with. Spaces
# separate keywords. If you wish to search for several keywords and put them in
# the same dir, you can do that. The search keyword is case-insensitive.
SORT_BY_KEYWORD_LIST="german|German.Movies divx|DivX xvid|DivX"

# Clean up dead symlinks after each run? Usually, this is done pretty quick,
SORT_BY_GROUP_NONE="#NONE#"
SORT_BY_GROUP_SPECIAL="VH-PROD|DVD-R"
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
for a in `echo $@ | sed "s|\" \"|^|g"`; do
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
if [ ! -z "`echo "$SYMLINK_PATH" | grep -v "/"`" ]; then
 echo "config error. check variables."
 exit 0
fi

# Let's check the if SYMLINK_PATH exist.
if [ ! -d "$GLROOT$SYMLINK_PATH" ]; then
 mkdir "$GLROOT$SYMLINK_PATH" >/dev/null 2>&1
 if [ $? -ne 0 ]; then
  echo "Could not create $GLROOT$SYMLINK_PATH. Exiting."
  exit 0
 fi
 chmod 777 "$GLROOT$SYMLINK_PATH" >/dev/null 2>&1
 if [ $? -ne 0 ]; then
  echo "Could not chmod 777 $GLROOT$SYMLINK_PATH. Exiting."
  exit 0
 fi
fi

# Make sure we are able to write in SYMLINK_PATH
if [ ! -w "$GLROOT$SYMLINK_PATH" ]; then
 echo "Unable to write to $GLROOT$SYMLINK_PATH. Exiting."
 exit 0
fi

if [ -z "$IMDBRELPATH" ]; then
 echo "Seems to me like you've run this script in standalone mode."
 echo "That's fine if you wish to clean up old symlinks. If this was"
 echo "not your intention, I suggest you read the docs on how to"
 echo "set this up."
 echo "I will now check for dead links, ignoring the CLEANUP_SYMLINKS"
 echo "flag. You can take advantage of this if you like, by adding a"
 echo "crontab entry and setting CLEANUP_SYMLINKS=0. The addon will be"
 echo "faster..."
 echo "A crontab entry can look like this (running every 30 mins):"
 echo "7,37 * * * * /glftpd/bin/psxc-symlink-maker.sh >/dev/null 2>&1"
 echo ""
 echo "Please wait while I check for dead links..."
 echo ""
 CLEANUP_SYMLINKS=1
fi

# finally, let's see if the release still exists. Maybe it's already been
# moved by a different script?
if [ ! -e "$GLROOT/$IMDBRELPATH" ]; then
 exit 0
fi

# Lets find out the number of arguments 'ls' gives...
MYNUMA=`pwd`
MYNUMB="$0"
if [ ! -z "`echo $MYNUMB | grep -e "^\."`" ]; then
 MYNUMB=`echo $MYNUMB | sed "s|^.||"`
 MYNUM=`ls -lF $MYNUMA/$MYNUMB | wc -w`
else
 MYNUM=`ls -lF $MYNUMB | wc -w`
fi

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

# Genre Section.

if [ $SORT_BY_GENRE -eq 1 ]; then
 if [ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_GENRE_NAME" ]; then
  mkdir -p "$GLROOT$SYMLINK_PATH/$SORT_BY_GENRE_NAME"
  chmod 777 "$GLROOT$SYMLINK_PATH/$SORT_BY_GENRE_NAME"
 fi

# Cleanup Genre-dirs
 if [ $CLEANUP_SYMLINKS -eq 1 ]; then
  GENRES="`ls -F $GLROOT$SYMLINK_PATH/$SORT_BY_GENRE_NAME/ | grep "/" | tr ' ' '%'`"
  if [ ! -z "$GENRES" ]; then
   for GENRE in $GENRES; do
    GENRE="`echo $GENRE | tr '%' ' '`"
    for GENRE_SYM in `ls -l "$GLROOT$SYMLINK_PATH/$SORT_BY_GENRE_NAME/$GENRE" | grep ">" | cut -d '>' -f 2 | tr ' ' '%'`; do
     GENRE_SYM="`echo "$GENRE_SYM" | tr '%' ' ' | awk '{for (i = 1; i <= NF; i++) print $i}' | tr '\n' ' '`"
     let GENRE_SYM_CNT="`echo "$GENRE_SYM" | wc -c`-2"
     GENRE_SYM="`echo "$GENRE_SYM" | cut -c 1-$GENRE_SYM_CNT`"
      if [ ! -e "$GLROOT$GENRE_SYM" ]; then
      rm -f  "$GLROOT$SYMLINK_PATH/$SORT_BY_GENRE_NAME/$GENRE`basename "$GENRE_SYM"`"
     fi
    done
    rmdir "$GLROOT$SYMLINK_PATH/$SORT_BY_GENRE_NAME/$GENRE" >/dev/null 2>&1
   done
  fi
 fi

# Make link if needed
 if [ ! -z "$IMDBGENRE" ] && [ ! $ISEXEMPT -eq 1 ]; then
  IMDBGENRES="`echo "$IMDBGENRE" | tr -s '/ ' ' '`"
  for GENRE in $IMDBGENRES; do
   if [ ! -d  "$GLROOT$SYMLINK_PATH/$SORT_BY_GENRE_NAME/$GENRE" ]; then
    mkdir -p "$GLROOT$SYMLINK_PATH/$SORT_BY_GENRE_NAME/$GENRE"
    chmod 777 "$GLROOT$SYMLINK_PATH/$SORT_BY_GENRE_NAME/$GENRE"
   fi
   if [ ! -L "$GLROOT$SYMLINK_PATH/$SORT_BY_GENRE_NAME/$GENRE/$IMDBDIRNAME" ]; then
    ln -s "$IMDBRELPATH" "$GLROOT$SYMLINK_PATH/$SORT_BY_GENRE_NAME/$GENRE/$IMDBDIRNAME"
   fi
  done
 fi
fi

# Language Section.
if [ $SORT_BY_LANGUAGE -eq 1 ]; then
 if [ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_LANGUAGE_NAME" ]; then
  mkdir -p "$GLROOT$SYMLINK_PATH/$SORT_BY_LANGUAGE_NAME"
  chmod 777 "$GLROOT$SYMLINK_PATH/$SORT_BY_LANGUAGE_NAME"
 fi
# Cleanup Language-dirs
 if [ $CLEANUP_SYMLINKS -eq 1 ]; then
  LANGUAGES="`ls -F $GLROOT$SYMLINK_PATH/$SORT_BY_LANGUAGE_NAME/ | grep "/" | tr ' ' '%'`"
  if [ ! -z "$LANGUAGES" ]; then
   for LANGUAGE in $LANGUAGES; do
    LANGUAGE="`echo $LANGUAGE | tr '%' ' '`"
    for LANGUAGE_SYM in `ls -l "$GLROOT$SYMLINK_PATH/$SORT_BY_LANGUAGE_NAME/$LANGUAGE" | grep ">" | cut -d '>' -f 2 | tr ' ' '%'`; do
     LANGUAGE_SYM="`echo "$LANGUAGE_SYM" | tr '%' ' ' | awk '{for (i = 1; i <= NF; i++) print $i}' | tr '\n' ' '`"
     let LANGUAGE_SYM_CNT="`echo "$LANGUAGE_SYM" | wc -c`-2"
     LANGUAGE_SYM="`echo "$LANGUAGE_SYM" | cut -c 1-$LANGUAGE_SYM_CNT`"
      if [ ! -e "$GLROOT$LANGUAGE_SYM" ]; then
      rm -f  "$GLROOT$SYMLINK_PATH/$SORT_BY_LANGUAGE_NAME/$LANGUAGE`basename "$LANGUAGE_SYM"`"
     fi
    done
    rmdir "$GLROOT$SYMLINK_PATH/$SORT_BY_LANGUAGE_NAME/$LANGUAGE" >/dev/null 2>&1
   done
  fi
 fi
# Make link if needed
 if [ ! -z "$IMDBLANGUAGE" ] && [ ! $ISEXEMPT -eq 1 ]; then
  IMDBLANGUAGES="`echo "$IMDBLANGUAGE" | tr -s ' ' '_' | sed s/'_|_'/' '/g`"
  for LANGUAGE in $IMDBLANGUAGES; do
   if [ ! -d  "$GLROOT$SYMLINK_PATH/$SORT_BY_LANGUAGE_NAME/$LANGUAGE" ]; then
    mkdir -p "$GLROOT$SYMLINK_PATH/$SORT_BY_LANGUAGE_NAME/$LANGUAGE"
    chmod 777 "$GLROOT$SYMLINK_PATH/$SORT_BY_LANGUAGE_NAME/$LANGUAGE"
   fi
   if [ ! -L "$GLROOT$SYMLINK_PATH/$SORT_BY_LANGUAGE_NAME/$LANGUAGE/$IMDBDIRNAME" ]; then
    ln -s "$IMDBRELPATH" "$GLROOT$SYMLINK_PATH/$SORT_BY_LANGUAGE_NAME/$LANGUAGE/$IMDBDIRNAME"
   fi
  done
 fi
fi
# Year Section.

if [ $SORT_BY_YEAR -eq 1 ]; then
 if [ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_YEAR_NAME" ]; then
  mkdir -p "$GLROOT$SYMLINK_PATH/$SORT_BY_YEAR_NAME"
  chmod 777 "$GLROOT$SYMLINK_PATH/$SORT_BY_YEAR_NAME"
 fi

# Cleanup Year-dirs
 if [ $CLEANUP_SYMLINKS -eq 1 ]; then
  YEARS="`ls -F "$GLROOT$SYMLINK_PATH/$SORT_BY_YEAR_NAME/" | grep "/" | tr ' ' '%'`"
  if [ ! -z "$YEARS" ]; then
   for YEAR in $YEARS; do
    YEAR="`echo $YEAR | tr '%' ' '`"
    for YEAR_SYM in `ls -lF "$GLROOT$SYMLINK_PATH/$SORT_BY_YEAR_NAME/$YEAR" | grep ">" | cut -d '>' -f 2 | tr ' ' '%'`; do
     YEAR_SYM="`echo "$YEAR_SYM" | tr '%' ' ' | awk '{for (i = 1; i <= NF; i++) print $i}' | tr '\n' ' '`"
     let YEAR_SYM_CNT="`echo "$YEAR_SYM" | wc -c`-2"
     YEAR_SYM="`echo "$YEAR_SYM" | cut -c 1-$YEAR_SYM_CNT`"
     if [ ! -e "$GLROOT$YEAR_SYM" ]; then
      rm -f "$GLROOT$SYMLINK_PATH/$SORT_BY_YEAR_NAME/$YEAR`basename "$YEAR_SYM"`"
     fi
    done
    rmdir "$GLROOT$SYMLINK_PATH/$SORT_BY_YEAR_NAME/$YEAR" >/dev/null 2>&1
   done
  fi
 fi

# Make link if needed
 if [ ! -z "$IMDBYEAR" ] && [ ! $ISEXEMPT -eq 1 ]; then
  MYYEAR="`echo "$IMDBYEAR" | tr -cd '0-9'`"
  if [ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_YEAR_NAME/$MYYEAR" ]; then
   mkdir -p "$GLROOT$SYMLINK_PATH/$SORT_BY_YEAR_NAME/$MYYEAR"
   chmod 777 "$GLROOT$SYMLINK_PATH/$SORT_BY_YEAR_NAME/$MYYEAR"
  fi
  if [ ! -L "$GLROOT$SYMLINK_PATH/$SORT_BY_YEAR_NAME/$MYYEAR/$IMDBDIRNAME" ]; then
   ln -s "$IMDBRELPATH" "$GLROOT$SYMLINK_PATH/$SORT_BY_YEAR_NAME/$MYYEAR/$IMDBDIRNAME"
  fi
 fi
fi

# Score Section.

if [ $SORT_BY_SCORE -eq 1 ]; then
 if [ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_SCORE_NAME" ]; then
  mkdir -p "$GLROOT$SYMLINK_PATH/$SORT_BY_SCORE_NAME"
  chmod 777 "$GLROOT$SYMLINK_PATH/$SORT_BY_SCORE_NAME"
 fi

# Cleanup Score-dirs
 if [ $CLEANUP_SYMLINKS -eq 1 ]; then
  SCORES="`ls -F "$GLROOT$SYMLINK_PATH/$SORT_BY_SCORE_NAME/" | grep "/" | tr ' ' '%'`"
  if [ ! -z "$SCORES" ]; then
   for SCORE in $SCORES; do
    SCORE="`echo $SCORE | tr '%' ' '`"
    for SCORE_SYM in `ls -lF "$GLROOT$SYMLINK_PATH/$SORT_BY_SCORE_NAME/$SCORE" | grep ">" | cut -d '>' -f 2 | tr ' ' '%'`; do
     SCORE_SYM="`echo "$SCORE_SYM" | tr '%' ' ' | awk '{for (i = 1; i <= NF; i++) print $i}' | tr '\n' ' '`"
     let SCORE_SYM_CNT="`echo "$SCORE_SYM" | wc -c`-2"
     SCORE_SYM="`echo "$SCORE_SYM" | cut -c 1-$SCORE_SYM_CNT`"
     if [ ! -e "$GLROOT$SCORE_SYM" ]; then
      rm -f "$GLROOT$SYMLINK_PATH/$SORT_BY_SCORE_NAME/$SCORE`basename "$SCORE_SYM"`"
     fi
    done
    rmdir "$GLROOT$SYMLINK_PATH/$SORT_BY_SCORE_NAME/$SCORE" >/dev/null 2>&1
   done
  fi
 fi

# Make link if needed
 if [ ! -z "$IMDBSCORE" ] && [ ! $ISEXEMPT -eq 1 ]; then
  SCORE="`echo "$IMDBSCORE" | cut -d '.' -f 1`"
  if [ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_SCORE_NAME/$SCORE" ]; then
   mkdir -p "$GLROOT$SYMLINK_PATH/$SORT_BY_SCORE_NAME/$SCORE"
   chmod 777 "$GLROOT$SYMLINK_PATH/$SORT_BY_SCORE_NAME/$SCORE"
  fi
  if [ ! -L "$GLROOT$SYMLINK_PATH/$SORT_BY_SCORE_NAME/$SCORE/$IMDBDIRNAME" ]; then
   ln -s "$IMDBRELPATH" "$GLROOT$SYMLINK_PATH/$SORT_BY_SCORE_NAME/$SCORE/$IMDBDIRNAME"
  fi
 fi
fi

# Title Section.

if [ $SORT_BY_TITLE -eq 1 ]; then
 if [ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_TITLE_NAME" ]; then
  mkdir -p "$GLROOT$SYMLINK_PATH/$SORT_BY_TITLE_NAME"
  chmod 777 "$GLROOT$SYMLINK_PATH/$SORT_BY_TITLE_NAME"
 fi

# Cleanup Title-dirs
 if [ $CLEANUP_SYMLINKS -eq 1 ]; then
  TITLES="`ls -F "$GLROOT$SYMLINK_PATH/$SORT_BY_TITLE_NAME/" | grep "/" | tr ' ' '%'`"
  if [ ! -z "$TITLES" ]; then
   for TITLE in $TITLES; do
    TITLE="`echo "$TITLE" | tr '%' ' '`"
    for TITLE_SYM in `ls -lF "$GLROOT$SYMLINK_PATH/$SORT_BY_TITLE_NAME/$TITLE" | grep -e ">" | sed "s|->|>|g" | tr ' \?' '%'`; do
     TITLE_SYM_SRC="`echo "$TITLE_SYM" | cut -d '>' -f 1 | tr '%' ' ' | tr '@' '\n' | grep -v "^$" | head -n 1 | awk '{for (i = ENVIRON["MYNUM"]; i <= NF; i++) print $i}' | tr '\n' ' '`"
     let TITLE_SYM_SRC_CNT="`echo "$TITLE_SYM_SRC" | wc -c`-2"
     TITLE_SYM_SRC="`echo "$TITLE_SYM_SRC" | cut -c 1-$TITLE_SYM_SRC_CNT`"
     TITLE_SYM_DST="`echo "$TITLE_SYM" | cut -d '>' -f 2 | tr '%' ' ' | awk '{for (i = 1; i <= NF; i++) print $i}' | tr '\n' ' '`"
     let TITLE_SYM_DST_CNT="`echo "$TITLE_SYM_DST" | wc -c`-2"
     TITLE_SYM_DST="`echo "$TITLE_SYM_DST" | cut -c 1-$TITLE_SYM_DST_CNT`"
     if [ ! -e "$GLROOT$TITLE_SYM_DST" ]; then
      rm -f  "$GLROOT$SYMLINK_PATH/$SORT_BY_TITLE_NAME/$TITLE/`basename "$TITLE_SYM_SRC"`"
     fi
    done
    rmdir "$GLROOT$SYMLINK_PATH/$SORT_BY_TITLE_NAME/$TITLE" >/dev/null 2>&1
   done
  fi
 fi

# Make link if needed
 if [ ! -z "$IMDBNAME" ] && [ ! $ISEXEMPT -eq 1 ]; then
  SECTION="`echo "$IMDBRELPATH" | tr ' ' '_' | tr '/' ' ' | wc -w | tr -d ' '`"
  SECTIONNAME="`echo "$IMDBRELPATH" | cut -d '/' -f $SECTION`"
  if [ "${IMDBNAME:0:1}" == "'" ] && [ -z `echo "${IMDBNAME:1:1}" | tr -d "A-Za-z0-9_\-()"` ]; then
    TITLECHAR=${IMDBNAME:1:1}
  elif [ "${IMDBNAME:0:1}" == "." ]; then
    TITLECHAR="DOT"
  elif [ ! -z `echo "${IMDBNAME:0:1}" | tr -d "A-Za-z0-9_\-()"` ]; then
    TITLECHAR="Other"
  else
    TITLECHAR=${IMDBNAME:0:1}
  fi
  TITLE="`echo "$IMDBNAME" | tr -d "'" | tr -sc 'A-Za-z0-9_\-()' '.'`""-.$SECTIONNAME"
  if [ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_TITLE_NAME/$TITLECHAR" ]; then
   mkdir -p "$GLROOT$SYMLINK_PATH/$SORT_BY_TITLE_NAME/$TITLECHAR"
   chmod 777 "$GLROOT$SYMLINK_PATH/$SORT_BY_TITLE_NAME/$TITLECHAR"
  fi
  if [ ! -L "$GLROOT$SYMLINK_PATH/$SORT_BY_TITLE_NAME/$TITLECHAR/$TITLE" ]; then
   ln -s "$IMDBRELPATH" "$GLROOT$SYMLINK_PATH/$SORT_BY_TITLE_NAME/$TITLECHAR/$TITLE"
  fi
 fi
fi

# Group Section.

if [ $SORT_BY_GROUP -eq 1 ]; then
 if [ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_GROUP_NAME" ]; then
  mkdir -p "$GLROOT$SYMLINK_PATH/$SORT_BY_GROUP_NAME"
  chmod 777 "$GLROOT$SYMLINK_PATH/$SORT_BY_GROUP_NAME"
 fi

# Cleanup Group-dirs
 if [ $CLEANUP_SYMLINKS -eq 1 ]; then
  DIRGROUPS="`ls -F "$GLROOT$SYMLINK_PATH/$SORT_BY_GROUP_NAME/" | grep "/" | tr ' ' '%'`"
  if [ ! -z "$DIRGROUPS" ]; then
   for GROUP in $DIRGROUPS; do
    GROUP="`echo $GROUP | tr '%' ' '`"
    for GROUP_SYM in `ls -lF "$GLROOT$SYMLINK_PATH/$SORT_BY_GROUP_NAME/$GROUP" | grep ">" | cut -d '>' -f 2 | tr ' ' '%'`; do
     GROUP_SYM="`echo "$GROUP_SYM" | tr '%' ' ' | awk '{for (i = 1; i <= NF; i++) print $i}' | tr '\n' ' '`"
     let GROUP_SYM_CNT="`echo "$GROUP_SYM" | wc -c`-2"
     GROUP_SYM="`echo "$GROUP_SYM" | cut -c 1-$GROUP_SYM_CNT`"
     if [ ! -e "$GLROOT$GROUP_SYM" ]; then
      rm -f "$GLROOT$SYMLINK_PATH/$SORT_BY_GROUP_NAME/$GROUP`basename "$GROUP_SYM"`"
     fi
    done
    rmdir "$GLROOT$SYMLINK_PATH/$SORT_BY_GROUP_NAME/$GROUP" >/dev/null 2>&1
   done
  fi
 fi

# Make link if needed
 if [ ! -z "$IMDBRELPATH" ]; then
  if [ "`basename "$IMDBRELPATH" | tr '-' '\n' | grep -v "^$" | wc -l`" -eq 1 ]; then
    GROUP="$SORT_BY_GROUP_NONE"
  elif [ ! -z `echo $IMDBRELPATH | egrep "$SORT_BY_GROUP_SPECIAL"` ]; then
    GROUP="`echo "$IMDBRELPATH" | egrep -o "*($SORT_BY_GROUP_SPECIAL)$"`"
  else
    GROUP="`basename "$IMDBRELPATH" | tr '-' '\n' | grep -v "^$" | tail -n 1`"
  fi
  if [ ! -z "$GROUP" ] && [ ! $ISEXEMPT -eq 1 ]; then
   if [ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_GROUP_NAME/$GROUP" ]; then
    mkdir -p "$GLROOT$SYMLINK_PATH/$SORT_BY_GROUP_NAME/$GROUP"
    chmod 777 "$GLROOT$SYMLINK_PATH/$SORT_BY_GROUP_NAME/$GROUP"
   fi
   if [ ! -L "$GLROOT$SYMLINK_PATH/$SORT_BY_GROUP_NAME/$GROUP/$IMDBDIRNAME" ]; then
    ln -s "$IMDBRELPATH" "$GLROOT$SYMLINK_PATH/$SORT_BY_GROUP_NAME/$GROUP/$IMDBDIRNAME"
   fi
  fi
 fi
fi

# Date Section.

if [ $SORT_BY_DATE -eq 1 ]; then
 if [ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_DATE_NAME" ]; then
  mkdir -p "$GLROOT$SYMLINK_PATH/$SORT_BY_DATE_NAME"
  chmod 777 "$GLROOT$SYMLINK_PATH/$SORT_BY_DATE_NAME"
 fi

# Cleanup Date-dirs
 for DATE_SYM in `ls -lF "$GLROOT$SYMLINK_PATH/$SORT_BY_DATE_NAME/" | grep -e ">" | sed "s|->|>|g" | tr ' \?' '%'`; do
  DATE_SYM_SRC="`echo $DATE_SYM | cut -d '>' -f 1 | tr '%' ' ' | tr '@' '\n' | grep -v "^$" | head -n 1 | awk '{for (i = ENVIRON["MYNUM"]; i <= NF; i++) print $i}' | tr '\n' ' '`"
  let DATE_SYM_SRC_CNT="`echo "$DATE_SYM_SRC" | wc -c`-2"
  if [ ! $DATE_SYM_SRC_CNT -eq 1 ]; then
   DATE_SYM_SRC="`echo "$DATE_SYM_SRC" | cut -c 1-$DATE_SYM_SRC_CNT`"
  fi
  DATE_SYM_DST="`echo "$DATE_SYM" | cut -d '>' -f 2 | tr '%' ' ' | awk '{for (i = 1; i <= NF; i++) print $i}' | tr '\n' ' '`"
  let DATE_SYM_DST_CNT="`echo "$DATE_SYM_DST" | wc -c`-2"
  DATE_SYM_DST="`echo "$DATE_SYM_DST" | cut -c 1-$DATE_SYM_DST_CNT`"
  if [ ! -e "$GLROOT$DATE_SYM_DST" ]; then
   rm -f  "$GLROOT$SYMLINK_PATH/$SORT_BY_DATE_NAME/`basename "$DATE_SYM_SRC"`"
  fi
 done

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
  if [ $MYDATEGRAB_DATE -le 9 ]; then
   MYDATEGRAB_DATE="0$MYDATEGRAB_DATE"
  fi
  if [ ! -z "$SORT_BY_DATE_FORMAT" ]; then
   MYDATE="$SORT_BY_DATE_FORMAT$IMDBDIRNAME"
  else
   MYDATE="`echo "$MYDATEGRAB_YEAR"".""$MYDATEGRAB_MONT"".""$MYDATEGRAB_DATE""-""$MYDATEGRAB_TIME"".-.""$IMDBDIRNAME" | tr -cs '0-9a-zA-Z_\-()\n' '.'`"
  fi
  MYIMDBDIRNAME="`echo "$IMDBDIRNAME" | tr -cs '0-9a-zA-Z_\-()\n' '.'`"
  MYDELETE="`ls -1F "$GLROOT$SYMLINK_PATH/$SORT_BY_DATE_NAME" | grep -e "@" | grep -e "$MYIMDBDIRNAME" | tr -d '@' | grep -v "^$MYDATE$" | head -n 1`"
  if [ ! -z "$MYDELETE" ]; then
   rm "$GLROOT$SYMLINK_PATH/$SORT_BY_DATE_NAME/$MYDELETE"
  fi
  if [ ! -L "$GLROOT$SYMLINK_PATH/$SORT_BY_DATE_NAME/$MYDATE" ]; then
   ln -s "$IMDBRELPATH" "$GLROOT$SYMLINK_PATH/$SORT_BY_DATE_NAME/$MYDATE"
   if [ -e "$GLROOT$IMDBRELPATH/.message" ]; then
    touch -acmr "$GLROOT$IMDBRELPATH/.message" "$GLROOT$SYMLINK_PATH/$SORT_BY_DATE_NAME/$MYDATE"
   fi
  fi
 fi
fi

# Top250 Section.

if [ $SORT_BY_TOP250 -eq 1 ]; then
 if [ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_TOP250_NAME" ]; then
  mkdir -p "$GLROOT$SYMLINK_PATH/$SORT_BY_TOP250_NAME"
  chmod 777 "$GLROOT$SYMLINK_PATH/$SORT_BY_TOP250_NAME"
 fi

# Cleanup TOP250-dirs
 if [ $CLEANUP_SYMLINKS -eq 1 ]; then
  DIRTOP250S="`ls -F "$GLROOT$SYMLINK_PATH/$SORT_BY_TOP250_NAME/" | grep "/" | tr ' ' '%'`"
  if [ ! -z "$DIRTOP250S" ]; then
   for TOP250S in $DIRTOP250S; do
    TOP250S="`echo $TOP250S | tr '%' ' '`"
    for TOP250_SYM in `ls -lF "$GLROOT$SYMLINK_PATH/$SORT_BY_TOP250_NAME/$TOP250S" | grep ">" | cut -d '>' -f2 | tr ' ' '%'`; do
     TOP250_SYM="`echo "$TOP250_SYM" | tr '%' ' ' | awk '{for (i = 1; i <= NF; i++) print $i}' | tr '\n' ' '`"
     let TOP250_SYM_CNT="`echo "$TOP250_SYM" | wc -c`-2"
     TOP250_SYM="`echo "$TOP250_SYM" | cut -c 1-$TOP250_SYM_CNT`"
     if [ ! -e "$GLROOT$TOP250_SYM" ]; then
      rm -f  "$GLROOT$SYMLINK_PATH/$SORT_BY_TOP250_NAME/$TOP250S`basename "$TOP250_SYM"`"
     fi
    done
    rmdir "$GLROOT$SYMLINK_PATH/$SORT_BY_TOP250_NAME/$TOP250S" >/dev/null 2>&1
   done
  fi
 fi

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
  IMDBNAMENEW="`echo "$IMDBNAME" | tr -sc 'A-Za-z0-9_\-()' '.'`"
  SECTION="`echo "$IMDBRELPATH" | tr ' ' '_' | tr '/' ' ' | wc -w | tr -d ' '`"
  SECTIONNAME="`echo "$IMDBRELPATH" | cut -d '/' -f $SECTION`"
  TOP250="`echo "$TOP250R"".-.""$IMDBNAME"".(.""$IMDBYEAR"".)" | tr -sc 'A-Za-z0-9_\-()\n' '.'`"
  if [ -z "`ls -1F "$GLROOT$SYMLINK_PATH/$SORT_BY_TOP250_NAME" | grep -e "^$TOP250R.-.$IMDBNAMENEW"`" ]; then
   rm -fr "$GLROOT$SYMLINK_PATH/$SORT_BY_TOP250_NAME/"$TOP250R.-.*
   rm -fr "$GLROOT$SYMLINK_PATH/$SORT_BY_TOP250_NAME/"*.-.$IMDBNAMENEW.*
  fi
  if [ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_TOP250_NAME/$TOP250" ]; then
   mkdir -p "$GLROOT$SYMLINK_PATH/$SORT_BY_TOP250_NAME/$TOP250"
   chmod 777 "$GLROOT$SYMLINK_PATH/$SORT_BY_TOP250_NAME/$TOP250"
  fi
  if [ ! -L "$GLROOT$SYMLINK_PATH/$SORT_BY_TOP250_NAME/$TOP250/$IMDBDIRNAME" ]; then
   ln -s "$IMDBRELPATH" "$GLROOT$SYMLINK_PATH/$SORT_BY_TOP250_NAME/$TOP250/$IMDBDIRNAME"
  fi
 fi
fi

# Keyword Section.

if [ ! -z "$SORT_BY_KEYWORD_LIST" ]; then
 if [ $SORT_BY_KEYWORD -eq 1 ]; then
  for KEYWORD_PAIR in $SORT_BY_KEYWORD_LIST; do
   KEYWORD_SEARCH=`echo $KEYWORD_PAIR | cut -d '|' -f 1 | tr 'A-Z' 'a-z'`
   KEYWORD_REPLACE=`echo $KEYWORD_PAIR | cut -d '|' -f 2 | tr -d '\\/\"&'`
   SORT_BY_KEYWORD_REPLACED=`echo $SORT_BY_KEYWORD_NAME | sed "s/%KEYWORD%/$KEYWORD_REPLACE/g"`
   if [ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_KEYWORD_REPLACED" ]; then
    mkdir -p "$GLROOT$SYMLINK_PATH/$SORT_BY_KEYWORD_REPLACED"
    chmod 777 "$GLROOT$SYMLINK_PATH/$SORT_BY_KEYWORD_REPLACED"
   fi
 
# Cleanup Keyword-dirs
   if [ $CLEANUP_SYMLINKS -eq 1 ]; then
    for KEYWORD_SYM in `ls -lF "$GLROOT$SYMLINK_PATH/$SORT_BY_KEYWORD_REPLACED/" | grep ">" | cut -d '>' -f 2 | tr ' ' '%'`; do
     KEYWORD_SYM="`echo "$KEYWORD_SYM" | tr '%' ' ' | awk '{for (i = 1; i <= NF; i++) print $i}' | tr '\n' ' '`"
     let KEYWORD_SYM_CNT="`echo "$KEYWORD_SYM" | wc -c`-2"
     KEYWORD_SYM="`echo "$KEYWORD_SYM" | cut -c 1-$KEYWORD_SYM_CNT`"
     if [ ! -e "$GLROOT$KEYWORD_SYM" ]; then
      rm -f "$GLROOT$SYMLINK_PATH/$SORT_BY_KEYWORD_REPLACED/`basename "$KEYWORD_SYM"`"
     fi
    done
   fi
  
  # Make link if needed
   if [ ! $ISEXEMPT -eq 1 ]; then
    if [ ! -z "`echo $IMDBDIRNAME | tr '\.\ \-' '\n' | tr 'A-Z' 'a-z' | grep -e "^$KEYWORD_SEARCH$"`" ]; then
     if [ ! -d "$GLROOT$SYMLINK_PATH/$SORT_BY_KEYWORD_REPLACED/" ]; then
      mkdir -p "$GLROOT$SYMLINK_PATH/$SORT_BY_KEYWORD_REPLACED/"
      chmod 777 "$GLROOT$SYMLINK_PATH/$SORT_BY_KEYWORD_REPLACED/"
     fi
     if [ ! -L "$GLROOT$SYMLINK_PATH/$SORT_BY_KEYWORD_REPLACED/$IMDBDIRNAME" ]; then
      ln -s "$IMDBRELPATH" "$GLROOT$SYMLINK_PATH/$SORT_BY_KEYWORD_REPLACED/$IMDBDIRNAME"
     fi
    fi
   fi
  done
 fi
fi

# You should always exit with a 0 - the parent won't give a crap anyway,
# but just to make sure...
exit 0
