#!/bin/bash

# PSXC IMDB INFO #
##################

# Just edit the 2 lines below, then continue on the "real" config file.

# your glftpd root path.
GLROOT=/glftpd

# path to the config file when chrooted by glftpd.
CONFFILE=/etc/psxc-imdb.conf

## End of config ##
###################

# version number. do not change.
VERSION="v2.9v"

######################################################################################################

RECVDARGS="$1"
# check if configfile exists
############################

if [ -r $GLROOT$CONFFILE ]; then
 . $GLROOT$CONFFILE
 if [ $? -ne 0 ]; then
  echo "Unable to open config file ($GLROOT$CONFFILE). Forced to exit."
  exit 0
 fi
elif [ -r $CONFFILE ]; then
 . $CONFFILE
 if [ $? -ne 0 ]; then
  echo "Unable to open config file ($CONFFILE). Forced to exit."
  exit 0
 fi
else
 echo "Config file not found. Forced to exit."
 exit 0
fi

# Start debugging
if [ "$DEBUG" = "ON" ] || [ "$DEBUG" = "2" ]; then
 set -x
elif [ "$DEBUG" = "3" ]; then
 set -x -v
elif [ "$DEBUG" = "4" ]; then
 set -x -v
fi

# Let's hack glftpd
if [ -z "$RECVDARGS" ] && [ ! -z "$GLFIX" ]; then
 RECVDARGS=$(ls -1Ft | grep -a -v "/" | grep -a -v "@" | head -n 1 | grep -a -e "[.][nN][fF][oO]$")
fi

# Remove locale settings which might cause problems
export LC_ALL=""
export LANG=""

if [ ! -z "$RECVDARGS" ]; then

# This is what is run under zs-c, chrooted.
###########################################

# PATH=$PATHCHROOTED
# IMDBLOG=$IMDBLOGCHROOTED
 FILENAME="$RECVDARGS"
 case $FILENAME in
  *.[nN][fF][oO])
   if [ ! -z "$GLROOT" ]; then
    MYTMPFILE=$(echo "$TMPRESCANFILE" | sed "s%$GLROOT%%")
   else
    MYTMPFILE=$TMPRESCANFILE
   fi
   PSXCFLAG=$(head -n 1 $MYTMPFILE | tr -cd '0-9')
   if [ ! -z "$PSXCFLAG" ]; then
    if [ $PSXCFLAG -ge 4 ]; then
     let PSXCFLAG=PSXCFLAG-4
    fi
    if [ $PSXCFLAG -ge 2 ]; then
     DOTIMDB=""
     INFOTEMPNAME=""
     DOTDATE=""
     DOTURL=""
    fi
   fi
   if [ ! -z "$DOTDATE" ]; then
    DOTDATEINFO="$(grep -a [Dd][Aa][Tt][Ee] $FILENAME | tr -c '/a-zA-Z0-9:. -/\n' ' ' | tr -s ' ')"
    if [ ! -z "$DOTDATEINFO" ]; then
     echo "$DOTDATEINFO" > $DOTDATE
     chmod 666 $DOTDATE
    fi
   fi

# Should we even begin searching for an url?
   SEARCHFORURLS=0
   if [ -z "$SCANDIRS" ]; then
    SEARCHFORURLS=1
   fi
   for SCANDIR in $SCANDIRS; do
    if [ ! -z "$(pwd | grep -a "$SCANDIR")" ]; then
     SEARCHFORURLS=1
     break
    fi
   done
   if [ $SEARCHFORURLS -eq 0 ]; then
    exit 0
   fi

# First, replace some old variable values
   if [ -z "$RELAXEDURLS" ]; then
    RELAXEDURLS=1
   fi
   if [ "$RELAXEDURLS" = "ON" ]; then
    RELAXEDURLS=3
   fi

# Level 0 search
   IMDBURLS="$(grep -a [Ii][Mm][Dd][Bb] $FILENAME | tr ' \|' '\n' | sed -n /[hH][tT][tT][pP]:[/][/].*[.][iI][mM][dD][bB][.].*.[0-9]/p | head -n 1 | tr -c -d '[:alnum:]\:./?')"
   if [ ! -z "$(echo $IMDBURLS | grep -a "imdb\.")" ]; then
#    IMDBURL="https://www.imdb.com/title/tt""$(echo $IMDBURLS | sed "s/=/-/g" | sed "s/imdb./=/" | cut -d "=" -f 2 | cut -d "/" -f 2,3 | tr -c -d '[:digit:]')"
    IMDBURL="https://www.imdb.com/title/tt""$(echo $IMDBURLS | sed "s/=/-/g" | sed "s/imdb./=/" | cut -d "=" -f 2 |  grep -a -o "[0-9]*" | head -n 1)"
    if [ -z $(echo $IMDBURL | tr -cd '0-9') ]; then
     IMDBURL=""
    fi
   fi

# Level 1 search
   if [ -z "$IMDBURL" ] && [ $RELAXEDURLS -ge 1 ]; then
    IMDBURLS="$(grep -a [Ii][Mm][Dd][Bb] $FILENAME | tr ' \|' '\n' | sed -n /[hH][tT][tT][pP]:[/][/].*[iI][mM][dD][bB][.].*.[0-9]/p | head -n 1 | tr -c -d '[:alnum:]\:./?')"
    if [ ! -z "$(echo $IMDBURLS | grep -a "imdb\.")" ]; then
     IMDBURL="https://www.imdb.com/title/tt""$(echo $IMDBURLS | sed "s/=/-/g" | sed "s/imdb./=/" | cut -d "=" -f 2 | cut -d "/" -f 2,3 | tr -c -d '[:digit:]')"
     if [ -z $(echo $IMDBURL | tr -cd '0-9') ]; then
      IMDBURL=""
     fi
    fi
   fi

# Level 2 search
   if [ -z "$IMDBURL" ] && [ $RELAXEDURLS -ge 2 ]; then
    IMDBURLS="$(grep -a [Ii][Mm][Dd][Bb] $FILENAME | tr ' \|' '\n' | sed -n /.*[iI][mM][dD][bB][.].*.[0-9]/p | head -n 1 | tr -c -d '[:alnum:]\:./?')"
    if [ ! -z "$(echo $IMDBURLS | grep -a "imdb\.")" ]; then
     IMDBURL="https://www.imdb.com/title/tt""$(echo $IMDBURLS | sed "s/=/-/g" | sed "s/imdb./=/" | cut -d "=" -f 2 | cut -d "/" -f 2,3 | tr -c -d '[:digit:]')"
     if [ -z $(echo $IMDBURL | tr -cd '0-9') ]; then
      IMDBURL=""
     fi
    fi
   fi

# Level 3 search
   if [ -z "$IMDBURL" ] && [ $RELAXEDURLS -ge 3 ]; then
    for IMDBURLS in $(grep -a [Ii][Mm][Dd][Bb] $FILENAME | tr -c '[:digit:]' '\n' | grep -a -v "^$"); do
     if [ ! -z $(echo $IMDBURLS | tr -cd '0-9') ]; then
      if [ $(echo $IMDBURLS | tr -cd '0-9' | wc -c) -eq 8 ] || [ $(echo $IMDBURLS | tr -cd '0-9' | wc -c) -eq 7 ]; then
       IMDBURL="$IMDBURLS"
       break
      fi
     fi
    done
    if [ ! -z "$IMDBURL" ]; then
     IMDBURL="https://www.imdb.com/title/tt""$IMDBURL"
    fi
   fi

# Level 4 search
   if [ -z "$IMDBURL" ] && [ $RELAXEDURLS -ge 4 ]; then
    for IMDBURLS in $(cat $FILENAME | tr -c '[:digit:]' '\n' | grep -a -v "^$"); do
     if [ $(echo $IMDBURLS | wc -c) -eq 8 ] || [ $(echo $IMDBURLS | wc -c) -eq 7 ]; then
      IMDBURL="$IMDBURLS"
      break
     fi
    done
    if [ ! -z "$IMDBURL" ]; then
     IMDBURL="https://www.imdb.com/title/tt""$IMDBURL"
    fi
   fi

