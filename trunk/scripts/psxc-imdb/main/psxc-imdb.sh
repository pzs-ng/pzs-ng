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
VERSION="v2.7e"

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
 RECVDARGS=`ls -1Ft | grep -a -v "/" | grep -a -v "@" | head -n 1 | grep -a -e "[.][nN][fF][oO]$"`
fi

# Remove locale settings which might cause problems
export LC_ALL=en_US

if [ ! -z "$RECVDARGS" ]; then

# This is what is run under zs-c, chrooted.
###########################################

# PATH=$PATHCHROOTED
# IMDBLOG=$IMDBLOGCHROOTED
 FILENAME="$RECVDARGS"
 case $FILENAME in
  *.[nN][fF][oO])
   if [ ! -z "$GLROOT" ]; then
    MYTMPFILE=`echo "$TMPFILE" | sed "s%$GLROOT%%"`
   else
    MYTMPFILE=$TMPFILE
   fi
   PSXCFLAG=`head -n 1 $MYTMPFILE | tr -cd '0-9'`
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
    DOTDATEINFO="`grep -a [Dd][Aa][Tt][Ee] $FILENAME | tr -c '/a-zA-Z0-9:. -/\n' ' ' | tr -s ' '`"
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
    if [ ! -z "`pwd | grep -a "$SCANDIR"`" ]; then
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
   IMDBURLS="`grep -a [Ii][Mm][Dd][Bb] $FILENAME | tr ' \|' '\n' | sed -n /[hH][tT][tT][pP]:[/][/].*[.][iI][mM][dD][bB][.].*.[0-9]/p | head -n 1 | tr -c -d '[:alnum:]\:./?'`"
   if [ ! -z "`echo $IMDBURLS | grep -a "imdb\."`" ]; then
    IMDBURL="http://us.imdb.com/title/tt""`echo $IMDBURLS | sed "s/=/-/g" | sed "s/imdb./=/" | cut -d "=" -f 2 | cut -d "/" -f 2,3 | tr -c -d '[:digit:]'`"
    if [ -z `echo $IMDBURL | tr -cd '0-9'` ]; then
     IMDBURL=""
    fi
   fi

# Level 1 search
   if [ -z "$IMDBURL" ] && [ $RELAXEDURLS -ge 1 ]; then
    IMDBURLS="`grep -a [Ii][Mm][Dd][Bb] $FILENAME | tr ' \|' '\n' | sed -n /[hH][tT][tT][pP]:[/][/].*[iI][mM][dD][bB][.].*.[0-9]/p | head -n 1 | tr -c -d '[:alnum:]\:./?'`"
    if [ ! -z "`echo $IMDBURLS | grep -a "imdb\."`" ]; then
     IMDBURL="http://us.imdb.com/title/tt""`echo $IMDBURLS | sed "s/=/-/g" | sed "s/imdb./=/" | cut -d "=" -f 2 | cut -d "/" -f 2,3 | tr -c -d '[:digit:]'`"
     if [ -z `echo $IMDBURL | tr -cd '0-9'` ]; then
      IMDBURL=""
     fi
    fi
   fi

# Level 2 search
   if [ -z "$IMDBURL" ] && [ $RELAXEDURLS -ge 2 ]; then
    IMDBURLS="`grep -a [Ii][Mm][Dd][Bb] $FILENAME | tr ' \|' '\n' | sed -n /.*[iI][mM][dD][bB][.].*.[0-9]/p | head -n 1 | tr -c -d '[:alnum:]\:./?'`"
    if [ ! -z "`echo $IMDBURLS | grep -a "imdb\."`" ]; then
     IMDBURL="http://us.imdb.com/title/tt""`echo $IMDBURLS | sed "s/=/-/g" | sed "s/imdb./=/" | cut -d "=" -f 2 | cut -d "/" -f 2,3 | tr -c -d '[:digit:]'`"
     if [ -z `echo $IMDBURL | tr -cd '0-9'` ]; then
      IMDBURL=""
     fi
    fi
   fi

# Level 3 search
   if [ -z "$IMDBURL" ] && [ $RELAXEDURLS -ge 3 ]; then
    for IMDBURLS in `grep -a [Ii][Mm][Dd][Bb] $FILENAME | tr -c '[:digit:]' '\n' | grep -a -v "^$"`; do
     if [ ! -z `echo $IMDBURLS | tr -cd '0-9'` ]; then
      if [ `echo $IMDBURLS | tr -cd '0-9' | wc -c` -eq 8 ] || [ `echo $IMDBURLS | tr -cd '0-9' | wc -c` -eq 7 ]; then
       IMDBURL="$IMDBURLS"
       break
      fi
     fi
    done
    if [ ! -z "$IMDBURL" ]; then
     IMDBURL="http://us.imdb.com/title/tt""$IMDBURL"
    fi
   fi

