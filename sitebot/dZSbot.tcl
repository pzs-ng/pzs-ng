#################################################################################
# Code part only, main config is moved to dZSbconf.tcl :)                       #
# THIS MEANS THAT YOU ARENT SUPPOSED TO EDIT THIS FILE                          #
# FOR CONFIGURATION PURPOSES!                                                   #
#################################################################################

#################################################################################
# READING THE CONFIG (and NOT being a cryptic bitch ;)                          #
#################################################################################
set dver "0.0.4"
set dzerror "0"
set pid 0
set tclroot [file dirname [info script]]
putlog "Launching dZSBot (v$dver) for zipscript-c..."

if {[catch {source [file dirname [info script]]/dZSbconf.tcl} tmperror]} {
	putlog "dZSbot: dZSbconf.tcl not found or has errors. Cannot continue."
	putlog "dZSbot: See FAQ for possible solutions/debugging options."
	die
}
if {[catch {source [file dirname [info script]]/dZSbvars.tcl} tmperror]} {
	putlog "dZSbot: dZSbvars.tcl not found or has errors. Cannot continue."
	putlog "dZSbot: See FAQ for possible solutions/debugging options."
	die
}

foreach bin [array names binary] {
	if {![file executable $binary($bin)]} {
		putlog "dZSbot: Wrong path/missing bin for $bin - Please fix."
		set dzerror "1"
	}
}

## READ LOGFILES
set countlog 0
foreach log [array names glftpdlog] {
    if {![file exists $glftpdlog($log)]} {
	putlog "dZSbot: Could not find log file $glftpdlog($log)."
	set dzerror "1"
	unset glftpdlog($log)
    } else {
	set countlog [expr $countlog + 1]
	set lastoct($log) [file size $glftpdlog($log)]
    }
}
if { $countlog == 0 } {
	putlog "dZSbot: WARNING! No gl logfiles found!"
	set dzerror "1"
} else {
	putlog "dZSbot: Number of gl logfiles found: $countlog"
}

set countlog 0
foreach login [array names loginlog] {
    if {![file exists $loginlog($login)]} {
	putlog "dZSbot: Could not find log file $loginlog($login)."
	set dzerror "1"
	unset loginlog($login)
    } else {
	set countlog [expr $countlog + 1]
	set loglastoct($login) [file size $loginlog($login)]
    }
}
if { $countlog == 0 } {
	putlog "dZSbot: WARNING! No login logfiles found!"
} else {
	putlog "dZSbot: Number of login logfiles found: $countlog"
}


if {![info exists use_glftpd2] || ($use_glftpd2 == "AUTO" && ![info exists binary(GLFTPD)])} {
	putlog "dZSbot: you did not thouroughly edit your [file dirname [info script]]/dZSbconf.tcl file. Try again."
	die
}

if {$use_glftpd2 == "AUTO"} {
	set glversion [exec strings $binary(GLFTPD) | grep -i "^glftpd " | cut -f1 -d. | tr A-Z a-z]
	if {$glversion == "glftpd 1"} {
		putlog "dZSbot: detected $glversion, running in legacy mode."
		set use_glftpd2 "NO"
	} elseif {$glversion == "glftpd 2"} {
		putlog "dZSbot: detected $glversion, running in standard mode."
		set use_glftpd2 "YES"
	} else {
		putlog "dZSbot: autodetecting glftpd-version failed. Set use_glftpd in [file dirname [info script]]/dZSbconf.tcl manually."
	}
}

if {![info exists invite_channels] && [info exists chanlist(INVITE)]} {
	putlog "dZSbot: no invite_channels variable found in config, setting to \"$chanlist(INVITE)\" (chanlist(INVITE))"
	set invite_channels $chanlist(INVITE)
}

#################################################################################
# SOME IMPORTANT GLOBAL VARIABLES                                               #
#################################################################################

set defaultsection "DEFAULT"
set nuke(LASTTYPE) ""
set nuke(LASTDIR) ""
set nuke(SHOWN) 1
set variables(NUKE)   ""
set variables(UNNUKE) ""
set mpath ""

#################################################################################
# SET BINDINGS                                                                  #
#################################################################################

# Where?	Flags	What?			Proc to call
bind pub	-|-	[set cmdpre]who		who
bind pub	-|-	[set cmdpre]speed	speed
bind pub	-|-	[set cmdpre]bw		ng_bandwidth
bind pub	-|-	[set cmdpre]bwup	ng_bwup
bind pub    	-|- 	[set cmdpre]uploaders	ng_uploaders
bind pub    	-|- 	[set cmdpre]up		ng_uploaders
bind pub	-|-	[set cmdpre]bwdn	ng_bwdn
bind pub	-|- 	[set cmdpre]leechers	ng_leechers
bind pub	-|- 	[set cmdpre]downloaders	ng_leechers
bind pub	-|- 	[set cmdpre]down	ng_leechers
bind pub	-|- 	[set cmdpre]dn		ng_leechers
bind pub	-|-	[set cmdpre]idlers	ng_idlers
bind pub	-|-	[set cmdpre]idle	ng_idlers
bind pub	-|-	[set cmdpre]bnc		ng_bnc_check
bind pub	-|-	[set cmdpre]free	show_free
bind pub	-|-	[set cmdpre]df		show_free

bind pub	-|-	[set cmdpre]dayup	stats_user_dayup
bind pub	-|-	[set cmdpre]wkup	stats_user_wkup
bind pub	-|-	[set cmdpre]monthup	stats_user_monthup
bind pub	-|-	[set cmdpre]allup	stats_user_allup

bind pub	-|-	[set cmdpre]daydn	stats_user_daydn
bind pub	-|-	[set cmdpre]wkdn	stats_user_wkdn
bind pub	-|-	[set cmdpre]monthdn	stats_user_monthdn
bind pub	-|-	[set cmdpre]alldn	stats_user_alldn

bind pub	-|-	[set cmdpre]gpwk	stats_group_gpwk
bind pub	-|-	[set cmdpre]gpal	stats_group_gpal
bind pub	-|-	[set cmdpre]inc		show_incompletes
bind pub	-|-	[set cmdpre]incomplete	show_incompletes
bind pub	-|-	[set cmdpre]incompletes	show_incompletes

bind pub	-|-	[set cmdpre]gwpd	stats_group_gpwd
bind pub	-|-	[set cmdpre]gpad	stats_group_gpad
bind pub	-|-	[set cmdpre]help	help

bind join	-|-	*			welcome_msg

bind pub	-|-	[set cmdpre]dupe	ng_search
bind pub	-|-	[set cmdpre]new		ng_new
bind pub	-|-	[set cmdpre]nukes	ng_nukes
bind pub	-|-	[set cmdpre]search	ng_search
bind pub	-|-	[set cmdpre]unnukes	ng_unnukes