# export what we've found
   if [ ! -z "$IMDBURL" ]; then
    echo "$IMDBURL""|""$PWD" >> $IMDBLOGCHROOTED
    if [ ! -z "$DOTIMDB" ]; then
     if [ ! -e "$DOTIMDB" ] || [ -w "$DOTIMDB" ]; then
      echo -n "" > $DOTIMDB
      chmod 666 $DOTIMDB
     fi
    fi
    if [ ! -z "$DOTURL" ]; then
     DOTURLF="$(basename "$PWD" | sed "s/ /./g")"
     if [ ! "$DOTURL" = "URL" ]; then
      DOTURLF="$DOTURLF"".imdb.html"
      if [ ! -e "$DOTURLF" ] || [ -w "$DOTURLF" ]; then
       echo "<TITLE>IMDB REDIRECT</TITLE>" > $DOTURLF
       echo "<META HTTP-EQUIV=\"refresh\" CONTENT=\"0;URL=$IMDBURL\">" >> $DOTURLF
       chmod 666 $DOTURLF
      fi
     else
      DOTURLF="$DOTURLF"".imdb.url"
      if [ ! -e "$DOTURLF" ] || [ -w "$DOTURLF" ]; then
       echo "[InternetShortcut]" > $DOTURLF
       echo "URL=""$IMDBURL" >> $DOTURLF
       chmod 666 $DOTURLF
      fi
     fi
    fi
    if [ ! -z "$INFOTEMPNAME" ]; then
     if [ ! -e "$INFOTEMPNAME" ] || [ -w "$INFOTEMPNAME" ]; then
      if [ ! -z "$INFOFILEIS" ]; then
       echo -n "" > $INFOTEMPNAME
      chmod 666 $INFOTEMPNAME
      else
       mkdir -p $INFOTEMPNAME
       chmod 777 $INFOTEMPNAME
      fi
     fi
    fi
   fi
   touch -acmr "$FILENAME" $(pwd) >/dev/null 2>&1
  ;;
 esac
# exit 0

#else
fi
if [ ! -z "$RUNCONTINOUS" ] || [ -z "$RECVDARGS" ]; then
# run major part.

 if [ "$(basename "$0")" = "$PRENAME" ]; then

# This is what is done with pre's
#################################
  PATH=$GLPATHPRE

  a="$(tail -n 5 $GLPRELOG | grep -a "$PRETRIGGER" | tail -n 1)"
  DIRNAME=""
  for WORD in $WORDS; do
   count=0
   combine=0
   if [ -z "$a" ]; then
    exit 0
   fi
   if [ ! -z "$PRETRIGGER" ]; then
    let WORD=WORD+1
   fi
   for b in $a; do
    if [ -z $(echo $b | grep -a "$PRETRIGGER") ] && [ $count -gt 0 ] || [ -z "$PRETRIGGER" ]; then
     if [ $combine -eq 1 ]; then
      c=$c$b
     else
      c=$b
     fi
     if [ ! -z $(echo "$c" | grep -a "^\"") ]; then
      combine=1
     else
      c=$b
      let count=count+1
     fi
     if [ ! -z $(echo "$c" | grep -a "\"$") ]; then
      combine=0
      let count=count+1
     fi
     if [ $count -eq $WORD ]; then
      break
     fi
    else
     if [ "$b" = "$PRETRIGGER" ]; then
      count=1
     fi
    fi
   done
   DIRNAME="$DIRNAME""$c"
  done
  DIRNAME=$(echo $DIRNAME | sed "s|\"\"|$SEPARATOR|g" | sed "s|\"||g")
  if [ -d $GLROOT$DIRNAME ]; then
   FILENAME=$(ls -1 $GLROOT$DIRNAME | grep -a "\.[Nn][Ff][Oo]$" | head -n 1)
   IMDBURL="$(grep -a [Ii][Mm][Dd][Bb] $GLROOT$DIRNAME/$FILENAME | tr ' ' '\n' | sed -n /[hH][tT][tT][pP]:[/][/].*[.][iI][mM][dD][bB].*.[0-9]/p | head -n 1 | tr -c -d '[:alnum:]\:./?')"
   if [ ! -z "$(echo $IMDBURL | grep -a "\.imdb\.")" ]; then
    IMDBURL="https://www.imdb.com/title/tt""$(echo $IMDBURL | sed "s/=/-/g" | sed "s/.imdb./=/" | cut -d "=" -f 2 | cut -d "/" -f 2,3 | tr -c -d '[:digit:]')"
   fi
   if [ ! -z "$IMDBURL" ]; then
    a="$(tail -n 5 $GLLOG | grep -a "$TRIGGER" | grep -a "$DIRNAME" | tail -n 1)"
    if [ -z "$a" ]; then
     SEARCHFORURLS=0
     if [ -z "$SCANDIRS" ]; then
      SEARCHFORURLS=1
     fi
     for SCANDIR in $SCANDIRS; do
      if [ ! -z "$(echo "$DIRNAME" | grep -a "$SCANDIR")" ]; then
       SEARCHFORURLS=1
       break
      fi
     done
     if [ ! $SEARCHFORURLS -eq 0 ]; then
      echo "$IMDBURL""|""$DIRNAME" >> $IMDBLOG
     fi
    fi
   fi
  fi
  if [ -z "$RUNCONTINOUS" ]; then
   exit 0
  fi
 fi

 if [ ! -e $IMDBLOG ]; then

# Check to see if it's a first-run
##################################

  echo "Please read the docs before trying to run this script."
  exit 0
 fi

# The main part.
################

 if [ -z "$(cat $IMDBLOG)" ]; then

# No new imdb-info. let's quit.
###############################
  exit 0
 fi

# Make sure this script isn't already running.
##############################################
 sleep 0.$RANDOM
 IMDBPIDCONTENT="$(head -n2 $IMDBPID | tail -n1)"
 [[ ! -z "$IMDBPIDCONTENT" ]] &&
   [[ -1 -eq "$IMDBPIDCONTENT" || ! -z $(ps ax | awk '{print $1}' | grep -a -e "^$IMDBPIDCONTENT$") ]] &&
     exit 0
 echo $$ > $IMDBPID

# Seems like something was put into the log. Let's check it.
############################################################
 IMDBFLAGS=$(head -n 1 $TMPRESCANFILE | tr -cd '0-9')
 if [ ! -z "$IMDBFLAGS" ]; then
  if [ $IMDBFLAGS -ge 4 ]; then
   EXTERNALSCRIPTNAME=""
   let IMDBFLAGS=IMDBFLAGS-4
  fi
  if [ $IMDBFLAGS -ge 2 ]; then
   DOTIMDB=""
   INFOTEMPNAME=""
   let IMDBFLAGS=IMDBFLAGS-2
  fi
  if [ $IMDBFLAGS -ge 1 ]; then
   USEBOT=""
  fi
 fi

 if [ -z "$LANGUAGENUM" ] || [ $LANGUAGENUM -eq 0 ]; then
  LANGUAGENUM=99
 fi
 if [ -z "$COUNTRYNUM" ] || [ $COUNTRYNUM -eq 0 ]; then
  COUNTRYNUM=99
 fi
 if [ -z "$CERTIFICATIONNUM" ] || [ $CERTIFICATIONNUM -eq 0 ]; then
  CERTIFICATIONNUM=99
 fi
 if [ -z "$CASTNUM" ] || [ $CASTNUM -eq 0 ]; then
  CASTNUM=99
 fi
 if [ -z "$GENRENUM" ] || [ $GENRENUM -eq 0 ]; then
  GENRENUM=99
 fi
 if [ -z "$RUNTIMENUM" ] || [ $RUNTIMENUM -eq 0 ]; then
  RUNTIMENUM=99
 fi
 if [ -z "$DIRECTORNUM" ] || [ $DIRECTORNUM -eq 0 ]; then
  DIRECTORNUM=99
 fi

 while [ ! -z "$(cat $IMDBLOG)" ]; do
  IMDBLINE="$(grep -a -e "/" "$IMDBLOG" | head -n 1)"
  grep -a -F -v "$IMDBLINE" "$IMDBLOG" > $TMPFILE
  cat $TMPFILE > $IMDBLOG
  ISLIMITED=""
  BUSINESS=""
  BUSINESSSHORT=""
  PREMIERE=""
  LIMITED=""
  EXEMPTED=""
  LYNXTRIESORIG=$LYNXTRIES
  IMDBURL="$(echo $IMDBLINE | cut -d "|" -f 1)"
  IMDBLNK="$(echo $IMDBLINE | cut -d "|" -f 2)"
  IMDBDST="$(echo $IMDBLINE | cut -d "|" -f 3)"
  DEBUGCOUNT=1
  if [ ! -z $DEBUG ]; then
   echo "$DEBUGCOUNT : DOTIMDB = '$DOTIMDB'"
   echo "$DEBUGCOUNT : USEBOT = '$USEBOT'"
  fi
  if [ -d "$GLROOT$IMDBLNK" ]; then
   IMDBDIR="$(basename "$IMDBLNK")"
   BASELNK="$(dirname "$IMDBLNK")"
   IMDBLKL="$IMDBLNK"
   IMDBLNK="$GLROOT$IMDBLKL/$DOTIMDB"
  elif [ "$IMDBLNK" = "/dev/null" ]; then
   IMDBLKL="$IMDBLNK"
   DOTIMDB=""
   EXTERNALSCRIPTNAME=""
   INFOTEMPNAME=""
   BOTONELINE=$FINDBOTONELINE
   TRIGGER="$FINDTRIGGER"
   LOGFORMAT="$FINDLOGFORMAT"
   MYOWNFORMAT="$FINDMYOWNFORMAT"
   MYOWNEMPTY="$FINDMYOWNEMPTY"
   if [ ! -z "$PSXCFINDLOG" ] && [ -w $PSXCFINDLOG ]; then
    GLLOG=$PSXCFINDLOG
   fi
  else
   DOTIMDB=""
   USEBOT=""
   EXTERNALSCRIPTNAME=""
   INFOTEMPNAME=""
  fi
  DEBUGCOUNT=2
  if [ ! -z $DEBUG ]; then
   echo "$DEBUGCOUNT : DOTIMDB = '$DOTIMDB'"
   echo "$DEBUGCOUNT : USEBOT = '$USEBOT'"  
  fi
  for EXEMPT in $BOTEXEMPT; do
   if [ ! -z $(echo "$IMDBLKL" | grep -a "$EXEMPT") ]; then
    USEBOT=""
    EXEMPTED="ON"
   fi
  done
  if [ ! -z "$LOGFORMAT" ]; then
   BOTONELINE="YES"
   TAGPLOT=""
   BOTHEAD=""
  fi
  DEBUGCOUNT=3
  if [ ! -z $DEBUG ]; then
   echo "$DEBUGCOUNT : DOTIMDB = '$DOTIMDB'"
   echo "$DEBUGCOUNT : USEBOT = '$USEBOT'"  
  fi
  if [ ! -z $(grep -a "$IMDBURL" "$IMDBURLLOG") ] || [ ! -z $EXEMPTED ]; then
   if [ ! "$IMDBLKL" = "/dev/null" ]; then
    USEBOT=""
   fi
  else
   if [ ! "$IMDBLKL" = "/dev/null" ]; then
    echo "$IMDBURL" >> $IMDBURLLOG
    tail -n $KEEPURLS $IMDBURLLOG > $TMPFILE
    cat $TMPFILE > $IMDBURLLOG
    echo -n "" > $TMPFILE
   fi
  fi
  DEBUGCOUNT=4
  if [ ! -z $DEBUG ]; then
   echo "$DEBUGCOUNT : DOTIMDB = '$DOTIMDB'"
   echo "$DEBUGCOUNT : USEBOT = '$USEBOT'"
  fi

