#################################################################################
# Code part only, main config is moved to dZSbconf.tcl :)                       #
#            THIS MEANS THAT YOU ARENT SUPPOSED TO EDIT THIS FILE               #
#                        FOR CONFIGURATION PURPOSES!                            #
#################################################################################

#################################################################################
# READING THE CONFIG (and NOT being a cryptic bitch;)                           #
#################################################################################
set dver "0.0.4"
set dzerror "0"

putlog "Launching dZSBot (v$dver) for zipscript-c..."

if {[catch {source [file dirname [info script]]/dZSbconf.tcl} tmperror]} {
    putlog "dZSbot: dZSbconf.tcl not found. Cannot continue."
    die
}

foreach bin [array names binary] { 
    if {![file executable $binary($bin)]} {
        putlog "dZSbot: Wrong path/missing bin for $bin - Please fix."
		set dzerror "1"
    }
}

#################################################################################
# SOME IMPORTANT GLOBAL VARIABLES                                               #
#################################################################################

set lastoct [file size $location(GLLOG)]
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

# Where?   Flags    What?       Proc to call
bind pub    -|- [set cmdpre]who     who
bind pub    -|- [set cmdpre]speed   speed
bind pub    -|- [set cmdpre]bw      ng_bandwidth
bind pub    -|- [set cmdpre]bnc     ng_bnc_check
bind pub    -|- [set cmdpre]free    show_free

bind pub    -|- [set cmdpre]dayup   stats_user_dayup
bind pub    -|- [set cmdpre]wkup    stats_user_wkup
bind pub    -|- [set cmdpre]monthup stats_user_monthup
bind pub    -|- [set cmdpre]allup   stats_user_allup

bind pub    -|- [set cmdpre]daydn   stats_user_daydn
bind pub    -|- [set cmdpre]wkdn    stats_user_wkdn
bind pub    -|- [set cmdpre]monthdn stats_user_monthdn
bind pub    -|- [set cmdpre]alldn   stats_user_alldn

bind pub    -|- [set cmdpre]gpwk    stats_group_gpwk
bind pub    -|- [set cmdpre]gpal    stats_group_gpal
bind pub    -|- [set cmdpre]inc     show_incompletes

bind pub    -|- [set cmdpre]gwpd    stats_group_gpwd
bind pub    -|- [set cmdpre]gpad    stats_group_gpad
bind pub    -|- [set cmdpre]help    help

bind join   -|- *                   welcome_msg

bind msg    -|- !invite             invite


#################################################################################
# MAIN LOOP - PARSES DATA FROM GLFTPD.LOG                                       #
#################################################################################
proc readlog {} {
    global location lastoct disable defaultsection variables msgtypes chanlist dZStimer use_glftpd2

    set dZStimer [utimer 1 "readlog"]

    set glftpdlogsize [file size $location(GLLOG)]

    if {$glftpdlogsize == $lastoct} { return 0 }
    if {$glftpdlogsize  < $lastoct} { set lastoct 0 }
    if {[catch {set of [open $location(GLLOG) r]} ]} { return 0 }

    seek $of $lastoct
    while {![eof $of]} {
        set line [gets $of]
        if {$line == ""} {continue}
        set msgtype [string trim [lindex $line 5] ":"]
        set path [lindex $line 6]

        if {![string compare $msgtype "INVITE"]} {
            set nick [lindex $line 6]
            foreach channel $chanlist(INVITE) { puthelp "INVITE $nick $channel" }
        }

        set section [getsection $path $msgtype]
        if {[denycheck "$path"] == 0} {
            if {[string compare "$section" "$defaultsection"]} {
                if {$disable($msgtype) == 0 || $disable(DEFAULT) == 0} {
                    if {[info exists variables($msgtype)]} {
                        set echoline [parse $msgtype [lrange $line 6 end] $section]
                        sndall $section $echoline
                    } else {
                        set echoline [parse DEFAULT [lrange $line 6 end] $section]
                        sndall $section $echoline
                    }
                }
            } else {
                if {[lsearch -glob $msgtypes(DEFAULT) $msgtype] != -1} {
                    if {$disable($msgtype) == 0} {
                        set echoline [parse $msgtype [lrange $line 6 end] "DEFAULT"]
                        sndall "DEFAULT" $echoline
                    }
                } else {
                    if {$disable(DEFAULT) == 0} {
                        set echoline [parse $msgtype [lrange $line 6 end] "DEFAULT"]
                        sndall "DEFAULT" $echoline
                    }
                }
            }
        }
    }

    close $of
    set lastoct [file size $location(GLLOG)]
    if { $use_glftpd2 != "YES" } {
       launchnuke
    }
    return 0
}
#################################################################################