if {$bindnopre == "YES"} {
	bind pub	-|- !who		who
	bind pub	-|- !speed		speed
	bind pub	-|- !bw			ng_bandwidth
	bind pub	-|- !bwdn		ng_bwdn
	bind pub	-|- !uploaders		ng_uploaders
	bind pub	-|- !up			ng_uploaders
	bind pub	-|- !bwup		ng_bwup
	bind pub	-|- !leechers		ng_leechers
	bind pub	-|- !downloaders	ng_leechers
	bind pub	-|- !down		ng_leechers
	bind pub	-|- !dn			ng_leechers
	bind pub	-|- !idlers		ng_idlers
	bind pub	-|- !idle		ng_idlers
	bind pub	-|- !bnc		ng_bnc_check
	bind pub	-|- !free		show_free
	bind pub	-|- !df			show_free

	bind pub	-|- !dayup		stats_user_dayup
	bind pub	-|- !wkup		stats_user_wkup
	bind pub	-|- !monthup		stats_user_monthup
	bind pub	-|- !allup		stats_user_allup

	bind pub	-|- !daydn		stats_user_daydn
	bind pub	-|- !wkdn		stats_user_wkdn
	bind pub	-|- !monthdn		stats_user_monthdn
	bind pub	-|- !alldn		stats_user_alldn

	bind pub	-|- !gpwk		stats_group_gpwk
	bind pub	-|- !gpal		stats_group_gpal
	bind pub	-|- !inc		show_incompletes
	bind pub	-|- !incomplete		show_incompletes
	bind pub	-|- !incompletes	show_incompletes

	bind pub	-|- !gwpd		stats_group_gpwd
	bind pub	-|- !gpad		stats_group_gpad
	bind pub	-|- !help		help

	bind pub	-|- !dupe		ng_search
	bind pub	-|- !new		ng_new
	bind pub	-|- !nukes		ng_nukes
	bind pub	-|- !search		ng_search
	bind pub	-|- !unnukes		ng_unnukes

} else {
	catch { unbind pub    -|- !who		who }
	catch { unbind pub    -|- !speed	speed }
	catch { unbind pub    -|- !bw		ng_bandwidth }
	catch { unbind pub    -|- !bwup		ng_bwup }
	catch { unbind pub    -|- !uploaders	ng_uploaders }
	catch { unbind pub    -|- !up		ng_uploaders }
	catch { unbind pub    -|- !bwdn		ng_bwdn }
	catch { unbind pub    -|- !leechers	ng_leechers }
	catch { unbind pub    -|- !downloaders	ng_leechers }
	catch { unbind pub    -|- !down		ng_leechers }
	catch { unbind pub    -|- !dn		ng_leechers }
	catch { unbind pub    -|- !idlers	ng_idlers }
	catch { unbind pub    -|- !idle		ng_idlers }
	catch { unbind pub    -|- !bnc		ng_bnc_check }
	catch { unbind pub    -|- !free		show_free }
	catch { unbind pub    -|- !df		show_free }
	catch { unbind pub    -|- !dayup	stats_user_dayup }
	catch { unbind pub    -|- !wkup		stats_user_wkup }
	catch { unbind pub    -|- !monthup	stats_user_monthup }
	catch { unbind pub    -|- !allup	stats_user_allup }

	catch { unbind pub    -|- !daydn	stats_user_daydn }
	catch { unbind pub    -|- !wkdn		stats_user_wkdn }
	catch { unbind pub    -|- !monthdn	stats_user_monthdn }
	catch { unbind pub    -|- !alldn	stats_user_alldn }

	catch { unbind pub    -|- !gpwk		stats_group_gpwk }
	catch { unbind pub    -|- !gpal		stats_group_gpal }
	catch { unbind pub    -|- !inc		show_incompletes }
	catch { unbind pub    -|- !incomplete	show_incompletes }
	catch { unbind pub    -|- !incompletes	show_incompletes }

	catch { unbind pub    -|- !gwpd		stats_group_gpwd }
	catch { unbind pub    -|- !gpad		stats_group_gpad }
	catch { unbind pub    -|- !help		help }

	catch { unbind pub    -|- !dupe		ng_search}
	catch { unbind pub    -|- !new		ng_new}
	catch { unbind pub    -|- !nukes	ng_nukes}
	catch { unbind pub    -|- !search	ng_search}
	catch { unbind pub    -|- !unnukes	ng_unnukes}
}

## Some 'constants'
proc DEFAULT_LEVEL {{string 0}} {
	if {$string} { return [DEBUG_INFO 1];
	} else { return [DEBUG_INFO]; }
}

proc DEBUG_INFO {{string 0}} {
	if {$string} { return "D/iNFO";
	} else { return 2; }
}
proc DEBUG_WARN {{string 0}} {
	if {$string} { return "D/WARNiNG";
	} else { return 1; }
}
proc DEBUG_ERROR {{string 0}} {
	if {$string} { return "D/ERROR";
	} else { return 0; }
}
proc DEBUG_FATAL {{string 0}} {
	if {$string} { return "D/FATAL";
	} else { return 0; }
}

set debuglevel [DEBUG_INFO]


#################################################################################
# MAIN LOOP - PARSES DATA FROM GLFTPD.LOG                                       #
# Modified: 2004-11-18 by Zenuka
#################################################################################

