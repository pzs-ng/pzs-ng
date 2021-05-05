################################################################################
#
#                            ngBot - Sitebot
#            Project Zipscript - Next Generation (www.pzs-ng.com)
#    (Based on dZSbot by Dark0n3 - http://kotisivu.raketti.net/darkone/)
#
################################################################################
#
# TODO:
#
#   - Invites are somewhat hardcoded into readlog, but it seems to be glftpd
#     only. need to look into moving this to the glftpd tcl, along with
#     cmd_invite and the other invite procs.
#   - Only deinit and unbind the binds of the plugins that were successfully
#     init'd. not all sub namespaces.
#   - init_logs has glftpd specific code it seems. ie the glftpdlog variable, but
#     i assume this is used by cuftpd as well.
#   - Possibly create a rename_ng, unrename_ng & exec_ng function for modules
#     that rename procs. At the moment we only support a function being renamed
#     once.
#     * As an after thought: The way its been done a proc can choose not to
#       continue the chain loading. What happens when you have 2 procs that
#       rename the original and the first decides to stop the chain.
#     * A solution I came up with was just creating 2 levels. The first which is
#       also run first would be the ones that continue chain loading and second
#       are the ones that halt it.
#   - ::ngBot::eventhandler will stop processing precommands if one returns false.
#   - Create rename_ng and derename_ng!
#
################################################################################

namespace eval ::ngBot {
	variable ns [namespace current]
	variable dzerror 0
	variable ng_timer
	variable scriptpath [file dirname [info script]]

	variable zeroconvert

	#############################################################################
	# De/Initialisation Commands                                                #
	#############################################################################

	proc init_binds {} {
		variable ns
		variable cmdpre
		variable bindnopre
		variable trigger
		variable triggerflag
		variable defaulttriggerflag

		if {[istrue $bindnopre] && [lsearch [split $cmdpre] "!"] == -1} {
			set cmdpre "$cmdpre !"
		}

		foreach pre [split $cmdpre] {
			foreach {cmd trigs} [array get trigger] {
				# Check if cmd isn't disabled
				if {[string length "$trigs"]} {
					# process $cmd, split by _ and use 0 as procname (prefixing with cmd_) and rest as args
					set cmdlist [split $cmd "_"]
					set cmdname ${ns}::cmd_[lindex $cmdlist 0]
					# check if the cmd is a real proc
					if {[llength [info procs $cmdname]]} {
						lset cmdlist 0 $cmdname
						foreach trig [split $trigs] {
							set flags "$defaulttriggerflag"
							if {[array exists triggerflag] && [info exists triggerflag($trig)]} {
								set flags $triggerflag($trig)
							}
							bind pub "$flags" $pre$trig $cmdlist
						}
					}
				}
			}
		}
	}

	# Uses _ng to avoid being overwriten by namespace import.
	proc init_ng {type} {
		variable ns
		variable dzerror
		variable die_on_error
		variable ng_is_loaded

		if {$ng_is_loaded} {
			return
		}

		${ns}::init_version
		${ns}::init_binaries
		${ns}::init_logs
		${ns}::init_invite
		${ns}::init_theme
		${ns}::init_timer
		${ns}::init_binds
		${ns}::init_plugins

		if {!$dzerror} {
			putlog "\[ngBot\] Loaded successfully!"
		} else {
			putlog "\[ngBot\] Errors were encountered while loading, refer to the messages above for details."
			if {[istrue $die_on_error]} { die }
		}

		bind evnt -|- prerehash ${ns}::deinit_ng
		# Prevent re-init when only userfile is (re)loaded, like with botnet userfile sharing
		set ng_is_loaded 1
	}

	proc init_version {} {}

	proc init_binaries {} {
		variable ns
		variable bnc
		variable binary
		variable dzerror
		variable enable_irc_invite

		## Check binary and file locations
		foreach {filename filepath} [array get binary] {
			if {![istrue $bnc(ENABLED)]} {
				if {[string equal "CURL" $filename]} {continue}
				if {[string equal "PING" $filename] && ![istrue $bnc(PING)]} {continue}
			}
			if {![istrue $enable_irc_invite] && [string equal "PASSCHK" $filename]} {continue}

			if {![file executable $filepath]} {
				putlog "\[ngBot\] Error :: Invalid path/permissions for $filename ($filepath)."
				set dzerror 1
			}
		}
	}

	proc init_logs {} {
		variable ns
		variable dzerror
		variable loglist
		variable lastread
		variable loginlog
		variable sysoplog
		variable glftpdlog

		## Logs to parse
		set logid 0
		set loglist {}
		foreach {varname logtype} {glftpdlog 0 loginlog 1 sysoplog 2} {
			foreach {filename filepath} [array get $varname] {
				if {![file readable $filepath]} {
					putlog "\[ngBot\] Error :: Unable to read the log file \"$filepath\"."
					set dzerror 1
				} else {
					lappend loglist $logtype [incr logid] [file normalize $filepath]
					set lastread($logid) [file size $filepath]
				}
			}
		}
		if {!$logid} {
			putlog "\[ngBot\] Error :: No logs found!"
			set dzerror 1
		} else {
			putlog "\[ngBot\] Number of logs found: $logid"
		}
	}

	proc init_invite {} {
		variable ns
		variable chanlist
		variable invite_channels

		## Invite checks
		if {![info exists invite_channels] && [info exists chanlist(INVITE)]} {
			putlog "\[ngBot\] Warning :: No \"invite_channels\" defined in the config, setting to \"$chanlist(INVITE)\" (chanlist(INVITE))"
			set invite_channels $chanlist(INVITE)
		}
	}

	proc init_theme {} {
		variable ns
		variable dzerror
		variable disable
		variable announce
		variable chanlist
		variable variables
		variable msgreplace

		## Load the theme file
		if {![${ns}::loadtheme $announce(THEMEFILE)]} {
			set invalidtheme $announce(THEMEFILE)
			if {[${ns}::loadtheme "themes/default.zst"]} {
				putlog "\[ngBot\] Warning :: Unable to load theme $invalidtheme, loaded default.zst instead."
			} else {
				putlog "\[ngBot\] Error :: Unable to load the themes $invalidtheme and default.zst."
				set dzerror 1
			}
		}

		## Default channels and variables
		if {![array exists chanlist] || ![info exists chanlist(DEFAULT)]} {
			putlog "\[ngBot\] Error :: No entry in chanlist set, or \"chanlist(DEFAULT)\" not set."
			set dzerror 1
		}
		if {![array exists announce] || ![info exists announce(DEFAULT)]} {
			putlog "\[ngBot\] Warning :: No \"announce\" entries defined, or \"announce(DEFAULT)\" is not set."
			set announce(DEFAULT) "\[DEFAULT\] %msg"
		}
		if {![array exists variables] || ![info exists variables(DEFAULT)]} {
			putlog "\[ngBot\] Warning :: No \"variables\" entries defined, or \"variables(DEFAULT)\" is not set."
			set variables(DEFAULT) "%pf %msg"
		}
		if {![array exists disable] || ![info exists disable(DEFAULT)]} {
			putlog "\[ngBot\] Warning :: No \"disable\" entries defined, or \"disable(DEFAULT)\" is not set."
			set disable(DEFAULT) 0
		}

		# Hook up variables and announce definitions for the message replacing code
		# New message should have identical variables definition as old message
		# New announce set to old announce if not found in theme file (and output a
		# warning message).
		foreach rep [array names msgreplace] {
			set rep [split $msgreplace($rep) ":"]
			set variables([lindex $rep 2]) $variables([lindex $rep 0])
			if {![info exists disable([lindex $rep 2])]} {
				set disable([lindex $rep 2]) 0
			}
			if {![info exists announce([lindex $rep 2])]} {
				set announce([lindex $rep 2]) $announce([lindex $rep 0])
				putlog "\[ngBot\] Warning :: Custom message [lindex $rep 2] defined, but no announce definition found. Using same announce as [lindex $rep 0] for now. Please edit the theme file!"
			}
		}
	}

