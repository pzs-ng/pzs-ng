#################################################################################
# dZSbot - ProjectZS-NG Sitebot                                                 #
#################################################################################
# - Displays information, real-time events, and stats for your glFTPd site.     #
# - Based on the original dZSbot written by dark0n3.                            #
#################################################################################

#################################################################################
# Read The Config Files                                                         #
#################################################################################

putlog "\[dZSbot\] Loading..."

namespace eval ::dZSbot {
    variable scriptPath [file dirname [info script]]
    namespace export *
    set defaultsection "DEFAULT"
    set variables(NUKE)   ""
    set variables(UNNUKE) ""
}

## This Tcl version check is here for a reason - don't remove it. If you think
## removing this check in order for dZSbot to work on an older Tcl is clever,
## it's not. In fact, you're a moron. dZSbot uses several features which are specific
## to Tcl 8.4, don't even think of complaining about backwards compatibility either.
if {[catch {package require Tcl 8.4} error]} {
    die "\[dZSbot\] Error :: You must be using Tcl v8.4, or newer, with dZSbot."
}

#################################################################################
# Miscellaneous Commands                                                        #
#################################################################################

proc ::dZSbot::DebugMsg {section text} {
    variable debugmode
    if {[IsTrue $debugmode]} {putlog "\[dZSbot\] Debug :: $section - $text"}
}

proc ::dZSbot::InfoMsg {text} {
    putlog "\[dZSbot\] $text"
}

proc ::dZSbot::ErrorMsg {section text} {
    putlog "\[dZSbot\] Error :: $section - $text"
}

proc ::dZSbot::WarningMsg {section text} {
    putlog "\[dZSbot\] Warning :: $section - $text"
}

proc ::dZSbot::b {} {return \002}
proc ::dZSbot::c {} {return \003}
proc ::dZSbot::u {} {return \037}
proc ::dZSbot::r {} {return \026}
proc ::dZSbot::o {} {return \015}

interp alias {} IsTrue {} string is true -strict
interp alias {} IsFalse {} string is false -strict

#################################################################################
# Internal Commands                                                             #
#################################################################################

proc ::dZSbot::BindCommands {prefix} {
    set context [namespace current]
    bind pub -|- ${prefix}bnc         ${context}::CmdBnc
    bind pub -|- ${prefix}bw          ${context}::CmdBw
    bind pub -|- ${prefix}bwdn        ${context}::CmdBwDn
    bind pub -|- ${prefix}bwup        ${context}::CmdBwUp
    bind pub -|- ${prefix}df          ${context}::CmdFree
    bind pub -|- ${prefix}free        ${context}::CmdFree
    bind pub -|- ${prefix}dn          ${context}::CmdLeechers
    bind pub -|- ${prefix}down        ${context}::CmdLeechers
    bind pub -|- ${prefix}downloaders ${context}::CmdLeechers
    bind pub -|- ${prefix}leechers    ${context}::CmdLeechers
    bind pub -|- ${prefix}dupe        ${context}::CmdSearch
    bind pub -|- ${prefix}help        ${context}::CmdHelp
    bind pub -|- ${prefix}idle        ${context}::CmdIdlers
    bind pub -|- ${prefix}idlers      ${context}::CmdIdlers
    bind pub -|- ${prefix}inc         ${context}::CmdIncompletes
    bind pub -|- ${prefix}incomplete  ${context}::CmdIncompletes
    bind pub -|- ${prefix}incompletes ${context}::CmdIncompletes
    bind pub -|- ${prefix}new         ${context}::CmdNew
    bind pub -|- ${prefix}nukes       ${context}::CmdNukes
    bind pub -|- ${prefix}search      ${context}::CmdSearch
    bind pub -|- ${prefix}speed       ${context}::CmdSpeed
    bind pub -|- ${prefix}unnukes     ${context}::CmdUnnukes
    bind pub -|- ${prefix}up          ${context}::CmdUploaders
    bind pub -|- ${prefix}uploaders   ${context}::CmdUploaders
    bind pub -|- ${prefix}uptime      ${context}::CmdUptime
    bind pub -|- ${prefix}who         ${context}::CmdWho

    bind pub -|- ${prefix}gpad    [list ${context}::CmdStats "-d" "-A"]
    bind pub -|- ${prefix}gpal    [list ${context}::CmdStats "-u" "-A"]
    bind pub -|- ${prefix}gpwk    [list ${context}::CmdStats "-d" "-W"]
    bind pub -|- ${prefix}gwpd    [list ${context}::CmdStats "-u" "-W"]

    bind pub -|- ${prefix}alldn   [list ${context}::CmdStats "-d" "-a"]
    bind pub -|- ${prefix}allup   [list ${context}::CmdStats "-u" "-a"]
    bind pub -|- ${prefix}daydn   [list ${context}::CmdStats "-d" "-t"]
    bind pub -|- ${prefix}dayup   [list ${context}::CmdStats "-u" "-t"]
    bind pub -|- ${prefix}monthdn [list ${context}::CmdStats "-d" "-m"]
    bind pub -|- ${prefix}monthup [list ${context}::CmdStats "-u" "-m"]
    bind pub -|- ${prefix}wkdn    [list ${context}::CmdStats "-d" "-w"]
    bind pub -|- ${prefix}wkup    [list ${context}::CmdStats "-u" "-w"]
}

proc ::dZSbot::UnbindCommands {prefix} {
    set context [namespace current]
    catch {unbind pub -|- ${prefix}bnc         ${context}::CmdBnc}
    catch {unbind pub -|- ${prefix}bw          ${context}::CmdBw}
    catch {unbind pub -|- ${prefix}bwdn        ${context}::CmdBwDn}
    catch {unbind pub -|- ${prefix}bwup        ${context}::CmdBwUp}
    catch {unbind pub -|- ${prefix}df          ${context}::CmdFree}
    catch {unbind pub -|- ${prefix}free        ${context}::CmdFree}
    catch {unbind pub -|- ${prefix}dn          ${context}::CmdLeechers}
    catch {unbind pub -|- ${prefix}down        ${context}::CmdLeechers}
    catch {unbind pub -|- ${prefix}downloaders ${context}::CmdLeechers}
    catch {unbind pub -|- ${prefix}leechers    ${context}::CmdLeechers}
    catch {unbind pub -|- ${prefix}dupe        ${context}::CmdSearch}
    catch {unbind pub -|- ${prefix}help        ${context}::CmdHelp}
    catch {unbind pub -|- ${prefix}idle        ${context}::CmdIdlers}
    catch {unbind pub -|- ${prefix}idlers      ${context}::CmdIdlers}
    catch {unbind pub -|- ${prefix}inc         ${context}::CmdIncompletes}
    catch {unbind pub -|- ${prefix}incomplete  ${context}::CmdIncompletes}
    catch {unbind pub -|- ${prefix}incompletes ${context}::CmdIncompletes}
    catch {unbind pub -|- ${prefix}new         ${context}::CmdNew}
    catch {unbind pub -|- ${prefix}nukes       ${context}::CmdNukes}
    catch {unbind pub -|- ${prefix}search      ${context}::CmdSearch}
    catch {unbind pub -|- ${prefix}speed       ${context}::CmdSpeed}
    catch {unbind pub -|- ${prefix}unnukes     ${context}::CmdUnnukes}
    catch {unbind pub -|- ${prefix}up          ${context}::CmdUploaders}
    catch {unbind pub -|- ${prefix}uploaders   ${context}::CmdUploaders}
    catch {unbind pub -|- ${prefix}uptime      ${context}::CmdUptime}
    catch {unbind pub -|- ${prefix}who         ${context}::CmdWho}

    catch {unbind pub -|- ${prefix}gpad    [list ${context}::CmdStats "-d" "-A"]}
    catch {unbind pub -|- ${prefix}gpal    [list ${context}::CmdStats "-u" "-A"]}
    catch {unbind pub -|- ${prefix}gpwk    [list ${context}::CmdStats "-d" "-W"]}
    catch {unbind pub -|- ${prefix}gwpd    [list ${context}::CmdStats "-u" "-W"]}

    catch {unbind pub -|- ${prefix}alldn   [list ${context}::CmdStats "-d" "-a"]}
    catch {unbind pub -|- ${prefix}allup   [list ${context}::CmdStats "-u" "-a"]}
    catch {unbind pub -|- ${prefix}daydn   [list ${context}::CmdStats "-d" "-t"]}
    catch {unbind pub -|- ${prefix}dayup   [list ${context}::CmdStats "-u" "-t"]}
    catch {unbind pub -|- ${prefix}monthdn [list ${context}::CmdStats "-d" "-m"]}
    catch {unbind pub -|- ${prefix}monthup [list ${context}::CmdStats "-u" "-m"]}
    catch {unbind pub -|- ${prefix}wkdn    [list ${context}::CmdStats "-d" "-w"]}
    catch {unbind pub -|- ${prefix}wkup    [list ${context}::CmdStats "-u" "-w"]}
}

proc ::dZSbot::ShowError {args} {
    global errorInfo tcl_platform
    putlog "--\[[b]Error Info[b]\]------------------------------------------"
    putlog "Tcl: [info patchlevel]"
    putlog "Box: $tcl_platform(os) $tcl_platform(osVersion), $tcl_platform(machine) $tcl_platform(byteOrder)"
    putlog "Message:"
    foreach line [split $errorInfo "\n"] {putlog $line}
    putlog "--------------------------------------------------------"
}

#################################################################################
# Event Handling                                                                #
#################################################################################

proc ::dZSbot::EventHandler {type event args} {
    variable $type
    set varName "${type}($event)"
    if {![info exists $varName]} {return 1}

    foreach script [set $varName] {
        if {[catch {set retval [eval [list $script $event] $args]} error]} {
            ErrorMsg EventHandler "Error evaluating the script \"$script\" for \"$varName\" ($error)."
        } elseif {[IsFalse $retval]} {
            DebugMsg EventHandler "The script \"$script\" for \"$varName\" returned false."
            return 0
        } elseif {![IsTrue $retval]} {
            WarningMsg EventHandler "The script \"$script\" for \"$varName\" must return a boolean value (0/FALSE or 1/TRUE)."
        }
    }
    return 1
}

