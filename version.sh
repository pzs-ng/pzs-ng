#/bin/sh
# extract abbreviated hash from master
if [ -f .git/refs/heads/master ]; then
  GIT_HASH=$(cut -c"1-8" .git/refs/heads/master 2>&1 | tee)
else GIT_HASH="svn-checkout"
fi
cat << EOF > zipscript/src/ng-version.c
#include "ng-version.h"

/*** 
 * DO NOT CHANGE THIS FILE ON SVN PRIOR TO RELEASE!
 * The bot (pzs-ng) takes care of this when you do something like:
 *  <@psxc> pzs-ng: release 2291 stable v1.0.9
 * So just leave this, so that you can identify a svn revision versus a normal release. :-)
 ***/

const char* ng_version = "${GIT_HASH}";
EOF