# grab info from web
####################
  OUTPUTTRIES=1
  OUTPUTOK=""
  while [ -z "$OUTPUTOK" ]; do
   while [ $OUTPUTTRIES -le $LYNXTRIESORIG ]; do
    let OUTPUTTRIES=OUTPUTTRIES+1
    OUTPUTOK="OK"
    while [ $LYNXTRIES -gt 0 ]; do
     if [ -z "$USEWGET" ] && [ -z "$USECURL" ]; then
     lynx $LYNXFLAGS $IMDBURL/reference | grep -a -v "^$" | tr '\t' ' ' | tr -s ' ' | tr '\n' '~' | sed "s/\.\.\.~ /... /g" | tr '~' '\n' | sed 's/*/\\\*/' > $TMPFILE 2>&1
      if [ $? = "0" ]; then
       LYNXTRIES=$LYNXTRIESORIG
       break
      else
       let LYNXTRIES=LYNXTRIES-1
       sleep 1
      fi
     else
      if [ ! -z "$USEWGET" ]; then
       #http_proxy=192.168.0.1:8080
       wget $WGETFLAGS -U "Internet Explorer" -O $TMPFILE --timeout=30 $IMDBURL/reference >/dev/null 2>&1
      elif [ ! -z "$USECURL" ]; then
       curl $CURLFLAGS -A "Internet Explorer" -o $TMPFILE --connect-timeout 30 $IMDBURL/reference >/dev/null 2>&1
      fi
      if [ $? = "0" ] || [ -z "$(cat $TMPFILE)" ]; then
       TMBURL=$(grep -a "\.jpg" $TMPFILE | head -n 1 | tr ' \"' '\n' | grep -a "\.jpg" | head -n 1)
       LYNXTRIES=$LYNXTRIESORIG
       HTMLPAGE="$(lynx $LYNXFLAGS -force_html $TMPFILE)"
       echo "$HTMLPAGE" | grep -a -v "^$" | tr '\t' ' ' | tr -s ' ' | tr '\n' '~' | sed "s/\.\.\.~ /... /g" | tr '~' '\n' | sed 's/*/\\\*/' > $TMPFILE 2>&1
       break
      else
       let LYNXTRIES=LYNXTRIES-1
       echo -n "" > $TMPFILE
       sleep 1
      fi
     fi
    done
    if [ $LYNXTRIES -ne $LYNXTRIESORIG ]; then
     exit 0
    fi

