#!/bin/bash

# psxc-imdb-nuker
# this script will nuke releases based on imdb score.
#####################################################
#
# This script first appeared with psxc-imdb v2.0k

# Config
########
# Any of the following variables put in psxc-imdb.conf will override what is
# put in here. The only "needed" variable is the variable describing the path
# to psxc-imdb.conf. Fyi.
# After you're done configging, run the script from shell. It will test a
# few variables/settings and report back to you.
#############################################################################

# Version number. No need to change
VERSION=2.9i

# glftpd's root dir
GLROOT=/glftpd

# The location of psxc-imdb.conf. This is the full path.
IMDB_CONF=$GLROOT/etc/psxc-imdb.conf
#IMDB_CONF=$GLROOT/bin/psxc-imdb.conf

# What imdb score should be the minimum *allowed* on site? For now no decimals
# is allowed.
#MIN_SCORE=6
MIN_SCORE=0

# Nuke message for score nukes.
MIN_SCORE_MSG="Too low iMDB score"

# On the other hand, you can choose to nuke the release if there is less than
# a certain number of votes on the movie, no matter how high it scored.
#NUKE_VOTES=100
NUKE_VOTES=0

# Nuke message for votes nuke.
NUKE_VOTES_MSG="Too few votes on iMDB"

# Some sites allow only movies after a certain year. Here's an option to
# auto-nuke movies older than the following year.
#NUKE_YEAR=2000
NUKE_YEAR=0

# Nuke message for year nukes.
NUKE_YEAR_MSG="Movie is too old"

# Some people may want to nuke limited releases. Or only allow movies with
# a certain number of screens. Here's a way to do so. Less than the following
# number will be nuked.
#NUKE_SCREENS=500
NUKE_SCREENS=0

# Nuke message for screens nuke.
NUKE_SCREENS_MSG="Limited movies not allowed."

# You may have banned some genres. Enter here the genres wish to nuke. Please
# use care when entering the names of the genres - they are not case sensitive,
# but they have to match what iMDB uses.
# For a list of genres, look here: http://us.imdb.com/Sections/Genres/
# The following will nuke movies having one of the listed genres, even if it
# has more genres listed.

NUKE_GENRES=""
#NUKE_GENRES="drama musical film-noir"

# If you just want to nuke movies which have a single genre listed, enter the
# banned genres here.
NUKE_SINGLE_GENRE=""
#NUKE_SINGLE_GENRE="drama documentary"

# To compensate, you may wish to always allow certain genres, even if one of
# the genres in the movie is listed as banned. If you ban drama, but wish all
# sci-fi / drama movies to be allowed, enter sc-fi here.
NO_NUKE_GENRES=""
#NO_NUKE_GENRES="sci-fi action adventure"

# Nuke message for genres nuked. You can use NUKEGENRE as the name of the
# genre the release is nuked on.
NUKE_GENRES_MSG="NUKEGENRE movies not allowed."

# What should be the multiplier of these nukes?
MULTIPLIER=3
#MULTIPLIER=2

# You can select a number of groups not to nuke here (affils etc). Separate
# the groupname with a space.
NO_NUKE_GROUP="siteops affils"
#NO_NUKE_GROUP=""

# You can also exclude a number of people from being nuked. Separate list by
# a space.
NO_NUKE_USER="sitop pr0nking"
#NO_NUKE_USER=""

# You can also include dirs here which holds the name of all affils. This is
# perhaps the best option as you can add and remove affils dynamically without
# editing this config all the time. The listed groupnames must be directories.
# The dirs listed must be relative to GLROOT in psxc-imdb.conf
NO_NUKE_AFFIL_DIR="/site/AFFILS /site/GROUPS"
#NO_NUKE_AFFIL_DIR=""

# You can also enter a list of groups which release movies you never want to
# nuke, no matter how bad score, votes etc.
NO_NUKE_RELGROUP="CPY QSP"
#NO_NUKE_RELGROUP=""

