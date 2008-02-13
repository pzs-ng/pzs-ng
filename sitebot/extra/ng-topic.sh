#!/bin/bash

gllog="/ftp-data/logs/glftpd.log"
channel=$1
topic=`echo $@ | sed "s/$1 //g"`
if [ "$channel" == "#chan" ]; then
    echo `date "+%a %b %e %T %Y"` SETTOPIC: \"$channel\" \"$topic\" >> $gllog
    echo "Sent topic ($topic) to $channel"
else
    echo "Usage: site topic <channel> <message>"
fi