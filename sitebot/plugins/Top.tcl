#################################################################################
# ngBot - Auto Announce Top Uploaders                                           #
#################################################################################
# 
# Description:
# - Auto announces the top uploaders at a configurable interval.
#
# Installation:
# 1. Add the following to your eggdrop.conf:
#    source pzs-ng/plugins/Top.tcl
#
# 2. Rehash or restart your eggdrop for the changes to take effect.
#
#################################################################################

namespace eval ::ngBot::plugin::Top {
	variable ns [namespace current]
	variable np [namespace qualifiers [namespace parent]]

	variable top

	## Config Settings ###############################
	##
	## Interval between announces in seconds (default: 7200 - 2 hours)
	set top(interval)   7200
	##
	## Section to display (0 = DEFAULT)
	set top(sect)       0
	##
	## Maximum number of users to display
	set top(users)      10
	##
	## Message prefix
	set top(prefix)     "Week Top (Up) "
	##
	## Output channels
	set top(chan)       "#pzs-ng"
	##
	##################################################

	variable timer


	proc init {args} {
		[namespace current]::startTimer
	}

	proc deinit {args} {
		[namespace current]::killTimer

		namespace delete [namespace current]
	}

	proc killTimer {} {
		variable timer

		if {[catch {killutimer $timer} error] != 0} {
			putlog "\[ngBot\] Top :: Warning: Unable to kill announce timer \"$error\""
		}
	}

	proc startTimer {} {
		variable top

		variable timer [utimer $top(interval) "[namespace current]::showTop"]
	}

	proc showTop {args} {
		variable top
		variable ${np}::binary
		variable ${np}::location

		[namespace current]::startTimer

		if {[catch {exec $binary(STATS) -r $location(GLCONF) -u -w -x $top(users) -s $top(sect)} output] != 0} {
			putlog "\[ngBot\] Top :: Error: Problem executing stats-exec \"$output\""
			return
		}

		set msg [list]
		foreach line [split $output "\n"] {
			regsub -all -- {(\s+)\s} $line " " line

			if {[regexp -- {^\[(\d+)\] (\w+) (.*?) (\d+) (\d+)\w+ (\S+)} $line -> pos username tagline files bytes speed]} {
				lappend msg "\[$pos. $username \002$bytes\002M\]"
			}
		}

		if {[llength $msg] == 0} {
			set msg "Empty..."
		}

		foreach chan [split $top(chan)] {
			puthelp "PRIVMSG $chan :$top(prefix)[join $msg " "]"
		}
	}
}