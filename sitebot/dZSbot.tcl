#################################################################################
# dZSbot - ProjectZS-NG Sitebot                                                 #
#################################################################################
# - Displays information real-time events and stats for your glFTPd site.       #
# - Based on the original dZSbot written by dark0n3.                            #
#################################################################################

#################################################################################
# Read The Config Files                                                         #
#################################################################################
putlog "Launching dZSbot for project-zs-ng..."

set scriptpath [file dirname [info script]]

if {[catch {source $scriptpath/dZSbot.conf.defaults} error]} {
	putlog "dZSbot error: Unable to load dZSbot.conf.defaults ($error), cannot continue."
	putlog "dZSbot error: See FAQ for possible solutions/debugging options."
	die
}
if {[catch {source $scriptpath/dZSbot.vars} error]} {
	putlog "dZSbot error: Unable to load dZSbot.vars ($error), cannot continue."
	putlog "dZSbot error: See FAQ for possible solutions/debugging options."
	die
}
if {[catch {source $scriptpath/dZSbot.conf} error]} {
	putlog "dZSbot warning: Unable to load dZSbot.conf ($error), using defaults."
	putlog "dZSbot warning: If this is your first install, do: cp dZSbot.conf.dist dZSbot.conf"
}

#################################################################################
# Important Global Variables                                                    #
#################################################################################

set dzerror 0; set mpath ""
set defaultsection "DEFAULT"
set nuke(LASTDIR) ""
set nuke(LASTTYPE) ""
set nuke(SHOWN) 1
set variables(NUKE)   ""
set variables(UNNUKE) ""

interp alias {} istrue {} string is true -strict
interp alias {} isfalse {} string is false -strict

#################################################################################
# Bind Commands                                                                 #
#################################################################################

bind join -|- * ng_welcome
bind dcc n errorinfo errorinfo

proc errorinfo {args} {
	global errorInfo tcl_patchLevel tcl_platform
	putlog "--\[\002Error Info\002\]------------------------------------------"
	putlog "Tcl: $tcl_patchLevel"
	putlog "Box: $tcl_platform(os) $tcl_platform(osVersion)"
	putlog "Message:"
	foreach line [split $errorInfo "\n"] {putlog $line}
	putlog "--------------------------------------------------------"
}

proc bindcommands {cmdpre} {
	bind pub -|- [set cmdpre]bnc         ng_bnc
	bind pub -|- [set cmdpre]bw          ng_bw
	bind pub -|- [set cmdpre]bwdn        ng_bwdn
	bind pub -|- [set cmdpre]bwup        ng_bwup
	bind pub -|- [set cmdpre]df          ng_free
	bind pub -|- [set cmdpre]free        ng_free
	bind pub -|- [set cmdpre]dn          ng_leechers
	bind pub -|- [set cmdpre]down        ng_leechers
	bind pub -|- [set cmdpre]downloaders ng_leechers
	bind pub -|- [set cmdpre]leechers    ng_leechers
	bind pub -|- [set cmdpre]dupe        ng_search
	bind pub -|- [set cmdpre]help        ng_help
	bind pub -|- [set cmdpre]idle        ng_idlers
	bind pub -|- [set cmdpre]idlers      ng_idlers
	bind pub -|- [set cmdpre]inc         ng_incompletes
	bind pub -|- [set cmdpre]incomplete  ng_incompletes
	bind pub -|- [set cmdpre]incompletes ng_incompletes
	bind pub -|- [set cmdpre]new         ng_new
	bind pub -|- [set cmdpre]nukes       ng_nukes
	bind pub -|- [set cmdpre]search      ng_search
	bind pub -|- [set cmdpre]speed       ng_speed
	bind pub -|- [set cmdpre]unnukes     ng_unnukes
	bind pub -|- [set cmdpre]up          ng_uploaders
	bind pub -|- [set cmdpre]uploaders   ng_uploaders
	bind pub -|- [set cmdpre]uptime      ng_uptime
	bind pub -|- [set cmdpre]who         ng_who

	bind pub -|- [set cmdpre]gpad    [list ng_stats "-d" "-A"]
	bind pub -|- [set cmdpre]gpal    [list ng_stats "-u" "-A"]
	bind pub -|- [set cmdpre]gpwk    [list ng_stats "-d" "-W"]
	bind pub -|- [set cmdpre]gwpd    [list ng_stats "-u" "-W"]

	bind pub -|- [set cmdpre]alldn   [list ng_stats "-d" "-a"]
	bind pub -|- [set cmdpre]allup   [list ng_stats "-u" "-a"]
	bind pub -|- [set cmdpre]daydn   [list ng_stats "-d" "-t"]
	bind pub -|- [set cmdpre]dayup   [list ng_stats "-u" "-t"]
	bind pub -|- [set cmdpre]monthdn [list ng_stats "-d" "-m"]
	bind pub -|- [set cmdpre]monthup [list ng_stats "-u" "-m"]
	bind pub -|- [set cmdpre]wkdn    [list ng_stats "-d" "-w"]
	bind pub -|- [set cmdpre]wkup    [list ng_stats "-u" "-w"]
}

if {[istrue $bindnopre]} {
	bindcommands "!"
} elseif {![string equal "!" $cmdpre]} {
	catch {unbind pub -|- !bnc         ng_bnc}
	catch {unbind pub -|- !bw          ng_bw}
	catch {unbind pub -|- !bwdn        ng_bwdn}
	catch {unbind pub -|- !bwup        ng_bwup}
	catch {unbind pub -|- !df          ng_free}
	catch {unbind pub -|- !free        ng_free}
	catch {unbind pub -|- !dn          ng_leechers}
	catch {unbind pub -|- !down        ng_leechers}
	catch {unbind pub -|- !downloaders ng_leechers}
	catch {unbind pub -|- !leechers    ng_leechers}
	catch {unbind pub -|- !dupe        ng_search}
	catch {unbind pub -|- !help        ng_help}
	catch {unbind pub -|- !idle        ng_idlers}
	catch {unbind pub -|- !idlers      ng_idlers}
	catch {unbind pub -|- !inc         ng_incompletes}
	catch {unbind pub -|- !incomplete  ng_incompletes}
	catch {unbind pub -|- !incompletes ng_incompletes}
	catch {unbind pub -|- !new         ng_new}
	catch {unbind pub -|- !nukes       ng_nukes}
	catch {unbind pub -|- !search      ng_search}
	catch {unbind pub -|- !speed       ng_speed}
	catch {unbind pub -|- !unnukes     ng_unnukes}
	catch {unbind pub -|- !up          ng_uploaders}
	catch {unbind pub -|- !uploaders   ng_uploaders}
	catch {unbind pub -|- !uptime      ng_uptime}
	catch {unbind pub -|- !who         ng_who}

	catch {unbind pub -|- !gpad    [list ng_stats "-d" "-A"]}
	catch {unbind pub -|- !gpal    [list ng_stats "-u" "-A"]}
	catch {unbind pub -|- !gpwk    [list ng_stats "-d" "-W"]}
	catch {unbind pub -|- !gwpd    [list ng_stats "-u" "-W"]}
	catch {unbind pub -|- !alldn   [list ng_stats "-d" "-a"]}
	catch {unbind pub -|- !allup   [list ng_stats "-u" "-a"]}
	catch {unbind pub -|- !daydn   [list ng_stats "-d" "-t"]}
	catch {unbind pub -|- !dayup   [list ng_stats "-u" "-t"]}
	catch {unbind pub -|- !monthdn [list ng_stats "-d" "-m"]}
	catch {unbind pub -|- !monthup [list ng_stats "-u" "-m"]}
	catch {unbind pub -|- !wkdn    [list ng_stats "-d" "-w"]}
	catch {unbind pub -|- !wkup    [list ng_stats "-u" "-w"]}
}

## Bind the user defined command prefix.
bindcommands $cmdpre

#################################################################################
# Log Parsing for glFTPd and Login Logs                                         #
#################################################################################

proc denycheck {release} {
	global denypost
	foreach deny $denypost {
		if {[string match $deny $release]} {
			putlog "dZSbot: Post denied \"$release\" ($deny)."
			return 1
		}
	}
	return 0
}

proc eventcheck {section msgtype} {
	global disabletypes
	if {[info exists disabletypes($section)]} {
		foreach deny $disabletypes($section) {
			if {[string match $deny $msgtype]} {return 1}
		}
	}
	return 0
}

proc eventhandler {type event argv} {
	global $type
	set varname "$type\($event)"
	if {![info exists $varname]} {return 1}
	foreach script [set $varname] {
		if {[catch {set retval [eval $script $event $argv]} error]} {
			putlog "dZSbot error: Error evaluating the script \"$script\" for $varname ($error)."
		} elseif {[isfalse $retval]} {
			#putlog "dZSbot: The script \"$script\" for $varname returned false."
			return 0
		} elseif {![istrue $retval]} {
			putlog "dZSbot warning: The script \"$script\" for $varname must return a boolean value (0/FALSE or 1/TRUE)."
		}
	}
	return 1
}

proc readlogtimer {} {
	global dZStimer
	if {[catch {readlog} error]} {
		putlog "dZSbot error: Unable to read log data ($error)."
	}
	set dZStimer [utimer 1 readlogtimer]
}

