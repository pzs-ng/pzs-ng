#################################################################################
# dZSbot - Deluser Ban Plug-in                                                  #
#################################################################################
# - Automatically kicks and bans user when they are deleted or purged.          #
# - This plug-in requires the NickDb plug-in and Sysop.log reading.             #
#################################################################################

## TODO (neoxed):
## - Use NickDB's UserToNick function to get the IRC user name.
## - Maybe add an option to KILL or KLINE/GLINE users (for compieter).
## - Add LogEvent to precommand(DELUSER) and precommand(PURGED).

namespace eval DeluserBan {
    namespace export LogEvent
}

proc DeluserBan::Init {} {
    global precommand
    set scriptName [namespace current]::LogEvent
}

proc DeluserBan::LogEvent {} {
}

DeluserBan::Init