	proc init_timer {} {
		variable ns
		variable ng_timer

		## Start the log timer
		if {[info exists ng_timer] && [catch {killutimer $ng_timer} error]} {
			putlog "\[ngBot\] Warning :: Unable to kill log timer ($error)."
			putlog "\[ngBot\] Warning :: You should .restart the bot to be safe."
		}
		set ng_timer [utimer 1 ${ns}::readlogtimer]
	}

	proc init_plugins {} {
		variable ns

		if {![namespace exists ${ns}::plugin]} { return }

		set loaded [list]
		foreach plugin [namespace children ${ns}::plugin] {
			switch -- [catch {${plugin}::init} error] {
				-1 {
					catch {${plugin}::deinit}
					catch {namespace delete $plugin}
				}
				0 { lappend loaded [namespace tail $plugin] }
				default {
					putlog "\[ngBot\] [namespace tail $plugin] Error :: Unable to load plugin: $error"
					catch {${plugin}::deinit}
					catch {namespace delete $plugin}
				}
			}
		}

		if {[llength $loaded] >= 1} {
			putlog "\[ngBot\] Plugins Loaded: [join $loaded ", "]."
		}
	}

	# Uses _ng to avoid being overwriten by namespace import.
	proc deinit_ng {type} {
		variable ns
		variable ng_timer

		catch {killutimer $ng_timer}

		# Remove all binds bound to any procs that match ::ngBot::*
		foreach bind [binds "${ns}::*"] {
			foreach {type flags command {} proc} $bind { break }
			catch {unbind $type $flags $command $proc}
		}

		# Recursivly run the deinit procs of each child namespace.
		set stack [list [namespace children]]
		while {[llength $stack] > 0} {
			set frame [lindex $stack end]
			# An empty frame signals that index 0 of index end-1 (stack) is ready to
			# be popped.
			if {[llength $frame] == 0} {
				set stack [lreplace $stack end end]
				# If there is nothing left on the stack, dont push an empty frame on.
				if {[lindex $stack 0] != ""} {
					set stack [lreplace $stack end end [lreplace [lindex $stack end] 0 0]]
				}
				continue
			}
			set item [lindex $frame 0]
			# Run the deinit proc before looking for any children within the current
			# namespace. Allow the current namespaces deinit to clean them up if it
			# wants to.
			catch {${item}::deinit}
			set children [expr { [namespace exists $item] ? [namespace children $item] : [list] }]
			if {[llength $children] > 0} {
				lappend stack $children
			} else {
				set stack [lreplace $stack end end [lreplace $frame 0 0]]
			}
		}

		namespace delete $ns
	}

	#############################################################################
	#                                                                           #
	#############################################################################

	proc debug {text} {
		variable debugmode
		if {[istrue $debugmode]} {
			putlog "\[ngBot\] Debug :: $text"
		}
	}

	#################################################################################
	# Log Parsing for glFTPd and Login Logs                                         #
	#################################################################################

	proc denycheck {release} {
		variable ns
		variable denypost
		foreach deny $denypost {
			if {[string match $deny $release]} {
				${ns}::debug "Announce skipped, \"$release\" matched \"$deny\" (denypost)."
				return 1
			}
		}
		return 0
	}

	proc eventcheck {section event} {
		variable ns
		variable disabletypes
		if {![info exists disabletypes($section)]} {return 0}
		foreach deny $disabletypes($section) {
			if {[string match $deny $event]} {
				${ns}::debug "Announce skipped, \"$event\" is disabled in \"disabletypes($section)\"."
				return 1
			}
		}
		return 0
	}

	proc eventhandler {type event argv} {
		variable ns
		variable $type
		set varname "$type\($event)"
		if {![info exists $varname]} {return 1}
		foreach script [set $varname] {
			if {[catch {set retval [eval $script $event $argv]} error]} {
				putlog "\[ngBot\] Error :: Error evaluating the script \"$script\" for \"$varname\" ($error)."
			} elseif {[isfalse $retval]} {
				${ns}::debug "The script \"$script\" for \"$varname\" returned false."
				return 0
			} elseif {![istrue $retval]} {
				putlog "\[ngBot\] Warning :: The script \"$script\" for \"$varname\" must return a boolean value (0/FALSE or 1/TRUE)."
			}
		}
		return 1
	}

	proc readlogtimer {} {
		variable ns
		variable ng_timer
		global errorInfo
		if {[catch {${ns}::readlog}]} {
			putlog "\[ngBot\] Error :: Unhandled error, please report to developers:"
			${ns}::cmd_error
		}
		set ng_timer [utimer 1 ${ns}::readlogtimer]
	}

