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
# 1. Compile and install the SQLite v3.1.x Tcl binding:
#    http://www.sqlite.org/download.html
#
#    tar zxf sqlite-3.1.x.tar.gz
#    cd sqlite-3.1.x
#    mkdir bld
#    cd bld
#    ../configure
#    Users may have to specify the path to tclConfig.sh:
#    ../configure --with-tcl=/usr/local/lib/tcl8.4/
#    make
#    make install
#
# 2. Edit the configuration options below.
#
# 3. Add the following to your eggdrop.conf:
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
    ## Path to the Tcl binding for SQLite v3.1.
    variable libSQLite "/usr/local/lib/tcl8.4/sqlite3/libtclsqlite3.so"
    ##
    ##################################################

    namespace export GetFtpUser GetIrcUser
    variable filePath   [file join [file dirname [info script]] "Nicks.db"]
    variable scriptName [namespace current]::InviteEvent
    bind evnt -|- prerehash [namespace current]::DeInit
    bind nick -|- "*" [namespace current]::NickChange
}

interp alias {} IsTrue {} string is true -strict
interp alias {} IsFalse {} string is false -strict

####
# NickDb::Init
#
# Called on initialization; opens the database and
# registers the event handler.
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

    ## The event handler will only be registered if the SQLite
    ## database is opened successfully.
    if {[catch {sqlite3 db $filePath} errorMsg]} {
        putlog "\[ngBot\] NickDb :: Unable to open database \"$filePath\" - $errorMsg"
        return
    }

    ## Create the Ftp table
    if {![db eval {SELECT count(*) FROM sqlite_master WHERE name='UserNames' AND type='table'}]} {
        db eval {CREATE TABLE UserNames (time INT, ircUser TEXT, ftpUser TEXT UNIQUE)}
    }
    db function StrCaseEq {string equal -nocase}

    ## Register the event handler.
    lappend postcommand(INVITEUSER) $scriptName

    putlog "\[ngBot\] NickDb :: Loaded successfully."
    return
}

####
# NickDb::DeInit
#
# Called on rehash; closes the database and
# unregisters the event handler.
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
    catch {unbind nick -|- "*" [namespace current]::NickChange}

    namespace delete [namespace current]
    return
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
# NickDb::InviteEvent
#
# Called by the sitebot's event handler on
# the "INVITEUSER" event.
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
    db eval {INSERT OR REPLACE INTO UserNames (time,ircUser,ftpUser) values($time,$ircUser,$ftpUser)}

    return 1
}

####
# NickDb::NickChange
#
# Update the ircNick column when a user changes their nickname.
#
proc ::ngBot::NickDb::NickChange {nick host handle channel newNick} {
    db eval {UPDATE UserNames SET ircUser=$newNick WHERE ircUser=$nick}
    return
}

####
# NickDb::GetIrcUser
#
# Retrieve the IRC user name for the specified FTP user. If
# no matches are found, an empty string is returned.
#
proc ::ngBot::NickDb::GetIrcUser {ftpUser} {
    ## Since the ftpUser column is unique, the query is simplier.
    return [db eval {SELECT ircUser FROM UserNames WHERE ftpUser=$ftpUser}]
}

####
# NickDb::GetFtpUser
#
# Retrieve the FTP user name for the specified IRC user. If
# no matches are found, an empty string is returned.
#
proc ::ngBot::NickDb::GetFtpUser {ircUser} {
    ## IRC user names are case-insensitive.
    return [db eval {SELECT ftpUser FROM UserNames WHERE StrCaseEq(ircUser,$ircUser) ORDER BY time DESC LIMIT 1}]
}

::ngBot::NickDb::Init
