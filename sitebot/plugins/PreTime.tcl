#################################################################################
# dZSbot - Pre Time Plug-in                                                     #
#################################################################################
#
# Description:
# - Announce the pre time of a release on NEWDIR.
# - If a pre time can't be found on lookup, the regular NEWDIR announce will be shown.
# - This example uses a MySQL server, you may have to change it for your database.
#
# Installation:
# 1. Customize the PreTime::LookUp function for your pre database.
#
# 2. Edit the configuration options below.
#
# 3. Add the following to your eggdrop.conf:
#    source pzs-ng/plugins/PreTime.tcl
#
# 4. Rehash or restart your eggdrop for the changes to take effect.
#
#################################################################################

namespace eval ::ngBot::PreTime {
    global disable

    variable mysql

    ## Config Settings ###############################
    ##
    ## If a pre time is older than the defined amount, it is
    ## considered old and OLDPRETIME is announced instead of NEWPRETIME.
    variable lateMins 10
    ##
    ## Skip the pre time for these directories.
    variable ignoreDirs {cd[0-9] dis[ck][0-9] dvd[0-9] codec cover covers extra extras sample subs vobsub vobsubs}
    ##
    ## MySQL server information.
    set mysql(host)  "127.0.0.1"
    set mysql(port)  3306
    set mysql(user)  "preuser"
    set mysql(pass)  "prepass"
    set mysql(db)    "predb"
    set mysql(table) "pretable"
    ##
    ## Path to the MySQLTcl v3.0 library.
    variable libMySQLTcl "/usr/local/lib/tcl8.4/mysqltcl/mysqltcl3.so"
    ##
    ## Disable announces. (0 = No, 1 = Yes)
    set disable(NEWPRETIME) 0
    set disable(OLDPRETIME) 0
    ##
    ##################################################

    set mysql(handle) ""
    variable scriptFile [info script]
    variable scriptName [namespace current]::LogEvent
    bind evnt -|- prerehash [namespace current]::DeInit
}

####
# PreTime::Init
#
# Called on initialization; registers the event handler.
#
proc ::ngBot::PreTime::Init {args} {
    global precommand variables

    variable libMySQLTcl
    variable mysql
    variable scriptName
    variable scriptFile

    set variables(NEWPRETIME) "%pf %u_name %g_name %u_tagline %preage %predate %pretime"
    set variables(OLDPRETIME) "%pf %u_name %g_name %u_tagline %preage %predate %pretime"

    set theme_file [file normalize "[pwd]/[file rootname $scriptFile].zpt"]
    if {[file isfile $theme_file]} {
        loadtheme $theme_file true
    }

    ## Load the MySQLTcl library.
    if {[catch {load $libMySQLTcl Mysqltcl} errorMsg]} {
        putlog "\[ngBot\] PreTime :: $errorMsg"
        return
    }

    ## Connect to the MySQL server.
    if {[catch {set mysql(handle) [mysqlconnect -host $mysql(host) -user $mysql(user) -password $mysql(pass) -port $mysql(port) -db $mysql(db)]} errorMsg]} {
        putlog "\[ngBot\] PreTime :: Unable to connect to MySQL server: $errorMsg"
        return
    }

    ## Register the event handler.
    lappend precommand(NEWDIR) $scriptName

    putlog "\[ngBot\] PreTime :: Loaded successfully."
    return
}

####
# PreTime::DeInit
#
# Called on rehash; unregisters the event handler.
#
proc ::ngBot::PreTime::DeInit {args} {
    global precommand
    variable mysql
    variable scriptName

    ## Close the MySQL connection.
    catch {mysqlclose $mysql(handle)}

    ## Remove the script event from precommand.
    if {[info exists precommand(NEWDIR)] && [set pos [lsearch -exact $precommand(NEWDIR) $scriptName]] !=  -1} {
        set precommand(NEWDIR) [lreplace $precommand(NEWDIR) $pos $pos]
    }

    catch {unbind evnt -|- prerehash [namespace current]::DeInit}

    namespace delete [namespace current]
    return
}

####
# PreTime::LookUp
#
# Look up the pre time of the release. Returns 1 if
# found and 0 if not.
#
proc ::ngBot::PreTime::LookUp {release timeVar} {

    ## Note ##########################################
    ##
    ## - You will have to customize this function to
    ##   work properly with your MySQL database/table.
    ##
    return -code error "PreTime::LookUp - Not implemented."
    ##
    ##################################################

    variable mysql
    upvar $timeVar preTime

    ## Query the database for the release name.
    set release [mysqlescape $release]
    set preTime [mysqlsel $mysql(handle) "SELECT timestamp FROM $mysql(table) WHERE release='$release' LIMIT 1" -flatlist]

    ## The SELECT statement will return an empty string if no match is found.
    return [string is digit -strict $preTime]
}

####
# PreTime::LogEvent
#
# Called by the sitebot's event handler on the "NEWDIR" announce.
#
proc ::ngBot::PreTime::LogEvent {event section logData} {
    variable lateMins
    variable ignoreDirs
    if {![string equal "NEWDIR" $event]} {return 1}

    ## Log Data:
    ## NEWDIR - path user group tagline
    set release [file tail [lindex $logData 0]]

    ## Check the release directory is ignored.
    foreach ignore $ignoreDirs {
        if {[string match -nocase $ignore $release]} {return 1}
    }

    if {[LookUp $release preTime]} {
        ## Check if the pre is older than the defined time.
        set preAge [expr {[clock seconds] - $preTime}]
        if {$preAge > $lateMins * 60} {
            set event "OLDPRETIME"
        } else {
            set event "NEWPRETIME"
        }

        ## Format the pre time and append it to the log data.
        set formatDate [clock format $preTime -format "%m/%d/%y"]
        set formatTime [clock format $preTime -format "%H:%M:%S"]
        lappend logData [format_duration $preAge] $formatDate $formatTime

        ## We'll announce the event ourself since we'll return zero
        ## to cancel the regular NEWDIR announce.
        sndall $event $section [ng_format $event $section $logData]
        return 0
    }

    return 1
}

::ngBot::PreTime::Init