	proc readlog {} {
		variable ns
		variable disable
		variable loglist
		variable lastread
		variable msgtypes
		variable variables
		variable msgreplace
		variable defaultsection
		variable max_log_change

		set lines ""

		foreach {logtype logid logpath} $loglist {
			if {![file readable $logpath]} {
				putlog "\[ngBot\] Error :: Unable to read log file \"$logpath\"."
				continue
			}
			## The regex pattern to use for the logfile
			## - means "use next regex"
			switch -exact -- $logtype {
				0 {set regex {^.+ \d+:\d+:\d+ \d{4} (\S+): (.+)}}
				1 -
				2 {set regex {^.+ \d+:\d+:\d+ \d{4} \[(\d+)\s*\] (.+)}}
				default {putlog "\[ngBot\] Error :: Internal error, unknown log type ($logtype)."; continue}
			}
			## Read the log data
			set logsize [file size $logpath]
			if {$lastread($logid) < $logsize && ($logsize - $lastread($logid) - $max_log_change) < 0} {
				if {![catch {set handle [open $logpath r]} error]} {
					seek $handle $lastread($logid)
					set data [read -nonewline $handle]
					set logsize [tell $handle]
					close $handle

					foreach line [split $data "\n"] {
						## Remove the date and time from the log line.
						if {[regexp $regex $line result event line]} {
							lappend lines $logtype $event $line
						} else {
							putlog "\[ngBot\] Warning :: Invalid log line: $line"
						}
					}
				} else {
					putlog "\[ngBot\] Error :: Unable to open log file \"$logpath\" ($error)."
				}
			}
			set lastread($logid) $logsize
		}

		foreach {type event line} $lines {
			## Login and sysop log specific parsing.
			if {$type == 1 && ![${ns}::parselogin $line event line]} {
				putlog "\[ngBot\] Error :: Unknown login.log line: $line"
				continue
			} elseif {$type == 2 && ![${ns}::parsesysop $line event line]} {
				set event "SYSOP"
				set line [list $line]
			}
			${ns}::debug "Received event: $event (log: $type)."

			## Check that the log line is a valid Tcl list.
			if {[catch {llength $line} error]} {
				putlog "\[ngBot\] Error :: Invalid log line (not a tcl list): $line"
				continue
			}
			## Invite users to public and private channels.
			if {[string equal $event "INVITE"]} {
				foreach {nick user group flags} $line {break}
				${ns}::inviteuser $nick $user $group $flags
			}
			if {[lsearch -exact $msgtypes(SECTION) $event] != -1} {
				set path [lindex $line 0]
				if {[${ns}::denycheck $path]} {continue}
				set section [${ns}::getsectionname $path]

				# Replace messages with custom messages
				foreach name [lsort -integer [array names msgreplace]] {
					set value [split $msgreplace($name) ":"]
					if {[string equal $event [lindex $value 0]]} {
						if {[string match -nocase [lindex $value 1] $path]} {
							set newevent [lindex $value 2]
						}
					}
				}
				if {[info exists newevent]} { set event $newevent; unset newevent }
			} elseif {[lsearch -exact $msgtypes(DEFAULT) $event] != -1} {
				set section $defaultsection
			} else {
				putlog "\[ngBot\] Error :: Undefined message type \"$event\", check \"msgtypes(SECTION)\" and \"msgtypes(DEFAULT)\" in the config."
				continue
			}

			## If the event is disabled, skip the announce.
			if {([info exists disable($event)] && $disable($event) == 1)} {
				${ns}::debug "Announce skipped, \"$event\" is disabled."
				continue
			}

			## If a pre-event script returns false, skip the announce.
			if {[${ns}::eventhandler precommand $event [list $section $line]]} {
				if {![info exists variables($event)]} {
					putlog "\[ngBot\] Error :: \"variables($event)\" not defined in the config, type becomes \"$defaultsection\"."
					set event $defaultsection
				}

				if {![${ns}::eventcheck $section $event]} {
					${ns}::sndall $event $section [${ns}::ng_format $event $section $line]
				}
			} else {
				${ns}::debug "Default announce skipped, \"$event\" pre-command script returned false."
			}

			if {![${ns}::eventcheck $section $event]} {
				${ns}::eventhandler postcommand $event [list $section $line]
			}
		}
	}

