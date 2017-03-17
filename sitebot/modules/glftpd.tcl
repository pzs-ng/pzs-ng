################################################################################
#
#                        ngBot - glftpd FTP Module
#            Project Zipscript - Next Generation (www.pzs-ng.com)
#    (Based on dZSbot by Dark0n3 - http://kotisivu.raketti.net/darkone/)
#
################################################################################

namespace eval ::ngBot::module::glftpd {
	variable ns [namespace current]
	variable np [namespace qualifiers [namespace parent]]
	variable renames [list init_invite init_binaries readlog ng_format]
	variable glversion

	# This also exports the cmd_* procs, enabling binding to it from the base tcl
	namespace export *

	#############################################################################
	# De/Initialisation Commands                                                #
	#############################################################################

	proc init {} {
		variable np
		variable renames

		foreach name $renames {
			rename ${np}::$name ${np}::_$name
		}

		namespace inscope $np {
			namespace import -force ::ngBot::module::glftpd::*
		}
	}

	# 'Prepend' to the existing init_binaries proc in base.
	proc init_binaries {} {
		variable np
		variable ${np}::dzerror
		variable ${np}::location

		foreach {filename filepath} [array get location] {
				if {![file exists $filepath]} {
						putlog "\[ngBot\] Error :: Invalid path for $filename ($filepath)."
						set dzerror 1
				}
		}

		${np}::_init_binaries
	}

	# Replace the stubbed init_version proc in base.
	proc init_version {} {
		variable np
		variable glversion
		variable ${np}::binary
		variable ${np}::scriptpath
		variable ${np}::use_glftpd2

		if {[string equal -nocase "AUTO" $use_glftpd2]} {
			if {![info exists binary(GLFTPD)]} {
				putlog "\[ngBot\] Error :: you did not thoroughly edit the $scriptpath/ngBot.conf file (hint: binary(GLFTPD))."
				die
			}

			catch {exec strings $binary(GLFTPD) | grep -i "^glftpd " | cut -f1 -d. | tr A-Z a-z} glversion

			if {[string equal "glftpd 1" $glversion]} {
				putlog "\[ngBot\] Detected $glversion, running in legacy mode."
				set glversion 1
			} elseif {[string equal "glftpd 2" $glversion]} {
				putlog "\[ngBot\] Detected $glversion, running in standard mode."
				set glversion 2
			} else {
				putlog "\[ngBot\] Auto-detecting glftpd version failed, set \"use_glftpd2\" in $scriptpath/ngBot.conf manually."
				die
			}
		} else {
			set glversion [expr [istrue $use_glftpd2] ? 2 : 1]
			putlog "\[ngBot\] glftpd version defined as: $glversion."
		}
	}

	# 'Prepend' to the existing init_invite proc in base.
	proc init_invite {} {
		variable ns
		variable np
		variable ${np}::enable_irc_invite

		if {[istrue $enable_irc_invite]} {
			bind msg -|- !invite ${ns}::cmd_invite
		}

		${np}::_init_invite
	}

	proc deinit {} {
		variable ns
		variable np
		variable renames

		foreach name $renames {
			if {[llength [info commands "${np}::_$name"]] == 1} {
				catch {rename ${np}::$name {}}
				catch {rename ${np}::_$name ${np}::$name}
			}
		}

		namespace delete $ns
	}

	#################################################################################
	# Hooked Commands                                                               #
	#################################################################################

	proc readlog {} {
		variable np
		variable ns
		variable glversion

		${np}::_readlog

		if {$glversion == 1} {
			${ns}::launchnuke
		}
	}

	proc ng_format {event section line} {
		variable ns
		variable np
		variable glversion

		if {[string equal $event "NUKE"] || [string equal $event "UNNUKE"]} {
			if {$glversion == 1} {
				${ns}::fuelnuke $event [lindex $line 0] $section $line
			} elseif {$glversion == 2} {
				${ns}::launchnuke2 $event [lindex $line 0] $section [lrange $line 1 3] [lrange $line 4 end]
			} else {
				putlog "\[ngBot\] Error :: Internal error, unknown glftpd version ($glversion)."
			}
			return ""
		}

		return [${np}::_ng_format $event $section $line]
	}

	#################################################################################
	# Nuke and Unnuke Handlers                                                      #
	#################################################################################

