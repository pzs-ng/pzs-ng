#!/usr/bin/tclsh
set ip 127.0.0.1
set port 443
set port1 444
set user "TML"
set pass "b0t.be.i"
proc ftpcheck {host port user pass} {
	set timeout "false"
	catch { set sock [socket -async $host $port] }
	puts "sock=$sock"
	if { $sock == "sock7" } { return "Error: Connection Refused" }
#	fconfigure $sock -blocking no
	set start [clock clicks -milli]
	while {[flush $sock] == "" && [fblocked $sock]} { 
		if {[clock clicks -milli] > $start + 2000} { 
			close $sock
			return "DOWN - Timeout"
			set timeout true; break 
		}
	}
	puts "STATUS: connection established"
	if { $timeout != "true" } {
		while {![eof $sock]} {
			set line [gets $sock]
			puts "line=$line"
			set numeric [lindex [split $line] 0]
			puts "numeric=$numeric"
			if {[string trim $numeric "0123456789"] != ""} {set numeric "" }
			switch $numeric {
				220 { puts "USER $user" ; puts $sock "USER $user" }
				331 { puts "PASS $pass" ; puts $sock "PASS $pass" }
				230 { close $sock ; return "logged in" }
				530 { close $sock ; return "failed login -user" }
				default  { close $sock ; return "failed login" }
			}
		}
	}
	close $sock
}
puts "ftpcheck: checking bnc ftp://$user:$pass@$ip:$port1 should fail"
puts [ ftpcheck $ip $port1 $user $pass ]
puts ""
puts "ftpcheck: checking bnc ftp://$user:$pass@$ip:$port should succede" 
puts [ ftpcheck $ip $port $user $pass ]
