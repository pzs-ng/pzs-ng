#!/bin/bash

# psxc-trailer v0.8.2009.03.19
##############################
#
# Small script that fetches the qt trailer and image for movies.
# Takes one argument (path to releasedir). If no arg is given, it uses
# current path.
#
# Required bins are:
# wget, sed, echo, tr, cut, head, tail, grep, bash, wc, basename, dirname, uname, stat, chmod
#
# Use as a site command:
#   Make sure all required bins are availible in chroot.
#   Add the following to glftpd.conf
#     site_cmd		TRAILER	EXEC	/bin/psxc-trailer.sh
#     custom-trailer	1 2 7
#
# Use as a nfo-script in pzs-ng:
#   Not really recommended as it may take a few seconds for the script to
#   download the trailer.
#
# Use with psxc-unpack:
#   Works fine with latest release. Since psxc-unpack is run inside and outside
#   chroot this script must be accessible (by ln -s or otherwise) both ways.
#
# Use with psxc-imdb:
#   Not tested, but should work fine. This script will discard $1 (first arg)
#   if it cannot use it and instead work with $PWD.
#
# Can probably also be used with total-rescan and other scripts, but this is
# untested.
#
# NOTE FOR LINUX USER:
#   If this script does not work as a site command, and the command
#   'chroot /glftpd /bin/wget http://www.apple.com' give you a 'Resolving failed'
#   error, try the following:
#     cp /lib/libnss_dns* /glftpd/lib/
#     cp /lib/libresolv* /glftpd/lib/
#     cp -fRp /etc/resolvconf /glftpd/etc/     (only applicable on some systems)
#
# QUICK WAY TO COPY NEEDED BINS:
#   for bin in $(grep -A 1 "^# Required bins" psxc-trailer.sh | tail -n 1 | tr -d '#,'); do
#     cp $(which $bin) /glftpd/bin/
#   done
# Remember to run libcopy afterwards.
#
################# CONFIG OPTIONS #################
#
# What dirs to execute in (no wildcards). Use "/" to include all.
# Example: validdirs="/site/MOVIES/ /site/FILMS/"
validdirs="/"

# quality of trailer. Choose between
# 320 (smallest), 480, 640, 480p, 720p, 1080p (highest)
# More than one quality setting is allowed - first found will be used
# use "" to disable
# Example: trailerquality="480p 640 480 320"
trailerquality="480p 640 480 320"

# what name should be used on the trailer?
# use "" to keep name as is.
# Example: trailername="trailer.mov"
trailername="trailer.mov"

# should all trailers be downloaded to a special dir? The default is
# "" which downloads to releasedir. This variable actually takes two
# dirs - one for chroot and one for outside chroot.
# NOTE: if both trailerdir and trailername is set, the name of the trailer
# will be named "name.of.searched.for.movie.mov".
# If none of the trailerdirs are found, the trailer will be downloaded
# in the releasedir.
# Example: trailerdirs="/site/trailers /glftpd/site/trailers"
trailerdirs=""

# Should we download to both releasedir and trailerdir?
# Set to "yes" to use both, and "" to use only one.
# Example: usebothdirs="yes"
usebothdirs=""

# download trailer image? ("yes"=yes, ""=no)
# Example: downloadimage=""
downloadimage="yes"

# if yes, what name is to be used?
# Example: imagename="folder.jpg
imagename=folder.jpg

# you can define how accurate you wish the search to be. lower the number
# if you need more results, or increase if you get a lot of false positives
# Example: accuracy=1
accuracy=2

#
###### ADVANCED CONFIG - USUALLY NO NEED TO CHANGE THESE VARS ######
#
# subdirs - regex to match. The default checks for cd, dvd, disk, disc, subs.
subdirs="^[CcDdSs][DdVvIiUu][DdSsBb]?[CcKkSs]?[0-9A-Za-z]?[0-9A-Za-z]?$"

# words to ignore/remove from search - case does not matter
removewords="XViD DiVX H264 x264 DVDR 720p 1080p BluRay BluRay BRRiP HDTV CAM TC TELECINE TS TELESYNC SCREENER R5 SCR DVDSCR DVDSCREENER DVDRiP REPACK PROPER INTERNAL LiMiTED UNRATED READNFO LINE DiRECT SYNCFIX AC3 DTS DTS-ES"

