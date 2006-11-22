namespace eval ::ngBot::Top {
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

	bind evnt -|- prerehash [namespace current]::DeInit
}

proc ::ngBot::Top::Init {args} {
	[namespace current]::startTimer

	putlog "\[ngBot\] Top :: Loaded successfully."
}

proc ::ngBot::Top::DeInit {args} {
	[namespace current]::killTimer

	namespace delete [namespace current]
}

proc ::ngBot::Top::killTimer {} {
	variable timer

	if {[catch {killutimer $timer} error] != 0} {
		putlog "\[ngBot\] Top :: Warning: Unable to kill announce timer \"$error\""
	}
}

proc ::ngBot::Top::startTimer {} {
	variable top

	variable timer [utimer $top(interval) "[namespace current]::showTop"]
}

proc ::ngBot::Top::showTop {args} {
	global location binary

	variable top

	[namespace current]::startTimer

	if {[catch {exec $binary(STATS) -r location(GLCONF) -u -w -x $top(users) -s $top(sect)} output] != 0} {
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

::ngBot::Top::Init