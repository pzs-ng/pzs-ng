#!/bin/sh

# Simple script that updates the file FILE with revision-number
# Also prepends/appends STRING_PRE/POST. :)
STRING_PRE="0.7.7."
STRING_POST=" (pre-v1)"
FILE="zipscript/src/ng-version.c"
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

echo '#include "ng-version.h"' > $FILE
echo '' >> $FILE
echo -n 'const char* ng_version(void) { const char* NG_Version = "' >> $FILE
echo -n "$STRING_PRE$REV$STRING_POST" >> $FILE
echo '"; return NG_Version; }' >> $FILE