# If you wish to keep all releases uploaded in certain dirs, enter them here.
# Paths are relative to GLROOT
NO_NUKE_DIR="/site/ARCHIVE /site/PRIVATE"
#NO_NUKE_DIR=""

# The location of glftpd.conf. This is the full path.
GLFTPD_CONF=/glftpd/etc/glftpd.conf
#GLFTPD_CONF=/etc/glftpd.conf

# Since nuking require root privileges, or that the nuker binary has the set-UID-bit
# set (chmod +s /glftpd/bin/nuker), You can instead of performing the nuke in this
# script log the offending dirs to a file. Then you can use a crontab'ed script to
# do the actual nuking, or whatever else you prefer.
# Either set the name of a logfile here, or set this empty to do nukes on the fly.
# If you want to use CRON_CRACE (see below) you must put a logfile here.
# Also check NUKE_WARN below.
# The syntax of the logfile is as follows:
# <date> <dir relative to $GLROOT> <multiplier> <message>
NUKE_LOGFILE="$GLROOT/ftp-data/logs/imdb-nukes.log"
#NUKE_LOGFILE=""

# Putting the following variables non-empty will stop nuke on the fly - instead, a
# msg will be put in a logfile, using standard glftpd.log format.
# This warning will not stop any logging done to a file in NUKE_LOGFILE
# The file is *not* relative to GLROOT.
# The message uses the following keywords:
# RELNAME   - name of the release directory
# REASON    - the reason why the release will be nuked. See nuke msg's above.
# MOVIENAME - Name of the movie.
# BOLD      - Bold characters on/off.
NUKE_WARN_FILE=$GLROOT/ftp-data/logs/glftpd.log
#NUKE_WARN_FILE=""

NUKE_WARN_MSG="[ BOLDIMDB NUKEWARNINGBOLD ] : [ BOLDRELNAMEBOLD ] is a nukee - REASON"
#NUKE_WARN_MSG=""

NUKE_WARN_TRIGGER="UPDATE:"
#NUKE_WARN_TRIGGER="RAW:"

# There are some accept scripts out there, who add a file/dir in the release-dir.
# If you have one of these, put here a word which the dir/file created by the
# accept script uses.
REL_ACCEPT_WORD="approved"
#REL_ACCEPT_WORD=""

# Should subdirs in the releasedir be checked? Will test for cd, dvd, disk and
# disc directories. If the .nfo is in one of these, no nuke will occur.
CHECK_SUB="NO"
#CHECK_SUB=""

# In case you do nuking on the fly, what user should be the one doing the nuking?
# This has got to be a valid user in glftpd's password list.
NUKER_PERSON="sitebot"

# Sometimes an imdb lookup fails. For some moderate protection against this (you
# do not wish to autonuke a release if the imdb script haven't found the data)
# To safeguard from this you should keep the following variable set to 1
NUKE_SAFEGUARD=1
#NUKE_SAFEGUARD=0

# Sometimes you wish to nuke a movie if it's in more than one nuke-cathegory, but
# not if it's in just one. For instance, you wish to nuke it if it's limited and
# the score is below 6, but not if it's just one. I call this a nuke combo of two
# (2). You can set here how many combinations is needed for the release to be set
# as a nuke. Default is one (1).
NUKE_COMBO=1

# You can add this script to crontab, to allow for a nukewarning in channel
# and an auto-nuke later.
# There is no need to change this unless you plan to use this feature.
# How many seconds should be allowed as grace-time before a release is auto-nuked?
# Default is 10 minutes (600 seconds).
# Note that if the release is accepted in the meantime, the release will *not* be
# nuked.
CRON_GRACE=600

# End of config
####################################################################################

# Let's load some variables from psxc-imdb.conf
. $IMDB_CONF

####################################################################################
# The following grabs exported variables. The code is taken from psxc-imdb-parser.sh
# found in the extras/ dir.

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

############################################################

###### Let's start