# Level 4 search
   if [ -z "$IMDBURL" ] && [ $RELAXEDURLS -ge 4 ]; then
    for IMDBURLS in `cat $FILENAME | tr -c '[:digit:]' '\n' | grep -a -v "^$"`; do
     if [ `echo $IMDBURLS | wc -c` -eq 8 ] || [ `echo $IMDBURLS | wc -c` -eq 7 ]; then
      IMDBURL="$IMDBURLS"
      break
     fi
    done
    if [ ! -z "$IMDBURL" ]; then
     IMDBURL="http://us.imdb.com/title/tt""$IMDBURL"
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
     DOTURLF="`basename "$PWD" | sed "s/ /./g"`"
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
   touch -acmr "$FILENAME" `pwd` >/dev/null 2>&1
  ;;
 esac
# exit 0

#else
fi
if [ ! -z "$RUNCONTINOUS" ] || [ -z "$RECVDARGS" ]; then
# run major part.

 if [ "`basename "$0"`" = "$PRENAME" ]; then

# This is what is done with pre's
#################################
  PATH=$GLPATHPRE

  a="`tail -n 5 $GLPRELOG | grep -a "$PRETRIGGER" | tail -n 1`"
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
    if [ -z `echo $b | grep -a "$PRETRIGGER"` ] && [ $count -gt 0 ] || [ -z "$PRETRIGGER" ]; then
     if [ $combine -eq 1 ]; then
      c=$c$b
     else
      c=$b
     fi
     if [ ! -z `echo "$c" | grep -a "^\""` ]; then
      combine=1
     else
      c=$b
      let count=count+1
     fi
     if [ ! -z `echo "$c" | grep -a "\"$"` ]; then
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
  DIRNAME=`echo $DIRNAME | sed "s|\"\"|$SEPARATOR|g" | sed "s|\"||g"`
  if [ -d $GLROOT$DIRNAME ]; then
   FILENAME=`ls -1 $GLROOT$DIRNAME | grep -a "\.[Nn][Ff][Oo]$" | head -n 1`
    IMDBURL="`grep -a [Ii][Mm][Dd][Bb] $GLROOT$DIRNAME/$FILENAME | tr ' ' '\n' | sed -n /[hH][tT][tT][pP]:[/][/].*[.][iI][mM][dD][bB].*.[0-9]/p | head -n 1 | tr -c -d '[:alnum:]\:./?'`"
   if [ ! -z "`echo $IMDBURL | grep -a "\.imdb\."`" ]; then
     IMDBURL="http://us.imdb.com/title/tt""`echo $IMDBURL | sed "s/=/-/g" | sed "s/.imdb./=/" | cut -d "=" -f 2 | cut -d "/" -f 2,3 | tr -c -d '[:digit:]'`"
   fi
    if [ ! -z "$IMDBURL" ]; then
     a="`tail -n 5 $GLLOG | grep -a "$TRIGGER" | grep -a "$DIRNAME" | tail -n 1`"
     if [ -z "$a" ]; then
      SEARCHFORURLS=0
      if [ -z "$SCANDIRS" ]; then
       SEARCHFORURLS=1
      fi
      for SCANDIR in $SCANDIRS; do
       if [ ! -z "`echo "$DIRNAME" | grep -a "$SCANDIR"`" ]; then
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

 if [ -z "`cat $IMDBLOG`" ]; then

# No new imdb-info. let's quit.
###############################
  exit 0
 fi

# Make sure this script isn't already running.
##############################################
 sleep 0.$RANDOM
 MYNAMEIS=`basename "$0"`
 IMDBPIDCONTENT=`cat $IMDBPID`
 if [ ! -z "$IMDBPIDCONTENT" ]; then
  if [ ! -z `ps ax | awk '{print $1}' | grep -a -e "^$IMDBPIDCONTENT$"` ]; then
   exit 0
  fi
 fi
 echo $$ > $IMDBPID

# Seems like something was put into the log. Let's check it.
############################################################
 IMDBFLAGS=`head -n 1 $TMPFILE | tr -cd '0-9'`
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

 while [ ! -z "`cat $IMDBLOG`" ]; do
  IMDBLINE="`cat $IMDBLOG | grep -a -e "/" | head -n 1`"
  cat $IMDBLOG | grep -a -F -v "$IMDBLINE" > $TMPFILE
  cat $TMPFILE > $IMDBLOG
  ISLIMITED=""
  BUSINESS=""
  BUSINESSSHORT=""
  PREMIERE=""
  LIMITED=""
  EXEMPTED=""
  LYNXTRIESORIG=$LYNXTRIES
  IMDBURL="`echo $IMDBLINE | cut -d "|" -f 1`"
  IMDBLNK="`echo $IMDBLINE | cut -d "|" -f 2`"
  IMDBDST="`echo $IMDBLINE | cut -d "|" -f 3`"
  DEBUGCOUNT=1
  if [ ! -z $DEBUG ]; then
   echo "$DEBUGCOUNT : DOTIMDB = '$DOTIMDB'"
   echo "$DEBUGCOUNT : USEBOT = '$USEBOT'"
  fi
  if [ -d "$GLROOT$IMDBLNK" ]; then
   IMDBDIR="`basename "$IMDBLNK"`"
   BASELNK="`dirname "$IMDBLNK"`"
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
   if [ ! -z `echo "$IMDBLKL" | grep -a "$EXEMPT"` ]; then
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
  if [ ! -z `cat $IMDBURLLOG | grep -a "$IMDBURL"` ] || [ ! -z $EXEMPTED ]; then
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
     if [ -z "$USEWGET" ]; then
      lynx $LYNXFLAGS $IMDBURL > $TMPFILE 2>&1
      if [ $? = "0" ]; then
       LYNXTRIES=$LYNXTRIESORIG
       break
      else
       let LYNXTRIES=LYNXTRIES-1
       sleep 1
      fi
     else
      #http_proxy=192.168.0.1:8080
      wget -U "Internet Explorer" -O $TMPFILE --timeout=30 $IMDBURL >/dev/null 2>&1
      if [ $? = "0" ] || [ -z "`cat $TMPFILE`" ]; then
       LYNXTRIES=$LYNXTRIESORIG
       HTMLPAGE="`lynx $LYNXFLAGS -force_html $TMPFILE`"
       echo "$HTMLPAGE" > $TMPFILE
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
    TITLE=`cat "$TMPFILE" | grep -a -e "^[a-zA-Z0-9\"\.]" | sed "s/\"//g" | head -n 1`
    if [ -z "$TITLE" ]; then
     # a new version of lynx? let's try a diff. approach.
     TITLE=`cat "$TMPFILE" | grep -a -e "([12][089][0-9][0-9])" | head -n 1 | sed "s/PageFlicker//" | sed "s/\"//g"`
    fi
    if [ -z "$TITLE" ] || [ ! -z "`echo "$TITLE" | grep -a -e "with other users on IMDb"`" ] || [ ! -z "`echo "$TITLE" | grep -a -e "laserdisc details"`" ] || [ ! -z "`echo "$TITLE" | grep -a -e "(\ )"`" ] || [ ! -z "`echo "$TITLE" | grep -a -e "\.\.\.\."`" ]; then
     OUTPUTOK=""
     break
    fi

