#!/bin/bash

# PSXC IMDB SANITY CHECK #
##########################

# This little thingy will check permissions, content etc in your setup. Hopefully, it will fix whatever
# problems you have. At least it's worth a try. It will not do anything dramatic (I hope), but imdb-logs
# may be cleaned etc. glftpd.log (or the equivalent in your setup) will be created if it do not exists,
# and chmod'ed world read-writable.

# version number. do not change.
VERSION="v2.7d"

# needed binaries. do not change.
BINARIES="date cat echo cut lynx grep head sed awk fold rm bash ls basename dirname chmod ps wc wget"
BINARIESCHROOT="wc grep tr sed head echo bash chmod basename pwd"

# your glftpd root path.
GLROOT=/glftpd

# path to the config file when chrooted by glftpd.
CONFFILE=/etc/psxc-imdb.conf

## End of config ##
###################

######################################################################################################

# check if configfile exists
############################

MYGLROOT=$GLROOT
if [ -r $MYGLROOT$CONFFILE ]; then
 . $MYGLROOT$CONFFILE
 if [ $? -ne 0 ]; then
  echo "Unable to open config file ($MYGLROOT$CONFFILE). Forced to exit."
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
CHGLROOT=$GLROOT
GLROOT=$MYGLROOT
  clear
  echo "This script will do a few things to make sure the script can be run"
  echo "without too much fuss. First it will check to see if all binaries are"
  echo "present. If binaries are missing in $GLROOT/bin, it will try to copy"
  echo "the needed ones there. All binaries listed will be chmod'ed ugoa+rx."
  echo "Should a binary fail to be found, this program will exit, and you must"
  echo "either broaden the path or copy the file yourself. If the script cannot"
  echo "chmod the binaries, it will NOT exit, but it will report it, leaving"
  echo "you to make sure it has the correct permissions."
  echo ""
  echo "It will then check all needed logfiles, creating those not present and"
  echo "chmod them ugoa-rw. Should this fail, this program will abort, leaving"
  echo "you to fix it."
  echo "Lastly, it will check to see if your config-file is world readable."
  echo ""
  echo "It will not currently check permissions on psxc-imdb.sh or the tcl-files"
  echo "if you use them, so you're on your own there. And this is no substitute"
  echo "for a brain... Things could still go wrong."
  echo ""
  echo "PRESS <RETURN> TO CONTINUE"; read line;
  clear
  echo "This script will not do Something Bad to your files, and should be quite"
  echo "safe to run whenever you wish/need to. Please remember this if a future"
  echo "upgrade leaves you hanging high and dry - running this script might be"
  echo "able to fix your problem. If new files are introduced, this script will be"
  echo "updated, so make sure you use the most recent one availible."
  echo ""
  echo "New in this version is the ability to check dependencies of the binaries"
  echo "in $GLROOT/bin - the ones we need in any case."
  echo "If you wish to run it, write 'depcheck'. It will first remove all the"
  echo "needed binaries in $GLROOT/bin, then copy them back from your environment"
  echo "as well as copy any missing libs the binaries need."
  echo "If you are nervous, you should make a backup of $GLROOT/bin before"
  echo "doing this, or use psxc-depcheck.sh after copying the bins."
  echo ""
  echo "Press <RETURN> or 'depcheck' to Start, <CTRL-C> to Break"; read line;
  echo ""
  echo "Searching in $GLROOT/bin: "
  if [ ! "$CHGLROOT" = "$GLROOT" ]; then
   BINARIESCHROOT="$BINARIESCHROOT $BINARIES"
  fi
  for BINAR in $BINARIESCHROOT; do
   if [ "$line" = "depcheck" ]; then
    echo "removing $BINAR"
    rm -f $GLROOT/bin/$BINAR
   fi
   echo -n " $BINAR "
   BINARF=""
   test -e $GLROOT/bin/$BINAR || {
     echo -n "missing. I'll try to fix...";
     BINARF=`which $BINAR`
     if [ ! -z "$BINARF" ]; then
      cp $BINARF $GLROOT/bin/ || {
       echo ""
       echo "Unable to copy $BINARF to $GLROOT/bin. Please try manually and run this script again."
       exit 1
      }
      echo "$BINAR copied. Checking dependencies."
      MYLDD=`which ldd`
      if [ -e $MYLDD ]; then
       for DEP in `$MYLDD $BINARF 2>/dev/null | grep "=>" | cut -d '>' -f 2 | cut -d ' ' -f 2`; do
        mkdir -p $GLROOT`dirname $DEP`
        if [ ! -e $GLROOT$DEP ]; then
         echo "copying $DEP to $GLROOT$DEP..."
         cp $DEP $GLROOT$DEP || { echo "failed to copy $DEP to $GLROOT$DEP - please try manually"; };
        fi
       done
      fi
     else
      echo "Unable to find $BINAR in your path. Please try manually and run this script again."
      exit 1;
     fi
   };
   if [ -z "$BINARF" ]; then
    echo -n "found."
   fi
   chmod ugoa+rx $GLROOT/bin/$BINAR || {
    BINARPERM=`ls -l $GLROOT/bin/$BINAR | grep "$BINAR" | cut -d ' ' -f 1 | head -n 1`
    echo -n " Failed to chmod the file. Current permissions on file: $BINARPERM"
   }
   echo ""
  done
  echo ""
  echo "Searching in $PATH: "
  for BINAR in $BINARIES; do
   echo -n " $BINAR "
   which $BINAR >/dev/null 2>&1 || {
    echo ""
    echo "$BINAR missing/not in path. Please fix."
    exit 1
   }
   echo -n "found."
   chmod ugoa+rx `which $BINAR` || {
    BINARWHICH=`which $BINAR`
    BINARPERM=`ls -l $BINARWHICH | grep "$BINAR" | cut -d ' ' -f 1 | head -n 1`
    echo -n " Failed to chmod the file. Current permissions on file: $BINARPERM"
   }
   echo ""
  done
  echo ""
  echo "Initializing the files..."
  echo -n " $GLLOG .. "
  if [ ! -e $GLROOT$GLLOG ]; then
   touch $GLROOT$GLLOG >/dev/null 2>&1
   if [ ! $? -eq 0 ] && [ ! -e $GLLOG ]; then
    touch $GLLOG >/dev/null 2>&1 || { echo "ERROR! Could not create \$GLLOG ($GLLOG or $GLROOT$GLLOG)"; exit 1; };
   fi
  fi
  if [ -e $GLROOT$GLLOG ]; then
   chmod ugoa+rw $GLROOT$GLLOG || { echo "could not make $GLROOT$GLLOG world writable."; exit 1; };
   MYGLROOT=$GLROOT
  else
   chmod ugoa+rw $GLLOG || { echo "could not make $GLLOG world writable."; exit 1; };
   MYGLROOT=""
  fi
  echo "OK."
  if [ ! -z "$PSXCFINDLOG" ]; then
   echo -n " $PSXCFINDLOG .. "
   touch $MYGLROOT$PSXCFINDLOG >/dev/null 2>&1 || { echo "ERROR! Could not create \$PSXCFINDLOG ($MYGLROOT$PSXCFINDLOG)"; exit 1; };
   chmod ugoa+rw $MYGLROOT$PSXCFINDLOG || { echo "could not make $MYGLROOT$PSXCFINDLOG world writable."; exit 1; };
   echo "OK."
  fi
  echo -n " $TMPFILE .. "
  touch $MYGLROOT$TMPFILE >/dev/null 2>&1 || { echo "ERROR! Could not create \$TMPFILE ($MYGLROOT$TMPFILE)"; exit 1; };
  chmod ugoa+rw $MYGLROOT$TMPFILE || { echo "could not make $MYGLROOT$TMPFILE world writable."; exit 1; };
  echo "OK."
  echo -n " $IMDBURLLOG .. "
  touch $MYGLROOT$IMDBURLLOG >/dev/null 2>&1 || { echo "ERROR! Could not create \$IMDBURLLOG ($MYGLROOT$IMDBURLLOG)"; exit 1; };
  chmod ugoa+rw $MYGLROOT$IMDBURLLOG || { echo "could not make $MYGLROOT$IMDBURLLOG world writable."; exit 1; };
  echo "OK."
  echo -n " $IMDBLOG .. "
  touch $MYGLROOT$IMDBLOG >/dev/null 2>&1 || { echo "ERROR! Could not create \$IMDBLOG ($MYGLROOT$IMDBLOG)"; exit 1; };
  chmod ugoa+rw $MYGLROOT$IMDBLOG || { echo "could not make $MYGLROOT$IMDBLOG world writable."; exit 1; };
  echo "OK."
  echo -n " $IMDBPID .. "
  touch $MYGLROOT$IMDBPID >/dev/null 2>&1 || { echo "ERROR! Could not create \$IMDBPID ($MYGLROOT$IMDBPID)"; exit 1; };
  chmod ugoa+rw $MYGLROOT$IMDBPID || { echo "could not make $MYGLROOT$IMDBPID world writable."; exit 1; };
  echo "OK."
  echo -n " $GLROOT$CONFFILE .. "
  chmod ugoa+r $GLROOT$CONFFILE || { echo "could not make $MYGLROOT$CONFFILE world readable."; exit 1; };
  echo "OK."
  echo -n " $GLROOT/bin/$PRENAME .. "
  if [ ! -e $GLROOT/bin/$PRENAME ]; then
   if [ -e $GLROOT/bin/psxc-imdb.sh ]; then
    cd $GLROOT/bin
    ln -s psxc-imdb.sh $PRENAME || { echo "could not make symlink to psxc-imdb. Please make it yourself."; break; };
    echo "OK."
   else
    echo "could not find psxc-imdb.sh in $GLROOT/bin. You must make the symlink yourself."
   fi
  else
   echo "symlink already exist."
  fi
  echo ""
  if [ ! -e /bin/bash ]; then
   echo -n "/bin/bash not found. Will try to link it there.."
   ln -s `which bash` /bin/ || { echo "could not make symlink to /bin/bash. Please make it yourself."; break; };
   echo "Done."
   echo ""
  fi
  echo -n "Checking version of bash ..."
  if [ ! -z "`/bin/bash --version 2>/dev/null`" ] && [ ! -z "`$GLROOT/bin/bash --version 2>/dev/null`" ]; then
   echo " OK."
  else
   echo " Failed."
   echo "    Please make sure you're using bash v2.05 or newer."
  fi
  echo -n "Verifying that lynx works ..."
  lynxerror="`lynx -dump -nolist -width=1000 -hiddenlinks=ignore -connect_timeout=10 http://www.imdb.com >/dev/null`"
  if [ $? -eq 0 ]; then
   echo " looks good."
  else
   echo " lynx returned an error. Please check and correct."
   echo "$lynxerror"
  fi
  echo -n "Verifying that wget works ..."
  wgeterror="`wget -U "Internet Explorer" -O /dev/null --timeout=10 http://www.imdb.com 2>/dev/null`"
  if [ $? -eq 0 ]; then
   echo " looks good."
  else
   echo " wget returned an error. Please check and correct."
   echo "$wgeterror"
  fi
  echo -n "Checking for locale settings ..."
  if [ -z "$LC_ALL" ]; then
   echo " OK."
  else
   echo " locale setting found."
   echo -n "    Testing for problems .."
   export LC_ALL=en_US
   sleep 0.$RANDOM >/dev/null 2>&1
   if [ $? -eq 0 ]; then
    echo "    fix should work."
   else
    echo "    fix does not work. Please contact psxc."
   fi
  fi
  echo -n "Checking for duplicate entries in $GLROOT$CONFFILE ..."
  dupa=`cat $GLROOT$CONFFILE | grep -e "^[A-Za-z]" | cut -d '=' -f 1 | uniq | wc | awk '{print $1}'`
  dupb=`cat $GLROOT$CONFFILE | grep -e "^[A-Za-z]" | cut -d '=' -f 1 | wc | awk '{print $1}'`
  dupc="`cat $GLROOT$CONFFILE | grep -e "^[A-Za-z]" | cut -d '=' -f 1 | uniq -d | tr '\n' ' '`"
  if [ $dupa -ne $dupb ]; then
   echo " duplicates found."
   echo "    Listing duplicates: $dupc"
   echo "    Please fix."
  else
   echo "OK."
  fi
  touch $GLROOT/etc/lynx.cfg
  if [ ! "$CHGLROOT" = "$GLROOT" ]; then
   if [ -z "`echo $LYNXFLAGS | grep -e "\-cfg="`" ]; then
    echo "You seem to run psxc-imdb totally under chroot. This require a change in your setup."
    echo "Please change the following in psxc-imdb.conf :"
    echo "   LYNXFLAGS=\"$LYNXFLAGS\""
    echo "to"
    echo "   LYNXFLAGS=\"$LYNXFLAGS -cfg=\$GLROOT/etc/lynx.cfg\""
    echo ""
   fi
  fi
  echo ""
  echo "Done testing."
  exit 0
 fi

