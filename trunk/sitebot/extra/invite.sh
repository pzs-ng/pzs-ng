#!/bin/sh

log="/ftp-data/logs/glftpd.log"

if [ -z "$1" ] || [ ! -z "$2" ] ; then
 echo -e "\nUsage: SITE INVITE <irc-nick>\n"
 exit 0
fi

badchars=`echo -n "$1" | tr -d "[a-z][A-Z][0-9]|\-_^]["`
if [ ! -z "$badchars" ]; then
 echo -e "\nERROR - Invalid characters in nick: $badchars\n"
 exit 0
fi

echo `/bin/date '+%a %b %d %X %Y'` INVITE: \"$1\" \"$USER\" \"$GROUP\" \"$FLAGS\" >> "$log"
echo -e "\nInvited $USER with nick $1 \n"
