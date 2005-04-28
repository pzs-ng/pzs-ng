#################################################################################
# dZSbot - NickDb Plug-in                                                       #
#################################################################################
#
# Description:
# - Keeps track of IRC nick names and their corresponding FTP user name.
# - Remember, this module is only effective if the channel is invite-only.
# - Thanks to compieter for the idea.
#
# Installation:
# 1. Compile and install the SQLite v3.2.x Tcl binding:
#    http://www.sqlite.org/download.html
#
#    tar zxf sqlite-3.2.x.tar.gz
#    cd sqlite-3.2.x
#    mkdir bld
#    cd bld
#    ../configure
#    FreeBSD users may have to specify the location of tclConfig.sh:
#    ../configure --with-tcl=/usr/local/lib/tcl8.4/
#    make
#    make install
#
# 2. Edit the configuration options below.
#
# 3. Add the following to your eggdrop.conf AFTER dZSbot.tcl:
#    source pzs-ng/plugins/NickDb.tcl
#
# 4. Make sure the bot has read/write access to the pzs-ng/plugins/ directory.
#
# 5. Rehash or restart your eggdrop for the changes to take effect.
#
#################################################################################

namespace eval ::ngBot::NickDb {

    ## Config Settings ###############################
    ##
    ## Change host masks of users on invite.
    ## The eggdrop must be an IRCOp on an UnrealIRCd (or CHGHOST compatible) daemon.
    variable hostChange True
    ##
    ## Host mask format, cookies: %(user) and %(group).
    variable hostFormat "%(user).Users.PZS-NG.com"
    ##
    ## Exempt users from the host change (uses glFTPd-style permissions).
    variable hostExempt "=STAFF =SiTEOPS 1"
    ##
    ## Monitor user events in these channels, usually the invite channels.
    variable monitorChans "#NG #NG-Spam"
    ## or if you're lazy:
    ##variable monitorChans $::invite_channels
    ##
    ## Path to the Tcl binding for SQLite v3.1.
    variable libSQLite "/usr/local/lib/tcl8.4/sqlite3/libtclsqlite3.so"
    ##
    ##################################################

    namespace export GetFtpUser GetIrcUser IsFtpUserOnline IsIrcUserOnline
    variable filePath   [file join [file dirname [info script]] "Nicks.db"]
    variable scriptName [namespace current]::InviteEvent

    bind evnt -|- prerehash [namespace current]::DeInit
}

interp alias {} IsTrue {} string is true -strict
interp alias {} IsFalse {} string is false -strict

####
# NickDb::Init
#
# Called on initialization. This procedure opens the user database and
# registers the INVITEUSER script and Eggdrop event callbacks.
#
proc ::ngBot::NickDb::Init {args} {
    global postcommand
    variable filePath
    variable libSQLite
    variable scriptName

    ## Load the Tcl SQLite library.
    if {[catch {load $libSQLite Tclsqlite3} errorMsg]} {
        putlog "\[ngBot\] NickDb :: $errorMsg"
        return
    }
    if {[catch {sqlite3 [namespace current]::db $filePath} errorMsg]} {
        putlog "\[ngBot\] NickDb :: Unable to open database \"$filePath\" - $errorMsg"
        return
    }

    ## The second table revision introduces the 'online' column.
    if {![db eval {SELECT count(*) FROM sqlite_master WHERE name='UserNames' AND type='table'}]} {
        db eval {CREATE TABLE UserNames (time INT, online INT, ircUser TEXT, ftpUser TEXT UNIQUE)}
        db eval {PRAGMA user_version = 2}
    } elseif {[set version [db eval {PRAGMA user_version}]] != 2} {
        putlog "\[ngBot\] NickDb :: Unsupported database version ($version), please remove \"$filePath\"."
        db close
        return
    }
    db function StrCaseEq {string equal -nocase}

    ## Register the invite handler and event callbacks.
    bind join -|- "*" [list [namespace current]::ChanEvent join]
    bind kick -|- "*" [list [namespace current]::ChanEvent kick]
    bind nick -|- "*" [list [namespace current]::ChanEvent nick]
    bind part -|- "*" [list [namespace current]::ChanEvent part]
    bind sign -|- "*" [list [namespace current]::ChanEvent quit]
    lappend postcommand(INVITEUSER) $scriptName

    putlog "\[ngBot\] NickDb :: Loaded successfully."
    return
}

