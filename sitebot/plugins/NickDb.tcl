#################################################################################
# dZSbot - NickDb Plug-in                                                       #
#################################################################################
# - Keeps track of IRC nick names and their corrosponding FTP user name.        #
# - Remember, this only effective if the channel is invite-only.                #
#################################################################################

## TODO (neoxed):
## - Use SQLite v3 databse to keep track of users (or a simple text file?).
## - Maybe add an option for an in-memory db (array) and offline db (sqlite)?
## - Add InviteEvent to postcommand(INVITEUSER).

namespace eval NickDb {
    namespace export InviteEvent NickToUser UserToNick
}

proc NickDb::Init {} {
    global postcommand
    set scriptName [namespace current]::InviteEvent
}

proc NickDb::NickToUser {ircNick} {
}

proc NickDb::UserToNick {ftpUser} {
}

proc NickDb::InviteEvent {} {
}

NickDb::Init
