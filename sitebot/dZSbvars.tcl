##########################################################################
#                                                                        #
# Default variable-list for announces done by glftpd and pzs-ng          #
# This list should probably not be edited.                               #
#                                                                        #
##########################################################################

## Defining variables for announce
#
# Example:
#  set variables(PRE) "%pf %user %group %pregroup %files %mbytes"
#  set announce(PRE)  "-%sitename- \[%section\] %user@%group launches new %pregroup-pre called %release (%mbytesM in %filesF)"
#
# Special variables:
#  %pf       = path filter, must be the first parameter and contain full path of the release, it defines:
#   %reldir  = Last directory in path ( /site/xxx/marissa-exxtasy/cd1 => cd1 )
#   %path    = Second last directory in path ( /site/xxx/marissa-exxtasy/cd1 => marissa-exxtasy )
#   %relname = all directories after those defined in paths
#              ( paths(ISO) = "/site/xxx/" does: /site/xxx/marissa-exxtasy/cd1 => marissa-exxtasy/cd1 )
#
#  %bold    = bolds text
#  %uline   = underlines text
#  %section = current section name
#
#  %nuker   = name of nuker
#  %multiplier = nuke multiplier
#  %reason  = nuke reason
#  %nukee   = ppl that got nuked
#
# PLEASE! DO NOT CHANGE THESE VALUES IF YOU DO NOT KNOW WHAT THEY DO! IT *WILL* BREAK THE BOT!

## Read as:
# %t_ = Total, %t_files: Total files.
# %u_ = User, %u_count: User count.
# %g_ = Group, %g_name: Group name.

# Variables pasted from glftpd
set variables(DEBUG)			"%msg %u_pid"
set variables(DEFAULT)			"%pf %msg"
set variables(NEWDIR)			"%pf %u_name %g_name %u_tagline"
set variables(DELDIR)			"%pf %u_name %g_name %u_tagline"
set variables(LOGIN)			"%u_hostmask %u_ip %dummy %u_name %g_name %u_tagline %u_pid"
set variables(LOGOUT)			"%u_hostmask %u_ip %u_name %g_name %u_tagline"
set variables(TIMEOUT)			"%u_name %u_hostmask %dummy1 %dummy2 %dummy3 %dummy4 %dummy5 %u_idletime %dummy6"
set variables(TAGLINE)			"%u_name %u_group %u_tagline"
set variables(WIPE)			"%pf %u_name %g_name %u_tagline"
set variables(WIPE-r)			"%pf %u_name %g_name %u_tagline"
set variables(FAILLOGIN)		"%failed_nick %u_hostmask %u_ip %word1 %word2 %u_pid"
set variables(IPNOTADDED)		"%u_hostmask %u_ip"
set variables(BADUSERHOST)		"%u_name %u_hostmask %u_ip"
set variables(BADPASSWD)		"%u_name %u_hostmask %u_ip"

