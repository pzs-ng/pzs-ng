#################################################################################
# Code part only, main config is moved to dZSbconf.tcl                          #
# THIS MEANS THAT YOU ARENT SUPPOSED TO EDIT THIS FILE                          #
# FOR CONFIGURATION PURPOSES!                                                   #
#################################################################################

#################################################################################
# Read The Config                                                               #
#################################################################################
interp alias {} istrue {} string is true -strict
interp alias {} isfalse {} string is false -strict

set dzerror 0; set pid 0
set scriptpath [file dirname [info script]]

putlog "Launching dZSbot for project-zs-ng..."

if {[catch {source $scriptpath/dZSbconf.defaults.tcl} error]} {
	putlog "dZSbot error: Unable to load dZSbconf.defaults.tcl ($error), cannot continue."
	putlog "dZSbot error: See FAQ for possible solutions/debugging options."
	die
}
if {[catch {source $scriptpath/dZSbconf.tcl} error]} {
	putlog "dZSbot warning: Unable to load dZSbconf.tcl ($error), using defaults."
	putlog "dZSbot warning: If this is your first install, do: cp dZSbconf.dist.tcl dZSbconf.tcl"
}
if {[catch {source $scriptpath/dZSbvars.tcl} error]} {
	putlog "dZSbot error: Unable to load dZSbvars.tcl ($error), cannot continue."
	putlog "dZSbot error: See FAQ for possible solutions/debugging options."
	die
}

foreach entry [array names binary] {
	if {$entry == "NCFTPLS" && ![istrue $bnc(ENABLED)]} {continue}
	if {$entry == "PING" && (![istrue $bnc(ENABLED)] || ![istrue $bnc(PING)])} {continue}

	if {![file executable $binary($entry)]} {
		putlog "dZSbot error: Invalid path/missing bin for $entry - please fix."
		set dzerror 1
	}
}

foreach entry [array names location] {
	if {![file exists $location($entry)]} {
		putlog "dZSbot error: Invalid path for $entry - please fix."
		set dzerror 1
	}
}

set logcount 0
foreach entry [array names glftpdlog] {
	if {![file exists $glftpdlog($entry)]} {
		putlog "dZSbot error: Could not find log file $glftpdlog($entry)."
		set dzerror 1
		unset glftpdlog($entry)
	} else {
		incr logcount
		set lastoct($entry) [file size $glftpdlog($entry)]
	}
}
if {!$logcount} {
	putlog "dZSbot error: No gl logfiles found!"
	set dzerror 1
} else {
	putlog "dZSbot: Number of gl logfiles found: $logcount"
}

set logcount 0
foreach entry [array names loginlog] {
	if {![file exists $loginlog($entry)]} {
		putlog "dZSbot: Could not find log file $loginlog($entry)."
		set dzerror 1
		unset loginlog($entry)
	} else {
		incr logcount
		set loglastoct($entry) [file size $loginlog($entry)]
	}
}
if {!$logcount} {
	putlog "dZSbot warning: No login logfiles found!"
} else {
	putlog "dZSbot: Number of login logfiles found: $logcount"
}

if {[string equal -nocase "AUTO" $use_glftpd2]} {
	if {![info exists binary(GLFTPD)]} {
		die "dZSbot: you did not thoroughly edit the $scriptpath/dZSbconf.tcl file (hint: binary(GLFTPD))."
	}
	set glversion [exec strings $binary(GLFTPD) | grep -i "^glftpd " | cut -f1 -d. | tr A-Z a-z]

	if {[string equal "glftpd 1" $glversion]} {
		putlog "dZSbot: Detected $glversion, running in legacy mode."
		set glversion 1
	} elseif {[string equal "glftpd 2" $glversion]} {
		putlog "dZSbot: Detected $glversion, running in standard mode."
		set glversion 2
	} else {
		die "dZSbot: Autodetecting glftpd-version failed. Set \"use_glftpd2\" in $scriptpath/dZSbconf.tcl manually."
	}
} else {
	set glversion [expr [istrue $use_glftpd2] ? 2 : 1]
}

if {![info exists invite_channels] && [info exists chanlist(INVITE)]} {
	putlog "dZSbot warning: No \"invite_channels\" defined in the config, setting to \"$chanlist(INVITE)\" (chanlist(INVITE))"
	set invite_channels $chanlist(INVITE)
}

#################################################################################
# Important Global Variables                                                    #
#################################################################################

set defaultsection "DEFAULT"
set nuke(LASTTYPE) ""
set nuke(LASTDIR) ""
set nuke(SHOWN) 1
set variables(NUKE)   ""
set variables(UNNUKE) ""
set mpath ""

#################################################################################
# Set Bindings                                                                  #
#################################################################################

bind join -|- * welcome_msg
bind dcc n errorinfo errorinfo
proc errorinfo {args} {
	global errorInfo tcl_patchLevel tcl_platform
	putlog "--\[\002Error Info\002\]------------------------------------------"
	putlog "Tcl: $tcl_patchLevel"
	putlog "Box: $tcl_platform(os) $tcl_platform(osVersion)"
	putlog "Message:"
	foreach line [split $errorInfo \n] {putlog $line}
	putlog "--------------------------------------------------------"
}