proc readlog {} {

	global location glftpdlog loginlog lastoct disable defaultsection variables msgtypes chanlist dZStimer use_glftpd2 invite_channels loglastoct pid msgreplace privchannel privgroups privusers

	set dZStimer [utimer 1 "readlog"]
	set lines ""

	foreach log [array names glftpdlog] {
	    if {$lastoct($log) < [file size $glftpdlog($log)]} {
		if {![catch {set of [open $glftpdlog($log) r]} ]} {
			seek $of $lastoct($log)
			while {![eof $of]} {
				set line [gets $of]
				if {$line == ""} { continue; }
				lappend lines $line
			}
			close $of
		} else {
			putlog "dZSbot error: Could not open GLLOG: $glftpdlog($log)"
			return 0
		}
	    }
	    set lastoct($log) [file size $glftpdlog($log)]
	}

	foreach login [array names loginlog] {
	    if {$loglastoct($login) < [file size $loginlog($login)]} {
		if {![catch {set of [open $loginlog($login) r]} ]} {
			seek $of $loglastoct($login)
			while {![eof $of]} {
				set line [gets $of]
				if {$line == ""} { continue; }
				lappend lines $line
			}
			close $of
		} else {
			putlog "dZSbot error: Could not open LOGINLOG: $loginlog($login)"
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
		if {[regexp {\[([0-9\ ]+)\]} "$line" dud pid]} {
			regsub "\\\[$pid\\\] " "$line" "" line
			set pid [string trim $pid]
		}

		# If we cannot detect a msgtype - default to DEBUG: and insert that into the list ($line).
		if {[string first ":" [lindex $line 5]] < 0} {
			set msgtype "DEBUG"
			if {[llength $line] < 5} {
				set line [lappend $line "dummy debug"]
			} else {
				set line [linsert $line 5 "DEBUG:"]
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

		set path [lindex $line 6]

		# Invite users to public and private channels
		if {![string compare $msgtype "INVITE"]} {
			set ircnick [lindex $line 6]
			set nick [lindex $line 7]
			set group [lindex $line 8]
			foreach channel $invite_channels { puthelp "INVITE $ircnick $channel" }
			if {[info exists privchannel]} {
				foreach privchan [array names privchannel] {
					foreach privgroup $privgroups($privchan) {
						if {![string compare $group $privgroup]} {
							foreach channel $privchannel($privchan) { puthelp "INVITE $ircnick $channel" }
						}
					}
					foreach privuser $privusers($privchan) {
						if {![string compare $nick $privuser]} {
							foreach channel $privchannel($privchan) { puthelp "INVITE $ircnick $channel" }
						}
					}
				}
			}
		}

		set section [getsection $path $msgtype]

		# Replace messages with custom messages
		foreach rep [array names msgreplace] {
			set rep [split $msgreplace($rep) ":"]
			if {![string compare $msgtype [lindex $rep 0]]} {
				if {[string match -nocase [lindex $rep 1] $path]} {
					set msgtype [lindex $rep 2]
				}
			}
		}

		if {[denycheck "$path"] == 0} {
			if {[string compare "$section" "$defaultsection"]} {
				if {[info exists variables($msgtype)] && $disable($msgtype) == 0} {
					set echoline [parse $msgtype [lrange $line 6 end] $section]
					sndall $section $echoline
					postcmd $msgtype $section $path
				} else {
					if {![info exists variables($msgtype)] && $pid == 0} {
						putlog "dZSbot error: \"variables($msgtype)\" not set in config, type becomes \"DEFAULT\""
					}
					if {![info exists variables($msgtype)] && $disable(DEFAULT) == 0 && $pid == 0} {
						set echoline [parse DEFAULT [lrange $line 6 end] $section]
						sndall $section $echoline
						postcmd $msgtype $section $path
					}
				}
			} else {
				if {[lsearch -glob $msgtypes(DEFAULT) $msgtype] != -1} {
					if {$disable($msgtype) == 0} {
						set echoline [parse $msgtype [lrange $line 6 end] "DEFAULT"]
						if { [info exists chanlist($msgtype)] } {
							sndall "$msgtype" $echoline
						} else {
							sndall "DEFAULT" $echoline
						}
						postcmd $msgtype "DEFAULT" $path
					}
				} else {
					if {$disable(DEFAULT) == 0} {
						set echoline [parse $msgtype [lrange $line 6 end] "DEFAULT"]
						sndall "DEFAULT" $echoline
						postcmd $msgtype "DEFAULT" $path
					} else {
						if {![info exists variables($msgtype)] && $pid > 0} {
							set echoline [parse $msgtype [lrange $line 6 end] $section]
							sndall $section $echoline
							postcmd $msgtype $section $path
						}
					}
				}
			}
		}
	}

	if {$use_glftpd2 != "YES"} {
		launchnuke
	}

	return 0
}

#################################################################################

#################################################################################
# POST COMMAND                                                                  #
#################################################################################
proc postcmd {msgtype section path} {
	global postcommand

	if {[info exists postcommand($msgtype)]} {
		foreach cmd $postcommand($msgtype) {
			if {[lindex $cmd 0] == "exec"} { set cmd [lindex $cmd 1] ; set isexec 1}
			if {[info exists isexec]} {
				if {[catch {exec $cmd $msgtype $section $path} result] != 0} {
					putlog "dZSbot error: exec \"$cmd\" caused an error - \"$result\""
					unset isexec
				}
			} else {
				if {[catch {$cmd $msgtype $section $path} result] != 0} {
					putlog "dZSbot error: \"$cmd\" caused an error - \"$result\""
				}
			}
		}
	}
}

#################################################################################
# GET SECTION NAME (BASED ON PATH)                                              #
#################################################################################
proc getsection {cpath msgtype} {
	global sections msgtypes paths type defaultsection mpath

	foreach section $sections {
		if {![llength [array names "paths" $section]]} {
			putlog "dZSbot error: \"paths($section)\" not set in config, section becomes \"$defaultsection\""
			continue
		}

		foreach path $paths($section) {
			if {[string match $path $cpath] == 1 && [string first $msgtype $msgtypes($type($section))] != -1} {
				set mpath $path
				return $section
			}
		}
	}
	return $defaultsection
}
#################################################################################


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


#################################################################################
# CONVERT ANYTHING>MB TO MEGABYTES                                              #
#################################################################################
proc to_mb {str} {
	set type [string index $str end]
	if {($type == "b" || $type == "B") && [string is alpha [string index $str end-1]]} {
		set type [string index $str end-1]
		set size [string range $str 0 end-2]
	} else { set size [string range $str 0 end-1] }

	set factor 0
	switch -regexp [string index $str end] {
		[mM] { set factor 1 }
		[gG] { set factor 1000 }
		[tT] { set factor 1000000 }
	}
	if {$factor == 0} { return -1 }
	return [expr round($size*$factor)]
}
#################################################################################


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


#################################################################################
# CONVERT SPEED UNIT TO A CUSTOMIZED UNIT                                       #
#################################################################################
proc speed_convert {value section} {

	global speedmeasure speedthreshold theme

	switch -exact -- [string tolower $speedmeasure] {

		"mb"		{
					set value [format "%.2f" [expr $value / 1024.0]]
					set type $theme(MB)
				}
		"kbit"		{
					set value [expr $value * 8]
					set type $theme(KBIT)
				}
		"mbit"		{
					set value [format "%.1f" [expr $value * 8 / 1000.0]]
					set type $theme(MBIT)
				}
		"autobit"	{
					if {$value > $speedthreshold} {
						set value [format "%.1f" [expr $value * 8 / 1000.0]]
						set type $theme(MBIT)
					} else {
						set value [expr $value * 8]
						set type $theme(KBIT)
					}
				}
		"autobyte"	{
					if {$value > $speedthreshold} {
						set value [format "%.2f" [expr $value / 1024.0]]
						set type $theme(MB)
					} else {
						set type $theme(KB)
					}
				}
		"disabled"	{
					set type ""
				}
		"default"	{
					set type $theme(KB)
				}

	}

	return [themereplace "$value$type" $section]

}
#################################################################################


#################################################################################
# CONVERT BASIC COOKIES TO DATA                                                 #
#################################################################################
proc basicreplace {rstring section} {
	global sitename

	set output $rstring
	set output [replacevar $output "%sitename" $sitename]
	set output [replacevar $output "%bold" "\002"]
	set output [replacevar $output "%uline" "\037"]
	set output [replacevar $output "%section" $section]
	return "$output"
}
#################################################################################


#################################################################################
# JUSTIFY AND PAD OUTPUT                                                        #
#################################################################################
proc justifyandpad {targetString} {
	while {[regexp {%([lrm])(\d\d?)\{([^\{\}]+)\}} $targetString matchString padOp padLength padString]} {
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

	return $targetString
}


#################################################################################
# CONVERT COOKIES TO DATA                                                       #
#################################################################################
proc parse {msgtype msgline section} { global variables announce random mpath use_glftpd2 theme theme_fakes defaultsection pid disable sitename
	set type $msgtype

	if {![string compare $type "NUKE"] || ! [string compare $type "UNNUKE"]} {
		if { $use_glftpd2 != "YES" } {
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
			if { $disable(FAILLOGIN) == 1 } {
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
			if { $disable(FAILLOGIN) == 1 } {
				return ""
			} else {
				set f_user $type
				set type "FAILLOGIN"
			}
		}
	}

	set vars $variables($type)

	if {![string compare [lindex $announce($type) 0] "random"] && [string is alnum -strict [lindex $announce($type) 1]] == 1} {
		set output $random($msgtype\-[rand [lindex $announce($type) 1]])
	} else {
		set output $announce($type)
	}

	set output [themereplace "$theme(PREFIX)$output" $section]
	if {![string compare $section $defaultsection] && [llength [array names "theme_fakes" "$type"]] > 0} { set section $theme_fakes($type) }
	set output [basicreplace $output $section]
	set cnt 0

	if {[ string compare [lindex $vars 0] "%pf" ] == 0} {
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
	} elseif {[ string compare [lindex $vars 0] "%failed_nick" ] == 0 && $pid != 0} {
		set output [replacevar $output "%failed_nick" $f_user ]
		set vars [string range $vars 13 end]
		set ip1 [lindex $msgline 1]
		set ip2 [string range $ip1 1 [expr [string length $ip1] - 3]]
		set msgline [replacevar $msgline $ip1 $ip2]
		set cnt 0
	}

	set loop 1

	foreach vari $vars {
		if { [llength $vari] > 1 } {
			set cnt2 0
			set cnt3 1
			set values [lindex $msgline $cnt]
			set output2 ""
			foreach value $values {
				if { $cnt2 == 0 } { append output2 "[themereplace $announce(${type}_LOOP${loop}) $section]" }
				if { [string match "*speed" [lindex $vari $cnt2]] } {
					set output2 [replacevar $output2 "[lindex $vari $cnt2]" [speed_convert $value $section]]
				} else {
					set output2 [replacevar $output2 "[lindex $vari $cnt2]" $value]
				}
				set cnt2 [expr $cnt2 + 1]
				if { [lindex $vari $cnt2] ==  "" } {
					set cnt3 [expr $cnt3 + 1]
					set cnt2 0
				}
			}
			set output2 [replacevar $output2 "%section" $section]
			set output2 [replacevar $output2 "%sitename" $sitename]
			set output2 [replacevar $output2 "%splitter" $theme(SPLITTER)]
			set output2 [string trim $output2 "$theme(SPLITTER)"]
			set output [replacevar $output "%loop$loop" $output2]
			set loop [expr $loop + 1]
		} else {
			if { [string match "*speed" $vari] } {
				set output [replacevar $output $vari [speed_convert [lindex $msgline $cnt] $section]]
			} else {
				set output [replacevar $output $vari [lindex $msgline $cnt]]
			}
		}
		set cnt [expr $cnt + 1]
	}

	set output [themereplace $output $section]
	return $output
}
#################################################################################

#################################################################################
# Channel trigger check.                                                        #
#################################################################################
proc checkchan {chan} {
	global disable mainchan
	## Better than including this in every proc!
	if {$disable(TRIGINALLCHAN) == 1 && [string equal -nocase $chan $mainchan]} {
		return -code return
	}
}
#################################################################################


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


#################################################################################
# Display the latest releases.                                                  #
#################################################################################
proc ng_new {nick uhost hand chan argv} {
	global announce binary defaultsection lastbind location sections theme
	checkchan $chan

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
			puthelp "PRIVMSG $nick :Invalid section, sections: \002[join [lsort -ascii $sections] {, }]\002"
			return
		}
		foreach {section sectionpath} $sectiondata {break}
		set lines [exec $binary(SHOWLOG) -f -l -m $results -p $sectionpath -r $location(GLCONF)]
	}

	## Retrieve a list of UIDs/users and GIDs/groups
	array set uid [gl_userids]
	array set gid [gl_groupids]

	set output [themereplace "$theme(PREFIX)$announce(NEW)" "none"]
	set output [replacevar $output "%section" $section]
	sndone $nick [basicreplace $output "NEW"]
	set body [themereplace "$theme(PREFIX)$announce(NEW_BODY)" "none"]
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
		set output [themereplace "$theme(PREFIX)$announce(NEW_NONE)" "none"]
		sndone $nick [basicreplace $output "NEW"]
	}
}
#################################################################################


#################################################################################
# Search the dirlog for releases.                                               #
#################################################################################
proc ng_search {nick uhost hand chan argv} {
	global announce binary defaultsection lastbind location search_chars theme
	checkchan $chan

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

	set output [themereplace "$theme(PREFIX)$announce(SEARCH)" "none"]
	set output [replacevar $output "%pattern" $pattern]
	sndone $nick [basicreplace $output "SEARCH"]
	set body [themereplace "$theme(PREFIX)$announce(SEARCH_BODY)" "none"]
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
		set output [themereplace "$theme(PREFIX)$announce(SEARCH_NONE)" "none"]
		sndone $nick [basicreplace $output "SEARCH"]
	}
}
#################################################################################


#################################################################################
# Display the latest nukes.                                                     #
#################################################################################
proc ng_nukes {nick uhost hand chan argv} {
	global announce binary defaultsection lastbind location sections theme
	checkchan $chan

	if {![getoptions $argv results section]} {
		puthelp "PRIVMSG $chan :\002Usage:\002 $lastbind \[-max <num>\] \[section\]"
		return
	}
	if {$section == ""} {
		set section $defaultsection
		set lines [exec $binary(SHOWLOG) -n -m $results -r $location(GLCONF)]
	} else {
		if {[set sectiondata [getsectionpath $section]] == ""} {
			puthelp "PRIVMSG $nick :Invalid section, sections: \002[join [lsort -ascii $sections] {, }]\002"
			return
		}
		foreach {section sectionpath} $sectiondata {break}
		set lines [exec $binary(SHOWLOG) -f -n -m $results -p $sectionpath -r $location(GLCONF)]
	}

	set output [themereplace "$theme(PREFIX)$announce(NUKES)" "none"]
	set output [replacevar $output "%section" $section]
	sndone $nick [basicreplace $output "NUKES"]
	set body [themereplace "$theme(PREFIX)$announce(NUKES_BODY)" "none"]
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
		set output [themereplace "$theme(PREFIX)$announce(NUKES_NONE)" "none"]
		sndone $nick [basicreplace $output "NUKES"]
	}
}
#################################################################################


#################################################################################
# Display the latest unnukes.                                                   #
#################################################################################
proc ng_unnukes {nick uhost hand chan argv} {
	global announce binary defaultsection lastbind location sections theme
	checkchan $chan

	if {![getoptions $argv results section]} {
		puthelp "PRIVMSG $chan :\002Usage:\002 $lastbind \[-max <num>\] \[section\]"
		return
	}
	if {$section == ""} {
		set section $defaultsection
		set lines [exec $binary(SHOWLOG) -u -m $results -r $location(GLCONF)]
	} else {
		if {[set sectiondata [getsectionpath $section]] == ""} {
			puthelp "PRIVMSG $nick :Invalid section, sections: \002[join [lsort -ascii $sections] {, }]\002"
			return
		}
		foreach {section sectionpath} $sectiondata {break}
		set lines [exec $binary(SHOWLOG) -f -u -m $results -p $sectionpath -r $location(GLCONF)]
	}

	set output [themereplace "$theme(PREFIX)$announce(UNNUKES)" "none"]
	set output [replacevar $output "%section" $section]
	sndone $nick [basicreplace $output "UNNUKES"]
	set body [themereplace "$theme(PREFIX)$announce(UNNUKES_BODY)" "none"]
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
		set output [themereplace "$theme(PREFIX)$announce(UNNUKES_NONE)" "none"]
		sndone $nick [basicreplace $output "UNNUKES"]
	}
}
#################################################################################


