#!/bin/bash

# small script to create a "full" zsconfig.

if [ -e zipscript/include/zsconfig.defaults.h ]; then
  cat zipscript/include/zsconfig.defaults.h | grep -e "^#define" -a -e "^ " -a -e "^/" -a -e "^	" >zipscript/conf/zsconfig.h.all
  echo "zipscript/conf/zsconfig.h.all created."
else
  echo "FAIL: could not find zipscript/include/zsconfig.defaults.h."
fi
exit 0

