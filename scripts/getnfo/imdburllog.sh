#!/bin/bash

log="/ftp-data/logs/imdburl.log"

date=`date +"%a %b %d %H:%M:%S %Y"`
fname="$@"

case $fname in
 *.[nN][fF][oO])
  link=`grep -i imdb $fname | tr ' ' '\n' | sed -n /[hH][tT][tT][pP]:[/][/].*[.][iI][mM][dD][bB].*?.[0-9]/p | head -n 1`
  if [ ! -z "$link" ] && [ -e "$log" ] ; then
   echo "IMDB: \"$PWD\" OOOO \"$link\"" >> $log
  fi
  link=`grep -i allocine $fname | tr ' ' '\n' | sed -n /[hH][tT][tT][pP]:[/][/].*[.][aA][lL][lL][oO][cC][iI][nN][eE].*/p | head -n 1`
  if [ ! -z "$link" ] && [ -e "$log" ] ; then
   echo "$date ALLOCINE: \"$PWD\" \"$link\"" >> $log
  fi
 ;;
esac
