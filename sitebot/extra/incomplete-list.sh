#!/bin/bash

# Where glftpd is installed, and where to find the
# racestats bin
glroot="/glftpd"
racestats="$glroot/bin/racestats"

# Enter here your sections/paths. These dirs will
# be searched and announced.
# You can use %d, %y etc variables date cmd uses
# see man date for help. You can also add / remove
# path variables... 
path[0]="$glroot/site/incoming/games:games"
path[1]="$glroot/site/incoming/apps:apps"
path[2]="$glroot/site/incoming/mp3/%m%d:mp3"
path[3]="$glroot/site/incoming/0day/%m%d:0day"
path[3]="$glroot/site/incoming/musicvideos:musicvideos"

# Verbose level.
# 0 - Only announce incomplete dirs.
# 1 - Announce incomplete dirs, and if the section does not
#     have any incomplete dirs.
verbose=1

# Set the format of the file/dir which marks a release
# incomplete.
multi_cd="[iNCoMPLETe]-[%C]-%R"       
single_cd="[iNCoMPLETe]-%R"     

# Scan today/yesterday's dir? (1 = yes, 0 = no)
show_today=1
show_yesterday=1

# bold char.
bold="\002"

#
#################### END OF CONFIG #############################
#

function list_incompletes () {
 topic=$1
 date=$2
 target=$3

 hits=0
 cd $3

 list=`ls -1 | awk 'BEGIN {
  m = "'"$multi_cd"'"
  s = "'"$single_cd"'"
  p = "'"$3"'"
  g = "'"$glroot"'"
  p = substr(p, length(g) + 1)
  a = split(m, ta, "")
  m = ""
  for ( c = 1 ; c <= a ; c++ ) if ( ta[c] == "[" ) m = m "\\\[";
                                              else m = m ta[c] 
                                             
  a = split(s, ta, "")
  s = ""
  for ( c = 1 ; c <= a ; c++ ) if ( ta[c] == "[" ) s = s "\\\[";
                                              else s = s ta[c]

  split(m, multi_1, "%C");
  split(m, multi_2, "%R");
  split(s, single, "%R")
 } {
  cp = length(multi_1[1]);
  rp = length(multi_2[1]);

  shown = 0

  if ( rp > cp ) {
   r1 = substr(m, 1, cp);
   r2 = substr(m, cp + 3, rp - cp - 2);
   r3 = substr(m, rp + 3);
   if ( match($0, r1 "..." r2 ".*" r3) ) {
    shown = 1
    c = length(substr($0, rp)) - length(r3)
    print p "" substr($0, rp, c) "/" substr($0, cp - 1, 3)
   } else if ( match($0, r1 "....." r2 ".*" r3) ) {  
    shown = 1
    c = length(substr($0, rp + 2)) - length(r3)
    print p "" substr($0, rp + 2, c) "/" substr($0, cp - 1, 5)
   }
  } else {
   r1 = substr(m, 1, rp);
   r2 = substr(m, rp + 3, cp - rp - 2);
   r3 = substr(m, cp + 3);
 
   if ( match($0, r1 ".*" r2 "..." r3) ) {
    shown = 1
    c = match($0, r2 "..." r3) - rp
    print p "" substr($0, rp, c) "/" substr($0, cp + 1, 3)
   } else if ( match($0, r1 ".*" r2 "....." r3) ) {   
    shown = 1
    c = match($0, r2 "....." r3) - rp
    print p "" substr($0, rp, c) "/" substr($0, cp + 1, 5)    
   }
  }

  if ( shown == 0 && match($0, single[1] ".*" single[2]) ) {
   c = length(substr($0, length(single[1]))) - length(single[2])
   print p "" substr($0, length(single[1]), c)
  }
 }'`

 if [ ! -z "$list" ] ; then
  for file in $list ; do
   data=`$racestats $file`
   if [ ! -z "$data" ] ; then 
    echo -e "[$bold$topic$bold] $data"
    hits=1; 
   fi
  done
 fi
 if [ $hits -eq 0 ] && [ $verbose -gt 0 ]; then
  echo -e "[$bold$topic$bold] No incomplete releases."
 fi
}


cnt=0
while [ ! -z "${path[$cnt]}" ] ; do
 current_dir=`echo ${path[$cnt]} | cut -d':' -f1`
 title=`echo ${path[$cnt]} | cut -d':' -f2`
 
 today_dir=`date +"$current_dir"`
 if [ ! -z `uname -s | grep -i "linux"` ]; then
  yesterday_dir=`date +"$current_dir" --date "1 day ago"`
 else
  yesterday_dir=`date -v-1d +"$current_dir"`
 fi

 if [ "$yesterday_dir" != "$today_dir" ] ; then
  if [ $show_yesterday -eq 1 ] && [ -e $yesterday_dir ] ; then
   list_incompletes "$title" "yesterday" "$yesterday_dir"
  fi
  if [ "$show_today" -eq 1 ] && [ -e "$today_dir" ] ; then
   list_incompletes "$title" "today" "$today_dir"
  fi
 else
  list_incompletes "$title" "today" "$current_dir"
 fi
  
 cnt=$[cnt + 1] 
done

