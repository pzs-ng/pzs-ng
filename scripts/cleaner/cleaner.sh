#!/bin/bash

for file in * ; do
 if [ ! -e "$file" ] ; then
  echo "Removing $file..."
  rm -f "$file"
 fi
done