# movie-releases usually have at least one of the 'removewords' listed in the dirname.
# should we exit if we cannot find one? set to "yes" if we do, or "" if not.
checkwords="yes"

# wget output - usually set to /dev/null but can be directed to a file for debug purposes
wgetoutput=/dev/null

# wget tempfile - usually no need to change
wgettemp=the.fake

# make sure we have access to all bins needed. Should not need to change this
PATH=$PATH:/bin:/usr/bin:/usr/local/bin:/sbin:/usr/sbin:/usr/local/sbin:/glftpd/bin:$HOME/bin

# We use extended regex in sed - the switch differ in bsd and linux. The script has a autodetect
# feature, but if that fails you may need to set the switch manually. Set to "-E on BSD and
# "-r" on linux. The default is "".
sedswitch=""

# stat differs from system to system. We use stat to find perms on the dir(s) we try to save the
# trailer and image. If you end up with 0-byte files or permission-errors you should change this
# setting. FBSD uses "-f %p", linux uses "-c %a". The script will autodetect if this setting is
# set to it's default value, "".
statswitch=""

# We use certain flags with wget. Here we list the flags. Only change if you know what you're doing
wgetflags='--ignore-length --timeout=10 -U "Internet Explorer"'

# debug option. do not remove the hash unless you know what you're doing
#set -x -v

######################################## END OF CONFIG ##########################################

# code below
[[ "$1" != "" && -d "$1" ]] && {
  cd $1
}
[[ "$(basename "$PWD" | grep -E "$subdirs")" != "" ]] && {
  cd $(dirname $PWD)
}
founddir=""
for validdir in $validdirs; do
  [[ "$(echo $PWD | grep $validdir)" != "" ]] && {
    founddir="yes"
    break
  }
done
[[ "$founddir" == "" ]] && {
  echo "Not in a moviedir - not searching for a trailer"
  exit 0
}
[[ "$sedswitch" == "" && "$(uname | grep -i "bsd$")" == "" ]] && {
  sedswitch="-r"
} || {
  sedswitch="-E"
}
[[ "$statswitch" == "" && "$(uname | grep -i "bsd$")" == "" ]] && {
  statswitch="-c %a"
} || {
  statswitch="-f %p"
}
trailerdir=""
for trailer in $trailerdirs; do
  [[ -d "$trailer" ]] && {
    trailerdir=${trailer}/
    break
  }
done
count=$(echo $PWD | tr -cd '-' | wc -c)
let count=count+0
[[ count -ge 1 ]] && {
  releasename="$(echo "$PWD" | tr '/' '\n' | tail -n 1 | cut -d '-' -f 1-$count | tr 'A-Z' 'a-z')"
} || {
  releasename="$(echo "$PWD" | tr '/' '\n' | tail -n 1 | tr 'A-Z' 'a-z')"
}
while [ 1 ]; do
  whilename=$releasename
  for word in $removewords; do
    word=$(echo $word | tr 'A-Z' 'a-z')
    rname="$(echo "$releasename" | sed $sedswitch "s/[\.|_]$word$//")"
    [[ "$releasename" != "$rname" ]] && {
      releasename=$rname
      ismovie="yes"
      break
    }
  done
  [[ "$releasename" == "$whilename" ]] && {
    break
  }
done

[[ "$ismovie" == "" && "$checkwords" != "" ]] && {
  echo "Not in a moviedir - not searching for a trailer"
  exit 0
}

echo "Looking up trailer for \"$(echo $releasename | tr '\.' ' ')\"..."
orgrelname=$releasename
countdot=$(echo $releasename | tr -cd '\.' | wc -c)
let countdot=countdot-0

