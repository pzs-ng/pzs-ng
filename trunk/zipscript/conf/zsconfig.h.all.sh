#!/bin/bash

# small script to create a "full" zsconfig.

if [ -e zipscript/include/zsconfig.defaults.h ]; then
  cat zipscript/include/zsconfig.defaults.h | grep -e "^#define" -a -e "^ " -a -e "^/" -a -e "^	" >zipscript/conf/zsconfig.h.all
  echo "zipscript/conf/zsconfig.h.all created."
elif [ -e ../include/zsconfig.defaults.h ]; then
  cat ../include/zsconfig.defaults.h | grep -e "^#define" -a -e "^ " -a -e "^/" -a -e "^ " >zsconfig.h.all
  echo "zsconfig.h.all created."
else
  echo "FAIL: could not find zsconfig.defaults.h."
fi
exit 0