# Grab hold of the info we'll use later. Also do some formatting.
#################################################################

    TITLEYEAR=`echo $TITLE | tr ' ' '\n' | grep -a -v "^[a-zA-Z]" | grep -a -e "^([12]" | head -n 1`
    TITLENAME=`echo $TITLE | sed "s| $TITLEYEAR||"`
    TITLEYEAR=`echo $TITLEYEAR | tr -cd '0-9'`
    if [ -z "$TITLEYEAR" ]; then
     OUTPUTOK=""
     break
    fi
    GENRE=`cat $TMPFILE | grep -a -e "Genre:" | sed "s/(more)//" | sed "s/^\ *//g" | sed "s/\ *$//g" | sed "s/Genre:/Genre........:/" | sed s/\"/$QUOTECHAR/g | tr '/' '\n' | head -n $GENRENUM | tr '\n' '/' | sed "s|/$||" | sed "s/ *$//"`
    GENRECLEAN=`echo $GENRE | sed "s/Genre........: *//"`
    RATING=`cat $TMPFILE | grep -a -e "User\ Rating:" | sed "s/^\ *//g" | sed "s/\ *$//g" | sed "s/ [Vv][Oo][Tt][Ee] [Hh][Ee][Rr][Ee]//" | sed "s/User Rating:/User Rating..:/" | sed "s/* //g" | sed "s/_ //g" | sed s/\"/$QUOTECHAR/g | head -n 1`
    RATINGCLEAN=`echo $RATING | sed "s/User Rating..: *//"`
    RATINGBAR="`cat $TMPFILE | grep -a -e "User\ Rating:" | tr -cd '\*_' | tr '\*_' '#-' | head -n 1`"
    if [ "`echo $RATINGCLEAN | grep -a -e "[Ww][Aa][Ii][Tt]"`" = "" ]; then
     RATINGVOTES=`echo $RATINGCLEAN | sed "s/ [Vv][Oo][Tt][Ee][Ss]//" | tr '() ' '\n' | grep -a -v "/" | grep -a -e "[0-9]" | head -n 1`
     RATINGSCORE=`echo $RATINGCLEAN | grep -a -e "/" | tr '/' '\n' | head -n 1`
    else
     RATINGVOTES=""
     RATINGSCORE=""
    fi
    COUNTRY=`cat $TMPFILE | grep -a -e "Country:" | sed "s/^\ *//g" | sed "s/\ *$//g" | sed "s/Country:/Country......:/" | sed s/\"/$QUOTECHAR/g | tr '/' '\n' | head -n $COUNTRYNUM | tr '\n' '/' | sed "s|/$||" | sed "s/ *$//"`
    COUNTRYCLEAN=`echo $COUNTRY | sed "s/Country......: *//"`
    TAGLINE=`cat $TMPFILE | grep -a -e "Tagline:" | sed "s/(more)//" | sed "s/^\ *//g" | sed "s/\ *$//g" | sed "s/([Mm][Oo][Rr][Ee])//" | sed "s/Tagline:/Tagline......:/" | sed s/\"/$QUOTECHAR/g | head -n 1`
    TAGLINECLEAN=`echo $TAGLINE | sed "s/Tagline......: *//"`
    LANGUAGE=`cat $TMPFILE | grep -a -e "Language:" | sed "s/^\ *//g" | sed "s/\ *$//g" | sed "s/Language:/Language.....:/" | sed s/\"/$QUOTECHAR/g | tr '/' '\n' | head -n $LANGUAGENUM | tr '\n' '/' | sed "s|/$||" | sed "s/ *$//"`
    LANGUAGECLEAN=`echo $LANGUAGE | sed "s/Language.....: *//"`
    PLOT=`cat $TMPFILE | grep -a -e "Plot\ [OS][u][tm][lm][ia][nr][ey]:" | sed "s/^\ *//g" | sed "s/\ *$//g" | sed "s/([Vv][Ii][Ee][Ww] [Tt][Rr][Aa][Ii][Ll][Ee][Rr])//" | sed "s/([Mm][Oo][Rr][Ee])//" | sed "s/Plot [OS][u][tm][lm][ia][nr][ey][:]/Plot Outline.:/" | sed s/\"/$QUOTECHAR/g | tr -s ' ' | head -n 1`
    PLOTCLEAN=`echo $PLOT | sed "s/Plot Outline.: *//"`
    if [ ! -z "`echo "$PLOTCLEAN" | grep -a -e "\(\ \)\ \(\ \)"`" ]; then
     OUTPUTOK=""
     break
    fi
    CERT=`cat $TMPFILE | grep -a -e "Certification:" | sed "s/^\ *//g" | sed "s/\ *$//g" | sed s/\"/$QUOTECHAR/g | tr '/' '\n' | head -n $CERTIFICATIONNUM | sed "s/(.*//g" | tr '\n' '/' | sed "s|/$||" | sed "s/ *$//"`
    CERTCLEAN=`echo $CERT | sed "s/Certification: *//" | tr '/' '\n' | grep -a -e "[uU][sS][aA]" | tr -d ' ' | head -n 1`
    CAST=`cat $TMPFILE | grep -a -e "\ \.\.\.\.\ " | sed s/\"/$QUOTECHAR/g | head -n $CASTNUM`
    CASTCLEAN=`echo "$CAST" | sed "s/\.\.\.\..*/|/g" | tr '\n' ' ' | tr -s ' ' | sed "s/^\ *//g" | sed "s/\ *$//g" | sed "s/ |/\,/g" | sed "s/,$//"`
    CASTLEADNAME="`echo "$CAST" | head -n 1 | tr '.' '\n' | sed -e /^$/d | head -n 1 | tr -s ' ' | sed "s/^\ //g" | sed "s/\ $//g"`"
    CASTLEADCHAR="`echo "$CAST" | head -n 1 | tr '.' '\n' | sed -e /^$/d | tail -n 1 | tr -s ' ' | sed "s/^\ //g" | sed "s/\ $//g"`"
    COMMENTSHORT=`cat $TMPFILE | grep -a -e "User Comments:" | head -n 1 | sed "s/^\ *//g" | sed "s/\ *$//g" | sed "s/([Mm][Oo][Rr][Ee])//" | sed s/\"/$QUOTECHAR/g`
    COMMENTSHORTCLEAN=`echo $COMMENTSHORT | sed "s/User Comments: *//"`
    COMMENT=`cat $TMPFILE | awk '/User Comments:$/, /Check for other user comments.$/' | sed s/\"/$QUOTECHAR/g`
    COMMENTCLEAN=`echo "$COMMENT" | grep -a -e "^\ \ \ \ \ " | sed "s/^\ *//g" | sed "s/\ *$//g" | sed s/\{\}\"/$QUOTECHAR/g | tr '\n' '|'`
    RUNTIME=`cat $TMPFILE | grep -a -e "Runtime:" | sed "s/^\ *//g" | sed "s/\ *$//g" | sed "s/Runtime:/Runtime......:/" | sed s/\"/$QUOTECHAR/g | tr '/' '\n' | head -n $RUNTIMENUM | tr '\n' '/' | sed "s|/$||" | sed "s/ *$//"`
    RUNTIMECLEAN="`echo $RUNTIME | sed "s/Runtime......: *//" | tr '/ ' '\n' | sed -e /^$/d | head -n 1 | tr -c -d '[:digit:]'`"
    if [ ! -z "$RUNTIMECLEAN" ]; then
     RUNTIMECLEAN="$RUNTIMECLEAN min"
    fi
    DIRECTOR=`cat $TMPFILE | awk '/Directed by$/, /Writing credits$/' | grep -a -v "Directed by" | grep -a -v "Writing Credits" | sed "s/^\ *//g" | sed "s/\ *$//g" | head -n 1 | sed s/\"/$QUOTECHAR/g`
    DIRECTORCLEAN=`echo $DIRECTOR`
    if [ ! -z "`echo "$DIRECTOR" | grep -a -e "\(\ \)\ \(\ \)"`" ]; then
     OUTPUTOK=""
     break
    fi
    ONELINE="`echo "$BOLD$TITLE$BOLD""[$COUNTRY]: $GENRE - $BOLD$RATING$BOLD"" - $IMDBURL" | sed "s/Genre........: //" | sed "s/User Rating..: //" | sed "s/Country......: *//" | tr -s ' '`"
    if [ ! -z "`echo "$ONELINE" | grep -a -e "\(\ \)\ \(\ \)"`" ]; then
     OUTPUTOK=""
     break
    fi
    if [ ! -z "`echo "$GENRECLEAN" | grep -a -e "with other users on IMDb"`" ] || [ ! -z "`echo "$GENRECLEAN" | grep -a -e "laserdisc details"`" ] || [ ! -z "`echo "$GENRECLEAN" | grep -a -e "(\ )"`" ] || [ ! -z "`echo "$GENRECLEAN" | grep -a -e "\.\.\."`" ] || [ ! -z "`echo "$GENRECLEAN" | grep -a -e "Binary"`" ]; then
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
   if [ ! -z "`echo $IMDBLKL | grep -a -e "/dev/null"`" ]; then
    if [ -z "$LOGFORMAT" ]; then
     echo "$DATE $TRIGGER \"$IMDBLKL\" \"Failed to fetch iMDB details. Please try again.\" \"$IMDBDST\"" >> $GLLOG
    elif [ "$LOGFORMAT" = "MYOWN" ]; then
     echo "$DATE $TRIGGER \"$IMDBLKL\" \"Failed to fetch iMDB details. Please try again.\" \"$IMDBDST\"" >> $GLLOG
    else
     echo "$DATE $TRIGGER \"$IMDBLKL\" \"\" \"Failed to fetch iMDB details. Please try again.\"" >> $GLLOG
    fi
   fi
  else