	proc fuelnuke {type path section line} {
		variable ns
		variable np
		variable ${np}::nuke
		variable ${np}::hidenuke

		if {$type == $nuke(LASTTYPE) && $path == $nuke(LASTDIR) && $nuke(SHOWN) == 0} {
			if {[lsearch -exact $hidenuke [lindex $line 2]] == -1} {
				append nuke(NUKEE) "\002[lindex $line 2]\002 (\002[lindex $line 3 1]\002MB), "
			}
		} else {
			${ns}::launchnuke
			if {[lsearch -exact $hidenuke [lindex $line 2]] == -1} {
				set nuke(TYPE) $type
				set nuke(PATH) $path
				set nuke(SECTION) $section
				set nuke(NUKER) [lindex $line 1]
				set nuke(NUKEE) "\002[lindex $line 2]\002 (\002[lindex $line 3 1]\002MB) "
				set nuke(MULT) [lindex $line 3 0]
				set nuke(REASON) [lindex $line 4]
				set nuke(SHOWN) 0
			}
		}
		set nuke(LASTTYPE) $type
		set nuke(LASTDIR) $path
	}

	proc launchnuke {} {
		variable np
		variable ${np}::nuke
		variable ${np}::mpath
		variable ${np}::theme
		variable ${np}::announce

		if {$nuke(SHOWN) == 1} {return 0}
		set nuke(NUKEE) [${np}::trimtail $nuke(NUKEE) $theme(SPLITTER)]
		set output "$theme(PREFIX)$announce($nuke(TYPE))"
		set output [${np}::replacebasic $output $nuke(SECTION)]
		set output [${np}::replacevar $output "%nuker" $nuke(NUKER)]
		set output [${np}::replacevar $output "%nukees" $nuke(NUKEE)]
		set output [${np}::replacevar $output "%type" $nuke(TYPE)]
		set output [${np}::replacevar $output "%multiplier" $nuke(MULT)]
		set output [${np}::replacevar $output "%reason" $nuke(REASON)]
		set output [${np}::replacepath $output $mpath $nuke(PATH)]
		${np}::sndall $nuke(TYPE) $nuke(SECTION) $output
		set nuke(SHOWN) 1
	}

	proc launchnuke2 {type path section info nukees} {
		variable np
		variable ${np}::nuke
		variable ${np}::theme
		variable ${np}::mpath
		variable ${np}::hidenuke
		variable ${np}::announce
		variable ${np}::sitename

		set nuke(TYPE) $type
		set nuke(PATH) $path
		set nuke(SECTION) $section
		set nuke(NUKER) [lindex $info 0]
		set nuke(MULT) [lindex $info 1]
		set nuke(REASON) [lindex $info 2]
		set nuke(NUKEE) {}

		foreach entry $nukees {
			if {[lsearch -exact $hidenuke [lindex $entry 0]] == -1} {
				set mb [format "%.1f" [expr [lindex $entry 1] / 1024]]
				set nukee $announce(NUKEES)
				set nukee [${np}::replacevar $nukee "%u_name" [lindex $entry 0]]
				set nukee [${np}::replacevar $nukee "%size" $mb]
				append nuke(NUKEE) $nukee $theme(SPLITTER)
			}
		}
		set nuke(NUKEE) [${np}::trimtail $nuke(NUKEE) $theme(SPLITTER)]
		set output "$theme(PREFIX)$announce($nuke(TYPE))"
		set output [${np}::replacebasic $output $nuke(SECTION)]
		set output [${np}::replacevar $output "%nuker" $nuke(NUKER)]
		set output [${np}::replacevar $output "%nukees" $nuke(NUKEE)]
		set output [${np}::replacevar $output "%type" $nuke(TYPE)]
		set output [${np}::replacevar $output "%multiplier" $nuke(MULT)]
		set output [${np}::replacevar $output "%reason" $nuke(REASON)]
		set output [${np}::replacepath $output $mpath $nuke(PATH)]
		${np}::sndall $nuke(TYPE) $nuke(SECTION) $output
	}

	#################################################################################
	# glFTPd Users and Groups                                                       #
	#################################################################################

	proc gluserids {} {
		variable np
		variable ${np}::location
		set userlist {}
		if {![catch {set fh [open $location(PASSWD) r]} error]} {
			while {![eof $fh]} {
				## user:password:uid:gid:date:homedir:irrelevant
				set line [split [gets $fh] ":"]
				if {[llength $line] != 7} {continue}
				lappend userlist [lindex $line 2] [lindex $line 0]
			}
			close $fh
		} else {
			putlog "\[ngBot\] Error :: Could not open passwd ($error)."
		}
		return $userlist
	}

	proc glgroupids {} {
		variable np
		variable ${np}::location
		set grouplist {}
		if {![catch {set fh [open $location(GROUP) r]} error]} {
			while {![eof $fh]} {
				## group:description:gid:irrelevant
				set line [split [gets $fh] ":"]
				if {[llength $line] != 4} {continue}
				lappend grouplist [lindex $line 2] [lindex $line 0]
			}
			close $fh
		} else {
			putlog "\[ngBot\] Error :: Could not open group ($error)."
		}
		return $grouplist
	}