proc bindcommands {cmdpre} {
	bind pub -|- [set cmdpre]bnc         ng_bnc_check
	bind pub -|- [set cmdpre]bw          ng_bandwidth
	bind pub -|- [set cmdpre]bwdn        ng_bwdn
	bind pub -|- [set cmdpre]bwup        ng_bwup
	bind pub -|- [set cmdpre]df          show_free
	bind pub -|- [set cmdpre]free        show_free
	bind pub -|- [set cmdpre]dn          ng_leechers
	bind pub -|- [set cmdpre]down        ng_leechers
	bind pub -|- [set cmdpre]downloaders ng_leechers
	bind pub -|- [set cmdpre]leechers    ng_leechers
	bind pub -|- [set cmdpre]dupe        ng_search
	bind pub -|- [set cmdpre]help        help
	bind pub -|- [set cmdpre]idle        ng_idlers
	bind pub -|- [set cmdpre]idlers      ng_idlers
	bind pub -|- [set cmdpre]inc         show_incompletes
	bind pub -|- [set cmdpre]incomplete  show_incompletes
	bind pub -|- [set cmdpre]incompletes show_incompletes
	bind pub -|- [set cmdpre]new         ng_new
	bind pub -|- [set cmdpre]nukes       ng_nukes
	bind pub -|- [set cmdpre]search      ng_search
	bind pub -|- [set cmdpre]speed       speed
	bind pub -|- [set cmdpre]unnukes     ng_unnukes
	bind pub -|- [set cmdpre]up          ng_uploaders
	bind pub -|- [set cmdpre]uploaders   ng_uploaders
	bind pub -|- [set cmdpre]who         who

	bind pub -|- [set cmdpre]gpad    [list showstats "-d" "-A"]
	bind pub -|- [set cmdpre]gpal    [list showstats "-u" "-A"]
	bind pub -|- [set cmdpre]gpwk    [list showstats "-d" "-W"]
	bind pub -|- [set cmdpre]gwpd    [list showstats "-u" "-W"]

	bind pub -|- [set cmdpre]alldn   [list showstats "-d" "-a"]
	bind pub -|- [set cmdpre]allup   [list showstats "-u" "-a"]
	bind pub -|- [set cmdpre]daydn   [list showstats "-d" "-t"]
	bind pub -|- [set cmdpre]dayup   [list showstats "-u" "-t"]
	bind pub -|- [set cmdpre]monthdn [list showstats "-d" "-m"]
	bind pub -|- [set cmdpre]monthup [list showstats "-u" "-m"]
	bind pub -|- [set cmdpre]wkdn    [list showstats "-d" "-w"]
	bind pub -|- [set cmdpre]wkup    [list showstats "-u" "-w"]
}

if {[istrue $bindnopre]} {
	bindcommands "!"
} elseif {![string equal "!" $cmdpre]} {
	catch {unbind pub -|- !bnc         ng_bnc_check}
	catch {unbind pub -|- !bw          ng_bandwidth}
	catch {unbind pub -|- !bwdn        ng_bwdn}
	catch {unbind pub -|- !bwup        ng_bwup}
	catch {unbind pub -|- !df          show_free}
	catch {unbind pub -|- !free        show_free}
	catch {unbind pub -|- !dn          ng_leechers}
	catch {unbind pub -|- !down        ng_leechers}
	catch {unbind pub -|- !downloaders ng_leechers}
	catch {unbind pub -|- !leechers    ng_leechers}
	catch {unbind pub -|- !dupe        ng_search}
	catch {unbind pub -|- !help        help}
	catch {unbind pub -|- !idle        ng_idlers}
	catch {unbind pub -|- !idlers      ng_idlers}
	catch {unbind pub -|- !inc         show_incompletes}
	catch {unbind pub -|- !incomplete  show_incompletes}
	catch {unbind pub -|- !incompletes show_incompletes}
	catch {unbind pub -|- !new         ng_new}
	catch {unbind pub -|- !nukes       ng_nukes}
	catch {unbind pub -|- !search      ng_search}
	catch {unbind pub -|- !speed       speed}
	catch {unbind pub -|- !unnukes     ng_unnukes}
	catch {unbind pub -|- !up          ng_uploaders}
	catch {unbind pub -|- !uploaders   ng_uploaders}
	catch {unbind pub -|- !who         who}

	catch {unbind pub -|- !gpad    [list showstats "-d" "-A"]}
	catch {unbind pub -|- !gpal    [list showstats "-u" "-A"]}
	catch {unbind pub -|- !gpwk    [list showstats "-d" "-W"]}
	catch {unbind pub -|- !gwpd    [list showstats "-u" "-W"]}
	catch {unbind pub -|- !alldn   [list showstats "-d" "-a"]}
	catch {unbind pub -|- !allup   [list showstats "-u" "-a"]}
	catch {unbind pub -|- !daydn   [list showstats "-d" "-t"]}
	catch {unbind pub -|- !dayup   [list showstats "-u" "-t"]}
	catch {unbind pub -|- !monthdn [list showstats "-d" "-m"]}
	catch {unbind pub -|- !monthup [list showstats "-u" "-m"]}
	catch {unbind pub -|- !wkdn    [list showstats "-d" "-w"]}
	catch {unbind pub -|- !wkup    [list showstats "-u" "-w"]}
}

## Bind the user defined command prefix.
bindcommands $cmdpre

#################################################################################
# Check if the release should not be announced                                  #
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

proc typecheck {section msgtype} {
	global disabletypes
	if {[info exists disabletypes($section)]} {
		foreach deny $disabletypes($section) {
			if {[string match $deny $msgtype]} {return 1}
		}
	}
	return 0
}