# We won't check business and release as closely right away... may do so later
##############################################################################
   BUSINESSURL="$IMDBURL""/business"
   RELEASEURL="$IMDBURL""/releaseinfo"
   if [ -z "`cat $TMPFILE | grep -a -F "$BUSINESSURL"`" ] && [ -z "$USEWGET" ]; then
    USEBUSINESS=""
   fi
   if [ -z "`cat $TMPFILE | grep -a -F "$RELEASEURL"`" ] &[ -z "$USEWGET" ]; then
    USEPREMIERE=""
    USELIMITED=""
   fi
   if [ ! -z "$USEBUSINESS" ]; then
    while [ $LYNXTRIES -gt 0 ]; do
     if [ -z "$USEWGET" ]; then
      lynx $LYNXFLAGS $BUSINESSURL > $TMPFILE 2>&1
      if [ $? = "0" ]; then
       LYNXTRIES=$LYNXTRIESORIG
       break
      else
       let LYNXTRIES=LYNXTRIES-1
       sleep 1
      fi
     else
      #http_proxy=192.168.0.1:8080
      wget -U "Internet Explorer" -O $TMPFILE --timeout=30 $BUSINESSURL >/dev/null 2>&1
      if [ $? = "0" ] || [ -z "`cat $TMPFILE`" ]; then
       LYNXTRIES=$LYNXTRIESORIG
       HTMLPAGE="`lynx $LYNXFLAGS -force_html $TMPFILE`"
       echo "$HTMLPAGE" > $TMPFILE
       break
      else
       let LYNXTRIES=LYNXTRIES-1
       echo -n "" > $TMPFILE
       sleep 1
      fi
     fi
    done
    if [ "$LYNXTRIES" = "$LYNXTRIESORIG" ]; then
     BUSINESS=`cat $TMPFILE  | awk '/Opening Weekend$/,/Gross$/' | grep -a -e "(" | sed "s/^\ *//g" | sed "s/\ *$//g" | sed s/\"/$QUOTECHAR/g`
     BUSINESSSHORT=`cat $TMPFILE | awk '/Opening Weekend$/,/Gross$/' | grep -a -e "[Ss][Cc][Rr][Ee][Ee][Nn]" | grep -a -e "[Uu][Ss][Aa]" | sed "s/^\ *//g" | sed "s/\ *$//g" | sed s/\"/$QUOTECHAR/g | head -n 1`
     if [ -z "$BUSINESSSHORT" ]; then
      BUSINESSSHORT=`cat $TMPFILE | awk '/Opening Weekend$/,/Gross$/' | grep -a -e "[Uu][Ss][Aa]" | sed "s/^\ *//g" | sed "s/\ *$//g" | sed s/\"/$QUOTECHAR/g | head -n 1`
     fi
     if [ -z "$BUSINESSSHORT" ]; then
      BUSINESSSHORT=`echo "$BUSINESS" | head -n 1`
     fi
     BUSINESSSCREENS=`echo "$BUSINESSSHORT" | tr '()' '\n' | grep -a -e "[Ss][Cc][Rr][Ee][Ee][Nn]" | tr ' ' '\n' | head -n 1`
     BUSINESSSCREENSNUMBER=`echo "$BUSINESSSCREENS" | sed "s/,//"` #tr -d ','`