####
# NickDb::DeInit
#
# Called on rehash. This procedure closes the user database and unregisters
# the INVITEUSER script and Eggdrop event callbacks.
#
proc ::ngBot::NickDb::DeInit {args} {
    global postcommand
    variable scriptName

    ## Close the SQLite database in case we're being unloaded.
    catch {db close}

    ## Remove the script event from postcommand.
    if {[info exists postcommand(INVITEUSER)] && [set pos [lsearch -exact $postcommand(INVITEUSER) $scriptName]] !=  -1} {
        set postcommand(INVITEUSER) [lreplace $postcommand(INVITEUSER) $pos $pos]
    }

    catch {unbind evnt -|- prerehash [namespace current]::DeInit}
    catch {unbind join -|- "*" [list [namespace current]::ChanEvent join]}
    catch {unbind kick -|- "*" [list [namespace current]::ChanEvent kick]}
    catch {unbind nick -|- "*" [list [namespace current]::ChanEvent nick]}
    catch {unbind part -|- "*" [list [namespace current]::ChanEvent part]}
    catch {unbind sign -|- "*" [list [namespace current]::ChanEvent quit]}

    namespace delete [namespace current]
    return
}

####
# NickDb::ChanEvent
#
# Updates the IRC nickname and online status of users.
#
proc ::ngBot::NickDb::ChanEvent {event args} {
    variable monitorChans

    switch -exact -- $event {
        "nick" {
            foreach {oldNick host handle channel nick} $args {break}
            set online 1
        }
        "join" {
            foreach {nick host handle channel} $args {break}
            set online 1
        }
        "kick" {
            foreach {kicker host handle channel nick reason} $args {break}
            set online [OnMonitorChannels $nick $channel]
        }
        "quit" - "part" {
            foreach {nick host handle channel message} $args {break}
            set online [OnMonitorChannels $nick $channel]
        }
        default {return}
    }

    ## Ignore any channels which are not defined in $monitorChans.
    set valid 0
    foreach entry $monitorChans {
        if {[string equal -nocase $entry $channel]} {
            set valid 1; break
        }
    }
    if {!$valid} {return}

    set time [clock seconds]
    if {[string equal "nick" $event]} {
        db eval {UPDATE UserNames SET time=$time, online=$online, ircUser=$nick WHERE StrCaseEq(ircUser,$oldNick)}
    } else {
        db eval {UPDATE UserNames SET time=$time, online=$online WHERE StrCaseEq(ircUser,$nick)}
    }
    return
}

####
# NickDb::InviteEvent
#
# Called by the sitebot's event handler on the "INVITEUSER" event.
#
proc ::ngBot::NickDb::InviteEvent {event ircUser ftpUser ftpGroup ftpFlags} {
    variable hostChange
    variable hostExempt
    variable hostFormat
    if {![string equal "INVITEUSER" $event]} {return 1}

    if {[IsTrue $hostChange] && ![rightscheck $hostExempt $ftpUser $ftpGroup $ftpFlags]} {
        ## glFTPD allows characters in user names which are not allowed on IRC.
        set stripUser [StripName $ftpUser]
        set stripGroup [StripName $ftpGroup]

        ## Change the user's host.
        set host [string map [list %(user) $stripUser %(group) $stripGroup] $hostFormat]
        putquick "CHGHOST $ircUser :$host"
    }

    ## Update the user name database.
    set time [clock seconds]
    set online [OnMonitorChannels $ircUser]
    db eval {INSERT OR REPLACE INTO UserNames (time,online,ircUser,ftpUser) values($time,$online,$ircUser,$ftpUser)}

    return 1
}

