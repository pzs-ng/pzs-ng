#!/bin/bash
#################################################################################
# Script made from scratch for use by project-zs-ng admins by themolester.	#
# however it will most likely be beneficial to people while modifying any	#
# sitebot.									#
# If you feel like improving this script feel free, but please send me/us back	#
# a modified copy, or diffs so that everybody benefits				#
#										#
# www.glftpd.com								#
# www.glftpd.at									#
#################################################################################
#										#
#  ChangeLog									#
#										#
#################################################################################
#										#
#										#
# Version 0.1:									#
#	Created script. then redesigned from scratch cutting filesize in half,	#
#   causing the script to execute much quicker (not that its not fast enough)	#
#										#
# Version 0.2
#   2004-08-16: Changed default path and texts.
#               Added reqfilled (avizion)
#################################################################################

usr="TestUser"
grp="Siteops"
tag="Default Tagline"
rls="Some.Release-GRP"

glprefix="/site/test"

if [ ! -r $gllog ] ; then
	echo "error: can not read >$gllog<"
	die
fi

function gllog {
	gldate=`date "+%a %b %e %T %Y"`
	gllog=/glftpd/ftp-data/logs/glftpd.log
	echo "$gldate $*" >> $gllog
}


function request {
	what=${2:-$rls}
	user=${3:-$usr}
	group=${4:-$grp}
	tagline=${5:-$tag}
	echo  "Test REQUEST: $what for $user of $group ($tagline)"
	gllog "REQUEST: \"$what\" \"$user\" \"$group\" \"$tagline\""
}

function reqfilled {
	what=${2:-$rls}
	user=${3:-"pzs-ng"}
	group=${4:-"System"}
	tagline=${5:-$tag}
	who=${6:-$usr}	
	echo  "Test REQFILLED: $what by $user of $group for $who ($tagline)"
	gllog "REQFILLED: \"$what\" \"$user\" \"$group\" \"$tagline\" \"$who\""
}

function wipe {
	what=${2:-"$glprefix/$rls"}
	user=${3:-$usr}
	group=${4:-$grp}
	tagline=${5:-$tag}
	echo  "Test WIPE: $user of $group ($tagline) just fake wiped $what"
	gllog "WIPE: \"$what\" \"$user\" \"$group\" \"$tagline\""
}

function wipe-r {
	what=${2:-"$glprefix/$rls"}
	user=${3:-$usr}
	group=${4:-$grp}
	tagline=${5:-$tag}
	echo  "$user@$group-$tagline just fake wiped >$what< and all subdirectories"
	gllog "WIPE-r: \"$what\" \"$user\" \"$group\" \"$tagline\""
}
function newdir {
	what=${2:-"$glprefix/$rls"}
	user=${3:-$usr}
	group=${4:-$grp}
	tagline=${5:-$tag}
	echo "$user@$group-$tagline just fake created >$what<"
	gllog "NEWDIR: \"$what\" \"$user\" \"$group\" \"$tagline\""
}
function deldir {
	what=${2:-"$glprefix/$rls"}
	user=${3:-$usr}
	group=${4:-$grp}
	tagline=${5:-$tag}
	echo "$user@$group-$tagline just fake deleted >$what<"
	gllog "DELDIR: \"$what\" \"$user\" \"$group\" \"$tagline\""
}
function login { 
	host=${2:-"*@127.0.0.1"}
	ip=${3:-"127.0.0.1"}
	user=${3:-$usr}
	group=${4:-$grp}
	tagline=${6:-$tag}
	echo "$user@$group-$tagline just fake logged in from >$host< using >$ip<"
	gllog "LOGIN: \"$host\" \"$ip\" \"$user\" \"$group\" \"$tagline\""
}
function logout {
	host=${2:-"*@127.0.0.1"}
	ip=${3:-"127.0.0.1"}
	user=${3:-$usr}
	group=${4:-$grp}
	tagline=${6:-$tag}
	echo "$user@$group-$tagline just fake logged out from >$host< using >$ip<"
	gllog "LOGOUT: \"$host\" \"$ip\" \"$user\" \"$group\" \"$tagline\""
}
function invite {
	nick=$2
	user=${4:-$usr}
	group=${5:-$grp}
	tagline=${6:-$tag}
	echo "WARNING: due to the nature of the usage of this entry by"
	echo " most bots the username used will most likely be invited"
	echo " for real!! please use with caution."
	echo ""
	read -s -n1 -p "Are you sure you want to continue to \"fake\" invite $nick into your sitechan and possibly do so for real? [y,N]" Keypress
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
	what=${2:-"$glprefix/$rls"}
	pregrp=${3:-"AFFiL"}
	section=${4:-"TEST"}
	files=${5:-"13"}
	size=${6:-"34.2"}
	echo "Test PRE: $pregrp just fake pred $what/$rel"
	gllog "PRE: \"$what\" \"$rls\" \"$pregrp\" \"$section\" $files $size 1 \"\" \"\" \"\" \"\""
}