# First, a couple of test to make sure we got the variables. These should be better.
if [ -z "$GLFTPD_CONF" ]; then
 echo "config error. check variables. (glftpd.conf)"
 exit 0
fi
if [ -z "$NUKE_LOGFILE" ] && [ -z "$NUKE_WARN_MSG" ]; then
 if [ -z "`ls -la $GLROOT/bin/nuker | grep "-" | awk '{print $1}' | tr -d 'rwx-'`" ]; then
  echo "config error. check variables. the 'nuker' binary is not +s."
  exit 0
 fi
 if [ ! `ls -lan $GLROOT/bin/nuker | awk '{print $3}'` -eq 0 ]; then
  echo "config error. the 'nuker' binary is not owned by root."
  exit 0
 fi
 GL_DATAPATH="`cat $GLFTPD_CONF | grep -e "^datapath" | awk '{print $2}'`"
 if [ -z "$GL_DATAPATH" ]; then
  GL_DATAPATH="/ftp-data"
 fi
 if [ ! -e $GLROOT$GL_DATAPATH/users/$NUKER_PERSON ]; then
  echo "config error. check variables. could not find $NUKER_PERSON in list of users."
  exit 0
 fi
fi
if [ ! -z "$NUKE_LOGFILE" ]; then
 if [ ! -e $NUKE_LOGFILE ]; then
  touch $NUKE_LOGFILE || { 
   echo "config error. could not create $NUKE_LOGFILE."; exit 0;
  };
  chmod 777 $NUKE_LOGFILE || {
   echo "config error. could not chmod $NUKE_LOGFILE."; exit 0;
  };
 fi
 if [ ! -w $NUKE_LOGFILE ]; then
  echo "config error. unable to write to $NUKE_LOGFILE."; exit 0;
 fi
fi
if [ ! -z "$NUKE_WARN_FILE" ] && [ ! -z"$NUKE_WARN_MSG" ]; then
 if [ ! -w $GLROOT/$NUKE_WARN_FILE ]; then
  echo "config error. unable to write to $NUKE_WARN_FILE."; exit 0
 fi
fi

# Zero the nuke-combo
COMBO=0

# Let's check to see if this script is run independently
if [ -z "$IMDBRELPATH" ]; then
 echo "Looks like you've run this as a standalone script."
 echo "Please look at the docs to see how to implement it."
 echo "FYI, the script's setup looks okay."
 echo "To use a delayed nuke, see NUKE_GRACE and add the"
 echo "following to crontab:"
 echo "* * * * * /glftpd/bin/psxc-imdb-nuker.sh >/dev/null 2>&1"
 let COMBO=NUKE_COMBO+1
else

# finally, let's see if the release still exists. Maybe it's already been
# moved by a different script?
 if [ ! -e "$GLROOT$IMDBRELPATH" ]; then
  exit 0
 fi

##### From here on I will assume the needed bins exists, and the correct permissions
##### are set. The above should've catched most of such errors.

# First, let's make sure the variables we use are non-empty.
 if [ -z "`echo "$IMDBSCORE" | tr '. ' '\n' | grep -v "^$" | head -n 1`" ]; then
  if [ $NUKE_SAFEGUARD -ne 0 ]; then
   IMDBSCORE=0
  else
   IMDBSCORE=10
  fi
 fi
 if [ -z "`echo "$IMDBVOTES" | tr -cd '0-9'`" ]; then
  if [ $NUKE_SAFEGUARD -ne 0 ]; then
   IMDBVOTES=0
  else
   IMDBVOTES=9999999
  fi
 fi
 if [ -z "`echo "$IMDBNUMSCREENS" | tr -cd '0-9'`" ]; then
  IMDBNUMSCREENS=0
 fi
 if [ -z "`echo "$IMDBYEAR" | tr -cd '0-9'`" ]; then
  if [ $NUKE_SAFEGUARD -ne 0 ]; then
   IMDBYEAR=0
  else
   exit 0
  fi
 fi
 if [ -z "$NUKE_COMBO" ]; then
  NUKE_COMBO=1
 fi