	#################################################################################
	# Stats Command                                                                 #
	#################################################################################

	proc cmd_stats {type time nick uhost hand chan argv} {
		variable np
		variable ${np}::binary
		variable ${np}::statsection
		variable ${np}::statdefault
		variable ${np}::location
		variable ${np}::announcetochan
		variable ${np}::stats

		${np}::checkchan $nick $chan

		if {[info exists ::mclastbind]} {set ::lastbind $::mclastbind}
		set rcvr $nick
		if {[${np}::announcetochancheck $::lastbind 0]} {
			set rcvr $chan
		}

		set argv [split $argv]
		set section $statdefault
		if {[string length [lindex $argv 0]]} {
			set error 1
			set sections ""
			foreach {name value} [array get statsection] {
				if {[string equal -nocase $value [lindex $argv 0]]} {
					set section $name
					set error 0; break
				}
				lappend sections $value
			}
			if {$error} {
				puthelp "PRIVMSG $rcvr :Invalid section, sections: [join [lsort -ascii $sections] {, }]"
				return
			}
		}

		set zeros ""
		if {[istrue $stats(hide_zeros)]} {
			set zeros "-0"
		}
		set hideusers [list]
		if {[info exists stats(hide_users)]} {
			foreach {u} [split "$stats(hide_users)"] {
				lappend hideusers "-e$u"
			}
		}
		set hidegroups [list]
		if {[info exists stats(hide_groups)]} {
			foreach {u} [split "$stats(hide_groups)"] {
				lappend hidegroups "-g$u"
			}
		}
		if {[catch {set output [exec $binary(STATS) -r $location(GLCONF) $type $time -s $section $zeros {*}$hideusers {*}$hidegroups]} error]} {
			putlog "\[ngBot\] Error :: Unable to retrieve stats ($error)."
			return
		}

		foreach line [split $output "\n"] {
			if {![info exists newline($line)]} {
				set newline($line) 0
			} else {
				incr newline($line)
			}
			puthelp "PRIVMSG $rcvr :$line\003$newline($line)"
		}
		puthelp "PRIVMSG $rcvr :------------------------------------------------------------------------"
		return
	}

	################################################################################
	# Latest Dirs/Nukes Commands                                                   #
	################################################################################

	proc cmd_new {nick uhost hand chan argv} {
		variable ns
		variable np
		variable ${np}::theme
		variable ${np}::binary
		variable ${np}::announce
		variable ${np}::location
		variable ${np}::sections
		variable ${np}::defaultsection
		variable ${np}::announcetochan

		${np}::checkchan $nick $chan

		if {[info exists ::mclastbind]} {set ::lastbind $::mclastbind}
		set rcvr $nick
		if {[${np}::announcetochancheck $::lastbind 0]} {
			set rcvr $chan
		}

		if {![${np}::getoptions $argv results section]} {
			## By displaying the command syntax in the channel (opposed to private message), we can inform others
			## at the same time. There's this recurring phenomena, every time a user types an "uncommon" command, half
			## a dozen others will as well...to learn about this command. So, let's kill a few idiots with one stone.
			puthelp "PRIVMSG $chan :\002Usage:\002 $::lastbind \[-max <num>\] \[section\]"
			return
		}
		if {[string equal "" $section]} {
			set section $defaultsection
			set sectionpath "/site/*"
			set lines [exec $binary(SHOWLOG) -l -m $results -r $location(GLCONF)]
		} else {
			if {[set sectiondata [${np}::getsectionpath $section]] == ""} {
				puthelp "PRIVMSG $rcvr :Invalid section, sections: [join [lsort -ascii $sections] {, }]"
				return
			}
			foreach {section sectionpath} $sectiondata {break}
			set lines [exec $binary(SHOWLOG) -f -l -m $results -p $sectionpath -r $location(GLCONF)]
		}

		## Retrieve a list of UIDs/users and GIDs/groups
		array set uid [${ns}::gluserids]
		array set gid [${ns}::glgroupids]

		set output "$theme(PREFIX)$announce(NEW)"
		set output [${np}::replacevar $output "%section" $section]
		${np}::sndone $rcvr [${np}::replacebasic $output "NEW"]
		set body "$theme(PREFIX)$announce(NEW_BODY)"
		set num 0

		foreach line [split $lines "\n"] {
			## Format: status|uptime|uploader|group|files|kilobytes|dirpath
			if {[llength [set line [split $line "|"]]] != 7} {continue}
			foreach {status ctime userid groupid files kbytes dirpath} $line {break}

			## If array get returns "", zeroconvert will replace the value with NoOne/NoGroup.
			set user [lindex [array get uid $userid] 1]
			set group [lindex [array get gid $groupid] 1]

			set output [${np}::replacevar $body "%num" [format "%02d" [incr num]]]
			set age [lrange [${np}::format_duration [expr {[clock seconds] - $ctime}]] 0 1]
			set output [${np}::replacevar $output "%age" $age]
			set output [${np}::replacevar $output "%date" [${np}::format_clock "date" $ctime]]
			set output [${np}::replacevar $output "%time" [${np}::format_clock "time" $ctime]]
			set output [${np}::replacevar $output "%u_name" $user]
			set output [${np}::replacevar $output "%g_name" $group]
			set output [${np}::replacevar $output "%files" $files]
			set output [${np}::replacevar $output "%mbytes" [format "%.1f" [expr {$kbytes / 1024.0}]]]
			set output [${np}::replacepath $output $sectionpath $dirpath]
			${np}::sndone $rcvr [${np}::replacebasic $output "NEW"]
		}

		if {!$num} {
			set output "$theme(PREFIX)$announce(NEW_NONE)"
			${np}::sndone $rcvr [${np}::replacebasic $output "NEW"]
		}
		return
	}

