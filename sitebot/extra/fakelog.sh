#!/bin/bash
#################################################################################
# Script made from scratch for use by project-zs-ng admins by themolester.		#
# however it will most likely be beneficial to people while modifying any     #
# sitebot.
# If you feel like improving this script feel free, but please send me/us back#
# a modified copy, or diffs so that everybody benefits						  #
# 																			  #
# www.glftpd.com															  #
# www.glftpd.at																  #
###############################################################################
#																			  #
#  ChangeLog																  #
#																			  #
###############################################################################
#																			  #
# Version 0.1: 																  #
#	Created script. then redesigned from scratch cutting filesize in half,    #
#   causing the script to execute much quicker (not that its not fast enough) #
#																			  #
###############################################################################
if [ ! -r $gllog ] ; then
	echo "error: can not read >$gllog<"
	die
fi
function gllog {
gldate=`date "+%a %b %e %T %Y"`
gllog=/glftpd/ftp-data/logs/glftpd.log
	echo "$gldate $*" >> $gllog
}
glprefix="/site/incoming/apps"
function request {
	what=${2:-"Some.Rls-GRP"}
	user=${3:-"g0d"}
	group=${4:-"SiTEOPS"}
	tagline=${5:-"3y3.0wn.j00"}
	echo "requesting >$what< for $user@$group-$tagline"
	gllog "REQUEST: \"$what\" \"$user\" \"$group\" \"$tagline\""
}
function wipe {
	what=${2:-"$glprefix/Some.Rls-GRP"}
	user=${3:-"g0d"}
	group=${4:-"SiTEOPS"}
	tagline=${5:-"3y3.0wn.j00"}
	echo "$user@$group-$tagline just fake wiped >$what<"
	gllog "WIPE: \"$what\" \"$user\" \"$group\" \"$tagline\""
}
function wipe-r {
	what=${2:-"$glprefix/Some.Rls-GRP"}
	user=${3:-"g0d"}
	group=${4:-"SiTEOPS"}
	tagline=${5:-"3y3.0wn.j00"}
	echo "$user@$group-$tagline just fake wiped >$what< and all subdirectories"
	gllog "$gldate WIPE-r: \"$what\" \"$user\" \"$group\" \"$tagline\""
}
function newdir {
	what=${2:-"$glprefix/Some.Rls-GRP"}
	user=${3:-"c00rry"}
	group=${4:-"iND"}
	tagline=${5:-"i.r.c00l"}
	echo "$user@$group-$tagline just fake created >$what<"
	gllog "NEWDIR: \"$what\" \"$user\" \"$group\" \"$tagline\""
}
function deldir {
	what=${2:-"$glprefix/Some.Rls-GRP"}
	user=${3:-"g0d"}
	group=${4:-"SiTEOPS"}
	tagline=${5:-"3y3.0wn.j00"}
	echo "$user@$group-$tagline just fake deleted >$what<"
	gllog "DELDIR: \"$what\" \"$user\" \"$group\" \"$tagline\""
}
function login { 
	host=${2:-"*@127.0.0.1"}
	ip=${3:-"127.0.0.1"}
	user=${4:-"c00rry"}
	group=${5:-"iND"}
	tagline=${6:-"i.r.c00l"}
	echo "$user@$group-$tagline just fake logged in from >$host< using >$ip<"
	gllog "LOGIN: \"$host\" \"$ip\" \"$user\" \"$group\" \"$tagline\""
}
function logout {
	host=${2:-"*@127.0.0.1"}
	ip=${3:-"127.0.0.1"}
	user=${4:-"c00rry"}
	group=${5:-"iND"}
	tagline=${6:-"i.r.c00l"}
	echo "$user@$group-$tagline just fake logged out from >$host< using >$ip<"
	gllog "LOGOUT: \"$host\" \"$ip\" \"$user\" \"$group\" \"$tagline\""
}
function invite {
	nick=$2
	user=${4:-"c00rry"}
	group=${5:-"iND"}
	tagline=${6:-"i.r.c00l"}
	echo "WARNING: due to the nature of the usage of this entry by"
	echo " most bots the username used will most likely be invited"
	echo " for real!! please use with caution."
	echo ""
	read -s -n1 -p "Are you sure you want to continue to \"fake\" invite $user into your sitechan and possibly do so for real? [y,N]" Keypress
	echo ""
	echo ""
	case $Keypress in
	[yY]      ) ## user told us yes so we will procede
		if [ $nick ]; then
		echo "$user@$group-$tagline just fake invited >$nick<"
		gllog "INVITE: \"$nick\" \"$user\" \"$group\" \"$tagline\""
		else
		echo "Due to the reasons in the previous warning the nick arg is NOT"
		echo " optional, and will not be defaulted, however this script did"
		echo " not recieve enough args to continue"
		exit 0
		fi
	;;

	*         ) ## [ ! $yes ] = no For safety
		echo "you did not respond yes and we have assumed that you meant no"
		exit 0
	;;
	esac
}
function pre {
	what=${2:-"$glprefix/Some.Rls-GRP"}
	user=${3:-"AFFiL"}
	group=${4:-"GRP"}
	tagline=${5:-"3y3.r.1337"}
	echo "$user@$group-$tagline just fake pred >$what<"
	gllog "PRE: \"$what\" \"$user\" \"$group\" \"$tagline\""
}
function nuke {
	what=${2:-"$glprefix/Some.Rls-GRP"}
	nuker=${3:-"g0d"}
	mult=${4:-"1"}
	reason=${5:-"j00 R n07 l337 nuFF"}
	nukees=${6:-"c00ry"}
	size=${7:-"10"}
	echo "$nuker pretended to condemn >$nukees< to a life of slavery to pay back  $size MB worth of credit for fucking up on >$what<"
	gllog "NUKE: \"$what\" \"$nuker\" \"$nukees\" \"$mult $size\" \"$reason\""
}
function unnuke {
	what=${2:-"$glprefix/Some.Rls-GRP"}
	nuker=${3:-"g0d"}
	mult=${4:-"1"}
	reason=${5:-"j00 R n07 l337 nuFF"}
	nukees=${6:-"c00ry"}
	size=${7:-"10"}
	echo "$nuker pretended to forgive >$nukees< for fucking up on >$what< and they are saved from a life of slavery to pay back $size MB worth of credit"
	gllog "UNNUKE: \"$what\" \"$nuker\" \"$nukees\" \"$mult $size\" \"$reason\""
}

