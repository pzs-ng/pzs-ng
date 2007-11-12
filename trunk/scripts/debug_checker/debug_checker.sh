#!/bin/bash

# Tiny script that goes thru the .debug files, searching for crashes.
###

debug=".debug"
storage="/glftpd/ftp-data/pzs-ng/"

## END OF CONFIG

proc_chkdebug() {
 ignorepid=$(head -n 1 "$1" | cut -d' ' -f7)

 cnt=0
 lastpid=0

 grep -E "( - create_lock: lock set| - remove_lock: queue)" "$1" | cut -d' ' -f7 | sort | while read pid; do
  if [ "$pid" = "$ignorepid" ]; then continue; fi
  let "odd = $cnt % 2"
  if [ "$odd" = 1 ]; then
   if [ ! "$pid" = "$lastpid" ]; then
    echo "check pid $lastpid in $1"
    echo "listing last three lines below:"
    grep -e " $lastpid - " $1 | tail -n 3
    cnt=0
   fi
  fi
  let "cnt += 1"
  lastpid=$pid
 done
}

find $storage -name "$debug" | while read line; do
 proc_chkdebug "$line"
done

