#!/bin/sh

log="/ftp-data/logs/glftpd.log"

if [ -z "$1" ] || [ ! -z "$2" ] ; then
 printf "\nUsage: SITE INVITE <irc-nick>\n\n"
 exit 0
fi

badchars=$(printf "%s" "$1" | tr -d "[a-z][A-Z][0-9]|\-_^][")
if [ ! -z "$badchars" ]; then
 printf "\nERROR - Invalid characters in nick: %s\n\n" "$badchars"
 exit 0
fi

printf "%s INVITE: \"%s\" \"%s\" \"%s\" \"%s\" \"%s\"\n" "$(/bin/date '+%a %b %d %T %Y')" "$1" "$USER" "$GROUP" "$FLAGS" "$TAGLINE" >> "$log"
printf "\nInvited %s with nick %s\n\n" "$USER" "$1"