while [ 1 ]; do
  releasename="$(echo "$releasename" | sed $sedswitch "s/[\.|_][12][0-9][0-9][0-9]$//" | tr -d '\.')"
  output="$(wget $wgetflags -o $wgetoutput -O - "http://www.apple.com/trailers/home/scripts/quickfind.php?q=$releasename")"
  outparse="$(echo $output | tr -d '\"' | tr ',' '\n')"
  iserror=$(echo $outparse | grep -i "error:true")
  isresult=$(echo $outparse | grep -i "results:\[\]")
  [[ "$iserror" != "" ]] && {
    echo "An error occured. Unable to parse output"
    exit 1
  }
  [[ "$isresult" == "" ]] && {
    break
  }
  [[ $countdot -le $accuracy ]] && {
    break
  }
  releasename=$(echo $orgrelname | cut -d '.' -f 1-$countdot)
  let countdot=countdot-1
  [[ $countdot -lt 1 ]] && {
    break
  }
done
[[ "$isresult" != "" ]] && {
  echo "Unable to find movie"
  exit 0
}

poster="$(echo "$outparse" | grep -i "^poster:" | cut -d ':' -f 2- | tr -d '\\')"
location="http://www.apple.com$(echo "$outparse" | grep -i "^location:" | cut -d ':' -f 2- | tr -d '\\' | tr ' ' '\n' | head -n 1)"

output2="$(wget $wgetflags -o $wgetoutput -O - $location)"
output2parse="$(echo $output2 | tr ' \?' '\n' | grep -v "/images/" | grep -E -i "^href=.*\.mov[\"]*$|^href=.*small[_]?.*\.html[\"]*|^href=.*medium[_]?.*\.html[\"]*|^href=.*large[_]?.*\.html[\"]*|^href=.*low[_]?.*\.html[\"]*|^href=.*high[_]?.*\.html[\"]?.*" | tr -d '\"' | cut -d '=' -f 2-)"
#echo DEBUG 1: $output2parse
#echo DEBUG 5: $location
for quality in $trailerquality; do
  urllink=$(echo "$output2parse" | grep -i "${quality}.mov$" | head -n 1)
  [[ "$urllink" != "" ]] && {
    break
  }
  sublink=""
  [[ "$quality" == "320" && "$(echo "$output2parse" | tr ' ' '\n'  | grep -E -i "small[_]?.*.html|low[_]?.*.html")" != "" ]] && {
    sublink=${location}$(echo "$output2parse" | grep -v "/images/" | grep -E -i "small.html[_]?.*|low[_]?.*.html" | tr '\>\<\ ' '\n' | head -n 1)
    movsearch="\.mov\$"
  }
  [[ "$quality" == "480" && "$(echo "$output2parse" | tr ' ' '\n' | grep -E -i "medium[_]?.*.html")" != "" ]] && {
    sublink=${location}$(echo "$output2parse" | grep -v "/images/" | grep -E -i "medium[_]?.*.html" | tr '\>\<\ ' '\n' | head -n 1)
    movsearch="\.mov\$"
  }
  [[ "$quality" == "640" && "$(echo "$output2parse" | tr ' ' '\n'  | grep -E -i "large[_]?.*.html|high[_]?.*.html")" != "" ]] && {
    sublink=${location}$(echo "$output2parse" | grep -v "/images/" | grep -E -i "large[_]?.*.html|high[_]?.*.html" | tr '\>\<\ ' '\n' | head -n 1)
    movsearch="\.mov\$"
  }
  [[ "$quality" == "480p" || "$quality" == "720p" || "$quality" == "1080p" ]] && {
    sublink="${location}hd/"
    movsearch="${quality}.mov\$"
  }
  #echo DEBUG 4: $sublink
  [[ "$sublink" != "" ]] && {
    output3="$(wget $wgetflags -o $wgetoutput -O - $sublink)"
    output3parse="$(echo $output3 | tr -c 'a-zA-Z/:\._0-9\-' '\n' | grep -E -i "$movsearch")"
    urllink=$(echo "$output3parse" | grep -v "/images/" | grep -i "\.mov$" | head -n 1)
    #echo DEBUG 2: $output3parse
  }
  [[ "$urllink" != "" ]] && {
    break
  }
done
#echo DEBUG 3: $urllink
[[ "$urllink" == "" ]] && {
  echo "Failed to fetch movielink"
  exit 0
}