# Let's do a quick test to see if the release should be nuked
 NUKE_REASON=""
 if [ "`echo "$IMDBSCORE" | tr -cd '0-9'`" != "" ]; then
  if [ `echo "$IMDBSCORE" | tr '. ' '\n' | grep -v "^$" | head -n 1` -lt $MIN_SCORE ]; then
   NUKE_REASON="$MIN_SCORE_MSG"
   let COMBO=COMBO+1
  fi
 fi
 if [ "`echo "$IMDBVOTES" | tr -cd '0-9'`" != "" ]; then
  if [ `echo "$IMDBVOTES" | tr -cd '0-9'` -lt $NUKE_VOTES ]; then
   NUKE_REASON="$NUKE_VOTES_MSG"
   let COMBO=COMBO+1
  fi
 fi
 if [ `echo "$IMDBYEAR" | tr -cd '0-9'` -lt $NUKE_YEAR ]; then
  NUKE_REASON="$NUKE_YEAR_MSG"
  let COMBO=COMBO+1
 fi
 if [ "`echo "$IMDBNUMSCREENS" | tr -cd '0-9'`" != "" ]; then
  if [ `echo "$IMDBNUMSCREENS" | tr -cd '0-9'` -lt $NUKE_SCREENS ]; then
   NUKE_REASON="$NUKE_SCREENS_MSG"
   let COMBO=COMBO+1
  fi
 fi
 GEN_NUKE_REASON=""
 if [ ! -z "$IMDBGENRE" ]; then
  MYGENRES="`echo "$IMDBGENRE" | tr 'A-Z' 'a-z' | tr -d '/'`"
  if [ ! -z "$NUKE_GENRES" ]; then
   for NUKE_GENRE in $NUKE_GENRES; do
    if [ ! -z "`echo "$MYGENRES" | grep -e "$NUKE_GENRE"`" ]; then
     GEN_NUKE_REASON="`echo "$NUKE_GENRES_MSG" | sed "s|NUKEGENRE|$NUKE_GENRE|g"`"
    fi
   done
  fi
  if [ ! -z "$NUKE_SINGLE_GENRE" ]; then
   MYGENRESINGLE="`echo "$MYGENRES" | tr -cd 'a-z\-'`"
   for NUKE_GENRE in $NUKE_SINGLE_GENRE; do
    if [ ! -z "`echo "$MYGENRESINGLE" | grep -e "$NUKE_GENRE"`" ]; then
     GEN_NUKE_REASON="`echo "$NUKE_GENRES_MSG" | sed "s|NUKEGENRE|$NUKE_GENRE|g"`"
    fi
   done
  fi
  if [ ! -z "$NO_NUKE_GENRES" ]; then
   for NUKE_GENRE in $NO_NUKE_GENRES; do
    if [ ! -z "`echo "$MYGENRES" | grep -e "$NUKE_GENRE"`" ]; then
     GEN_NUKE_REASON=""
    fi
   done
  fi
 fi
 if [ ! -z "$GEN_NUKE_REASON" ]; then
  let COMBO=COMBO+1
  NUKE_REASON="$GEN_NUKE_REASON"
 fi

# If there's no reason to nuke, let's just exit
 if [ -z "$NUKE_REASON" ]; then
  exit 0
 fi

# Before we nuke it, let's check for reasons not to.
# First, let's make sure we're not in an affil/pre/group/private dir

 if [ ! -z "$BOTEXEMPT" ]; then
  for EXEMPT in $BOTEXEMPT; do
   if [ ! -z "`echo "$IMDBRELPATH" | grep -e "$EXEMPT"`" ]; then
    exit 0
   fi
  done
 fi