#################################################################################
# Main loop - Parses data from logs.                                            #
#################################################################################
proc readlog {} {
	global dZStimer defaultsection glftpdlog glversion invite_channels lastoct loginlog loglastoct max_log_change pid
	global chanlist disable variables msgreplace msgtypes privchannel privgroups privusers

	set dZStimer [utimer 1 "readlog"]
	set lines ""

	foreach log [array names glftpdlog] {
		if {$lastoct($log) < [file size $glftpdlog($log)] && [expr [file size $glftpdlog($log)] - $lastoct($log) - $max_log_change] < 0} {
			if {![catch {set of [open $glftpdlog($log) r]} error]} {
				seek $of $lastoct($log)
				while {![eof $of]} {
					if {[set line [gets $of]] != ""} {
						lappend lines $line
					}
				}
				close $of
			} else {
				putlog "dZSbot error: Unable to open log file \"$glftpdlog($log)\" ($error)."
				return 0
			}
		}
		set lastoct($log) [file size $glftpdlog($log)]
	}

	foreach login [array names loginlog] {
		if {$loglastoct($login) < [file size $loginlog($login)] && [expr [file size $loginlog($login)] - $loglastoct($login) - $max_log_change] < 0} {
			if {![catch {set of [open $loginlog($login) r]} error]} {
				seek $of $loglastoct($login)
				while {![eof $of]} {
					if {[set line [gets $of]] != ""} {
						lappend lines $line
					}
				}
				close $of
			} else {
				putlog "dZSbot error: Unable to open log file \"$loginlog($login)\" ($error)."
				return 0
			}
		}
		set loglastoct($login) [file size $loginlog($login)]
	}

	foreach line $lines {
		# Slightly hackish /daxxar
		# (We remove the element if it's [*], since it's PID)
		# (I store it in $pid so we can add a cookie for that later (or?))
		set pid 0
		if {[regexp {\[([0-9\ ]+)\]} $line dud pid]} {
			regsub "\\\[$pid\\\] " $line "" line
			set pid [string trim $pid]
		}

		# login.log hack by mcr
		# fake a msgtype for each of the known login.log messages
		#
		if {[lrange $line 7 14] == "connection refused: ident@ip not added to any users."} {
			set line [linsert $line 5 "IPNOTADDED:"]
		}
		if {[lrange $line 8 9] == "Bad user@host."} {
		set line [linsert $line 5 "BADUSERHOST:"]
		}
		if {[lrange $line 8 9] == "Login failure."} {
			set line [linsert $line 5 "BADPASSWD:"]
		}

		# If we cannot detect a msgtype - default to DEBUG: and insert that into the list ($line).
		if {[string first ":" [lindex $line 5]] < 0} {
			if {[string first "@" [lindex $line 5]] < 0} {
				set msgtype "DEBUG"
				if {[llength $line] < 5} {
					set line [lappend $line "dummy debug"]
				} else {
					set line [linsert $line 5 "DEBUG:"]
				}
			} else {
				set msgtype [string trim [lindex $line 5] "@"]
			}
		} else {
			set msgtype [string trim [lindex $line 5] ":"]
		}

		# Catch regular as generated DEBUG lines.
		if {$msgtype == "DEBUG"} {
			# Now gather all list items after item 5 into one item compounded by {}
			set tmp_begin [lrange $line 0 5]
			set tmp [list [lrange $line 6 end]]
			set line "$tmp_begin $tmp"
		}

		# Since PID is kinda special, we append this _after_ the above compound list item is generated.
		if {$pid > 0} {
			set line "$line $pid"
		}

		# Invite users to public and private channels
		if {[string equal $msgtype "INVITE"]} {
			set ircnick [lindex $line 6]
			set nick [lindex $line 7]
			set group [lindex $line 8]
			foreach channel $invite_channels {puthelp "INVITE $ircnick $channel"}

			foreach privchan [array names privchannel] {
				if {[info exists privgroups($privchan)]} {
					foreach privgroup $privgroups($privchan) {
						if {[string equal $group $privgroup]} {
							foreach channel $privchannel($privchan) {puthelp "INVITE $ircnick $channel"}
						}
					}
				}
				if {[info exists privusers($privchan)]} {
					foreach privuser $privusers($privchan) {
						if {[string equal $nick $privuser]} {
							foreach channel $privchannel($privchan) {puthelp "INVITE $ircnick $channel"}
						}
					}
				}
			}
		}

		set path [lindex $line 6]

		if {[lsearch -exact $msgtypes(SECTION) $msgtype] != -1} {
			if {[denycheck $path]} {continue}
			set section [getsectionname $path]

			# Replace messages with custom messages
			foreach rep [array names msgreplace] {
				set rep [split $msgreplace($rep) ":"]
				if {[string equal $msgtype [lindex $rep 0]]} {
					if {[string match -nocase [lindex $rep 1] $path]} {
						set msgtype [lindex $rep 2]
					}
				}
			}
		} elseif {[lsearch -exact $msgtypes(DEFAULT) $msgtype] != -1} {
			set section $defaultsection
		} else {
			putlog "dZSbot error: Undefined message type \"$msgtype\", check \"msgtypes(SECTION)\" and \"msgtypes(DEFAULT)\" in the config."; continue
		}
		if {![info exists variables($msgtype)]} {
			putlog "dZSbot error: \"variables($msgtype)\" not defined in the config, type becomes \"DEFAULT\"."
			set msgtype "DEFAULT"
		}
		if {([info exists disable($msgtype)] && $disable($msgtype) != 1) && ![typecheck $section $msgtype]} {
			sndall $msgtype $section [parse $msgtype [lrange $line 6 end] $section]
			postcmd $msgtype $section $path
		}
	}

	if {$glversion == 2} {
		launchnuke
	}

	return 0
}

