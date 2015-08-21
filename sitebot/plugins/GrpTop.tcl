#################################################################################
# ngBot - Auto Announce Top Groups                                              #
#################################################################################
# 
# Description:
# - Auto announces the top groups at a configurable interval.
#
# Installation:
# 1. Add the following to your eggdrop.conf:
#    source pzs-ng/plugins/GrpTop.tcl
#
# 2. Rehash or restart your eggdrop for the changes to take effect.
#
# Changelog:
# - 20132701 - Initial release
#
#################################################################################

namespace eval ::ngBot::plugin::GrpTop {
	variable ns [namespace current]
	variable np [namespace qualifiers [namespace parent]]

	variable top

	## Config Settings ###############################
	##
	## Interval between announces in seconds (default: 7200 - 2 hours)
	set top(interval)   10800
	##
	## Section to display (0 = DEFAULT)
	set top(sect)       0
	##
	## Maximum number of users to display
	set top(groups)      5
	##
	## Message prefix
	set top(prefix)     "\002charts\002"
	set top(header)     "$top(prefix) > This week impressive groups >"

	##
	## Output channels
	set top(chan)       "#YOURCHAN"
	##
	##################################################

	set top(version) "20132701"

	variable timer


	proc init {args} {
		variable top
		[namespace current]::startTimer
		putlog "\[ngBot\] GrpTop :: Loaded successfully (Version: $top(version))."
	}

	proc deinit {args} {
		[namespace current]::killTimer

		namespace delete [namespace current]
	}

	proc killTimer {} {
		variable timer

		if {[catch {killutimer $timer} error] != 0} {
			putlog "\[ngBot\] GrpTop :: Warning: Unable to kill announce timer \"$error\""
		}
	}

	proc startTimer {} {
		variable top

		variable timer [utimer $top(interval) "[namespace current]::showTop"]
	}

	proc showTop {args} {
		variable np
		variable ns
		variable top
		variable ${np}::binary
		variable ${np}::location

		[namespace current]::startTimer

		if {[catch {exec $binary(STATS) -r $location(GLCONF) -u -W -x $top(groups) -s $top(sect)} output] != 0} {
			putlog "\[ngBot\] GrpTop :: Error: Problem executing stats-exec \"$output\""
			return
		}

		set msg [list]
		foreach line [split $output "\n"] {
			regsub -all -- {(\s+)\s} $line " " line

			if {[regexp -- {^\[(\d+)\] (.*?) (\d+) (\d+)\w+ (\d+)} $line -> pos groupname files bytes members]} {
				lappend msg "$pos. $groupname \002$bytes\002MB "
			}
		}

		if {[llength $msg] == 0} {
			set msg "Not enough data in the pipe yet..."
		}

		foreach chan [split $top(chan)] {
			putquick "PRIVMSG $chan : "
			putquick "PRIVMSG $chan :$top(header)"
			putquick "PRIVMSG $chan :$top(prefix) > [join $msg " ! "]"
			putquick "PRIVMSG $chan : "
		}
	}
}
