#!/bin/bash

#############################################################################
# ng-install.sh                                                             #
# -------------                                                             #
# A small script that installs pzs-ng.                                      #
# You can run it at any time - no config-files will be overwritten, and no  #
# scripts will be overwritten. The only files that will be overwritten if   #
# they exists are the binary files and the bot files (not the bot config)   #
#                                                                           #
#############################################################################

# Enter glpath (usually not necessay - autodetect will take care of it).
glpath=

# Enter sslpath (usually not necessary - autodetect will take care of it).
sslpath=

# If you wish to install the bot, enter a valid path below.
eggpath="/path/to/eggdrop/"

# Do you use ss5? Leave empty if not.
use_ss5=

# Do you wish to edit the full config? Leave empty if not.
use_expert=

# Do you wish to compile the bins static? Leave empty if not.
use_static=

# Do you wish to add formatting in the zipscript output? Leave empty if not.
use_format=

# Do you wish to disable alternative output in sitewho? Leave empty if not.
disable_altwho=


## END OF CONFIG ##

PATH=./:/sbin:/bin:/usr/sbin:/usr/bin:/usr/games:/usr/local/sbin:/usr/local/bin:/usr/X11R6/bin:/$home/bin
possible_glroot_paths="/glftpd /jail/glftpd /usr/glftpd /usr/jail/glftpd /usr/local/glftpd /usr/local/jail/glftpd /$HOME/glftpd /glftpd/glftpd /opt/glftpd /opt/jail/glftpd"
scriptlist="./scripts/cleaner/cleaner.sh ./scripts/logtrimmer/logtrimmer.sh ./scripts/nfoextract/nfoextract.sh ./scripts/nfoextract/complete_script_wrapper.sh ./scripts/libcopy/libcopy.sh ./scripts/mp3-genre/mp3-genre-create.sh ./sitebot/extra/incomplete-list.sh ./sitebot/extra/invite.sh"

mywhich() {
  unset binname; for p in `echo $PATH | tr -s ':' '\n'`; do test -x $p/$bin && { binname=$p/$bin; break; }; done;
}

# Getting glpath
if [ ! -z "$glpath" ] && [ -e "$glpath" ]; then
  echo "Using glpath: $glpath"
else
  for possible_glroot in $possible_glroot_paths; do
    if [ -e ${possible_glroot}/bin/glftpd ]; then
      glpath=${possible_glroot}
      break
    fi
  done
  if [ ! -z "$glpath" ]; then
    echo "Using glpath: $glpath"
  else
    echo "Failed to locate glpath. Please set in config."
    exit 1
  fi
fi

# Checking eggdrop path
if [ ! -z "$eggpath" ] && [ -e "$eggpath" ]; then
  echo -n "Installing bot in $eggpath/pzs-ng/"
  mkdir -p $eggpath/pzs-ng/themes
  mkdir -p $eggpath/pzs-ng/plugins
  cp -fR sitebot/dZSbot.* sitebot/plugins sitebot/themes $eggpath/pzs-ng/
  if [ ! -e $eggpath/pzs-ng/dZSbot.conf ]; then
    cp sitebot/dZSbot.conf.dist $eggpath/pzs-ng/dZSbot.conf
  fi
  echo " .. DONE."
else
  echo "Bot will NOT be installed."
fi

# Setting configure options
unset configline
if [ ! -z "$use_ss5" ]; then configline="$configline --enable-ss5"; fi
if [ ! -z "$use_expert" ]; then configline="$configline --enable-expert"; fi
if [ ! -z "$use_static" ]; then configline="$configline --enable-static"; fi
if [ ! -z "$use_format" ]; then configline="$configline --enable-format"; fi
if [ ! -z "$disable_altwho" ]; then configline="$configline --disable-altwho"; fi
if [ "$glpath" != "/glftpd" ]; then configline="$configline --with-glpath=$glpath"; fi

# Copying some scripts
if [ -d $glpath/bin ]; then
  echo -en "\nCopying scripts : "
  for script in $scriptlist; do
    echo -n "$(basename $script):"
    if [ ! -e "$glpath/bin/$(basename $script)" ]; then
      cp -f $script "$glpath/bin/$(basename $script)"
      echo -n "OK "
    else
      echo -n "IGNORED "
    fi
  done
  echo
fi

# Compile/install zipscript and sitewho
echo -e "\nrunning 'make distclean'"
make distclean 2>/dev/null
echo -e "\nrunning './configure $configline'"
./configure $configline
echo -e "\nrunning 'make install'"
make install

# Run libcopy
if [ -e "$glpath/bin/libcopy.sh" ]; then
  echo -e "\nrunning $glpath/bin/libcopy.sh'"
  $glpath/bin/libcopy.sh "$glpath"
fi

# Check imdb script
if [ ! -e "$glpath/bin/psxc-imdb.sh" ]; then
  echo -n "Do you wish to install psxc-imdb? (Y/n)> "
  read answer
  if [ ! "`echo $answer | tr 'A-Z' 'a-z' | cut -c 1-1`" = "n" ]; then
    mypath=$PWD
    cd scripts/psxc-imdb
    ./installer.sh
    cd $mypath
  fi
else
  echo -e "\npsxc-imdb already installed. Ignoring it."
fi
if [ -e "$eggpath" ]; then
  echo -e "\nMake sure you add the following to your eggdrop.conf:"
  echo "  source pzs-ng/dZSbot.tcl"
fi
echo -e "\n\npzs-ng installed."

