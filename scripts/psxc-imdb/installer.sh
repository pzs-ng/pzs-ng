#!/bin/bash

VERSION=2.7c

############################################################################
# some variables.                                                          #

# Fold output at this number of chars
foldline=77

# This is a tempfile.
tmpfile=./imdb-tempfile

installdir=.install
installvars=.install.vars
backupdir=.backup_`date +%Y%m%d-%H%M.%S`

scrlist="psxc-imdb.conf|c psxc-imdb-nuker.sh|g psxc-symlink-maker.sh|g psxc-imdb-dotimdb.pl|g psxc-imdb-rescan.sh|g psxc-imdb-find.sh|g psxc-imdb-sanity.sh|g psxc-imdb.tcl|s psxc-imdb.sh|g"

#                                                                          #
############################################################################

choose_editor ()
{
# Choose an editor for the user.
 while [ -z "$EDITOROK" ]; do
  if [ ! -z "$EDITOR" ]; then
   EDITOR=`basename $EDITOR`
  else
   if [ ! -z "`which nano`" ]; then
    EDITOR="nano -w"
   elif [ ! -z "`which pico`" ]; then
    EDITOR="pico"
   elif [ ! -z "`which vi`" ]; then
    EDITOR="vi"
   else
    EDITOR=""
   fi
  fi
  clear
  echo ""
  if [ -z "`which $EDITOR`" ]; then
   echo "Could not find your specified editor, or the EDITOR variable is blank."
   echo ""
  fi
  echo -n "During the install-process you need to make some config changes. You should choose a good editor to work in, like pico, vi or aee. Please name your favourite editor. [$EDITOR]> " | fold -s -w $foldline
  read line
  if [ ! -z "$line" ]; then
   EDITOR="$line"
  fi
  if [ ! -z "`which $EDITOR`" ]; then
   if [ -x `which $EDITOR` ]; then
    EDITOROK="OK"
   fi
  fi
 done

# To read files, 'less' is prefered over 'more'. If none of
# those can be found, use the editor.
 if [ ! -z "`which less`" ]; then
  MYPAGER=`which less`
 elif [ ! -z "`which more`" ]; then
  MYPAGER=`which more`
 else
  MYPAGER=$EDITOR
 fi
}

check_dir ()
{
# Check to see if we're in the correct dir, and that some of the files
# needed exists.
 if [ ! -e main/psxc-imdb.sh ] || [ ! -d extras ] || [ ! -e addons ]; then
  cd `dirname $0`
 fi
 if [ ! -e main/psxc-imdb.sh ] || [ ! -d extras ] || [ ! -e addons ]; then
  echo "Please start this script from within the installdir."
  exit 1
 fi
}

check_id ()
{
 if [ ! `id -u` -eq 0 ]; then
  echo "You need to be root (UID 0) to install this script."
  exit 1
 fi
}