# Variables pasted from the zipscript
set variables(DOUBLESFV)		"%pf %u_name %g_name %releasename %filename"
set variables(BADBITRATE)		"%pf %u_name %g_name %bitrate"
set variables(BADGENRE)			"%pf %u_name %g_name %genre"
set variables(BADYEAR)			"%pf %u_name %g_name %year"
set variables(RACE_RAR)			"%pf %u_name %g_name {%a_name %a_gname} %releasename %u_speed %filename %t_duration %t_percent %u_count %g_count %t_upfiles %t_files %missing_files %u_tagline {%r_name %r_gname} %compression"
set variables(RACE_ZIP)			"%pf %u_name %g_name {%a_name %a_gname} %releasename %u_speed %filename %t_duration %t_percent %u_count %g_count %t_upfiles %t_files %missing_files %u_tagline {%r_name %r_gname}"
set variables(RACE_OTHER)		"%pf %u_name %g_name {%a_name %a_gname} %releasename %u_speed %filename %t_duration %t_percent %u_count %g_count %t_upfiles %t_files %missing_files %u_tagline {%r_name %r_gname}"
set variables(RACE_VIDEO)		"%pf %u_name %g_name {%a_name %a_gname} %releasename %u_speed %filename %t_duration %t_percent %u_count %g_count %t_upfiles %t_files %missing_files %u_tagline {%r_name %r_gname}"
set variables(RACE_AUDIO)		"%pf %u_name %g_name {%a_name %a_gname} %releasename %u_speed %filename %t_duration %t_percent %u_count %g_count %t_upfiles %t_files %missing_files %u_tagline {%r_name %r_gname}"
set variables(SFV_RAR)			"%pf %releasename %t_filecount %u_name %g_name %u_tagline"
set variables(SFV_OTHER)		"%pf %releasename %t_filecount %u_name %g_name %u_tagline"
set variables(SFV_VIDEO)		"%pf %releasename %t_filecount %u_name %g_name %u_tagline"
set variables(SFV_AUDIO)		"%pf %releasename %t_filecount %u_name %g_name %u_tagline"
set variables(UPDATE_RAR)		"%pf %u_name %g_name %t_files %u_speed %t_mbytes %filename %releasename %u_tagline %compression"
set variables(UPDATE_ZIP)		"%pf %u_name %g_name %t_files %u_speed %t_mbytes %filename %releasename %u_tagline"
set variables(UPDATE_OTHER)		"%pf %u_name %g_name %t_files %u_speed %t_mbytes %filename %releasename %u_tagline"
set variables(UPDATE_VIDEO)		"%pf %u_name %g_name %t_files %u_speed %t_mbytes %filename %releasename %u_tagline"
set variables(UPDATE_VBR)		"%pf %u_name %g_name %t_files %u_speed %t_mbytes %filename %releasename %u_tagline %genre %year %bitrate %sampling %mode %audio %a_stream %a_version %artist %album %title %codec %layer"
set variables(UPDATE_CBR)		"%pf %u_name %g_name %t_files %u_speed %t_mbytes %filename %releasename %u_tagline %genre %year %bitrate %sampling %mode %audio %artist %album %title %codec %layer"
set variables(HALFWAY_RACE_RAR)		"%pf %releasename %u_leader_name %u_leader_gname %u_leader_mbytes %u_leader_files %u_leader_percent %u_leader_avgspeed %g_leader_name %g_leader_mbytes %g_leader_files %g_leader_percent %g_leader_avgspeed %t_mbytes %t_files %t_percent %t_avgspeed %r_avgspeed %t_missingfiles %filename %u_count %g_count %u_name %g_name %t_total_files {%r_name %r_gname} %compression"
set variables(HALFWAY_RACE_ZIP)		"%pf %releasename %u_leader_name %u_leader_gname %u_leader_mbytes %u_leader_files %u_leader_percent %u_leader_avgspeed %g_leader_name %g_leader_mbytes %g_leader_files %g_leader_percent %g_leader_avgspeed %t_mbytes %t_files %t_percent %t_avgspeed %r_avgspeed %t_missingfiles %filename %u_count %g_count %u_name %g_name %t_total_files {%r_name %r_gname}"
set variables(HALFWAY_RACE_OTHER)	"%pf %releasename %u_leader_name %u_leader_gname %u_leader_mbytes %u_leader_files %u_leader_percent %u_leader_avgspeed %g_leader_name %g_leader_mbytes %g_leader_files %g_leader_percent %g_leader_avgspeed %t_mbytes %t_files %t_percent %t_avgspeed %r_avgspeed %t_missingfiles %filename %u_count %g_count %u_name %g_name %t_total_files {%r_name %r_gname}"
set variables(HALFWAY_RACE_VIDEO)	"%pf %releasename %u_leader_name %u_leader_gname %u_leader_mbytes %u_leader_files %u_leader_percent %u_leader_avgspeed %g_leader_name %g_leader_mbytes %g_leader_files %g_leader_percent %g_leader_avgspeed %t_mbytes %t_files %t_percent %t_avgspeed %r_avgspeed %t_missingfiles %filename %u_count %g_count %u_name %g_name %t_total_files {%r_name %r_gname}"
set variables(HALFWAY_RACE_AUDIO)	"%pf %releasename %u_leader_name %u_leader_gname %u_leader_mbytes %u_leader_files %u_leader_percent %u_leader_avgspeed %g_leader_name %g_leader_mbytes %g_leader_files %g_leader_percent %g_leader_avgspeed %t_mbytes %t_files %t_percent %t_avgspeed %r_avgspeed %t_missingfiles %filename %u_count %g_count %u_name %g_name %t_total_files {%r_name %r_gname}"
set variables(HALFWAY_NORACE_RAR)	"%pf %releasename %u_leader_name %u_leader_gname %u_leader_mbytes %u_leader_files %u_leader_percent %u_leader_avgspeed %g_leader_name %g_leader_mbytes %g_leader_files %g_leader_percent %g_leader_avgspeed %t_mbytes %t_files %t_percent %t_avgspeed %r_avgspeed %t_missingfiles %filename %u_count %g_count %u_name %g_name %t_total_files %u_tagline %compression"
set variables(HALFWAY_NORACE_ZIP)	"%pf %releasename %u_leader_name %u_leader_gname %u_leader_mbytes %u_leader_files %u_leader_percent %u_leader_avgspeed %g_leader_name %g_leader_mbytes %g_leader_files %g_leader_percent %g_leader_avgspeed %t_mbytes %t_files %t_percent %t_avgspeed %r_avgspeed %t_missingfiles %filename %u_count %g_count %u_name %g_name %t_total_files %u_tagline"
set variables(HALFWAY_NORACE_OTHER)	"%pf %releasename %u_leader_name %u_leader_gname %u_leader_mbytes %u_leader_files %u_leader_percent %u_leader_avgspeed %g_leader_name %g_leader_mbytes %g_leader_files %g_leader_percent %g_leader_avgspeed %t_mbytes %t_files %t_percent %t_avgspeed %r_avgspeed %t_missingfiles %filename %u_count %g_count %u_name %g_name %t_total_files %u_tagline"
set variables(HALFWAY_NORACE_VIDEO)	"%pf %releasename %u_leader_name %u_leader_gname %u_leader_mbytes %u_leader_files %u_leader_percent %u_leader_avgspeed %g_leader_name %g_leader_mbytes %g_leader_files %g_leader_percent %g_leader_avgspeed %t_mbytes %t_files %t_percent %t_avgspeed %r_avgspeed %t_missingfiles %filename %u_count %g_count %u_name %g_name %t_total_files %u_tagline"
set variables(HALFWAY_NORACE_AUDIO)	"%pf %releasename %u_leader_name %u_leader_gname %u_leader_mbytes %u_leader_files %u_leader_percent %u_leader_avgspeed %g_leader_name %g_leader_mbytes %g_leader_files %g_leader_percent %g_leader_avgspeed %t_mbytes %t_files %t_percent %t_avgspeed %r_avgspeed %t_missingfiles %filename %u_count %g_count %u_name %g_name %t_total_files %u_tagline"
set variables(NEWLEADER_RAR)		"%pf %u_name %g_name %u_speed %t_duration %uploaded_files %uploaded_percent %uploaded_mbytes %releasename %filename %u_count %g_count %t_files %t_missingfiles %u_leader_name %u_leader_gname %u_leader_mbytes %u_leader_files %u_leader_percent %u_leader_avgspeed %g_leader_name %g_leader_mbytes %g_leader_files %g_leader_percent %g_leader_avgspeed %u_tagline {%a_name %a_gname} %compression"
set variables(NEWLEADER_ZIP)		"%pf %u_name %g_name %u_speed %t_duration %uploaded_files %uploaded_percent %uploaded_mbytes %releasename %filename %u_count %g_count %t_files %t_missingfiles %u_leader_name %u_leader_gname %u_leader_mbytes %u_leader_files %u_leader_percent %u_leader_avgspeed %g_leader_name %g_leader_mbytes %g_leader_files %g_leader_percent %g_leader_avgspeed %u_tagline {%a_name %a_gname}"
set variables(NEWLEADER_OTHER)		"%pf %u_name %g_name %u_speed %t_duration %uploaded_files %uploaded_percent %uploaded_mbytes %releasename %filename %u_count %g_count %t_files %t_missingfiles %u_leader_name %u_leader_gname %u_leader_mbytes %u_leader_files %u_leader_percent %u_leader_avgspeed %g_leader_name %g_leader_mbytes %g_leader_files %g_leader_percent %g_leader_avgspeed %u_tagline {%a_name %a_gname}"
set variables(NEWLEADER_VIDEO)		"%pf %u_name %g_name %u_speed %t_duration %uploaded_files %uploaded_percent %uploaded_mbytes %releasename %filename %u_count %g_count %t_files %t_missingfiles %u_leader_name %u_leader_gname %u_leader_mbytes %u_leader_files %u_leader_percent %u_leader_avgspeed %g_leader_name %g_leader_mbytes %g_leader_files %g_leader_percent %g_leader_avgspeed %u_tagline {%a_name %a_gname}"
set variables(NEWLEADER_AUDIO)		"%pf %u_name %g_name %u_speed %t_duration %uploaded_files %uploaded_percent %uploaded_mbytes %releasename %filename %u_count %g_count %t_files %t_missingfiles %u_leader_name %u_leader_gname %u_leader_mbytes %u_leader_files %u_leader_percent %u_leader_avgspeed %g_leader_name %g_leader_mbytes %g_leader_files %g_leader_percent %g_leader_avgspeed %u_tagline {%a_name %a_gname}"
set variables(COMPLETE_RAR)		"%pf %releasename %t_mbytes %t_files %t_avgspeed %r_avgspeed %t_duration %u_name %g_name %filename %u_tagline %compression"
set variables(COMPLETE_ZIP)		"%pf %releasename %t_mbytes %t_files %t_avgspeed %r_avgspeed %t_duration %u_name %g_name %filename %u_tagline"
set variables(COMPLETE_OTHER)		"%pf %releasename %t_mbytes %t_files %t_avgspeed %r_avgspeed %t_duration %u_name %g_name %filename %u_tagline"
set variables(COMPLETE_VIDEO)		"%pf %releasename %t_mbytes %t_files %t_avgspeed %r_avgspeed %t_duration %u_name %g_name %filename %u_tagline"
set variables(COMPLETE_AUDIO_CBR)	"%pf %releasename %t_mbytes %t_files %t_avgspeed %r_avgspeed %t_duration %u_name %g_name %genre %year %bitrate %sampling %mode %audio %artist %album %title %codec %layer %filename %u_tagline"
set variables(COMPLETE_AUDIO_VBR)	"%pf %releasename %t_mbytes %t_files %t_avgspeed %r_avgspeed %t_duration %u_name %g_name %genre %year %bitrate %sampling %mode %audio %a_stream %a_version %artist %album %title %codec %layer %filename %u_tagline"
set variables(COMPLETE_STAT_RACE_RAR)	"%pf %releasename %t_mbytes %t_files %t_avgspeed %r_avgspeed %t_duration %u_count %g_count %u_slowest_name %u_slowest_gname %u_slowest_speed %u_fastest_name %u_fastest_gname %u_fastest_speed %u_winner_name %u_winner_gname %u_winner_mbytes %u_winner_files %u_winner_percent %u_winner_avgspeed %g_winner_name %g_winner_mbytes %g_winner_files %g_winner_percent %g_winner_avgspeed %filename {%r_name %r_gname} %compression {%g_racer_position %g_racer_name %g_racer_mbytes %g_racer_files %g_racer_percent %g_racer_avgspeed} {%u_racer_position %u_racer_name %g_racer_name %u_racer_mbytes %u_racer_files %u_racer_percent %u_racer_avgspeed %u_racer_dayup %u_racer_wkup %u_racer_monthup %u_racer_allup}"
set variables(COMPLETE_STAT_RACE_ZIP)	"%pf %releasename %t_mbytes %t_files %t_avgspeed %r_avgspeed %t_duration %u_count %g_count %u_slowest_name %u_slowest_gname %u_slowest_speed %u_fastest_name %u_fastest_gname %u_fastest_speed %u_winner_name %u_winner_gname %u_winner_mbytes %u_winner_files %u_winner_percent %u_winner_avgspeed %g_winner_name %g_winner_mbytes %g_winner_files %g_winner_percent %g_winner_avgspeed %filename {%r_name %r_gname} {%g_racer_position %g_racer_name %g_racer_mbytes %g_racer_files %g_racer_percent %g_racer_avgspeed} {%u_racer_position %u_racer_name %g_racer_name %u_racer_mbytes %u_racer_files %u_racer_percent %u_racer_avgspeed %u_racer_dayup %u_racer_wkup %u_racer_monthup %u_racer_allup}"
set variables(COMPLETE_STAT_RACE_OTHER)	"%pf %releasename %t_mbytes %t_files %t_avgspeed %r_avgspeed %t_duration %u_count %g_count %u_slowest_name %u_slowest_gname %u_slowest_speed %u_fastest_name %u_fastest_gname %u_fastest_speed %u_winner_name %u_winner_gname %u_winner_mbytes %u_winner_files %u_winner_percent %u_winner_avgspeed %g_winner_name %g_winner_mbytes %g_winner_files %g_winner_percent %g_winner_avgspeed %filename {%r_name %r_gname} {%g_racer_position %g_racer_name %g_racer_mbytes %g_racer_files %g_racer_percent %g_racer_avgspeed} {%u_racer_position %u_racer_name %g_racer_name %u_racer_mbytes %u_racer_files %u_racer_percent %u_racer_avgspeed %u_racer_dayup %u_racer_wkup %u_racer_monthup %u_racer_allup}"
set variables(COMPLETE_STAT_RACE_VIDEO)	"%pf %releasename %t_mbytes %t_files %t_avgspeed %r_avgspeed %t_duration %u_count %g_count %u_slowest_name %u_slowest_gname %u_slowest_speed %u_fastest_name %u_fastest_gname %u_fastest_speed %u_winner_name %u_winner_gname %u_winner_mbytes %u_winner_files %u_winner_percent %u_winner_avgspeed %g_winner_name %g_winner_mbytes %g_winner_files %g_winner_percent %g_winner_avgspeed %filename {%r_name %r_gname} {%g_racer_position %g_racer_name %g_racer_mbytes %g_racer_files %g_racer_percent %g_racer_avgspeed} {%u_racer_position %u_racer_name %g_racer_name %u_racer_mbytes %u_racer_files %u_racer_percent %u_racer_avgspeed %u_racer_dayup %u_racer_wkup %u_racer_monthup %u_racer_allup}"
set variables(COMPLETE_STAT_RACE_AUDIO_CBR)	"%pf %releasename %t_mbytes %t_files %t_avgspeed %r_avgspeed %t_duration %u_count %g_count %genre %year %bitrate %sampling %mode %audio %artist %album %title %codec %layer %u_slowest_name %u_slowest_gname %u_slowest_speed %u_fastest_name %u_fastest_gname %u_fastest_speed %u_winner_name %u_winner_gname %u_winner_mbytes %u_winner_files %u_winner_percent %u_winner_avgspeed %g_winner_name %g_winner_mbytes %g_winner_files %g_winner_percent %g_winner_avgspeed %filename {%r_name %r_gname} {%g_racer_position %g_racer_name %g_racer_mbytes %g_racer_files %g_racer_percent %g_racer_avgspeed} {%u_racer_position %u_racer_name %g_racer_name %u_racer_mbytes %u_racer_files %u_racer_percent %u_racer_avgspeed %u_racer_dayup %u_racer_wkup %u_racer_monthup %u_racer_allup}"
set variables(COMPLETE_STAT_RACE_AUDIO_VBR)	"%pf %releasename %t_mbytes %t_files %t_avgspeed %r_avgspeed %t_duration %u_count %g_count %genre %year %bitrate %sampling %mode %audio %a_stream %a_version %artist %album %title %codec %layer %u_slowest_name %u_slowest_gname %u_slowest_speed %u_fastest_name %u_fastest_gname %u_fastest_speed %u_winner_name %u_winner_gname %u_winner_mbytes %u_winner_files %u_winner_percent %u_winner_avgspeed %g_winner_name %g_winner_mbytes %g_winner_files %g_winner_percent %g_winner_avgspeed %filename {%r_name %r_gname} {%g_racer_position %g_racer_name %g_racer_mbytes %g_racer_files %g_racer_percent %g_racer_avgspeed} {%u_racer_position %u_racer_name %g_racer_name %u_racer_mbytes %u_racer_files %u_racer_percent %u_racer_avgspeed %u_racer_dayup %u_racer_wkup %u_racer_monthup %u_racer_allup}"
set variables(INCOMPLETE)		"%pf %u_name %g_name %releasename"
set variables(BAD)			"%pf %releasename %u_name %g_name"

## Converts empty or zero variable to something else if defined
#
# Example:
#  set zeroconvert(%user) "NoOne"
set zeroconvert(%u_name)		"NoOne"
set zeroconvert(%g_name) 		"NoGroup"