function nuke {
	what=${2:-"$glprefix/$rls"}
	nuker=${3:-$usr}
	mult=${4:-"3"}
	reason=${5:-"watch and learn"}
	nukees=${6:-"Nukee"}
	size=${7:-"10"}
	echo "$nuker pretended to condemn >$nukees< to a life of slavery to pay back $size MB worth of credit for fucking up on >$what<"
	gllog "NUKE: \"$what\" \"$nuker\" \"$nukees\" \"$mult $size\" \"$reason\""
}

function unnuke {
	what=${2:-"$glprefix/$rls"}
	nuker=${3:-$usr}
	mult=${4:-"1"}
	reason=${5:-"watch and learn"}
	nukees=${6:-"Nuk33"}
	size=${7:-"10"}
	echo "$nuker pretended to forgive >$nukees< for fucking up on >$what< and they are saved from a life of slavery to pay back $size MB worth of credit"
	gllog "UNNUKE: \"$what\" \"$nuker\" \"$nukees\" \"$mult $size\" \"$reason\""
}

function sfv {
	what=${2:-"$glprefix/$rls/CD1"}
	echo "Test SFV"
	gllog "SFV: \"$what\" \"$rls/CD1\" \"50\""
}

function update_rar {
	what=${2:-"$glprefix/$rls"}
	echo "Test UPDATE_RAR"
	gllog "UPDATE_RAR: \"$what\" user group 715.2"
}

function update_other {
	what=${2:-"$glprefix/$rls"}
	echo "Test UPDATE_OTHER"
	gllog "UPDATE_OTHER: \"$what\" user group 715.2"
}

function update_audio {
	what=${2:-"$glprefix/$rls"}
	echo "Test UPDATE_AUDIO"
	gllog "UPDATE_AUDIO: \"$what\" avizion Siteops Dance 2004 232 44100 \"Joint Stereo\" \"VBR APS\""
}

function update_video {
	what=${2:-"$glprefix/$rls"}
	echo "Test UPDATE_VIDEO"
	gllog "UPDATE_VIDEO: \"$what\" user group 715.2"
}

function complete {
	what=${2:-"$glprefix/$rls"}
	echo "Test COMPLETE"
	gllog "COMPLETE: \"$what\" $rls 2 1 522 3s 1 1 avizion Siteops 439 avizion Siteops 439"
}

function incomplete {
	what=${2:-"$glprefix/$rls"}
	echo "Test INCOMPLETE"
	gllog "INCOMPLETE: \"$what\" $usr $grp $rls"
}

function stat_users_head {
	what=${2:-"$glprefix/$rls"}
	echo "Test STAT_USERS_HEAD"
	gllog "STAT_USERS_HEAD: \"$what\" dummy"
}

function stat_users {
	what=${2:-"$glprefix/$rls"}
	position=${3:-"1"}
	user=${4:-$usr}
	group=${5:-$grp}
	mbytes=${6:-"45.6"}
	files=${7:-"3"}
	percent=${8:-"100.0"}
	speed=${9:-"4529"}
	echo "Test STAT_USERS"
	gllog "STAT_USERS: \"$what\" $position \"$user\" \"$group\" $mbytes $files $percent $speed"
}

function stat_groups_head {
	what=${2:-"$glprefix/Some-Rls-GRP"}
	echo "Test STAT_GROUPS_HEAD"
	gllog "STAT_GROUPS_HEAD: \"$what\" dummy"
}

function stat_groups {
	what=${2:-"$glprefix/Some-Rls-GRP"}
	echo "Test STAT_GROUPS"
	gllog "STAT_GROUPS: \"$what\" 1 \"Siteops  \" 1.5 1 100.0 439"
}

