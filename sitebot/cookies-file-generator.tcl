#!/usr/bin/tclsh

source ngBot.conf.defaults
source ngBot.conf.dist
source ngBot.vars

puts "Quickreference of cookies:"
puts "        (ANNOUNCES)       "
puts "##########################"
puts "## Read %u_ as 'User'"
puts "## Read %g_ as 'Group'"
puts "## Read %t_ as 'Total'"
puts ""

foreach var [lsort -ascii [array names "variables"]] {
	if {[lindex $variables($var) 0] == "%pf"} {
		set variables($var) [lreplace $variables($var) 0 0]
	}
	puts "# $var:"
	set vars [split $variables($var)]
	regsub -all {[{}]} $vars "" vars
	while {[llength $vars]} {
		set str [lindex $vars 0]
		set vars [lreplace $vars 0 0]
		while {[string length $str] < 80 && [llength $vars]} {
			set str [concat "$str," [lindex $vars 0]]
			set vars [lreplace $vars 0 0]
		}
		puts $str
	}
#	puts "[join [lsort -ascii [split $variables($var)]] ", "]"
	puts ""
}
