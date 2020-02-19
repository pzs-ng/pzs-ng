#!/bin/bash

# This script will do a rescan of the imdb info in current dir (and dirs
# directly below the current dir). The script should be set up as a site
# command.

# 1. edit the config below
# 2. copy this file to /glftpd/bin
# 3. add the following in glftpd.conf
#       site_cmd IMDB-RESCAN EXEC /bin/psxc-imdb-rescan.sh
#       custom-imdb-rescan   1 =staff
#
# syntax is:
# site imdb-rescan [-r|-1] (-nobot) (-nofile) (-noaddon) (-queue)

# config
########

# Version. No need to change
VERSION=2.9w

# Path to psxc-imdb.sh. This is relative to GLROOT.
PSXC_IMDB=/bin/psxc-imdb.sh

# Directories and symlinks to exclude, seperated by a |.
# We don't want to rescan nuked dirs, so put here the output of this line:
#
# grep "^nukedir_style" /etc/glftpd.conf | awk '{print $2}' | tr '%' '\n' | head -n 1
#
# We also don't want to rescan incomplete or missing-file symlinks (in case you use sorting,
# these symlinks will appear in your sorted dirs), check the output of the following line
# and add accordingly:
#
# /glftpd/bin/zipscript-c --fullconfig | grep "_indicator"
#
# Be sure to put a \ before each ()[]
EXCLUDES="^NUKED-|^\(incomplete\)-|^\(no-sfv\)-|^\(no-nfo\)-|^\(no-sample\)-"

# When the following file is found in the dir, the imdb-link inside that file
# will be used over that of the nfo. Useful for when a wrong link is present
# in the nfo or when no nfo is present at all.
# It is recommended to not use an nfo-file here and to use an extension that is
# ignored by the zipscript.
PREF_NFO="real.imdb"

# end of config
###############

if [ $# -eq 0 ]; then
 echo "PSXC's imdb rescanner v$VERSION."
 echo "----------------------------"
 echo "Syntax:"
 echo "site imdb-rescan [-1|-r] (-nobot) (-nofile) (-noaddon) (-queue)"
 echo "  -1 = scan current dir"
 echo "  -r = scan all dirs below current"
 echo ""
 echo "  -nobot    = no bot output in channel."
 echo "  -nofile   = no file changes in releasedir."
 echo "  -noaddon  = do not run any addons."
 echo "  -queue    = enable queuemode"
 echo ""
 echo "Queuemode: when enabled, this allows for multiple psxc-imdb-rescans"
 echo " being run after eachother. Each run needs to have the -queue flag,"
 echo " except for the last one. If no psxc-imdb-rescan is run without the"
 echo " -queue flag, the script will not know the queue has ended and thus"
 echo " keep the lock. Running multiple rescans simultaneously will not work."
 echo "Use caution - chanflooding may occur."
 echo "-1 is not needed if one or more of the other flags are specified."
 echo "You can combine any of the three -no flags if you wish."
 echo ""
 exit 0
fi
ARG=$1

PSXC_CONF="`cat "$PSXC_IMDB" | grep -e "^CONFFILE" | head -n 1 | cut -d '=' -f 2`"
. $PSXC_CONF

if [ ! -z $GLROOT ]; then
 MYTMPFILE="`echo "$TMPFILE" | sed "s%$GLROOT%%"`"
 MYTMPRESCANFILE="`echo "$TMPRESCANFILE" | sed "s%$GLROOT%%"`"
 IMDBPIDCHROOTED="`echo "$IMDBPID" | sed "s%$GLROOT%%"`"
else
  MYTMPFILE=$TMPFILE
  MYTMPRESCANFILE=$TMPRESCANFILE
  IMDBPIDCHROOTED=$IMDBPID
fi

echo "Checking to see if psxc-imdb is running..."
# If the PID is -1, it means we had psxc-imdb-rescan started in queuemode before
[[ ! -z "`cat $IMDBPIDCHROOTED`" && ( ( 1 -eq "`cat $IMDBPIDCHROOTED | wc -l`" && -1 -ne "`cat $IMDBPIDCHROOTED`" ) || 2 -eq "`cat $IMDBPIDCHROOTED | wc -l`" ) ]] &&
 sleep 1
IMDBPIDF="$(cat "$IMDBPIDCHROOTED")"
[[ ! -z "$IMDBPIDF" && ( ( 1 -eq "$(echo $IMDBPIDF | wc -l)" && -1 -ne "`cat $IMDBPIDCHROOTED`" ) || 2 -eq "`cat $IMDBPIDCHROOTED | wc -l`" ) ]] &&
 echo "I'm sorry. It looks to me that psxc-imdb or psxc-imdb-rescan is already running." &&
 echo "I cannot perform a rescan while this is the case. Please try again a bit later." &&
 exit 2

echo $$ > "$IMDBPIDCHROOTED"

if [ ! -z "`echo $@ | grep -e "[-][nNqQ][oOUu][bBfFaAEe][oOiIdDUu][tTlLdDEe]"`" ]; then
 PSXCFLAGS=$@
 PSXCFLAG=0

 [[ ! -z "`echo "$@" | grep -e "[nN][oO][bB][oO][tT]"`" ]] &&
  let PSXCFLAG=PSXCFLAG+1
 [[ ! -z "`echo "$@" | grep -e "[nN][oO][fF][iI][lL][eE]"`" ]] &&
  let PSXCFLAG=PSXCFLAG+2
 [[ ! -z "`echo "$@" | grep -e "[nN][oO][aA][dD][dD][oO][nN]"`" ]] &&
  let PSXCFLAG=PSXCFLAG+4
 [[ ! -z "`echo "$@" | grep -e "[Qq][Uu][Ee][Uu][Ee]"`" ]] &&
  QUEUED=1 && echo "-1" >> "$IMDBPIDCHROOTED"

 echo $PSXCFLAG >$MYTMPRESCANFILE

fi

proc_doscan() {
 IMDB_NFO="$(ls -1 *.[Nn][Ff][Oo] 2>/dev/null)"
 NFO_CNT="$(echo "$IMDB_NFO" | wc -l)"
 [[ 1 -lt $NFO_CNT ]] && echo "WARNING: More than one nfo found" >&2
 [[ -f "$PREF_NFO" ]] &&
  echo "Processing $PREF_NFO" &&
  $PSXC_IMDB $PREF_NFO &&
  IMDB_NFO=""
 # Can"t rely on NFO_CNT as it'll be 1 even when there are none (an echo+wc-l gimmick)
 [[ ! -z "$IMDB_NFO" ]] &&
  for NFO in $IMDB_NFO; do
   echo "Processing $NFO" &&
   $PSXC_IMDB $NFO
  done
}

if [ ! "$ARG" = "-r" ]; then
 echo "Doing a scan in current dir .. searching for iMDB info."
 proc_doscan
else
 echo "Scanning recursively for iMDB info.."
 for REL_DIR in `ls -1F | egrep "@$|/$" | grep -E -v "($EXCLUDES)" | tr ' ' '%'`; do
  REL_DIR="`echo $REL_DIR | tr '%@' ' /'`"
  echo "$REL_DIR ..."
  cd "$REL_DIR"
  proc_doscan
  cd ..
 done
fi

echo "Done."
echo "-1" > "$IMDBPIDCHROOTED"
[[ -z "$QUEUED" ]] && > "$IMDBPIDCHROOTED"

exit 0
