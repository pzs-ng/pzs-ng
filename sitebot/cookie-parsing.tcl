#!/usr/bin/tclsh
#
# - ParseCookies
#
# Description:
#  Based on dark0n3's project-zs convert() function, this TCL procedure will
#  substitute all cookies with the list of values in the input string.
#
# Parameters:
#  InputStr   - Input string to be parsed.
#  ValueList  - Values to be replaced (the order coincides with the CookieList parameter).
#  CookieList - Cookie names (see above comment).
#
# Returns:
#  The formatted string with all cookies substituted and formatted accordingly.
#
proc ParseCookies {InputStr ValueList CookieList} {
	set InputLen [string length $InputStr]
	set OutputStr ""

	for {set InputIdx 0} {$InputIdx < $InputLen} {incr InputIdx} {
		if {[string index $InputStr $InputIdx] == "%"} {
			## Save this index for invalid cookies
			set StartIdx $InputIdx

			## Find position field
			set BeforeIdx [incr InputIdx]
			if {[string index $InputStr $InputIdx] == "-"} {
				## Ignore the negative sign if a does not number follow, for example: %-(cookie)
				if {[string is digit -strict [string index $InputStr [incr InputIdx]]]} {incr InputIdx} else {incr BeforeIdx}
			}
			while {[string is digit -strict [string index $InputStr $InputIdx]]} {incr InputIdx}
			if {$BeforeIdx != $InputIdx} {
				set RightPos [string range $InputStr $BeforeIdx [expr {$InputIdx - 1}]]
			} else {
				set RightPos 0
			}

			## Find minimum/precision field
			if {[string index $InputStr $InputIdx] == "."} {
				set BeforeIdx [incr InputIdx]
				## Ignore the negative sign, for example: %.-(cookie)
				if {[string index $InputStr $InputIdx] == "-"} {incr BeforeIdx; incr InputIdx}
				while {[string is digit -strict [string index $InputStr $InputIdx]]} {incr InputIdx}
				if {$BeforeIdx != $InputIdx} {
					set LeftPos [string range $InputStr $BeforeIdx [expr {$InputIdx - 1}]]
				} else {
					set LeftPos 0
				}
			} else {
				## TCL's [format ...] function doesn't accept -1 for the minimum field
				## like printf() does - so a reasonably large number will suffice.
				set LeftPos 999999
			}

			## Find cookie name
			if {[string index $InputStr $InputIdx] == "("} {
				set BeforeIdx [incr InputIdx]
				while {[string index $InputStr $InputIdx] != ")" && [string index $InputStr $InputIdx] != ""} {incr InputIdx}
				set CookieName [string range $InputStr $BeforeIdx [expr {$InputIdx - 1}]]
			} else {
				## Invalid cookie format, an open parenthesis is expected at this point.
				append OutputStr [string range $InputStr $StartIdx $InputIdx]
				continue
			}

			if {[set CookiePos [lsearch -exact $CookieList $CookieName]] != -1} {
				set Value [lindex $ValueList $CookiePos]
				## Type of cookie replacement to perform.
				if {[string is integer -strict $Value]} {
					## Avoid OCTAL interpretation by trimming the leading zero's
					set Value [string trimleft $Value "0"]
					append OutputStr [format "%${RightPos}i" $Value]
				} elseif {[regexp {^-?[0-9]+\.[0-9]+$} $Value]} {
					append OutputStr [format "%${RightPos}.${LeftPos}f" $Value]
				} else {
					append OutputStr [format "%${RightPos}.${LeftPos}s" $Value]
				}
			} else {
				## Append the starting point of the cookie to the current index in hope that
				## the user will notice that he or she has made an error in the template line.
				append OutputStr [string range $InputStr $StartIdx $InputIdx]
			}
		} else {
			append OutputStr [string index $InputStr $InputIdx]
		}
	}
	return $OutputStr
}

## Test strings
set TestCase(1) "User: %(username)/%(group) Files: %(files) Size: %(size) Speed: %.0(speed)"
set TestCase(2) "User: %10.3(username)/%-10(group) Files: %02(files) Size: %(size) Speed: %9.1(speed)"
set TestCase(3) "User: %5(username)/%.5(group) Files: %02(files) Size: %-7(size) Speed: %.0(speed)"
set TestCase(4) "User: %3.3(username)/%4.4(group) Files: %02(files) Size: %-7(size) Speed: %.2(speed)"
set TestCase(5) "Testing percent sign handling: % or 95%, double time %%!"
set TestCase(6) "Invalid cookie position and precision fields: %.(speed)/%-(username)/%.-(username)/(expecting \"\")"
set TestCase(7) "Error handling %(fakecookie) with %(username) and %error) %(size)"
set TestCase(8) "No closing parenthesis %(username whoops, %(group)!"
set TestCase(9) "We've reached the end and still no closing parenthesis! %(username"

## Cookies
set ValueList	"neoxed STAFF 1 200KB 12345.6789"
set CookieList	"username group files size speed"

## Test cases
puts "Testing ParseCookies function..."
set TestCases [lsort [array names TestCase]]

set fp [open "CookieResults.txt" w]
foreach Element $TestCases {
	set Data [set TestCase($Element)]
	puts "Test case: $Element"
	puts $fp "-\[Test Case #$Element\]-------------------------------------------"
	puts $fp "Before: $Data"
	puts $fp "After : [ParseCookies $Data $ValueList $CookieList]"
	puts $fp "1000x : [time {ParseCookies $Data $ValueList $CookieList} 1000]\n\n"
	flush $fp
}
close $fp

puts "Finished."
return