find_glpaths ()
{
# Try to find glftpd.conf and the rootpath for glftpd.
 if [ -z "$GLBOTOK" ]; then
  ITYPE="`ps ax | grep inetd`"
  ITYPE=`echo "$ITYPE" | grep -v "ps" | grep -v "grep" | tr ' ' '\n' | grep -e "inetd"`
  ITYPE=`basename $ITYPE`
  if [ "$ITYPE" = "inetd" ]; then
   if [ -e /etc/inetd.conf ]; then
    GLFTPD_CONF="`cat /etc/inetd.conf | grep -e "^glftpd" | grep -e "-r" | sed "s|-r |-r_|" | tr ' ' '\n' | grep -e "-r" | head -n 1 | sed "s|-r_||"`"
   fi
  elif [ "$ITYPE" = "xinetd" ]; then
   if [ -e /etc/xinetd.d/glftpd ]; then
    GLFTPD_CONF="`cat /etc/xinetd.d/glftpd | grep -e "bin/glftpd" | grep -e "-r" | sed "s|-r |-r_|" | tr ' ' '\n' | grep -e "-r" | head -n 1 | sed "s|-r_||"`"
   fi
  fi
  if [ -z "$GLFTPD_CONF" ] || [ ! -e $GLFTPD_CONF ]; then
   if [ -e /etc/glftpd.conf ]; then
    GLFTPD_CONF="/etc/glftpd.conf"
   else
    GLFTPD_CONF=""
   fi
  fi
  if [ ! -z "$GLFTPD_CONF" ]; then
   GLROOT="`cat $GLFTPD_CONF | grep ^rootpath | awk '{print $2}'`"
  fi
  if [ -z "$GLROOT" ] || [ ! -e $GLROOT ]; then
   GLROOT="/glftpd"
  fi
  
  # In case we grabbed the wrong info, let the user verify.
  GLOK=""
  while [ -z "$GLOK" ]; do
   echo ""
   echo -n "Please enter the installdir of glftpd [$GLROOT]> "
   read line
   if [ ! -z $line ]; then
    GLROOT="$line"
   fi
   if [ -e $GLROOT/etc/glftpd.conf ] && [ -z "$GLFTPD_CONF" ]; then
    GLFTPD_CONF=$GLROOT/etc/glftpd.conf
   else
    GLFTPD_CONF=/etc/glftpd.conf
   fi
   echo -n "Please enter full path to glftpd.conf [$GLFTPD_CONF]> "
   read line
   if [ ! -z $line ]; then
    GLFTPD_CONF="$line"
   fi
   if [ -d $GLROOT ]; then
    if [ -e $GLFTPD_CONF ]; then
     GLOK="YES"
    fi
   fi
  done

  while [ -z "$GLBOTOK" ]; do
   echo ""
   echo "If you wish to use any of the tcl files that comes with the script, please enter the full path to your eggdrop's homedir here. If you do not have an eggdrop, enter '-' here." | fold -s -w $foldline
   echo -n "Full path to the eggdrop dir here: [$GLROOT/sitebot]> " | fold -s -w $foldline
   read line
   if [ -z "$line" ]; then
    GLBOT=$GLROOT/sitebot
   elif [ "$line" = "-" ]; then
    GLBOT=""
    GLBOTOK="OK"
    break
   else
    GLBOT=$line
   fi
   if [ ! -d $GLBOT ]; then
    echo "Unable to find a dir named $GLBOT"
   else
    EGGDROP_CONF=eggdrop.conf
    while [ -z "$EGGCONFOK" ]; do
     echo -n "Please enter the name of your eggdrop.conf: [$EGGDROP_CONF]> " | fold -s -w $foldline
     read line
     if [ ! -z "$line" ]; then
      EGGDROP_CONF=$line
     fi
     if [ -e $GLBOT/$EGGDROP_CONF ]; then
      EGGCONFOK="OK"
     else
      echo "Unable to find $GLBOT/$EGGDROP_CONF"
     fi
    done
    GLBOTPATH=`cat $GLBOT/$EGGDROP_CONF | grep -e "^source" | head -n1 | awk '{print $2}'`
    if [ -z $GLBOTPATH ]; then
     GLBOTPATH=`cat $GLBOT/$EGGDROP_CONF | grep -e "^set version" | head -n 1 | awk '{print $3}' | tr -cd 'a-zA-Z0-9\.'`
    fi
    if [ ! -z "$GLBOTPATH" ] && [ ! -z `echo $GLBOTPATH | grep -e "/"` ]; then
     GLBOTPATH=`echo $GLBOTPATH | cut -d '/' -f 1`
    else
     GLBOTPATH=scripts
    fi
    echo ""
    echo -n "The tcl files needs to be placed somewhere. Please enter the relative path (ie, without \"$GLBOT\" in front) you wish to place the files, or hit enter for the default. [$GLBOTPATH]> " | fold -s -w $foldline
    read line
    if [ ! -z "$line" ]; then
     GLBOTPATH=$line
    fi
    if [ ! -d $GLBOT/$GLBOTPATH ]; then
     echo -n "$GLBOT/$GLBOTPATH does not exists. Should I create it? [y]> " | fold -s -w $foldline
     read line
     if [ -z "$line" ] || [ "$line" = "y" ]; then
      mkdir -p $GLBOT/$GLBOTPATH
      GLBOTOK="OK"
     fi
    else
     GLBOTOK="OK"
    fi
   fi
  done
 fi
 
 # Get working.
 # First, grab some needed variables from glftpd's config
 GLDATA=`cat $GLFTPD_CONF | grep -e "^datapath" | head -n 1 | awk '{print $2}' | sed "s|/$||"`
 GLSITE=`cat $GLFTPD_CONF | grep -e "^min_homedir" | head -n 1 | awk '{print $2}' | sed "s|/$||"`
 GLROOT=`echo $GLROOT | sed "s|/$||"`
  echo -n "" >/$tmpfile || { 
   echo "I could not make a temporary file \"$tmpfile\". Cannot continue until this is fixed." | fold -s -w $foldline
   exit 1
 }
 test -w $GLROOT/etc || {
   echo "Unable to write in $GLROOT/etc. Cannot continue until this is fixed." | fold -s -w $foldline
   exit 1
 }
 test -w $GLROOT/bin || {
   echo "Unable to write in $GLROOT/bin. Cannot continue until this is fixed." | fold -s -w $foldline
   exit 1
 }
}