echo "BIZNUM : $BUSINESSSCREENSNUMBER ( $BUSINESSSCREENS )" >/tmp/imdebug.log
     if [ ! -z "$BUSINESSSCREENSNUMBER" ]; then
      if [ $BUSINESSSCREENSNUMBER -lt 500 ]; then
       ISLIMITED=$LIMITEDYES
      else
       ISLIMITED=$LIMITEDNO
      fi
     else
      ISLIMITED=""
     fi
    fi
   fi
   if [ ! -z "$USEPREMIERE" ] || [ ! -z "$USELIMITED" ]; then
    while [ $LYNXTRIES -gt 0 ]; do
     if [ -z "$USEWGET" ]; then
      lynx $LYNXFLAGS $RELEASEURL > $TMPFILE 2>&1
      if [ $? = "0" ]; then
       LYNXTRIES=$LYNXTRIESORIG
       break
      else
       let LYNXTRIES=LYNXTRIES-1
       sleep 1
      fi
     else
      #http_proxy=192.168.0.1:8080
      wget -U "Internet Explorer" -O $TMPFILE --timeout=30 $RELEASEURL >/dev/null 2>&1
      if [ $? = "0" ] || [ -z "`cat $TMPFILE`" ]; then
       LYNXTRIES=$LYNXTRIESORIG
       HTMLPAGE="`lynx $LYNXFLAGS -force_html $TMPFILE`"
       echo "$HTMLPAGE" > $TMPFILE
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
      PREMIERE=`cat $TMPFILE | awk /Date$/,/endofThefile/ | grep -a -e "(" | grep -a -e "[Pp][Rr][Ee][Mm][Ii][Ee][Rr][Ee]" | head -n 1 | sed "s/^\ *//g" | sed "s/\ *$//g" | sed "s/ (/(/" | tr '(' '\n' | tr -s ' ' | head -n 1 | sed s/\"/$QUOTECHAR/g`
      if [ -z "$PREMIERE" ]; then
       PREMIERE=`cat $TMPFILE | awk /Date$/,/endofThefile/ | grep -a -v "Date$" | head -n 1 | sed "s/^\ *//g" | sed "s/\ *$//g" | sed "s/ (/(/" | tr '(' '\n' | tr -s ' ' | head -n 1 | sed s/\"/$QUOTECHAR/g`
      fi
      if [ -z "`echo "$PREMIERE" | tr -cd '0-9'`" ]; then
       PREMIERE=""
      fi
     fi
     if [ ! -z "$USELIMITED" ]; then
      LIMITED=`cat $TMPFILE | awk /Date$/,/endofThefile/ | grep -a -e "(" | grep -a -e "[Ll][Ii][Mm][Ii][Tt][Ee][Dd]" | head -n 1 | sed "s/^\ *//g" | sed "s/\ *$//g" | tr -s ' ' | sed s/\"/$QUOTECHAR/g`
