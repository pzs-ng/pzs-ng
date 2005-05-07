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
# 3. Load this script in eggdrop.conf after dZSbot.tcl and NickDb.tcl, for example:
#    source pzs-ng/dZSbot.tcl
#    source pzs-ng/plugins/NickDb.tcl
#    source pzs-ng/plugins/DeluserBan.tcl
#
# 4. Rehash or restart your eggdrop for the changes to take effect.
#
#################################################################################

namespace eval ::dZSbot::DeluserBan {

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

    namespace import -force ::dZSbot::*
    namespace import -force ::dZSbot::NickDb::*
    variable scriptName [namespace current]::LogEvent
    bind evnt -|- prerehash [namespace current]::DeInit
}

####
# DeluserBan::Init
#
# Called on initialization; registers the event handler.
#
proc ::dZSbot::DeluserBan::Init {args} {
    variable scriptName

    ## Register event handler.
    EventRegister precommand DELUSER $scriptName
    EventRegister precommand PURGED $scriptName

    InfoMsg "DeluserBan - Loaded successfully."
    return
}

####
# DeluserBan::DeInit
#
# Called on rehash; unregisters the event handler.
#
proc ::dZSbot::DeluserBan::DeInit {args} {
    variable scriptName

    ## Remove script events and callbacks.
    EventRegister precommand DELUSER $scriptName
    EventRegister precommand PURGED $scriptName
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
proc ::dZSbot::DeluserBan::LogEvent {event section sectionPath logData} {
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
        ErrorMsg DeluserBan "Unable to retrieve the IRC user for \"$ftpUser\", you will have to kick them manually."
        return 1
    }

    set reason [string map [list %(user) $ftpSiteop] $message]

    ## Kill the user, die you bastard!
    if {[IsTrue $killUser]} {
        InfoMsg "DeluserBan - Killing IRC user \"$ircUser\"."
        putquick "KILL $ircUser :$reason"
    }

    ## Create a ban mask for the user.
    set userHost [getchanhost $ircUser]
    if {[string equal "" $userHost]} {
        set userHost "$ircUser!*@*"
    }

    ## Kick/ban the user from all channels.
    InfoMsg "DeluserBan - Kicking/banning IRC user \"$ircUser\" from all channels."
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

::dZSbot::DeluserBan::Init
