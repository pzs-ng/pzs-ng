#!/bin/bash

# This thingy will make .diffs. It's what I use when I do.
# There is really no need for you to run this - it's just
# here to let people using diffs see how they're done.
# If my method could be improved, I'd like to know. I have
# no experience in making diffs like this.
# - psxc

AV="2.0i 2.0j 2.0k 2.1 2.2 2.2f 2.3 2.3f 2.4"
BV=2.5

# End of config
B=psxc-imdb_v$BV
if [ ! -e $B/diff ]; then
 mkdir $B/diff
fi
for E in $AV; do
 D="psxc-imdb.sh psxc-imdb.conf psxc-imdb-bot.tcl psxc-imdb-conf.tcl psxc-imdb-parser.sh psxc-imdb-sanity.sh psxc-imdb-rescan.sh psxc-imdb-find.sh psxc-imdb-nuker.sh psxc-symlink-maker.sh psxc-imdb-dotimdb.pl"
 A=psxc-imdb_v$E
 for C in $D; do
  AX=`find $A -name $C`
  BX=`find $B -name $C`
  if [ $AX ] && [ $BX ]; then
   diff -b -B -c -C 2 $AX $BX > $B/diff/$C.patch.$E.$BV
   if [ ! -s $B/diff/$C.patch.$E.$BV ]; then
    rm $B/diff/$C.patch.$E.$BV
   fi
  fi
 done
done

