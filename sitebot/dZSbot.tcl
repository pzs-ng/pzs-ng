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
	die
}
if {[catch {source [file dirname [info script]]/dZSbvars.tcl} tmperror]} {
	putlog "dZSbot: dZSbvars.tcl not found or has errors. Cannot continue."
	die
}

foreach bin [array names binary] {
	if {![file executable $binary($bin)]} {
		putlog "dZSbot: Wrong path/missing bin for $bin - Please fix."
		set dzerror "1"
	}
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

set lastoct [file size $location(GLLOG)]
set loglastoct [file size $location(LOGINLOG)]
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
}

if {$bindnopre != "YES"} {
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
#################################################################################
proc readlog {} {
	global location lastoct disable defaultsection variables msgtypes chanlist dZStimer use_glftpd2 invite_channels loglastoct pid

	set dZStimer [utimer 1 "readlog"]

	set glftpdlogsize [file size $location(GLLOG)]
	set loginlogsize [file size $location(LOGINLOG)]

	if {$glftpdlogsize == $lastoct && $loginlogsize == $loglastoct} { return 0 }
	if {$glftpdlogsize  < $lastoct} { set lastoct 0 }
	
	set lines ""
	
	if {$glftpdlogsize != $lastoct} {
		if {![catch {set of [open $location(GLLOG) r]} ]} {
			seek $of $lastoct
			while {![eof $of]} {
				set line [gets $of]
				if {$line == ""} { continue; }
				lappend lines $line
			}
			close $of
		} else {
			putlog "dZSbot error: Could not open GLLOG. ($location(GLLOG))"
			return 0
		}
	}

	
	if {$loginlogsize != $loglastoct} {
		if {![catch {set of [open $location(LOGINLOG) r]} ]} {
			seek $of $loglastoct
			while {![eof $of]} {
				set line [gets $of]
				if {$line == ""} { continue; }
				lappend lines $line
			}
			close $of
		} else {
			putlog "dZSbot error: Could not open LOGINLOG. ($location(LOGINLOG))"
			return 0
		}
	}

	set lastoct [file size $location(GLLOG)]
	set loglastoct [file size $location(LOGINLOG)]

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
		
		if {![string compare $msgtype "INVITE"]} {
			set nick [lindex $line 6]
			foreach channel $invite_channels { puthelp "INVITE $nick $channel" }
		}

		set section [getsection $path $msgtype]
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
# REPLACE WHAT WITH WITHWHAT                                                    #
#################################################################################
proc replacevar {rstring what withwhat} {
	global zeroconvert

	set output $rstring
	set replacement $withwhat

	if {[string length $replacement] == 0 && [info exists zeroconvert($what)]} {
		set replacement $zeroconvert($what)
	}
	set cutpos 0

	while {[string first $what $output] != -1} {
		set cutstart [expr [string first $what $output] - 1]
		set cutstop  [expr $cutstart + [string length $what] + 1]
		set output [string range $output 0 $cutstart]$replacement[string range $output $cutstop end]
	}
	return $output
}
#################################################################################


#################################################################################
# CONVERT ANYTHING>MB TO MEGABYTES                                              #
#################################################################################
proc to_mb {str} {
	set type [string index $str end]
	set size [string range $str 0 end-1]
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
# CONVERT BASIC COOKIES TO DATA                                                 #
#################################################################################
proc basicreplace {rstring section} {
	global sitename

	set output [themereplace $rstring]
	set output [replacevar $output "%sitename" $sitename]
	set output [replacevar $output "%bold" "\002"]
	set output [replacevar $output "%uline" "\037"]
	set output [replacevar $output "%section" $section]
	return "$output"
}
#################################################################################


#################################################################################
# CONVERT COOKIES TO DATA                                                       #
#################################################################################
proc parse {msgtype msgline section} { global variables announce random mpath use_glftpd2 theme theme_fakes defaultsection pid
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
			set f_user $type
			set type "FAILLOGIN"
		}
	}
	if {![info exists announce($type)]} {
		if {$pid == 0} {
			putlog "dZSbot error: \"announce($type)\" not set in theme, type becomes \"DEFAULT\""
			set type "DEFAULT"
		} else {
			set f_user $type
			set type "FAILLOGIN"
		}
	}

	set vars $variables($type)

	if {![string compare [lindex $announce($type) 0] "random"] && [string is alnum -strict [lindex $announce($type) 1]] == 1} {
		set output $random($msgtype\-[rand [lindex $announce($type) 1]])
	} else {
		set output $announce($type)
	}

	set output "$theme(PREFIX)$output"
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
		set cnt 1
	}

	foreach vari $vars {
		set output [replacevar $output $vari [lindex $msgline $cnt]]
		set cnt [expr $cnt + 1]
	}

	return $output
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
# POST WHO INFO                                                                 #
#################################################################################
proc who {nick uhost hand chan args} {
	global binary disable mainchan

	if { $disable(TRIGINALLCHAN) == 1 } {
		if {$chan!=$mainchan} {
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
		if {$chan!=$mainchan} {
			return 0
		}
	}

	set line ""
	if { $disable(ALTWHO) == 0 } {
		set output "$theme(PREFIX)$announce(SPEEDERROR)"
		foreach line [split [exec $binary(WHO) --raw [lindex $args 0]] "\n"] {
			set action [lindex $line 4]
			if {$action == "DN"} {
				set output "$theme(PREFIX)$announce(SPEEDDN)"
			} elseif {$action == "UP"} {
				set output "$theme(PREFIX)$announce(SPEEDUP)"
			} elseif {$action == "ID"} {
				set output "$theme(PREFIX)$announce(SPEEDID)"
			}
			set output [replacevar $output "%u_name" [lindex $line 2]]
			set output [replacevar $output "%g_name" [lindex $line 3]]
			set output [replacevar $output "%upspeed" [lindex $line 5]]
			set output [replacevar $output "%dnspeed" [lindex $line 5]]
			set output [replacevar $output "%idletime" [lindex $line 5]]
			set output [replacevar $output "%tagline" [lindex $line 6]]
			set output [replacevar $output "%timeonline" [lindex $line 7]]
			set output [replacevar $output "%f_name" [lindex $line 8]]
			set output [replacevar $output "%dnpercent" [lindex $line 9]]
			set output [replacevar $output "%uppercent" [lindex $line 9]]
			set output [basicreplace $output "SPEED"]
			putserv "PRIVMSG $chan :$output"
		}
	} else {
		set base_output "$theme(PREFIX)$announce(DEFAULT)"
		foreach line [split [exec $binary(WHO) [lindex $args 0]] "\n"] {
			set output [replacevar $base_output "%msg" $line]
			set output [basicreplace $output "SPEED"]
			putserv "PRIVMSG $chan :$output"
		}
	}

        if {$line == ""} {
              set output [basicreplace $output "SPEED"]
                putserv "PRIVMSG $chan :$output"
        }

}
#################################################################################


#################################################################################
# POST BANDWIDTH                                                                #
#################################################################################
proc bandwidth {nick uhost hand chan args} {
	global binary announce theme disable mainchan

	if { $disable(TRIGINALLCHAN) == 1 } {
		if {$chan!=$mainchan} {
			return 0
		}
	}
	set output "$theme(PREFIX)$announce(BW)"
	set data [exec $binary(BW)]
	set output [replacevar $output "%uploads" [lindex $data 0]]
	set output [replacevar $output "%downloads" [lindex $data 2]]
	set output [replacevar $output "%transfers" [lindex $data 4]]
	set output [replacevar $output "%upspeed" [lindex $data 1]]
	set output [replacevar $output "%dnspeed" [lindex $data 3]]
	set output [replacevar $output "%totalspeed" [lindex $data 5]]
	set output [basicreplace "$output" "BW"]

	putserv "PRIVMSG $chan :$output "
}
#################################################################################


#################################################################################
# uploaders BANDWIDTH                                                           #
#################################################################################
proc ng_bwup { nick uhost hand chan args} {
	global binary announce speed theme disable mainchan

	if { $disable(TRIGINALLCHAN) == 1 } {
		if {$chan!=$mainchan} {
			return 0
		}
	}
	set output "$theme(PREFIX)$announce(BWUP)"
	set raw [exec $binary(BW)]
	set upper [format "%.1f" [expr 100 * ([lindex $raw 1] / $speed(INCOMING))]]
	set dnper [format "%.1f" [expr 100 * ([lindex $raw 3] / $speed(OUTGOING))]]
	set totalper [format "%.0f" [expr 100 * ([lindex $raw 5] / ( $speed(INCOMING) + $speed(OUTGOING)))]]

	set output [replacevar $output "%uploads" [lindex $raw 0]]
	set output [replacevar $output "%upspeed" [lindex $raw 1]]
	set output [replacevar $output "%downloads" [lindex $raw 2]]
	set output [replacevar $output "%dnspeed" [lindex $raw 3]]
	set output [replacevar $output "%transfers" [lindex $raw 4]]
	set output [replacevar $output "%totalspeed" [lindex $raw 5]]
	set output [replacevar $output "%idlers" [lindex $raw 6]]
	set output [replacevar $output "%active" [lindex $raw 7]]
	set output [replacevar $output "%totallogins" [lindex $raw 8]]

	set output [replacevar $output "%uppercent" $upper]
	set output [replacevar $output "%dnpercent" $dnper]
#	set output [replacevar $output "%totalpercent" $totalper]

	set output [basicreplace "$output" "BW"]

	putserv "PRIVMSG $chan :$output"
}
################################################################################

#################################################################################
# ng_uploaders - Origional by Celerex - Mod/Merge by themolester                #
#################################################################################
proc ng_uploaders {nick uhost hand chan args} {
	global binary announce speed theme disable mainchan

	if { $disable(TRIGINALLCHAN) == 1 } {
		if {$chan!=$mainchan} {
			return 0
		}
	}
	set output "$theme(PREFIX)$announce(UPLOAD)"
	set output [basicreplace "$output" "UPLOAD"]
	putserv "PRIVMSG $chan :$output "

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
						set output [replacevar "$theme(PREFIX)$announce(USER)" "%u_name" $user]
						set output [replacevar $output "%g_name" $group]
						set output [replacevar $output "%fper" $progress]
						set output [replacevar $output "%uspeed" $uspeed]
						set output [replacevar $output "%per" $per]
						set output [replacevar $output "%tagline" $tagline]
						set output [replacevar $output "%since" $since]
						set output [replacevar $output "%filename" $filename]
						set output [basicreplace "$output" "UPLOAD"]
						putserv "PRIVMSG $chan :$output"
						incr count
						set total [expr $total+$uspeed]
					}
				}
			}
		}
	}
	set per [format "%.1f" [expr double($total) * 100 / double($speed(INCOMING)) ]]

	set output [replacevar "$theme(PREFIX)$announce(TOTUPDN)" "%type" "Uploaders:"]
	set output [replacevar $output "%count" $count]
	set output [replacevar $output "%total" $total]
	set output [replacevar $output "%per" $per]
	set output [basicreplace $output "UPLOAD"]

	putserv "PRIVMSG $chan :$output "
}
#################################################################################

