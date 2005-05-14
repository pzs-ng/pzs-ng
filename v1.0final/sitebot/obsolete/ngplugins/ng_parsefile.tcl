## ngplugins/ng_parsefile.tcl
# New zs-c bot :)
# Code part only! Edit ngbot_conf.tcl for configuration :-)
##
# This bot is a modularized version to replace the old and 
# borken dZSbot. <3 project-zs-ng! :D
##
# Code: daxxar ^ project-zs-ng.
###########################################################

proc parselog {} {
	global glroot lastoct disable defaultsection variables msgtypes chanlist dZStimer use_glftpd2

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