proc ::dZSbot::EventRegister {type event script} {
    variable $type
    set varName "${type}($event)"
    if {![info exists $varName] || [lsearch -exact [set $varName] $script] == -1} {
        lappend $varName $script
    }
    return
}

proc ::dZSbot::EventUnregister {type event script} {
    variable $type
    set varName "${type}($event)"
    if {[info exists $varName] && [set pos [lsearch -exact [set $varName] $script]] != -1} {
        set $varName [lreplace [set $varName] $pos $pos]
    }
    return
}

#################################################################################
# Log Parsing for glFTPd and Login Logs                                         #
#################################################################################

proc ::dZSbot::IsPathDenied {release} {
    variable denypost
    foreach deny $denypost {
        if {[string match $deny $release]} {
            DebugMsg PathDenied "Announce skipped, \"$release\" matched \"$deny\" (denypost)."
            return 1
        }
    }
    return 0
}

proc ::dZSbot::IsEventDenied {section event} {
    variable disabletypes
    if {![info exists disabletypes($section)]} {return 0}

    foreach deny $disabletypes($section) {
        if {[string match $deny $event]} {
            DebugMsg EventDenied "Announce skipped, \"$event\" disabled, check \"disabletypes($section)\"."
            return 1
        }
    }
    return 0
}

proc ::dZSbot::LogTimer {} {
    variable logTimerId
    if {[catch {LogRead}]} {
        ErrorMsg LogTimer "Unhandled error, please report to developers:\n$::errorInfo"
    }
    set logTimerId [utimer 1 [namespace current]::LogTimer]
}

proc ::dZSbot::LogRead {} {
    variable defaultsection
    variable disable
    variable glVersion
    variable lastread
    variable logList
    variable max_log_change
    variable msgreplace
    variable msgtypes
    variable variables

    set lines ""
    foreach {logType logId logPath} $logList {
        if {![file readable $logPath]} {
            ErrorMsg LogRead "Unable to read log file \"$logPath\"."
            continue
        }
        switch -exact -- $logType {
            0 {set regex {^.+ \d+:\d+:\d+ \d{4} (\S+): (.+)}}
            1 - 2 {set regex {^.+ \d+:\d+:\d+ \d{4} \[(.+)\] (.+)}}
            default {ErrorMsg LogRead "Internal error, unknown log type ($logType)."; continue}
        }
        set logSize [file size $logPath]
        if {$lastread($logId) < $logSize && ($logSize - $lastread($logId) - $max_log_change) < 0} {
            if {![catch {set handle [open $logPath r]} error]} {
                seek $handle $lastread($logId)
                set data [read -nonewline $handle]
                set logSize [tell $handle]
                close $handle

                foreach line [split $data "\n"] {
                    ## Remove the date and time from the log line.
                    if {[regexp $regex $line result event line]} {
                        lappend lines $logType $event $line
                    } else {
                        WarningMsg LogRead "Invalid log line: $line"
                    }
                }
            } else {
                ErrorMsg LogRead "Unable to open log file \"$logPath\" ($error)."
            }
        }
        set lastread($logId) $logSize
    }

    foreach {type event line} $lines {
        ## Login and sysop log specific parsing.
        if {$type == 1 && ![LogParseLogin $line event line]} {
            ErrorMsg LogRead "Unknown login.log line: $line"
            continue
        } elseif {$type == 2 && ![LogParseSysop $line event line]} {
            set event "SYSOP"
            set line [list $line]
        }
        DebugMsg LogRead "Received event: $event (log: $type)."

        ## Check that the log line is a valid Tcl list.
        if {[catch {llength $line} error]} {
            ErrorMsg LogRead "Invalid log line (not a tcl list): $line"
            continue
        }

        if {[string equal $event "INVITE"]} {
            foreach {nick user group flags} $line {break}
            InviteUser $nick $user $group $flags
        }

        if {[lsearch -exact $msgtypes(SECTION) $event] != -1} {
            set path [lindex $line 0]
            if {[IsPathDenied $path]} {continue}
            GetSectionName $path section sectionPath

            # Replace messages with custom messages
            foreach {name value} [array get msgreplace] {
                set value [split $value ":"]
                if {[string equal $event [lindex $value 0]]} {
                    if {[string match -nocase [lindex $value 1] $path]} {
                        set event [lindex $value 2]
                    }
                }
            }
        } elseif {[lsearch -exact $msgtypes(DEFAULT) $event] != -1} {
            set section $defaultsection
            set sectionPath ""
        } else {
            ErrorMsg LogRead "Undefined message type \"$event\", check \"msgtypes(SECTION)\" and \"msgtypes(DEFAULT)\" in the config."
            continue
        }

        ## If a pre-event script returns false, skip the announce.
        if {![EventHandler precommand $event $section $sectionPath $line] || ([info exists disable($event)] && $disable($event) == 1)} {
            DebugMsg LogRead "Announce skipped, \"$event\" is disabled or a pre-command script returned false."
            continue
        }

        if {![info exists variables($event)]} {
            ErrorMsg LogRead "\"variables($event)\" not defined in the config, type becomes \"$defaultsection\"."
            set event $defaultsection
        }
        if {![IsEventDenied $section $event]} {
            SendAll $event $section [LogFormat $event $section $sectionPath $line]
            EventHandler postcommand $event $section $sectionPath $line
        }
    }
    if {$glVersion == 1} {
        LaunchNuke
    }
    return
}

proc ::dZSbot::LogParseLogin {line eventVar dataVar} {
    upvar $eventVar event $dataVar data

    ## The data in login.log is not at all consistent,
    ## which makes it fun for us to parse.
    if {[regexp {^'(.+)' killed a ghost with PID (\d+)\.$} $line result user pid]} {
        set event "KILLGHOST"
        set data [list $user $pid]
    } elseif {[regexp {^(.+@.+) \((.+)\): connection refused: .+$} $line result hostmask ip]} {
        set event "IPNOTADDED"
        set data [list $hostmask $ip]
    } elseif {[regexp {^(\S+): (.+@.+) \((.+)\): (.+)} $line result user hostmask ip error]} {
        switch -exact -- $error {
            "Bad user@host."    {set event "BADHOSTMASK"}
            "Banned user@host." {set event "BANNEDHOST"}
            "Deleted."          {set event "DELETED"}
            "Login failure."    {set event "BADPASSWORD"}
            default {return 0}
        }
        set data [list $user $hostmask $ip]
    } elseif {![regexp {^(\S+): (.+)$} $line result event data]} {
        return 0
    }
    return 1
}