# Let's see if the releasedir was created by someone unnukable

 if [ ! -z "$NO_NUKE_USER" ]; then
  USER_ID=`ls -lan "$GLROOT/$IMDBRELPATH" | grep -e "\ \.$" | awk '{print $3}'`
  USER_NAME="`cat $GLROOT/etc/passwd | tr -d ' ' | tr ':' ' ' | awk '{print $1,$3}' | grep -e "\ $USER_ID$" | awk '{print $1}'`"
  if [ ! -z "`echo "$NO_NUKE_USER" | grep $USER_NAME`" ]; then
   exit 0
  fi
 fi

# Perhaps it was uploaded by someone in a special group?

 if [ ! -z "$NO_NUKE_GROUP" ]; then
  GROUP_ID=`ls -lan "$GLROOT/$IMDBRELPATH" | grep -e "\ \.$" | awk '{print $4}'`
  GROUP_NAME="`cat $GLROOT/etc/group | tr -d ' ' | sed "s|::|:vomit:|g" | tr ':' ' ' | awk '{print $1,$3}' | grep -e "\ $GROUP_ID$" | awk '{print $1}'`"
  if [ ! -z "`echo "$NO_NUKE_GROUP" | grep $GROUP_NAME`" ]; then
   exit 0
  fi
 fi

# Maybe it's one of the affils doing the upload?

 if [ ! -z "$NO_NUKE_AFFIL_DIR" ]; then
  AFFIL_ID=`ls -lan "$GLROOT/$IMDBRELPATH" | grep -e "\ \.$" | awk '{print $4}'`
  AFFIL_NAME="`cat $GLROOT/etc/group | tr -d ' ' | sed "s|::|:vomit:|g" | tr ':' ' ' | awk '{print $1,$3}' | grep -e "\ $AFFIL_ID$" | awk '{print $1}'`"
  for AFFIL_DIR in $NO_NUKE_AFFIL_DIR; do
   if [ -d $GLROOT/$AFFIL_DIR ]; then
    if [ ! -z "`ls -lF $GLROOT/$AFFIL_DIR | grep -e "/" | grep -e "\ $AFFIL_NAME/"`" ]; then
     exit 0
    fi
   fi
  done
 fi

# or maybe the releasegroup is someone we like?

 if [ ! -z "$NO_NUKE_RELGROUP" ]; then
  RELGROUP="`echo "$IMDBDIRNAME" | tr '-' '\n' | grep -v "^$" | tail -n 1`"
  if [ ! -z "`echo "$NO_NUKE_RELGROUP" | grep -e "$RELGROUP"`" ]; then
   exit 0
  fi
 fi

# how about the location of the release?

 if [ ! -z "$NO_NUKE_DIR" ]; then
  for DIR_NAME in $NO_NUKE_DIR; do
   if [ ! -z "`echo "$IMDBRELPATH" | grep -e "$DIR_NAME"`" ]; then
    exit 0
   fi
  done
 fi

# Are we in a sub directory?
 if [ ! -z "$CHECK_SUB" ]; then
  MYPATH=`echo "$IMDBRELPATH" | tr 'A-Z' 'a-z' | tr -s '0-9 ' '0'`
  MYPATH=`basename "$MYPATH"`
  if [ ! -z "`echo "$MYPATH" | grep -e "^cd[0a-z]$"`" ] || [ ! -z "`echo "$MYPATH" | grep -e "^dvd[0a-z]$"`" ] || [ ! -z "`echo "$MYPATH" | grep -e "^dis[ck][0a-z]$"`" ]; then
   exit 0
  fi
 fi
fi