getversions ()
{
 if [ -z "$VERSIONSOK" ]; then
  if [ -e $installdir/psxc-imdb.sh ]; then
   destversion=`cat $installdir/psxc-imdb.sh | grep -e "^VERSION=" | head -n 1 | cut -d '=' -f 2 | tr -cd 'a-uA-U0-9\.'`
   if [ -e $GLROOT/bin/psxc-imdb.sh ]; then
    srcversion=`cat $GLROOT/bin/psxc-imdb.sh | grep -e "^VERSION=" | head -n 1 | cut -d '=' -f 2 | tr -cd 'a-uA-U0-9\.'`
   else
    srcversion=$destversion
   fi
   VERSIONSOK="YES"
  fi
 fi
}

backitup ()
{
 if [ -e $1 ]; then
  mkdir -p $backupdir
  cp $1 $backupdir
 else
  return 1
 fi
}

patchit ()
{
 patchok=""
 getversions
 if [ -e $1 ]; then
  if [ ! -z "$srcversion" ]; then
   cleanfile=`basename $1`
   echo "Trying to patch $1 from v${srcversion} to v${destversion}..." | fold -s -w $foldline
   if [ -e diff/${cleanfile}.patch.$srcversion.$destversion ]; then
    cat $1 >$tmpfile
    patch -N -p1 $tmpfile <diff/${cleanfile}.patch.$srcversion.$destversion 2>/dev/null 
    if [ $? -eq 0 ]; then
     echo -n "backing up ${cleanfile} ... " && backitup $1
     if [ $? -eq 0 ]; then
      patch -N -p1 $1 <diff/${cleanfile}.patch.$srcversion.$destversion && patchok="1"
     fi
    fi
    if [ -z "$patchok" ]; then
     echo ""
     echo -n "It looks like the patch will fail. Would you like to [r]eplace your original file with the new version, try to [p]atch anyway, or [i]gnore this for now? [i]> " | fold -s -w $foldline
     read line;
     if [ "$line" = "p" ]; then
      echo -n "backing up ${cleanfile} ... " && backitup $1
      if [ $? -eq 0 ]; then
       echo "OK." && patch -N -p0 $1 <diff/${cleanfile}.patch.$srcversion.$destversion && patchok="1"
      fi
      if [ $? -eq 0 ]; then
       echo "Failure."
      fi
     elif [ "$line" = "r" ]; then
      echo -n "backing up ${cleanfile} ... " && backitup $1
      if [ $? -eq 0 ]; then
       echo "OK." && cat $installdir/${cleanfile} >$1 && patchok="1"
      fi
     fi
    fi
   else
    echo "Could not find any patch for this file. Maybe it does not need any patching?" | fold -s -w $foldline
   fi
  else
   echo "Unable to find what version you are currently using." | fold -s -w $foldline
  fi
 else
  echo "unable to find $1." | fold -s -w $foldline
 fi
 if [ -z "$patchok" ]; then
  echo ""
  echo "Please take note on what failed."
  echo ""
  return 1
 fi
}