	proc cmd_silo {type nick uhost hand chan argv} {
		variable np
		variable ${np}::theme
		variable ${np}::binary
		variable ${np}::announce
		variable ${np}::location
		variable ${np}::sections
		variable ${np}::defaultsection
		variable ${np}::announcetochan

		${np}::checkchan $nick $chan

		if {[info exists ::mclastbind]} {set ::lastbind $::mclastbind}
		set rcvr $nick
		if {[${np}::announcetochancheck $::lastbind 0]} {
			set rcvr $chan
		}

		if {![${np}::getoptions $argv results section]} {
			puthelp "PRIVMSG $chan :\002Usage:\002 $::lastbind \[-max <num>\] \[section\]"
			return
		}
		if {[string equal [set type [string toupper $type]] "NUKES"]} {
			set type_opt "-n"
		} else {
			set type_opt "-u"
		}
		if {[string equal "" $section]} {
			set section $defaultsection
			set sectionpath "/site/*"
			set lines [exec $binary(SHOWLOG) $type_opt -m $results -r $location(GLCONF)]
		} else {
			if {[set sectiondata [${np}::getsectionpath $section]] == ""} {
				puthelp "PRIVMSG $rcvr :Invalid section, sections: [join [lsort -ascii $sections] {, }]"
				return
			}
			foreach {section sectionpath} $sectiondata {break}
			set lines [exec $binary(SHOWLOG) -f $type_opt -m $results -p $sectionpath -r $location(GLCONF)]
		}

		set output "$theme(PREFIX)$announce($type)"
		set output [${np}::replacevar $output "%section" $section]
		${np}::sndone $rcvr [${np}::replacebasic $output $type]
		set body "$theme(PREFIX)$announce(${type}_BODY)"
		set num 0

		foreach line [split $lines "\n"] {
			## Format: status|nuketime|nuker|unnuker|nukee|multiplier|reason|kilobytes|dirpath
			if {[llength [set line [split $line "|"]]] != 9} {continue}
			foreach {status nuketime nuker unnuker nukee multiplier reason kbytes dirpath} $line {break}

			set output [${np}::replacevar $body "%num" [format "%02d" [incr num]]]
			set output [${np}::replacevar $output "%date" [${np}::format_clock "date" $nuketime]]
			set output [${np}::replacevar $output "%time" [${np}::format_clock "time" $nuketime]]
			set output [${np}::replacevar $output "%nuker" $nuker]
			set output [${np}::replacevar $output "%unnuker" $unnuker]
			set output [${np}::replacevar $output "%nukee" $nukee]
			set output [${np}::replacevar $output "%multiplier" $multiplier]
			set output [${np}::replacevar $output "%reason" $reason]
			set output [${np}::replacevar $output "%mbytes" [format "%.1f" [expr {$kbytes / 1024.0}]]]
			set output [${np}::replacepath $output $sectionpath $dirpath]
			${np}::sndone $rcvr [${np}::replacebasic $output $type]
		}

		if {!$num} {
			set output "$theme(PREFIX)$announce(${type}_NONE)"
			${np}::sndone $rcvr [${np}::replacebasic $output $type]
		}
		return
	}

