#!/bin/bash

extract() {
 if [ -z "$(ls -1 2>/dev/null | grep -i "$1")" ] ; then
#  nfo=$(unzip -Clqq "$zipfile" "$2" 2>/dev/null | head -n 1 | awk '{print $4}')
#  nfo=$(unzip -Clqq "$zipfile" "$2" 2>/dev/null | head -n 1 | cut -c29-)
  unzip -Clqq "$zipfile" "$2" 2>/dev/null | {
   read _Length _Date _Time file
   if [ "$file" ]; then
    newfile=$(echo ${file##*/} | tr 'A-Z' 'a-z')
    unzip -qqp "$zipfile" "$file" > "$newfile"
    chmod 644 "$newfile"
   fi
  }
 fi
}

if [ -r "$1" ]; then
 zipfile=$1
 extract "\.nfo$" "*.nfo"
 extract "^file_id.diz$" "file_id.diz"
fi