#################################################################################
# Post Command                                                                  #
#################################################################################
proc postcmd {msgtype section path} {
	global postcommand

	if {[info exists postcommand($msgtype)]} {
		foreach cmd $postcommand($msgtype) {
			if {[string equal -length 5 "exec " $cmd]} {
				set cmd [string range $cmd 5 end]
				if {[catch {exec $cmd $msgtype $section $path} error]} {
					putlog "dZSbot error: Unable to execute post command \"$cmd\" ($error)."
				}
			} else {
				if {[catch {$cmd $msgtype $section $path} error]} {
					putlog "dZSbot error: Unable to evaluate post command \"$cmd\" ($error)."
				}
			}
		}
	}
}

#################################################################################
# Get Section Name                                                              #
#################################################################################
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

#################################################################################
# Replace Cookie With Value                                                     #
#################################################################################
proc replacevar {string cookie value} {
	global zeroconvert
	if {[string length $value] == 0 && [info exists zeroconvert($cookie)]} {
		set value $zeroconvert($cookie)
	}
	## Why not use Tcl's string replacement function? It's faster :P
	return [string map [list $cookie $value] $string]
}

#################################################################################
# CONVERT ANYTHING>MB TO MEGABYTES                                              #
#################################################################################
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

#################################################################################
# CONVERT ANYTHING>1000MB TO BETTER UNIT                                        #
#################################################################################
proc from_mb {str} {
	set units(1) "MB"
	set units(2) "GB"
	set units(3) "TB"
	set units(4) "PB"

	set unit 1
	while {$str >= 1000} {
		set str [expr $str / 1000.00]
		incr unit
	}

	return "[format %.1f $str]$units($unit)"
}

#################################################################################
# TRIMS TRAILING STRING FROM ANOTHER STRING                                     #
#################################################################################
proc trimtail {strsrc strrm} {
	if { [expr [string length $strsrc] - [string length $strrm]] == [string last $strrm $strsrc] } {
		return [string range $strsrc 0 [expr [string length $strsrc] - [string length $strrm] - 1]]
	}
	return $strsrc
}

#################################################################################
# CONVERT BASIC COOKIES TO DATA                                                 #
#################################################################################
proc basicreplace {string section} {
	global cmdpre sitename
	return [string map [list "%cmdpre" $cmdpre "%section" $section\
		"%sitename" $sitename "%bold" \002 "%uline" \037 "%color" \003] $string]
}

#################################################################################
# CONVERT COOKIES TO DATA                                                       #
#################################################################################
proc parse {msgtype msgline section} {
	global announce defaultsection disable mpath pid random sitename theme theme_fakes glversion variables
	set type $msgtype

	if {[string equal $type "NUKE"] || [string equal $type "UNNUKE"]} {
		if {$glversion == 1} {
			fuelnuke $type [lindex $msgline 0] $section $msgline
		} else {
			launchnuke2 $type [lindex $msgline 0] $section [lrange $msgline 1 3] [lrange $msgline 4 end]
		}
		return ""
	}

	if {![info exists variables($type)]} {
		if {$pid == 0} {
			putlog "dZSbot error: \"variables($type)\" not set in theme, type becomes \"DEFAULT\""
			set type "DEFAULT"
		} else {
			if {$disable(FAILLOGIN) == 1} {
				return ""
			} else {
				set f_user $type
				set type "FAILLOGIN"
			}
		}
	}
	if {![info exists announce($type)]} {
		if {$pid == 0} {
			putlog "dZSbot error: \"announce($type)\" not set in theme, type becomes \"DEFAULT\""
			set type "DEFAULT"
		} else {
			if {$disable(FAILLOGIN) == 1} {
				return ""
			} else {
				set f_user $type
				set type "FAILLOGIN"
			}
		}
	}

	set vars $variables($type)

	if {[string equal [lindex $announce($type) 0] "random"] && [string is alnum -strict [lindex $announce($type) 1]]} {
		set output $random($msgtype\-[rand [lindex $announce($type) 1]])
	} else {
		set output $announce($type)
	}

	set output "$theme(PREFIX)$output"
	if {[string equal $section $defaultsection] && [info exists theme_fakes($type)]} {
		set section $theme_fakes($type)
	}
	set output [basicreplace $output $section]
	set cnt 0

	if {[string equal "%pf" [lindex $vars 0]]} {
		set split [split [lindex $msgline 0] "/"]
		set ll [llength $split]

		set split2 [split $mpath "/"]
		set sl [llength $split2]

		set temp [lrange $split [expr $sl - 1] end]
		set relname ""
		foreach part $temp {
			set relname $relname/$part
		}
		set temp [string range $relname 1 end]
		set output [replacevar $output "%relname" $temp]
		set output [replacevar $output "%reldir" [lindex $split [expr $ll -1]]]
		set output [replacevar $output "%path" [lindex $split [expr $ll -2]]]
		set vars [string range $vars 4 end]
		set cnt 1
	} elseif {[string equal "%failed_nick" [lindex $vars 0]] && $pid != 0} {
		set output [replacevar $output "%failed_nick" $f_user ]
		set vars [string range $vars 13 end]
		set ip1 [lindex $msgline 1]
		set ip2 [string range $ip1 1 [expr [string length $ip1] - 3]]
		set msgline [replacevar $msgline $ip1 $ip2]
		set cnt 0
	}

	set loop 1

	foreach vari $vars {
		if {[llength $vari] > 1} {
			set cnt2 0
			set cnt3 1
			set values [lindex $msgline $cnt]
			set output2 ""
			foreach value $values {
				if {$cnt2 == 0} {
					append output2 "$announce(${type}_LOOP${loop})"
				}
				if {[string match "*speed" [lindex $vari $cnt2]]} {
					set output2 [replacevar $output2 "[lindex $vari $cnt2]" [format_speed $value $section]]
				} else {
					set output2 [replacevar $output2 "[lindex $vari $cnt2]" $value]
				}
				incr cnt2
				if {[lindex $vari $cnt2] == ""} {
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
				set output [replacevar $output $vari [format_speed [lindex $msgline $cnt] $section]]
			} else {
				set output [replacevar $output $vari [lindex $msgline $cnt]]
			}
		}
		incr cnt
	}

	return [themereplace $output $section]
}

#################################################################################
# Channel trigger check.                                                        #
#################################################################################
proc checkchan {nick chan} {
	global disable lastbind mainchan
	if {$disable(TRIGINALLCHAN) == 1 && ![string equal -nocase $chan $mainchan]} {
		putlog "dZSbot: \002$nick\002 tried to use \002$lastbind\002 from an invalid channel ($chan)."
		return -code return
	}
}

#################################################################################
# Parse command options.                                                        #
#################################################################################
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

#################################################################################
# Retrieve a list of UIDs and users.                                            #
#################################################################################
proc gl_userids {} {
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
		putlog "dZSbot error: Could not open PASSWD: $error"
	}
	return $userlist
}