	proc cmd_search {nick uhost hand chan argv} {
		variable ns
		variable np
		variable ${np}::theme
		variable ${np}::binary
		variable ${np}::announce
		variable ${np}::location
		variable ${np}::search_chars
		variable ${np}::defaultsection
		variable ${np}::announcetochan

		${np}::checkchan $nick $chan

		if {[info exists ::mclastbind]} {set ::lastbind $::mclastbind}
		set rcvr $nick
		if {[${np}::announcetochancheck $::lastbind 0]} {
			set rcvr $chan
		}

		if {![${np}::getoptions $argv results pattern] || [string equal "" $pattern]} {
			puthelp "PRIVMSG $chan :\002Usage:\002 $::lastbind \[-max <num>\] <pattern>"
			return
		}
		if {$search_chars > 0 && [regexp -all {[a-zA-Z0-9]} $pattern] < $search_chars} {
			puthelp "PRIVMSG $rcvr :The search pattern must be at least \002$search_chars\002 alphanumeric characters."
			return
		}
		## Retrieve matching dirlog entries
		regsub -all {[\s\*]+} "*${pattern}*" {*} pattern
		set lines [exec $binary(SHOWLOG) -l -s -m $results -p $pattern -r $location(GLCONF)]

		## Retrieve a list of UIDs/users and GIDs/groups
		array set uid [${ns}::gluserids]
		array set gid [${ns}::glgroupids]

		set output "$theme(PREFIX)$announce(SEARCH)"
		set output [${np}::replacevar $output "%pattern" $pattern]
		${np}::sndone $rcvr [${np}::replacebasic $output "SEARCH"]
		set body "$theme(PREFIX)$announce(SEARCH_BODY)"
		set num 0

		foreach line [split $lines "\n"] {
			## Format: status|uptime|uploader|group|files|kilobytes|dirpath
			if {[llength [set line [split $line "|"]]] != 7} {continue}
			foreach {status ctime userid groupid files kbytes dirpath} $line {break}

			## If array get returns "", zeroconvert will replace the value with NoOne/NoGroup.
			set user [lindex [array get uid $userid] 1]
			set group [lindex [array get gid $groupid] 1]

			set output [${np}::replacevar $body "%num" [format "%02d" [incr num]]]
			set age [lrange [${np}::format_duration [expr {[clock seconds] - $ctime}]] 0 1]
			set output [${np}::replacevar $output "%age" $age]
			set output [${np}::replacevar $output "%date" [${np}::format_clock "date" $ctime]]
			set output [${np}::replacevar $output "%time" [${np}::format_clock "time" $ctime]]
			set output [${np}::replacevar $output "%u_name" $user]
			set output [${np}::replacevar $output "%g_name" $group]
			set output [${np}::replacevar $output "%files" $files]
			set output [${np}::replacevar $output "%mbytes" [format "%.1f" [expr {$kbytes / 1024.0}]]]
			set output [${np}::replacepath $output "/site/*" $dirpath]
			${np}::sndone $rcvr [${np}::replacebasic $output "SEARCH"]
		}

		if {!$num} {
			set output "$theme(PREFIX)$announce(SEARCH_NONE)"
			${np}::sndone $rcvr [${np}::replacebasic $output "SEARCH"]
		}
		return
	}

	################################################################################
	# Online Stats Commands                                                        #
	################################################################################

	proc cmd_bw {type nick uhost hand chan argv} {
		variable np
		variable ${np}::speed
		variable ${np}::theme
		variable ${np}::binary
		variable ${np}::announce
		variable ${np}::announcetochan

		${np}::checkchan $nick $chan

		set output "$theme(PREFIX)$announce([string toupper $type])"
		set raw [exec $binary(WHO) --nbw]
		set upper [format "%.0f" [expr [lindex $raw 1] * 100 / $speed(INCOMING)]]
		set dnper [format "%.0f" [expr [lindex $raw 3] * 100 / $speed(OUTGOING)]]
		set totalper [format "%.0f" [expr [lindex $raw 5] * 100 / ( $speed(INCOMING) + $speed(OUTGOING) )]]

		set output [${np}::replacevar $output "%uploads" [lindex $raw 0]]
		set output [${np}::replacevar $output "%upspeed" [${np}::format_speed [lindex $raw 1] "none"]]
		set output [${np}::replacevar $output "%downloads" [lindex $raw 2]]
		set output [${np}::replacevar $output "%dnspeed" [${np}::format_speed [lindex $raw 3] "none"]]
		set output [${np}::replacevar $output "%transfers" [lindex $raw 4]]
		set output [${np}::replacevar $output "%totalspeed" [${np}::format_speed [lindex $raw 5] "none"]]
		set output [${np}::replacevar $output "%idlers" [lindex $raw 6]]
		set output [${np}::replacevar $output "%active" [lindex $raw 7]]
		set output [${np}::replacevar $output "%totallogins" [lindex $raw 8]]
		set output [${np}::replacevar $output "%maxusers" [lindex $raw 9]]

		set output [${np}::replacevar $output "%uppercent" $upper]
		set output [${np}::replacevar $output "%dnpercent" $dnper]
		set output [${np}::replacevar $output "%totalpercent" $totalper]

		if {[info exists ::mclastbind]} {set ::lastbind $::mclastbind}
		set rcvr $chan
		if {![${np}::announcetochancheck $::lastbind 1]} {
			set rcvr $nick
		}
		${np}::sndone $rcvr [${np}::replacebasic $output "BW"]
		return
	}

