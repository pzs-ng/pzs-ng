#!/bin/bash

# This small thingy is useful when you wish to strip away blank lines
# and comments from a file. It takes one argument - the file you
# wish to strip/make clean. It will output the result to STDOUT

if [ ! -z "$1" ]; then
 echo "## stripping file: $1"
 echo "##"
 cat $1 | awk '/#!/ , /# [Ee][Nn][Dd] [Oo][Ff] [Cc][Oo][Nn][Ff][Ii][Gg]/' | grep -v "^#" | sed -e /^$/d
 if [ "$?" = "0" ]; then
 echo "##"
  echo "## command completed successfully."
 else
 echo "##"
  echo "## command failed."
 fi
else
 echo "## usage: $0 filename"
fi
exit 0