#################################################################################
# Retrieve a list of GIDs and groups.                                           #
#################################################################################
proc gl_groupids {} {
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
		putlog "dZSbot error: Could not open GROUP: $error"
	}
	return $grouplist
}

#################################################################################
# Parse IRC command options.                                                    #
#################################################################################
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

#################################################################################
# Format The Time Duration                                                      #
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

#################################################################################
# Format the Speed To A Customized Unit                                         #
#################################################################################
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
		"default" {
			set type $theme(KB)
		}
	}
	return [themereplace [replacevar $type "%value" $value] $section]
}

#################################################################################
# Display the latest releases.                                                  #
#################################################################################
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
	if {$section == ""} {
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
	array set uid [gl_userids]
	array set gid [gl_groupids]

	set output "$theme(PREFIX)$announce(NEW)"
	set output [replacevar $output "%section" $section]
	sndone $nick [basicreplace $output "NEW"]
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
		set path [lrange [file split [string trim [file dirname $dirname] "/"]] 1 end]
		set output [replacevar $output "%path" [join $path "/"]]
		set output [replacevar $output "%reldir" [file tail $dirname]]
		sndone $nick [basicreplace $output "NEW"]
	}

	if {!$num} {
		set output "$theme(PREFIX)$announce(NEW_NONE)"
		sndone $nick [basicreplace $output "NEW"]
	}
}

#################################################################################
# Search the dirlog for releases.                                               #
#################################################################################
proc ng_search {nick uhost hand chan argv} {
	global announce binary defaultsection lastbind location search_chars theme
	checkchan $nick $chan

	if {![getoptions $argv results pattern] || $pattern == ""} {
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
	array set uid [gl_userids]
	array set gid [gl_groupids]

	set output "$theme(PREFIX)$announce(SEARCH)"
	set output [replacevar $output "%pattern" $pattern]
	sndone $nick [basicreplace $output "SEARCH"]
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
		set path [lrange [file split [string trim [file dirname $dirname] "/"]] 1 end]
		set output [replacevar $output "%path" [join $path "/"]]
		set output [replacevar $output "%reldir" [file tail $dirname]]
		sndone $nick [basicreplace $output "SEARCH"]
	}

	if {!$num} {
		set output "$theme(PREFIX)$announce(SEARCH_NONE)"
		sndone $nick [basicreplace $output "SEARCH"]
	}
}

#################################################################################
# Display the latest nukes.                                                     #
#################################################################################
proc ng_nukes {nick uhost hand chan argv} {
	global announce binary defaultsection lastbind location sections theme
	checkchan $nick $chan

	if {![getoptions $argv results section]} {
		puthelp "PRIVMSG $chan :\002Usage:\002 $lastbind \[-max <num>\] \[section\]"
		return
	}
	if {$section == ""} {
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
	sndone $nick [basicreplace $output "NUKES"]
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
		set path [lrange [file split [string trim [file dirname $dirname] "/"]] 1 end]
		set output [replacevar $output "%path" [join $path "/"]]
		set output [replacevar $output "%reldir" [file tail $dirname]]
		sndone $nick [basicreplace $output "NUKES"]
	}

	if {!$num} {
		set output "$theme(PREFIX)$announce(NUKES_NONE)"
		sndone $nick [basicreplace $output "NUKES"]
	}
}

#################################################################################
# Display the latest unnukes.                                                   #
#################################################################################
proc ng_unnukes {nick uhost hand chan argv} {
	global announce binary defaultsection lastbind location sections theme
	checkchan $nick $chan

	if {![getoptions $argv results section]} {
		puthelp "PRIVMSG $chan :\002Usage:\002 $lastbind \[-max <num>\] \[section\]"
		return
	}
	if {$section == ""} {
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
	sndone $nick [basicreplace $output "UNNUKES"]
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
		set path [lrange [file split [string trim [file dirname $dirname] "/"]] 1 end]
		set output [replacevar $output "%path" [join $path "/"]]
		set output [replacevar $output "%reldir" [file tail $dirname]]
		sndone $nick [basicreplace $output "UNNUKES"]
	}

	if {!$num} {
		set output "$theme(PREFIX)$announce(UNNUKES_NONE)"
		sndone $nick [basicreplace $output "UNNUKES"]
	}
}