	proc cmd_idlers {nick uhost hand chan argv} {
		variable np
		variable ${np}::speed
		variable ${np}::theme
		variable ${np}::binary
		variable ${np}::announce
		variable ${np}::minidletime
		variable ${np}::announcetochan

		${np}::checkchan $nick $chan

		if {[info exists ::mclastbind]} {set ::lastbind $::mclastbind}
		set rcvr $chan
		if {![${np}::announcetochancheck $::lastbind 1]} {
			set rcvr $nick
		}

		set output "$theme(PREFIX)$announce(IDLE)"
		${np}::sndone $rcvr [${np}::replacebasic $output "IDLE"]

		set raw [exec $binary(WHO) "--raw"]
		set count 0; set total 0.0

		foreach line [split $raw "\n"] {
			if {[string equal "USER" [lindex $line 0]] && [string equal "ID" [lindex $line 4]]} {
				set user  [lindex $line 2]
				set group [lindex $line 3]
				set idletime [lindex $line 5]
				set tagline [lindex $line 6]
				set since [lindex $line 7]
				set curdir [lindex $line 10]
				set upid [lindex $line 11]

				if {$idletime > $minidletime} {
					set output [${np}::replacevar "$theme(PREFIX)$announce(USERIDLE)" "%u_name" $user]
					set output [${np}::replacevar $output "%g_name" $group]
					set output [${np}::replacevar $output "%idletime" [${np}::format_duration $idletime]]
					set output [${np}::replacevar $output "%tagline" $tagline]
					set output [${np}::replacevar $output "%since" $since]
					set output [${np}::replacevar $output "%currentdir" $curdir]
					set output [${np}::replacevar $output "%u_pid" $upid]
					${np}::sndone $rcvr [${np}::replacebasic $output "IDLE"]
					incr count
				}
			}
		}
		set output [${np}::replacevar "$theme(PREFIX)$announce(TOTIDLE)" "%count" $count]
		${np}::sndone $rcvr [${np}::replacebasic $output "IDLE"]
		return
	}

	proc cmd_speed {nick uhost hand chan argv} {
		variable np
		variable ${np}::theme
		variable ${np}::binary
		variable ${np}::disable
		variable ${np}::announce
		variable ${np}::announcetochan

		${np}::checkchan $nick $chan

		if {[info exists ::mclastbind]} {set ::lastbind $::mclastbind}
		set rcvr $chan
		if {![${np}::announcetochancheck $::lastbind 1]} {
			set rcvr $nick
		}

		if {[string equal $argv ""]} {
			puthelp "PRIVMSG $chan :\002Usage:\002 $::lastbind <user>"
			return
		}
		set line ""
		set argv [split $argv]
		if {$disable(ALTWHO) != 1} {
			set output "$theme(PREFIX)$announce(SPEEDERROR)"
			foreach line [split [exec $binary(WHO) --raw [lindex $argv 0]] "\n"] {
				set action [lindex $line 4]
				if {$action == "DN"} {
					set output "$theme(PREFIX)$announce(SPEEDDN)"
					set output [${np}::replacevar $output "%dnspeed" [${np}::format_speed [lindex $line 5] "none"]]
					set output [${np}::replacevar $output "%dnpercent" [lindex $line 9]]
				} elseif {$action == "UP"} {
					set output "$theme(PREFIX)$announce(SPEEDUP)"
					set output [${np}::replacevar $output "%upspeed" [${np}::format_speed [lindex $line 5] "none"]]
					set output [${np}::replacevar $output "%uppercent" [lindex $line 9]]
				} elseif {$action == "ID"} {
					set output "$theme(PREFIX)$announce(SPEEDID)"
					set output [${np}::replacevar $output "%idletime" [${np}::format_duration [lindex $line 5]]]
				}
				set output [${np}::replacevar $output "%u_name" [lindex $line 2]]
				set output [${np}::replacevar $output "%g_name" [lindex $line 3]]
				set output [${np}::replacevar $output "%tagline" [lindex $line 6]]
				set output [${np}::replacevar $output "%timeonline" [lindex $line 7]]
				set output [${np}::replacevar $output "%f_name" [lindex $line 8]]
				set output [${np}::replacevar $output "%currentdir" [lindex $line 10]]
				set output [${np}::replacevar $output "%u_pid" [lindex $line 11]]
				${np}::sndone $rcvr [${np}::replacebasic $output "SPEED"]
			}
		} else {
			set base_output "$theme(PREFIX)$announce(DEFAULT)"
			foreach line [split [exec $binary(WHO) [lindex $argv 0]] "\n"] {
				set output [${np}::replacevar $base_output "%msg" $line]
				${np}::sndone $rcvr [${np}::replacebasic $output "SPEED"]
			}
		}

		if {[string equal "" $line]} {
			set output "$theme(PREFIX)$announce(SPEEDERROR)"
			set output [${np}::replacevar $output "%msg" "User not online."]
			${np}::sndone $rcvr [${np}::replacebasic $output "SPEED"]
		}
		return
	}

