#################################################################################
# dZSbot - Ban on Deluser Plug-in                                               #
#################################################################################
#
# Description:
# - Automatically kills or kick/bans a user from all channels when they are deleted/purged.
# - This plug-in requires the NickDb plug-in and sysop.log reading must be enabled.
# - Thanks to compieter for the idea.
#
# Installation:
# 1. Load the NickDb plugin, refer to NickDb.tcl for instructions.
#
# 2. Edit the configuration options below.
#
# 3. Add the following to your eggdrop.conf (load AFTER NickDb):
#    source pzs-ng/plugins/DeluserBan.tcl
#
# 4. Rehash or restart your eggdrop for the changes to take effect.
#
#################################################################################

namespace eval ::dZSBot::DeluserBan {

    ## Config Settings ###############################
    ##
    ## The ban/kill message, cookies: %(user).
    variable message "you.have.been.deleted.by.%(user)"
    ##
    ## By default the user is only kicked; if you would like
    ## them banned as well, enable this option.
    variable banUser True
    ##
    ## Kill the user, if the kill fails they will still be kicked.
    ## The eggdrop must be an IRCOp on the IRCd.
    variable killUser True
    ##
    ##################################################

    namespace import -force ::dZSBot::NickDb::*
    variable scriptName [namespace current]::LogEvent
    bind evnt -|- prerehash [namespace current]::DeInit
}

interp alias {} IsTrue {} string is true -strict
interp alias {} IsFalse {} string is false -strict

####
# DeluserBan::Init
#
# Called on initialization; registers the event handler.
#
proc ::dZSBot::DeluserBan::Init {args} {
    global precommand
    variable scriptName

    ## Register the event handler.
    lappend precommand(DELUSER) $scriptName
    lappend precommand(PURGED) $scriptName

    putlog "\[dZSBot\] DeluserBan :: Loaded successfully."
    return
}

####
# DeluserBan::DeInit
#
# Called on rehash; unregisters the event handler.
#
proc ::dZSBot::DeluserBan::DeInit {args} {
    global precommand
    variable scriptName

    ## Remove the script events from precommand.
    foreach type {DELUSER PURGED} {
        if {[info exists precommand($type)] && [set pos [lsearch -exact $precommand($type) $scriptName]] !=  -1} {
            set precommand($type) [lreplace $precommand($type) $pos $pos]
        }
    }

    catch {unbind evnt -|- prerehash [namespace current]::DeInit}

    namespace delete [namespace current]
    return
}

####
# DeluserBan::InviteEvent
#
# Called by the sitebot's event handler on the
# "DELUSER" and "PURGED" announces.
#
proc ::dZSBot::DeluserBan::LogEvent {event section logData} {
    global botnick
    variable banUser
    variable killUser
    variable message
    if {![string equal "DELUSER" $event] && ![string equal "PURGED" $event]} {return 1}

    ## Log Data:
    ## DELUSER - siteop user
    ## PURGED  - siteop user
    foreach {ftpSiteop ftpUser} $logData {break}

    ## Retrieve the IRC user name.
    set ircUser [GetIrcUser $ftpUser]
    if {[string equal "" $ircUser]} {
        putlog "\[dZSBot\] DeluserBan :: Unable to retrieve the IRC user for \"$ftpUser\", you will have to kick them manually."
        return 1
    }

    set reason [string map [list %(user) $ftpSiteop] $message]

    ## Kill the user, die you bastard!
    if {[IsTrue $killUser]} {
        putlog "\[dZSBot\] DeluserBan :: Killing IRC user \"$ircUser\"."
        putquick "KILL $ircUser :$reason"
    }

    ## Create a ban mask for the user.
    set userHost [getchanhost $ircUser]
    if {[string equal "" $userHost]} {
        set userHost "$ircUser!*@*"
    }

    ## Kick/ban the user from all channels.
    putlog "\[dZSBot\] DeluserBan :: Kicking/banning IRC user \"$ircUser\" from all channels."
    foreach channel [channels] {
        if {[botisop $channel] && [onchan $ircUser $channel]} {
            putkick $channel $ircUser $reason
        }
        if {[IsTrue $banUser]} {
            newchanban $channel $userHost $botnick $reason
        }
    }

    return 1
}

::dZSBot::DeluserBan::Init
