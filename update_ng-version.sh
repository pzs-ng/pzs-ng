#!/bin/sh

# Simple script that updates the file FILE with revision-number
# Also prepends/appends STRING_PRE/POST. :)
STRING_PRE="0.7.3 (r"
STRING_POST=", pre beta5)"
FILE="zipscript/src/ng-version.c"

if [ ! -r "$FILE" ]; then
	echo " ERROR: Could not read version-file. ($FILE)"
	echo "syntax: $0 <revision>";
	exit 1;
fi

REV="`echo $1|sed 's/[^0-9]+//g'`"
if [ -z "$1" ]; then
	echo " ERROR: Invalid or missing revision parameter.";
	echo "syntax: $0 <revision>";
	exit 1
fi
echo '' > $FILE
echo -n 'const char* ng_version(void) { const char* NG_Version = "' >> $FILE
echo -n "$STRING_PRE$REV$STRING_POST" >> $FILE
echo '"; return NG_Version; }' >> $FILE
echo '' >> $FILE