	proc cmd_transfers {type nick uhost hand chan argv} {
		variable np
		variable ${np}::speed
		variable ${np}::theme
		variable ${np}::binary
		variable ${np}::announce
		variable ${np}::announcetochan

		${np}::checkchan $nick $chan

		if {[string equal [set type [string toupper $type]] "UP"]} {
			set type_long "UPLOAD"
			set type_speed $speed(INCOMING)
			set type_string "Uploaders:"
		} else {
			set type_long "LEECH"
			set type_speed $speed(OUTGOING)
			set type_string "Leechers:"
		}

		if {[info exists ::mclastbind]} {set ::lastbind $::mclastbind}
		set rcvr $chan
		if {![${np}::announcetochancheck $::lastbind 1]} {
			set rcvr $nick
		}

		set output "$theme(PREFIX)$announce($type_long)"
		${np}::sndone $rcvr [${np}::replacebasic $output $type_long]

		set raw [exec $binary(WHO) "--raw"]
		set count 0; set total 0.0

		foreach line [split $raw "\n"] {
			if {[string equal "USER" [lindex $line 0]] && [string equal $type [lindex $line 4]]} {
				set uspeed [${np}::replacevar [lindex $line 5] "KB/s" ""]
				set per [format "%.2f%%" [expr double($uspeed) * 100 / double($type_speed)]]
				set output [${np}::replacevar "$theme(PREFIX)$announce(USER)" "%u_name" [lindex $line 2]]
				set output [${np}::replacevar $output "%g_name" [lindex $line 3]]
				set output [${np}::replacevar $output "%fper" [lindex $line 9]]
				set output [${np}::replacevar $output "%speed" [${np}::format_speed $uspeed "none"]]
				set output [${np}::replacevar $output "%per" $per]
				set output [${np}::replacevar $output "%tagline" [lindex $line 6]]
				set output [${np}::replacevar $output "%since" [lindex $line 7]]
				set output [${np}::replacevar $output "%filename" [lindex $line 8]]
				set output [${np}::replacevar $output "%currentdir" [lindex $line 10]]
				set output [${np}::replacevar $output "%u_pid" [lindex $line 11]]
				${np}::sndone $rcvr [${np}::replacebasic $output $type_long]
				incr count
				set total [expr {$total + $uspeed}]
			}
		}

		set per [format "%.1f" [expr double($total) * 100 / double($type_speed) ]]

		set output [${np}::replacevar "$theme(PREFIX)$announce(TOTUPDN)" "%type" $type_string]
		set output [${np}::replacevar $output "%count" $count]
		set output [${np}::replacevar $output "%total" [${np}::format_speed $total "none"]]
		set output [${np}::replacevar $output "%per" $per]
		${np}::sndone $rcvr [${np}::replacebasic $output $type_long]
		return
	}

	proc cmd_who {nick uhost hand chan argv} {
		variable np
		variable ${np}::binary
		variable ${np}::announcetochan

		${np}::checkchan $nick $chan

		if {[info exists ::mclastbind]} {set ::lastbind $::mclastbind}
		set rcvr $nick
		if {[${np}::announcetochancheck $::lastbind 0]} {
			set rcvr $chan
		}

		foreach line [split [exec $binary(WHO)] "\n"] {
			if {![info exists newline($line)]} {
				set newline($line) 0
			} else {
				incr newline($line)
			}
			puthelp "PRIVMSG $rcvr :$line\003$newline($line)"
		}
		return
	}

	#################################################################################
	# Invite User                                                                   #
	#################################################################################

	proc flagcheck {currentflags needflags} {
		set currentflags [split $currentflags ""]
		foreach needflag [split $needflags ""] {
			if {![string equal "" $needflag] && [lsearch -glob $currentflags $needflag] != -1} {return 1}
		}
		return 0
	}