proc ::dZSbot::LogParseSysop {line eventVar dataVar} {
    upvar $eventVar event $dataVar newdata
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

proc ::dZSbot::LogRandom {event randVar} {
    upvar $randVar randEvent
    variable random
    ## Select a random announce theme.
    set randList [array names random "${event}-*"]
    if {[set randCount [llength $randList]]} {
        set randEvent [lindex $randList [rand $randCount]]
        return 1
    }
    return 0
}

proc ::dZSbot::LogFormat {event section sectionPath line} {
    variable announce
    variable defaultsection
    variable disable
    variable glVersion
    variable random
    variable sitename
    variable theme
    variable theme_fakes
    variable variables

    if {[string equal $event "NUKE"] || [string equal $event "UNNUKE"]} {
        if {$glVersion == 1} {
            FuelNuke $event $section $sectionPath $line
        } elseif {$glVersion == 2} {
            LaunchNuke2 $event $section $sectionPath $line
        } else {
            ErrorMsg LogFormat "Internal error, unknown glftpd version ($glVersion)."
        }
        return ""
    }

    if {![info exists announce($event)]} {
        ErrorMsg LogFormat "\"announce($event)\" not set in theme, event becomes \"$defaultsection\"."
        set event $defaultsection
    }
    set output $theme(PREFIX)

    ## Random announce themes.
    if {[string equal "random" $announce($event)] && [LogRandom $event randEvent]} {
        append output $random($randEvent)
    } else {
        append output $announce($event)
    }

    if {[string equal $section $defaultsection] && [info exists theme_fakes($event)]} {
        set section $theme_fakes($event)
    }
    set output [ReplaceBasic $output $section]

    ## Substitute all cookies with their corresponding values.
    set loop 1
    foreach varName $variables($event) value $line {
        if {[string equal "%pf" $varName]} {
            set output [ReplacePath $output $sectionPath $value]
        } elseif {[set varCount [llength $varName]] > 1} {
            set outputLoop ""
            set varIndex 0
            foreach subValue $value {
                if {!$varIndex} {
                    append outputLoop "$announce(${event}_LOOP${loop})"
                }
                ## Special cookie formatting.
                set subVarName [lindex $varName $varIndex]
                switch -glob -- $subVarName {
                    {%*_avgspeed} -
                    {%*_speed}    {set subValue [FormatSpeed $subValue $section]}
                    {%*_duration} {set subValue [FormatDuration $subValue]}
                }
                set outputLoop [ReplaceVar $outputLoop $subVarName $subValue]

                ## Reset the index and proceed to the next data set once
                ## we have processed all cookies for the current data.
                if {[incr varIndex] == $varCount} {
                    set varIndex 0
                }
            }
            set outputLoop [ReplaceBasic $outputLoop $section]
            set outputLoop [ReplaceVar $outputLoop "%splitter" $theme(SPLITTER)]
            set outputLoop [TrimTail $outputLoop $theme(SPLITTER)]
            set output [ReplaceVar $output "%loop$loop" $outputLoop]
            incr loop
        } else {
            ## Special cookie formatting.
            switch -glob -- $varName {
                {%*_avgspeed} -
                {%*_speed}    {set value [FormatSpeed $value $section]}
                {%*_duration} {set value [FormatDuration $value]}
            }
            set output [ReplaceVar $output $varName $value]
        }
    }
    return $output
}

#################################################################################
# Nuke and Unnuke Handlers                                                      #
#################################################################################

proc ::dZSbot::FuelNuke {event section sectionPath logData} {
    variable announce
    variable hidenuke
    variable nuke

    ## Log Data:
    ## NUKE   - path nuker nukee "multi amount" reason
    ## UNNUKE - path nuker nukee "multi amount" reason
    foreach {path nuker nukee info reason} $logData {break}
    set multi [lindex $info 0]
    set size [lindex $info 1]

    if {[info exists nuke($path)]} {
        set nukeeList [lindex $nuke($path) end]
    } else {
        set nukeeList ""
    }

    ## Format nuked user and append to the list.
    if {[lsearch -exact $hidenuke $nukee] == -1} {
        set output $announce(NUKEES)
        set output [ReplaceVar $output "%u_name" $nukee]
        set output [ReplaceVar $output "%size" $size]
        lappend nukeeList $output
    }

    set nuke($path) [list $event $section $sectionPath $nuker $multi $reason $nukeeList]
}

proc ::dZSbot::LaunchNuke {} {
    variable announce
    variable nuke
    variable theme

    foreach {path info} [array get nuke] {
        foreach {event section sectionPath nuker multi reason nukeeList} $info {break}
        set nukees [join $nukeeList $theme(SPLITTER)]

        set output [ReplaceBasic "$theme(PREFIX)$announce($event)" $section]
        set output [ReplaceVar $output "%nuker" $nuker]
        set output [ReplaceVar $output "%multiplier" $multi]
        set output [ReplaceVar $output "%reason" $reason]
        set output [ReplaceVar $output "%nukees" $nukees]
        set output [ReplacePath $output $sectionPath $path]

        SendAll $event $section $output
        unset nuke($path)
    }
}

proc ::dZSbot::LaunchNuke2 {event section sectionPath logData} {
    variable announce
    variable hidenuke
    variable theme

    ## Log Data:
    ## NUKE   - path nuker multi reason "nukee amount ..."
    ## UNNUKE - path nuker multi reason "nukee amount ..."
    foreach {path nuker multi reason nukees} $logData {break}

    ## Format nuked user list.
    set nukeeList ""
    foreach {user size} $nukees {
        if {[lsearch -exact $hidenuke $user] == -1} {
            set size [format "%.1f" [expr {$size / 1024}]]
            set output $announce(NUKEES)
            set output [ReplaceVar $output "%u_name" $user]
            set output [ReplaceVar $output "%size" $size]
            lappend nukeeList $output
        }
    }
    set nukees [join $nukeeList $theme(SPLITTER)]

    set output [ReplaceBasic "$theme(PREFIX)$announce($event)" $section]
    set output [ReplaceVar $output "%nuker" $nuker]
    set output [ReplaceVar $output "%multiplier" $multi]
    set output [ReplaceVar $output "%reason" $reason]
    set output [ReplaceVar $output "%nukees" $nukees]
    set output [ReplacePath $output $sectionPath $path]
    SendAll $event $section $output
}

#################################################################################
# Format Size, Speed and Time Units                                             #
#################################################################################

proc ::dZSbot::FormatDuration {secs} {
    set duration ""
    foreach div {31536000 604800 86400 3600 60 1} mod {0 52 7 24 60 60} unit {y w d h m s} {
        set num [expr {$secs / $div}]
        if {$mod > 0} {set num [expr {$num % $mod}]}
        if {$num > 0} {lappend duration "[b]$num[b]$unit"}
    }
    if {[llength $duration]} {return [join $duration]} else {return "[b]0[b]s"}
}

proc ::dZSbot::FormatKb {amount} {
    foreach dec {0 1 2 2 2} unit {KB MB GB TB PB} {
        if {abs($amount) >= 1024} {
            set amount [expr {double($amount) / 1024.0}]
        } else {break}
    }
    return [format "%.*f%s" $dec $amount $unit]
}

proc ::dZSbot::FormatSpeed {value section} {
    variable speedmeasure
    variable speedthreshold
    variable theme

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
    return [ThemeReplace [ReplaceVar $type "%value" $value] $section]
}

#################################################################################
# glFTPd Users and Groups                                                       #
#################################################################################

proc ::dZSbot::GetUserIds {} {
    variable location
    set userList ""
    if {![catch {set handle [open $location(PASSWD) r]} error]} {
        while {![eof $handle]} {
            ## Format: user:password:uid:gid:date:homedir:irrelevant
            set line [split [gets $handle] ":"]
            if {[llength $line] != 7} {continue}
            lappend userList [lindex $line 2] [lindex $line 0]
        }
        close $handle
    } else {
        ErrorMsg UserIds "Could not open passwd ($error)."
    }
    return $userList
}

proc ::dZSbot::GetGroupIds {} {
    variable location
    set groupList ""
    if {![catch {set handle [open $location(GROUP) r]} error]} {
        while {![eof $handle]} {
            ## Format: group:description:gid:irrelevant
            set line [split [gets $handle] ":"]
            if {[llength $line] != 4} {continue}
            lappend groupList [lindex $line 2] [lindex $line 0]
        }
        close $handle
    } else {
        ErrorMsg GroupIds "Could not open group ($error)."
    }
    return $groupList
}

#################################################################################
# Replace Text                                                                  #
#################################################################################

proc ::dZSbot::ReplaceBasic {message section} {
    variable cmdpre
    variable sitename
    return [string map [list "%cmdpre" $cmdpre "%section" $section \
        "%sitename" $sitename "%bold" [b] "%uline" \037 "%color" [c]] $message]
}

proc ::dZSbot::ReplacePath {message basePath fullPath} {
    variable subdirs

    set basePath [file split $basePath]
    set fullPath [file split $fullPath]
    set fullPath [lrange $fullPath [expr {[llength $basePath] - 1}] end]

    set relDir [lindex $fullPath end]
    set relFull [join [lrange $fullPath 0 end-1] "/"]

    set isSubDir 0
    foreach subDir $subdirs {
        if {[string match -nocase $subDir $relDir]} {
            set isSubDir 1; break
        }
    }
    if {$isSubDir && [llength $fullPath] > 1} {
        set relName "[lindex $fullPath end-1] ([lindex $fullPath end])"
        set relPath [join [lrange $fullPath 0 end-2] "/"]
    } else {
        set relName $relDir
        set relPath $relFull
    }

    set message [ReplaceVar $message "%reldir" $relDir]
    set message [ReplaceVar $message "%relfull" $relFull]
    set message [ReplaceVar $message "%relname" $relName]
    set message [ReplaceVar $message "%relpath" $relPath]
    return $message
}

proc ::dZSbot::ReplaceVar {string cookie value} {
    variable zeroconvert
    if {[string length $value] == 0 && [info exists zeroconvert($cookie)]} {
        set value $zeroconvert($cookie)
    }
    return [string map [list $cookie $value] $string]
}

proc ::dZSbot::TrimTail {base tail} {
    if {([string length $base] - [string length $tail]) == [string last $tail $base]} {
        return [string range $base 0 [expr {[string length $base] - [string length $tail] - 1}]]
    }
    return $base
}

#################################################################################
# Send Messages                                                                 #
#################################################################################

proc ::dZSbot::SendAll {event section text} {
    variable chanlist
    variable redirect

    if {[info exists redirect($event)]} {
        set channels $redirect($event)
        DebugMsg SendAll "Redirecting the \"$event\" event to \"$channels\"."
    } elseif {[info exists chanlist($section)]} {
        set channels $chanlist($section)
    } else {
        ErrorMsg SendAll "\"chanlist($section)\" not defined in the config."
        return
    }
    foreach chan $channels {
        SendOne $chan $text $section
    }
}

proc ::dZSbot::SendOne {chan text {section "none"}} {
    variable splitter
    foreach line [split $text $splitter(CHAR)] {
        putquick "PRIVMSG $chan :[ThemeReplace $line $section]"
    }
}

#################################################################################
# Invite User                                                                   #
#################################################################################

proc ::dZSbot::RightsFlagCheck {currentflags needflags} {
    set currentflags [split $currentflags ""]
    foreach needflag [split $needflags ""] {
        if {![string equal "" $needflag] && [lsearch -glob $currentflags $needflag] != -1} {return 1}
    }
    return 0
}

proc ::dZSbot::RightsCheck {rights user group flags} {
    foreach right $rights {
        set prefix [string index $right 0]
        if {[string equal "!" $prefix]} {
            ## 'Not' matching (!)
            set right [string range $right 1 end]
            set prefix [string index $right 0]

            if {[string equal "-" $prefix]} {
                set right [string range $right 1 end]
                if {[string match $right $user]} {return 0}
            } elseif {[string equal "=" $prefix]} {
                set right [string range $right 1 end]
                if {[string match $right $group]} {return 0}
            } elseif {[RightsFlagCheck $flags $right]} {return 0}

        ## Regular matching
        } elseif {[string equal "-" $prefix]} {
            set right [string range $right 1 end]
            if {[string match $right $user]} {return 1}
        } elseif {[string equal "=" $prefix]} {
            set right [string range $right 1 end]
            if {[string match $right $group]} {return 1}
        } elseif {[RightsFlagCheck $flags $right]} {return 1}
    }
    return 0
}

proc ::dZSbot::InviteChan {nick chan} {
    if {![validchan $chan] || ![botonchan $chan]} {
        ErrorMsg InviteChan "Unable to invite \"$nick\" to \"$chan\", I'm not in the channel."
    } elseif {![botisop $chan]} {
        ErrorMsg InviteChan "Unable to invite \"$nick\" to \"$chan\", I'm not opped."
    } else {
        putquick "INVITE $nick $chan"
    }
}

proc ::dZSbot::InviteUser {nick user group flags} {
    variable invite_channels
    variable privchannel
    if {![EventHandler precommand INVITEUSER $nick $user $group $flags]} {return}

    ## Invite the user to the defined channels.
    foreach chan $invite_channels {
        InviteChan $nick $chan
    }
    foreach {chan rights} [array get privchannel] {
        if {[RightsCheck $rights $user $group $flags]} {
            InviteChan $nick $chan
        }
    }

    EventHandler postcommand INVITEUSER $nick $user $group $flags
    return
}

proc ::dZSbot::CmdInvite {nick host hand argv} {
    variable announce
    variable binary
    variable disable
    variable invite_channel
    variable location
    variable theme

    if {[llength $argv] > 1} {
        set user [lindex $argv 0]
        set pass [lindex $argv 1]
        set result [exec $binary(PASSCHK) $user $pass $location(PASSWD)]
        set group ""
        set flags ""

        if {[string equal $result "MATCH"]} {
            set output "$theme(PREFIX)$announce(MSGINVITE)"
            ## Check the user file for the user's group.
            if {![catch {set handle [open "$location(USERS)/$user" r]} error]} {
                set data [read $handle]
                close $handle
                foreach line [split $data "\n"] {
                    switch -exact -- [lindex $line 0] {
                        "FLAGS" {set flags [lindex $line 1]}
                        "GROUP" {set group [lindex $line 1]}
                    }
                }
            } else {
                ErrorMsg Invite "Unable to open user file for \"$user\" ($error)."
            }
            InviteUser $nick $user $group $flags
        } else {
            set output "$theme(PREFIX)$announce(BADMSGINVITE)"
        }

        if {$disable(MSGINVITE) != 1} {
            set output [ReplaceVar $output "%u_ircnick" $nick]
            set output [ReplaceVar $output "%u_name" $user]
            set output [ReplaceVar $output "%u_host" $host]
            set output [ReplaceVar $output "%g_name" $group]
            SendAll "MSGINVITE" "DEFAULT" [ReplaceBasic $output "INVITE"]
        }
    }
    return
}

#################################################################################
# Show Welcome Message                                                          #
#################################################################################

proc ::dZSbot::ShowWelcome {nick uhost hand chan} {
    variable announce
    variable chanlist
    variable cmdpre
    variable disable
    variable sitename
    if {$disable(WELCOME) == 1 || [isbotnick $nick]} {return}

    foreach allowed $chanlist(WELCOME) {
        if {![string equal -nocase $allowed $chan]} {continue}
        set output [ReplaceBasic $announce(WELCOME) "WELCOME"]
        set output [ReplaceVar $output "%ircnick" $nick]
        set output [ThemeReplace $output "none"]
        puthelp "NOTICE $nick :$output"
        break
    }
    return
}

################################################################################
# Command Utilities                                                            #
################################################################################

proc ::dZSbot::CheckChan {nick chan} {
    variable disable
    variable mainchan

    if {$disable(TRIGINALLCHAN) == 1 && ![string equal -nocase $chan $mainchan]} {
        WarningMsg CheckChan "[b]$nick[b] tried to use [b]$::lastbind[b] from an invalid channel ($chan)."
        return -code return
    }
}

proc ::dZSbot::GetOptions {argv resultsVar otherVar} {
    upvar $resultsVar results $otherVar other
    variable default_results
    variable maximum_results

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

proc ::dZSbot::GetSectionName {fullPath sectionVar secionPathVar} {
    upvar $sectionVar sectionName $secionPathVar sectionPath
    variable defaultsection
    variable paths
    variable sections

    set bestMatch 0
    set sectionName $defaultsection
    set sectionPath ""

    foreach section $sections {
        if {![info exists paths($section)]} {
            ErrorMsg SectionName "\"paths($section)\" is not defined in the config."
            continue
        }
        foreach path $paths($section) {
            ## Compare the path length of the previous match (best match wins).
            if {[string match $path $fullPath] && [set pathLength [string length $path]] > $bestMatch} {
                set bestMatch $pathLength
                set sectionName $section
                set sectionPath $path
            }
        }
    }
    return
}

proc ::dZSbot::GetSectionPath {getsection} {
    variable paths
    variable sections

    foreach section $sections {
        if {![info exists paths($section)]} {
            ErrorMsg SectionPath "\"paths($section)\" not set in config."
        } elseif {[string equal -nocase $getsection $section]} {
            return [list $section $paths($section)]
        }
    }
    return ""
}

################################################################################
# General Commands                                                             #
################################################################################

proc ::dZSbot::CmdBnc {nick uhost hand chan arg} {
    global errorCode
    variable announce
    variable binary
    variable bnc
    variable theme
    CheckChan $nick $chan
    if {![IsTrue $bnc(ENABLED)]} {return}

    set output "$theme(PREFIX)$announce(BNC)"
    SendOne $nick [ReplaceBasic $output "BNC"]

    set num 0
    foreach entry $bnc(LIST) {
        set entrysplit [split $entry ":"]
        if {[llength $entrysplit] != 3} {
            ErrorMsg BncList "Invalid bouncer line \"$entry\" (check bnc(LIST))."
            continue
        }
        incr num; set ping "N/A"
        foreach {desc ip port} $entrysplit {break}

        if {[IsTrue $bnc(PING)]} {
            if {[catch {exec $binary(PING) -c 1 -t $bnc(TIMEOUT) $ip} reply]} {
                set output "$theme(PREFIX)$announce(BNC_PING)"
                set output [ReplaceVar $output "%num" $num]
                set output [ReplaceVar $output "%desc" $desc]
                set output [ReplaceVar $output "%ip" $ip]
                set output [ReplaceVar $output "%port" $port]
                SendOne $nick [ReplaceBasic $output "BNC"]
                continue
            }
            set reply [lindex [split $reply "\n"] 1]
            if {[regexp {.+time=(\S+) ms} $reply reply ping]} {
                set ping [format "%.1f" $ping]
            } else {
                ErrorMsg BncPing "Unable to parse ping reply \"$reply\", please report to pzs-ng developers."
            }
        }

        set response [clock clicks -milliseconds]
        if {[IsTrue $bnc(SECURE)]} {
            set status [catch {exec $binary(CURL) --connect-timeout $bnc(TIMEOUT) --ftp-ssl --insecure -u $bnc(USER):$bnc(PASS) ftp://$ip:$port 2>@stdout} reply]
        } else {
            set status [catch {exec $binary(CURL) --connect-timeout $bnc(TIMEOUT) -u $bnc(USER):$bnc(PASS) ftp://$ip:$port 2>@stdout} reply]
        }
        set response [expr {[clock clicks -milliseconds] - $response}]

        if {!$status} {
            set output "$theme(PREFIX)$announce(BNC_ONLINE)"
            set output [ReplaceVar $output "%ping" $ping]
            set output [ReplaceVar $output "%response" $response]
        } else {
            set error "unknown error"
            ## Check curl's exit code (stored in errorCode).
            if {[string equal "CHILDSTATUS" [lindex $errorCode 0]]} {
                set code [lindex $errorCode 2]
                switch -exact -- $code {
                    6 {set error "couldn't resolve host"}
                    7 {set error "connection refused"}
                    9 - 10 {set error "couldn't login"}
                    28 {set error "timed out"}
                    default {ErrorMsg BncCurl "Unknown curl exit code \"$code\", check the \"exit codes\" section of curl's man page."}
                }
            } else {
                ## If the first list item in errorCode is not "CHILDSTATUS",
                ## Tcl was unable to execute the binary.
                ErrorMsg BncCurl "Unable to execute curl ($reply)."
            }
            set output "$theme(PREFIX)$announce(BNC_OFFLINE)"
            set output [ReplaceVar $output "%error" $error]
        }

        set output [ReplaceVar $output "%num" $num]
        set output [ReplaceVar $output "%desc" $desc]
        set output [ReplaceVar $output "%ip" $ip]
        set output [ReplaceVar $output "%port" $port]
        SendOne $nick [ReplaceBasic $output "BNC"]
    }
    return
}

proc ::dZSbot::CmdHelp {nick uhost hand chan arg} {
    variable scriptPath
    variable sections
    variable statsection
    CheckChan $nick $chan

    if {[catch {set handle [open [file join $scriptPath dZSbot.help] r]} error]} {
        ErrorMsg Help "Unable to read the help file ($error)."
        puthelp "PRIVMSG $nick :Unable to read the help file, please contact a siteop."
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
        set line [ReplaceVar $line "%sections" $sectlist]
        set line [ReplaceVar $line "%statsections" $statlist]
        puthelp "PRIVMSG $nick :[ThemeReplace [ReplaceBasic $line "HELP"] "none"]"
    }
    return
}

proc ::dZSbot::CmdFree {nick uhost hand chan arg} {
    variable announce
    variable binary
    variable dev_max_length
    variable device
    variable theme
    CheckChan $nick $chan

    array set tmpdev [array get device]
    set devices(0) ""
    set devCount 0; set lineCount 0
    set totalFree 0; set totalUsed 0; set totalSize 0

    foreach line [split [exec $binary(DF) "-Pk"] "\n"] {
        foreach {name value} [array get tmpdev] {
            if {[string equal [lindex $line 0] [lindex $value 0]]} {
                if {[llength $line] < 4} {
                    WarningMsg Free "Invalid \"df -Pk\" line: $line"
                    continue
                }
                foreach {devName devSize devUsed devFree} $line {break}
                set devPercFree [format "%.1f" [expr {(double($devFree) / double($devSize)) * 100}]]
                set devPercUsed [format "%.1f" [expr {(double($devUsed) / double($devSize)) * 100}]]

                set output $announce(FREE-DEV)
                set output [ReplaceVar $output "%free" [FormatKb $devFree]]
                set output [ReplaceVar $output "%used" [FormatKb $devUsed]]
                set output [ReplaceVar $output "%total" [FormatKb $devSize]]
                set output [ReplaceVar $output "%perc_free" $devPercFree]
                set output [ReplaceVar $output "%perc_used" $devPercUsed]
                set output [ReplaceVar $output "%section" [lrange $value 1 end]]

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
        WarningMsg Free "The following devices had no matching \"df -Pk\" entry: [join $devList {, }]"
    }

    if {$totalSize} {
        set percFree [format "%.1f" [expr {(double($totalFree) / double($totalSize)) * 100}]]
        set percUsed [format "%.1f" [expr {(double($totalUsed) / double($totalSize)) * 100}]]
    } else {
        set percFree 0.0; set percUsed 0.0
    }
    set totalFree [FormatKb $totalFree]
    set totalUsed [FormatKb $totalUsed]
    set totalSize [FormatKb $totalSize]

    set index 0
    while {$index < ($lineCount + 1)} {
        set output "$theme(PREFIX)$announce(FREE)"
        set output [ReplaceVar $output "%free" $totalFree]
        set output [ReplaceVar $output "%used" $totalUsed]
        set output [ReplaceVar $output "%total" $totalSize]
        set output [ReplaceVar $output "%perc_free" $percFree]
        set output [ReplaceVar $output "%perc_used" $percUsed]
        set output [ReplaceVar $output "%devices" $devices($index)]
        SendOne $chan [ReplaceBasic $output "FREE"]
        incr index
    }
    return
}

proc ::dZSbot::CmdIncompletes {nick uhost hand chan arg} {
    variable binary
    CheckChan $nick $chan

    foreach line [split [exec $binary(INCOMPLETE)] "\n"] {
        if {![info exists newline($line)]} {
            set newline($line) 0
        } else {
            incr newline($line)
        }
        puthelp "PRIVMSG $nick :$line[c]$newline($line)"
    }
    return
}

proc ::dZSbot::CmdStats {type time nick uhost hand chan argv} {
    variable binary
    variable statsection
    variable statdefault
    variable location
    CheckChan $nick $chan

    set section $statdefault
    if {[string length [lindex $argv 0]]} {
        set error 1
        set sections ""
        foreach {name value} [array get statsection] {
            if {[string equal -nocase $value [lindex $argv 0]]} {
                set section $value
                set error 0; break
            }
            lappend sections $value
        }
        if {$error} {
            puthelp "PRIVMSG $nick :Invalid section, sections: [join [lsort -ascii $sections] {, }]"
            return
        }
    }

    if {[catch {set output [exec $binary(STATS) -r $location(GLCONF) $type $time -s $section]} error]} {
        ErrorMsg Stats "Unable to retrieve stats ($error)."
        return
    }

    foreach line [split $output "\n"] {
        if {![info exists newline($line)]} {
            set newline($line) 0
        } else {
            incr newline($line)
        }
        puthelp "PRIVMSG $nick :$line[c]$newline($line)"
    }
    puthelp "PRIVMSG $nick :------------------------------------------------------------------------"
    return
}

proc ::dZSbot::CmdUptime {nick uhost hand chan argv} {
    global uptime
    variable announce
    variable binary
    variable theme
    CheckChan $nick $chan

    if {[catch {exec $binary(UPTIME)} reply]} {
        ErrorMsg Uptime "Unable to execute uptime ($reply)."
    }
    ## The linux 'uptime' pads the output with spaces, so we'll remove them.
    regsub -all {\s+} $reply { } reply

    if {![regexp {.+ up (.+), (.+) users?, load averages?: (.+)} $reply reply time users load]} {
        set load "N/A"; set time "N/A"; set users "N/A"
        ErrorMsg Uptime "Unable to parse uptime reply \"$reply\", please report to pzs-ng developers."
    }
    set eggup [FormatDuration [expr {[clock seconds] - $uptime}]]

    set output "$theme(PREFIX)$announce(UPTIME)"
    set output [ReplaceVar $output "%eggdrop" $eggup]
    set output [ReplaceVar $output "%time" $time]
    set output [ReplaceVar $output "%users" $users]
    set output [ReplaceVar $output "%load" $load]
    SendOne $chan [ReplaceBasic $output "UPTIME"]
    return
}

################################################################################
# Latest Dirs/Nukes Commands                                                   #
################################################################################

proc ::dZSbot::CmdNew {nick uhost hand chan argv} {
    variable announce
    variable binary
    variable defaultsection
    variable location
    variable sections
    variable theme
    CheckChan $nick $chan

    if {![GetOptions $argv results section]} {
        ## By displaying the command syntax in the channel (opposed to private message), we can inform others
        ## at the same time. There's this recurring phenomena, every time a user types an "uncommon" command, half
        ## a dozen others will as well...to learn about this command. So, let's kill a few idiots with one stone.
        puthelp "PRIVMSG $chan :[b]Usage:[b] $::lastbind \[-max <num>\] \[section\]"
        return
    }
    if {[string equal "" $section]} {
        set section $defaultsection
        set sectionpath "/site/*"
        set lines [exec $binary(SHOWLOG) -l -m $results -r $location(GLCONF)]
    } else {
        if {[set sectiondata [GetSectionPath $section]] == ""} {
            puthelp "PRIVMSG $nick :Invalid section, sections: [join [lsort -ascii $sections] {, }]"
            return
        }
        foreach {section sectionpath} $sectiondata {break}
        set lines [exec $binary(SHOWLOG) -f -l -m $results -p $sectionpath -r $location(GLCONF)]
    }

    ## Retrieve a list of UIDs/users and GIDs/groups
    array set uid [GetUserIds]
    array set gid [GetGroupIds]

    set output "$theme(PREFIX)$announce(NEW)"
    set output [ReplaceVar $output "%section" $section]
    SendOne $nick [ReplaceBasic $output "NEW"]
    set body "$theme(PREFIX)$announce(NEW_BODY)"
    set num 0

    foreach line [split $lines "\n"] {
        ## Format: status|uptime|uploader|group|files|kilobytes|dirpath
        if {[llength [set line [split $line "|"]]] != 7} {continue}
        foreach {status ctime userid groupid files kbytes dirpath} $line {break}

        ## If array get returns "", zeroconvert will replace the value with NoOne/NoGroup.
        set user [lindex [array get uid $userid] 1]
        set group [lindex [array get gid $groupid] 1]

        set output [ReplaceVar $body "%num" [format "%02d" [incr num]]]
        set age [lrange [FormatDuration [expr {[clock seconds] - $ctime}]] 0 1]
        set output [ReplaceVar $output "%age" $age]
        set output [ReplaceVar $output "%date" [clock format $ctime -format "%m-%d-%y"]]
        set output [ReplaceVar $output "%time" [clock format $ctime -format "%H:%M:%S"]]
        set output [ReplaceVar $output "%u_name" $user]
        set output [ReplaceVar $output "%g_name" $group]
        set output [ReplaceVar $output "%files" $files]
        set output [ReplaceVar $output "%mbytes" [format "%.1f" [expr {$kbytes / 1024.0}]]]
        set output [ReplacePath $output $sectionpath $dirpath]
        SendOne $nick [ReplaceBasic $output "NEW"]
    }

    if {!$num} {
        set output "$theme(PREFIX)$announce(NEW_NONE)"
        SendOne $nick [ReplaceBasic $output "NEW"]
    }
    return
}

proc ::dZSbot::CmdNukes {nick uhost hand chan argv} {
    variable announce
    variable binary
    variable defaultsection
    variable location
    variable sections
    variable theme
    CheckChan $nick $chan

    if {![GetOptions $argv results section]} {
        puthelp "PRIVMSG $chan :[b]Usage:[b] $::lastbind \[-max <num>\] \[section\]"
        return
    }
    if {[string equal "" $section]} {
        set section $defaultsection
        set sectionpath "/site/*"
        set lines [exec $binary(SHOWLOG) -n -m $results -r $location(GLCONF)]
    } else {
        if {[set sectiondata [GetSectionPath $section]] == ""} {
            puthelp "PRIVMSG $nick :Invalid section, sections: [join [lsort -ascii $sections] {, }]"
            return
        }
        foreach {section sectionpath} $sectiondata {break}
        set lines [exec $binary(SHOWLOG) -f -n -m $results -p $sectionpath -r $location(GLCONF)]
    }

    set output "$theme(PREFIX)$announce(NUKES)"
    set output [ReplaceVar $output "%section" $section]
    SendOne $nick [ReplaceBasic $output "NUKES"]
    set body "$theme(PREFIX)$announce(NUKES_BODY)"
    set num 0

    foreach line [split $lines "\n"] {
        ## Format: status|nuketime|nuker|unnuker|nukee|multiplier|reason|kilobytes|dirpath
        if {[llength [set line [split $line "|"]]] != 9} {continue}
        foreach {status nuketime nuker unnuker nukee multiplier reason kbytes dirpath} $line {break}

        set output [ReplaceVar $body "%num" [format "%02d" [incr num]]]
        set output [ReplaceVar $output "%date" [clock format $nuketime -format "%m-%d-%y"]]
        set output [ReplaceVar $output "%time" [clock format $nuketime -format "%H:%M:%S"]]
        set output [ReplaceVar $output "%nuker" $nuker]
        set output [ReplaceVar $output "%nukee" $nukee]
        set output [ReplaceVar $output "%multiplier" $multiplier]
        set output [ReplaceVar $output "%reason" $reason]
        set output [ReplaceVar $output "%mbytes" [format "%.1f" [expr {$kbytes / 1024.0}]]]
        set output [ReplacePath $output $sectionpath $dirpath]
        SendOne $nick [ReplaceBasic $output "NUKES"]
    }

    if {!$num} {
        set output "$theme(PREFIX)$announce(NUKES_NONE)"
        SendOne $nick [ReplaceBasic $output "NUKES"]
    }
    return
}

proc ::dZSbot::CmdSearch {nick uhost hand chan argv} {
    variable announce
    variable binary
    variable defaultsection
    variable location
    variable search_chars
    variable theme
    CheckChan $nick $chan

    if {![GetOptions $argv results pattern] || [string equal "" $pattern]} {
        puthelp "PRIVMSG $chan :[b]Usage:[b] $::lastbind \[-max <num>\] <pattern>"
        return
    }
    if {$search_chars > 0 && [regexp -all {[a-zA-Z0-9]} $pattern] < $search_chars} {
        puthelp "PRIVMSG $nick :The search pattern must at be at least [b]$search_chars[b] alphanumeric characters."
        return
    }
    ## Retrieve matching dirlog entries
    regsub -all {[\s\*]+} "*${pattern}*" {*} pattern
    set lines [exec $binary(SHOWLOG) -l -s -m $results -p $pattern -r $location(GLCONF)]

    ## Retrieve a list of UIDs/users and GIDs/groups
    array set uid [GetUserIds]
    array set gid [GetGroupIds]

    set output "$theme(PREFIX)$announce(SEARCH)"
    set output [ReplaceVar $output "%pattern" $pattern]
    SendOne $nick [ReplaceBasic $output "SEARCH"]
    set body "$theme(PREFIX)$announce(SEARCH_BODY)"
    set num 0

    foreach line [split $lines "\n"] {
        ## Format: status|uptime|uploader|group|files|kilobytes|dirpath
        if {[llength [set line [split $line "|"]]] != 7} {continue}
        foreach {status ctime userid groupid files kbytes dirpath} $line {break}

        ## If array get returns "", zeroconvert will replace the value with NoOne/NoGroup.
        set user [lindex [array get uid $userid] 1]
        set group [lindex [array get gid $groupid] 1]

        set output [ReplaceVar $body "%num" [format "%02d" [incr num]]]
        set age [lrange [FormatDuration [expr {[clock seconds] - $ctime}]] 0 1]
        set output [ReplaceVar $output "%age" $age]
        set output [ReplaceVar $output "%date" [clock format $ctime -format "%m-%d-%y"]]
        set output [ReplaceVar $output "%time" [clock format $ctime -format "%H:%M:%S"]]
        set output [ReplaceVar $output "%u_name" $user]
        set output [ReplaceVar $output "%g_name" $group]
        set output [ReplaceVar $output "%files" $files]
        set output [ReplaceVar $output "%mbytes" [format "%.1f" [expr {$kbytes / 1024.0}]]]
        set output [ReplacePath $output "/site/*" $dirpath]
        SendOne $nick [ReplaceBasic $output "SEARCH"]
    }

    if {!$num} {
        set output "$theme(PREFIX)$announce(SEARCH_NONE)"
        SendOne $nick [ReplaceBasic $output "SEARCH"]
    }
    return
}

proc ::dZSbot::CmdUnnukes {nick uhost hand chan argv} {
    variable announce
    variable binary
    variable defaultsection
    variable location
    variable sections
    variable theme
    CheckChan $nick $chan

    if {![GetOptions $argv results section]} {
        puthelp "PRIVMSG $chan :[b]Usage:[b] $::lastbind \[-max <num>\] \[section\]"
        return
    }
    if {[string equal "" $section]} {
        set section $defaultsection
        set sectionpath "/site/*"
        set lines [exec $binary(SHOWLOG) -u -m $results -r $location(GLCONF)]
    } else {
        if {[set sectiondata [GetSectionPath $section]] == ""} {
            puthelp "PRIVMSG $nick :Invalid section, sections: [join [lsort -ascii $sections] {, }]"
            return
        }
        foreach {section sectionpath} $sectiondata {break}
        set lines [exec $binary(SHOWLOG) -f -u -m $results -p $sectionpath -r $location(GLCONF)]
    }

    set output "$theme(PREFIX)$announce(UNNUKES)"
    set output [ReplaceVar $output "%section" $section]
    SendOne $nick [ReplaceBasic $output "UNNUKES"]
    set body "$theme(PREFIX)$announce(UNNUKES_BODY)"
    set num 0

    foreach line [split $lines "\n"] {
        ## Format: status|nuketime|nuker|unnuker|nukee|multiplier|reason|kilobytes|dirpath
        if {[llength [set line [split $line "|"]]] != 9} {continue}
        foreach {status nuketime nuker unnuker nukee multiplier reason kbytes dirpath} $line {break}

        set output [ReplaceVar $body "%num" [format "%02d" [incr num]]]
        set output [ReplaceVar $output "%date" [clock format $nuketime -format "%m-%d-%y"]]
        set output [ReplaceVar $output "%time" [clock format $nuketime -format "%H:%M:%S"]]
        set output [ReplaceVar $output "%nuker" $nuker]
        set output [ReplaceVar $output "%unnuker" $unnuker]
        set output [ReplaceVar $output "%nukee" $nukee]
        set output [ReplaceVar $output "%multiplier" $multiplier]
        set output [ReplaceVar $output "%reason" $reason]
        set output [ReplaceVar $output "%mbytes" [format "%.1f" [expr {$kbytes / 1024.0}]]]
        set output [ReplacePath $output $sectionpath $dirpath]
        SendOne $nick [ReplaceBasic $output "UNNUKES"]
    }

    if {!$num} {
        set output "$theme(PREFIX)$announce(UNNUKES_NONE)"
        SendOne $nick [ReplaceBasic $output "UNNUKES"]
    }
    return
}

################################################################################
# Online Stats Commands                                                        #
################################################################################

proc ::dZSbot::CmdBw {nick uhost hand chan argv} {
    variable announce
    variable binary
    variable speed
    variable theme
    CheckChan $nick $chan

    set output "$theme(PREFIX)$announce(BW)"
    set raw [exec $binary(WHO) --nbw]
    set upper [format "%.0f" [expr [lindex $raw 1] * 100 / $speed(INCOMING)]]
    set dnper [format "%.0f" [expr [lindex $raw 3] * 100 / $speed(OUTGOING)]]
    set totalper [format "%.0f" [expr [lindex $raw 5] * 100 / ( $speed(INCOMING) + $speed(OUTGOING) )]]

    set up [FormatSpeed [lindex $raw 1] "none"]
    set dn [FormatSpeed [lindex $raw 3] "none"]
    set totalspeed [FormatSpeed [lindex $raw 5] "none"]

    set output [ReplaceVar $output "%uploads" [lindex $raw 0]]
    set output [ReplaceVar $output "%upspeed" $up]
    set output [ReplaceVar $output "%downloads" [lindex $raw 2]]
    set output [ReplaceVar $output "%dnspeed" $dn]
    set output [ReplaceVar $output "%transfers" [lindex $raw 4]]
    set output [ReplaceVar $output "%totalspeed" $totalspeed]
    set output [ReplaceVar $output "%idlers" [lindex $raw 6]]
    set output [ReplaceVar $output "%active" [lindex $raw 7]]
    set output [ReplaceVar $output "%totallogins" [lindex $raw 8]]
    set output [ReplaceVar $output "%maxusers" [lindex $raw 9]]

    set output [ReplaceVar $output "%uppercent" $upper]
    set output [ReplaceVar $output "%dnpercent" $dnper]
    set output [ReplaceVar $output "%totalpercent" $totalper]
    SendOne $chan [ReplaceBasic $output "BW"]
    return
}

proc ::dZSbot::CmdBwDn {nick uhost hand chan argv} {
    variable announce
    variable binary
    variable speed
    variable theme
    CheckChan $nick $chan

    set output "$theme(PREFIX)$announce(BWDN)"
    set raw [exec $binary(WHO) --nbw]
    set upper [format "%.1f" [expr 100 * ([lindex $raw 1] / $speed(INCOMING))]]
    set dnper [format "%.1f" [expr 100 * ([lindex $raw 3] / $speed(OUTGOING))]]
    set totalper [format "%.0f" [expr [lindex $raw 5] / ( $speed(INCOMING) + $speed(OUTGOING))]]

    set output [ReplaceVar $output "%uploads" [lindex $raw 0]]
    set output [ReplaceVar $output "%upspeed" [FormatSpeed [lindex $raw 1] "none"]]
    set output [ReplaceVar $output "%downloads" [lindex $raw 2]]
    set output [ReplaceVar $output "%dnspeed" [FormatSpeed [lindex $raw 3] "none"]]
    set output [ReplaceVar $output "%transfers" [lindex $raw 4]]
    set output [ReplaceVar $output "%totalspeed" [FormatSpeed [lindex $raw 5] "none"]]
    set output [ReplaceVar $output "%idlers" [lindex $raw 6]]
    set output [ReplaceVar $output "%active" [lindex $raw 7]]
    set output [ReplaceVar $output "%totallogins" [lindex $raw 8]]
    set output [ReplaceVar $output "%maxusers" [lindex $raw 9]]

    set output [ReplaceVar $output "%uppercent" $upper]
    set output [ReplaceVar $output "%dnpercent" $dnper]
    set output [ReplaceVar $output "%totalpercent" $totalper]
    SendOne $chan [ReplaceBasic $output "BW"]
    return
}

proc ::dZSbot::CmdBwUp {nick uhost hand chan argv} {
    variable announce
    variable binary
    variable speed
    variable theme
    CheckChan $nick $chan

    set output "$theme(PREFIX)$announce(BWUP)"
    set raw [exec $binary(WHO) --nbw]
    set upper [format "%.1f" [expr 100 * ([lindex $raw 1] / $speed(INCOMING))]]
    set dnper [format "%.1f" [expr 100 * ([lindex $raw 3] / $speed(OUTGOING))]]
    set totalper [format "%.0f" [expr 100 * ([lindex $raw 5] / ( $speed(INCOMING) + $speed(OUTGOING)))]]

    set output [ReplaceVar $output "%uploads" [lindex $raw 0]]
    set output [ReplaceVar $output "%upspeed" [FormatSpeed [lindex $raw 1] "none"]]
    set output [ReplaceVar $output "%downloads" [lindex $raw 2]]
    set output [ReplaceVar $output "%dnspeed" [FormatSpeed [lindex $raw 3] "none"]]
    set output [ReplaceVar $output "%transfers" [lindex $raw 4]]
    set output [ReplaceVar $output "%totalspeed" [FormatSpeed [lindex $raw 5] "none"]]
    set output [ReplaceVar $output "%idlers" [lindex $raw 6]]
    set output [ReplaceVar $output "%active" [lindex $raw 7]]
    set output [ReplaceVar $output "%totallogins" [lindex $raw 8]]
    set output [ReplaceVar $output "%maxusers" [lindex $raw 9]]

    set output [ReplaceVar $output "%uppercent" $upper]
    set output [ReplaceVar $output "%dnpercent" $dnper]
#   set output [ReplaceVar $output "%totalpercent" $totalper]
    SendOne $chan [ReplaceBasic $output "BW"]
}

proc ::dZSbot::CmdIdlers {nick uhost hand chan argv} {
    variable announce
    variable binary
    variable minidletime
    variable speed
    variable theme
    CheckChan $nick $chan

    set output "$theme(PREFIX)$announce(IDLE)"
    SendOne $chan [ReplaceBasic $output "IDLE"]

    set raw [exec $binary(WHO) "--raw"]
    set count 0; set total 0.0

    foreach line [split $raw "\n"] {
        if {[string equal "USER" [lindex $line 0]] && [string equal "ID" [lindex $line 4]]} {
            set user  [lindex $line 2]
            set group [lindex $line 3]
            set idletime [lindex $line 5]
            set tagline [lindex $line 6]
            set since [lindex $line 7]

            if {$idletime > $minidletime} {
                set output [ReplaceVar "$theme(PREFIX)$announce(USERIDLE)" "%u_name" $user]
                set output [ReplaceVar $output "%g_name" $group]
                set output [ReplaceVar $output "%idletime" [FormatDuration $idletime]]
                set output [ReplaceVar $output "%tagline" $tagline]
                set output [ReplaceVar $output "%since" $since]
                SendOne $chan [ReplaceBasic $output "IDLE"]
                incr count
            }
        }
    }
    set output [ReplaceVar "$theme(PREFIX)$announce(TOTALIDLE)" "%count" $count]
    SendOne $chan [ReplaceBasic $output "IDLE"]
    return
}

proc ::dZSbot::CmdLeechers {nick uhost hand chan argv} {
    variable announce
    variable binary
    variable speed
    variable theme
    CheckChan $nick $chan

    set output "$theme(PREFIX)$announce(LEECH)"
    SendOne $chan [ReplaceBasic $output "LEECH"]

    set raw [exec $binary(WHO) "--raw"]
    set count 0; set total 0.0

    foreach line [split $raw "\n"] {
        if {[string equal "USER" [lindex $line 0]] && [string equal "DN" [lindex $line 4]]} {
            set user  [lindex $line 2]
            set group [lindex $line 3]
            set uspeed [ReplaceVar [lindex $line 5] "KB/s" ""]
            set tagline [lindex $line 6]
            set since [lindex $line 7]
            set filename [lindex $line 8]
            set per [format "%.2f%%" [expr double($uspeed) * 100 / double($speed(OUTGOING))]]
            set fper [lindex $line 9]
            set output [ReplaceVar "$theme(PREFIX)$announce(USERDN)" "%u_name" $user]
            set output [ReplaceVar $output "%g_name" $group]
            set output [ReplaceVar $output "%fper"  $fper]
            set output [ReplaceVar $output "%speed" [FormatSpeed $uspeed "none"]]
            set output [ReplaceVar $output "%per" $per]
            set output [ReplaceVar $output "%tagline" $tagline]
            set output [ReplaceVar $output "%since" $since]
            set output [ReplaceVar $output "%filename" $filename]
            SendOne $chan [ReplaceBasic $output "LEECH"]
            incr count
            set total [expr {$total + $uspeed}]
        }
    }
    set per [format "%.1f" [expr double($total) * 100 / double($speed(OUTGOING)) ]]

    set output "$theme(PREFIX)$announce(TOTALDN)"
    set output [ReplaceVar $output "%count" $count]
    set output [ReplaceVar $output "%total" [FormatSpeed $total "none"]]
    set output [ReplaceVar $output "%per" $per]
    SendOne $chan [ReplaceBasic $output "LEECH"]
    return
}

proc ::dZSbot::CmdSpeed {nick uhost hand chan argv} {
    variable announce
    variable binary
    variable disable
    variable theme
    CheckChan $nick $chan

    set line ""
    if {$disable(ALTWHO) != 1} {
        set output "$theme(PREFIX)$announce(SPEEDERROR)"
        foreach line [split [exec $binary(WHO) --raw [lindex $argv 0]] "\n"] {
            set action [lindex $line 4]
            if {$action == "DN"} {
                set output "$theme(PREFIX)$announce(SPEEDDN)"
                set output [ReplaceVar $output "%dnspeed" [FormatSpeed [lindex $line 5] "none"]]
                set output [ReplaceVar $output "%dnpercent" [lindex $line 9]]
            } elseif {$action == "UP"} {
                set output "$theme(PREFIX)$announce(SPEEDUP)"
                set output [ReplaceVar $output "%upspeed" [FormatSpeed [lindex $line 5] "none"]]
                set output [ReplaceVar $output "%uppercent" [lindex $line 9]]
            } elseif {$action == "ID"} {
                set output "$theme(PREFIX)$announce(SPEEDID)"
                set output [ReplaceVar $output "%idletime" [FormatDuration [lindex $line 5]]]
            }
            set output [ReplaceVar $output "%u_name" [lindex $line 2]]
            set output [ReplaceVar $output "%g_name" [lindex $line 3]]
            set output [ReplaceVar $output "%tagline" [lindex $line 6]]
            set output [ReplaceVar $output "%timeonline" [lindex $line 7]]
            set output [ReplaceVar $output "%f_name" [lindex $line 8]]
            SendOne $chan [ReplaceBasic $output "SPEED"]
        }
    } else {
        set base_output "$theme(PREFIX)$announce(DEFAULT)"
        foreach line [split [exec $binary(WHO) [lindex $argv 0]] "\n"] {
            set output [ReplaceVar $base_output "%msg" $line]
            SendOne $chan [ReplaceBasic $output "SPEED"]
        }
    }

    if {[string equal "" $line]} {
        set output "$theme(PREFIX)$announce(SPEEDERROR)"
        set output [ReplaceVar $output "%msg" "User not online."]
        SendOne $chan [ReplaceBasic $output "SPEED"]
    }
    return
}

proc ::dZSbot::CmdUploaders {nick uhost hand chan argv} {
    variable announce
    variable binary
    variable speed
    variable theme
    CheckChan $nick $chan

    set output "$theme(PREFIX)$announce(UPLOAD)"
    SendOne $chan [ReplaceBasic $output "UPLOAD"]

    set raw [exec $binary(WHO) "--raw"]
    set count 0; set total 0.0

    foreach line [split $raw "\n"] {
        if {[string equal "USER" [lindex $line 0]] && [string equal "UP" [lindex $line 4]]} {
            set user  [lindex $line 2]
            set group [lindex $line 3]
            set uspeed [ReplaceVar [lindex $line 5] "KB/s" ""]
            set tagline [lindex $line 6]
            set since [lindex $line 7]
            set filename [lindex $line 8]
            set progress [lindex $line 9]
            set per [format "%.2f%%" [expr double($uspeed) * 100 / double($speed(INCOMING))]]
            set output [ReplaceVar "$theme(PREFIX)$announce(USERUP)" "%u_name" $user]
            set output [ReplaceVar $output "%g_name" $group]
            set output [ReplaceVar $output "%fper" $progress]
            set output [ReplaceVar $output "%speed" [FormatSpeed $uspeed "none"]]
            set output [ReplaceVar $output "%per" $per]
            set output [ReplaceVar $output "%tagline" $tagline]
            set output [ReplaceVar $output "%since" $since]
            set output [ReplaceVar $output "%filename" $filename]
            SendOne $chan [ReplaceBasic $output "UPLOAD"]
            incr count
            set total [expr {$total + $uspeed}]
        }
    }
    set per [format "%.1f" [expr double($total) * 100 / double($speed(INCOMING)) ]]

    set output "$theme(PREFIX)$announce(TOTALUP)"
    set output [ReplaceVar $output "%count" $count]
    set output [ReplaceVar $output "%total" [FormatSpeed $total "none"]]
    set output [ReplaceVar $output "%per" $per]
    SendOne $chan [ReplaceBasic $output "UPLOAD"]
    return
}

proc ::dZSbot::CmdWho {nick uhost hand chan argv} {
    variable binary
    CheckChan $nick $chan
    foreach line [split [exec $binary(WHO)] "\n"] {
        if {![info exists newline($line)]} {
            set newline($line) 0
        } else {
            incr newline($line)
        }
        puthelp "PRIVMSG $nick :$line[c]$newline($line)"
    }
    return
}

#################################################################################
# Theme Loading and Replacement                                                 #
#################################################################################

proc ::dZSbot::ThemeLoad {file} {
    variable announce
    variable random
    variable scriptPath
    variable theme
    variable theme_fakes
    variable variables
    unset -nocomplain announce random

    if {[string index $file 0] != "/"} {
        set file "$scriptPath/$file"
    }
    set announce(THEMEFILE) [file normalize $file]

    InfoMsg "Loading theme \"$file\"."

    if {[catch {set handle [open $file r]} error]} {
        ErrorMsg ThemeLoad "Unable to read the theme file ($error)."
        return 0
    }
    set data [read -nonewline $handle]
    close $handle

    foreach line [split $data "\n"] {
        if {[string equal "" $line] || [string index $line 0] == "#"} {continue}
        if {[regexp -- {(\S+)\.(\S+)\s*=\s*(['\"])(.+)\3} $line dud type setting quote value]} {
            switch -exact -- [string tolower $type] {
                "announce"    {set announce($setting) $value}
                "fakesection" {set theme_fakes($setting) $value}
                "random"      {set random($setting) $value}
                default       {WarningMsg ThemeLoad "Invalid theme setting \"$type.$setting\"."}
            }
        } elseif {[regexp -- {(\S+)\s*=\s*(['\"])(.*)\2} $line dud setting quote value]} {
            set theme($setting) $value
        }
    }

    foreach name [array names random] {
        if {![regexp {(.+)-\d+$} $name dud base]} {
            WarningMsg ThemeLoad "Invalid setting \"random.$name\", must be in the format of \"random.EVENT-#\"."
            unset random($name)
        } else {
            set announce($base) "random"
            set random($name) [ThemeReplaceBasic $random($name)]
        }
    }
    foreach name [array names announce] {set announce($name) [ThemeReplaceBasic $announce($name)]}
    foreach name [array names theme] {set theme($name) [ThemeReplaceBasic $theme($name)]}
    foreach name [array names theme_fakes] {set theme_fakes($name) [ThemeReplaceBasic $theme_fakes($name)]}

    ## Sanity checks
    foreach type {COLOR1 COLOR2 COLOR3 PREFIX KB KBIT MB MBIT} {
        if {[lsearch -exact [array names theme] $type] == -1} {
            ErrorMsg ThemeLoad "Missing required theme setting \"$type\", failing."
            return 0
        }
    }
    foreach type [concat [array names variables] NUKE UNNUKE NUKEES] {
        if {[lsearch -exact [array names announce] $type] == -1} {
            WarningMsg ThemeLoad "Missing announce setting \"announce.$type\" in the theme file."
        }
    }
    return 1
}

proc ::dZSbot::ThemePreview {handle idx text} {
    variable announce
    variable defaultsection

    if {[string equal "" $text]} {
        putdcc $idx "[b]Preview Usage:[b]"
        putdcc $idx "- .$::lastbind <event pattern>"
        putdcc $idx "- Only events matching the pattern are shown (* for all)."
        return
    }

    if {[catch {set handle [open $announce(THEMEFILE) r]} error]} {
        ErrorMsg ThemePreview "Unable to read the theme file ($error)."
        return
    }
    set data [read -nonewline $handle]
    close $handle

    putdcc $idx "[b]Previewing:[b] $announce(THEMEFILE)"
    putdcc $idx ""
    foreach line [split $data "\n"] {
        if {[string equal "" $line] || [string index $line 0] == "#"} {continue}
        ## TODO: Clean this mess up.
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
            set value [ThemeReplace [ReplaceBasic $value $defaultsection] $defaultsection]
            putdcc $idx "$prefix$setting = $value"
        }
    }
    return
}

proc ::dZSbot::ThemeReplace {targetString section} {
    variable theme

    ## Escape any "$" characters so they aren't interpreted as variables in the final "subst".
    set targetString [string map {$ \\$} $targetString]

    # We replace %cX{string}, %b{string} and %u{string} with their coloured, bolded and underlined equivilants ;)
    # We also do the justification and padding that is required for %r / %l / %m to work.
    # bold and underline replacement should not be needed here...
    while {[regexp {(%c(\d)\{([^\{\}]+)\}|%b\{([^\{\}]+)\}|%u\{([^\{\}]+)\}|%([lrm])(\d\d?)\{([^\{\}]+)\})} $targetString matchString dud padOp padLength padString]} {
        # Check if any innermost %r/%l/%m are present.
        while {[regexp {%([lrm])(\d\d?)\{([^\{\}]+)\}} $targetString matchString padOp padLength padString]} {
            set tmpPadString $padString
            regsub -all {\003\d\d} $tmpPadString {} tmpPadString
            set tmpPadString [string map {\002 "" \003 "" \037 ""} $tmpPadString]
            incr padLength [expr {[string length $padString] - [string length $tmpPadString]}]

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

        regsub -all {%b\{([^\{\}]+)\}} $targetString {\\002\1\\002} targetString
        regsub -all {%u\{([^\{\}]+)\}} $targetString {\\037\1\\037} targetString

        set colorString [format "COLOR_%s_1" $section]
        if {[lsearch -exact [array names theme] $colorString] != -1} {
            regsub -all {%c(\d)\{([^\{\}]+)\}} $targetString {\\003$theme([format "COLOR_%s_" $section]\1)\2\\003} targetString
            regsub {\003(\d)(?!\d)} $targetString {\\0030\1} targetString
        } else {
            regsub -all {%c(\d)\{([^\{\}]+)\}} $targetString {\\003$theme(COLOR\1)\2\\003} targetString
            regsub {\003(\d)(?!\d)} $targetString {\\0030\1} targetString
        }
    }
    return [subst -nocommands $targetString]
}

proc ::dZSbot::ThemeReplaceBasic {rstring} {
    # We replace %b{string} and %u{string} with their bolded and underlined equivilants ;)
    while {[regexp {(%b\{([^\{\}]+)\}|%u\{([^\{\}]+)\})} $rstring]} {
        regsub -all {%b\{([^\{\}]+)\}} $rstring {\\002\1\\002} rstring
        regsub -all {%u\{([^\{\}]+)\}} $rstring {\\037\1\\037} rstring
    }
    return [subst -nocommands -novariables $rstring]
}

#################################################################################
# Start Up Stuff                                                                #
#################################################################################

## TODO: Clean up initialization.
namespace eval ::dZSbot {
    variable glVersion 0
    variable loadError 0
    variable logList ""
    set context [namespace current]

    if {[catch {source [file join $scriptPath dZSbot.conf.defaults]} error]} {
        ErrorMsg SourceDefault "Unable to load dZSbot.conf.defaults ($error), cannot continue."
        ErrorMsg SourceDefault "See FAQ for possible solutions/debugging options."
        die
    }

    if {[catch {source [file join $scriptPath dZSbot.vars]} error]} {
        ErrorMsg SourceVars "Unable to load dZSbot.vars ($error), cannot continue."
        ErrorMsg SourceVars "See FAQ for possible solutions/debugging options."
        die
    }

    if {[catch {source [file join $scriptPath dZSbot.conf]} error]} {
        WarningMsg SourceConf "Unable to load dZSbot.conf, using default values."
        if {[file exists  [file join $scriptPath dZSbot.conf]]} {
            WarningMsg SourceConf "Config error:\n$errorInfo"
        } else {
            WarningMsg SourceConf "Since this is your first install, do: cp dZSbot.conf.dist dZSbot.conf"
        }
        set loadError 1
    }

    ## Check binary and file locations
    foreach {filename filepath} [array get binary] {
        if {![IsTrue $bnc(ENABLED)]} {
            if {[string equal "CURL" $filename]} {continue}
            if {[string equal "PING" $filename] && ![IsTrue $bnc(PING)]} {continue}
        }
        if {![IsTrue $enable_irc_invite] && [string equal "PASSCHK" $filename]} {continue}

        if {![file executable $filepath]} {
            ErrorMsg BinaryCheck "Invalid path/permissions for $filename ($filepath)."
            set loadError 1
        }
    }

    foreach {filename filepath} [array get location] {
        if {![file exists $filepath]} {
            ErrorMsg LocationCheck "Invalid path for for $filename ($filepath)."
            set loadError 1
        }
    }

    ## Logs to parse.
    set logId 0
    foreach logType {0 1 2} varName {glftpdlog loginlog sysoplog} {
        foreach {filename filepath} [array get $varName] {
            if {![file readable $filepath]} {
                ErrorMsg LogCheck "Unable to read the log file \"$filepath\"."
                set loadError 1
            } else {
                lappend logList $logType [incr logId] [file normalize $filepath]
                set lastread($logId) [file size $filepath]
            }
        }
    }
    if {!$logId} {
        ErrorMsg LogCheck "No logs found!"
        set loadError 1
    } else {
        InfoMsg "Logs found: $logId"
    }

    ## Detect glftpd version.
    if {[string equal -nocase "AUTO" $use_glftpd2]} {
        if {![info exists binary(GLFTPD)]} {
            ErrorMsg GlVersion "You did not thoroughly edit the [file join $scriptPath dZSbot.conf] file (hint: binary(GLFTPD))."
            die
        }
        set glVersion [exec strings $binary(GLFTPD) | grep -i "^glftpd " | cut -f1 -d. | tr A-Z a-z]

        if {[string equal "glftpd 1" $glVersion]} {
            set glVersion 1
        } elseif {[string equal "glftpd 2" $glVersion]} {
            set glVersion 2
        } else {
            ErrorMsg GlVersion "Version auto-detection failed, set \"use_glftpd2\" in [file join $scriptPath dZSbot.conf] manually."
            die
        }
    } else {
        set glVersion [expr {[IsTrue $use_glftpd2] ? 2 : 1}]
    }
    InfoMsg "glFTPD version: $glVersion"

    ## Bind commands and events.
    BindCommands $cmdpre
    if {[IsTrue $bindnopre]} {
        BindCommands "!"
    } elseif {![string equal "!" $cmdpre]} {
        UnbindCommands "!"
    }
    if {[IsTrue $enable_irc_invite]} {
        bind msg -|- !invite ${context}::CmdInvite
    }
    bind dcc n errorinfo ${context}::ShowError
    bind dcc n preview   ${context}::ThemePreview
    bind join -|- *      ${context}::ShowWelcome

    ## Load theme file.
    if {![ThemeLoad $announce(THEMEFILE)]} {
        if {[ThemeLoad "themes/default.zst"]} {
            WarningMsg Theme "Unable to load theme $announce(THEMEFILE), loaded default.zst instead."
        } else {
            ErrorMsg Theme "Unable to load the themes $announce(THEMEFILE) and default.zst."
            set loadError 1
        }
    }

    ## Start log timer.
    if {[info exists logTimerId] && [catch {killutimer $logTimerId} error]} {
        WarningMsg StartTimer "Unable to kill log timer, restart the bot to be safe ($error)."
    }
    variable logTimerId [utimer 1 ${context}::LogTimer]

    foreach rep [array names msgreplace] {
        set rep [split $msgreplace($rep) ":"]
        set variables([lindex $rep 2]) $variables([lindex $rep 0])
        if {![info exists disable([lindex $rep 2])]} {
            set disable([lindex $rep 2]) 0
        }
        if {![info exists announce([lindex $rep 2])]} {
            set announce([lindex $rep 2]) $announce([lindex $rep 0])
            WarningMsg MsgReplace "Custom message [lindex $rep 2] defined, but no announce definition found. Using same announce as [lindex $rep 0] for now."
        }
    }

    if {!$loadError} {
        InfoMsg "Loaded successfully!"
    } else {
        InfoMsg "Errors were encountered while loading, refer to the messages above for details."
        if {[IsTrue $die_on_error]} {die}
    }
}