#################################################################################
# GET SECTION NAME (BASED ON PATH)                                              #
#################################################################################
proc getsection {cpath msgtype} {
    global sections msgtypes paths type defaultsection mpath

    foreach section $sections {
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
proc parse {msgtype msgline section} {
    global variables announce random mpath use_glftpd2 theme theme_fakes defaultsection

    set type $msgtype

    if {![string compare $type "NUKE"] || ! [string compare $type "UNNUKE"]} {
		if { $use_glftpd2 != "YES" } {
			fuelnuke $type [lindex $msgline 0] $section $msgline
		} else {
			launchnuke2 $type [lindex $msgline 0] $section [lrange $msgline 1 3] [lrange $msgline 4 end]
		}
        return ""
    }

    if {![info exists announce($type)] || ! [info exists variables($type)]} { set type "DEFAULT" }

    set vars $variables($type)

	if {![string compare [lindex $announce($type) 0] "random"] && [string is alnum -strict [lindex $announce($type) 1]] == 1} {
		set output $random($msgtype\-[rand [lindex $announce($type) 1]])
	} else {
		set output $announce($type)
	}

	set output "$theme(PREFIX) $output"
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

        set output [replacevar $output "%release" [lindex $split [expr $ll -1]]]
        set output [replacevar $output "%path" [lindex $split [expr $ll -2]]]
        set vars [string range $vars 4 end]
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
    global binary
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
    global binary announce

    set output $announce(DEFAULT)
    set output [replacevar $output "%msg" [exec $binary(WHO) [lindex $args 0]]]
    set output [basicreplace $output "SPEED"]
    putserv "PRIVMSG $chan :$output "
}
#################################################################################




#################################################################################
# POST BANDWIDTH                                                                #
#################################################################################
proc bandwidth {nick uhost hand chan args} {
    global binary announce

    set output $announce(BW)
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
# UPDATED BANDWIDTH								#
#################################################################################
proc ng_bandwidth {nick uhost hand chan args} { global binary announce speed

	set output $announce(BW)
	set raw [exec $binary(BW)]
	set $upper [format "%.0f" [expr [lindex $raw 1] / $speed(INCOMING)]]]
	set $dnper [format "%.0f" [expr [lindex $raw 3] / $speed(OUTGOING)]]]
	set $totalper [format "%.0f" [expr [lindex $raw 5] / ( $speed(INCOMING) + $speed(OUTGOING) )]]]

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
    global binary statsection

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

    foreach line [split [exec $binary(STATS) $type $time -s $sect] "\n"] {
        if {![info exists newline($line)]} {
            set newline($line) 0
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
    global location binary chanlist announce

    if {[llength $arg] == 2} {
        set username [lindex $arg 0]
        set password [lindex $arg 1]
        set result [exec $binary(PASSCHK) $username $password $location(PASSWD)]
        set group ""

        set userfile $location(USERS)$username


        if {![string compare $result "MATCH"]} {
            set output $announce(MSGINVITE)
            foreach channel $chanlist(INVITE) { puthelp "INVITE $nick $channel" }
            foreach line [split [exec $binary(CAT) $userfile] "\n"] {
                if {![string compare [lindex $line 0] "GROUP"]} {
                    set group [lrange $line 1 end]
                    break
                }
            }
        } else {
            if {![string compare $result "NOMATCH"]} {
                set output $announce(BADMSGINVITE)
            } else { set output $announce(BADMSGINVITE)  }
        }

        set output [replacevar $output "%ircnick" $nick]
        set output [replacevar $output "%user" $username]
        set output [replacevar $output "%host" $host]
        set output [replacevar $output "%group" $group]
        set output [basicreplace $output "INVITE"]
        sndall "DEFAULT" $output
    }
}
#################################################################################



#################################################################################
# SHOW FREE SPACE                                                               #
#################################################################################
proc show_free {nick uhost hand chan arg} {
    global binary announce device

    set output $announce(FREE)
    set devices ""; set free 0; set used 0
    set total 0; set num 0; set perc 0
    foreach line [split [exec $binary(DF) "-Ph"] "\n"] {
        foreach dev [array names device] {
            if {[string match [lindex $line 0] [lindex $device($dev) 0]] == 1} {
                set tmp [replacevar $announce(FREE-DEV) "%total" "[lindex $line 1]B"]
                set tmp [replacevar $tmp "%used" "[lindex $line 2]B"]
                set tmp [replacevar $tmp "%free" "[lindex $line 3]B"]
                set tmp [replacevar $tmp "%percentage" "[string trim [lindex $line 4] " %"]"]
                set tmp [replacevar $tmp "%section" [lrange $device($dev) 1 end]]
                append devices $tmp

                incr total [to_mb [lindex $line 1]]; incr used [to_mb [lindex $line 2]]
                incr free [to_mb [lindex $line 3]]; incr num
                incr perc [string trim [lindex $line 4] " %"]
            }
        }
    }
    set output [replacevar $output "%total" $total]
    set output [replacevar $output "%used" $used]
    set output [replacevar $output "%free" $free]
    set output [replacevar $output "%percentage" [expr round($perc/$num)]]
    set output [replacevar $output "%devices" $devices]
    set output [basicreplace $output "FREE"]
    putserv "PRIVMSG $chan :$output"
}
#################################################################################


#################################################################################
# LAUNCH A NUKE (GL2.0)                                                         #
#################################################################################
proc launchnuke2 {type path section sargs dargs} { global nuke hidenuke announce sitename
 set nuke(TYPE) $type
 set nuke(PATH) $path
 set nuke(SECTION) $section
 set nuke(NUKER) [lindex $sargs 0]
 set nuke(MULT) [lindex $sargs 1]
 set nuke(REASON) [lindex $sargs 2]
 set nuke(NUKEE) {}

 foreach entry $dargs {
  set mb [format "%.1f" [expr [lindex $entry 1] / 1024]]
  append nuke(NUKEE) "\002[lindex $entry 0]\002 (\002$mb\002MB), "
 }

 set nuke(NUKEE) [string trim $nuke(NUKEE) ", "]
 set split [split $nuke(PATH) "/"]
 set ll [llength $split]

 set output $announce($nuke(TYPE))
 set output [replacevar $output "%nuker" $nuke(NUKER)]
 set output [replacevar $output "%nukees" $nuke(NUKEE)]
 set output [replacevar $output "%type" $nuke(TYPE)]
 set output [replacevar $output "%mult" $nuke(MULT)]
 set output [replacevar $output "%reason" $nuke(REASON)]
 set output [replacevar $output "%section" $nuke(SECTION)]
 set output [replacevar $output "%release" [lindex $split [expr $ll -1]]]
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
    global nuke sitename announce
    if {$nuke(SHOWN) == 1} {return 0}
    set nuke(NUKEE) [string trim $nuke(NUKEE) ", "]

    set split [split $nuke(PATH) "/"]
    set ll [llength $split]

    set output $announce($nuke(TYPE))
    set output [replacevar $output "%nuker" $nuke(NUKER)]
    set output [replacevar $output "%nukees" $nuke(NUKEE)]
    set output [replacevar $output "%type" $nuke(TYPE)]
    set output [replacevar $output "%mult" $nuke(MULT)]
    set output [replacevar $output "%reason" $nuke(REASON)]
    set output [replacevar $output "%section" $nuke(SECTION)]
    set output [replacevar $output "%release" [lindex $split [expr $ll -1]]]
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
    global sitename binary

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
proc welcome_msg { nick uhost hand chan } {
    global announce disable chanlist

    if {$disable(WELCOME) == 0} {
        foreach c_chan $chanlist(WELCOME) {
            if {[string match -nocase $c_chan $chan] == 1} {
                puthelp "NOTICE $nick : $announce(WELCOME)"
            }
        }
    }
}

#################################################################################
# SHOW BNC LIST                                                                 #
#################################################################################
proc bnc {nick uhost hand chan arg} {
    global bnc sitename binary
    putquick "NOTICE $nick : list of bnc's for $sitename"
    foreach eachbnc $bnc(LIST) {
        if {$eachbnc == ""} {continue}
        set ipport [split $eachbnc ":"]
        set status [exec $binary(BNCTEST) "$binary(NCFTPLS)" "$bnc(USER)" "$bnc(PASS)" "[lindex $ipport 1]" "$bnc(TIMEOUT)" "[lindex $ipport 0]"]
        puthelp "NOTICE $nick : $eachbnc - $status"
    }
}
#################################################################################



#################################################################################
# CHECK BOUNCER STATUSES							#
#################################################################################
proc ng_bnc_check { nick host hand arg } { global bnc sitename binary errorCode
        putquick "NOTICE $nick :Checking bouncer(s) status..."
        foreach line [split [exec $binary(BNCCHECK) $binary(NCFTPLS) $bnc(USER) $bnc(PASS) $bnc(TIMEOUT) $bnc(LIST)] "\n"]  {
                putquick "NOTICE $nick :$line"
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
#                                  Help Section                                 #
#################################################################################
proc help {nick uhost hand chan arg} {
    global sections cmdpre dver

    if {![file exist scripts/dZSbot.help]} {
        puthelp "PRIVMSG $nick : help file dZSbot.help is missing please check install"
        return 0
    }

    puthelp "PRIVMSG $nick : -------zipscript-c--------"
    puthelp "PRIVMSG $nick : - Dark0n3's sitebot help -"
    puthelp "PRIVMSG $nick : ---------v$dver-----------"
    puthelp "PRIVMSG $nick : "

    set helpfile [open scripts/dZSbot.help r]
    set helpdb [read $helpfile]
    close $helpfile
    foreach line [split $helpdb "\n"] {
        regsub -all "%cmdpre" "$line" "$cmdpre"
        puthelp "PRIVMSG $nick :$line"
    }
    puthelp "PRIVMSG $nick : Valid sections are: $sections"
}
#################################################################################



#################################################################################
# LOAD A THEME FILE                                                             #
#################################################################################
proc loadtheme {file} {
	global theme announce

	if {[string index $file 0] != "/"} { set file "[file dirname [info script]]/$file" }
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
				set setting [string toupper $setting]
				set theme_fakes($setting) $value
			} elseif {[regexp -nocase -- {announce\.(\S+)\s*=\s*(['\"])(.+)\2} $line dud setting quote value]} {
				set setting [string toupper $setting]
				set announcetmp($setting) $value
			} elseif {[regexp -nocase -- {(\S+)\s*=\s*(['\"])(.+)\2} $line dud setting quote value]} {
				set setting [string toupper $setting]
				set theme($setting) $value
			}
		}
	}
	
	foreach name [array names theme] { set theme($name) [themereplace $theme($name)] }
	foreach name [array names theme_fakes] { set theme_fakes($name) [themereplace $theme_fakes($name)] }
	foreach name [array names announcetmp] { set announce($name) [themereplace $announcetmp($name)] }

	set ret 1
	set required "PREFIX"
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
	regsub -all {%c(\d)\{([^\}]+)\}} $rstring {\\003$theme(COLOR\1)\2\\003} rstring
	regsub -all {%b\{([^\}]+)\}} $rstring {\\002$theme(COLOR\1)\2\\002} rstring
	regsub -all {%u\{([^\}]+)\}} $rstring {\\037$theme(COLOR\1)\2\\037} rstring
	
	regsub -all {\003(\d)(?!\d)} $rstring {\\0030\1} rstring
	regsub -all {\[} $rstring {\\[} rstring; regsub -all {\]} $rstring {\\]} rstring
	return [subst $rstring]
}
#################################################################################



if {[info exists dZStimer]} {
    if {[catch {killutimer $dZStimer} err]} {
        putlog "dZSbot.tcl: killutimer failed ($err)"
    }
}
set dZStimer [utimer 1 "readlog"]

if {![loadtheme $announce(THEMEFILE)]} {
	if {[loadtheme "default.zst"]} {
		putlog "dZSbot: Couldn't load theme '$announce(THEMEFILE)', loaded 'default.zst' instead!"
	} else {
		putlog "dZSbot: Couldn't load theme '$announce(THEMEFILE)' and not 'default.zst' either. Cannot continue!"
		set dzerror 1
	}
}

if { $dzerror == "0" } {
	putlog "dZSbot loaded ok!"
} else {
	putlog "dZSbot had errors. Please check log and fix."
	die
}