	proc rightscheck {rights user group flags} {
		variable ns
		variable np
		variable ${np}::location
		variable ${np}::privchannel

		set groups [list]
		if {![catch {set handle [open "$location(USERS)/$user" r]} error]} {
			set data [read $handle]
			close $handle
			foreach line [split $data "\n"] {
				switch -exact -- [lindex $line 0] {
					"PRIVATE" {lappend groups [lindex $line 1]}
					"GROUP" {lappend groups [lindex $line 1]}
				}
			}
		} else {
			putlog "\[ngBot\] Error :: Unable to open user file for \"$user\" ($error)."
		}
		foreach right $rights {
			set prefix [string index $right 0]
			if {[string equal "!" $prefix]} {
				## 'Not' matching (!)
				set right [string range $right 1 end]
				set prefix [string index $right 0]

				if {[string equal "-" $prefix]} {
					set right [string range $right 1 end]
					if {[string match $right $user]} {return 0}
				} elseif {[string equal "=" $prefix]} {
					set right [string range $right 1 end]
					foreach g $groups {
						if {[string match $right $g]} {return 0}
					}
				} elseif {[${ns}::flagcheck $flags $right]} {return 0}

			## Regular matching
			} elseif {[string equal "-" $prefix]} {
				set right [string range $right 1 end]
				if {[string match $right $user]} {return 1}
			} elseif {[string equal "=" $prefix]} {
				set right [string range $right 1 end]
				foreach g $groups {
					if {[string match $right $g]} {return 1}
				}
			} elseif {[${ns}::flagcheck $flags $right]} {return 1}
		}
		return 0
	}

	proc inviteuser {nick user group flags} {
		variable np
		variable ns
		variable ${np}::privchannel
		variable ${np}::invite_channels
		variable ${np}::invite_channels_rights

		if {![${np}::eventhandler precommand INVITEUSER [list $nick $user $group $flags]]} {return}

		## Invite the user to the defined channels.
		foreach chan $invite_channels {
			if {[${ns}::rightscheck $invite_channels_rights $user $group $flags]} {
				${ns}::invitechan $nick $chan
			}
		}
		foreach {chan rights} [array get privchannel] {
			if {[${ns}::rightscheck $rights $user $group $flags]} {
				${ns}::invitechan $nick $chan
			}
		}

		${np}::eventhandler postcommand INVITEUSER [list $nick $user $group $flags]

		return
	}

	proc invitechan {nick chan} {
		if {![validchan $chan] || ![botonchan $chan]} {
			putlog "\[ngBot\] Error :: Unable to invite \"$nick\" to \"$chan\", I'm not in the channel."
			} elseif {![botisop $chan] && ![botishalfop $chan]} {
			putlog "\[ngBot\] Error :: Unable to invite \"$nick\" to \"$chan\", I'm not opped."
		} else {
			putquick "INVITE $nick $chan"
		}
	}

	proc cmd_invite {nick host hand argv} {
		variable ns
		variable np
		variable ${np}::theme
		variable ${np}::binary
		variable ${np}::disable
		variable ${np}::location
		variable ${np}::announce

		set bad 0
		set argv [split $argv]
		if {[llength $argv] > 1} {
			set user [lindex $argv 0]
			set pass [lindex $argv 1]
			set result [exec $binary(PASSCHK) $user $pass $location(PASSWD)]
			set group ""; set flags ""

			if {[string equal $result "MATCH"]} {
				set output "$theme(PREFIX)$announce(MSGINVITE)"
				## Check the user file for the user's groups.
				if {![catch {set handle [open "$location(USERS)/$user" r]} error]} {
					set data [read $handle]
					close $handle
					foreach line [split $data "\n"] {
						switch -exact -- [lindex $line 0] {
							"FLAGS" {set flags [lindex $line 1]}
							"GROUP" {set group [lindex $line 1]}
						}
					}
				} else {
					putlog "\[ngBot\] Error :: Unable to open user file for \"$user\" ($error)."
				}

				${ns}::inviteuser $nick $user $group $flags

			} else {
				set bad 1
				set output "$theme(PREFIX)$announce(BADMSGINVITE)"
			}

			if {($bad == 1 && $disable(BADMSGINVITE) != 1) || ($bad == 0 && $disable(MSGINVITE) != 1)} {
				set output [${np}::replacevar $output "%u_ircnick" $nick]
				set output [${np}::replacevar $output "%u_name" $user]
				set output [${np}::replacevar $output "%u_host" $host]
				set output [${np}::replacevar $output "%g_name" $group]
				${np}::sndall "MSGINVITE" "DEFAULT" [${np}::replacebasic $output "INVITE"]
			}
		}
		return
	}
}