	proc parselogin {line eventvar datavar} {
		upvar $eventvar event $datavar data
		## The data in login.log is not at all consistent,
		## which makes it fun for us to parse.
		## The BADUSERNAME's $user can even hold ANY char as it's completely usercrafted,
		## luckily with a 23char limit
		if {[regexp {^'(.+)' killed a ghost with PID (\d+)\.$} $line result user pid]} {
			set event "KILLGHOST"
			set data [list $user $pid]
		} elseif {[regexp {^(.+@.+) \((.+)\): connection refused: .+$} $line result hostmask ip]} {
			set event "IPNOTADDED"
			set data [list $hostmask $ip]
		} elseif {[regexp {^(.+): (.+@.+) \(([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+|disabled)\): (.+)} $line result user hostmask ip error]} {
			switch -exact -- $error {
				"Bad user@host."    {set event "BADHOSTMASK"}
				"Banned user@host." {set event "BANNEDHOST"}
				"Deleted."          {set event "DELETED"}
				"Login failure. (wrong password)"    {set event "BADPASSWORD"}
				"Login failure. (user does not exist)"    {set event "BADUSERNAME"}
				"Login failure. (incorrect email address)"    {set event "BADEMAILPASS"}
				default {return 0}
			}
			set data [list $user $hostmask $ip]
		} elseif {[regexp {^(\S+): user expired.$} $line result user]} {
			set event "EXPIRED"
			set data [list $user]
		} elseif {![regexp {^(\S+): (.+)$} $line result event data]} {
			return 0
		}
		return 1
	}

	proc parsesysop {line eventvar datavar} {
		upvar $eventvar event $datavar newdata
		set patterns [list \
			ADDUSER  {^'(\S+)' added user '(\S+)'\.$} \
			GADDUSER {^'(\S+)' added user '(\S+)' to group '(\S+)'\.$} \
			CHGRPADD {^'(\S+)': successfully added to '(\S+)' by (\S+)$} \
			CHGRPDEL {^'(\S+)': successfully removed from '(\S+)' by (\S+)$} \
			ADDIP    {^'(\S+)' added ip '(\S+)' to '(\S+)'$} \
			DELIP    {^'(\S+)' .*removed ip '(\S+)' from '(\S+)'$} \
			READDED  {^'(\S+)' readded '(\S+)'\.$} \
			DELUSER  {^'(\S+)' deleted user '(\S+)'\.$} \
			PURGED   {^'(\S+)' purged '(\S+)'$} \
			KILLED   {^'(\S+)' killed PID (\d+) \((\S+)\)\.$} \
		]
		foreach {event pattern} $patterns {
			if {[llength [set data [regexp -inline -- $pattern $line]]]} {
				set newdata [lrange $data 1 end]
				return 1
			}
		}
		return 0
	}

	proc ng_random {event rndvar} {
		variable random

		upvar $rndvar rndevent

		## Select a random announce theme
		set eventlist [array names random "${event}-*"]
		if {[set items [llength $eventlist]]} {
			set rndevent [lindex $eventlist [rand $items]]
			return 1
		}
		return 0
	}

	proc ng_format {event section line} {
		variable ns
		variable theme
		variable mpath
		variable random
		variable disable
		variable announce
		variable sitename
		variable variables
		variable theme_fakes
		variable defaultsection

		if {![info exists announce($event)]} {
			putlog "\[ngBot\] Error :: \"announce($event)\" not set in theme, event becomes \"$defaultsection\"."
			set event $defaultsection
		}
		set vars $variables($event)
		set output $theme(PREFIX)

		## Random announce messages
		if {[string equal "random" $announce($event)] && [${ns}::ng_random $event rndevent]} {
			append output $random($rndevent)
		} else {
			append output $announce($event)
		}

		if {[string equal $section $defaultsection] && [info exists theme_fakes($event)]} {
			set section $theme_fakes($event)
		}
		set output [${ns}::replacebasic $output $section]
		set cnt 0

		## Path filter parsing
		if {[string equal "%pf" [lindex $vars 0]]} {
			set output [${ns}::replacepath $output $mpath [lindex $line 0]]
			set vars [lreplace $vars 0 0]
			incr cnt
		}

		## Replace variables and format the %loop cookies
		set loop 1
		foreach vari $vars {
			if {[llength $vari] > 1} {
				set cnt2 0
				set cnt3 1
				set values [lindex $line $cnt]
				set output2 ""
				foreach value $values {
					if {$cnt2 == 0} {
						append output2 "$announce(${event}_LOOP${loop})"
					}
					if {[string match "*speed" [lindex $vari $cnt2]]} {
						set output2 [${ns}::replacevar $output2 [lindex $vari $cnt2] [${ns}::format_speed $value $section]]
					} else {
						set output2 [${ns}::replacevar $output2 [lindex $vari $cnt2] $value]
					}
					incr cnt2
					if {[string equal "" [lindex $vari $cnt2]]} {
						incr cnt3
						set cnt2 0
					}
				}
				set output2 [${ns}::replacevar $output2 "%section" $section]
				set output2 [${ns}::replacevar $output2 "%sitename" $sitename]
				set output2 [${ns}::replacevar $output2 "%splitter" $theme(SPLITTER)]
				set output2 [${ns}::trimtail $output2 $theme(SPLITTER)]
				set output [${ns}::replacevar $output "%loop$loop" $output2]
				incr loop
			} else {
				if {[string match "*speed" $vari]} {
					set output [${ns}::replacevar $output $vari [${ns}::format_speed [lindex $line $cnt] $section]]
				} else {
					set output [${ns}::replacevar $output $vari [lindex $line $cnt]]
				}
			}
			incr cnt
		}
		return $output
	}

	#################################################################################
	# Format Size, Speed and Time Units                                             #
	#################################################################################

	proc format_duration {secs} {
		set duration ""
		foreach div {31536000 604800 86400 3600 60 1} unit {y w d h m s} {
			set num [expr {$secs / $div}]
			if {$num > 0} {lappend duration "\002$num\002$unit"}
			set secs [expr {$secs % $div}]
		}
		if {[llength $duration]} {return [join $duration]} else {return "\0020\002s"}
	}

	proc format_kb {amount} {
		variable device_size

		set round 1
		if {[info exists device_size] && [regexp -nocase -- {^[kmgtpe]b$} $device_size]} {
			set round 0
		}

		foreach dec {0 1 2 2 2 2} unit {KB MB GB TB PB EB} {
			if {![istrue $round] && [string equal -nocase $device_size $unit]} {
				break
			}
			if {![istrue $round] || abs($amount) >= 1024} {
				set amount [expr {double($amount) / 1024.0}]
			} elseif {[istrue $round]} {break}
		}
		return [format "%.*f%s" $dec $amount $unit]
	}

	proc format_speed {value section} {
		variable ns
		variable theme
		variable speedmeasure
		variable speedthreshold

		if {![string is double $value]} {
			return $value
		}

		switch -exact -- [string tolower $speedmeasure] {
			"mb" {
				set value [format "%.2f" [expr {$value / 1024.0}]]
				set type $theme(MB)
			}
			"kbit" {
				set value [expr {$value * 8}]
				set type $theme(KBIT)
			}
			"mbit" {
				set value [format "%.1f" [expr {$value * 8 / 1000.0}]]
				set type $theme(MBIT)
			}
			"autobit" {
				if {$value > $speedthreshold} {
					set value [format "%.1f" [expr {$value * 8 / 1000.0}]]
					set type $theme(MBIT)
				} else {
					set value [expr {$value * 8}]
					set type $theme(KBIT)
				}
			}
			"autobyte" {
				if {$value > $speedthreshold} {
					set value [format "%.2f" [expr {$value / 1024.0}]]
					set type $theme(MB)
				} else {
					set type $theme(KB)
				}
			}
			"disabled" {
				return $value
			}
			default {
				set type $theme(KB)
			}
		}

		return [${ns}::themereplace [${ns}::replacevar $type "%value" $value] $section]
	}

	proc format_clock {type ctime} {
		variable format_time
		variable format_date

		set str ""
		switch -- [string tolower $type] {
			"time" { set str [expr { [info exists format_time] ? $format_time : "%H:%M:%S" }] }
			"date" { set str [expr { [info exists format_date] ? $format_date : "%m-%d-%y" }] }
		}

		return [clock format $ctime -format $str]
	}

	#################################################################################
	# Replace Text                                                                  #
	#################################################################################

	proc replacebasic {message section} {
		variable cmdpre
		variable sitename
		set pre $cmdpre
		if {[llength $cmdpre] > 1} { set pre "<[join $cmdpre |]>" }
		return [string map [list "%cmdpre" $pre "%section" $section "%sitename" $sitename "%bold" \002 "%uline" \037 "%color" \003] $message]
	}

	proc replacepath {message basepath path} {
		variable ns

		set npath [split $path "/"]
		set pathitems [llength $npath]

		set x 0
		set bpaths [split $basepath " "]
		set blength [llength $bpaths]
		while {$x < $blength && ![string match [lindex $bpaths $x] $path]} {
			incr x
		}

		if {$x < $blength} {
			set bpath [lindex $bpaths $x]
			set bnpath [split $bpath "/"]
			set baseitems [llength $bnpath]
			set relname [join [lrange $npath [expr {$baseitems - 1}] end] "/"]

			set message [${ns}::replacevar $message "%relname" $relname]
			set message [${ns}::replacevar $message "%reldir" [lindex $npath [expr {$pathitems - 1}]]]
			set message [${ns}::replacevar $message "%path" [lindex $npath [expr {$pathitems - 2}]]]
		}
		return $message
	}

	proc replacevar {string cookie value} {
		variable zeroconvert
		if {[string length $value] == 0 && [info exists zeroconvert($cookie)]} {
			set value $zeroconvert($cookie)
		}
		return [string map [list $cookie $value] $string]
	}

	proc trimtail {strsrc strrm} {
		if { [expr [string length $strsrc] - [string length $strrm]] == [string last $strrm $strsrc] } {
			return [string range $strsrc 0 [expr [string length $strsrc] - [string length $strrm] - 1]]
		}
		return $strsrc
	}

	#################################################################################
	# Send Messages                                                                 #
	#################################################################################

	proc sndall {event section text} {
		variable ns
		variable chanlist
		variable redirect

		if {[info exists redirect($event)]} {
			set channels $redirect($event)
			${ns}::debug "Redirecting the \"$event\" event to \"$channels\"."
		} elseif {[info exists chanlist($section)]} {
			set channels $chanlist($section)
		} else {
			putlog "\[ngBot\] Error :: \"chanlist($section)\" not defined in the config."
			return
		}
		foreach chan $channels {
			${ns}::sndone $chan $text $section
		}
	}

	proc sndone {chan text {section "none"}} {
		variable ns
		variable splitter
		variable disable
		if {![info exists disable($section)] || $disable($section) != 1} {
			foreach line [split $text $splitter(CHAR)] {
				putquick "PRIVMSG $chan :[${ns}::themereplace $line $section]"
			}
		}
	}

	#################################################################################
	# Invite User                                                                   #
	#################################################################################

	proc inviteuser {args} {
		putlog "\[ngBot\] Error :: Current ftpd not supported for site chan inviting."
	}

	################################################################################
	# Command Utilities                                                            #
	################################################################################

	proc checkchan {nick chan} {
		variable disable
		variable mainchan

		if {$disable(TRIGINALLCHAN) == 1 && [lsearch [string tolower $mainchan] [string tolower $chan]] == -1} {
			if {[info exists ::mclastbind]} {set ::lastbind $::mclastbind}
			putlog "\[ngBot\] Warning :: \002$nick\002 tried to use \002$::lastbind\002 from an invalid channel ($chan)."
			return -code return
		}
	}

	proc announcetochancheck {bind default} {
		variable bindnopre
		variable cmdpre
		variable announcetochan

		if {[istrue $bindnopre]} {
			set trimupperbind [string toupper [string range $bind 1 end]]
			if {[info exists announcetochan($trimupperbind)]} { return $announcetochan($trimupperbind) }
		}
		set trimupperbind [string toupper [string range $bind [string length $cmdpre] end]]
		if {[info exists announcetochan($trimupperbind)]} { return $announcetochan($trimupperbind) }

		return $default
	}

	proc getoptions {argv p_results p_other} {
		variable default_results
		variable maximum_results

		upvar $p_results results $p_other other

		set results $default_results
		set other ""

		## Arguments should be "split" to form a valid list, this avoids any issues when performing
		## list operations (lindex,lrange,llength etc.) on "reserved" characters (i.e. curly braces).
		set argv [split $argv]
		set argc [llength $argv]

		for {set i 0} {$i < $argc} {incr i} {
			set arg [lindex $argv $i]
			if {[string equal -nocase "-h" $arg] || [string equal -nocase "-help" $arg]} {
				return 0
			} elseif {[string equal -nocase "-m" $arg] || [string equal -nocase "-max" $arg]} {
				set arg [lindex $argv [incr i]]
				if {![string is digit -strict $arg]} {return 0}
				set results [expr {$arg < $maximum_results ? $arg : $maximum_results}]
			} elseif {![string equal -length 1 "-" $arg] || $i == ($argc - 1)} {
				## The lrange result must be joined to reverse the effect of the "split".
				set other [join [lrange $argv $i end]]
				return 1
			} else {
				return 0
			}
		}
		return 1
	}

	proc getsectionname {checkpath} {
		variable mpath
		variable paths
		variable sections
		variable defaultsection

		set bestmatch 0
		set returnval $defaultsection

		foreach section $sections {
			if {![info exists paths($section)]} {
				putlog "\[ngBot\] Error :: \"paths($section)\" is not defined in the config."
				continue
			}

			foreach path [split $paths($section)] {
				## Compare the path length of the previous match (best match wins)
				if {[string match $path $checkpath] && [set pathlen [string length $path]] > $bestmatch} {
					set mpath $path
					set bestmatch $pathlen
					set returnval $section
				}
			}
		}
		return $returnval
	}

	proc getsectionpath {getsection} {
		variable paths
		variable sections
		foreach section $sections {
			if {![info exists paths($section)]} {
				putlog "\[ngBot\] Error :: \"paths($section)\" not set in config."
			} elseif {[string equal -nocase $getsection $section]} {
				return [list $section $paths($section)]
			}
		}
		return ""
	}

	#################################################################################
	# Internal Commands                                                             #
	#################################################################################

	proc cmd_error {args} {
		global errorInfo tcl_patchLevel tcl_platform
		putlog "--\[\002Error Info\002\]------------------------------------------"
		putlog "Tcl: $tcl_patchLevel"
		putlog "Box: $tcl_platform(os) $tcl_platform(osVersion)"
		putlog "Message:"
		foreach line [split $errorInfo "\n"] {putlog $line}
		putlog "--------------------------------------------------------"
	}

	# STATUS: OK
	proc cmd_preview {handle idx text} {
		variable ns
		variable announce
		variable defaultsection

		if {[string equal "" $text]} {
			if {[info exists ::mclastbind]} {set ::lastbind $::mclastbind}
			putdcc $idx "\002Preview Usage:\002"
			putdcc $idx "- .$::lastbind <event pattern>"
			putdcc $idx "- Only events matching the pattern are shown (* for all)."
			return
		}

		if {[catch {set handle [open $announce(THEMEFILE) r]} error]} {
			putlog "\[ngBot\] Error :: Unable to read the theme file ($error)."
			return
		}
		set data [read -nonewline $handle]
		close $handle

		putdcc $idx "\002Previewing:\002 $announce(THEMEFILE)"
		putdcc $idx ""
		foreach line [split $data "\n"] {
			if {![string equal "" $line] && [string index $line 0] != "#"} {
				if {[regexp -nocase -- {announce\.(\S+)\s*=\s*(['\"])(.+)\2} $line dud setting quote value]} {
					set prefix "announce."
				} elseif {[regexp -nocase -- {random\.(\S+)\s*=\s*(['\"])(.+)\2} $line dud setting quote value]} {
					set prefix "random."
				} elseif {[regexp -- {(\S+)\s*=\s*(['\"])(.*)\2} $line dud setting quote value]} {
					set prefix ""
				} else {
					continue
				}
				if {[string match -nocase $text $setting]} {
					set value [${ns}::themereplace [${ns}::replacebasic $value $defaultsection] $defaultsection]
					putdcc $idx "$prefix$setting = $value"
				}
			}
		}
		return
	}

	#################################################################################
	# Welcome Message Command                                                       #
	#################################################################################

	proc cmd_welcome {nick uhost hand chan} {
		variable ns
		variable disable
		variable announce
		variable chanlist
		if {$disable(WELCOME) == 1 || [isbotnick $nick]} {return}
		foreach c_chan $chanlist(WELCOME) {
			if {[string match -nocase $c_chan $chan]} {
				set output [${ns}::replacebasic $announce(WELCOME) "WELCOME"]
				set output [${ns}::replacevar $output "%ircnick" $nick]
				set output [${ns}::themereplace $output "none"]
				puthelp "NOTICE $nick :$output"
			}
		}
		return
	}

	################################################################################
	# General Commands                                                             #
	################################################################################

	proc cmd_bnc {nick uhost hand chan arg} {
		variable ns
		variable bnc
		variable theme
		variable binary
		variable announce
		variable announcetochan

		global errorCode

		${ns}::checkchan $nick $chan
		if {![istrue $bnc(ENABLED)]} {return}

		if {[info exists ::mclastbind]} {set ::lastbind $::mclastbind}
		set rcvr $nick
		if {[${ns}::announcetochancheck $::lastbind 0]} {
			set rcvr $chan
		}

		set output "$theme(PREFIX)$announce(BNC)"
		${ns}::sndone $rcvr [${ns}::replacebasic $output "BNC"] "BNC"

		set num 0
		foreach entry $bnc(LIST) {
			set entrysplit [split $entry ":"]
			if {[llength $entrysplit] != 3} {
				putlog "\[ngBot\] Error :: Invalid bouncer line \"$entry\" (check bnc(LIST))."
				continue
			}
			incr num; set ping "N/A"; set min "-"; set avg "-"; set max "-"; set mdev "-"
			foreach {desc ip port} $entrysplit {break}

			if {[istrue $bnc(PING)]} {
				if {[catch {exec $binary(PING) -c $bnc(PINGCOUNT) -t $bnc(TIMEOUT) $ip} reply]} {
					set output "$theme(PREFIX)$announce(BNC_PING)"
					set output [${ns}::replacevar $output "%num" $num]
					set output [${ns}::replacevar $output "%desc" $desc]
					set output [${ns}::replacevar $output "%ip" $ip]
					set output [${ns}::replacevar $output "%port" $port]
					${ns}::sndone $rcvr [${ns}::replacebasic $output "BNC"] "BNC_PING"
					continue
				}
				set firstreply [lindex [split $reply "\n"] 1]
				if {[regexp {.+time=(\S+) ms} $firstreply -> ping]} {
					set ping [format "%.1fms" $ping]
				} else {
					putlog "\[ngBot\] Error :: Unable to parse ping reply \"$firstreply\", please report to pzs-ng developers."
				}
				set reply [lindex [split $reply "\n"] end]
				# Match iputils and GNU inetutils/BSD version
				if {[regexp {rtt min/avg/max/mdev = ([^/]+)/([^/]+)/([^/]+)/(\S+) ms} $reply -> min avg max mdev] ||
					[regexp {round-trip min/avg/max/stddev = ([^/]+)/([^/]+)/([^/]+)/(\S+) ms} $reply -> min avg max mdev]} {
					set min [format "%.1fms" $min]
					set avg [format "%.1fms" $avg]
					set max [format "%.1fms" $max]
					set mdev [format "%.1fms" $mdev]
				} else {
					putlog "\[ngBot\] Error :: Unable to parse ping reply \"$reply\", please report to pzs-ng developers."
				}
			}

			set response [clock clicks -milliseconds]
			if {[istrue $bnc(SECURE)]} {
				set status [catch {exec $binary(CURL) --no-ftp-skip-pasv-ip --disable-epsv --max-time $bnc(TIMEOUT) --ftp-ssl --insecure -u $bnc(USER):$bnc(PASS) ftp://$ip:$port 2>@stdout} reply]
			} else {
				set status [catch {exec $binary(CURL) --no-ftp-skip-pasv-ip --disable-epsv --max-time $bnc(TIMEOUT) -u $bnc(USER):$bnc(PASS) ftp://$ip:$port 2>@stdout} reply]
			}
			set response [expr {[clock clicks -milliseconds] - $response}]
			set type "ONLINE"

			if {!$status} {
				set output "$theme(PREFIX)$announce(BNC_ONLINE)"
				set output [${ns}::replacevar $output "%ping" $ping]
				set output [${ns}::replacevar $output "%min" $min]
				set output [${ns}::replacevar $output "%avg" $avg]
				set output [${ns}::replacevar $output "%max" $max]
				set output [${ns}::replacevar $output "%mdev" $mdev]
				set output [${ns}::replacevar $output "%response" $response]
			} else {
				set error "unknown error"
				set type "OFFLINE"
				## Check curl's exit code (stored in errorCode).
				if {[string equal "CHILDSTATUS" [lindex $errorCode 0]]} {
					set code [lindex $errorCode 2]
					switch -exact -- $code {
						6 {set error "couldn't resolve host"}
						7 {set error "connection refused"}
						9 - 10 {set error "couldn't login"}
						28 {set error "timed out"}
						60 {set error "SSL certificate problem. Or set insecure option in ngBot.conf"}
						default {putlog "\[ngBot\] Error :: Unknown curl exit code \"$code\", check the \"exit codes\" section of curl's man page."}
					}
				} else {
					## If the first list item in errorCode is not "CHILDSTATUS",
					## Tcl was unable to execute the binary.
					putlog "\[ngBot\] Error :: Unable to execute curl ($reply)."
				}
				set output "$theme(PREFIX)$announce(BNC_OFFLINE)"
				set output [${ns}::replacevar $output "%error" $error]
			}

			set output [${ns}::replacevar $output "%num" $num]
			set output [${ns}::replacevar $output "%desc" $desc]
			set output [${ns}::replacevar $output "%ip" $ip]
			set output [${ns}::replacevar $output "%port" $port]
			${ns}::sndone $rcvr [${ns}::replacebasic $output "BNC"] "BNC_$type"
		}
		return
	}

	proc cmd_help {nick uhost hand chan arg} {
		variable ns
		variable sections
		variable scriptpath
		variable statsection
		variable announcetochan

		${ns}::checkchan $nick $chan

		if {[info exists ::mclastbind]} {set ::lastbind $::mclastbind}
		set rcvr $nick
		if {[${ns}::announcetochancheck $::lastbind 0]} {
			set rcvr $chan
		}

		if {[catch {set handle [open "$scriptpath/ngBot.help" r]} error]} {
			putlog "\[ngBot\] Error :: Unable to read the help file ($error)."
			puthelp "PRIVMSG $rcvr :Unable to read the help file, please contact a siteop."
			return
		}
		set data [read -nonewline $handle]
		close $handle

		set statlist ""
		foreach {name value} [array get statsection] {
			lappend statlist $value
		}
		set sectlist [join [lsort -ascii $sections] {, }]
		set statlist [join [lsort -ascii $statlist] {, }]

		foreach line [split $data "\n"] {
			set line [${ns}::replacevar $line "%sections" $sectlist]
			set line [${ns}::replacevar $line "%statsections" $statlist]
			puthelp "PRIVMSG $rcvr :[${ns}::themereplace [${ns}::replacebasic $line "HELP"] "none"]"
		}
		return
	}

	proc cmd_free {nick uhost hand chan arg} {
		variable ns
		variable theme
		variable device
		variable binary
		variable announce
		variable dev_max_length
		variable local_devices_only
		variable announcetochan

		${ns}::checkchan $nick $chan

		array set tmpdev [array get device]
		set devices(0) ""
		set devCount 0; set lineCount 0
		set totalFree 0; set totalUsed 0; set totalSize 0

			set flags "Pk"
			if {[istrue $local_devices_only]} { set flags "Pkl" }
		if {[catch {exec $binary(DF) -$flags} output] != 0} {
			putlog "\[ngBot\] Error :: \"df -$flags\" failed:"
			foreach line [split $output "\n"] {
				putlog "\[ngBot\] Error ::   $line"
			}
			return
		}

		foreach line [split $output "\n"] {
			foreach {name value} [array get tmpdev] {
				if {([string equal [lindex $line 0] [lindex $value 0]]) || ([string equal [lindex $line 5] [lindex $value 0]])} {
					if {[scan $line "%s %lu %lu %lu %s %s" devName devSize devUsed devFree devPercent devMount] != 6} {
						putlog "\[ngBot\] Warning :: Invalid \"df -$flags\" line: $line"
						continue
					}
					set devPercFree [format "%.1f" [expr {(double($devFree) / double($devSize)) * 100}]]
					set devPercUsed [format "%.1f" [expr {(double($devUsed) / double($devSize)) * 100}]]

					set output $announce(FREE-DEV)
					set output [${ns}::replacevar $output "%free" [${ns}::format_kb $devFree]]
					set output [${ns}::replacevar $output "%used" [${ns}::format_kb $devUsed]]
					set output [${ns}::replacevar $output "%total" [${ns}::format_kb $devSize]]
					set output [${ns}::replacevar $output "%perc_free" $devPercFree]
					set output [${ns}::replacevar $output "%perc_used" $devPercUsed]
					set output [${ns}::replacevar $output "%section" [lrange $value 1 end]]

					if {$dev_max_length > 0 && ([string length $devices($lineCount)] + [string length $output]) > $dev_max_length} {
						incr lineCount
						set devices($lineCount) ""
					}
					append devices($lineCount) $output

					set totalFree [expr {double($totalFree) + double($devFree)}]
					set totalUsed [expr {double($totalUsed) + double($devUsed)}]
					set totalSize [expr {double($totalSize) + double($devSize)}]

					incr devCount
					unset tmpdev($name)

					## End the device match loop (not the df output loop)
					break
				}
			}
		}
		if {[llength [array names tmpdev]]} {
			set devList ""
			foreach {name value} [array get tmpdev] {lappend devList $value}
			putlog "\[ngBot\] Warning :: The following devices had no matching \"df -$flags\" entry: [join $devList {, }]"
		}

		if {$totalSize} {
			set percFree [format "%.1f" [expr {(double($totalFree) / double($totalSize)) * 100}]]
			set percUsed [format "%.1f" [expr {(double($totalUsed) / double($totalSize)) * 100}]]
		} else {
			set percFree 0.0; set percUsed 0.0
		}
		set totalFree [${ns}::format_kb $totalFree]
		set totalUsed [${ns}::format_kb $totalUsed]
		set totalSize [${ns}::format_kb $totalSize]

		set index 0

		if {[info exists ::mclastbind]} {set ::lastbind $::mclastbind}
		set rcvr $chan
		if {![${ns}::announcetochancheck $::lastbind 1]} {
			set rcvr $nick
		}

		while {$index < $lineCount + 1} {
			set output "$theme(PREFIX)$announce(FREE)"
			set output [${ns}::replacevar $output "%free" $totalFree]
			set output [${ns}::replacevar $output "%used" $totalUsed]
			set output [${ns}::replacevar $output "%total" $totalSize]
			set output [${ns}::replacevar $output "%perc_free" $percFree]
			set output [${ns}::replacevar $output "%perc_used" $percUsed]
			set output [${ns}::replacevar $output "%devices" $devices($index)]
			${ns}::sndone $rcvr [${ns}::replacebasic $output "FREE"] "FREE"
			incr index
		}
		return
	}

	proc cmd_incompletes {nick uhost hand chan arg} {
		variable ns
		variable binary
		variable announcetochan

		${ns}::checkchan $nick $chan

		if {[info exists ::mclastbind]} {set ::lastbind $::mclastbind}
		set rcvr $chan
		if {![${ns}::announcetochancheck $::lastbind 1]} {
			set rcvr $nick
		}

		#puthelp "PRIVMSG $chan :Processing incomplete list for $nick."
		foreach line [split [exec $binary(INCOMPLETE)] "\n"] {
			if {![info exists newline($line)]} {
				set newline($line) 0
			} else {
				incr newline($line)
			}
			puthelp "PRIVMSG $rcvr :$line\003$newline($line)"
		}
		return
	}

	proc cmd_uptime {nick uhost hand chan argv} {
		variable ns
		variable theme
		variable binary
		variable announce
		variable announcetochan

		global uptime

		${ns}::checkchan $nick $chan

		if {[catch {exec $binary(UPTIME)} reply]} {
			putlog "\[ngBot\] Error :: Unable to execute uptime ($reply)."
		}
		## The linux 'uptime' pads the output with spaces, so we'll trim it.
		regsub -all {\s+} $reply { } reply
		if {![regexp {.+ up (.+), (.+) users?, load averages?: (.+)} $reply reply time users load]} {
			set load "N/A"; set time "N/A"; set users "N/A"
			putlog "\[ngBot\] Error :: Unable to parse uptime reply \"$reply\", please report to pzs-ng developers."
		}
		set eggup [${ns}::format_duration [expr {[clock seconds] - $uptime}]]

		set output "$theme(PREFIX)$announce(UPTIME)"
		set output [${ns}::replacevar $output "%eggdrop" $eggup]
		set output [${ns}::replacevar $output "%time" $time]
		set output [${ns}::replacevar $output "%users" $users]
		set output [${ns}::replacevar $output "%load" $load]
		if {[info exists ::mclastbind]} {set ::lastbind $::mclastbind}
		set rcvr $chan
		if {![${ns}::announcetochancheck $::lastbind 1]} {
			set rcvr $nick
		}
		${ns}::sndone $rcvr [${ns}::replacebasic $output "UPTIME"] "UPTIME"
		return
	}

	#################################################################################
	# Theme Loading and Replacement                                                 #
	#################################################################################

	proc loadtheme {file {isplugin false}} {
		variable ns
		variable theme
		variable random
		variable announce
		variable variables
		variable scriptpath
		variable theme_fakes

		if {![istrue $isplugin]} {
			if {[string index $file 0] != "/"} {
				set file "$scriptpath/$file"
			}

			set announce(THEMEFILE) [file normalize $file]

			putlog "\[ngBot\] Loading theme \"$file\"."
		}

		if {[catch {set handle [open $file r]} error]} {
			putlog "\[ngBot\] Error :: Unable to read the theme file ($error)."
			return 0
		}
		set data [read -nonewline $handle]
		close $handle

		foreach line [split $data "\n"] {
			if {[string index $line 0] != "#"} {
				if {[regexp -- {(\S+)\.(\S+)\s*=\s*(['\"])(.+)\3} $line dud type setting quote value]} {
					switch -exact -- [string tolower $type] {
						"announce" {
							if {(![info exists announce($setting)]) || (![istrue $isplugin])} {
								set announce($setting) $value
							}
						}
						"fakesection" {
							if {(![info exists theme_fakes($setting)]) || (![istrue $isplugin])} {
								set theme_fakes($setting) $value
							}
						}
						"random" {
							if {(![info exists random($setting)]) || (![istrue $isplugin])} {
								set random($setting) $value
							}
						}
						default {
							putlog "\[ngBot\] Warning :: Invalid theme setting \"$type.$setting\"."
						}
					}
				} elseif {[regexp -- {(\S+)\s*=\s*(['\"])(.*)\2} $line dud setting quote value]} {
					if {(![info exists theme($setting)]) || (![istrue $isplugin])} {
						set theme($setting) $value
					}
				}
			}
		}

		foreach name [array names random] {
			if {![regexp {(.+)-\d+$} $name dud base]} {
				putlog "\[ngBot\] Warning :: Invalid setting \"random.$name\", must be in the format of \"random.EVENT-#\"."
				unset random($name)
			} else {
				set announce($base) "random"
				set random($name) [${ns}::themereplace_startup $random($name)]
			}
		}
		foreach name [array names announce] {set announce($name) [${ns}::themereplace_startup $announce($name)]}
		foreach name [array names theme] {set theme($name) [${ns}::themereplace_startup $theme($name)]}
		foreach name [array names theme_fakes] {set theme_fakes($name) [${ns}::themereplace_startup $theme_fakes($name)]}

		if {![istrue $isplugin]} {
			## Sanity checks
			foreach type {COLOR1 COLOR2 COLOR3 PREFIX KB KBIT MB MBIT} {
				if {[lsearch -exact [array names theme] $type] == -1} {
					putlog "\[ngBot\] Error :: Missing required theme setting \"$type\", failing."
					return 0
				}
			}
			foreach type [concat [array names variables] NUKE UNNUKE NUKEES] {
				if {[lsearch -exact [array names announce] $type] == -1} {
					putlog "\[ngBot\] Warning :: Missing announce setting \"announce.$type\" in the theme file."
				}
			}
		}

		return 1
	}

	proc themereplace {targetString section} {
		variable theme

		## Escape any "$" characters so they aren't interpreted as variables in the final "subst".
		set targetString [string map {$ \\$} $targetString]

		# We need to escape [] for the final "subst" due to how we do casealtering.
		regsub -all {\[} $targetString {\[} targetString
		regsub -all {\]} $targetString {\]} targetString

		# We replace %cX{string}, %b{string} and %u{string} with their coloured, bolded and underlined equivilants ;)
		# We also do the justification and padding that is required for %r / %l / %m to work.
		# And we alter text within %T{}, %U{} or %L{} to Titlecase, UPPERCASE or lowercase.
		# bold and underline replacement should not be needed here...
		while {[regexp {(%c(\d)\{([^\{\}]+)\}|%b\{([^\{\}]+)\}|%u\{([^\{\}]+)\}|%T\{([^\{\}]+)\}|%U\{([^\{\}]+)\}|%L\{([^\{\}]+)\}|%([lrm])(\d{1,3})\{([^\{\}]+)\})} $targetString matchString dud padOp padLength padString]} {
			# Check if any innermost %r/%l/%m are present.
			while {[regexp {%([lrm])(\d{1,3})\{([^\{\}]+)\}} $targetString matchString padOp padLength padString]} {
				set tmpPadString $padString
				regsub -all {\003\d\d} $tmpPadString {} tmpPadString
				set tmpPadString [string map {\002 "" \003 "" \037 ""} $tmpPadString]
				incr padLength [expr [string length $padString] - [string length $tmpPadString]]

				if {[string length $padString] >= $padLength} {
					set paddedString $padString
				} elseif {$padOp == "l"} {
					set paddedString [format "%-*s" $padLength $padString]
				} elseif {$padOp == "r"} {
					set paddedString [format "%*s" $padLength $padString]
				} elseif {$padOp == "m"} {
					set paddedString [format "%*s%s" [expr ($padLength - [string length $padString]) / 2] "" $padString]
					set paddedString [format "%-*s" $padLength $paddedString]
				}

				set targetString [string map [list $matchString $paddedString] $targetString]
			}

			regsub -all {%T\{([^\{\}]+)\}} $targetString {[string totitle "\1"]} targetString
			regsub -all {%U\{([^\{\}]+)\}} $targetString {[string toupper "\1"]} targetString
			regsub -all {%L\{([^\{\}]+)\}} $targetString {[string tolower "\1"]} targetString

			regsub -all {%b\{([^\{\}]+)\}} $targetString {\\002\1\\002} targetString
			regsub -all {%u\{([^\{\}]+)\}} $targetString {\\037\1\\037} targetString
			regsub -all {(%c\d\{[^\{\}]+\})(\d)} $targetString {\1\\002\\002\2} targetString

			set colorString [format "COLOR_%s_1" $section]
			if {[lsearch -exact [array names theme] $colorString] != -1} {
				regsub -all {%c(\d)\{([^\{\}]+)\}} $targetString {\\003$theme([format "COLOR_%s_" $section]\1)\2\\003} targetString
				regsub {\003(\d)(?!\d)} $targetString {\\0030\1} targetString
			} else {
				regsub -all {%c(\d)\{([^\{\}]+)\}} $targetString {\\003$theme(COLOR\1)\2\\003} targetString
				regsub {\003(\d)(?!\d)} $targetString {\\0030\1} targetString
			}
		}
		return [subst $targetString]
	}

	proc themereplace_startup {rstring} {
		# We replace %b{string} and %u{string} with their bolded and underlined equivilants ;)
		while {[regexp {(%b\{([^\{\}]+)\}|%u\{([^\{\}]+)\})} $rstring]} {
			regsub -all {%b\{([^\{\}]+)\}} $rstring {\\002\1\\002} rstring
			regsub -all {%u\{([^\{\}]+)\}} $rstring {\\037\1\\037} rstring
		}
		return [subst -nocommands -novariables $rstring]
	}

	#############################################################################
	# Read The Config Files                                                     #
	#############################################################################

	if {[catch {package require Tcl 8.5} error]} {
		die "\[ngBot\] Error :: You must be using Tcl v8.5, or newer, with ngBot."
	}

	# Load the defaults/conf/vars files.
	if {[catch {source $scriptpath/ngBot.conf.defaults} error]} {
		putlog "\[ngBot\] Error :: Unable to load ngBot.conf.defaults ($error), cannot continue."
		putlog "\[ngBot\] Error :: See FAQ for possible solutions/debugging options."
		die
	}
	if {[catch {source $scriptpath/ngBot.vars} error]} {
		putlog "\[ngBot\] Error :: Unable to load ngBot.vars ($error), cannot continue."
		putlog "\[ngBot\] Error :: See FAQ for possible solutions/debugging options."
		die
	}
	if {[catch {source $scriptpath/ngBot.conf} error]} {
		putlog "\[ngBot\] Warning :: Unable to load ngBot.conf, using default values."
		if {[file exists $scriptpath/ngBot.conf]} {
			putlog "\[ngBot\] Warning :: Config error:\n$errorInfo"
		} else {
			putlog "\[ngBot\] Warning :: Since this is your first install, do: cp ngBot.conf.dist ngBot.conf"
		}
		set dzerror 1
	}

	variable mpath ""
	variable defaultsection "DEFAULT"
	set nuke(LASTDIR) ""
	set nuke(LASTTYPE) ""
	set nuke(SHOWN) 1
	set variables(NUKE) ""
	set variables(UNNUKE) ""

	# Source the ftpd module.
	regsub -all -- {([^a-zA-Z0-9.-_])} $ftpd_type "" ftpd_type
	if {[catch {source $scriptpath/modules/${ftpd_type}.tcl} error]} {
		putlog "\[ngBot\] Error :: Unable to load ftpd module file modules/${ftpd_type}.tcl ($error), cannot continue."
		putlog "\[ngBot\] Error :: See FAQ for possible solutions/debugging options."
		die
	}

	# Check to see if the correct ftpd module is available.
	if {![namespace exists "[namespace current]::module::$ftpd_type"]} {
		putlog "\[ngBot\] Error :: Unable to locate ftpd module namespace \"$ftpd_type\", cannot continue."
		putlog "\[ngBot\] Error :: See FAQ for possible solutions/debugging options."
		die
	}

	# Attempt to load the ftpd module.
	if {[catch {[namespace current]::module::${ftpd_type}::init} error]} {
		putlog "\[ngBot\] Error :: Unable to load ftpd module ($error), cannot continue."
		putlog "\[ngBot\] Error :: See FAQ for possible solutions/debugging options."
		die
	}

	# Run init_ng proc once all scripts are loaded.
	variable ng_is_loaded 0
	bind evnt -|- userfile-loaded ${ns}::init_ng

	bind join -|- * ${ns}::cmd_welcome
	bind dcc n errorinfo ${ns}::cmd_error
	bind dcc n preview ${ns}::cmd_preview

	interp alias {} istrue {} string is true -strict
	interp alias {} isfalse {} string is false -strict
}
