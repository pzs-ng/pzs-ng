#!/bin/bash

# Simple script to delete old cruft in the zipscript dir. This should be done
# on a regular basis, else you end up having too many files for the disk/fs
# to handle, and lots of space is lost.

zipscriptdir=/glftpd/ftp-data/zipscript
days2keep=30

# End of config
#################

find ${zipscriptdir}/ -type d ! -ctime -${days2keep} -exec rm -fR {} ";" >/dev/null 2>&1