#       if [ "$PREMIERE" = "$LIMITED" ]; then
#        PREMIERE=""
#       fi
     fi
    fi
   fi
   if [ ! -z "$IMDBHEAD" ]; then
    BOTHEAD=`echo $BOTHEADORIG | sed "s/RELEASENAME/$BOLD$IMDBDIR$BOLD/"`
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
     IMDBURL="`echo $IMDBURL | sed "s|/us.|/$LOCALURL.|g" | tr 'A-Z' 'a-z'`"
    fi
    HEADTMP="Title........: $BOLD$TITLE$BOLD"
    if [ ! -z "$COUNTRY" ]; then
     HEADTMP="$HEADTMP / $COUNTRY"
    fi
    if [ ! -z "$LANGUAGE" ]; then
     HEADTMP="$HEADTMP / $BOLD$LANGUAGE$BOLD"
    fi
    HEAD=`echo "$HEADTMP" | sed "s/Country......: //" | sed "s/Language.....: //" | tr -s ' '`
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
     echo "$DATE $TRIGGER \"$IMDBLKL\" \"Opening Stats: $BUSINESSSHORT\" \"$IMDBDST\"" | tr '[=$=]' '¤' | sed "s|¤|USD|g" >> $GLLOG
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
    if [ ! -z "$SHOWCOMMENTSHORT" ] && [ ! "$COMMENTSHORT" = "User Comments:" ] && [ -z "$BOTONELINE" ]; then
     echo "$DATE $TRIGGER \"$IMDBLKL\" \"$COMMENTSHORT\" \"$IMDBDST\"" >> $GLLOG
    fi
    if  [ ! -z "$BOTONELINE" ]; then
     if [ -z "$LOGFORMAT" ]; then
      echo "$DATE $TRIGGER \"$IMDBLKL\" \"$ONELINE\" \"$IMDBDST\"" >> $GLLOG
     elif [ "$LOGFORMAT" = "MYOWN" ]; then