# Check for a movie-title. This *must* be present, else the script will just exit.
##################################################################################
    TITLE=$(grep -a -i "^[^ ].* ([0-9][0-9][0-9][0-9])$" "$TMPFILE" | head -n 1 | sed s/\"/$QUOTECHAR/g)
    if [ -z "$TITLE" ]; then
     OUTPUTOK=""
     break
    fi
    ORIGTITLE=$(grep -a -i "^.* (original title)$" "$TMPFILE" | head -n 1 | sed s/\"/$QUOTECHAR/g )

# Grab hold of the info we'll use later. Also do some formatting.
#################################################################

    TITLEYEAR=$(echo $TITLE | tr ' ' '\n' | grep -a -v "^[a-zA-Z]" | grep -a -e "^([12]" | head -n 1)
    TITLENAME=$(echo $TITLE | sed "s| $TITLEYEAR||")
    TITLEYEAR=$(echo $TITLEYEAR | tr -cd '0-9')
    if [ -z "$TITLEYEAR" ]; then
     OUTPUTOK=""
     break
    fi
    if [ ! -z "$USEORIGTITLE" ] && [ ! -z "$ORIGTITLE" ]; then
     TITLENAME="$( echo $ORIGTITLE | sed -e 's/^ *//g' -e 's/ (original title)//' )"
     TITLE="$TITLENAME $TITLEYEAR"
    fi
    GENRE="Genre........: $(sed -n '/^ Genres$/,/^ [^ \*]/p' "$TMPFILE" | sed -n 's/^ \\\* //p' | sed s/\"/$QUOTECHAR/g | sed '/^ \*$/d' | head -n $GENRENUM | tr '\n' '/' | sed "s/[ /]*$//")"
    GENRECLEAN=$(echo $GENRE | sed "s/Genre........: *//")
    RATING="User Rating..: $(grep -a " \\\\\* [0-9][0-9]*\(\.[0-9]\)* (\([0-9]*,\)*[0-9][0-9]*)" "$TMPFILE" | sed "s/^ \\\\\* //" | sed s/\"/$QUOTECHAR/g)"
    if [ "$RATING" = "User Rating..: 0 (0)" ]; then
      RATING="User Rating..: Awaiting 5 votes"
    fi
    RATINGCLEAN=$(echo $RATING | sed "s/User Rating..: *//")
    if [ "$(echo $RATINGCLEAN | grep -a -e "[Ww][Aa][Ii][Tt]")" = "" ]; then
     RATINGVOTES=$(echo $RATINGCLEAN | sed "s/.* (//;s/)//")
     RATINGSCORE=$(echo $RATINGCLEAN | sed "s/ (.*//")
     PLUS="##########"
     MINUS="----------"
     PNUM=$(echo $RATINGSCORE | cut -d '.' -f 1)
     let MNUM=10-PNUM
     if [ $MNUM -eq 0 ]; then
      RATINGBAR="$MINUS"
     elif [ $MNUM -eq 10 ]; then
      RATINGBAR="$PLUS"
     else
      RATINGBAR="$(echo $PLUS | cut -c 1-$PNUM)""$(echo $MINUS | cut -c 1-$MNUM)"
     fi
    else
     RATINGVOTES=""
     RATINGSCORE=""
     RATINGBAR=""
    fi
    TOP=$(grep -a -e "Top Rated Movies: #" "$TMPFILE" | head -n 1 | cut -d '#' -f 2 | tr -cd '0-9')
    BOTTOM=$(grep -a -e "Bottom Rated Movies: #" "$TMPFILE" | head -n 1 | cut -d '#' -f 2 | tr -cd '0-9')
    if [ ! -z "$TOP" ]; then
      RATINGCLEAN="$(echo "$RATINGCLEAN Top 250: #$TOP")"
    fi
    if [ ! -z "$BOTTOM" ]; then
      RATINGCLEAN="$(echo "$RATINGCLEAN Bottom 100: #$BOTTOM")"
    fi
    COUNTRY="Country......: $(sed -n '/^ Country$/,/^ [^ *]/p' "$TMPFILE" | sed -n 's/^ \\\* //p' | sed s/\"/$QUOTECHAR/g | sed '/^ *$/d' | head -n $COUNTRYNUM | tr '\n' '/' | sed "s/[ /]*$//")"
    COUNTRYCLEAN=$(echo $COUNTRY | sed "s/Country......: *//")
    TAGLINE="Tagline......: $(sed -n 's/^ Taglines //p' "$TMPFILE" | sed "s/ See more ..*//" | sed s/\"/$QUOTECHAR/g | head -n 1)"
    TAGLINECLEAN=$(echo $TAGLINE | sed "s/Tagline......: *//")
    LANGUAGE="Language.....: $(sed -n '/^ Language$/,/^ [^ *]/p' "$TMPFILE" | sed -n 's/^ \\\* //p' | sed s/\"/$QUOTECHAR/g | head -n $LANGUAGENUM | tr '\n' '/' | sed "s/[ /]*$//")"
    LANGUAGECLEAN=$(echo $LANGUAGE | sed "s/Language.....: *//")
    # Yeah, this keeps getting worse ;)
    if [ -z $PLOTWIDTH ]; then
      PLOTWIDTH=275
    fi
    PLOT="Plot: "$(sed -n '/^ Plot Summary$/,/\(^ \\\* Plot \(Summary\|Synopsis\)\|Plot Keywords\)$/{//d;p;}' "$TMPFILE" | \
                   sed -e 's/\( \\\* Plot Summary\|Written by .*\)$//' -e '/(.*@.*)/d' | \
                   sed s/\"/$QUOTECHAR/g | sed 's/^\ *//g' | tr -s ' ' | sed "s/ *$//" | \
                   fold -s -w $PLOTWIDTH | sed ':a;N;$!ba;s/\n/\\\\n/g' | sed 's/\(.\{1000\}\).*/\1.../' | grep ^[0-9A-Za-z])""
    PLOTCLEAN=$(echo $PLOT | sed "s/Plot: *//")
    if [ ! -z "$(echo "$PLOTCLEAN" | grep -a -e "\(\ \)\ \(\ \)")" ]; then
     OUTPUTOK=""
     break
    fi
    CERT="Certification: $(sed -n '/^ Certification$/,/^[^ *]/p' "$TMPFILE" | sed -n 's/^ \\\* //p' | sed s/\"/$QUOTECHAR/g | head -n $CERTIFICATIONNUM | tr '\n' '/' | sed "s/[ /]*$//")"
    CERTCLEAN=$(echo $CERT | sed "s/Certification: *//" | tr '/' '\n' | grep -a -e "United States:" | tr -d ' ' | tail -n 1)
    # We get the name twice (due to the image alt-text) so need to remove by counting spaces
    CASTRAW=$(sed -E -n '/^(Cast|Cast verified as complete|Complete, Cast awaiting verification)$/,/^(Directed|Written) by$/{//d;p;}' $TMPFILE | \
              sed -E '/^ (Edit|Rest of cast listed alphabetically:)/d' | sed -n 's/^ //p' | head -n $CASTNUM)
    CAST=""
    OLDIFS=$IFS
    IFS="
"
     # Need newline above so keep " there.
    for CASTN in $CASTRAW; do
     CASTNC=$(echo "$CASTN" | sed 's/ \.\.\..*//' | tr ' ' '\n' | wc -l)
     CASTNC=$(((CASTNC / 2) + 1))
     CAST="$CAST$(echo $CASTN | cut -d' ' -f$CASTNC- | sed s/\"/$QUOTECHAR/g)
"
     # Need newline above so keep " there.
    done
    IFS=$OLDIFS
    # remove trailing newline
    CAST=$(echo "$CAST" | sed '$d')
    CASTCLEAN=$(echo "$CAST" | sed "s/\.\.\..*/|/g" | tr -s '\n' ' ' | sed "s/^\ *//g" | sed "s/\ *$//g" | sed "s/ |/\,/g" | sed "s/,$//")
    CASTLEADNAME="$(echo "$CAST" | head -n 1 | sed 's/\.\.\./\n/' | head -n 1 | tr -s ' ' | sed "s/^\ //g" | sed "s/\ $//g")"
    CASTLEADCHAR="$(echo "$CAST" | head -n 1 | sed 's/\.\.\./\n/' | tail -n 1 | tr -s ' ' | sed "s/^\ //g" | sed "s/\ $//g")"
    COMMENTSHORT="User Reviews: Not supported anymore."
    COMMENTSHORTCLEAN=$(echo $COMMENTSHORT | sed "s/User Reviews: *//")
    COMMENT="Not supported anymore"
    [[ -n "$COMMENT" ]] && COMMENTCLEAN=$(echo "$COMMENT" | sed "s/^\ *//g" | sed "s/\ *$//g" | sed s/\{\}\"/$QUOTECHAR/g | tr '\n' '|' | sed "s/[ /]*$//")
    RUNTIME="Runtime......: $(sed -n '/^ Runtime$/,/^ [^ *]/p' "$TMPFILE" | sed -n 's/^ \\\* //p' | sed s/\"/$QUOTECHAR/g | sed '/^ *$/d' | head -n $RUNTIMENUM | tr '\n' '/' | sed "s/[ /]*$//")"
    RUNTIMECLEAN="$(echo $RUNTIME | sed "s/Runtime......: *//" | tr '/ ' '\n' | sed -e /^$/d | head -n 1 | tr -c -d '[:digit:]')"
    if [ ! -z "$RUNTIMECLEAN" ]; then
     RUNTIMECLEAN="$RUNTIMECLEAN min"
    fi
    DIRECTOR=$(sed -n '/Directed by$/,/^[^ ]/p' "$TMPFILE" | sed '1,2d;$d;s/^ *//' | sed 's/\"/$QUOTECHAR/g' | head -n $DIRECTORNUM | tr '\n' '/' | sed "s/ \.\.\..*//;s/ *$//;s/\/$//")
    DIRECTORCLEAN=$(echo $DIRECTOR)
    if [ ! -z "$(echo "$DIRECTOR" | grep -a -e "\(\ \)\ \(\ \)")" ]; then
     OUTPUTOK=""
     break
    fi
    ONELINE="$(echo "$BOLD$TITLE$BOLD""[$COUNTRY]: $GENRE - $BOLD$RATING$BOLD"" - $IMDBURL" | sed "s/Genre........: //" | sed "s/User Rating..: //" | sed "s/Country......: *//" | tr -s ' ')"
    if [ ! -z "$(echo "$ONELINE" | grep -a -e "\(\ \)\ \(\ \)")" ]; then
     OUTPUTOK=""
     break
    fi
    if [ ! -z "$(echo "$GENRECLEAN" | grep -a -e "with other users on IMDb")" ] || [ ! -z "$(echo "$GENRECLEAN" | grep -a -e "laserdisc details")" ] || [ ! -z "$(echo "$GENRECLEAN" | grep -a -e "(\ )")" ] || [ ! -z "$(echo "$GENRECLEAN" | grep -a -e "\.\.\.")" ] || [ ! -z "$(echo "$GENRECLEAN" | grep -a -e "Binary")" ]; then
     OUTPUTOK=""
     break
    fi
    if [ ! -z "$OUTPUTOK" ]; then
     OUTPUTTRIES=$LYNXTRIESORIG
     break
    fi
   done
   if [ $OUTPUTTRIES -gt $LYNXTRIESORIG ]; then
    break
   fi
  done
  if [ -z "$OUTPUTOK" ]; then
   DOTIMDB=""
   USEBOT=""
   if [ ! -z "$(echo $IMDBLKL | grep -a -e "/dev/null")" ]; then
    if [ -z "$LOGFORMAT" ]; then
     echo "$DATE $TRIGGER \"$IMDBLKL\" \"Failed to fetch iMDB details. Please try again.\" \"$IMDBDST\"" >> $GLLOG
    elif [ "$LOGFORMAT" = "MYOWN" ]; then
     echo "$DATE $TRIGGER \"$IMDBLKL\" \"Failed to fetch iMDB details. Please try again.\" \"$IMDBDST\"" >> $GLLOG
    else
     echo "$DATE $TRIGGER \"$IMDBLKL\" \"\" \"Failed to fetch iMDB details. Please try again.\"" >> $GLLOG
    fi
   else
    rm -f "$GLROOT/$IMDBLKL/$INFOTEMPNAME" >/dev/null 2>&1
    rmdir "$GLROOT/$IMDBLKL/$INFOTEMPNAME" >/dev/null 2>&1
   fi
  else

# We won't check business and release as closely right away... may do so later
##############################################################################
   BUSINESSURL="${IMDBURL}/" # Page has become imdbpro, need to parse from main page
   RELEASEURL="$(echo "$IMDBURL""/releaseinfo" | tr -s '/' | sed "s|:/|://|")"
   if [ ! -z "$USEBUSINESS" ]; then
    while [ $LYNXTRIES -gt 0 ]; do
     if [ -z "$USEWGET" ] && [ -z "$USECURL" ]; then
      lynx $LYNXFLAGS $BUSINESSURL | grep -a -v "^$" | tr '\t' ' ' | tr -s ' ' | tr '\n' '~' | sed "s/:~ /: /g" | tr '~' '\n' | sed 's/*/\\\*/'> $TMPFILE 2>&1
      if [ $? = "0" ]; then
       LYNXTRIES=$LYNXTRIESORIG
       break
      else
       let LYNXTRIES=LYNXTRIES-1
       sleep 1
      fi
     else
      if [ ! -z "$USEWGET" ]; then
       #http_proxy=192.168.0.1:8080
       wget $WGETFLAGS -U "Internet Explorer" -O $TMPFILE --timeout=30 $BUSINESSURL >/dev/null 2>&1
      elif [ ! -z "$USECURL" ]; then
       curl $CURLFLAGS -A "Internet Explorer" -o $TMPFILE --connect-timeout 30 $BUSINESSURL >/dev/null 2>&1
      fi
      if [ $? = "0" ] || [ -z "$(cat $TMPFILE)" ]; then
       LYNXTRIES=$LYNXTRIESORIG
       HTMLPAGE="$(lynx $LYNXFLAGS -force_html $TMPFILE)"
       echo "$HTMLPAGE" | grep -a -v "^$" | tr '\t' ' ' | tr -s ' ' | tr '\n' '~' | sed "s/:~ /: /g" | tr '~' '\n' | sed 's/*/\\\*/' > $TMPFILE
       break
      else
       let LYNXTRIES=LYNXTRIES-1
       echo -n "" > $TMPFILE
       sleep 1
      fi
     fi
    done
    if [ "$LYNXTRIES" = "$LYNXTRIESORIG" ]; then
     BUSINESS=$(sed -n '/^Box Office$/,/^ See more on IMDbPro/p' "$TMPFILE" | sed '1d;$d' | sed s/\"/$QUOTECHAR/g)
     ISLIMITED=""
     [[ -n "$(echo "$BUSINESS" | grep -a "^Opening Weekend.* Limited Release$")" ]] && ISLIMITED="$LIMITEDYES"
     [[ -n "$(echo "$BUSINESS" | grep -a "^Opening Weekend.* Wide Release$")" ]] && ISLIMITED="$LIMITEDNO"
     # IMDBPro only, replaced by BOM below (where possible)
     BUSINESSSHORT=""
     BUSINESSSCREENS=""
     BUSINESSSHORTUSA=$( echo "$BUSINESS" | sed -n -E 's/^Opening Weekend (USA|United States): ([0-9,$]+), ([0-9]{1,2}) ([A-Z][a-z][a-z])[a-z]* ([0-9]{4})/\2 \3 \4 \5/p' )
     BUSINESSSHORTUK=$( echo "$BUSINESS" | sed -n -E 's/^Opening Weekend (UK|United Kingdom): ([0-9,$]+), ([0-9]{1,2}) ([A-Z][a-z][a-z])[a-z]* ([0-9]{4})/\2 \3 \4 \5/p' )
     if [ -z "$BUSINESSSHORTUSA" ]; then
      BUSINESSSHORT="$BUSINESSSHORTUK"
     else
      BUSINESSSHORT="$BUSINESSSHORTUSA"
     fi
    fi
   fi
   if [ ! -z "$USEPREMIERE" ] || [ ! -z "$USELIMITED" ]; then
    while [ $LYNXTRIES -gt 0 ]; do
     if [ -z "$USEWGET" ] && [ -z "$USECURL" ]; then
      lynx $LYNXFLAGS $RELEASEURL | grep -a -v "^$" | tr '\t' ' ' | tr -s ' ' | tr '\n' '~' | sed "s/:~ /: /g" | tr '~' '\n' > $TMPFILE 2>&1
      if [ $? = "0" ]; then
       LYNXTRIES=$LYNXTRIESORIG
       break
      else
       let LYNXTRIES=LYNXTRIES-1
       sleep 1
      fi
     else
      if [ ! -z "$USEWGET" ]; then
       #http_proxy=192.168.0.1:8080
       wget $WGETFLAGS -U "Internet Explorer" -O $TMPFILE --timeout=30 $RELEASEURL >/dev/null 2>&1
      elif [ ! -z "$USECURL" ]; then
       curl $CURLFLAGS -A "Internet Explorer" -o $TMPFILE --connect-timeout 30 $RELEASEURL >/dev/null 2>&1
      fi
      if [ $? = "0" ] || [ -z "$(cat $TMPFILE)" ]; then
       LYNXTRIES=$LYNXTRIESORIG
       HTMLPAGE="$(lynx $LYNXFLAGS -force_html $TMPFILE)"
       echo "$HTMLPAGE" | grep -a -v "^$" | tr '\t' ' ' | tr -s ' ' | tr '\n' '~' | sed "s/:~ /: /g" | tr '~' '\n' | sed 's/*/\\\*/' > $TMPFILE
       break
      else
       let LYNXTRIES=LYNXTRIES-1
       echo -n "" > $TMPFILE
       sleep 1
      fi
     fi
    done
    if [ "$LYNXTRIES" = "$LYNXTRIESORIG" ]; then
     if [ ! -z "$USEPREMIERE" ]; then
      PREMIERE=$(grep -a -e "(premiere)" "$TMPFILE" | head -n 1 | sed "s/ (premiere)//" | sed "s/^\ *//g" | sed "s/\ *$//g" | tr -s ' ' | sed s/\"/$QUOTECHAR/g)
      if [ -z "$PREMIERE" ]; then
       PREMIERE=$(sed -n "/^Release Date$/,+1p" "$TMPFILE" | tail -n 1 | sed "s/^\ *//g" | sed "s/\ *$//g" | tr -s ' ' | sed s/\"/$QUOTECHAR/g)
      fi
      if [ -z "$(echo "$PREMIERE" | tr -cd '0-9')" ]; then
       PREMIERE=""
      fi
     fi
     if [ ! -z "$USELIMITED" ]; then
      LIMITED=$(grep -a -e "(limited)" "$TMPFILE" | head -n 1 | sed "s/ (limited)//" | sed "s/^\ *//g" | sed "s/\ *$//g" | tr -s ' ' | sed s/\"/$QUOTECHAR/g)
     fi
    fi
   fi

# Get screens/islimited from parsing Box Office Mojo page (source: slftp) 
##############################################################################

   if [ ! -z "$USEBOM" ]; then
    BOMURL="https://www.boxofficemojo.com/title/$(echo "$IMDBURL" | grep -Pow "tt[0-9]+")/"
    while [ $LYNXTRIES -gt 0 ]; do
     if [ -z "$USEWGET" ] && [ -z "$USECURL" ]; then
      lynx -source $LYNXFLAGS $BOMURL > $TMPFILE 2>&1
      if [ $? = "0" ]; then
       LYNXTRIES=$LYNXTRIESORIG
       break
      else
       let LYNXTRIES=LYNXTRIES-1
       sleep 1
      fi
     else
      if [ ! -z "$USEWGET" ]; then
       #http_proxy=192.168.0.1:8080
       wget $WGETFLAGS -U "Internet Explorer" -O $TMPFILE --timeout=30 $BOMURL >/dev/null 2>&1
      elif [ ! -z "$USECURL" ]; then
       curl $CURLFLAGS -A "Internet Explorer" -o $TMPFILE --connect-timeout 30 $BOMURL >/dev/null 2>&1
      fi
      if [ $? = "0" ] || [ -z "$(cat $TMPFILE)" ]; then
       LYNXTRIES=$LYNXTRIESORIG
       break
      else
       let LYNXTRIES=LYNXTRIES-1
       echo -n "" > $TMPFILE
       sleep 1
      fi
     fi
    done
    BOMRELEASEGROUP="$(sed -n -E 's|.*<option value="(/releasegroup/gr[0-9]+/)">Original Release</option>.*|\1|p' $TMPFILE)"
    BOMURLRELEASEGROUP="https://www.boxofficemojo.com${BOMRELEASEGROUP}"
    while [ $LYNXTRIES -gt 0 ]; do
     if [ -z "$USEWGET" ] && [ -z "$USECURL" ]; then
      lynx -source $LYNXFLAGS $BOMURLRELEASEGROUP > $TMPFILE 2>&1
      if [ $? = "0" ]; then
       LYNXTRIES=$LYNXTRIESORIG
       break
      else
       let LYNXTRIES=LYNXTRIES-1
       sleep 1
      fi
     else
      if [ ! -z "$USEWGET" ]; then
       #http_proxy=192.168.0.1:8080
       wget $WGETFLAGS -U "Internet Explorer" -O $TMPFILE --timeout=30 $BOMURLRELEASEGROUP >/dev/null 2>&1
      elif [ ! -z "$USECURL" ]; then
       curl $CURLFLAGS -A "Internet Explorer" -o $TMPFILE --connect-timeout 30 $BOMURLRELEASEGROUP >/dev/null 2>&1
      fi
      if [ $? = "0" ] || [ -z "$(cat $TMPFILE)" ]; then
       LYNXTRIES=$LYNXTRIESORIG
       break
      else
       let LYNXTRIES=LYNXTRIES-1
       echo -n "" > $TMPFILE
       sleep 1
      fi
     fi
    done
    BOMRELEASE="$(sed -n -E 's|.*<a class="a-link-normal" href="(/release/rl[0-9]+/)[^\"]*">Domestic[^\n]*</a>.*|\1|p' $TMPFILE | head -n 1)"
    BOMURLRELEASE="https://www.boxofficemojo.com${BOMRELEASE}"
    while [ $LYNXTRIES -gt 0 ]; do
     if [ -z "$USEWGET" ] && [ -z "$USECURL" ]; then
      #lynx $LYNXFLAGS $BOMURLRELEASE | grep -a -v "^$" | tr '\t' ' ' | tr -s ' ' | tr '\n' '~' | sed "s/:~ /: /g" | tr '~' '\n' | sed 's/*/\\\*/' > $TMPFILE 2>&1
      lynx -source $LYNXFLAGS $BOMURLRELEASE > $TMPFILE 2>&1
      if [ $? = "0" ]; then
       LYNXTRIES=$LYNXTRIESORIG
       break
      else
       let LYNXTRIES=LYNXTRIES-1
       sleep 1
      fi
     else
      if [ ! -z "$USEWGET" ]; then
       #http_proxy=192.168.0.1:8080
       wget $WGETFLAGS -U "Internet Explorer" -O $TMPFILE --timeout=30 $BOMURLRELEASE >/dev/null 2>&1
      elif [ ! -z "$USECURL" ]; then
       curl $CURLFLAGS -A "Internet Explorer" -o $TMPFILE --connect-timeout 30 $BOMURLRELEASE >/dev/null 2>&1
      fi
      if [ $? = "0" ] || [ -z "$(cat $TMPFILE)" ]; then
       LYNXTRIES=$LYNXTRIESORIG
       break
      else
       let LYNXTRIES=LYNXTRIES-1
       echo -n "" > $TMPFILE
       sleep 1
      fi
     fi
    done
    if [ "$LYNXTRIES" = "$LYNXTRIESORIG" ]; then
     if [ ! -z "$USEWIDEST" ]; then
      BUSINESSSCREENS="$(sed -n -E 's|.*<div[^>]*><span>Widest Release</span><span>([0-9,]+) theaters</span></div>.*|\1|p' $TMPFILE | head -n1 | tr -d ',' )"
     else 
      BUSINESSSCREENS=$(sed -n -E 's|.*<div[^>]*><span>Opening</span><span><span class="money">[0-9,$]+</span><br/>*([0-9,]+)$|\1|p' $TMPFILE | head -n1 | tr -d ',' )
     fi
     if [ -z "$(echo "$BUSINESSSCREENS" | tr -cd '0-9')" ]; then
      BUSINESSSCREENS=""
     fi
     if [ ! -z "$BUSINESSSCREENS" ] && [ -z "$ISLIMITED" ]; then
      if [ $BUSINESSSCREENS -lt 500 ]; then
       ISLIMITED=$LIMITEDYES
      else
       ISLIMITED=$LIMITEDNO
      fi
     else
      ISLIMITED=""
     fi
    fi 
   fi
   if [ ! -z "$IMDBHEAD" ]; then
    BOTHEAD=$(echo $BOTHEADORIG | sed "s/RELEASENAME/$BOLD$IMDBDIR$BOLD/")
   fi
   if [ ! -z $DEBUG ]; then
    DEBUGCOUNT=5
    echo "$DEBUGCOUNT : DOTIMDB = '$DOTIMDB'"
    echo "$DEBUGCOUNT : USEBOT = '$USEBOT'"
   fi
   if [ ! -z "$DIRECTOR" ]; then
    DIRECTOR="Directed by..: $DIRECTOR"
   fi
   if [ ! -z $DEBUG ]; then
    DEBUGCOUNT=6
    echo "$DEBUGCOUNT : DOTIMDB = '$DOTIMDB'"
    echo "$DEBUGCOUNT : USEBOT = '$USEBOT'"  
   fi

   if [ ! -z "$USEBOT" ]; then

# Time to put stuff out so the bot can read it.
###############################################

    if [ ! -z "$LOCALURL" ]; then
     IMDBURL="$(echo $IMDBURL | sed "s|/www.|/$LOCALURL.|g" | tr 'A-Z' 'a-z')"
    fi
    HEADTMP="Title........: $BOLD$TITLE$BOLD"
    if [ ! -z "$COUNTRY" ]; then
     HEADTMP="$HEADTMP / $COUNTRY"
    fi
    if [ ! -z "$LANGUAGE" ]; then
     HEADTMP="$HEADTMP / $BOLD$LANGUAGE$BOLD"
    fi
    HEAD=$(echo "$HEADTMP" | sed "s/Country......: //" | sed "s/Language.....: //" | tr -s ' ')
    if [ ! -z "$BOTHEAD" ]; then
     echo "$DATE $TRIGGER \"$IMDBLKL\" \"$BOTHEAD\" \"$IMDBDST\"" >> $GLLOG
    fi
    if [ ! -z "$HEAD" ] && [ -z "$BOTONELINE" ]; then
     echo "$DATE $TRIGGER \"$IMDBLKL\" \"$HEAD\" \"$IMDBDST\"" >> $GLLOG
    fi
    if [ -z "$BOTONELINE" ]; then
     echo "$DATE $TRIGGER \"$IMDBLKL\" \"IMDb Link....: $IMDBURL\" \"$IMDBDST\"" >> $GLLOG
    fi
    if [ ! -z "$DIRECTOR" ] && [ -z "$BOTONELINE" ]; then
     echo "$DATE $TRIGGER \"$IMDBLKL\" \"$DIRECTOR\" \"$IMDBDST\"" >> $GLLOG
    fi
    if [ ! -z "$GENRE" ] && [ -z "$BOTONELINE" ]; then
     echo "$DATE $TRIGGER \"$IMDBLKL\" \"$GENRE\" \"$IMDBDST\"" >> $GLLOG
    fi
    if [ ! -z "$RATING" ] && [ -z "$BOTONELINE" ]; then
     echo "$DATE $TRIGGER \"$IMDBLKL\" \"$RATING\" \"$IMDBDST\"" >> $GLLOG
    fi
    if [ ! -z "$SHOWSTAR" ] && [ -z "$BOTONELINE" ] && [ ! -z "$CASTLEADNAME" ]; then
     echo "$DATE $TRIGGER \"$IMDBLKL\" \"Starring.....: $CASTLEADNAME as $CASTLEADCHAR\" \"$IMDBDST\"" >> $GLLOG
    fi
    if [ ! -z "$RUNTIME" ] && [ -z "$BOTONELINE" ]; then
     echo "$DATE $TRIGGER \"$IMDBLKL\" \"$RUNTIME\" \"$IMDBDST\"" >> $GLLOG
    fi
    if [ ! -z "$BUSINESSSHORT" ] && [ -z "$BOTONELINE" ]; then
     echo "$DATE $TRIGGER \"$IMDBLKL\" \"Opening Stats: $BUSINESSSHORT\" \"$IMDBDST\"" | tr '[=$=]' '�' | sed "s|�|USD|g" >> $GLLOG
    fi
    if [ ! -z "$PREMIERE" ] && [ -z "$BOTONELINE" ]; then
     echo "$DATE $TRIGGER \"$IMDBLKL\" \"Premiere Date: $PREMIERE\" \"$IMDBDST\"" >> $GLLOG
    fi
    if [ ! -z "$LIMITED" ] && [ -z "$BOTONELINE" ]; then
     echo "$DATE $TRIGGER \"$IMDBLKL\" \"Limited Date.: $LIMITED\" \"$IMDBDST\"" >> $GLLOG
    fi
    if [ ! -z "$TAGLINE" ] && [ ! -z "$PLOT" ] && [ -z "$BOTONELINE" ]; then
     if [ "$TAGPLOT" = "TAG" ] || [ -z "$TAGPLOT" ] ; then
      echo "$DATE $TRIGGER \"$IMDBLKL\" \"$TAGLINE\" \"$IMDBDST\"" >> $GLLOG
     fi
     if [ "$TAGPLOT" = "PLOT" ] || [ -z "$TAGPLOT" ]; then
      echo "$DATE $TRIGGER \"$IMDBLKL\" \"$PLOT\" \"$IMDBDST\"" >> $GLLOG
     fi
    elif [ ! -z "$TAGLINE" ] && [ ! "$TAGPLOT" = "NONE" ] && [ -z "$BOTONELINE" ]; then
     echo "$DATE $TRIGGER \"$IMDBLKL\" \"$TAGLINE\" \"$IMDBDST\"" >> $GLLOG
    elif [ ! -z "$PLOT" ] && [ ! "$TAGPLOT" = "NONE" ] && [ -z "$BOTONELINE" ]; then
     echo "$DATE $TRIGGER \"$IMDBLKL\" \"$PLOT\" \"$IMDBDST\"" >> $GLLOG
    fi
    if [ ! -z "$SHOWCOMMENTSHORT" ] && [ ! "$COMMENTSHORT" = "User Reviews:" ] && [ -z "$BOTONELINE" ]; then
     echo "$DATE $TRIGGER \"$IMDBLKL\" \"$COMMENTSHORT\" \"$IMDBDST\"" >> $GLLOG
    fi
    if  [ ! -z "$BOTONELINE" ]; then
     if [ -z "$LOGFORMAT" ]; then
      echo "$DATE $TRIGGER \"$IMDBLKL\" \"$ONELINE\" \"$IMDBDST\"" >> $GLLOG
     elif [ "$LOGFORMAT" = "MYOWN" ]; then
#      NEWLINE="|"
      MYOWNPAIRS="%imdbdirname|IMDBDIR %imdburl|IMDBURL %imdbtitle|TITLE %imdbgenre|GENRECLEAN %imdbrating|RATINGCLEAN %imdbcountry|COUNTRYCLEAN %imdblanguage|LANGUAGECLEAN %imdbcertification|CERTCLEAN %imdbruntime|RUNTIMECLEAN %imdbdirector|DIRECTORCLEAN %imdbbusinessdata|BUSINESSSHORT %imdbpremiereinfo|PREMIERE %imdblimitedinfo|LIMITED %imdbvotes|RATINGVOTES %imdbscore|RATINGSCORE %imdbname|TITLENAME %imdbyear|TITLEYEAR %imdbnumscreens|BUSINESSSCREENS %imdbislimited|ISLIMITED %imdbcastleadname|CASTLEADNAME %imdbcastleadchar|CASTLEADCHAR %imdbtagline|TAGLINECLEAN %imdbplot|PLOTCLEAN %imdbbar|RATINGBAR %imdbcasting|CASTCLEAN %imdbcommentshort|COMMENTSHORTCLEAN %newline|NEWLINE %bold|BOLD"
      MYOWNFORMAT1="$MYOWNFORMAT"
      for OWNPAIR in $MYOWNPAIRS; do
       MYOWNSTRING="$(echo "$OWNPAIR" | cut -d '|' -f 1)"
       MYOWNVAR="$(echo "$OWNPAIR" | cut -d '|' -f 2)"
       if [ ! -z "${!MYOWNVAR}" ]; then
        MYTEMPVAR="$(echo "${!MYOWNVAR}" | tr '\&' '\`')"
        MYOWNFORMAT1="$(echo "${MYOWNFORMAT1}" | sed "s^$MYOWNSTRING^$MYTEMPVAR^g" | tr '\`' '\&')"
       else
        MYOWNFORMAT1="$(echo "${MYOWNFORMAT1}" | sed "s^$MYOWNSTRING^$MYOWNEMPTY^g")"
       fi
      done
      echo "$DATE $TRIGGER \"$IMDBLKL\" \"${MYOWNFORMAT1}\" \"$IMDBDST\"" | tr '[=$=]' '�' | sed "s|�|USD|g" >> $GLLOG
     else
      echo "$DATE $TRIGGER \"$IMDBLKL\" \"$IMDBDIR\" \"$IMDBURL\" \"$TITLE\" \"$GENRECLEAN\" \"$RATINGCLEAN\" \"$COUNTRYCLEAN\" \"$LANGUAGECLEAN\" \"$CERTCLEAN\" \"$RUNTIMECLEAN\" \"$DIRECTORCLEAN\" \"$BUSINESSSHORT\" \"$PREMIERE\" \"$LIMITED\" \"$RATINGVOTES\" \"$RATINGSCORE\" \"$TITLENAME\" \"$TITLEYEAR\" \"$BUSINESSSCREENS\" \"$ISLIMITED\" \"$CASTLEADNAME\" \"$CASTLEADCHAR\" \"$TAGLINECLEAN\" \"$PLOTCLEAN\" \"$RATINGBAR\" \"$CASTCLEAN\" \"$COMMENTSHORTCLEAN\" \"$IMDBDST\"" | tr '[=$=]' '�' | sed "s|�|USD|g" >> $GLLOG
     fi
    fi
   fi
   if [ ! -z "$DOTIMDB" ]; then

# Echo stuff to the .imdb file
##############################

    echo -e "$IMDBHEAD" > "$IMDBLNK"
    OWNER=$(ls -1nl "$GLROOT$IMDBLKL" | tail -n 1 | { read junk junk owner group junk; echo $owner:$group; };)
    echo "Title........: $TITLE" | fold -s -w $IMDBWIDTH | head -n 1 >> "$IMDBLNK"
    echo "-" >> "$IMDBLNK"
    echo "IMDb Link....: $IMDBURL" | head -n 1 >> "$IMDBLNK"
    if [ ! -z "$DIRECTOR" ]; then
     echo "$DIRECTOR" | fold -s -w $IMDBWIDTH | head -n 1 >> "$IMDBLNK"
    fi
    if [ ! -z "$GENRE" ]; then
     echo "$GENRE" | fold -s -w $IMDBWIDTH | head -n 1 >> "$IMDBLNK"
    fi
    if [ ! -z "$RATING" ]; then
     echo "$RATING" | fold -s -w $IMDBWIDTH | head -n 1 >> "$IMDBLNK"
    fi
    if [ ! -z "$TAGLINE" ]; then
     echo "$TAGLINE" | fold -s -w $IMDBWIDTH >> "$IMDBLNK"
    fi
    echo "-" >> "$IMDBLNK"
    if [ ! -z "$COUNTRY" ]; then
     echo "$COUNTRY" | fold -s -w $IMDBWIDTH | head -n 1 >> "$IMDBLNK"
    fi
    if [ ! -z "$LANGUAGE" ]; then
     echo "$LANGUAGE" | fold -s -w $IMDBWIDTH | head -n 1 >> "$IMDBLNK"
    fi
    if [ ! -z "$CERT" ]; then
     echo "$CERT" | fold -s -w $IMDBWIDTH | head -n 1 >> "$IMDBLNK"
    fi
    if [ ! -z "$PREMIERE" ]; then
     echo "Premiere Date: $PREMIERE" | fold -s -w $IMDBWIDTH | head -n 1 >> "$IMDBLNK"
    fi
    if [ ! -z "$LIMITED" ]; then
     echo "Limited Date.: $LIMITED" | fold -s -w $IMDBWIDTH | head -n 1 >> "$IMDBLNK"
    fi
    if [ ! -z "$RUNTIME" ]; then
     echo "$RUNTIME" | fold -s -w $IMDBWIDTH | head -n 1 >> "$IMDBLNK"
    fi
    if [ ! -z "$CAST" ]; then
     echo "-" >> "$IMDBLNK"
     echo "Credited Cast:" >> "$IMDBLNK"
     echo "$CAST" | fold -s -w $IMDBWIDTH >> "$IMDBLNK"
    fi
    if [ ! -z "$BUSINESS" ]; then
     echo "-" >> "$IMDBLNK"
     echo "Business Data on Opening Weekend:" >> "$IMDBLNK"
     echo "$BUSINESS" | fold -s -w $IMDBWIDTH >> "$IMDBLNK"
    fi
    if [ ! -z "$PLOT" ]; then
     echo "-" >> "$IMDBLNK"
     #echo "$PLOT" | fold -s -w $IMDBWIDTH >> "$IMDBLNK"
     echo "$PLOT" | sed 's/\\\\n//g' | fold -s -w $IMDBWIDTH >> "$IMDBLNK"
    fi
    if [ ! -z "$SHOWCOMMENT" ] && [ ! -z "$COMMENT" ]; then
     echo "---" >> "$IMDBLNK"
     echo "User Review:" >> "$IMDBLNK"
     echo "$COMMENT" | sed "s/^\ *//g" | sed "s/\ *$//g" | tr -s ' ' | fold -s -w $IMDBWIDTH >> "$IMDBLNK"
    fi
    echo -e "$IMDBTAIL" >> "$IMDBLNK"
   fi

   if [ ! -z "$INFOTEMPNAME" ] && [ -e "$GLROOT$IMDBLKL/$INFOTEMPNAME" ]; then

# make a file/dir with imdb info in the name 

    INFOGENRES=$(echo $GENRECLEAN | tr '/ ' '\n' |  sed -e /^$/d | wc -l)
    if [ ! $INFOGENRES -gt $INFOGENREMAX ]; then
     let INFOGENREMAXED=INFOGENRES
    else
     let INFOGENREMAXED=INFOGENREMAX
    fi
    if [ ! $INFOGENRES -lt 1 ]; then
     GENREFILE="$(echo $GENRECLEAN | tr '/ ' '\n' |  sed -e /^$/d | head -n $INFOGENREMAX | tr '\n' ' ' | sed "s/ /$INFOGENRESEP/g" | cut -d "$INFOGENRESEP" -f 1-$INFOGENREMAXED)"
    else
     GENREFILE="Unclassified"
    fi
    VOTESFILE="$(echo $RATINGVOTES | tr ',' '.')"
    [[ -z "$VOTESFILE" ]] && VOTESFILE="NA"
    SCOREFILE="$RATINGSCORE"
    [[ -z "$SCOREFILE" ]] && SCOREFILE="NA"
    LIMITEDFILE="$ISLIMITED"
    [[ -z "$LIMITEDFILE" ]] && LIMITEDFILE="unknown"
    NUMSCREENS="$(echo $BUSINESSSCREENS | tr ',' '.')"
    [[ -z "$NUMSCREENS" ]] && NUMSCREENS="unknown"
    RUNTIMEFILE="$RUNTIMECLEAN"
    [[ -z "$RUNTIMEFILE" ]] && RUNTIMEFILE="unknown"
    INFOFILENAMEOLD="$(echo "$INFOFILENAME" | tr -c $INFOVALID $INFOCHARTO | sed "s%VOTES%*%g" | sed "s%SCORE%*%g" | sed "s%GENRE%*%g" | sed "s%RUNTIME%*%g" | sed "s%YEAR%*%g" | sed "s%ISLIMITED%*%g" | sed "s%SCREENS%*%g")"
    INFOFILENAMEOLDA="$(echo "$INFOFILENAMEOLD" | sed "s%*%.*%g")"
    INFOFILENAMEOLDB="$(echo "$INFOFILENAMEOLDA" | tr '\]\[' '.')"
    INFOFILENAMEPRINT="$(echo "$INFOFILENAME" | sed "s%VOTES%$VOTESFILE%g" | sed "s%SCORE%$SCOREFILE%g" | sed "s%GENRE%$GENREFILE%g" | sed "s%RUNTIME%$RUNTIMEFILE%g" | sed "s%YEAR%$TITLEYEAR%g" | sed "s%ISLIMITED%$LIMITEDFILE%g" | sed "s%SCREENS%$NUMSCREENS%g")"
    INFOFILENAMEPRINT="$(echo "$INFOFILENAMEPRINT" | tr -c $INFOVALID $INFOCHARTO)"
    if [ ! -z "$(ls -1 "$GLROOT$IMDBLKL" | grep -a -e "$INFOFILENAMEOLDB")" ]; then
     for OLDINFOFILE in $(ls -1  "$GLROOT$IMDBLKL" | grep -a -e "$INFOFILENAMEOLDB" | tr ' ' '^'); do
      OLDINFOFILE="$(echo $OLDINFOFILE | tr '^' ' ')"
      rm -f "$GLROOT$IMDBLKL/$OLDINFOFILE" >/dev/null 2>&1
      rmdir "$GLROOT$IMDBLKL/$OLDINFOFILE" >/dev/null 2>&1
     done
    fi
    mv "$GLROOT$IMDBLKL/$INFOTEMPNAME" "$GLROOT$IMDBLKL/$INFOFILENAMEPRINT"
   fi

# create a thumbnail?

   if [ "$DOWNLOADTHUMB" = "YES" ]; then
    FILENAME=$(ls -1Ftr "$GLROOT$IMDBLKL" | grep -a -v "/" | grep -a -v "@" | grep -a -e "[.][nN][fF][oO]" | head -n 1)
    TMBNAME=$(echo $FILENAME | sed "s/\.nfo/.jpg/")
    if [ ! -z "$USEWGET" ]; then
     wget $WGETFLAGS -U "Internet Explorer" -O $TMPFILE --timeout=30 $GLROOT$IMDBLKL/$TMBNAME >/dev/null 2>&1
    elif [ ! -z "$USECURL" ]; then
     curl $CURLFLAGS -A "Internet Explorer" -o $TMPFILE --connect-timeout 30 $GLROOT$IMDBLKL/$TMBNAME >/dev/null 2>&1
    fi
   fi

# Should we run any external scripts?

   if [ ! -z "$EXTERNALSCRIPTNAME" ]; then
    FILENAMED=$(ls -1Ftr "$GLROOT$IMDBLKL" | grep -a -v "/" | grep -a -v "@" | grep -a -e "[.][nN][fF][oO]" | head -n 1)
    if [ ! -z "$FILENAMED" ]; then
     touch -acmr "$GLROOT$IMDBLKL/$FILENAMED" "$GLROOT$IMDBLKL" >/dev/null 2>&1
    fi
    for EXTERNALNAME in $EXTERNALSCRIPTNAME; do
     if [ "$DEBUG" = "4" ] && [ ! -z "$(head -n 1 $EXTERNALNAME | grep -a -e "/bin/bash")" ]; then
      bash -x -v $EXTERNALNAME "\"$DATE\" \"$IMDBLNK\" \"$IMDBLKL\" \"$IMDBDIR\" \"$IMDBURL\" \"$TITLE\" \"$GENRECLEAN\" \"$RATINGCLEAN\" \"$COUNTRYCLEAN\" \"$LANGUAGECLEAN\" \"$CERTCLEAN\" \"$RUNTIMECLEAN\" \"$DIRECTORCLEAN\" \"$BUSINESSSHORT\" \"$PREMIERE\" \"$LIMITED\" \"$RATINGVOTES\" \"$RATINGSCORE\" \"$TITLENAME\" \"$TITLEYEAR\" \"$BUSINESSSCREENS\" \"$ISLIMITED\" \"$CASTLEADNAME\" \"$CASTLEADCHAR\" \"$TAGLINECLEAN\" \"$PLOTCLEAN\" \"$RATINGBAR\" \"$CASTCLEAN\" \"$COMMENTSHORTCLEAN\" \"$COMMENTCLEAN\""
     else
      $EXTERNALNAME "\"$DATE\" \"$IMDBLNK\" \"$IMDBLKL\" \"$IMDBDIR\" \"$IMDBURL\" \"$TITLE\" \"$GENRECLEAN\" \"$RATINGCLEAN\" \"$COUNTRYCLEAN\" \"$LANGUAGECLEAN\" \"$CERTCLEAN\" \"$RUNTIMECLEAN\" \"$DIRECTORCLEAN\" \"$BUSINESSSHORT\" \"$PREMIERE\" \"$LIMITED\" \"$RATINGVOTES\" \"$RATINGSCORE\" \"$TITLENAME\" \"$TITLEYEAR\" \"$BUSINESSSCREENS\" \"$ISLIMITED\" \"$CASTLEADNAME\" \"$CASTLEADCHAR\" \"$TAGLINECLEAN\" \"$PLOTCLEAN\" \"$RATINGBAR\" \"$CASTCLEAN\" \"$COMMENTSHORTCLEAN\" \"$COMMENTCLEAN\""
     fi
    done

# restore the releasedir's original date.
#########################################
    FILENAMED=$(ls -1Ftr "$GLROOT$IMDBLKL" | grep -a -v "/" | grep -a -v "@" | grep -a -e "[.][nN][fF][oO]" | head -n 1)
    if [ ! -z "$FILENAMED" ]; then
     touch -acmr "$GLROOT$IMDBLKL/$FILENAMED" "$GLROOT$IMDBLKL" >/dev/null 2>&1
    fi
   fi
  fi

# clean up and make ready for next run.
#######################################

  grep -a -F -v "$IMDBLINE" "$IMDBLOG" > $TMPFILE
  cat $TMPFILE > $IMDBLOG
  > $TMPFILE
 done
 > $TMPRESCANFILE
 > $IMDBPID
fi
exit 0
