#################################################################################
# ngBot - AutoOp Users on their flags                                           #
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
# 2. Add the following lines to your ngBot.conf:
#    (NOTE BY PSXC: This is already done)
#    set variables(WHOIS)   "%msg"
#    set redirect(WHOIS)    $staffchan
#    set disable(WHOIS)     0
#
#    and add "WHOIS" at the end of your msgtypes(DEFAULT) line.
#    (NOTE BY PSXC: This is already done)
#
# 4. Add the following line to your theme file:
#    (NOTE BY PSXC: This is already done)
#    announce.WHOIS = "%b{[WHOIS]} %msg"
#
# 5. Rehash or restart your eggdrop for the changes to take effect.
#
#################################################################################

namespace eval ::ngBot::plugin::AutoOp {
    variable ns [namespace current]
    variable np [namespace qualifiers [namespace parent]]

    ## Config Settings ###############################
    ##
    ## Permissions! Who get's opped in chan?
    ## Leave the default to op siteops, nukers and users with flag +J
    variable permsAutoOp "1 A"
    ##################################################

    #bind evnt -|- prerehash [namespace current]::deinit

    #interp alias {} IsTrue {} string is true -strict
    #interp alias {} IsFalse {} string is false -strict

    ####
    # AutoOp::Init
    #
    # Called on initialization; registers the event handler. Yeah, nothing fancy.
    #
    proc init {args} {
        if {![namespace exists ::ngBot::plugin::NickDb]} {
            putlog "\[ngBot\] AutoOp Error :: Unable to find NickDb plugin."
            return -code -1
        }

        namespace import ::ngBot::plugin::NickDb::*

        ## Bind event callbacks.
        bind join -|- * [namespace current]::GiveOp
        return
    }

    ####
    # AutoOp::DeInit
    #
    # Called on rehash; unregisters the event handler.
    #
    proc deinit {args} {
        ## Remove event callbacks.
        #catch {unbind evnt -|- prerehash [namespace current]::DeInit}
        #catch {unbind join -|- *!*@* [namespace current]::GiveOp}

        namespace delete [namespace current]
        return
    }

    ####
    # AutoOp::GetInfo
    #
    # gets $group and $flags from the userfile
    #
    proc GetInfo {ftpUser groupVar flagsVar} {
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
            putlog "\[ngBot\] AutoOp Error :: Unable to open user file for \"$ftpUser\" ($error)."
            return 0
        }
    }

    ####
    # AutoOp::GiveOp
    #
    # Op IRC users depending on their ftp flags
    #
    proc GiveOp {nick host handle channel} {
        variable ns
        variable np
        variable permsAutoOp
        
        set ftpUser [${ns}::GetFtpUser $nick]
     
        if {[${ns}::GetInfo $ftpUser group flags]} {
            if {[${np}::rightscheck $permsAutoOp $ftpUser $group $flags]} {
            pushmode $channel +o $nick
            putlog "\[ngBot\] AutoOp :: Gave OP to $nick ($ftpUser) in $channel"
            }
        }

    }
}