#      NEWLINE="|"
      MYOWNPAIRS="%imdbdirname|IMDBDIR %imdburl|IMDBURL %imdbtitle|TITLE %imdbgenre|GENRECLEAN %imdbrating|RATINGCLEAN %imdbcountry|COUNTRYCLEAN %imdblanguage|LANGUAGECLEAN %imdbcertification|CERTCLEAN %imdbruntime|RUNTIMECLEAN %imdbdirector|DIRECTORCLEAN %imdbbusinessdata|BUSINESSSHORT %imdbpremiereinfo|PREMIERE %imdblimitedinfo|LIMITED %imdbvotes|RATINGVOTES %imdbscore|RATINGSCORE %imdbname|TITLENAME %imdbyear|TITLEYEAR %imdbnumscreens|BUSINESSSCREENS %imdbislimited|ISLIMITED %imdbcastleadname|CASTLEADNAME %imdbcastleadchar|CASTLEADCHAR %imdbtagline|TAGLINECLEAN %imdbplot|PLOTCLEAN %imdbbar|RATINGBAR %imdbcasting|CASTCLEAN %imdbcommentshort|COMMENTSHORTCLEAN %newline|NEWLINE %bold|BOLD"
      for OWNPAIR in $MYOWNPAIRS; do
       MYOWNSTRING="`echo "$OWNPAIR" | cut -d '|' -f 1`"
       MYOWNVAR="`echo "$OWNPAIR" | cut -d '|' -f 2`"
       if [ ! -z "${!MYOWNVAR}" ]; then
        MYTEMPVAR="`echo "${!MYOWNVAR}" | tr '\&' '\`'`"
        MYOWNFORMAT="`echo "$MYOWNFORMAT" | sed "s^$MYOWNSTRING^$MYTEMPVAR^g" | tr '\`' '\&'`"
       else
        MYOWNFORMAT="`echo "$MYOWNFORMAT" | sed "s^$MYOWNSTRING^$MYOWNEMPTY^g"`"
       fi
      done
      echo "$DATE $TRIGGER \"$IMDBLKL\" \"$MYOWNFORMAT\" \"$IMDBDST\"" | tr '[=$=]' '¤' | sed "s|¤|USD|g" >> $GLLOG
     else
      echo "$DATE $TRIGGER \"$IMDBLKL\" \"$IMDBDIR\" \"$IMDBURL\" \"$TITLE\" \"$GENRECLEAN\" \"$RATINGCLEAN\" \"$COUNTRYCLEAN\" \"$LANGUAGECLEAN\" \"$CERTCLEAN\" \"$RUNTIMECLEAN\" \"$DIRECTORCLEAN\" \"$BUSINESSSHORT\" \"$PREMIERE\" \"$LIMITED\" \"$RATINGVOTES\" \"$RATINGSCORE\" \"$TITLENAME\" \"$TITLEYEAR\" \"$BUSINESSSCREENS\" \"$ISLIMITED\" \"$CASTLEADNAME\" \"$CASTLEADCHAR\" \"$TAGLINECLEAN\" \"$PLOTCLEAN\" \"$RATINGBAR\" \"$CASTCLEAN\" \"$COMMENTSHORTCLEAN\" \"$IMDBDST\"" | tr '[=$=]' '¤' | sed "s|¤|USD|g" >> $GLLOG
     fi
    fi
   fi
   if [ ! -z "$DOTIMDB" ]; then

# Echo stuff to the .imdb file
##############################

    echo -e "$IMDBHEAD" > "$IMDBLNK"
    OWNER=`ls -1nl "$GLROOT$IMDBLKL" | tail -n 1 | { read junk junk owner group junk; echo $owner:$group; };`
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
     echo "$PLOT" | fold -s -w $IMDBWIDTH >> "$IMDBLNK"
    fi
    if [ ! -z "$SHOWCOMMENT" ] && [ ! -z "`echo $COMMENT | grep -a -e "^User Comments:" | grep -a -e "Check for other user comments.$"`" ]; then
     echo "---" >> "$IMDBLNK"
     echo "User Comments:" >> "$IMDBLNK"
     echo "$COMMENT" | grep -a -e "^     " | sed "s/^\ *//g" | sed "s/\ *$//g" | tr -s ' ' | fold -s -w $IMDBWIDTH >> "$IMDBLNK"
    fi
    echo -e "$IMDBTAIL" >> "$IMDBLNK"
   fi

   if [ ! -z "$INFOTEMPNAME" ]; then

# make a file/dir with imdb info in the name 

    INFOGENRES=`echo $GENRECLEAN | tr '/ ' '\n' |  sed -e /^$/d | wc -l`
    if [ ! $INFOGENRES -gt $INFOGENREMAX ]; then
     let INFOGENREMAXED=INFOGENRES
    else
     let INFOGENREMAXED=INFOGENREMAX
    fi
    if [ ! $INFOGENRES -lt 1 ]; then
     GENREFILE="`echo $GENRECLEAN | tr '/ ' '\n' |  sed -e /^$/d | head -n $INFOGENREMAX | tr '\n' ' ' | sed "s/ /$INFOGENRESEP/g" | cut -d "$INFOGENRESEP" -f 1-$INFOGENREMAXED`"
    else
     GENREFILE="Unclassified"
    fi
    VOTESFILE="`echo $RATINGVOTES | tr ',' '.'`"
    if [ -z "$ISLIMITED" ]; then
     ISLIMITED="unknown"
    fi
    NUMSCREENS="`echo $BUSINESSSCREENS | tr ',' '.'`"
    if [ -z "$NUMSCREENS" ]; then
     NUMSCREENS="unknown"
    fi
    if [ -z "$RUNTIMECLEAN" ]; then
     RUNTIMECLEAN="unknown"
    fi
    INFOFILENAMEOLD="`echo "$INFOFILENAME" | tr -c $INFOVALID $INFOCHARTO | sed "s%VOTES%*%g" | sed "s%SCORE%*%g" | sed "s%GENRE%*%g" | sed "s%RUNTIME%*%g" | sed "s%YEAR%*%g" | sed "s%ISLIMITED%*%g" | sed "s%SCREENS%*%g" | tr -d ' '`"
    INFOFILENAMEOLDA="`echo "$INFOFILENAMEOLD" | sed "s%*%.*%g"`"
    INFOFILENAMEOLDB="`echo "$INFOFILENAMEOLDA" | tr '\]\[' '.'`"
    INFOFILENAMEPRINT="`echo "$INFOFILENAME" | sed "s%VOTES%$VOTESFILE%g" | sed "s%SCORE%$RATINGSCORE%g" | sed "s%GENRE%$GENREFILE%g" | sed "s%RUNTIME%$RUNTIMECLEAN%g" | sed "s%YEAR%$TITLEYEAR%g" | sed "s%ISLIMITED%$ISLIMITED%g" | sed "s%SCREENS%$NUMSCREENS%g"`"
    INFOFILENAMEPRINT="`echo "$INFOFILENAMEPRINT" | tr -c $INFOVALID $INFOCHARTO`"
    if [ ! -z "`ls -1 "$GLROOT$IMDBLKL" | grep -a -e "$INFOFILENAMEOLDB"`" ]; then
     for OLDINFOFILE in `ls -1  "$GLROOT$IMDBLKL" | grep -a -e "$INFOFILENAMEOLDB" | tr -d ' '`; do
      rm -f "$GLROOT$IMDBLKL/"$OLDINFOFILE >/dev/null 2>&1
      rmdir "$GLROOT$IMDBLKL/"$OLDINFOFILE >/dev/null 2>&1
     done
    fi
    if [ -e "$GLROOT$IMDBLKL/$INFOTEMPNAME" ]; then
     mv "$GLROOT$IMDBLKL/$INFOTEMPNAME" "$GLROOT$IMDBLKL/$INFOFILENAMEPRINT"
    fi
   fi
