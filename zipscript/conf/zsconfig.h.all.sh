#!/bin/bash

# small script to create a "full" zsconfig.

if [ -e zipscript/include/zsconfig.defaults.h ]; then
  conf=zipscript/include/zsconfig.defaults.h
  dest=zipscript/conf/zsconfig.h.all
elif  [ -e ../include/zsconfig.defaults.h ]; then
  conf=../include/zsconfig.defaults.h
  dest=zsconfig.h.all
else
  echo "FAIL: could not find zsconfig.defaults.h."
  exit 1
fi
echo "Creating config file ... this may take a few seconds.. bash is not lighning fast ;P"
IFS_ORG="$IFS"
IFS="
"
start=0;
echo -n "" >$dest
for line in `cat $conf | grep -v "^#ifndef" | grep -v "^#endif" | tr '/' '\^'`; do
  if [[ $start -eq 0 ]]; then
    [[ `echo "$line" | grep -e ":START:"` ]] && start=1 && continue;
  else
    [[ `echo "$line" | grep -e ":END:"` ]] && start=0 && continue;
  fi
  [[ $start -eq 1 ]] && echo $line | tr '\^' '/' >>$dest
done
echo "$dest created."
exit 0