#################################################################################
# SEND TO ALL CHANNELS LISTED                                                   #
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
		foreach line [split $text $splitter(CHAR)] {
			putquick "PRIVMSG $chan :$line"
		}
	}
}

#################################################################################
# SEND TO ONE CHANNEL                                                           #
#################################################################################
proc sndone {chan text} {
	global splitter
	foreach line [split $text $splitter(CHAR)] {
		putquick "PRIVMSG $chan :[themereplace $line "none"]"
	}
}

#################################################################################
# POST WHO INFO                                                                 #
#################################################################################
proc who {nick uhost hand chan argv} {
	global binary
	checkchan $nick $chan

	foreach line [split [exec $binary(WHO)] \n] {
		if {![info exists newline($line)]} {
			set newline($line) 0
		} else { set newline($line) [expr $newline($line) + 1] }
		puthelp "PRIVMSG $nick :$line\003$newline($line)"
	}
	puthelp "PRIVMSG $nick : "
}

#################################################################################
# POST SPEED                                                                    #
#################################################################################
proc speed {nick uhost hand chan argv} {
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
			set output [basicreplace $output "SPEED"]
			sndone $chan $output
		}
	} else {
		set base_output "$theme(PREFIX)$announce(DEFAULT)"
		foreach line [split [exec $binary(WHO) [lindex $argv 0]] "\n"] {
			set output [replacevar $base_output "%msg" $line]
			set output [basicreplace $output "SPEED"]
			sndone $chan $output
		}
	}

	if {$line == ""} {
		set output "$theme(PREFIX)$announce(SPEEDERROR)"
		set output [replacevar $output "%msg" "User not online."]
		set output [basicreplace $output "SPEED"]
		sndone $chan $output
	}
}

#################################################################################
# uploaders BANDWIDTH                                                           #
#################################################################################
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

	set output [basicreplace $output "BW"]
	sndone $chan $output
}

#################################################################################
# ng_uploaders - Origional by Celerex - Mod/Merge by themolester                #
#################################################################################
proc ng_uploaders {nick uhost hand chan argv} {
	global binary announce speed theme
	checkchan $nick $chan

	set output "$theme(PREFIX)$announce(UPLOAD)"
	set output [basicreplace $output "UPLOAD"]
	sndone $chan $output

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
			set output [basicreplace $output "UPLOAD"]
			sndone $chan $output
			incr count
			set total [expr $total+$uspeed]
		}
	}
	set per [format "%.1f" [expr double($total) * 100 / double($speed(INCOMING)) ]]

	set output [replacevar "$theme(PREFIX)$announce(TOTUPDN)" "%type" "Uploaders:"]
	set output [replacevar $output "%count" $count]
	set output [replacevar $output "%total" [format_speed $total "none"]]
	set output [replacevar $output "%per" $per]

	set output [basicreplace $output "UPLOAD"]
	sndone $chan $output
}

#################################################################################
# downloaders BANDWIDTH                                                         #
#################################################################################
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

	set output [basicreplace $output "BW"]
	sndone $chan $output
}

#################################################################################
# ng_leechers - Origional by Celerex - Mod/Merge by themolester                 #
#################################################################################
proc ng_leechers {nick uhost hand chan argv} {
	global binary announce speed theme
	checkchan $nick $chan

	set output "$theme(PREFIX)$announce(LEECH)"
	set output [basicreplace $output "LEECH"]
	sndone $chan $output

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
			set output [basicreplace $output "LEECH"]
			sndone $chan $output
			incr count
			set total [expr $total+$uspeed]
		}
	}
	set per [format "%.1f" [expr double($total) * 100 / double($speed(OUTGOING)) ]]

	set output [replacevar "$theme(PREFIX)$announce(TOTUPDN)" "%type" "Leechers:"]
	set output [replacevar $output "%count" $count]
	set output [replacevar $output "%total" [format_speed $total "none"]]
	set output [replacevar $output "%per" $per]

	set output [basicreplace $output "LEECH"]
	sndone $chan $output
}

#################################################################################
# ng_idlers - Origional by Celerex - Mod/Merge by themolester                   #
#################################################################################
proc ng_idlers {nick uhost hand chan argv} {
	global binary announce speed minidletime theme
	checkchan $nick $chan

	set output "$theme(PREFIX)$announce(IDLE)"
	set output [basicreplace $output "IDLE"]
	sndone $chan $output

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
				set output [basicreplace $output "IDLE"]
				sndone $chan $output
				incr count
			}
		}
	}
	set output [replacevar "$theme(PREFIX)$announce(TOTIDLE)" "%count" $count]
	set output [basicreplace $output "IDLE"]
	sndone $chan $output
}

#################################################################################
# UPDATED BANDWIDTH                                                             #
#################################################################################
proc ng_bandwidth {nick uhost hand chan argv} {
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

	set output [basicreplace $output "BW"]
	sndone $chan $output
}

#################################################################################
# POST STATS                                                                    #
#################################################################################
proc showstats {type time nick uhost hand chan argv} {
	global binary statsection location
	checkchan $nick $chan

	set sect 0
	set section [lindex $argv 1]
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
	puthelp "PRIVMSG $nick : "
}