# Should we run any external scripts?

   if [ ! -z "$EXTERNALSCRIPTNAME" ]; then
    FILENAMED=`ls -1Ftr "$GLROOT$IMDBLKL" | grep -a -v "/" | grep -a -v "@" | grep -a -e "[.][nN][fF][oO]" | head -n 1`
    if [ ! -z "$FILENAMED" ]; then
     touch -acmr "$GLROOT$IMDBLKL/$FILENAMED" "$GLROOT$IMDBLKL" >/dev/null 2>&1
    fi
    for EXTERNALNAME in $EXTERNALSCRIPTNAME; do
     if [ "$DEBUG" = "4" ] && [ ! -z "`head -n 1 $EXTERNALNAME | grep -a -e "/bin/bash"`" ]; then
      bash -x -v $EXTERNALNAME "\"$DATE\" \"$IMDBLNK\" \"$IMDBLKL\" \"$IMDBDIR\" \"$IMDBURL\" \"$TITLE\" \"$GENRECLEAN\" \"$RATINGCLEAN\" \"$COUNTRYCLEAN\" \"$LANGUAGECLEAN\" \"$CERTCLEAN\" \"$RUNTIMECLEAN\" \"$DIRECTORCLEAN\" \"$BUSINESSSHORT\" \"$PREMIERE\" \"$LIMITED\" \"$RATINGVOTES\" \"$RATINGSCORE\" \"$TITLENAME\" \"$TITLEYEAR\" \"$BUSINESSSCREENS\" \"$ISLIMITED\" \"$CASTLEADNAME\" \"$CASTLEADCHAR\" \"$TAGLINECLEAN\" \"$PLOTCLEAN\" \"$RATINGBAR\" \"$CASTCLEAN\" \"$COMMENTSHORTCLEAN\" \"$COMMENTCLEAN\""
     else
      $EXTERNALNAME "\"$DATE\" \"$IMDBLNK\" \"$IMDBLKL\" \"$IMDBDIR\" \"$IMDBURL\" \"$TITLE\" \"$GENRECLEAN\" \"$RATINGCLEAN\" \"$COUNTRYCLEAN\" \"$LANGUAGECLEAN\" \"$CERTCLEAN\" \"$RUNTIMECLEAN\" \"$DIRECTORCLEAN\" \"$BUSINESSSHORT\" \"$PREMIERE\" \"$LIMITED\" \"$RATINGVOTES\" \"$RATINGSCORE\" \"$TITLENAME\" \"$TITLEYEAR\" \"$BUSINESSSCREENS\" \"$ISLIMITED\" \"$CASTLEADNAME\" \"$CASTLEADCHAR\" \"$TAGLINECLEAN\" \"$PLOTCLEAN\" \"$RATINGBAR\" \"$CASTCLEAN\" \"$COMMENTSHORTCLEAN\" \"$COMMENTCLEAN\""
     fi
    done

# restore the releasedir's original date.
#########################################
    FILENAMED=`ls -1Ftr "$GLROOT$IMDBLKL" | grep -a -v "/" | grep -a -v "@" | grep -a -e "[.][nN][fF][oO]" | head -n 1`
    if [ ! -z "$FILENAMED" ]; then
     touch -acmr "$GLROOT$IMDBLKL/$FILENAMED" "$GLROOT$IMDBLKL" >/dev/null 2>&1
    fi
   fi
  fi

# clean up and make ready for next run.
#######################################

  cat $IMDBLOG | grep -a -F -v "$IMDBLINE" > $TMPFILE
  cat $TMPFILE > $IMDBLOG
  echo -n "" > $TMPFILE
 done
 echo -n "" > $IMDBPID
fi
exit 0