#################################################################################
# downloaders BANDWIDTH                                                         #
#################################################################################
proc ng_bwdn { nick uhost hand chan args} {
	global binary announce speed theme disable mainchan

	if { $disable(TRIGINALLCHAN) == 1 } {
		if {$chan!=$mainchan} {
			return 0
		}
	}
	set output "$theme(PREFIX)$announce(BWDN)"
	set raw [exec $binary(BW)]
	set upper [format "%.0f" [expr [lindex $raw 1] / $speed(INCOMING)]]
	set dnper [format "%.0f" [expr [lindex $raw 3] / $speed(OUTGOING)]]
	set totalper [format "%.0f" [expr [lindex $raw 5] / ( $speed(INCOMING) + $speed(OUTGOING))]]

	set output [replacevar $output "%uploads" [lindex $raw 0]]
	set output [replacevar $output "%upspeed" [lindex $raw 1]]
	set output [replacevar $output "%downloads" [lindex $raw 2]]
	set output [replacevar $output "%dnspeed" [lindex $raw 3]]
	set output [replacevar $output "%transfers" [lindex $raw 4]]
	set output [replacevar $output "%totalspeed" [lindex $raw 5]]
	set output [replacevar $output "%idlers" [lindex $raw 6]]
	set output [replacevar $output "%active" [lindex $raw 7]]
	set output [replacevar $output "%totallogins" [lindex $raw 8]]

	set output [replacevar $output "%uppercent" $upper]
	set output [replacevar $output "%dnpercent" $dnper]
	set output [replacevar $output "%totalpercent" $totalper]

	set output [basicreplace "$output" "BW"]

	putserv "PRIVMSG $chan :$output"
}
################################################################################