readyfiles()
{
 if [ -z "$FILESREADY" ]; then
  if [ -d $installdir ]; then
   rm -fR $installdir
  fi
  mkdir -p $installdir
  echo "Doing a quick mod on some files.. please wait..." | fold -s -w $foldline
  dirs="`ls -1F | grep -v "^\." | grep "/$" | grep -v "^diff"`"
  for dir in $dirs; do
   files="`ls -1F $dir | grep -v "/$" | grep -v "@$" | grep -v "gz$" | grep -v "README" | tr -d '\*'`"
   for file in $files; do
    cp -fp $dir$file $installdir
    if [ ! -z "`echo $file | grep -e "\.sh$"`" ]; then
     chmod 755 $installdir/$file
    elif [ ! -z "`echo $file | grep -e "\.pl$"`" ]; then
     chmod 755 $installdir/$file
    elif [ ! -z "`echo $file | grep -e "\.tcl$"`" ]; then
     chmod 600 $installdir/$file
     chown `ls -1ld $GLBOT | awk '{print $3":"$4}'` $installdir/$file
    elif [ ! -z "`echo $file | grep -e "conf"`" ]; then
     chmod 644 $installdir/$file
    else
     chmod 444 $installdir/$file
    fi
    glmodify $installdir/$file || echo "Failed to modify $file"
   done
  done
  FILESREADY="YES"
  echo ""
  echo "The modified files is now located in `pwd`/$installdir." | fold -s -w $foldline
  echo ""
  echo -n "Press <enter> to continue"
  read line
 fi
}

glmodify ()
{
 if [ -r $1 ]; then
  cat $1 | tr -d '' | sed "s|/etc/glftpd.conf|$GLFTPD_CONF|g" | \
   sed "s|/glftpd/|$GLROOT/|g" | sed "s|/glftpd$|$GLROOT|g" | \
   sed "s|/glftpd |$GLROOT |g" | sed "s|/glftpd\"|$GLROOT\"|g" | \
   sed "s|/ftp-data/|$GLDATA/|g" | sed "s|/ftp-data$|$GLDATA|g" | \
   sed "s|/ftp-data |$GLDATA |g" | sed "s|/ftp-data\"|$GLDATA\"|g" | \
   sed "s|/site/|$GLSITE/|g" | sed "s|/site$|$GLSITE|g" | \
   sed "s|/site |$GLSITE |g" | sed "s|/site \"|$GLSITE\"|g" >$tmpfile &&
   cat $tmpfile >$1
 else
  return 1
 fi
}

copy_scripts ()
{
 clear
 for scrnamed in $scrlist; do
  instfile=""
  scrname=`echo $scrnamed | cut -d '|' -f 1`
  scrpath=`echo $scrnamed | cut -d '|' -f 2`
  if [ -e $installdir/$scrname ]; then
   if [ "$scrpath" = "g" ]; then
    scrpath=$GLROOT/bin
   elif [ "$scrpath" = "c" ]; then
    scrpath=$GLROOT/etc
   elif [ "$scrpath" = "s" ]; then
    if [ ! -z "$GLBOTPATH" ]; then
     scrpath=$GLBOT/$GLBOTPATH
    else
     scrpath=/dev/null
    fi
   else
    echo "Something's fishy..."
    scrpath=/dev/null
   fi
   if [ ! "$scrpath" = "/dev/null" ]; then
    if [ -e $scrpath/$scrname ]; then
     instdef="s"
     echo "It seems that you have already installed $scrname." | fold -s -w $foldline
     echo "You can choose to overwrite your current file, patch it or skip the file." | fold -s -w $foldline
     echo "In case the file is patched or replaced, a backup will be taken." | fold -s -w $foldline
     echo -n "[o]verwrite or [s]kip? [$instdef]> " | fold -s -w $foldline
    else
     instdef="i"
     echo -n "Do you wish to [i]nstall $scrname or [s]kip for now? [$instdef]> " | fold -s -w $foldline
    fi
    read line
    if [ -z "$line" ]; then
     line=$instdef
    fi
    if [ "$line" = "o" ]; then
     instfile="OK"
     echo -n "backing up $scrname ... " && backitup $scrpath/$scrname
     if [ $? -eq 0 ]; then
      echo "OK."
     else
      echo "Failed."
     fi
    elif [ "$line" = "i" ]; then
     instfile="OK"
    elif [ "$line" = "p" ] || [ -z "$line" ]; then
      patchit $scrpath/$scrname && { echo "File is patched. Make sure paths in this file is correct." | fold -s -w $foldline; } || echo "Patching failed."
     instfile=""
    fi
    if [ ! -z "$instfile" ]; then
     echo "... copying $scrname to $scrpath" | fold -s -w $foldline
     cp -fRp $installdir/$scrname $scrpath/ || echo "Failed."
    fi
    echo ""
    echo -n "Do you wish to edit/check the config in $scrname? [y]> " | fold -s -w $foldline
    read line
    if [ -z "$line" ] || [ "$line" = "y" ]; then
     $EDITOR $scrpath/$scrname
    fi
    echo ""
   fi
  fi
 done
 echo "Please hit <enter> to continue."
 read line
}