proc readlog {} {
	global chanlist defaultsection disable glversion lastread loglist max_log_change msgreplace msgtypes variables
	set lines ""

	foreach {logtype logid logpath} $loglist {
		if {![file readable $logpath]} {
			putlog "dZSbot error: Unable to read log file \"$logpath\"."
			continue
		}
		## The regex pattern to use for the logfile
		switch -exact -- $logtype {
			0 {set regex {^.+ \d+:\d+:\d+ \d{4} (\S+): (.+)}}
			1 - 2 {set regex {^.+ \d+:\d+:\d+ \d{4} \[(.+)\] (.+)}}
			default {putlog "dZSbot error: Internal error, unknown log type ($logtype)."; continue}
		}
		## Read the log data
		set logsize [file size $logpath]
		if {$lastread($logid) < $logsize && ($logsize - $lastread($logid) - $max_log_change) < 0} {
			if {![catch {set handle [open $logpath r]} error]} {
				seek $handle $lastread($logid)
				while {![eof $handle]} {
					if {[gets $handle line] < 1} {continue}
					## Remove the date and time from the log line.
					if {[regexp $regex $line result event line]} {
						lappend lines $logtype $event $line
					} else {
						putlog "dZSbot warning: Invalid log line: $line"
					}
				}
				close $handle
			} else {
				putlog "dZSbot error: Unable to open log file \"$logpath\" ($error)."
			}
		}
		set lastread($logid) [file size $logpath]
	}

	foreach {type event line} $lines {
		## Login and sysop log specific parsing.
		if {$type == 1 && ![parselogin $line event line]} {
			putlog "dZSbot error: Unknown login.log line: $line"; continue
		} elseif {$type == 2 && ![parsesysop $line event line]} {
			set event "SYSOP"; set line [list $line]
		}
		## Check that the log line is a valid Tcl list.
		if {[catch {llength $line} error]} {
			putlog "dZSbot error: Invalid log line (not a valid list): $line"
			continue
		}
		## Invite users to public and private channels.
		if {[string equal $event "INVITE"]} {
			foreach {nick user group flags} $line {break}
			ng_inviteuser $nick $user $group $flags
		}
		if {[lsearch -exact $msgtypes(SECTION) $event] != -1} {
			set path [lindex $line 0]
			if {[denycheck $path]} {continue}
			set section [getsectionname $path]

			# Replace messages with custom messages
			foreach rep [array names msgreplace] {
				set rep [split $msgreplace($rep) ":"]
				if {[string equal $event [lindex $rep 0]]} {
					if {[string match -nocase [lindex $rep 1] $path]} {
						set event [lindex $rep 2]
					}
				}
			}
		} elseif {[lsearch -exact $msgtypes(DEFAULT) $event] != -1} {
			set section $defaultsection
		} else {
			putlog "dZSbot error: Undefined message type \"$event\", check \"msgtypes(SECTION)\" and \"msgtypes(DEFAULT)\" in the config."; continue
		}

		## If a pre-event script returns false, skip the announce.
		if {![eventhandler precommand $event [list $section $line]]} {continue}
		if {![info exists variables($event)]} {
			putlog "dZSbot error: \"variables($event)\" not defined in the config, type becomes \"DEFAULT\"."
			set event "DEFAULT"
		}
		if {([info exists disable($event)] && $disable($event) != 1) && ![eventcheck $section $event]} {
			sndall $event $section [ng_format $event $section $line]
			eventhandler postcommand $event [list $section $line]
		}
	}
	if {$glversion == 1} {
		launchnuke
	}
	return
}

proc parselogin {line eventvar datavar} {
	upvar $eventvar event $datavar data
	if {[regexp {^(.+@.+) \((.+)\): connection refused: .+$} $line result hostmask ip]} {
		set event "IPNOTADDED"
		set data [list $hostmask $ip]
	} elseif {[regexp {^(\S+): (.+@.+) \((.+)\): (.+)} $line result user hostmask ip error]} {
		switch -exact -- $error {
			"Bad user@host."    {set event "BADHOSTMASK"}
			"Banned user@host." {set event "BANNEDHOST"}
			"Deleted."          {set event "DELETED"}
			"Login failure."    {set event "BADPASSWORD"}
			default {return 0}
		}
		set data [list $user $hostmask $ip]
	} elseif {![regexp {^(\S+): (.+)$} $line result event data]} {
		return 0
	}
	return 1
}

proc parsesysop {line eventvar datavar} {
	upvar $eventvar event $datavar newdata
	set patterns [list \
		ADDUSER  {^'(.+)' added user '(.+)'\.$} \
		GADDUSER {^'(.+)' added user '(.+)' to group '(.+)'\.$} \
		CHGRPADD {^'(.+)': successfully added to '(.+)' by (.+)$} \
		CHGRPDEL {^'(.+)': successfully removed from '(.+)' by (.+)$} \
		ADDIP    {^'(.+)' added ip '(.+)' to '(.+)'$} \
		DELIP    {^'(.+)' .*removed ip '(.+)' from '(.+)'$} \
		READDED  {^'(.+)' readded '(.+)'\.$} \
		DELUSER  {^'(.+)' deleted user '(.+)'\.$} \
		PURGED   {^'(.+)' purged '(.+)'$} \
	]
	foreach {event pattern} $patterns {
		if {[llength [set data [regexp -inline -- $pattern $line]]]} {
			set newdata [lrange $data 1 end]
			return 1
		}
	}
	return 0
}

proc ng_format {event section line} {
	global announce defaultsection disable glversion mpath random sitename theme theme_fakes variables

	if {[string equal $event "NUKE"] || [string equal $event "UNNUKE"]} {
		if {$glversion == 1} {
			fuelnuke $event [lindex $line 0] $section $line
		} elseif {$glversion == 2} {
			launchnuke2 $event [lindex $line 0] $section [lrange $line 1 3] [lrange $line 4 end]
		} else {
			putlog "dZSbot error: Internal error, unknown glftpd version ($glversion)."
		}
		return ""
	}

	if {![info exists announce($event)]} {
		putlog "dZSbot error: \"announce($event)\" not set in theme, event becomes \"DEFAULT\"."
		set event "DEFAULT"
	}
	set vars $variables($event)

	## Random announce messages
	if {[string equal "random" [lindex $announce($event) 0]] && [string is digit -strict [lindex $announce($event) 1]]} {
		set output $random(${event}-[rand [lindex $announce($event) 1]])
	} else {
		set output $announce($event)
	}

	set output "$theme(PREFIX)$output"
	if {[string equal $section $defaultsection] && [info exists theme_fakes($event)]} {
		set section $theme_fakes($event)
	}
	set output [replacebasic $output $section]
	set cnt 0

	## Path filter parsing
	if {[string equal "%pf" [lindex $vars 0]]} {
		set output [replacepath $output $mpath [lindex $line 0]]
		set vars [lreplace $vars 0 0]
		incr cnt
	}

	## Replace variables and format the %loop cookies
	set loop 1
	foreach vari $vars {
		if {[llength $vari] > 1} {
			set cnt2 0
			set cnt3 1
			set values [lindex $line $cnt]
			set output2 ""
			foreach value $values {
				if {$cnt2 == 0} {
					append output2 "$announce(${event}_LOOP${loop})"
				}
				if {[string match "*speed" [lindex $vari $cnt2]]} {
					set output2 [replacevar $output2 [lindex $vari $cnt2] [format_speed $value $section]]
				} else {
					set output2 [replacevar $output2 [lindex $vari $cnt2] $value]
				}
				incr cnt2
				if {[string equal "" [lindex $vari $cnt2]]} {
					incr cnt3
					set cnt2 0
				}
			}
			set output2 [replacevar $output2 "%section" $section]
			set output2 [replacevar $output2 "%sitename" $sitename]
			set output2 [replacevar $output2 "%splitter" $theme(SPLITTER)]
			set output2 [trimtail $output2 $theme(SPLITTER)]
			set output [replacevar $output "%loop$loop" $output2]
			incr loop
		} else {
			if {[string match "*speed" $vari]} {
				set output [replacevar $output $vari [format_speed [lindex $line $cnt] $section]]
			} else {
				set output [replacevar $output $vari [lindex $line $cnt]]
			}
		}
		incr cnt
	}
	return $output
}

#################################################################################
# Nuke and Unnuke Handlers                                                      #
#################################################################################

proc fuelnuke {type path section line} {
	global hidenuke nuke

	if {$type == $nuke(LASTTYPE) && $path == $nuke(LASTDIR) && $nuke(SHOWN) == 0} {
		if {[lsearch -exact $hidenuke [lindex $line 2]] == -1} {
			append nuke(NUKEE) "\002[lindex $line 2]\002 (\002[lindex [lindex $line 3] 1]\002MB), "
		}
	} else {
		launchnuke
		if {[lsearch -exact $hidenuke [lindex $line 2]] == -1} {
			set nuke(TYPE) $type
			set nuke(PATH) $path
			set nuke(SECTION) $section
			set nuke(NUKER) [lindex $line 1]
			set nuke(NUKEE) "\002[lindex $line 2]\002 (\002[lindex [lindex $line 3] 1]\002MB) "
			set nuke(MULT) [lindex [lindex $line 3] 0]
			set nuke(REASON) [lindex $line 4]
			set nuke(SHOWN) 0
		}
	}
	set nuke(LASTTYPE) $type
	set nuke(LASTDIR) $path
}