#################################################################################
# ng_leechers - Origional by Celerex - Mod/Merge by themolester                 #
#################################################################################
proc ng_leechers {nick uhost hand chan args} {
	global binary announce speed theme disable mainchan

	if { $disable(TRIGINALLCHAN) == 1 } {
		if {$chan!=$mainchan} {
			return 0
		}
	}
	set output "$theme(PREFIX)$announce(LEECH)"
	set output [basicreplace "$output" "LEECH"]
	putserv "PRIVMSG $chan :$output "

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
						set output [replacevar "$theme(PREFIX)$announce(USER)" "%u_name" $user]
						set output [replacevar $output "%g_name" $group]
						set output [replacevar $output "%fper"	$fper]
						set output [replacevar $output "%uspeed" $uspeed]
						set output [replacevar $output "%per" $per]
						set output [replacevar $output "%tagline" $tagline]
						set output [replacevar $output "%since" $since]
						set output [replacevar $output "%filename" $filename]
						set output [basicreplace "$output" "LEECH"]
						putserv "PRIVMSG $chan :$output"
						incr count
						set total [expr $total+$uspeed]
					}
				}
			}
		}
	}
	set per [format "%.1f" [expr double($total) * 100 / double($speed(OUTGOING)) ]]

	set output [replacevar "$theme(PREFIX)$announce(TOTUPDN)" "%type" "Leechers:"]
	set output [replacevar $output "%count" $count]
	set output [replacevar $output "%total" $total]
	set output [replacevar $output "%per" $per]
	set output [basicreplace "$output" "LEECH"]

	putserv "PRIVMSG $chan :$output "
}
#################################################################################

