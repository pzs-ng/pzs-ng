################################################################################
# ngBot - AutoIRCFlag Gives users an IRC flag depending on their flags         #
################################################################################
#
# Author: Pee
# mod by: Sked
# 
# Description:
# - Based on Whois.tcl by Compieter & E-Liquid
# - Will give a defined IRC flag to ppl depending on their flags
#
# Installation:
# 1. You need to have NickDb.tcl loaded.
#
# 2. Add the following to your eggdrop.conf:
#    source pzs-ng/plugins/AutoIRCFlag.tcl
#
# Now redundant as already present
# 3. Add the following lines to your ngBot.conf:
#    set variables(WHOIS)   "%msg"
#    set redirect(WHOIS)    $staffchan
#    set disable(WHOIS)     0
#
#    Add "WHOIS" at the end of your msgtypes(DEFAULT) line.
#
#    Add the following line to your theme file:
#    announce.WHOIS = "%b{[WHOIS]} %msg"
#
# 4. Rehash or restart your eggdrop for the changes to take effect.
#
################################################################################

namespace eval ::ngBot::plugin::AutoIRCFlag {
    variable ns [namespace current]
    variable np [namespace qualifiers [namespace parent]]
    variable perms

    ## Config Settings ###############################
    ##
    ## Permissions! Who get's which flags in chan?
    ## Format: IRCFLAG "SITEFLAG =GROUPNAME -USERNAME"
    ##  with IRCFLAG the mode used on the ircd (ie o, v, h ...)
    ## Leave the default to op siteops (flag 1) and users with flag +A
    ##  and give normal users (flag 3) voice
    array set perms {
      o "1 A"
      v "3"
    }
    ##################################################

    ####
    # AutoIRCFlag::Init
    #
    # Called on initialization; registers the event handler. Yeah, nothing fancy.
    #
    proc init {args} {
        if {![namespace exists [namespace parent]::NickDb]} {
            putlog "\[ngBot\] AutoIRCFlag Error :: Unable to find NickDb plugin."
            return -code -1
        }

        namespace import [namespace parent]::NickDb::*

        ## Bind event callbacks.
        bind join -|- * [namespace current]::GiveIRCFlag
    }

    ####
    # AutoIRCFlag::DeInit
    #
    # Called on rehash; unregisters the event handler.
    #
    proc deinit {args} {
        namespace delete [namespace current]
    }

    ####
    # AutoIRCFlag::GetInfo
    #
    # gets $group and $flags from the userfile
    #
    proc GetInfo {ftpUser groupVar flagsVar} {
        variable np
        variable ${np}::location
        upvar $groupVar group $flagsVar flags

        set file "$location(USERS)/$ftpUser"
        # Linux will give an error if you open a directory and try to read from it.
        if {![file isfile $file]} {
            putlog "\[ngBot\] AutoIRCFlag Error :: Invalid user file for \"$ftpUser\" ($file)."
            return 0
        }

        set group ""; set flags ""
        if {[catch {set handle [open $file r]} error] == 0} {
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
            putlog "\[ngBot\] AutoIRCFlag Error :: Unable to open user file for \"$ftpUser\" ($error)."
            return 0
        }
    }

    ####
    # AutoIRCFlag::GiveIRCFlag
    #
    # Give IRC users an IRC flag depending on their site flags
    #
    proc GiveIRCFlag {nick host handle channel} {
        variable ns
        variable np
        variable perms

        set ftpUser [${ns}::GetFtpUser $nick]
        if {$ftpUser == ""} { return }

        if {[${ns}::GetInfo $ftpUser group flags]} {
            foreach mode [array names perms] {
                if {[${np}::rightscheck $perms($mode) $ftpUser $group $flags]} {
                    pushmode $channel +$mode $nick
                    putlog "\[ngBot\] AutoIRCFlag :: Gave +$mode to $nick ($ftpUser) in $channel"
                }
            }
        }
    }
}