####
# NickDb::OnMonitorChannels
#
# Checks if the specified user is currently in any of the monitor channels.
#
proc ::ngBot::NickDb::OnMonitorChannels {ircUser {ignoreChannel ""}} {
    variable monitorChans
    foreach channel $monitorChans {
        if {[string equal -nocase $ignoreChannel $channel]} {continue}
        if {[validchan $channel] && [onchan $ircUser $channel]} {return 1}
    }
    return 0
}

####
# NickDb::StripName
#
# Strips illegal characters from IRC user names.
#
proc ::ngBot::NickDb::StripName {name} {
    return [regsub -all {[^\w\[\]\{\}\-\`\^\\]+} $name {}]
}

####
# NickDb::GetIrcUser (exported)
#
# Retrieve the IRC user name for the specified FTP user. If there is no record
# of the user, an empty string is returned.
#
# Example:
# set ircUser [GetIrcUser $ftpUser]
# if {$ircUser eq ""} {
#     puts "No IRC user found for \"$ircUser\"."
# } else {
#     puts "The IRC user nick for \"$ftpUser\" is \"$ircUser\"."
# }
#
proc ::ngBot::NickDb::GetIrcUser {ftpUser} {
    return [db eval {SELECT ircUser FROM UserNames WHERE ftpUser=$ftpUser}]
}

####
# NickDb::GetFtpUser (exported)
#
# Retrieve the FTP user name for the specified IRC user. If there is no record
# of the user, an empty string is returned.
#
# Example:
#
# set ftpUser [GetFtpUser $ircUser]
# if {$ftpUser eq ""} {
#     puts "No FTP user found for \"$ircUser\"."
# } else {
#     puts "The FTP user name for \"$ircUser\" is \"$ftpUser\"."
# }
#
proc ::ngBot::NickDb::GetFtpUser {ircUser} {
    ## IRC user names are case-insensitive.
    return [db eval {SELECT ftpUser FROM UserNames WHERE StrCaseEq(ircUser,$ircUser) ORDER BY time DESC LIMIT 1}]
}

####
# NickDb::QueryFtpUser (exported)
#
# Query the database for the specified FTP user. If a match is found, the
# information will be stored in the array specified for varName and the return
# value is non-zero. If no matches are found, the return value is zero.
#
# Example:
# if {[QueryFtpUser $ftpUser info]} {
#     set lastSeen [expr {$info(online) != 1 ? [clock format $info(time)] : "now"}]
#     puts "The FTP user $ftpUser was last seen online: $lastSeen"
# } else {
#     puts "The FTP user $ftpUser was never seen online."
# }
#
proc ::ngBot::NickDb::QueryFtpUser {ftpUser varName} {
    upvar $varName values
    db eval {SELECT * FROM UserNames WHERE ftpUser=$ftpUser} values {
        return 1
    }
    return 0
}

####
# NickDb::QueryIrcUser (exported)
#
# Query the database for the specified IRC user. If a match is found, the
# information will be stored in the array specified for varName and the return
# value is non-zero. If no matches are found, the return value is zero.
#
# Example:
# if {[QueryIrcUser $ircUser info]} {
#     set lastSeen [expr {$info(online) != 1 ? [clock format $info(time)] : "now"}]
#     puts "The IRC user $ircUser was last seen online: $lastSeen"
# } else {
#     puts "The IRC user $ircUser was never seen online."
# }
#
proc ::ngBot::NickDb::QueryIrcUser {ircUser varName} {
    upvar $varName values
    ## IRC user names are case-insensitive.
    db eval {SELECT * FROM UserNames WHERE StrCaseEq(ircUser,$ircUser)} values {
        return 1
    }
    return 0
}

::ngBot::NickDb::Init