# download trailer and picture
[[ "$(echo "$wgettemp" | grep "^/")" == "" ]] && {
  wgettemp="$(echo "${PWD}/${wgettemp}" | tr -s '/')"
}
[[ "$trailerquality" != "" && "$urllink" != "" ]] && {
  [[ ! -w $(dirname $wgettemp) ]] && {
    wgettempperms=$(stat $statswitch $(dirname $wgettemp))
    [[ ! -w $(dirname $wgettemp) ]] && {
      echo "ERROR! Cannot save file $wgettemp"
      exit 1
    }
    chmod +w $(dirname $wgettemp)
  }
  wget $wgetflags -o $wgetoutput -O $wgettemp $urllink
  fakelinkname=$(echo $urllink | tr '/' '\n' | grep -i "\.mov$")
  reallinkname=$(cat $wgettemp | tr -c 'a-zA-Z0-9\-\.\_' '\n' | grep -i "\.mov" | sed "s|[Rr][Mm][Dd][Rr]||" | sed "
s|^0||")
  reallink=$(echo $urllink | sed "s|$fakelinkname|$reallinkname|")
  rm -f $wgettemp
  [[ "$wgettempperms" != "" ]] && {
    chmod $wgettempperms $(dirname $wgettemp)
  }
  [[ "$trailerdir" != "" ]] && {
    [[ "$trailername" != "" ]] && {
      orgtrailername=$trailername
    }
    trailername=$(echo ${orgrelname}.mov | tr -c 'a-zA-Z0-9\-\.\n' '.')
  }
  [[ "$trailername" == "" ]] && {
    trailername=$(echo $urllink | tr '/' '\n' | grep -i "mov$" | tail -n 1)
  }
  [[ "$orgtrailername" == "" ]] && {
    orgtrailername=$trailername
  }
  echo "Downloading trailer in $quality quality as $trailername"
  [[ "$trailerdir" == "" ]]&& {
    trailerdir="${PWD}/"
  }
  [[ ! -w $trailerdir ]] && {
    trailerdirperms=$(stat $statswitch $trailerdir)
    [[ ! -w $trailerdir ]] && {
      echo "ERROR! Cannot save file $trailername"
      exit 1
    }
    chmod +w $trailerdir
  }
  wget $wgetflags -o $wgetoutput -O ${trailerdir}${trailername} $reallink
  [[ ! -s ${trailerdir}${trailername} ]] && {
    echo "For unknown reasons the script failed to download the trailer"
    echo "Please report this as a bug to the developer (psxc - psxc@psxc.com)"
    echo "Do not forget to include the name of the movie that failed."
    rm -f ${trailerdir}${trailername}
    exit 1
  }
  [[ "$trailerdirperms" != "" ]] && {
    chmod $trailerdirperms $trailerdir
  }
  [[ "$trailerdir" != "" && "$usebothdirs" != "" ]] && {
    [[ "$(echo "$orgtrailername" | grep "^/")" == "" ]] && {
      orgtrailername="$(echo "${PWD}/${orgtrailername}" | tr -s '/')"
    }
    [[ ! -w $(dirname $orgtrailername) ]] && {
      orgtrailernameperms=$(stat $statswitch $(dirname $orgtrailername))
      [[ ! -w $(dirname $orgtrailername) ]] && {
        echo "ERROR! Cannot save file $orgtrailername"
        exit 1
      }
      chmod +w $(dirname $orgtrailername)
    }
    cp -fp ${trailerdir}${trailername} $orgtrailername
    [[ "$orgtrailernameperms" != "" ]] && {
      chmod $orgtrailernameperms $(dirname $orgtrailername)
    }
  }
}
[[ "$downloadimage" != "" && "$poster" != "" ]] && {
  echo "Downloading posterimage as $imagename"
  [[ "$(echo "$imagename" | grep "^/")" == "" ]] && {
    imagename="$(echo "${PWD}/${imagename}" | tr -s '/')"
  }
  [[ ! -w $(dirname $imagename) ]] && {
    imagenameperms=$(stat $statswitch $(dirname $imagename))
    [[ ! -w $(dirname $imagename) ]] && {
      echo "ERROR! Cannot save file $imagename"
      exit 1
    }
    chmod +w $(dirname $imagename)
  }
  wget $wgetflags -o $wgetoutput -O $imagename $poster
  [[ "$imagenameperms" != "" ]] && {
    chmod $imagenameperms $imagename
  }
}

echo "done"
exit 0