modify_glftpd_conf ()
{
 echo ""
 echo "Do you wish me to modify your glftpd.conf?" | fold -s -w $foldline
 echo "This will add your chosen DOTIMDB file to the \"show_diz\"-line, and add \"imdb-rescan\" as a site command." | fold -s -w $foldline
 echo -n "Auto-modify glftpd.conf? [n]> " | fold -s -w $foldline
 read line
 if [ "$line" = "y" ]; then
  backitup $GLFTPD_CONF
  if [ -e $GLROOT/etc/psxc-imdb.conf ]; then
   dotimdb="`cat $GLROOT/etc/psxc-imdb.conf | grep -e "^DOTIMDB=" | cut -d "=" -f 2 | tr -d '\ \"' | head -n 1`"
   if [ ! -z "$dotimdb" ]; then
    if [ -z "`cat $GLFTPD_CONF | tr 'A-Z' 'a-z' | grep -e "^show_diz" | grep -e "$dotimdb"`" ]; then
     doexecnr=`nl -ba $GLFTPD_CONF | awk '{print $2,$1}' | grep -e "^show_diz" | tail -n 1 | awk '{print $2}'`
     doexecall=`nl -ba $GLFTPD_CONF | tail -n 1 | awk '{print $1}'`
     if [ ! -z "$doexecnr" ]; then
      head -n $doexecnr $GLFTPD_CONF >$tmpfile
      echo -e "show_diz\t$dotimdb\t*" >>$tmpfile
      let doexec=doexecall-doexecnr
      tail -n $doexec $GLFTPD_CONF >>$tmpfile
      cat $tmpfile >$GLFTPD_CONF
     else
      echo -e "show_diz\t$dotimdb\t*" >>$GLFTPD_CONF
     fi
     echo "$dotimdb added to show_diz"
    fi
   fi
  fi

  if [ -z "`cat $GLFTPD_CONF | tr 'A-Z' 'a-z' | grep -e "imdb-rescan"`" ]; then
   doexecnr=`nl -ba $GLFTPD_CONF | awk '{print $2,$1}' | grep -e "^site_cmd" | tail -n 1 | awk '{print $2}'`
   doexecall=`nl -ba $GLFTPD_CONF | tail -n 1 | awk '{print $1}'`
   if [ ! -z "$doexecnr" ]; then
    head -n $doexecnr $GLFTPD_CONF >$tmpfile
    echo -e "site_cmd\tIMDB-RESCAN\tEXEC\t/bin/psxc-imdb-rescan.sh" >>$tmpfile
    let doexec=doexecall-doexecnr
    tail -n $doexec $GLFTPD_CONF >>$tmpfile
    cat $tmpfile >$GLFTPD_CONF
   else
    echo -e "site_cmd\tIMDB-RESCAN\tEXEC\t/bin/psxc-imdb-rescan.sh" >>$GLFTPD_CONF
   fi
   docustomnr=`nl -ba $GLFTPD_CONF | awk '{print $2,$1}' | grep -e "^custom" | tail -n 1 | awk '{print $2}'`
   docustomall=`nl -ba $GLFTPD_CONF | tail -n 1 | awk '{print $1}'`
   if [ ! -z "$docustomnr" ]; then
    head -n $docustomnr $GLFTPD_CONF >$tmpfile
    echo -e "custom-imdb-rescan\t1" >>$tmpfile
    let docustom=docustomall-docustomnr
    tail -n $docustom $GLFTPD_CONF >>$tmpfile
    cat $tmpfile >$GLFTPD_CONF
   else
    echo -e "custom-imdb-rescan\t1" >>$GLFTPD_CONF
   fi
   echo "imdb-rescan added as a site command (flag 1 users only)"
   echo "backup of the old file is in $backupdir"
  fi
 fi
 echo ""
 echo "Please hit <enter> to continue."
 read line
}