#################################################################################
# SEND TO ALL CHANNELS LISTED                                                   #
#################################################################################
proc sndall {section args} {
	global chanlist splitter
	foreach chan $chanlist($section) {
		foreach line [split [lindex $args 0] $splitter(CHAR)] {
			putquick "PRIVMSG $chan :$line"
		}
	}
}
#################################################################################


#################################################################################
# SEND TO ONE CHANNEL                                                           #
#################################################################################
proc sndone {chan args} {
	global splitter
	foreach line [split [lindex $args 0] $splitter(CHAR)] {
		putquick "PRIVMSG $chan :$line"
	}
}
#################################################################################


#################################################################################
# POST WHO INFO                                                                 #
#################################################################################
proc who {nick uhost hand chan args} {
	global binary disable mainchan

	if { $disable(TRIGINALLCHAN) == 1 } {
		if {[string match -nocase $chan $mainchan] == 0} {
			return 0
		}
	}
	foreach line [split [exec $binary(WHO)] \n] {
		if {![info exists newline($line)]} {
			set newline($line) 0
		} else { set newline($line) [expr $newline($line) + 1] }
		puthelp "PRIVMSG $nick :$line\003$newline($line)"
	}
	puthelp "PRIVMSG $nick : "
}
#################################################################################


#################################################################################
# POST SPEED                                                                    #
#################################################################################
proc speed {nick uhost hand chan args} {
	global binary announce theme disable mainchan

	if { $disable(TRIGINALLCHAN) == 1 } {
		if {[string match -nocase $chan $mainchan] == 0} {
			return 0
		}
	}

	set line ""
	if { $disable(ALTWHO) == 0 } {
		set output "[themereplace "$theme(PREFIX)$announce(SPEEDERROR)" "none"]"
		foreach line [split [exec $binary(WHO) --raw [lindex $args 0]] "\n"] {
			set action [lindex $line 4]
			if {$action == "DN"} {
				set output "[themereplace "$theme(PREFIX)$announce(SPEEDDN)" "none"]"
				set output [replacevar $output "%dnspeed" [speed_convert [lindex $line 5] "none"]]
				set output [replacevar $output "%dnpercent" [lindex $line 9]]
			} elseif {$action == "UP"} {
				set output "[themereplace "$theme(PREFIX)$announce(SPEEDUP)" "none"]"
				set output [replacevar $output "%upspeed" [speed_convert [lindex $line 5] "none"]]
				set output [replacevar $output "%uppercent" [lindex $line 9]]
			} elseif {$action == "ID"} {
				set output "[themereplace "$theme(PREFIX)$announce(SPEEDID)" "none"]"
				set output [replacevar $output "%idletime" [lindex $line 5]]
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
		set base_output "[themereplace "$theme(PREFIX)$announce(DEFAULT)" "none"]"
		foreach line [split [exec $binary(WHO) [lindex $args 0]] "\n"] {
			set output [replacevar $base_output "%msg" $line]
			set output [basicreplace $output "SPEED"]
			sndone $chan $output
		}
	}

	if {$line == ""} {
		set output "[themereplace "$theme(PREFIX)$announce(SPEEDERROR)" "none"]"
#		set output "[themereplace "$theme(PREFIX)$announce(DEFAULT)" "none"]"
		set output [replacevar $output "%msg" "User not online."]
		set output [basicreplace $output "SPEED"]
		sndone $chan $output
        }

}
#################################################################################


#################################################################################
# POST BANDWIDTH                                                                #
#################################################################################
proc bandwidth {nick uhost hand chan args} {
	global binary announce theme disable mainchan

	if { $disable(TRIGINALLCHAN) == 1 } {
		if {[string match -nocase $chan $mainchan] == 0} {
			return 0
		}
	}
	set output "[themereplace "$theme(PREFIX)$announce(BW)" "none"]"
	set data [exec $binary(WHO) --nbw]
	set output [replacevar $output "%uploads" [lindex $data 0]]
	set output [replacevar $output "%downloads" [lindex $data 2]]
	set output [replacevar $output "%transfers" [lindex $data 4]]
	set output [replacevar $output "%upspeed" [speed_convert [lindex $data 1] "none"]]
	set output [replacevar $output "%dnspeed" [speed_convert [lindex $data 3] "none"]]
	set output [replacevar $output "%totalspeed" [speed_convert [lindex $data 5] "none"]]
	set output [basicreplace "$output" "BW"]

	sndone $chan $output
}
#################################################################################


#################################################################################
# uploaders BANDWIDTH                                                           #
#################################################################################
proc ng_bwup { nick uhost hand chan args} {
	global binary announce speed theme disable mainchan

	if { $disable(TRIGINALLCHAN) == 1 } {
		if {[string match -nocase $chan $mainchan] == 0} {
			return 0
		}
	}
	set output "[themereplace "$theme(PREFIX)$announce(BWUP)" "none"]"
	set raw [exec $binary(WHO) --nbw]
	set upper [format "%.1f" [expr 100 * ([lindex $raw 1] / $speed(INCOMING))]]
	set dnper [format "%.1f" [expr 100 * ([lindex $raw 3] / $speed(OUTGOING))]]
	set totalper [format "%.0f" [expr 100 * ([lindex $raw 5] / ( $speed(INCOMING) + $speed(OUTGOING)))]]

	set output [replacevar $output "%uploads" [lindex $raw 0]]
	set output [replacevar $output "%upspeed" [speed_convert [lindex $raw 1] "none"]]
	set output [replacevar $output "%downloads" [lindex $raw 2]]
	set output [replacevar $output "%dnspeed" [speed_convert [lindex $raw 3] "none"]]
	set output [replacevar $output "%transfers" [lindex $raw 4]]
	set output [replacevar $output "%totalspeed" [speed_convert [lindex $raw 5] "none"]]
	set output [replacevar $output "%idlers" [lindex $raw 6]]
	set output [replacevar $output "%active" [lindex $raw 7]]
	set output [replacevar $output "%totallogins" [lindex $raw 8]]
	set output [replacevar $output "%maxusers" [lindex $raw 9]]

	set output [replacevar $output "%uppercent" $upper]
	set output [replacevar $output "%dnpercent" $dnper]
#	set output [replacevar $output "%totalpercent" $totalper]

	set output [basicreplace "$output" "BW"]

	sndone $chan $output
}
################################################################################

#################################################################################
# ng_uploaders - Origional by Celerex - Mod/Merge by themolester                #
#################################################################################
proc ng_uploaders {nick uhost hand chan args} {
	global binary announce speed theme disable mainchan

	if { $disable(TRIGINALLCHAN) == 1 } {
		if {[string match -nocase $chan $mainchan] == 0} {
			return 0
		}
	}
	set output "[themereplace "$theme(PREFIX)$announce(UPLOAD)" "none"]"
	set output [basicreplace "$output" "UPLOAD"]
	sndone $chan $output

	set raw [exec $binary(WHO) "--raw"]

	set getsecond 0
	set count 0
	set total 0.0
	foreach line [split $raw "\n"] {

		switch [lindex $line 0] {

			USER {
				switch [lindex $line 4] {

					UP {
						set user  [lindex $line 2]
						set group [lindex $line 3]
						set uspeed [replacevar [lindex $line 5] "KB/s" ""]
						set tagline [lindex $line 6]
						set since [lindex $line 7]
						set filename [lindex $line 8]
						set progress [lindex $line 9]
						set per [format "%.2f%%" [expr double($uspeed) * 100 / double($speed(INCOMING))]]
						set output [replacevar "[themereplace "$theme(PREFIX)$announce(USER)" "none"]" "%u_name" $user]
						set output [replacevar $output "%g_name" $group]
						set output [replacevar $output "%fper" $progress]
						set output [replacevar $output "%uspeed" [speed_convert $uspeed "none"]]
						set output [replacevar $output "%per" $per]
						set output [replacevar $output "%tagline" $tagline]
						set output [replacevar $output "%since" $since]
						set output [replacevar $output "%filename" $filename]
						set output [basicreplace "$output" "UPLOAD"]
						sndone $chan $output
						incr count
						set total [expr $total+$uspeed]
					}
				}
			}
		}
	}
	set per [format "%.1f" [expr double($total) * 100 / double($speed(INCOMING)) ]]

	set output [replacevar "[themereplace "$theme(PREFIX)$announce(TOTUPDN)" "none"]" "%type" "Uploaders:"]
	set output [replacevar $output "%count" $count]
	set output [replacevar $output "%total" [speed_convert $total "none"]]
	set output [replacevar $output "%per" $per]
	set output [basicreplace $output "UPLOAD"]

	sndone $chan $output
}
#################################################################################

#################################################################################
# downloaders BANDWIDTH                                                         #
#################################################################################
proc ng_bwdn { nick uhost hand chan args} {
	global binary announce speed theme disable mainchan

	if { $disable(TRIGINALLCHAN) == 1 } {
		if {[string match -nocase $chan $mainchan] == 0} {
			return 0
		}
	}
	set output "[themereplace "$theme(PREFIX)$announce(BWDN)" "none"]"
	set raw [exec $binary(WHO) --nbw]
	set upper [format "%.1f" [expr 100 * ([lindex $raw 1] / $speed(INCOMING))]]
	set dnper [format "%.1f" [expr 100 * ([lindex $raw 3] / $speed(OUTGOING))]]
	set totalper [format "%.0f" [expr [lindex $raw 5] / ( $speed(INCOMING) + $speed(OUTGOING))]]

	set output [replacevar $output "%uploads" [lindex $raw 0]]
	set output [replacevar $output "%upspeed" [speed_convert [lindex $raw 1] "none"]]
	set output [replacevar $output "%downloads" [lindex $raw 2]]
	set output [replacevar $output "%dnspeed" [speed_convert [lindex $raw 3] "none"]]
	set output [replacevar $output "%transfers" [lindex $raw 4]]
	set output [replacevar $output "%totalspeed" [speed_convert [lindex $raw 5] "none"]]
	set output [replacevar $output "%idlers" [lindex $raw 6]]
	set output [replacevar $output "%active" [lindex $raw 7]]
	set output [replacevar $output "%totallogins" [lindex $raw 8]]
	set output [replacevar $output "%maxusers" [lindex $raw 9]]

	set output [replacevar $output "%uppercent" $upper]
	set output [replacevar $output "%dnpercent" $dnper]
	set output [replacevar $output "%totalpercent" $totalper]

	set output [basicreplace "$output" "BW"]

	sndone $chan $output
}
################################################################################

#################################################################################
# ng_leechers - Origional by Celerex - Mod/Merge by themolester                 #
#################################################################################
proc ng_leechers {nick uhost hand chan args} {
	global binary announce speed theme disable mainchan

	if { $disable(TRIGINALLCHAN) == 1 } {
		if {[string match -nocase $chan $mainchan] == 0} {
			return 0
		}
	}
	set output "[themereplace "$theme(PREFIX)$announce(LEECH)" "none"]"
	set output [basicreplace "$output" "LEECH"]
	sndone $chan $output

	set raw [exec $binary(WHO) "--raw"]

	set getsecond 0
	set count 0
	set total 0.0
	foreach line [split $raw "\n"] {

		switch [lindex $line 0] {

			USER {
				switch [lindex $line 4] {

					DN {
						set user  [lindex $line 2]
						set group [lindex $line 3]
						set uspeed [replacevar [lindex $line 5] "KB/s" ""]
						set tagline [lindex $line 6]
						set since [lindex $line 7]
						set filename [lindex $line 8]
						set per [format "%.2f%%" [expr double($uspeed) * 100 / double($speed(OUTGOING))]]
						set fper [lindex $line 9]
						set output [replacevar "[themereplace "$theme(PREFIX)$announce(USER)" "none"]" "%u_name" $user]
						set output [replacevar $output "%g_name" $group]
						set output [replacevar $output "%fper"	$fper]
						set output [replacevar $output "%uspeed" [speed_convert $uspeed "none"]]
						set output [replacevar $output "%per" $per]
						set output [replacevar $output "%tagline" $tagline]
						set output [replacevar $output "%since" $since]
						set output [replacevar $output "%filename" $filename]
						set output [basicreplace "$output" "LEECH"]
						sndone $chan $output
						incr count
						set total [expr $total+$uspeed]
					}
				}
			}
		}
	}
	set per [format "%.1f" [expr double($total) * 100 / double($speed(OUTGOING)) ]]

	set output [replacevar "[themereplace "$theme(PREFIX)$announce(TOTUPDN)" "none"]" "%type" "Leechers:"]
	set output [replacevar $output "%count" $count]
	set output [replacevar $output "%total" [speed_convert $total "none"]]
	set output [replacevar $output "%per" $per]
	set output [basicreplace "$output" "LEECH"]

	sndone $chan $output
}
#################################################################################

#################################################################################
# ng_idlers - Origional by Celerex - Mod/Merge by themolester                   #
#################################################################################
proc ng_idlers { nick uhost hand chan args} {
	global binary announce speed minidletime theme disable mainchan

	if { $disable(TRIGINALLCHAN) == 1 } {
		if {[string match -nocase $chan $mainchan] == 0} {
			return 0
		}
	}
	set output "[themereplace "$theme(PREFIX)$announce(IDLE)" "none"]"
	set output [basicreplace "$output" "IDLE"]
	sndone $chan $output

	set raw [exec $binary(WHO) "--raw"]
	set count 0
	set total 0.0
	foreach line [split $raw "\n"] {

		switch [lindex $line 0] {

			USER {
				switch [lindex $line 4] {

					ID {
						set user  [lindex $line 2]
						set group [lindex $line 3]

						set rawtime [lindex $line 5]
						set hours [lindex [split $rawtime ":"] 0]
						set minutes [lindex [split $rawtime ":"] 1]
						set seconds [lindex [split $rawtime ":"] 2]
						set idletime [expr ($hours*60+$minutes)*60+$seconds]

						set tagline [lindex $line 6]
						set since [lindex $line 7]

						if { $idletime > $minidletime } {
							set output [replacevar "[themereplace "$theme(PREFIX)$announce(USERIDLE)" "none"]" "%u_name" $user]
							set output [replacevar $output "%g_name" $group]
							set output [replacevar $output "%idletime" $idletime]
							set output [replacevar $output "%tagline" $tagline]
							set output [replacevar $output "%since" $since]
							set output [basicreplace "$output" "IDLE"]
							sndone $chan $output
							incr count
						}
					}
				}
			}
		}
	}
	set output [replacevar "[themereplace "$theme(PREFIX)$announce(TOTIDLE)" "none"]" "%count" $count]
	set output [basicreplace $output "IDLE"]
	sndone $chan $output
}
#################################################################################

#################################################################################
# UPDATED BANDWIDTH                                                             #
#################################################################################
proc ng_bandwidth {nick uhost hand chan args} {
	global binary announce speed theme disable mainchan speedmeasure speedthreshold

	if { $disable(TRIGINALLCHAN) == 1 } {
		if {[string match -nocase $chan $mainchan] == 0} {
			return 0
		}
	}
	set output "[themereplace "$theme(PREFIX)$announce(BW)" "none"]"
	set raw [exec $binary(WHO) --nbw]
	set upper [format "%.0f" [expr [lindex $raw 1] * 100 / $speed(INCOMING)]]
	set dnper [format "%.0f" [expr [lindex $raw 3] *100 / $speed(OUTGOING)]]
	set totalper [format "%.0f" [expr [lindex $raw 5] * 100 / ( $speed(INCOMING) + $speed(OUTGOING) )]]
	
	set up [speed_convert [lindex $raw 1] "none"]
	set dn [speed_convert [lindex $raw 3] "none"]
	set totalspeed [speed_convert [lindex $raw 5] "none"]

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

	set output [basicreplace "$output" "BW"]

	putserv "PRIVMSG $chan :$output"

}
################################################################################


#################################################################################
# POST STATS                                                                    #
#################################################################################
proc showstats {chan nick type time section} {
	global binary statsection location disable mainchan

	if { $disable(TRIGINALLCHAN) == 1 } {
		if {[string match -nocase $chan $mainchan] == 0} {
			return 0
		}
	}
	set sect 0
	if {[string length $section] != 0} {
		set error 1
		set sections ""
		foreach sectnumb [array names statsection] {
			if {![string compare [string tolower $statsection($sectnumb)] [string tolower $section]]} {
				set sect $sectnumb
				set error 0
				break
			} else { append sections "$statsection($sectnumb) " }
		}
		if {$error == 1} {
			puthelp "PRIVMSG $nick :Invalid section."
			puthelp "PRIVMSG $nick :Valid sections are: \002$sections\002"
			puthelp "PRIVMSG $nick : "
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

		set userfile $location(USERS)$username


		if {![string compare $result "MATCH"]} {
			set output "[themereplace "$theme(PREFIX)$announce(MSGINVITE)" "none"]"
			foreach channel $invite_channels { puthelp "INVITE $nick $channel" }
			foreach line [split [exec $binary(CAT) $userfile] "\n"] {
				if {![string compare [lindex $line 0] "GROUP"]} {
					set group [lrange $line 1 end]
					break
				}
			}
		} else {
			set output "[themereplace "$theme(PREFIX)$announce(BADMSGINVITE)" "none"]"
		}

		if {!$disable(MSGINVITE)} {
			set output [replacevar $output "%u_ircnick" $nick]
			set output [replacevar $output "%u_name" $username]
			set output [replacevar $output "%u_host" $host]
			set output [replacevar $output "%g_name" $group]
			set output [basicreplace $output "INVITE"]
			sndall "DEFAULT" $output
		}
    }
}
#################################################################################


#################################################################################
# SHOW FREE SPACE                                                               #
#################################################################################
proc show_free {nick uhost hand chan arg} {
	global binary announce device theme disable mainchan dev_max_length

	if { $disable(TRIGINALLCHAN) == 1 } {
		if {[string match -nocase $chan $mainchan] == 0} {
			return 0
		}
	}
	set devices(0) ""; set free 0.0; set used 0.0
	set total 0.0; set num 0; set perc 0.0
	array set "tmpdev" [array get "device"]

	set i 0
	foreach line [split [exec $binary(DF) "-Ph"] "\n"] {
		regsub -all {,} $line {.} line
		foreach dev [array names "tmpdev"] {
			if {[string match [lindex $line 0] [lindex $tmpdev($dev) 0]] == 1} {
				set dev_total [to_mb [lindex $line 1]]
				set dev_used [to_mb [lindex $line 2]]
				set dev_free [to_mb [lindex $line 3]]
				set dev_percent [format "%.1f" "[expr (double($dev_used)/double($dev_total)) * 100]"]
				set tmp [replacevar [themereplace $announce(FREE-DEV) "none"] "%total" [from_mb $dev_total]]
				set tmp [replacevar $tmp "%used" [from_mb $dev_used]]
				set tmp [replacevar $tmp "%free" [from_mb $dev_free]]
				set tmp [replacevar $tmp "%percentage" "$dev_percent"]
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
				array unset "tmpdev" $dev
			}
		}
	}
	if {[llength [array names "tmpdev"]]} {
		foreach {id dev} [array get tmpdev] { append tmpstr "$dev " }
		putlog "dZSbot warning: The following devices had no matching \"df -Ph\" entry: $tmpstr"
	}

	set totalgb [from_mb $total]
	set usedgb [from_mb $used]
	set freegb [from_mb $free]

	set o 0
	while {$o < [expr $i + 1]} {
		set output "[themereplace "$theme(PREFIX)$announce(FREE)" "none"]"
		set output [replacevar $output "%total" "${totalgb}"]
		set output [replacevar $output "%used" "${usedgb}"]
		set output [replacevar $output "%free" "${freegb}"]
		set output [replacevar $output "%percentage" [expr round($perc/$num)]]
		set output [replacevar $output "%devices" $devices($o)]
		set output [basicreplace $output "FREE"]
		sndone $chan $output
		incr o
	}
}
#################################################################################


#################################################################################
# LAUNCH A NUKE (GL2.0)                                                         #
#################################################################################
proc launchnuke2 {type path section sargs dargs} {
	global nuke hidenuke announce sitename theme mpath

	set nuke(TYPE) $type
	set nuke(PATH) $path
	set nuke(SECTION) $section
	set nuke(NUKER) [lindex $sargs 0]
	set nuke(MULT) [lindex $sargs 1]
	set nuke(REASON) [lindex $sargs 2]
	set nuke(NUKEE) {}

	foreach entry $dargs {
		if {[lindex $entry 0] != $hidenuke} {
			set mb [format "%.1f" [expr [lindex $entry 1] / 1024]]
			set nukee "[themereplace $announce(NUKEES) "none"]"
			set nukee [replacevar $nukee "%u_name" [lindex $entry 0]]
			set nukee [replacevar $nukee "%size" $mb]
			append nuke(NUKEE) "$nukee" "[themereplace $theme(SPLITTER) "none"]"
		}
	}

	set nuke(NUKEE) [string trim $nuke(NUKEE) "$theme(SPLITTER)"]
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

	set output [themereplace "$theme(PREFIX)$announce($nuke(TYPE))" "none"]
	set output [replacevar $output "%nuker" $nuke(NUKER)]
	set output [replacevar $output "%nukees" $nuke(NUKEE)]
	set output [replacevar $output "%type" $nuke(TYPE)]
	set output [replacevar $output "%multiplier" $nuke(MULT)]
	set output [replacevar $output "%reason" $nuke(REASON)]
	set output [replacevar $output "%section" $nuke(SECTION)]
	set output [replacevar $output "%relname" $relname]
	set output [replacevar $output "%reldir" [lindex $split [expr $ll -1]]]
	set output [replacevar $output "%path" [lindex $split [expr $ll -2]]]
	set output [basicreplace $output $nuke(TYPE)]
	sndall $nuke(SECTION) $output
}
#################################################################################


#################################################################################
# UPDATE NUKE BUFFER (GL1.0)                                                    #
#################################################################################
proc fuelnuke {type path section args} {global nuke
	global hidenuke

	set args [lindex $args 0]
	if {$type == $nuke(LASTTYPE) && $path == $nuke(LASTDIR) && $nuke(SHOWN) == 0} {
		if {[lindex $args 2] != $hidenuke} {
			append nuke(NUKEE) "\002[lindex $args 2]\002 (\002[lindex [lindex $args 3] 1]\002MB), "
		}
	} else {
		launchnuke
		if {[lindex $args 2] != $hidenuke} {
			set nuke(TYPE) $type
			set nuke(PATH) $path
			set nuke(SECTION) $section
			set nuke(NUKER) [lindex $args 1]
			set nuke(NUKEE) "\002[lindex $args 2]\002 (\002[lindex [lindex $args 3] 1]\002MB) "
			set nuke(MULT) [lindex [lindex $args 3] 0]
			set nuke(REASON) [lindex $args 4]
			set nuke(SHOWN) 0
		}
	}
	set nuke(LASTTYPE) $type
	set nuke(LASTDIR) $path
}
#################################################################################


#################################################################################
# FLUSH NUKE BUFFER  (GL1.0)                                                    #
#################################################################################
proc launchnuke {} {
	global nuke sitename announce theme mpath
	if {$nuke(SHOWN) == 1} {return 0}
	set nuke(NUKEE) [string trim $nuke(NUKEE) "$theme(SPLITTER)"]

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

	set output [themereplace "$theme(PREFIX)$announce($nuke(TYPE))" "none"]
	set output [replacevar $output "%nuker" $nuke(NUKER)]
	set output [replacevar $output "%nukees" $nuke(NUKEE)]
	set output [replacevar $output "%type" $nuke(TYPE)]
	set output [replacevar $output "%multiplier" $nuke(MULT)]
	set output [replacevar $output "%reason" $nuke(REASON)]
	set output [replacevar $output "%section" $nuke(SECTION)]
	set output [replacevar $output "%relname" $relname]
	set output [replacevar $output "%reldir" [lindex $split [expr $ll -1]]]
	set output [replacevar $output "%path" [lindex $split [expr $ll -2]]]
	set output [basicreplace $output $nuke(TYPE)]
	sndall $nuke(SECTION) $output

	set nuke(SHOWN) 1
}
#################################################################################


#################################################################################
# CHECK IF RELEASE SHOULD NOT BE ANNOUNCED                                      #
#################################################################################
proc denycheck {release} {
	global denypost
	foreach deny $denypost {
		if {[string match $deny $release] == 1} { return 1 }
	}
	return 0
}
#################################################################################


#################################################################################
# SHOW INCOMPLETE LIST                                                          #
#################################################################################
proc show_incompletes { nick uhost hand chan arg } {
	global sitename binary disable mainchan

	if { $disable(TRIGINALLCHAN) == 1 } {
		if {[string match -nocase $chan $mainchan] == 0} {
			return 0
		}
	}
	foreach line [split [exec $binary(INCOMPLETE)] "\n"] {
		if {![info exists newline($line)]} {
			set newline($line) 0
		} else { set newline($line) [expr $newline($line) + 1] }
		puthelp "PRIVMSG $nick :$line\003$newline($line)"
	}
}
#################################################################################


#################################################################################
# SHOW WELCOME MSG                                                              #
#################################################################################
proc welcome_msg { nick uhost hand chan } {
	global announce disable chanlist sitename cmdpre

	if {$disable(WELCOME) == 0} {
		foreach c_chan $chanlist(WELCOME) {
			if {[string match -nocase $c_chan $chan] == 1} {
				set output [themereplace "$announce(WELCOME)" "none"]
				set output [replacevar $output "%bold" "\002"]
				set output [replacevar $output "%ircnick" $nick]
				set output [replacevar $output "%sitename" $sitename]
				set output [replacevar $output "%cmdpre" $cmdpre]
				puthelp "NOTICE $nick : $output"
			}
		}
	}
}
#################################################################################


#################################################################################
# CHECK BOUNCER STATUSES                                                        #
#################################################################################
proc ng_bnc_check {nick uhost hand chan arg} {
	global bnc binary disable mainchan

	if { $disable(TRIGINALLCHAN) == 1 } {
		if {[string match -nocase $chan $mainchan] == 0} {
			return 0
		}
	}
	putquick "NOTICE $nick :Checking bouncer(s) status..."
	set count 0
	foreach i $bnc(LIST) {
		incr count
		set i [split $i ":"]
		set loc [lindex $i 0]
		set ip [lindex $i 1]
		set port [lindex $i 2]
	
		if { $bnc(PING) == "TRUE" } {
		    if {[catch { set data [exec $binary(PING) -c1 $ip]} error]} { 
				putquick "NOTICE $nick :$count. .$loc - $ip:$port - DOWN (Can't ping host)"
				continue
		    }
		    set ping ", ping: [format %.1f [lindex [split [lindex [lindex [split $data \"\n\"] 1] 6] \"=\"] 1]]ms"
		} else { set ping "" }

		set dur [clock clicks -milliseconds]
		set exitlevel [catch {exec $binary(NCFTPLS) -P $port -u $bnc(USER) -p $bnc(PASS) -t $bnc(TIMEOUT) -r 0 ftp://$ip 2>@ stdout} raw]
		set dur [expr [clock clicks -milliseconds] - $dur]

		if { $exitlevel == 0 } {
			putquick "NOTICE $nick :$count. .$loc - $ip:$port - UP (login: [format %.0f $dur]ms$ping)"
		} else {
			switch -glob $raw {
				"*username was not accepted for login.*" {set error "Bad Username"}
				"*username and/or password was not accepted for login.*" {set error "Couldn't login"}
				"*Connection refused.*" {set error "Connection Refused"}
				"*try again later: Connection timed out.*" {set error "Connection Timed Out"}
				"*timed out while waiting for server response.*" {set error "No response"}
				"*Remote host has closed the connection.*" {set error "Connection Lost"}
				"*unknown host.*" {set error "Unknown Host?"}
				default { set error "Unhandled Error Type?" ; putlog "DEBUG: dZSbot.tcl bnc check unhandled error type \"$raw\", please report to project-zs-ng developers." }
			}
			putquick "NOTICE $nick :$count. .$loc - $ip:$port - DOWN ($error)"
		}
		set error ""
		set raw ""
	}
}
#################################################################################


#################################################################################
# CHOOSE STATS PARAMETERS                                                       #
#################################################################################
proc stats_user_dayup {nick uhost hand chan args} { showstats "$chan" "$nick" "-u" "-t" "[lindex $args 0]" }
proc stats_user_daydn {nick uhost hand chan args} { showstats "$chan" "$nick" "-d" "-t" "[lindex $args 0]" }
#################################################################################
proc stats_user_wkup {nick uhost hand chan args} { showstats "$chan" "$nick" "-u" "-w" "[lindex $args 0]" }
proc stats_user_wkdn {nick uhost hand chan args} { showstats "$chan" "$nick" "-d" "-w" "[lindex $args 0]" }
#################################################################################
proc stats_user_monthup {nick uhost hand chan args} { showstats "$chan" "$nick" "-u" "-m" "[lindex $args 0]" }
proc stats_user_monthdn {nick uhost hand chan args} { showstats "$chan" "$nick" "-d" "-m" "[lindex $args 0]" }
#################################################################################
proc stats_user_allup {nick uhost hand chan args} { showstats "$chan" "$nick" "-u" "-a" "[lindex $args 0]" }
proc stats_user_alldn {nick uhost hand chan args} { showstats "$chan" "$nick" "-d" "-a" "[lindex $args 0]" }
#################################################################################
proc stats_group_gpwk {nick uhost hand chan args} { showstats "$chan" "$nick" "-u" "-W" "[lindex $args 0]" }
proc stats_group_gpwd {nick uhost hand chan args} { showstats "$chan" "$nick" "-d" "-W" "[lindex $args 0]" }
#################################################################################
proc stats_group_gpal {nick uhost hand chan args} { showstats "$chan" "$nick" "-u" "-A" "[lindex $args 0]" }
proc stats_group_gpad {nick uhost hand chan args} { showstats "$chan" "$nick" "-d" "-A" "[lindex $args 0]" }
#################################################################################


#################################################################################
# Help Section                                                                  #
#################################################################################
proc help {nick uhost hand chan arg} {
	global sections cmdpre dver tclroot disable mainchan

	if { $disable(TRIGINALLCHAN) == 1 } {
		if {[string match -nocase $chan $mainchan] == 0} {
			return 0
		}
	}
	set file "$tclroot/dZSbot.help"
	if {![file readable $file]} {
		puthelp "PRIVMSG $nick : File dZSbot.help is missing, please check install"
		puthelp "PRIVMSG $nick : (file should reside in same dir as dZSbot.tcl)"
		return 0
	}

	puthelp "PRIVMSG $nick : -project-zipscript-ng-help-"
	puthelp "PRIVMSG $nick : ---------v$dver------------"
	puthelp "PRIVMSG $nick : "

	set helpfile [open $file r]
	set helpdb [read $helpfile]
	close $helpfile
	foreach line [split $helpdb "\n"] {
		regsub -all "%cmdpre" "$line" "$cmdpre" line
		puthelp "PRIVMSG $nick :$line"
	}
	puthelp "PRIVMSG $nick : Valid sections are: $sections"
}
#################################################################################


#################################################################################
# LOAD A THEME FILE                                                             #
#################################################################################
proc loadtheme {file} {
	global theme announce theme_fakes
	unset announce
	set announce(THEMEFILE) $file

	if {[string index $file 0] != "/"} { set file "[file dirname [info script]]/$file" }
	putlog "Theme loaded: $file"
	if {![file readable $file]} {
		putlog "dZSbot: themefile is not readable or does not exist. ($file)"
		return 0
	}

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

	foreach name [array names theme] { set theme($name) [themereplace_startup $theme($name)] }
	foreach name [array names theme_fakes] { set theme_fakes($name) [themereplace_startup $theme_fakes($name)] }
	foreach name [array names announcetmp] { set announce($name) [themereplace_startup $announcetmp($name)] }

	set ret 1
	set required "PREFIX COLOR1 COLOR2 COLOR3"
	foreach req [split $required " "] {
		if {[lsearch -exact [array names theme] $req] == -1} {
			putlog "dZSbot: missing required themefile setting (in $file): '$req', failing."
			set ret 0
		}
	}
	return $ret
}
#################################################################################


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

        regsub -all {\003(\d)(?!\d)} $rstring {\\0030\1} rstring
        return [subst -nocommands $rstring]
}

#################################################################################
# REPLACES THEMERELATED STUFF IN A GIVEN STRING, DYNAMIC REPLACE FOR RUNTIME    #
#################################################################################
proc themereplace {rstring section} {
	global theme

	# We replace %cX{string}, %b{string} and %u{string} with their coloured, bolded and underlined equivilants ;)
	# bold and underline replacement should not be needed here...
	while {[regexp {(%c(\d)\{([^\{\}]+)\}|%b\{([^\{\}]+)\}|%u\{([^\{\}]+)\}|%([lrm])(\d\d?)\{([^\{\}]+)\})} $targetString matchString dud padOp padLength padString]} {
		while {[regexp {%([lrm])(\d\d?)\{([^\{\}]+)\}} $targetString matchString padOp padLength padString]} {
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
		}

		set targetString [string map [list $matchString $paddedString] $targetString]
	
		set tmpstr [format "COLOR_%s_1" $section]
		if {[lsearch -exact [array names theme] $tmpstr] != -1} {
			regsub -all {%c(\d)\{([^\{\}]+)\}} $targetString {\\003$theme([format "COLOR_%s_" $section]\1)\2\\003} targetString
		} else {
			regsub -all {%c(\d)\{([^\{\}]+)\}} $targetString {\\003$theme(COLOR\1)\2\\003} targetString
		}
		regsub -all {%b\{([^\{\}]+)\}} $targetString {\\002\1\\002} targetString
		regsub -all {%u\{([^\{\}]+)\}} $targetString {\\037\1\\037} targetString
	}

	regsub -all {\003(\d)(?!\d)} $targetString {\\0030\1} targetString
	return [subst -nocommands $targetString]
}
#################################################################################


#################################################################################
# OUTPUTS STUFF TO IRC / DCC CHAT IF DEBUG MODE ON :)                           #
#################################################################################
proc dprint {arg1 {arg2 0}} {
	global disable announce chanlist debuglevel
	
	if {$arg2 != 0} {
		set level $arg1
		set string $arg2
	} else {
		set level "DEFAULT_LEVEL"
		set string $arg1
	}

	set level_str [$level "1"]
	set level     [$level]
	
	if {[llength [array names "disable" "DEBUG"]] == 0} { 
		putlog "dZSbot error: disable(DEBUG) not set, defaulting to 0."
		set disable(DEBUG) 0
	}
	if {[llength [array names "disable" "DEBUG_DCC"]] == 0} {
		putlog "dZSbot error: disable(DEBUG_DCC) not set, defaulting to 0."
		set disable(DEBUG_DCC) 0
	}
	
	if {$disable(DEBUG) && $disable(DEBUG_DCC)} { return; }
	if {$debuglevel < $level} { return; }

	if {!$disable(DEBUG_DCC)} {
		if {[llength [array names "announce" "DEBUG_DCC"]] > 0} {
			set outp [basicreplace [themereplace $announce(DEBUG_DCC) "none"] $level_str]
			putlog "dZSbot debug: $outp"
		} else {
			putlog "dZSbot debug: \[$section\] $txt"
		}
	}

	if {!$disable(DEBUG)} {
		set chans $chanlist(DEFAULT)
		set outp [themereplace $announce(DEFAULT) "none"]
		if {[llength [array names "chanlist" "DEBUG"]] > 0} { set chans $chanlist(DEBUG) }
		if {[llength [array names "announce" "DEBUG"]] > 0} { set outp [themereplace $announce(DEBUG) "none"] }
		set outp [basicreplace $outp $level_str]
		foreach chan $chans { putquick "PRIVMSG $chan :$outp" }
	}
}
#################################################################################

if {[info exists enable_irc_invite]} {
	if {[string compare -nocase $enable_irc_invite "YES"] == 0} {
		bind msg	-|-	!invite			invite
	}
} else { dprint DEBUG_WARN "Could not find variable 'enable_irc_invite', pretending it's set to \"NO\"." }

if {[info exists dZStimer]} {
	if {[catch {killutimer $dZStimer} err]} {
		putlog "dZSbot error: WARNING!"
		putlog "dZSbot error: killutimer failed ($err)"
		putlog "dZSbot error: You should .restart the bot to be safe."
	}
}
set dZStimer [utimer 1 "readlog"]

if {![loadtheme $announce(THEMEFILE)]} {
	if {[loadtheme "default.zst.dist"]} {
		putlog "dZSbot error: Couldn't load theme '$announce(THEMEFILE)', loaded 'default.zst.dist' instead!"
	} else {
		putlog "dZSbot error: Couldn't load theme '$announce(THEMEFILE)' and not 'default.zst.dist' either. Cannot continue!"
		set dzerror 1
	}
}

if {![array exists chanlist] || [llength [array names "chanlist" "DEFAULT"]] == 0} {
	putlog "dZSbot error: no entry in chanlist set, or chanlist(DEFAULT) not set."
	set dzerror 1
}
if {![array exists announce] || [llength [array names "announce" "DEFAULT"]] == 0} {
	putlog "dZSbot error: no entry in announce set, or announce(DEFAULT) not set."
	putlog "dZSbot error: setting announce(DEFAULT) to '\[DEFAULT\] %msg'."
	set announce(DEFAULT) "\[DEFAULT\] %msg"
}
if {![array exists variables] || [llength [array names "variables" "DEFAULT"]] == 0} {
	putlog "dZSbot error: no entry in variables set, or variables(DEFAULT) not set."
	putlog "dZSbot error: setting variables(DEFAULT) to '%pf %msg'."
	set variables(DEFAULT) "%pf %msg"
}
if {![array exists disable] || [llength [array names "disable" "DEFAULT"]] == 0} {
	putlog "dZSbot error: no entry in disable set, or disable(DEFAULT) not set."
	putlog "dZSbot error: setting disable(DEFAULT) to '0."
	set disable(DEFAULT) "0"
}

# Hook up variables and announce definitions for the message replacing code
# New message should have identical variables definition as old message
# New announce set to old announce if not found in theme file (and output a
# warning message).
foreach rep [array names "msgreplace"] {
	set rep [split $msgreplace($rep) ":"]
	set variables([lindex $rep 2]) $variables([lindex $rep 0])
	set disable([lindex $rep 2]) 0
	if {![info exists announce([lindex $rep 2])]} {
		set announce([lindex $rep 2]) $announce([lindex $rep 0])
		putlog "dZSbot warning: Custom message [lindex $rep 2] defined, but no announce definition found. Using same announce as [lindex $rep 0] for now. Please edit the theme file!"
	}
}

if { $dzerror == "0" } {
	putlog "dZSbot loaded ok!"
} else {
	putlog "dZSbot had errors. Please check log and fix."
	if { $die_on_error == "YES" } {
		die
	}
}
