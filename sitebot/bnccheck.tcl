#!/usr/bin/tclsh
#log using ssl ? [0/1]
set ssl 1

set ip 127.0.0.1

#good port
set port 443

#bad port
set port1 4444

set user "sitebot"
set pass "b0t.be.i"

#ssl support
if { $ssl == 1 } {
 package require tls 
 tls::init
}

proc ftpcheck {host port user pass} {
	global ssl
	set timeout "false"
	if { [catch { socket -async $host $port } sock ] } {
	 return "Error: Connection Refused" 
	}
	fconfigure $sock  -translation lf -buffering line
	
	set start [clock clicks -milli]
	while {[flush $sock] == "" && [fblocked $sock]} { 
		if {[clock clicks -milli] > $start + 5000} { 
			close $sock
			return "Errror: Timeout"
			set timeout true; break 
		}
	}
	if { $timeout != "true" } {
		while {![eof $sock]} {
			if { [catch { gets $sock } line ] } {
			 return "Error: Connection problem"
			}
			puts "$line"
			set numeric [string trim [lindex [split $line] 0] "-"]
			if {[string trim $numeric "0123456789"] != ""} {set numeric "" }
			switch $numeric {
				220 { 
					if { $ssl == 1 } {
						puts "AUTH TLS" ; puts $sock "AUTH TLS" ; flush $sock
					} else {
						puts "USER $user" ; puts $sock "USER $user" ; flush $sock;
					}
				}
				234 { tls::import $sock; flush $sock;puts "USER $user" ; puts $sock "USER $user" ; flush $sock }
				331 { puts "PASS $pass" ; puts $sock "PASS $pass" ; flush $sock }
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