modify_crontab ()
{
 echo ""
 echo "To make the imdb script work, it needs be run twice - once from the zipscript,"
 echo "and once from the bot or crontab. It is thus recommended to add a line in"
 echo "crontab - even if you use or plan to use a bot - to make sure it really runs."
 echo ""
 echo -n "Do you want me to add a line to (the botuser's) crontab? [y]> "
 read line
 if [ "$line" = "y" ] || [ -z "$line" ]; then
  while [ -z "$cronname" ]; do
   echo -n "Enter the name of the user you wish to hold the crontab entry >"
   read cronname
   cronid=`id -u $cronname 2>/dev/null`
   if [ -z "$cronid" ]; then
    echo "User $cronname does not exists."
    unset $cronname
   fi
  done
  crontab -u $cronname -l | grep -v "psxc-imdb.sh" >$tmpfile 2>/dev/null
  echo -e "*/10\t*\t*\t*\t*\t$GLROOT/bin/psxc-imdb.sh" >>$tmpfile
  crontab -u $cronname $tmpfile
  echo ""
  echo "crontab edited - it will now run the imdb-script every 10 minutes. Edit this"
  echo "to match your setup if you like."
 fi
}

modify_eggdrop_conf ()
{
 echo ""
 echo "Do you wish me to modify your eggdrop.conf?" | fold -s -w $foldline
 echo "This will add a line to source the tcl-file." | fold -s -w $foldline
 echo -n "Auto-modify $EGGDROP_CONF? [n]> "
 read line
 if [ "$line" = "y" ]; then
  if [ -z "`cat $GLBOT/$EGGDROP_CONF | grep -e "psxc-imdb.tcl"`" ]; then
   backitup $GLBOT/$EGGDROP_CONF
   echo "backup of the old file is in $backupdir"
   doeggnr=`nl -ba $GLBOT/$EGGDROP_CONF | awk '{print $2,$1}' | grep -e "^source " | tail -n 1 | awk '{print $2}'`
   doeggall=`nl -ba $GLBOT/$EGGDROP_CONF | tail -n 1 | awk '{print $1}'`
   if [ ! -z "$doeggnr" ]; then
    head -n $doeggnr $GLBOT/$EGGDROP_CONF >$tmpfile
    echo -e "source\t$GLBOTPATH/pzs-ng/plugins/psxc-imdb.tcl" >>$tmpfile
    let doegg=doeggall-doeggnr
    tail -n $doegg $GLBOT/$EGGDROP_CONF >>$tmpfile
    cat $tmpfile >$GLBOT/$EGGDROP_CONF
   else
    echo -e "source\t$GLBOTPATH/pzs-ng/plugins/psxc-imdb.tcl" >>$GLBOT/$EGGDROP_CONF
   fi
   echo "$GLBOTPATH/pzs-ng/plugins/psxc-imdb.tcl added to list of sources" | fold -s -w $foldline
  fi
 fi
 echo ""
 echo "Please hit <enter> to continue."
 read line
}

export_vars ()
{
 echo "#!/bin/bash" >$installvars
 echo "EDITOROK=\"$EDITOROK\"" >>$installvars
 echo "EDITOR=\"$EDITOR\"" >>$installvars
 echo "MYPAGER=\"$MYPAGER\"" >>$installvars
 echo "GLBOTOK=\"$GLBOTOK\"" >>$installvars
 echo "GLROOT=\"$GLROOT\"" >>$installvars
 echo "GLSITE=\"$GLSITE\"" >>$installvars
 echo "GLDATA=\"$GLDATA\"" >>$installvars
 echo "GLFTPD_CONF=\"$GLFTPD_CONF\"" >>$installvars
 echo "EGGDROP_CONF=\"$EGGDROP_CONF\"" >>$installvars
 echo "GLBOT=\"$GLBOT\"" >>$installvars
 echo "GLBOTPATH=\"$GLBOTPATH\"" >>$installvars
 echo "FILESREADY=\"$FILESREADY\"" >>$installvars
 echo "cronname=\"$cronname\"" >>$installvars
}