#################################################################################
# ng_idlers - Origional by Celerex - Mod/Merge by themolester                   #
#################################################################################
proc ng_idlers { nick uhost hand chan args} {
	global binary announce speed minidletime theme disable mainchan

	if { $disable(TRIGINALLCHAN) == 1 } {
		if {$chan!=$mainchan} {
			return 0
		}
	}
	set output "$theme(PREFIX)$announce(IDLE)"
	set output [basicreplace "$output" "IDLE"]
	putserv "PRIVMSG $chan :$output "

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
							set output [replacevar "$theme(PREFIX)$announce(USERIDLE)" "%u_name" $user]
							set output [replacevar $output "%g_name" $group]
							set output [replacevar $output "%idletime" $idletime]
							set output [replacevar $output "%tagline" $tagline]
							set output [replacevar $output "%since" $since]
							set output [basicreplace "$output" "IDLE"]
							putserv "PRIVMSG $chan :$output"
							incr count
						}
					}
				}
			}
		}
	}
	set output [replacevar "$theme(PREFIX)$announce(TOTIDLE)" "%count" $count]
	set output [basicreplace $output "IDLE"]
	putserv "PRIVMSG $chan :$output "
}
#################################################################################

#################################################################################
# UPDATED BANDWIDTH                                                             #
#################################################################################
proc ng_bandwidth {nick uhost hand chan args} {
	global binary announce speed theme disable mainchan

	if { $disable(TRIGINALLCHAN) == 1 } {
		if {$chan!=$mainchan} {
			return 0
		}
	}
	set output "$theme(PREFIX)$announce(BW)"
	set raw [exec $binary(BW)]
	set upper [format "%.0f" [expr [lindex $raw 1] * 100 / $speed(INCOMING)]]
	set dnper [format "%.0f" [expr [lindex $raw 3] *100 / $speed(OUTGOING)]]
	set totalper [format "%.0f" [expr [lindex $raw 5] * 100 / ( $speed(INCOMING) + $speed(OUTGOING) )]]

	set output [replacevar $output "%uploads" [lindex $raw 0]]
	set output [replacevar $output "%upspeed" [lindex $raw 1]]
	set output [replacevar $output "%downloads" [lindex $raw 2]]
	set output [replacevar $output "%dnspeed" [lindex $raw 3]]
	set output [replacevar $output "%transfers" [lindex $raw 4]]
	set output [replacevar $output "%totalspeed" [lindex $raw 5]]
	set output [replacevar $output "%idlers" [lindex $raw 6]]
	set output [replacevar $output "%active" [lindex $raw 7]]
	set output [replacevar $output "%totallogins" [lindex $raw 8]]

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
proc showstats {nick type time section} {
	global binary statsection location disable mainchan

	if { $disable(TRIGINALLCHAN) == 1 } {
		if {$chan!=$mainchan} {
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
			set output "$theme(PREFIX)$announce(MSGINVITE)"
			foreach channel $invite_channels { puthelp "INVITE $nick $channel" }
			foreach line [split [exec $binary(CAT) $userfile] "\n"] {
				if {![string compare [lindex $line 0] "GROUP"]} {
					set group [lrange $line 1 end]
					break
				}
			}
		} else {
			set output "$theme(PREFIX)$announce(BADMSGINVITE)"
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
	global binary announce device theme disable mainchan

	if { $disable(TRIGINALLCHAN) == 1 } {
		if {$chan!=$mainchan} {
			return 0
		}
	}
	set output "$theme(PREFIX)$announce(FREE)"
	set devices ""; set free 0; set used 0
	set total 0; set num 0; set perc 0
	array set "tmpdev" [array get "device"]

	foreach line [split [exec $binary(DF) "-Ph"] "\n"] {
		regsub -all {,} $line {.} line
		foreach dev [array names "tmpdev"] {
			if {[string match [lindex $line 0] [lindex $tmpdev($dev) 0]] == 1} {
				set tmp [replacevar $announce(FREE-DEV) "%total" "[lindex $line 1]B"]
				set tmp [replacevar $tmp "%used" "[lindex $line 2]B"]
				set tmp [replacevar $tmp "%free" "[lindex $line 3]B"]
				set tmp [replacevar $tmp "%percentage" "[string trim [lindex $line 4] " %"]"]
				set tmp [replacevar $tmp "%section" [lrange $device($dev) 1 end]]
				append devices $tmp

				incr total [to_mb [lindex $line 1]]; incr used [to_mb [lindex $line 2]]
				incr free [to_mb [lindex $line 3]]; incr num
				incr perc [string trim [lindex $line 4] " %"]
				array unset "tmpdev" $dev
			}
		}
	}
	if {[llength [array names "tmpdev"]]} {
		putlog "dZSbot error: following devices had no matching \"df -Phx\" entry: [join [array names tmpdev]]"
		return
	}
	set totalgb [format "%.1f" [expr $total / 1000]]
        set usedgb [format "%.1f" [expr $used / 1000]]
        set freegb [format "%.1f" [expr $free / 1000]]
	set output [replacevar $output "%total" "${totalgb}GB"]
	set output [replacevar $output "%used" "${usedgb}GB"]
	set output [replacevar $output "%free" "${freegb}GB"]
	set output [replacevar $output "%percentage" [expr round($perc/$num)]]
	set output [replacevar $output "%devices" $devices]
	set output [basicreplace $output "FREE"]
	putserv "PRIVMSG $chan :$output"
}
#################################################################################


#################################################################################
# LAUNCH A NUKE (GL2.0)                                                         #
#################################################################################
proc launchnuke2 {type path section sargs dargs} {
	global nuke hidenuke announce sitename theme

	set nuke(TYPE) $type
	set nuke(PATH) $path
	set nuke(SECTION) $section
	set nuke(NUKER) [lindex $sargs 0]
	set nuke(MULT) [lindex $sargs 1]
	set nuke(REASON) [lindex $sargs 2]
	set nuke(NUKEE) {}

	foreach entry $dargs {
		set mb [format "%.1f" [expr [lindex $entry 1] / 1024]]
		set nukee "$announce(NUKEES)"
		set nukee [replacevar $nukee "%u_name" [lindex $entry 0]]
		set nukee [replacevar $nukee "%size" $mb]
		append nuke(NUKEE) "$nukee"
	}

	set nuke(NUKEE) [string trim $nuke(NUKEE) ", "]
	set split [split $nuke(PATH) "/"]
	set ll [llength $split]

	set output "$theme(PREFIX)$announce($nuke(TYPE))"
	set output [replacevar $output "%nuker" $nuke(NUKER)]
	set output [replacevar $output "%nukees" $nuke(NUKEE)]
	set output [replacevar $output "%type" $nuke(TYPE)]
	set output [replacevar $output "%multiplier" $nuke(MULT)]
	set output [replacevar $output "%reason" $nuke(REASON)]
	set output [replacevar $output "%section" $nuke(SECTION)]
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
	global nuke sitename announce theme
	if {$nuke(SHOWN) == 1} {return 0}
	set nuke(NUKEE) [string trim $nuke(NUKEE) ", "]

	set split [split $nuke(PATH) "/"]
	set ll [llength $split]

	set output "$theme(PREFIX)$announce($nuke(TYPE))"
	set output [replacevar $output "%nuker" $nuke(NUKER)]
	set output [replacevar $output "%nukees" $nuke(NUKEE)]
	set output [replacevar $output "%type" $nuke(TYPE)]
	set output [replacevar $output "%multiplier" $nuke(MULT)]
	set output [replacevar $output "%reason" $nuke(REASON)]
	set output [replacevar $output "%section" $nuke(SECTION)]
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
		if {$chan!=$mainchan} {
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
				set output "$announce(WELCOME)"
				set output [replacevar $output "%bold" "\002"]
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
		if {$chan!=$mainchan} {
			return 0
		}
	}
	putquick "NOTICE $nick :Checking bouncer(s) status..."
	set count 1
	foreach i $bnc(LIST) {
		set i [split $i ":"]
		set loc [lindex $i 0]
		set ip [lindex $i 1]
		set port [lindex $i 2]
		set dur [clock clicks -milliseconds]
		set exitlevel [catch {exec $binary(NCFTPLS) -P $port -u $bnc(USER) -p $bnc(PASS) -t $bnc(TIMEOUT) -r 0 ftp://$ip 2>@ stdout} raw]
		set dur [expr [clock clicks -milliseconds] - $dur]
		if { $bnc(PING) == "TRUE" } {set ping ", ping: [format %.1f [lindex [split [lindex [lindex [split [ exec $binary(PING) -c1 $ip ] \"\n\"] 1] 6] \"=\"] 1]]ms"} else {set ping ""}

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
				default { set error "Unhandled Error Type?" ; putlog "DEBUG: dZSbot.tcl bnc check unhandled error type \"$raw\" please report to project-zs-ng developers" }
			}
			putquick "NOTICE $nick :$count. .$loc - $ip:$port - DOWN ($error)"
		}
		set error ""
		set raw ""
		incr count
	}
}
#################################################################################


#################################################################################
# CHOOSE STATS PARAMETERS                                                       #
#################################################################################
proc stats_user_dayup {nick uhost hand chan args} { showstats "$nick" "-u" "-t" "[lindex $args 0]" }
proc stats_user_daydn {nick uhost hand chan args} { showstats "$nick" "-d" "-t" "[lindex $args 0]" }
#################################################################################
proc stats_user_wkup {nick uhost hand chan args} { showstats "$nick" "-u" "-w" "[lindex $args 0]" }
proc stats_user_wkdn {nick uhost hand chan args} { showstats "$nick" "-d" "-w" "[lindex $args 0]" }
#################################################################################
proc stats_user_monthup {nick uhost hand chan args} { showstats "$nick" "-u" "-m" "[lindex $args 0]" }
proc stats_user_monthdn {nick uhost hand chan args} { showstats "$nick" "-d" "-m" "[lindex $args 0]" }
#################################################################################
proc stats_user_allup {nick uhost hand chan args} { showstats "$nick" "-u" "-a" "[lindex $args 0]" }
proc stats_user_alldn {nick uhost hand chan args} { showstats "$nick" "-d" "-a" "[lindex $args 0]" }
#################################################################################
proc stats_group_gpwk {nick uhost hand chan args} { showstats "$nick" "-u" "-W" "[lindex $args 0]" }
proc stats_group_gpwd {nick uhost hand chan args} { showstats "$nick" "-d" "-W" "[lindex $args 0]" }
#################################################################################
proc stats_group_gpal {nick uhost hand chan args} { showstats "$nick" "-u" "-A" "[lindex $args 0]" }
proc stats_group_gpad {nick uhost hand chan args} { showstats "$nick" "-d" "-A" "[lindex $args 0]" }
#################################################################################


#################################################################################
# Help Section                                                                  #
#################################################################################
proc help {nick uhost hand chan arg} {
	global sections cmdpre dver tclroot disable mainchan

	if { $disable(TRIGINALLCHAN) == 1 } {
		if {$chan!=$mainchan} {
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

	foreach name [array names theme] { set theme($name) [themereplace $theme($name)] }
	foreach name [array names theme_fakes] { set theme_fakes($name) [themereplace $theme_fakes($name)] }
	foreach name [array names announcetmp] { set announce($name) [themereplace $announcetmp($name)] }

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
# REPLACES THEMERELATED STUFF IN A GIVEN STRING                                 #
#################################################################################
proc themereplace {rstring} {
	global theme

	# We replace %cX{string}, %b{string} and %u{string} with their coloured, bolded and underlined equivilants ;)
	while {[regexp {(%c(\d)\{([^\{\}]+)\}|%b\{([^\{\}]+)\}|%u\{([^\{\}]+)\})} $rstring]} {
		regsub -all {%c(\d)\{([^\{\}]+)\}} $rstring {\\003$theme(COLOR\1)\2\\003} rstring
		regsub -all {%b\{([^\{\}]+)\}} $rstring {\\002\1\\002} rstring
		regsub -all {%u\{([^\{\}]+)\}} $rstring {\\037\1\\037} rstring
	}

	regsub -all {\003(\d)(?!\d)} $rstring {\\0030\1} rstring
	return [subst -nocommands $rstring]
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
			set outp [basicreplace $announce(DEBUG_DCC) $level_str]
			putlog "dZSbot debug: $outp"
		} else {
			putlog "dZSbot debug: \[$section\] $txt"
		}
	}

	if {!$disable(DEBUG)} {
		set chans $chanlist(DEFAULT)
		set outp $announce(DEFAULT)
		if {[llength [array names "chanlist" "DEBUG"]] > 0} { set chans $chanlist(DEBUG) }
		if {[llength [array names "announce" "DEBUG"]] > 0} { set outp $announce(DEBUG) }
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

if { $dzerror == "0" } {
	putlog "dZSbot loaded ok!"
} else {
	putlog "dZSbot had errors. Please check log and fix."
	if { $die_on_error == "YES" } {
		die
	}
}