#################################################################################
# INVITE CHECK                                                                  #
#################################################################################
proc invite {nick host hand arg} {
	global location binary chanlist announce theme invite_channels disable

	if {[llength $arg] == 2} {
		set username [lindex $arg 0]
		set password [lindex $arg 1]
		set result [exec $binary(PASSCHK) $username $password $location(PASSWD)]
		set group ""

		set userfile "$location(USERS)$username"

		if {[string equal $result "MATCH"]} {
			set output "$theme(PREFIX)$announce(MSGINVITE)"
			foreach channel $invite_channels {puthelp "INVITE $nick $channel"}
			foreach line [split [exec $binary(CAT) $userfile] "\n"] {
				if {[string equal -length 5 $line "GROUP"]} {
					set group [lindex $line 1]
					break
				}
			}
		} else {
			set output "$theme(PREFIX)$announce(BADMSGINVITE)"
		}

		if {$disable(MSGINVITE) != 1} {
			set output [replacevar $output "%u_ircnick" $nick]
			set output [replacevar $output "%u_name" $username]
			set output [replacevar $output "%u_host" $host]
			set output [replacevar $output "%g_name" $group]
			set output [themereplace [basicreplace $output "INVITE"] "none"]
			sndall "MSGINVITE" "DEFAULT" $output
		}
	}
}

#################################################################################
# SHOW FREE SPACE                                                               #
#################################################################################
proc show_free {nick uhost hand chan arg} {
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

				set total [expr $total + double($dev_total)];
				set used [expr $used + double($dev_used)]
				set free [expr $free + double($dev_free)];
				set perc [expr $perc + double($dev_percent)]
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
		set output [replacevar $output "%percentage" [expr round($perc/$num)]]
		set output [replacevar $output "%devices" $devices($o)]
		set output [basicreplace $output "FREE"]
		sndone $chan $output
		incr o
	}
}

#################################################################################
# LAUNCH A NUKE (GL2.0)                                                         #
#################################################################################
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
	set split [split $nuke(PATH) "/"]
	set ll [llength $split]
	set split2 [split $mpath "/"]
	set sl [llength $split2]
	set temp [lrange $split [expr $sl - 1] end]
	set relname ""
	foreach part $temp {
		set relname $relname/$part
	}
	set relname [string range $relname 1 end]

	set output "$theme(PREFIX)$announce($nuke(TYPE))"
	set output [replacevar $output "%nuker" $nuke(NUKER)]
	set output [replacevar $output "%nukees" $nuke(NUKEE)]
	set output [replacevar $output "%type" $nuke(TYPE)]
	set output [replacevar $output "%multiplier" $nuke(MULT)]
	set output [replacevar $output "%reason" $nuke(REASON)]
	set output [replacevar $output "%section" $nuke(SECTION)]
	set output [replacevar $output "%relname" $relname]
	set output [replacevar $output "%reldir" [lindex $split [expr $ll -1]]]
	set output [replacevar $output "%path" [lindex $split [expr $ll -2]]]
	set output [themereplace [basicreplace $output $nuke(TYPE)] "none"]
	sndall $nuke(TYPE) $nuke(SECTION) $output
}