proc launchnuke {} {
	global announce mpath nuke theme
	if {$nuke(SHOWN) == 1} {return 0}
	set nuke(NUKEE) [trimtail $nuke(NUKEE) $theme(SPLITTER)]

	set output "$theme(PREFIX)$announce($nuke(TYPE))"
	set output [replacebasic $output $nuke(SECTION)]
	set output [replacevar $output "%nuker" $nuke(NUKER)]
	set output [replacevar $output "%nukees" $nuke(NUKEE)]
	set output [replacevar $output "%type" $nuke(TYPE)]
	set output [replacevar $output "%multiplier" $nuke(MULT)]
	set output [replacevar $output "%reason" $nuke(REASON)]
	set output [replacepath $output $mpath $nuke(PATH)]
	sndall $nuke(TYPE) $nuke(SECTION) $output
	set nuke(SHOWN) 1
}

proc launchnuke2 {type path section info nukees} {
	global nuke hidenuke announce sitename theme mpath

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
			set nukee [replacevar $nukee "%u_name" [lindex $entry 0]]
			set nukee [replacevar $nukee "%size" $mb]
			append nuke(NUKEE) $nukee $theme(SPLITTER)
		}
	}
	set nuke(NUKEE) [trimtail $nuke(NUKEE) $theme(SPLITTER)]
	set output "$theme(PREFIX)$announce($nuke(TYPE))"
	set output [replacebasic $output $nuke(SECTION)]
	set output [replacevar $output "%nuker" $nuke(NUKER)]
	set output [replacevar $output "%nukees" $nuke(NUKEE)]
	set output [replacevar $output "%type" $nuke(TYPE)]
	set output [replacevar $output "%multiplier" $nuke(MULT)]
	set output [replacevar $output "%reason" $nuke(REASON)]
	set output [replacepath $output $mpath $nuke(PATH)]
	sndall $nuke(TYPE) $nuke(SECTION) $output
}

#################################################################################
# Format Size, Speed and Time Units                                             #
#################################################################################

proc format_duration {secs} {
	set duration ""
	foreach div {31536000 604800 86400 3600 60 1} mod {0 52 7 24 60 60} unit {y w d h m s} {
		set num [expr {$secs / $div}]
		if {$mod > 0} {set num [expr {$num % $mod}]}
		if {$num > 0} {lappend duration "\002$num\002$unit"}
	}
	if {[llength $duration]} {return [join $duration]} else {return "\0020\002s"}
}

proc format_speed {value section} {
	global speedmeasure speedthreshold theme
	switch -exact -- [string tolower $speedmeasure] {
		"mb" {
			set value [format "%.2f" [expr {$value / 1024.0}]]
			set type $theme(MB)
		}
		"kbit" {
			set value [expr {$value * 8}]
			set type $theme(KBIT)
		}
		"mbit" {
			set value [format "%.1f" [expr {$value * 8 / 1000.0}]]
			set type $theme(MBIT)
		}
		"autobit" {
			if {$value > $speedthreshold} {
				set value [format "%.1f" [expr {$value * 8 / 1000.0}]]
				set type $theme(MBIT)
			} else {
				set value [expr {$value * 8}]
				set type $theme(KBIT)
			}
		}
		"autobyte" {
			if {$value > $speedthreshold} {
				set value [format "%.2f" [expr {$value / 1024.0}]]
				set type $theme(MB)
			} else {
				set type $theme(KB)
			}
		}
		"disabled" {
			return $value
		}
		default {
			set type $theme(KB)
		}
	}
	return [themereplace [replacevar $type "%value" $value] $section]
}

proc to_mb {str} {
	set type [string index $str end]
	if {($type == "b" || $type == "B") && [string is alpha [string index $str end-1]]} {
		set type [string index $str end-1]
		set size [string range $str 0 end-2]
	} else { set size [string range $str 0 end-1] }

	switch -exact -- [string index $str end] {
		"m" - "M" {set factor 1}
		"g" - "G" {set factor 1000}
		"t" - "T" {set factor 1000000}
		default {return -1}
	}
	return [expr round($size*$factor)]
}

proc from_mb {str} {
	set units(1) "MB"
	set units(2) "GB"
	set units(3) "TB"
	set units(4) "PB"

	set unit 1
	while {$str >= 1000} {
		set str [expr {$str / 1000.00}]
		incr unit
	}

	return "[format %.1f $str]$units($unit)"
}

#################################################################################
# glFTPd Users and Groups                                                       #
#################################################################################

proc gluserids {} {
	global location
	set userlist ""
	if {![catch {set fh [open $location(PASSWD) r]} error]} {
		while {![eof $fh]} {
			## user:password:uid:gid:date:homedir:irrelevant
			set line [split [gets $fh] ":"]
			if {[llength $line] != 7} {continue}
			lappend userlist [lindex $line 2] [lindex $line 0]
		}
		close $fh
	} else {
		putlog "dZSbot error: Could not open passwd ($error)."
	}
	return $userlist
}

proc glgroupids {} {
	global location
	set grouplist ""
	if {![catch {set fh [open $location(GROUP) r]} error]} {
		while {![eof $fh]} {
			## group:description:gid:irrelevant
			set line [split [gets $fh] ":"]
			if {[llength $line] != 4} {continue}
			lappend grouplist [lindex $line 2] [lindex $line 0]
		}
		close $fh
	} else {
		putlog "dZSbot error: Could not open group ($error)."
	}
	return $grouplist
}

#################################################################################
# Replace Text                                                                  #
#################################################################################

proc replacebasic {message section} {
	global cmdpre sitename
	return [string map [list "%cmdpre" $cmdpre "%section" $section\
		"%sitename" $sitename "%bold" \002 "%uline" \037 "%color" \003] $message]
}

proc replacepath {message basepath path} {
	set path [split $path "/"]
	set pathitems [llength $path]
	set basepath [split $basepath "/"]
	set baseitems [llength $basepath]
	set relname [join [lrange $path [expr {$baseitems - 1}] end] "/"]

	set message [replacevar $message "%relname" $relname]
	set message [replacevar $message "%reldir" [lindex $path [expr {$pathitems - 1}]]]
	set message [replacevar $message "%path" [lindex $path [expr {$pathitems - 2}]]]
	return $message
}

proc replacevar {string cookie value} {
	global zeroconvert
	if {[string length $value] == 0 && [info exists zeroconvert($cookie)]} {
		set value $zeroconvert($cookie)
	}
	return [string map [list $cookie $value] $string]
}

proc trimtail {strsrc strrm} {
	if { [expr [string length $strsrc] - [string length $strrm]] == [string last $strrm $strsrc] } {
		return [string range $strsrc 0 [expr [string length $strsrc] - [string length $strrm] - 1]]
	}
	return $strsrc
}

#################################################################################
# Send Messages                                                                 #
#################################################################################

proc sndall {msgtype section text} {
	global chanlist splitter redirect

	if {[info exists redirect($msgtype)]} {
		set channels $redirect($msgtype)
	} elseif {[info exists chanlist($section)]} {
		set channels $chanlist($section)
	} else {
		putlog "dZSbot error: \"chanlist($section)\" not defined in the config."
		return
	}
	foreach chan $channels {
		sndone $chan $text $section
	}
}

proc sndone {chan text {section "none"}} {
	global splitter
	foreach line [split $text $splitter(CHAR)] {
		putquick "PRIVMSG $chan :[themereplace $line $section]"
	}
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

proc rightscheck {user group flags rights} {
	set retval 0
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
				if {[string match $right $group]} {return 0}
			} elseif {[flagcheck $flags $right]} {
				return 0
			}

		## Regular matching
		} elseif {[string equal "-" $prefix]} {
			set right [string range $right 1 end]
			if {[string match $right $user]} {set retval 1}
		} elseif {[string equal "=" $prefix]} {
			set right [string range $right 1 end]
			if {[string match $right $group]} {set retval 1}
		} elseif {[flagcheck $flags $right]} {
			set retval 1
		}
	}
	return $retval
}

proc ng_inviteuser {nick user group flags} {
	global invite_channels privchannel
	if {![eventhandler precommand INVITEUSER [list $nick $user $group]]} {return}

	## Invite the user to the defined channels.
	foreach chan $invite_channels {
		putquick "INVITE $nick :$chan"
	}
	foreach {chan rights} [array get privchannel] {
		if {[rightscheck $user $group $flags $rights]} {
			putquick "INVITE $nick :$chan"
		}
	}

	eventhandler postcommand INVITEUSER [list $nick $user $group $flags]
	return
}

proc ng_invite {nick host hand argv} {
	global location binary chanlist announce theme invite_channels disable

	if {[llength $argv] > 1} {
		set user [lindex $argv 0]
		set pass [lindex $argv 1]
		set result [exec $binary(PASSCHK) $user $pass $location(PASSWD)]
		set group ""; set flags ""

		if {[string equal $result "MATCH"]} {
			set output "$theme(PREFIX)$announce(MSGINVITE)"
			## Check the user file for the user's group.
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
				putlog "dZSbot error: Unable to open user file for \"$user\" ($error)."
			}
			ng_inviteuser $nick $user $group $flags
		} else {
			set output "$theme(PREFIX)$announce(BADMSGINVITE)"
		}

		if {$disable(MSGINVITE) != 1} {
			set output [replacevar $output "%u_ircnick" $nick]
			set output [replacevar $output "%u_name" $user]
			set output [replacevar $output "%u_host" $host]
			set output [replacevar $output "%g_name" $group]
			sndall "MSGINVITE" "DEFAULT" [replacebasic $output "INVITE"]
		}
	}
	return
}