case $1 in
  
	"help"    ) 
		echo "This script was designed to help admins test various scripts which
  may monitor the logfiles of a popular ftp daemon. This will NOT effect the
  actual ftp root, or any non log file. However, it MAY trigger some scripts to
  run on your site, which are expecting filesystem changes, or that you 
  otherwise dont want to run. For example, the invite arg. This is the only log
  entry i could think of that would cause excessive breaking (into?). It is for
  this reason a bit more difficult to run, but all should run without a hitch.
  
syntax:
  note: [args] are assumed in order listed (should be fixed next revision)
  
  ./fakelog <option> [args]
  ./fakelog request  [what]  [user]  [group]      [tagline]
  ./fakelog wipe     [what]  [user]  [group]      [tagline]
  ./fakelog wipe-r   [what]  [user]  [group]      [tagline]
  ./fakelog newdir   [what]  [user]  [group]      [tagline]
  ./fakelog deldir   [what]  [user]  [group]      [tagline]
  ./fakelog login    [host]  [ip]    [user]       [group]    [tagline]
  ./fakelog logout   [host]  [ip]    [user]       [group]    [tagline]
  ./fakelog invite   <nick>  [user]  [group]      [tagline]
  ./fakelog pre      [what]  [user]  [group]      [tagline]
  ./fakelog nuke     [what]  [nuker] [multiplier] [reason]   [nukees]  [size]
  ./fakelog unnuke   [what]  [nuker] [multiplier] [reason]   [nukees]  [size]
"
	;;
  
	"request"	) request "$@" ;;
	"wipe"		) wipe "$@" ;;
	"wipe-r"	) wipe-r "$@" ;;
	"newdir"	) newdir "$@" ;;
	"deldir"	) deldir "$@" ;;
	"login"		) login "$@" ;;
	"logout"	) logout "$@" ;;
	"invite"	) invite "$@" ;;
	"pre"		) pre "$@" ;;
	"nuke"		) nuke "$@" ;;
	"unnuke"	) unnuke "$@" ;;
	"all"		) 
		request "$@"
		wipe "$@"
		wipe-r "$@"
		newdir "$@"
		deldir "$@"
		login "$@"
		logout "$@"
		pre "$@"
		nuke "$@"
		unnuke "$@"
	;;
	* ) echo "try ./fakelog help" ;;
esac