# Check for crontab job
if [ -z "$IMDBRELPATH" ]; then
 if [ -z "$NUKE_LOGFILE" ]; then
  exit 0
 else
  if [ ! -z $NUKE_LOGFILE ]; then
   if [ ! -e $NUKE_LOGFILE ]; then
    echo "could not find $NUKE_LOGFILE"
    exit 0
   fi
   CHECK_NUKE="`cat "$NUKE_LOGFILE" | grep -e "/" | head -n 1`"
   if [ -z "$CHECK_NUKE" ]; then
    exit 0
   fi
   CHECK_NUKE_LOG="`cat "$NUKE_LOGFILE" | grep -v "$CHECK_NUKE" | grep -v "^$"`"
   CHECK_NUKE_DATE="`echo "$CHECK_NUKE" | cut -d '%' -f 1 | tr -cd '0-9'`"
   if [ -z "$CHECK_NUKE_DATE" ]; then
    echo "$CHECK_NUKE_LOG" >$NUKE_LOGFILE
    exit 0
   fi
   CHECK_NUKE_DIR="`echo "$CHECK_NUKE" | cut -d '%' -f 2`"
   if [ -z "$CHECK_NUKE_DIR" ]; then
    echo "$CHECK_NUKE_LOG" >$NUKE_LOGFILE
    exit 0
   fi
   CHECK_NUKE_MULT=`echo "$CHECK_NUKE" | cut -d '%' -f 3 | tr -cd '0-9'`
   if [ -z "$CHECK_NUKE_MULT" ]; then
    echo "$CHECK_NUKE_LOG" >$NUKE_LOGFILE
    exit 0
   fi
   CHECK_NUKE_MSG="`echo "$CHECK_NUKE" | cut -d '%' -f 4-`"
   let CHECK_DIFF="`date +%s` - $CHECK_NUKE_DATE"
   if [ $CHECK_DIFF -ge $CRON_GRACE ]; then
    echo "$CHECK_NUKE_LOG" >$NUKE_LOGFILE
    if [ -d "$GLROOT$CHECK_NUKE_DIR" ]; then
     NUKE_WARN_FILE=""
     NUKE_LOGFILE=""
     NUKE_REASON="$CHECK_NUKE_MSG"
     IMDBRELPATH="$CHECK_NUKE_DIR"
     MULTIPLIER=$CHECK_NUKE_MULT
    else
     exit 0
    fi
   else
    exit 0
   fi
  fi
 fi
fi
###################### Last check before nuke ######################

# last, let's check the dir for any approved info
if [ ! -z "$REL_ACCEPT_WORD" ]; then
 MYPATH="`ls -1 $GLROOT$IMDBRELPATH | tr 'A-Z' 'a-z'`"
 MYWORD="`echo "$REL_ACCEPT_WORD" | tr 'A-Z' 'a-z'`"
 if [ ! -z "`echo "$MYPATH" | grep -e "$MYWORD"`" ]; then
  exit 0
 fi
fi

# Check combo-points
if [ $NUKE_COMBO -gt $COMBO ]; then
 exit 0
fi

# Release is a candidate for nuking. Now all that needs be done is either
# nuke it, or log it.

if [ -z "$NUKE_WARN_FILE" ] && [ -z "$NUKE_LOGFILE" ]; then
 $GLROOT/bin/nuker -r $GLFTPD_CONF -N $NUKER_PERSON -n {$IMDBRELPATH}/ $MULTIPLIER $NUKE_REASON >/dev/null 2>&1
else
 if [ ! -z "$NUKE_LOGFILE" ]; then
  echo "`date +%s`""%""$IMDBRELPATH""%""$MULTIPLIER""%""$NUKE_REASON" >> $NUKE_LOGFILE
 fi
 if [ ! -z "$NUKE_WARN_FILE" ]; then
  NUKE_WARN_MSG="`echo "$NUKE_WARN_MSG" | sed "s%RELNAME%$IMDBDIRNAME%g" | sed "s%REASON%$NUKE_REASON%g" | sed "s%MOVIENAME%$IMDBNAME%g" | sed "s%BOLD%$BOLD%g"`" 
  echo "$DATE $NUKE_WARN_TRIGGER \"$IMDBRELPATH\" \"$NUKE_WARN_MSG\"" >> $NUKE_WARN_FILE
 fi
fi
# You should always exit with a 0 - the parent won't give a crap anyway,
# but just to make sure...
exit 0

