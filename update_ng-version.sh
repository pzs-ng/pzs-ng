#!/bin/sh

# Simple script that updates the file FILE with revision-number
# Also prepends/appends STRING_PRE/POST. :)
STRING_PRE="0.7.7."
STRING_POST=" (pre-v1)"
FILE="src/zipscript/ng-version.h"
REVHIST="../revhistory.log"


REV=$1
if [ -z "$REV" -a -r "$REVHIST" ]; then
        REV=`head -2 ../revhistory.log | tail -1 | awk '{print $1}'`
fi
if [ -z "$REV" ]; then
	REV=`svn info | grep -e ^Revision: | awk '{printf $2}'`
fi

REV="`echo $REV|sed 's/[^0-9]\{1,\}//'`"

if [ ! -r "$FILE" ]; then
        echo " ERROR: Could not read version-file. ($FILE)"
        echo "syntax: $0 [revision]";
        exit 1;
fi

if [ -z "$REV" ]; then
        echo " ERROR: Invalid or missing revision parameter.";
        echo "syntax: $0 [revision]";
        exit 1
fi

cat > $FILE << EOF
#ifndef _NG_VERSION_H_
#define _NG_VERSION_H_

#define NG_VERSION "$STRING_PRE$REV$STRING_POST"

#endif /* _NG_VERSION_H_ */
EOF
