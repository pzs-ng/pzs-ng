#################################################################################
# dZSbot - AutoOp Users on their flags                                          #
#################################################################################
#
# Author: Pee
# 
# Description:
# - You need to have NickDb.tcl loaded.
# - Based on Whois.tcl by Compieter & E-Liquid
# - Will give op to ppl depending on their flags
#
# Installation:
# 1. Add the following to your eggdrop.conf:
#    source pzs-ng/plugins/AutoOp.tcl
#
# 2. Add the following lines to your dZSbot.conf:
#    set variables(WHOIS)   "%msg"
#    set redirect(WHOIS)    $staffchan
#    set disable(WHOIS)     0
#
#    and add "WHOIS" at the end of your msgtypes(DEFAULT) line.
#
# 4. Add the following line to your theme file:
#    announce.WHOIS = "%b{[WHOIS]} %msg"
#
# 5. Rehash or restart your eggdrop for the changes to take effect.
#
#################################################################################

namespace eval ::ngBot::AutoOp {

    ## Config Settings ###############################
    ##
    ## Permissions! Who get's opped in chan?
    ## Leave the default to op siteops, nukers and users with flag +J
    variable permsAutoOp "1 A"
    ##################################################

    namespace import ::ngBot::NickDb::*
    bind evnt -|- prerehash [namespace current]::DeInit
}

interp alias {} IsTrue {} string is true -strict
interp alias {} IsFalse {} string is false -strict

####
# AutoOp::Init
#
# Called on initialization; registers the event handler. Yeah, nothing fancy.
#
proc ::ngBot::AutoOp::Init {args} {
    ## Bind event callbacks.
    bind join -|- * [namespace current]::GiveOp
    putlog "\[ngBot\] AutoOp :: Loaded successfully."
    return
}

####
# AutoOp::DeInit
#
# Called on rehash; unregisters the event handler.
#
proc ::ngBot::AutoOp::DeInit {args} {

    ## Remove event callbacks.
    catch {unbind evnt -|- prerehash [namespace current]::DeInit}
    catch {unbind join -|- *!*@* [namespace current]::GiveOp}
    
    namespace delete [namespace current]
    return
}

####
# AutoOp::GetInfo
#
# gets $group and $flags from the userfile
#
proc ::ngBot::AutoOp::GetInfo {ftpUser groupVar flagsVar} {
    global location
    upvar $groupVar group $flagsVar flags
    set group ""; set flags ""

    if {![catch {set handle [open "$location(USERS)/$ftpUser" r]} error]} {
        set data [read $handle]
        close $handle
        foreach line [split $data "\n"] {
            switch -exact -- [lindex $line 0] {
                "FLAGS" {set flags [lindex $line 1]}
                "GROUP" {set group [lindex $line 1]}
            }
        }
	return 1
    } else {
        putlog "dZSbot error: Unable to open user file for \"$ftpUser\" ($error)."
        return 0
    }
}

####
# AutoOp::GiveOp
#
# Op IRC users depending on their ftp flags
#
proc ::ngBot::AutoOp::GiveOp {nick host handle channel} {
    variable permsAutoOp
    
    set ftpUser [GetFtpUser $nick]
 
    if {[GetInfo $ftpUser group flags]} {
        if {[rightscheck $permsAutoOp $ftpUser $group $flags]} {
		pushmode $channel +o $nick
		putlog "Gave OP to $nick ($ftpUser) in $channel"
        }
    }

}

::ngBot::AutoOp::Init