#################################################################################
# Show Welcome Message                                                          #
#################################################################################

proc ng_welcome {nick uhost hand chan } {
	global announce disable chanlist sitename cmdpre
	if {$disable(WELCOME) == 1 || [isbotnick $nick]} {return}

	foreach c_chan $chanlist(WELCOME) {
		if {[string match -nocase $c_chan $chan]} {
			set output [replacebasic $announce(WELCOME) "WELCOME"]
			set output [replacevar $output "%ircnick" $nick]
			set output [themereplace $output "none"]
			puthelp "NOTICE $nick :$output"
		}
	}
	return
}

################################################################################
# Command Utilities                                                            #
################################################################################

proc checkchan {nick chan} {
	global disable lastbind mainchan
	if {$disable(TRIGINALLCHAN) == 1 && ![string equal -nocase $chan $mainchan]} {
		putlog "dZSbot: \002$nick\002 tried to use \002$lastbind\002 from an invalid channel ($chan)."
		return -code return
	}
}

proc getoptions {argv p_results p_other} {
	global default_results maximum_results
	upvar $p_results results $p_other other

	set results $default_results
	set other ""

	## Arguments should be "split" to form a valid list, this avoids any issues when performing
	## list operations (lindex,lrange,llength etc.) on "reserved" characters (i.e. curly braces).
	set argv [split $argv]
	set argc [llength $argv]

	for {set i 0} {$i < $argc} {incr i} {
		set arg [lindex $argv $i]
		if {[string equal -nocase "-h" $arg] || [string equal -nocase "-help" $arg]} {
			return 0
		} elseif {[string equal -nocase "-m" $arg] || [string equal -nocase "-max" $arg]} {
			set arg [lindex $argv [incr i]]
			if {![string is digit -strict $arg]} {return 0}
			set results [expr {$arg < $maximum_results ? $arg : $maximum_results}]
		} elseif {![string equal -length 1 "-" $arg] || $i == $argc} {
			## The lrange result must be joined to reverse the effect of the "split".
			set other [join [lrange $argv $i end]]
			return 1
		} else {
			return 0
		}
	}
	return 1
}

proc getsectionname {checkpath} {
	global defaultsection mpath paths sections
	set bestmatch 0
	set returnval $defaultsection

	foreach section $sections {
		if {![info exists paths($section)]} {
			putlog "dZSbot error: \"paths($section)\" is not defined in the config."
			continue
		}

		foreach path $paths($section) {
			## Compare the path length of the previous match (best match wins)
			if {[string match $path $checkpath] && [set pathlen [string length $path]] > $bestmatch} {
				set mpath $path
				set bestmatch $pathlen
				set returnval $section
			}
		}
	}
	return $returnval
}

proc getsectionpath {getsection} {
	global paths sections
	foreach section $sections {
		if {![info exists paths($section)]} {
			putlog "dZSbot error: \"paths($section)\" not set in config."
		} elseif {[string equal -nocase $getsection $section]} {
			return [list $section $paths($section)]
		}
	}
	return ""
}

################################################################################
# General Commands                                                             #
################################################################################