start_menu ()
{
# Start menu.
 MYSELECT=""
 while [ -z "$MYSELECT" ]; do
  clear
  echo ""
  echo "Welcome to the psxc-imdb v$VERSION installer."
  echo ""
  echo "Selections:"
  echo "1. Read the installation README."
  echo "2. Read the psxc-imdb README."
  echo "3. Read the psxc-imdb FAQ."
  echo "4. Read the psxc-imdb-find README."
  echo "5. Read the how-to on making your own formatted bot-output."
  echo "6. Read the psxc-imdb-dotimdb README."
  echo ""
  echo "7. Install/reinstall/upgrade psxc-imdb."
  echo "8. Modify install-files to fit your setup."
  echo "9. Run the sanity-checker."
  echo ""
  echo "g. Edit glftpd.conf."
  if [ ! -z "$EGGDROP_CONF" ]; then
   echo "e. Edit eggdrop.conf."
  fi
  if [ -e $installvars ]; then
   echo "r. Reset/zero the installer variables."
  fi
  echo ""
  echo "x. Exit."
  echo ""
  echo -n "Your choice: [1]> "
  read line
  if [ "$line" = "x" ]; then
   break
  elif [ "$line" = "1" ] || [ -z "$line" ]; then
   clear; choose_editor; $MYPAGER README.installer; echo ""
   echo "Please hit <enter> to continue."; read line; export_vars
  elif [ "$line" = "2" ]; then
   clear; choose_editor; $MYPAGER README; echo ""
   echo "Please hit <enter> to continue."; read line; export_vars
  elif [ "$line" = "3" ]; then
   clear; choose_editor; $MYPAGER FAQ; echo ""
   echo "Please hit <enter> to continue."; read line; export_vars
  elif [ "$line" = "4" ]; then
   clear; choose_editor; $MYPAGER README.psxc-imdb-find
   echo ""; echo "Please hit <enter> to continue."; read line; export_vars
  elif [ "$line" = "5" ]; then
   clear; choose_editor; $MYPAGER README.use.of.special.format; echo ""
   echo "Please hit <enter> to continue."; read line; export_vars
  elif [ "$line" = "6" ]; then
   clear; choose_editor; $MYPAGER README.psxc-imdb-dotimdb; echo ""
   echo "Please hit <enter> to continue."; read line; export_vars
  elif [ "$line" = "7" ]; then
   choose_editor
   find_glpaths
   readyfiles
   copy_scripts
   modify_glftpd_conf
   echo -n "Edit $GLFTPD_CONF? [n]> "
   read line
   if [ "$line" = "y" ]; then
    $EDITOR $GLFTPD_CONF
   fi
   modify_eggdrop_conf
   echo -n "Edit $EGGDROP_CONF? [n]> "
   read line
   if [ "$line" = "y" ]; then
    $EDITOR $GLBOT/$EGGDROP_CONF
   fi
   modify_crontab
   echo -n "All done. Should I run the sanity-checker? [y]> "
   read line
   if [ "$line" = "y" ] || [ -z "$line" ]; then
    if [ -e $GLROOT/bin/psxc-imdb-sanity.sh ]; then
     $GLROOT/bin/psxc-imdb-sanity.sh
    else
     echo "Error. Sanity-checker not found."
    fi
   fi
   echo ""
   echo "Please hit <enter> to continue."
   read line
   export_vars
  elif [ "$line" = "8" ]; then
   find_glpaths
   readyfiles
   export_vars
  elif [ "$line" = "9" ]; then
   choose_editor
   find_glpaths
   if [ -e $GLROOT/bin/psxc-imdb-sanity.sh ]; then
    $GLROOT/bin/psxc-imdb-sanity.sh
   else
    echo "Error. Sanity-checker not found."
   fi
   echo ""
   echo "Please hit <enter> to continue."
   read line
   export_vars
  elif [ "$line" = "g" ]; then
   choose_editor
   find_glpaths
   $EDITOR $GLFTPD_CONF
   echo ""
   echo "Please hit <enter> to continue."
   read line
   export_vars
  elif [ "$line" = "e" ]; then
   choose_editor
   find_glpaths
   $EDITOR $GLBOT/$EGGDROP_CONF
   echo ""
   echo "Please hit <enter> to continue."
   read line
   export_vars
  elif [ "$line" = "r" ]; then
   EDITOROK=""
   EDITOR=""
   MYPAGER=""
   GLBOTOK=""
   GLROOT=""
   GLSITE=""
   GLDATA=""
   GLFTPD_CONF=""
   EGGDROP_CONF=""
   GLBOT=""
   GLBOTPATH=""
   FILESREADY=""
   export_vars
  fi
 done
}

check_id
check_dir
if [ -e $installvars ]; then
 . $installvars
fi
start_menu
