## ngbot.tcl
# New zs-c bot :)
# Code part only! Edit ngbot_conf.tcl for configuration :-)
##
# This bot is a modularized version to replace the old and 
# borken dZSbot. <3 project-zs-ng! :D
##
# Code: daxxar ^ project-zs-ng.
###########################################################

#set glroot "/glftpd/"
#set lastoct [file size "$glroot/ftp-data/glftpd.log"]
set ng_displaylevels(DEBUG)		0
set ng_displaylevels(INFO)		1
set ng_displaylevels(WARNING)	2
set ng_displaylevels(ERROR)		3
set ng_displaylevels(CRITICAL)	4


# Safe settings :D
# Gets you the setting named $name from $section, or gets
# the setting named $section if $name is unspecified or ""
proc ng_get {setting {name ""}} {
	global conf
	if {$name == ""} {
		if {![info exists conf($setting)]} {
			ng_print "WARNING" "Tried to ng_get '$setting' which is an unknown variable."
			return 0
		}
		if {![array exists conf($setting)]} {
			return $conf($setting)
		} else {
			ng_print "WARNING" "Tried to ng_get '$setting' which is an array as a scalar."
			return 0
		}
	}
	if {![array exists conf($setting)]} {
		ng_print "WARNING" "Tried to ng_get '$setting($name)', and '$setting' isn't an array, or does not exist."
		return 0
	} elseif {![info exists conf($setting:$name)]} {
		ng_print "WARNING" "Tried to ng_get '$setting($name)', and '$setting($name)' does not exist."
		return 0
	} else {
		return $conf($setting:$name);
	}
}

# Message printing :-)
# Outputs $output of level $level to console or wherever else :-)
proc ng_print {level output} {
	global ng_displaylevels ng_mindisplaylevel
	if {[ng_get "ng_displaylevels" $level] < [ng_get "mindisplaylevel"]} { return }
	ng_triggerevent "print" {$level $output}
	putlog [ng_staticformat print "%bngbot[%u$level%u]:%b $message"]
}

# Message formatting. Formats $varmessage based on $type. :)
proc ng_format {type message} {
	global ng_formats ng_formatvars ng_format
	ng_print "DEBUG" "ng_format called. TYPE: '$type' MESSAGE: '$message'"
	set type [string tolower $type]
	if {![info exists ng_formats($type)] || ![info exists ng_formatvars($type)]} {
		return [join $message "$ng_format(joinchar)"]
	}
	set output $ng_formats($type)
	set vars $ng_formatvars($type)
	set i 0
	if {[llength $message] > [llength $vars]} { set length [llength $vars]
	} else { set length [llength $message] }

	while {$i < $length} {
		set var [lindex $vars $i]
		set content [lindex $message $i]
		regsub -all -nocase $var $output $content output
	}
		

	ng_print "DEBUG" "ng_format ended. MESSAGE -> OUTPUT: '$message' -> '$output'"
	return $output
}

# Formatting of static cookies. :)
proc ng_staticformat {type msg} {
	global ng_format
	set bold ng_format(bold) "\002"
	set uline ng_format(underline) "\037"
	set color ng_format(color) "\003"
	
	if {!$ng_format(print_format) && $type == "print"} { set bold ""; set uline ""; set color ""; }
	
	regsub -all -nocase "%b" $msg $bold msg 
	regsub -all -nocase "%u" $msg $uline msg 
	regsub -all -nocase "%c" $msg $color msg 

	return $msg
}

# Binds to the event named $event, and calls $procedure
# when it is triggered.
# (procedure must accept two parameters, name of event 
#  and a list of parameters. (specific to the event))
proc ng_bind {event procedure} {
	global ng_binds ng_events
	if {![info exists ng_events($event)]} {
		ng_print "ERROR" "Attempt at binding '$procedure' to non-existant event '$event'. Ignoring."
		return
	}

	set good 0
	if {[llength [info args $procedure]] != 2} {
		if {[llength [info args $procedure]] > 2} {
			set good 1
			foreach arg [lrange [info args $procedure] 1 end] {
				if {![info default $procedure dud]} { set good 0; break }
			}
		}
		if {!$good} {
			ng_print "ERROR" "Attempt at binding '$procedure' to '$event', but it doesn't take 2 arguments!"
		}
	}
	
	lappend ng_binds($event) $procedure
	incr $ng_events($event)
}

# Removes a bind to the event named $event that was supposed
# to call $procedure.
proc ng_unbind {event procedure} {
	global ng_binds ng_events
	if {![info exists ng_events($event)]} {
		ng_print "ERROR" "Attempt at unbinding '$procedure' from non-existant event '$event'. Ignoring."
		return
	}


	incr $ng_events($event) -1
}

# Registers event $name, for triggering later on.
proc ng_registerevent {name} {
	global ng_binds ng_events

}

# Triggers the $name event, with parameters in $params
proc ng_triggerevent {name params} {
	global ng_binds ng_events

}

# Loads a plugin named $name
proc ng_loadplugin {name} {
	global plugins

}

ng_print "DEBUG" "Lol!"
