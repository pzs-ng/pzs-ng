#!/bin/sh
$1 -u $2 -p $3 -r0 -P $4 -t$5 ftp://$6 2> /dev/null > /dev/null
if [ $? -eq 0 ] ; then
 echo "UP"
else
 echo "DOWN"
fi

exit 0
