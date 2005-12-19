#!/bin/bash

IFS_ORIG="$IFS"
IFS=$'\n'

start=0
for line in `cat zipscript/include/zsconfig.defaults.h | sed "s|^/\*|/:|g" | sed "s|^ \*| :|g"` ; do
  if [[ $start -eq 0 ]]; then
    [[ `echo "$line" | grep -e ":START:"` ]] && start=1;
  else
    [[ `echo "$line" | grep -e ":END:"` ]] && start=0;
    if [[ `echo "$line" | grep -e "^#define"` ]]; then
      setting="`echo "$line" | tr '\t' ' ' | tr -s ' ' | cut -d ' ' -f 3-`"
      echo -e "\tDEFAULT: $setting\n"
    else
      echo "$line" | grep -v "^/:" | grep -v "^ :/" | grep -v "^#ifndef" | grep -v "^#endif" | sed "s|^ : ||g" | sed "s|^ :||g"
    fi
  fi
done
IFS=$IFS_ORIG