proc ng_bnc {nick uhost hand chan arg} {
	global announce binary bnc errorCode theme
	checkchan $nick $chan
	if {![istrue $bnc(ENABLED)]} {return}

	set output "$theme(PREFIX)$announce(BNC)"
	sndone $nick [replacebasic $output "BNC"]

	set num 0
	foreach entry $bnc(LIST) {
		set entrysplit [split $entry ":"]
		if {[llength $entrysplit] != 3} {
			putlog "dZSbot error: Invalid bouncer line \"$entry\" (check bnc(LIST))."
			continue
		}
		incr num; set ping "N/A"
		foreach {desc ip port} $entrysplit {break}

		if {[istrue $bnc(PING)]} {
			if {[catch {exec $binary(PING) -c 1 -t $bnc(TIMEOUT) $ip} reply]} {
				set output "$theme(PREFIX)$announce(BNC_PING)"
				set output [replacevar $output "%num" $num]
				set output [replacevar $output "%desc" $desc]
				set output [replacevar $output "%ip" $ip]
				set output [replacevar $output "%port" $port]
				sndone $nick [replacebasic $output "BNC"]
				continue
			}
			set reply [lindex [split $reply "\n"] 1]
			if {[regexp {.+time=(\S+) ms} $reply reply ping]} {
				set ping [format "%.1f" $ping]
			} else {
				putlog "dZSbot error: Unable to parse ping reply \"$reply\", please report to pzs-ng developers."
			}
		}

		set response [clock clicks -milliseconds]
		if {[istrue $bnc(SECURE)]} {
			set status [catch {exec $binary(CURL) --connect-timeout $bnc(TIMEOUT) --ftp-ssl --insecure -u $bnc(USER):$bnc(PASS) ftp://$ip:$port 2>@stdout} reply]
		} else {
			set status [catch {exec $binary(CURL) --connect-timeout $bnc(TIMEOUT) -u $bnc(USER):$bnc(PASS) ftp://$ip:$port 2>@stdout} reply]
		}
		set response [expr {[clock clicks -milliseconds] - $response}]

		if {!$status} {
			set output "$theme(PREFIX)$announce(BNC_ONLINE)"
			set output [replacevar $output "%ping" $ping]
			set output [replacevar $output "%response" $response]
		} else {
			set error "unknown error"
			## Check curl's exit code (stored in errorCode).
			if {[string equal "CHILDSTATUS" [lindex $errorCode 0]]} {
				set code [lindex $errorCode 2]
				switch -exact -- $code {
					6 {set error "couldn't resolve host"}
					7 {set error "connection refused"}
					9 - 10 {set error "couldn't login"}
					28 {set error "timed out"}
					default {putlog "dZSbot error: Unknown curl exit code \"$code\", check the \"exit codes\" section of curl's man page."}
				}
			} else {
				## If the first list item in errorCode is not "CHILDSTATUS",
				## Tcl was unable to execute the binary.
				putlog "dZSbot error: Unable to execute curl ($reply)."
			}
			set output "$theme(PREFIX)$announce(BNC_OFFLINE)"
			set output [replacevar $output "%error" $error]
		}

		set output [replacevar $output "%num" $num]
		set output [replacevar $output "%desc" $desc]
		set output [replacevar $output "%ip" $ip]
		set output [replacevar $output "%port" $port]
		sndone $nick [replacebasic $output "BNC"]
	}
	return
}

proc ng_help {nick uhost hand chan arg} {
	global scriptpath sections
	checkchan $nick $chan

	set file "$scriptpath/dZSbot.help"
	if {![file readable $file]} {
		putlog "dZSbot error: The \"dZSbot.help\" file is missing, please check your install."
		puthelp "PRIVMSG $nick :Unable to find help file, please contact a siteop."
		return 0
	}

	set helpfile [open $file r]
	set helpdb [read $helpfile]
	close $helpfile
	foreach line [split $helpdb "\n"] {
		set line [themereplace [replacebasic $line "HELP"] "none"]
		puthelp "PRIVMSG $nick :$line"
	}
	puthelp "PRIVMSG $nick :Valid sections are: [join [lsort -ascii $sections] {, }]"
	return
}

proc ng_free {nick uhost hand chan arg} {
	global binary announce device theme dev_max_length
	checkchan $nick $chan

	set devices(0) ""; set free 0.0; set used 0.0
	set total 0.0; set num 0; set perc 0.0
	array set tmpdev [array get device]

	set i 0
	foreach line [split [exec $binary(DF) "-Ph"] "\n"] {
		set line [string map {, .} $line]
		foreach dev [array names tmpdev] {
			if {[string match [lindex $line 0] [lindex $tmpdev($dev) 0]]} {
				set dev_total [to_mb [lindex $line 1]]
				set dev_used [to_mb [lindex $line 2]]
				set dev_free [to_mb [lindex $line 3]]
				set dev_percent [format "%.1f" [expr (double($dev_used)/double($dev_total)) * 100]]
				set tmp [replacevar $announce(FREE-DEV) "%total" [from_mb $dev_total]]
				set tmp [replacevar $tmp "%used" [from_mb $dev_used]]
				set tmp [replacevar $tmp "%free" [from_mb $dev_free]]
				set tmp [replacevar $tmp "%percentage" $dev_percent]
				set tmp [replacevar $tmp "%section" [lrange $device($dev) 1 end]]

				if {[info exists dev_max_length] && $dev_max_length &&
					[expr [string length $devices($i)] + [string length $tmp]] > $dev_max_length} {
					incr i
					set devices($i) ""
				}
				append devices($i) $tmp

				set total [expr {$total + double($dev_total)}]
				set used [expr {$used + double($dev_used)}]
				set free [expr {$free + double($dev_free)}]
				set perc [expr {$perc + double($dev_percent)}]
				incr num
				unset tmpdev($dev)
			}
		}
	}
	if {[llength [array names tmpdev]]} {
		foreach {id dev} [array get tmpdev] { append tmpstr "$dev " }
		putlog "dZSbot warning: The following devices had no matching \"df -Ph\" entry: $tmpstr"
	}

	set totalgb [from_mb $total]
	set usedgb [from_mb $used]
	set freegb [from_mb $free]

	set o 0
	while {$o < $i + 1} {
		set output "$theme(PREFIX)$announce(FREE)"
		set output [replacevar $output "%total" $totalgb]
		set output [replacevar $output "%used" $usedgb]
		set output [replacevar $output "%free" $freegb]
		set output [replacevar $output "%percentage" [expr {$num > 0 ? round($perc/$num) : 0}]]
		set output [replacevar $output "%devices" $devices($o)]
		sndone $chan [replacebasic $output "FREE"]
		incr o
	}
	return
}

proc ng_incompletes {nick uhost hand chan arg } {
	global sitename binary
	checkchan $nick $chan

	foreach line [split [exec $binary(INCOMPLETE)] "\n"] {
		if {![info exists newline($line)]} {
			set newline($line) 0
		} else { set newline($line) [expr $newline($line) + 1] }
		puthelp "PRIVMSG $nick :$line\003$newline($line)"
	}
	return
}

proc ng_stats {type time nick uhost hand chan argv} {
	global binary statsection location
	checkchan $nick $chan

	set sect 0
	set section [lindex $argv 0]
	if {[string length $section] != 0} {
		set error 1
		set sections ""
		foreach sectnumb [array names statsection] {
			if {[string equal -nocase $statsection($sectnumb) $section]} {
				set sect $sectnumb
				set error 0
				break
			}
			lappend sections $statsection($sectnumb)
		}
		if {$error} {
			puthelp "PRIVMSG $nick :Invalid section, sections: [join [lsort -ascii $sections] {, }]"
			return
		}
	}

	foreach line [split [exec $binary(STATS) -r $location(GLCONF) $type $time -s $sect] "\n"] {
		if {![info exists newline($line)]} { set newline($line) 0
		} else { set newline($line) [expr $newline($line) + 1] }
		puthelp "PRIVMSG $nick :$line\003$newline($line)"
	}
	puthelp "PRIVMSG $nick :------------------------------------------------------------------------"
	return
}

proc ng_uptime {nick uhost hand chan argv} {
	global announce binary theme uptime
	checkchan $nick $chan

	if {[catch {exec $binary(UPTIME)} reply]} {
		putlog "dZSbot error: Unable to execute uptime ($reply)."
	}
	## The linux 'uptime' pads the output with spaces, so we'll trim it.
	regsub -all {\s+} $reply { } reply
	if {![regexp {.+ up (.+), (.+) users?, load averages?: (.+)} $reply reply time users load]} {
		set load "N/A"; set time "N/A"; set users "N/A"
		putlog "dZSbot error: Unable to parse uptime reply \"$reply\", please report to pzs-ng developers."
	}
	set eggup [format_duration [expr {[clock seconds] - $uptime}]]

	set output "$theme(PREFIX)$announce(UPTIME)"
	set output [replacevar $output "%eggdrop" $eggup]
	set output [replacevar $output "%time" $time]
	set output [replacevar $output "%users" $users]
	set output [replacevar $output "%load" $load]
	sndone $chan [replacebasic $output "UPTIME"]
	return
}

################################################################################
# Latest Dirs/Nukes Commands                                                   #
################################################################################

proc ng_new {nick uhost hand chan argv} {
	global announce binary defaultsection lastbind location sections theme
	checkchan $nick $chan

	if {![getoptions $argv results section]} {
		## By displaying the command syntax in the channel (opposed to private message), we can inform others
		## at the same time. There's this recurring phenomena, every time a user types an "uncommon" command, half
		## a dozen others will as well...to learn about this command. So, let's kill a few idiots with one stone.
		puthelp "PRIVMSG $chan :\002Usage:\002 $lastbind \[-max <num>\] \[section\]"
		return
	}
	if {[string equal "" $section]} {
		set section $defaultsection
		set lines [exec $binary(SHOWLOG) -l -m $results -r $location(GLCONF)]
	} else {
		if {[set sectiondata [getsectionpath $section]] == ""} {
			puthelp "PRIVMSG $nick :Invalid section, sections: [join [lsort -ascii $sections] {, }]"
			return
		}
		foreach {section sectionpath} $sectiondata {break}
		set lines [exec $binary(SHOWLOG) -f -l -m $results -p $sectionpath -r $location(GLCONF)]
	}

	## Retrieve a list of UIDs/users and GIDs/groups
	array set uid [gluserids]
	array set gid [glgroupids]

	set output "$theme(PREFIX)$announce(NEW)"
	set output [replacevar $output "%section" $section]
	sndone $nick [replacebasic $output "NEW"]
	set body "$theme(PREFIX)$announce(NEW_BODY)"
	set num 0

	foreach line [split $lines "\n"] {
		## Format: status|uptime|uploader|group|files|kilobytesdirname
		if {[llength [set line [split $line "|"]]] != 7} {continue}
		foreach {status ctime userid groupid files kbytes dirname} $line {break}

		## If array get returns "", zeroconvert will replace the value with NoOne/NoGroup.
		set user [lindex [array get uid $userid] 1]
		set group [lindex [array get gid $groupid] 1]

		set output [replacevar $body "%num" [format "%02d" [incr num]]]
		set age [lrange [format_duration [expr {[clock seconds] - $ctime}]] 0 1]
		set output [replacevar $output "%age" $age]
		set output [replacevar $output "%date" [clock format $ctime -format "%m-%d-%y"]]
		set output [replacevar $output "%time" [clock format $ctime -format "%H:%M:%S"]]
		set output [replacevar $output "%u_name" $user]
		set output [replacevar $output "%g_name" $group]
		set output [replacevar $output "%files" $files]
		set output [replacevar $output "%mbytes" [format "%.1f" [expr {$kbytes / 1024.0}]]]
		## TODO (neoxed): use replacepath instead
		set path [lrange [file split [string trim [file dirname $dirname] "/"]] 1 end]
		set output [replacevar $output "%path" [join $path "/"]]
		set output [replacevar $output "%reldir" [file tail $dirname]]
		sndone $nick [replacebasic $output "NEW"]
	}

	if {!$num} {
		set output "$theme(PREFIX)$announce(NEW_NONE)"
		sndone $nick [replacebasic $output "NEW"]
	}
	return
}

proc ng_nukes {nick uhost hand chan argv} {
	global announce binary defaultsection lastbind location sections theme
	checkchan $nick $chan

	if {![getoptions $argv results section]} {
		puthelp "PRIVMSG $chan :\002Usage:\002 $lastbind \[-max <num>\] \[section\]"
		return
	}
	if {[string equal "" $section]} {
		set section $defaultsection
		set lines [exec $binary(SHOWLOG) -n -m $results -r $location(GLCONF)]
	} else {
		if {[set sectiondata [getsectionpath $section]] == ""} {
			puthelp "PRIVMSG $nick :Invalid section, sections: [join [lsort -ascii $sections] {, }]"
			return
		}
		foreach {section sectionpath} $sectiondata {break}
		set lines [exec $binary(SHOWLOG) -f -n -m $results -p $sectionpath -r $location(GLCONF)]
	}

	set output "$theme(PREFIX)$announce(NUKES)"
	set output [replacevar $output "%section" $section]
	sndone $nick [replacebasic $output "NUKES"]
	set body "$theme(PREFIX)$announce(NUKES_BODY)"
	set num 0

	foreach line [split $lines "\n"] {
		## Format: status|nuketime|nuker|unnuker|nukee|multiplier|reason|kilobytes|dirname
		if {[llength [set line [split $line "|"]]] != 9} {continue}
		foreach {status nuketime nuker unnuker nukee multiplier reason kbytes dirname} $line {break}

		set output [replacevar $body "%num" [format "%02d" [incr num]]]
		set output [replacevar $output "%date" [clock format $nuketime -format "%m-%d-%y"]]
		set output [replacevar $output "%time" [clock format $nuketime -format "%H:%M:%S"]]
		set output [replacevar $output "%nuker" $nuker]
		set output [replacevar $output "%nukee" $nukee]
		set output [replacevar $output "%multiplier" $multiplier]
		set output [replacevar $output "%reason" $reason]
		set output [replacevar $output "%mbytes" [format "%.1f" [expr {$kbytes / 1024.0}]]]
		## TODO (neoxed): use replacepath instead
		set path [lrange [file split [string trim [file dirname $dirname] "/"]] 1 end]
		set output [replacevar $output "%path" [join $path "/"]]
		set output [replacevar $output "%reldir" [file tail $dirname]]
		sndone $nick [replacebasic $output "NUKES"]
	}

	if {!$num} {
		set output "$theme(PREFIX)$announce(NUKES_NONE)"
		sndone $nick [replacebasic $output "NUKES"]
	}
	return
}

proc ng_search {nick uhost hand chan argv} {
	global announce binary defaultsection lastbind location search_chars theme
	checkchan $nick $chan

	if {![getoptions $argv results pattern] || [string equal "" $pattern]} {
		puthelp "PRIVMSG $chan :\002Usage:\002 $lastbind \[-max <num>\] <pattern>"
		return
	}
	if {$search_chars > 0 && [regexp -all {[a-zA-Z0-9]} $pattern] < $search_chars} {
		puthelp "PRIVMSG $nick :The search pattern must at be at least \002$search_chars\002 alphanumeric characters."
		return
	}
	## Retrieve matching dirlog entries
	regsub -all {[\s\*]+} "*${pattern}*" {*} pattern
	set lines [exec $binary(SHOWLOG) -l -s -m $results -p $pattern -r $location(GLCONF)]

	## Retrieve a list of UIDs/users and GIDs/groups
	array set uid [gluserids]
	array set gid [glgroupids]

	set output "$theme(PREFIX)$announce(SEARCH)"
	set output [replacevar $output "%pattern" $pattern]
	sndone $nick [replacebasic $output "SEARCH"]
	set body "$theme(PREFIX)$announce(SEARCH_BODY)"
	set num 0

	foreach line [split $lines "\n"] {
		## Format: status|uptime|uploader|group|files|kilobytes|dirname
		if {[llength [set line [split $line "|"]]] != 7} {continue}
		foreach {status ctime userid groupid files kbytes dirname} $line {break}

		## If array get returns "", zeroconvert will replace the value with NoOne/NoGroup.
		set user [lindex [array get uid $userid] 1]
		set group [lindex [array get gid $groupid] 1]

		set output [replacevar $body "%num" [format "%02d" [incr num]]]
		set age [lrange [format_duration [expr {[clock seconds] - $ctime}]] 0 1]
		set output [replacevar $output "%age" $age]
		set output [replacevar $output "%date" [clock format $ctime -format "%m-%d-%y"]]
		set output [replacevar $output "%time" [clock format $ctime -format "%H:%M:%S"]]
		set output [replacevar $output "%u_name" $user]
		set output [replacevar $output "%g_name" $group]
		set output [replacevar $output "%files" $files]
		set output [replacevar $output "%mbytes" [format "%.1f" [expr {$kbytes / 1024.0}]]]
		## TODO (neoxed): use replacepath instead
		set path [lrange [file split [string trim [file dirname $dirname] "/"]] 1 end]
		set output [replacevar $output "%path" [join $path "/"]]
		set output [replacevar $output "%reldir" [file tail $dirname]]
		sndone $nick [replacebasic $output "SEARCH"]
	}

	if {!$num} {
		set output "$theme(PREFIX)$announce(SEARCH_NONE)"
		sndone $nick [replacebasic $output "SEARCH"]
	}
	return
}

proc ng_unnukes {nick uhost hand chan argv} {
	global announce binary defaultsection lastbind location sections theme
	checkchan $nick $chan

	if {![getoptions $argv results section]} {
		puthelp "PRIVMSG $chan :\002Usage:\002 $lastbind \[-max <num>\] \[section\]"
		return
	}
	if {[string equal "" $section]} {
		set section $defaultsection
		set lines [exec $binary(SHOWLOG) -u -m $results -r $location(GLCONF)]
	} else {
		if {[set sectiondata [getsectionpath $section]] == ""} {
			puthelp "PRIVMSG $nick :Invalid section, sections: [join [lsort -ascii $sections] {, }]"
			return
		}
		foreach {section sectionpath} $sectiondata {break}
		set lines [exec $binary(SHOWLOG) -f -u -m $results -p $sectionpath -r $location(GLCONF)]
	}

	set output "$theme(PREFIX)$announce(UNNUKES)"
	set output [replacevar $output "%section" $section]
	sndone $nick [replacebasic $output "UNNUKES"]
	set body "$theme(PREFIX)$announce(UNNUKES_BODY)"
	set num 0

	foreach line [split $lines "\n"] {
		## Format: status|nuketime|nuker|unnuker|nukee|multiplier|reason|kilobytes|dirname
		if {[llength [set line [split $line "|"]]] != 9} {continue}
		foreach {status nuketime nuker unnuker nukee multiplier reason kbytes dirname} $line {break}

		set output [replacevar $body "%num" [format "%02d" [incr num]]]
		set output [replacevar $output "%date" [clock format $nuketime -format "%m-%d-%y"]]
		set output [replacevar $output "%time" [clock format $nuketime -format "%H:%M:%S"]]
		set output [replacevar $output "%nuker" $nuker]
		set output [replacevar $output "%unnuker" $unnuker]
		set output [replacevar $output "%nukee" $nukee]
		set output [replacevar $output "%multiplier" $multiplier]
		set output [replacevar $output "%reason" $reason]
		set output [replacevar $output "%mbytes" [format "%.1f" [expr {$kbytes / 1024.0}]]]
		## TODO (neoxed): use replacepath instead
		set path [lrange [file split [string trim [file dirname $dirname] "/"]] 1 end]
		set output [replacevar $output "%path" [join $path "/"]]
		set output [replacevar $output "%reldir" [file tail $dirname]]
		sndone $nick [replacebasic $output "UNNUKES"]
	}

	if {!$num} {
		set output "$theme(PREFIX)$announce(UNNUKES_NONE)"
		sndone $nick [replacebasic $output "UNNUKES"]
	}
	return
}

################################################################################
# Online Stats Commands                                                        #
################################################################################

proc ng_bw {nick uhost hand chan argv} {
	global binary announce speed theme speedmeasure speedthreshold
	checkchan $nick $chan

	set output "$theme(PREFIX)$announce(BW)"
	set raw [exec $binary(WHO) --nbw]
	set upper [format "%.0f" [expr [lindex $raw 1] * 100 / $speed(INCOMING)]]
	set dnper [format "%.0f" [expr [lindex $raw 3] *100 / $speed(OUTGOING)]]
	set totalper [format "%.0f" [expr [lindex $raw 5] * 100 / ( $speed(INCOMING) + $speed(OUTGOING) )]]

	set up [format_speed [lindex $raw 1] "none"]
	set dn [format_speed [lindex $raw 3] "none"]
	set totalspeed [format_speed [lindex $raw 5] "none"]

	set output [replacevar $output "%uploads" [lindex $raw 0]]
	set output [replacevar $output "%upspeed" $up]
	set output [replacevar $output "%downloads" [lindex $raw 2]]
	set output [replacevar $output "%dnspeed" $dn]
	set output [replacevar $output "%transfers" [lindex $raw 4]]
	set output [replacevar $output "%totalspeed" $totalspeed]
	set output [replacevar $output "%idlers" [lindex $raw 6]]
	set output [replacevar $output "%active" [lindex $raw 7]]
	set output [replacevar $output "%totallogins" [lindex $raw 8]]
	set output [replacevar $output "%maxusers" [lindex $raw 9]]

	set output [replacevar $output "%uppercent" $upper]
	set output [replacevar $output "%dnpercent" $dnper]
	set output [replacevar $output "%totalpercent" $totalper]
	sndone $chan [replacebasic $output "BW"]
	return
}

proc ng_bwdn {nick uhost hand chan argv} {
	global binary announce speed theme
	checkchan $nick $chan

	set output "$theme(PREFIX)$announce(BWDN)"
	set raw [exec $binary(WHO) --nbw]
	set upper [format "%.1f" [expr 100 * ([lindex $raw 1] / $speed(INCOMING))]]
	set dnper [format "%.1f" [expr 100 * ([lindex $raw 3] / $speed(OUTGOING))]]
	set totalper [format "%.0f" [expr [lindex $raw 5] / ( $speed(INCOMING) + $speed(OUTGOING))]]

	set output [replacevar $output "%uploads" [lindex $raw 0]]
	set output [replacevar $output "%upspeed" [format_speed [lindex $raw 1] "none"]]
	set output [replacevar $output "%downloads" [lindex $raw 2]]
	set output [replacevar $output "%dnspeed" [format_speed [lindex $raw 3] "none"]]
	set output [replacevar $output "%transfers" [lindex $raw 4]]
	set output [replacevar $output "%totalspeed" [format_speed [lindex $raw 5] "none"]]
	set output [replacevar $output "%idlers" [lindex $raw 6]]
	set output [replacevar $output "%active" [lindex $raw 7]]
	set output [replacevar $output "%totallogins" [lindex $raw 8]]
	set output [replacevar $output "%maxusers" [lindex $raw 9]]

	set output [replacevar $output "%uppercent" $upper]
	set output [replacevar $output "%dnpercent" $dnper]
	set output [replacevar $output "%totalpercent" $totalper]
	sndone $chan [replacebasic $output "BW"]
	return
}

proc ng_bwup {nick uhost hand chan argv} {
	global binary announce speed theme
	checkchan $nick $chan

	set output "$theme(PREFIX)$announce(BWUP)"
	set raw [exec $binary(WHO) --nbw]
	set upper [format "%.1f" [expr 100 * ([lindex $raw 1] / $speed(INCOMING))]]
	set dnper [format "%.1f" [expr 100 * ([lindex $raw 3] / $speed(OUTGOING))]]
	set totalper [format "%.0f" [expr 100 * ([lindex $raw 5] / ( $speed(INCOMING) + $speed(OUTGOING)))]]

	set output [replacevar $output "%uploads" [lindex $raw 0]]
	set output [replacevar $output "%upspeed" [format_speed [lindex $raw 1] "none"]]
	set output [replacevar $output "%downloads" [lindex $raw 2]]
	set output [replacevar $output "%dnspeed" [format_speed [lindex $raw 3] "none"]]
	set output [replacevar $output "%transfers" [lindex $raw 4]]
	set output [replacevar $output "%totalspeed" [format_speed [lindex $raw 5] "none"]]
	set output [replacevar $output "%idlers" [lindex $raw 6]]
	set output [replacevar $output "%active" [lindex $raw 7]]
	set output [replacevar $output "%totallogins" [lindex $raw 8]]
	set output [replacevar $output "%maxusers" [lindex $raw 9]]

	set output [replacevar $output "%uppercent" $upper]
	set output [replacevar $output "%dnpercent" $dnper]
#	set output [replacevar $output "%totalpercent" $totalper]
	sndone $chan [replacebasic $output "BW"]
}

proc ng_idlers {nick uhost hand chan argv} {
	global binary announce speed minidletime theme
	checkchan $nick $chan

	set output "$theme(PREFIX)$announce(IDLE)"
	sndone $chan [replacebasic $output "IDLE"]

	set raw [exec $binary(WHO) "--raw"]
	set count 0; set total 0.0

	foreach line [split $raw "\n"] {
		if {[string equal "USER" [lindex $line 0]] && [string equal "ID" [lindex $line 4]]} {
			set user  [lindex $line 2]
			set group [lindex $line 3]
			set idletime [lindex $line 5]
			set tagline [lindex $line 6]
			set since [lindex $line 7]

			if {$idletime > $minidletime} {
				set output [replacevar "$theme(PREFIX)$announce(USERIDLE)" "%u_name" $user]
				set output [replacevar $output "%g_name" $group]
				set output [replacevar $output "%idletime" [format_duration $idletime]]
				set output [replacevar $output "%tagline" $tagline]
				set output [replacevar $output "%since" $since]
				sndone $chan [replacebasic $output "IDLE"]
				incr count
			}
		}
	}
	set output [replacevar "$theme(PREFIX)$announce(TOTIDLE)" "%count" $count]
	sndone $chan [replacebasic $output "IDLE"]
	return
}

proc ng_leechers {nick uhost hand chan argv} {
	global binary announce speed theme
	checkchan $nick $chan

	set output "$theme(PREFIX)$announce(LEECH)"
	sndone $chan [replacebasic $output "LEECH"]

	set raw [exec $binary(WHO) "--raw"]
	set count 0; set total 0.0

	foreach line [split $raw "\n"] {
		if {[string equal "USER" [lindex $line 0]] && [string equal "DN" [lindex $line 4]]} {
			set user  [lindex $line 2]
			set group [lindex $line 3]
			set uspeed [replacevar [lindex $line 5] "KB/s" ""]
			set tagline [lindex $line 6]
			set since [lindex $line 7]
			set filename [lindex $line 8]
			set per [format "%.2f%%" [expr double($uspeed) * 100 / double($speed(OUTGOING))]]
			set fper [lindex $line 9]
			set output [replacevar "$theme(PREFIX)$announce(USER)" "%u_name" $user]
			set output [replacevar $output "%g_name" $group]
			set output [replacevar $output "%fper"	$fper]
			set output [replacevar $output "%speed" [format_speed $uspeed "none"]]
			set output [replacevar $output "%per" $per]
			set output [replacevar $output "%tagline" $tagline]
			set output [replacevar $output "%since" $since]
			set output [replacevar $output "%filename" $filename]
			sndone $chan [replacebasic $output "LEECH"]
			incr count
			set total [expr {$total + $uspeed}]
		}
	}
	set per [format "%.1f" [expr double($total) * 100 / double($speed(OUTGOING)) ]]

	set output [replacevar "$theme(PREFIX)$announce(TOTUPDN)" "%type" "Leechers:"]
	set output [replacevar $output "%count" $count]
	set output [replacevar $output "%total" [format_speed $total "none"]]
	set output [replacevar $output "%per" $per]
	sndone $chan [replacebasic $output "LEECH"]
	return
}

proc ng_speed {nick uhost hand chan argv} {
	global binary announce theme disable
	checkchan $nick $chan

	set line ""
	if {$disable(ALTWHO) != 1} {
		set output "$theme(PREFIX)$announce(SPEEDERROR)"
		foreach line [split [exec $binary(WHO) --raw [lindex $argv 0]] "\n"] {
			set action [lindex $line 4]
			if {$action == "DN"} {
				set output "$theme(PREFIX)$announce(SPEEDDN)"
				set output [replacevar $output "%dnspeed" [format_speed [lindex $line 5] "none"]]
				set output [replacevar $output "%dnpercent" [lindex $line 9]]
			} elseif {$action == "UP"} {
				set output "$theme(PREFIX)$announce(SPEEDUP)"
				set output [replacevar $output "%upspeed" [format_speed [lindex $line 5] "none"]]
				set output [replacevar $output "%uppercent" [lindex $line 9]]
			} elseif {$action == "ID"} {
				set output "$theme(PREFIX)$announce(SPEEDID)"
				set output [replacevar $output "%idletime" [format_duration [lindex $line 5]]]
			}
			set output [replacevar $output "%u_name" [lindex $line 2]]
			set output [replacevar $output "%g_name" [lindex $line 3]]
			set output [replacevar $output "%tagline" [lindex $line 6]]
			set output [replacevar $output "%timeonline" [lindex $line 7]]
			set output [replacevar $output "%f_name" [lindex $line 8]]
			sndone $chan [replacebasic $output "SPEED"]
		}
	} else {
		set base_output "$theme(PREFIX)$announce(DEFAULT)"
		foreach line [split [exec $binary(WHO) [lindex $argv 0]] "\n"] {
			set output [replacevar $base_output "%msg" $line]
			sndone $chan [replacebasic $output "SPEED"]
		}
	}

	if {[string equal "" $line]} {
		set output "$theme(PREFIX)$announce(SPEEDERROR)"
		set output [replacevar $output "%msg" "User not online."]
		sndone $chan [replacebasic $output "SPEED"]
	}
	return
}

proc ng_uploaders {nick uhost hand chan argv} {
	global binary announce speed theme
	checkchan $nick $chan

	set output "$theme(PREFIX)$announce(UPLOAD)"
	sndone $chan [replacebasic $output "UPLOAD"]

	set raw [exec $binary(WHO) "--raw"]
	set count 0; set total 0.0

	foreach line [split $raw "\n"] {
		if {[string equal "USER" [lindex $line 0]] && [string equal "UP" [lindex $line 4]]} {
			set user  [lindex $line 2]
			set group [lindex $line 3]
			set uspeed [replacevar [lindex $line 5] "KB/s" ""]
			set tagline [lindex $line 6]
			set since [lindex $line 7]
			set filename [lindex $line 8]
			set progress [lindex $line 9]
			set per [format "%.2f%%" [expr double($uspeed) * 100 / double($speed(INCOMING))]]
			set output [replacevar "$theme(PREFIX)$announce(USER)" "%u_name" $user]
			set output [replacevar $output "%g_name" $group]
			set output [replacevar $output "%fper" $progress]
			set output [replacevar $output "%speed" [format_speed $uspeed "none"]]
			set output [replacevar $output "%per" $per]
			set output [replacevar $output "%tagline" $tagline]
			set output [replacevar $output "%since" $since]
			set output [replacevar $output "%filename" $filename]
			sndone $chan [replacebasic $output "UPLOAD"]
			incr count
			set total [expr {$total + $uspeed}]
		}
	}
	set per [format "%.1f" [expr double($total) * 100 / double($speed(INCOMING)) ]]

	set output [replacevar "$theme(PREFIX)$announce(TOTUPDN)" "%type" "Uploaders:"]
	set output [replacevar $output "%count" $count]
	set output [replacevar $output "%total" [format_speed $total "none"]]
	set output [replacevar $output "%per" $per]
	sndone $chan [replacebasic $output "UPLOAD"]
	return
}

proc ng_who {nick uhost hand chan argv} {
	global binary
	checkchan $nick $chan
	foreach line [split [exec $binary(WHO)] \n] {
		if {![info exists newline($line)]} {
			set newline($line) 0
		} else { set newline($line) [expr $newline($line) + 1] }
		puthelp "PRIVMSG $nick :$line\003$newline($line)"
	}
	return
}

#################################################################################
# Theme Loading and Replacement                                                 #
#################################################################################

proc loadtheme {file} {
	global announce scriptpath theme theme_fakes variables
	unset announce
	set announce(THEMEFILE) $file

	if {[string index $file 0] != "/"} {
		set file "$scriptpath/$file"
	}
	if {![file readable $file]} {
		putlog "dZSbot: Theme file is not readable or does not exist ($file)."
		return 0
	}
	putlog "dZSbot: Loading theme \"$file\"."

	set fh [open $file]
	set content [split [read -nonewline $fh] "\n"]
	close $fh

	foreach line $content {
		if {![regexp -nocase -- "^#" $line]} {
			if {[regexp -nocase -- {fakesection\.(\S+)\s*=\s*(['\"])(.+)\2} $line dud setting quote value]} {
				set theme_fakes($setting) $value
			} elseif {[regexp -nocase -- {announce\.(\S+)\s*=\s*(['\"])(.+)\2} $line dud setting quote value]} {
				set announcetmp($setting) $value
			} elseif {[regexp -nocase -- {(\S+)\s*=\s*(['\"])(.*)\2} $line dud setting quote value]} {
				set theme($setting) $value
			}
		}
	}

	foreach name [array names theme] {set theme($name) [themereplace_startup $theme($name)]}
	foreach name [array names theme_fakes] {set theme_fakes($name) [themereplace_startup $theme_fakes($name)]}
	foreach name [array names announcetmp] {set announce($name) [themereplace_startup $announcetmp($name)]}

	## Sanity checks
	foreach type {COLOR1 COLOR2 COLOR3 PREFIX KB KBIT MB MBIT} {
		if {[lsearch -exact [array names theme] $type] == -1} {
			putlog "dZSbot error: Missing required theme setting \"$type\", failing."
			return 0
		}
	}
	foreach type [concat [array names variables] NUKE UNNUKE NUKEES] {
		if {[lsearch -exact [array names announce] $type] == -1} {
			putlog "dZSbot warning: Missing announce setting \"announce.$type\" in the theme file."
		}
	}
	return 1
}

proc themereplace {targetString section} {
	global theme

	## Escape any "$" characters so they aren't interpreted as variables in the final "subst".
	set targetString [string map {$ \\$} $targetString]

	# We replace %cX{string}, %b{string} and %u{string} with their coloured, bolded and underlined equivilants ;)
	# We also do the justification and padding that is required for %r / %l / %m to work.
	# bold and underline replacement should not be needed here...
	while {[regexp {(%c(\d)\{([^\{\}]+)\}|%b\{([^\{\}]+)\}|%u\{([^\{\}]+)\}|%([lrm])(\d\d?)\{([^\{\}]+)\})} $targetString matchString dud padOp padLength padString]} {
		# Check if any innermost %r/%l/%m are present. :-)
		while {[regexp {%([lrm])(\d\d?)\{([^\{\}]+)\}} $targetString matchString padOp padLength padString]} {
			set tmpPadString $padString
			regsub -all {\003\d\d} $tmpPadString {} tmpPadString
			set tmpPadString [string map {\002 "" \003 "" \037 ""} $tmpPadString]
			incr padLength [expr [string length $padString] - [string length $tmpPadString]]

			if {[string length $padString] >= $padLength} {
				set paddedString $padString
			} elseif {$padOp == "l"} {
				set paddedString [format "%-*s" $padLength $padString]
			} elseif {$padOp == "r"} {
				set paddedString [format "%*s" $padLength $padString]
			} elseif {$padOp == "m"} {
				set paddedString [format "%*s%s" [expr ($padLength - [string length $padString]) / 2] "" $padString]
				set paddedString [format "%-*s" $padLength $paddedString]
			}

			set targetString [string map [list $matchString $paddedString] $targetString]
		}

		regsub -all {%b\{([^\{\}]+)\}} $targetString {\\002\1\\002} targetString
		regsub -all {%u\{([^\{\}]+)\}} $targetString {\\037\1\\037} targetString

		set colorString [format "COLOR_%s_1" $section]
		if {[lsearch -exact [array names theme] $colorString] != -1} {
			regsub -all {%c(\d)\{([^\{\}]+)\}} $targetString {\\003$theme([format "COLOR_%s_" $section]\1)\2\\003} targetString
			regsub {\003(\d)(?!\d)} $targetString {\\0030\1} targetString
		} else {
			regsub -all {%c(\d)\{([^\{\}]+)\}} $targetString {\\003$theme(COLOR\1)\2\\003} targetString
			regsub {\003(\d)(?!\d)} $targetString {\\0030\1} targetString
		}
	}
	return [subst -nocommands $targetString]
}

proc themereplace_startup {rstring} {
	# We replace %b{string} and %u{string} with their bolded and underlined equivilants ;)
	while {[regexp {(%b\{([^\{\}]+)\}|%u\{([^\{\}]+)\})} $rstring]} {
		regsub -all {%b\{([^\{\}]+)\}} $rstring {\\002\1\\002} rstring
		regsub -all {%u\{([^\{\}]+)\}} $rstring {\\037\1\\037} rstring
	}
	return [subst -nocommands -novariables $rstring]
}

#################################################################################
# Start Up Stuff                                                                #
#################################################################################

## Check binary and file locations
foreach {filename filepath} [array get binary] {
	if {![istrue $bnc(ENABLED)]} {
		if {[string equal "CURL" $filename]} {continue}
		if {[string equal "PING" $filename] && ![istrue $bnc(PING)]} {continue}
	}
	if {![file executable $filepath]} {
		putlog "dZSbot error: Invalid path/permissions for $filename ($filepath)."
		set dzerror 1
	}
}

foreach {filename filepath} [array get location] {
	if {![file exists $filepath]} {
		putlog "dZSbot error: Invalid path for for $filename ($filepath)."
		set dzerror 1
	}
}

## Logs to parse
set logid 0; set loglist ""
foreach {varname logtype} {glftpdlog 0 loginlog 1 sysoplog 2} {
	foreach {filename filepath} [array get $varname] {
		if {![file readable $filepath]} {
			putlog "dZSbot error: Unable to read the log file \"$filepath\"."
			set dzerror 1
		} else {
			lappend loglist $logtype [incr logid] $filepath
			set lastread($logid) [file size $filepath]
		}
	}
}
if {!$logid} {
	putlog "dZSbot error: No logs found!"
	set dzerror 1
} else {
	putlog "dZSbot: Number of logs found: $logid"
}

## Detect glftpd version
if {[string equal -nocase "AUTO" $use_glftpd2]} {
	if {![info exists binary(GLFTPD)]} {
		die "dZSbot: you did not thoroughly edit the $scriptpath/dZSbot.conf file (hint: binary(GLFTPD))."
	}
	set glversion [exec strings $binary(GLFTPD) | grep -i "^glftpd " | cut -f1 -d. | tr A-Z a-z]

	if {[string equal "glftpd 1" $glversion]} {
		putlog "dZSbot: Detected $glversion, running in legacy mode."
		set glversion 1
	} elseif {[string equal "glftpd 2" $glversion]} {
		putlog "dZSbot: Detected $glversion, running in standard mode."
		set glversion 2
	} else {
		die "dZSbot: Auto-detecting glftpd version failed, set \"use_glftpd2\" in $scriptpath/dZSbot.conf manually."
	}
} else {
	set glversion [expr [istrue $use_glftpd2] ? 2 : 1]
	putlog "dZSbot: glftpd version defined as: $glversion."
}

## Invite checks
if {![info exists invite_channels] && [info exists chanlist(INVITE)]} {
	putlog "dZSbot warning: No \"invite_channels\" defined in the config, setting to \"$chanlist(INVITE)\" (chanlist(INVITE))"
	set invite_channels $chanlist(INVITE)
}

if {[istrue $enable_irc_invite]} {
	bind msg -|- !invite ng_invite
}

## Load the theme file
if {![loadtheme $announce(THEMEFILE)]} {
	if {[loadtheme "themes/default.zst"]} {
		putlog "dZSbot warning: Unable to load theme $announce(THEMEFILE), loaded default.zst instead."
	} else {
		putlog "dZSbot error: Unable to load the themes $announce(THEMEFILE) and default.zst."
		set dzerror 1
	}
}

## Start the log timer
if {[info exists dZStimer] && [catch {killutimer $dZStimer} error]} {
	putlog "dZSbot warning: Unable to kill log timer ($error)."
	putlog "dZSbot warning: You should .restart the bot to be safe."
}
set dZStimer [utimer 1 readlogtimer]

## Default channels and variables
if {![array exists chanlist] || ![info exists chanlist(DEFAULT)]} {
	putlog "dZSbot error: No entry in chanlist set, or \"chanlist(DEFAULT)\" not set."
	set dzerror 1
}
if {![array exists announce] || ![info exists announce(DEFAULT)]} {
	putlog "dZSbot warning: No \"announce\" entries defined, or \"announce(DEFAULT)\" is not set."
	set announce(DEFAULT) "\[DEFAULT\] %msg"
}
if {![array exists variables] || ![info exists variables(DEFAULT)]} {
	putlog "dZSbot warning: No \"variables\" entries defined, or \"variables(DEFAULT)\" is not set."
	set variables(DEFAULT) "%pf %msg"
}
if {![array exists disable] || ![info exists disable(DEFAULT)]} {
	putlog "dZSbot warning: No \"disable\" entries defined, or \"disable(DEFAULT)\" is not set."
	set disable(DEFAULT) 0
}

# Hook up variables and announce definitions for the message replacing code
# New message should have identical variables definition as old message
# New announce set to old announce if not found in theme file (and output a
# warning message).
foreach rep [array names msgreplace] {
	set rep [split $msgreplace($rep) ":"]
	set variables([lindex $rep 2]) $variables([lindex $rep 0])
	set disable([lindex $rep 2]) 0
	if {![info exists announce([lindex $rep 2])]} {
		set announce([lindex $rep 2]) $announce([lindex $rep 0])
		putlog "dZSbot warning: Custom message [lindex $rep 2] defined, but no announce definition found. Using same announce as [lindex $rep 0] for now. Please edit the theme file!"
	}
}

if {!$dzerror} {
	putlog "dZSbot: Loaded successfully!"
} else {
	putlog "dZSbot: Errors were encountered while loading, please check the log and correct them."
	if {[istrue $die_on_error]} {die}
}