function stat_post {
	what=${2:-"$glprefix/Some-Rls-GRP"}
	echo "Test STAT_POST"
	gllog "STAT_POST: \"$what\" dummy"
}

function race {
	what=${2:-"$glprefix/Some-Rls-GRP"}
	echo "Test RACE"
	gllog "RACE: \"$what\" avizion Siteops "opponent1" Some-Rls-GRP 439"
}

function newleader {
	what=${2:-"$glprefix/Some-Rls-GRP"}
	echo "Test NEWLEADER"
	gllog "NEWLEADER: \"$what\" \"avizion  \" \"Siteops  \" 42.9 3 75.0 6234 Some-Rls-GRP"
}

function halfway {
	what=${2:-"$glprefix/Some-Rls-GRP"}
	echo "Test HALFWAY"
	gllog "HALFWAY: \"$what\" Some-Rls-GRP \"avizion  \" \"Siteops  \" 42.9 3 75.0 634 \"Siteops  \" 93.4 \"16\" 59.3 34"
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
  
  ./fakelog <option>	[args]
  ./fakelog request	[what]	[user]		[group]		[tagline]
  ./fakelog wipe	[what]	[user]		[group]		[tagline]
  ./fakelog wipe-r	[what]	[user]		[group]		[tagline]
  ./fakelog newdir	[what]	[user]		[group]		[tagline]
  ./fakelog deldir	[what]	[user]		[group]		[tagline]
  ./fakelog login	[host]	[ip]		[user]		[group]		[tagline]
  ./fakelog logout	[host]	[ip]		[user]		[group]		[tagline]
  ./fakelog invite	<nick>	[user]		[group]		[tagline]
  ./fakelog pre		[what]	[user]		[group]		[tagline]
  ./fakelog nuke	[what]	[nuker]		[multiplier]	[reason]	[nukees]	[size]
  ./fakelog unnuke	[what]	[nuker]		[multiplier]	[reason]	[nukees]	[size]
  ./fakelog stat_users	[what]	[position]	[user]		[group]		[mbytes]	[files]	[percent]	[speed]
"
	;;
  
	"request"		) request "$@" ;;
	"reqfilled"		) reqfilled "$@" ;;
	"wipe"			) wipe "$@" ;;
	"wipe-r"		) wipe-r "$@" ;;
	"newdir"		) newdir "$@" ;;
	"deldir"		) deldir "$@" ;;
	"login"			) login "$@" ;;
	"logout"		) logout "$@" ;;
	"invite"		) invite "$@" ;;
	"pre"			) pre "$@" ;;
	"nuke"			) nuke "$@" ;;
	"unnuke"		) unnuke "$@" ;;
	"sfv"			) sfv "$@" ;;
	"update_rar"		) update_rar "$@" ;;
	"update_other"		) update_other "$@" ;;
	"update_audio"		) update_audio "$@" ;;
	"update_video"		) update_video "$@" ;;
	"complete"		) complete "$@" ;;
	"incomplete"		) incomplete "$@" ;;
	"race"			) race "$@" ;;
	"newleader"		) newleader "$@" ;;
	"halfway"		) halfway "$@" ;;
	"stat_users_head"	) stat_users_head "$@" ;;
	"stat_users"		) stat_users "$@" ;;
	"stat_groups_head"	) stat_groups_head "$@" ;;
	"stat_groups"		) stat_groups "$@" ;;
	"stat_post"		) stat_post "$@" ;;

	"stat"		)
		stat_users_head "$@"
		stat_users "$@"
		stat_groups_head "$@"
		stat_groups "$@"
		stat_post "$@"
	;;
	
	"update"	)
		update_rar "$@"
		update_other "$@"
		update_audio "$@"
		update_video "$@"
	;;

	"all"		) 
		request "$@"
		reqfilled "$@"
		wipe "$@"
		wipe-r "$@"
		newdir "$@"
		deldir "$@"
		login "$@"
		logout "$@"
		pre "$@"
		nuke "$@"
		unnuke "$@"
		sfv "$@"
		update "$@"
		complete "$@"
		incomplete "$@"
		race "$@"
		newleader "$@"
		halfway "$@"
		stat_uhead "$@"
		stat_users "$@"
		stat_ghead "$@"
		stat_group "$@"

	;;
	* ) echo "try ./fakelog help" ;;
esac