#################################################################################
# UPDATE NUKE BUFFER (GL1.0)                                                    #
#################################################################################
proc fuelnuke {type path section line} {
	global nuke hidenuke

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

#################################################################################
# FLUSH NUKE BUFFER  (GL1.0)                                                    #
#################################################################################
proc launchnuke {} {
	global nuke sitename announce theme mpath
	if {$nuke(SHOWN) == 1} {return 0}
	set nuke(NUKEE) [trimtail $nuke(NUKEE) $theme(SPLITTER)]

	set split [split $nuke(PATH) "/"]
	set ll [llength $split]
	set split2 [split $mpath "/"]
	set sl [llength $split2]
	set temp [lrange $split [expr $sl - 1] end]
	set relname ""
	foreach part $temp {
		set relname $relname/$part
	}
	set relname [string range $relname 1 end]

	set output "$theme(PREFIX)$announce($nuke(TYPE))"
	set output [replacevar $output "%nuker" $nuke(NUKER)]
	set output [replacevar $output "%nukees" $nuke(NUKEE)]
	set output [replacevar $output "%type" $nuke(TYPE)]
	set output [replacevar $output "%multiplier" $nuke(MULT)]
	set output [replacevar $output "%reason" $nuke(REASON)]
	set output [replacevar $output "%section" $nuke(SECTION)]
	set output [replacevar $output "%relname" $relname]
	set output [replacevar $output "%reldir" [lindex $split [expr $ll -1]]]
	set output [replacevar $output "%path" [lindex $split [expr $ll -2]]]
	set output [themereplace [basicreplace $output $nuke(TYPE)] "none"]
	sndall $nuke(TYPE) $nuke(SECTION) $output

	set nuke(SHOWN) 1
}

#################################################################################
# SHOW INCOMPLETE LIST                                                          #
#################################################################################
proc show_incompletes {nick uhost hand chan arg } {
	global sitename binary
	checkchan $nick $chan

	foreach line [split [exec $binary(INCOMPLETE)] "\n"] {
		if {![info exists newline($line)]} {
			set newline($line) 0
		} else { set newline($line) [expr $newline($line) + 1] }
		puthelp "PRIVMSG $nick :$line\003$newline($line)"
	}
}

#################################################################################
# SHOW WELCOME MSG                                                              #
#################################################################################
proc welcome_msg {nick uhost hand chan } {
	global announce disable chanlist sitename cmdpre
	if {$disable(WELCOME) == 1} {return}

	foreach c_chan $chanlist(WELCOME) {
		if {[string match -nocase $c_chan $chan]} {
			set output [basicreplace $announce(WELCOME) "WELCOME"]
			set output [replacevar $output "%ircnick" $nick]
			set output [themereplace $output "none"]
			puthelp "NOTICE $nick :$output"
		}
	}
}

#################################################################################
# CHECK BOUNCER STATUSES                                                        #
#################################################################################
proc ng_bnc_check {nick uhost hand chan arg} {
	global binary bnc
	checkchan $nick $chan

	# We should probably just not bind at all, but this is easier.
	# (It's easier since we won't have to deal with unbinding etc)
	if {![istrue $bnc(ENABLED)]} {return}

	putquick "NOTICE $nick :Checking bouncer(s) status..."
	set count 0
	foreach i $bnc(LIST) {
		incr count
		set i [split $i ":"]
		set loc [lindex $i 0]
		set ip [lindex $i 1]
		set port [lindex $i 2]

		if {[istrue $bnc(PING)]} {
			if {[catch {set data [exec $binary(PING) -c1 $ip]} error]} {
			    putlog "dZSbot warning: Unable to ping $ip ($error)."
				putquick "NOTICE $nick :$count. .$loc - $ip:$port - DOWN (Can't ping host)"
				continue
			}
			set ping ", ping: [format %.1f [lindex [split [lindex [lindex [split $data \"\n\"] 1] 6] \"=\"] 1]]ms"
		} else { set ping "" }

		set dur [clock clicks -milliseconds]
		set exitlevel [catch {exec $binary(NCFTPLS) -P $port -u $bnc(USER) -p $bnc(PASS) -t $bnc(TIMEOUT) -r 0 ftp://$ip 2>@ stdout} raw]
		set dur [expr [clock clicks -milliseconds] - $dur]

		if {$exitlevel == 0} {
			putquick "NOTICE $nick :$count. .$loc - $ip:$port - UP (login: [format %.0f $dur]ms$ping)"
		} else {
			switch -glob -- $raw {
				"*username was not accepted for login.*" {set error "Bad Username"}
				"*username and/or password was not accepted for login.*" {set error "Couldn't login"}
				"*Connection refused.*" {set error "Connection Refused"}
				"*try again later: Connection timed out.*" {set error "Connection Timed Out"}
				"*timed out while waiting for server response.*" {set error "No response"}
				"*Remote host has closed the connection.*" {set error "Connection Lost"}
				"*unknown host.*" {set error "Unknown Host?"}
				default {
					set error "Unknown Error"
					putlog "dZSbot error: Unknown bnc check error \"$raw\", please report to pzs-ng developers."
				}
			}
			putquick "NOTICE $nick :$count. .$loc - $ip:$port - DOWN ($error)"
		}
		set error ""
		set raw ""
	}
}

#################################################################################
# Help Section                                                                  #
#################################################################################
proc help {nick uhost hand chan arg} {
	global scriptpath sections
	checkchan $nick $chan

	set file "$scriptpath/dZSbot.help"
	if {![file readable $file]} {
		puthelp "PRIVMSG $nick : File dZSbot.help is missing, please check install"
		puthelp "PRIVMSG $nick : (file should reside in same dir as dZSbot.tcl)"
		return 0
	}

	set helpfile [open $file r]
	set helpdb [read $helpfile]
	close $helpfile
	foreach line [split $helpdb "\n"] {
		set line [themereplace [basicreplace $line "HELP"] "none"]
		puthelp "PRIVMSG $nick :$line"
	}
	puthelp "PRIVMSG $nick :Valid sections are: [join [lsort -ascii $sections] {, }]"
}

#################################################################################
# LOAD A THEME FILE                                                             #
#################################################################################
proc loadtheme {file} {
	global announce scriptpath theme theme_fakes
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

	foreach type {COLOR1 COLOR2 COLOR3 PREFIX KB KBIT MB MBIT} {
		if {[lsearch -exact [array names theme] $type] == -1} {
			putlog "dZSbot error: Missing required theme setting \"$type\", failing."
			return 0
		}
	}
	return 1
}

#################################################################################
# REPLACES THEMERELATED STUFF IN A GIVEN STRING, STATIC REPLACE FOR STARTUP     #
#################################################################################
proc themereplace_startup {rstring} {
	global theme

	# We replace %b{string} and %u{string} with their bolded and underlined equivilants ;)
	while {[regexp {(%b\{([^\{\}]+)\}|%u\{([^\{\}]+)\})} $rstring]} {
		regsub -all {%b\{([^\{\}]+)\}} $rstring {\\002\1\\002} rstring
		regsub -all {%u\{([^\{\}]+)\}} $rstring {\\037\1\\037} rstring
	}

	return [subst -nocommands $rstring]
}

#################################################################################
# REPLACES THEMERELATED STUFF IN A GIVEN STRING, DYNAMIC REPLACE FOR RUNTIME    #
#################################################################################
proc themereplace {targetString section} {
	global theme

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

#################################################################################
# START UP STUFF                                                                #
#################################################################################

if {[istrue $enable_irc_invite]} {
	bind msg -|- !invite invite
}

if {[info exists dZStimer]} {
	if {[catch {killutimer $dZStimer} error]} {
		putlog "dZSbot warning: Unable to kill log timer ($error)."
		putlog "dZSbot warning: You should .restart the bot to be safe."
	}
}
set dZStimer [utimer 1 "readlog"]

if {![loadtheme $announce(THEMEFILE)]} {
	if {[loadtheme "default.zst"]} {
		putlog "dZSbot warning: Unable to load theme $announce(THEMEFILE), loaded default.zst instead."
	} else {
		putlog "dZSbot error: Unable to load the themes $announce(THEMEFILE) and default.zst."
		set dzerror 1
	}
}

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
