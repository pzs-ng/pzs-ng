#################################################################################
# dZSbot - Pre Time Plug-in                                                     #
#################################################################################
# - Announce the pre time of a release on NEWDIR.                               #
# - Users will have to write PreTime::LookUp for their pre database.            #
#################################################################################

## TODO (neoxed):
## - If the release is found in the pre database, return FALSE and cancel the script announce.
## - This plug-in will call ng_format and announce the event itself using NEWPRE and OLDPRE events.
## - Add an option for what time to consider backfills (BF=OLDPRE and OK=NEWPRE).
## - Add an option for which directories (/site/GROUPS) and subdirs to ignore (i.e. cd[0-9]).

namespace eval PreTime {
    namespace export NickToUser UserToNick
}

proc PreTime::Init {} {
    global precommand postcommand
    set scriptName [namespace current]::NewDirEvent
}

proc PreTime::LookUp {release} {
    ## This procedure will remain empty, since the end-user will have
    ## to write it to support their pre database (HTTP/MySQL/PostgreSQL etc).
}

proc PreTime::NewDirEvent {} {
}

PreTime::Init
