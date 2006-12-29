##
## Addon for psxc-imdb, done/converted by meij.
##
## Used to make !imdb lookups possible, and for pre.
## This version, unlike the old one, also has support for
## channel/user specific lookups, meaning that the channel/
## person did the !imdb will be the reciever of the output.
## The old version only put output in main channel.
##
## Require: dZSbot.vars from pzs-ng v1.0.4 or newer.

namespace eval ::ngBot::psxc-IMDb {
        global zeroconvert disable
 
        variable psxc
 
        ## Config Settings ###############################
        ##
        ## location of psxc-imdb.log file (aka IMDBLOG)
        set psxc(IMDBLOG)     "/glftpd/ftp-data/logs/psxc-imdb.log"
        ##
        ## location of the imdb-script
        set psxc(IMDBSCRIPT)  "/glftpd/bin/psxc-imdb.sh"
        ##
        ## location of psxc-moviedata.log file (aka GLLOG)
        ## used only in "full" mode, where this addon will do all 
        ## imdb-output to channel.
        ## !! DO NOT USE glftpd.log HERE !!
        set psxc(IMDBMOVIE)   "/glftpd/ftp-data/logs/psxc-moviedata.log"
        ##
        ## announce-channel(s) - separate by space
        ## used only in "full" mode.
        set psxc(IMDBCHANNEL) "#changethis"
        ##
        ## should the lines start with something?
        ## used only in "full" mode.
        set psxc(IMDBTRIG) "\002IMDB:\002"
        ##
        ## location of the .log file used for pre'ing
        set psxc(PRELOG)     "/glftpd/ftp-data/logs/glftpd.log"
        ##set psxc(PRELOG)     "/glftpd/ftp-data/logs/prelog"
        ##
        ## location of the imdb-pre-script. Normally, this is a symlink
        ## to psxc-imdb.sh.
        set psxc(PRESCRIPT)  "/glftpd/bin/psxc-imdb-pre.sh"
        ##
        ## What do you wish to use as a channel trigger to react to
        ## questions about a movie?
        set psxc(FINDTRIG) "!imdb"
        ##
        ## Where is the find script (full path)
        set psxc(FINDSCRIPT) "/glftpd/bin/psxc-imdb-find.sh"
        ##
        ## This char is used to split lines on output. It should be the
        ## same variable you have in psxc-imdb-conf (NEWLINE).
        set psxc(NEWLINE) "|"
        ##
        ##################################################
        ##
        ## this is where you enable/disable the parts of the script.
        ##
        ## Should the script be used to output imdb-info? Normally
        ## you set this to "YES"
        set psxc(USEBOT)  "YES"
        ##
        ## Should the script handle imdb-output? Normally dZsbot.tcl
        ## handles output, but in some cases that doesn't work. This was
        ## before known as "full" mode.
        ## You also set this to "YES" if you wish this addon to handle
        ## all !imdb requests.
        set psxc(USEFULL) "NO"
        ##
        ## Is pre-support wanted?
        set psxc(USEPRE)  "YES"
        ##
        ## Do you wish to answer to !imdb requests? Please read
        ## psxc-imdb-find.README before setting this to YES.
        ## Also REMOVE psxc-imdb-find.tcl from your eggdrop.conf if
        ## you used it!
        set psxc(USEFIND) "YES"
        ##
        ##################################################
        ##
        ## I know a lot of admins dislike the "<[psxc-imdb.sh] <defunct>"
        ## process - I strongly suggest you get used to it, but if you
        ## cannot, set this variable to YES - it will force the bot to wait
        ## for the script to finish. FYI - this can lead to a slow
        ## responding/freezing bot, it may die on occation, it may become
        ## very unstable in fact. But, you're free to test.
        set psxc(NODEFUNCT) "NO"
        ##
        ##################################################
        ##
        ## Disable announces. (0 = No, 1 = Yes)
        set disable(IMDB)                   0
        set disable(IMDBFIND)               0
        set disable(IMDBVAR)                0
        ##
        ## Convert empty or zero variables into something else.
        set zeroconvert(%imdbdirname)       "N/A"
        set zeroconvert(%imdburl)           "N/A"
        set zeroconvert(%imdbtitle)         "N/A"
        set zeroconvert(%imdbgenre)         "N/A"
        set zeroconvert(%imdbrating)        "N/A"
        set zeroconvert(%imdbcountry)       "N/A"
        set zeroconvert(%imdblanguage)      "N/A"
        set zeroconvert(%imdbcertification) "N/A"
        set zeroconvert(%imdbruntime)       "N/A"
        set zeroconvert(%imdbdirector)      "N/A"
        set zeroconvert(%imdbbusinessdata)  "N/A"
        set zeroconvert(%imdbpremiereinfo)  "N/A"
        set zeroconvert(%imdblimitedinfo)   "N/A"
        set zeroconvert(%imdbvotes)         "Less than 5"
        set zeroconvert(%imdbscore)         "0"
        set zeroconvert(%imdbname)          "N/A"
        set zeroconvert(%imdbyear)          "N/A"
        set zeroconvert(%imdbnumscreens)    "N/A"
        set zeroconvert(%imdbislimited)     "No idea."
        set zeroconvert(%imdbcastleadname)  "Uknown"
        set zeroconvert(%imdbcastleadchar)  "Uknown"
        set zeroconvert(%imdbtagline)       "No info found."
        set zeroconvert(%imdbplot)          "No info found."
        set zeroconvert(%imdbbar)           ".........."
        set zeroconvert(%imdbcasting)       "N/A"
        set zeroconvert(%imdbcommentshort)  "N/A"
        ##
        ##################################################
 
        set psxc(VERSION) "2.7"
 
        variable events [list "IMDB" "IMDBVAR" "IMDBFIND"]
 
        variable scriptFile [info script]
        variable scriptName [namespace current]::LogEvent
 
        bind evnt -|- prerehash [namespace current]::DeInit
}
 
proc ::ngBot::psxc-IMDb::Init {args} {
        global precommand variables msgtypes psxcimdb
 
        variable psxc
        variable events
        variable scriptName
        variable scriptFile
 
        lappend msgtypes(SECTION) "IMDB" "IMDBVAR"
        lappend msgtypes(DEFAULT) "IMDBFIND"
        set variables(IMDB)       "%pf %msg %imdbdestination"
        set variables(IMDBFIND)   "%pf %msg %imdbdestination"
        set variables(IMDBVAR)    "%pf %imdbdirname %imdburl %imdbtitle %imdbgenre %imdbrating %imdbcountry %imdblanguage %imdbcertification %imdbruntime %imdbdirector %imdbbusinessdata %imdbpremiereinfo %imdblimitedinfo %imdbvotes %imdbscore %imdbname %imdbyear %imdbnumscreens %imdbislimited %imdbcastleadname %imdbcastleadchar %imdbtagline %imdbplot %imdbbar %imdbcasting %imdbcommentshort %imdbdestination"
 
        set theme_file [file normalize "[pwd]/[file rootname $scriptFile].zpt"]
        if {[file isfile $theme_file]} {
                loadtheme $theme_file true
        }
 
        ## Register the event handler.
        foreach event $events {
                lappend precommand($event) $scriptName
        }
 
        set psxc(ERROR) 0
        set psxc(MODES) [list]
 
        ## Check existance of files and start reading the log
        if {[string is true -strict $psxc(USEBOT)]} {
                if {![file exist $psxc(IMDBLOG)]} {
                        [namespace current]::Error "IMDBLOG: $psxc(IMDBLOG) not found."
                        set psxc(ERROR) 1
                }
 
                if {![file exist $psxc(IMDBSCRIPT)]} {
                        [namespace current]::Error "IMDBSCRIPT: $psxc(IMDBSCRIPT) not found."
                        set psxc(ERROR) 1
                }
 
                if {$psxc(ERROR) == 0} {
                        if {![info exists psxcimdb(log)]} {
                                set psxcimdb(log) 0
 
                                [namespace current]::ReadIMDb
                        }
 
                        lappend psxc(MODES) "Logging"
                }
        }
 
        if {[string is true -strict $psxc(USEFULL)]} {
                if {![file exist $psxc(IMDBMOVIE)]} {
                        [namespace current]::Error "IMDBMOVIE: $psxc(IMDBMOVIE) not found."
                        set psxc(ERROR) 1
                }
 
                if {$psxc(ERROR) == 0} {
                        if {![info exists psxcimdb(movie)]} {
                                set psxcimdb(movie) [file size $psxc(IMDBMOVIE)]
 
                                [namespace current]::ShowIMDb
                        }
 
                        lappend psxc(MODES) "Full"
                }
        }
 
        if {[string is true -strict $psxc(USEPRE)]} {
                if {![file exist $psxc(PRELOG)]} {
                        [namespace current]::Error "PRELOG $psxc(PRELOG) not found."
                        set psxc(ERROR) 1
                }
                if {![file exist $psxc(PRESCRIPT)]} {
                        [namespace current]::Error "PRESCRIPT: $psxc(PRESCRIPT) not found."
                        set psxc(ERROR) 1
                }
 
                if {$psxc(ERROR) == 0} { 
                        if {![info exists psxcimdb(prelog)]} {
                                set psxcimdb(prelog) 0

                                [namespace current]::PreIMDb
                        }
 
                        lappend psxc(MODES) "Pre"
                }
        }
 
        if {[string is true -strict $psxc(USEFIND)]} {
                if {![file exist $psxc(FINDSCRIPT)]} {
                        [namespace current]::Error "FINDSCRIPT: $psxc(FINDSCRIPT) not found."
                        set psxc(ERROR) 1
                }
 
                if {$psxc(ERROR) == 0} { 
                        bind pub -|- $psxc(FINDTRIG) [namespace current]::Search
                        bind msg -|- $psxc(FINDTRIG) [namespace current]::SearchDcc
                        lappend psxc(MODES) "Find"
                }
        }
 
        if {$psxc(ERROR) != 0} {
                [namespace current]::Error "Errors in config - please check."
                [namespace current]::DeInit
        } else {
                if {[llength $psxc(MODES)] == 0} {
                        lappend psxc(MODES) "None"
                }
 
                [namespace current]::Debug "Loaded successfully. (Modes Enabled: [join $psxc(MODES) ", "])"
        }
}

proc ::ngBot::psxc-IMDb::DeInit {args} {
        global precommand
 
        variable psxc
        variable events
        variable scriptName
 
        ## Remove the script event from precommand.
        foreach event $events {
                if {[info exists precommand($event)] && [set pos [lsearch -exact $precommand($event) $scriptName]] !=  -1} {
                        set precommand($event) [lreplace $precommand($event) $pos $pos]
                }
        }
 
        catch {unbind pub -|- $psxc(FINDTRIG) [namespace current]::Search}
        catch {unbind msg -|- $psxc(FINDTRIG) [namespace current]::SearchDcc}
        catch {unbind evnt -|- prerehash [namespace current]::DeInit}
 
        namespace delete [namespace current]
}
 
proc ::ngBot::psxc-IMDb::Error {error} {
        putlog "\[ngBot\] psxc-imdb :: Error: $error"
}
 
proc ::ngBot::psxc-IMDb::Debug {error} {
        putlog "\[ngBot\] psxc-imdb :: $error"
}
 
proc ::ngBot::psxc-IMDb::LogEvent {event section logData} {
        set target [lindex $logData end]
 
        if {[string equal $target ""]} {
                sndall $event $section [ng_format $event $section $logData]
        } else {
                sndone $target [ng_format $event $section $logData] $section
        }
 
        ## Silence default output.
        return 0
}
 
proc ::ngBot::psxc-IMDb::ReadIMDb {} {
        global psxcimdb
 
        variable psxc
 
        utimer 5 [list [namespace current]::ReadIMDb]
 
        set psxclogsize [file size $psxc(IMDBLOG)]
 
        if {$psxclogsize == $psxcimdb(log)} {
                return 0
        }
 
        if {$psxclogsize < $psxcimdb(log)} {
                set psxcimdb(log) 0
        }
 
        if {![string is true -strict $psxc(NODEFUNCT)]} {
                set result [catch {exec $psxc(IMDBSCRIPT) &} psxcaout]
        } else {
                set result [catch {exec $psxc(IMDBSCRIPT)} psxcaout]
        }
 
        if {$result != 0} {
                [namespace current]::Error $psxcaout
        }
 
        return 0
}
 
proc ::ngBot::psxc-IMDb::ShowIMDb {} {
        global psxcimdb
 
        variable psxc
 
        utimer 5 [list [namespace current]::ShowIMDb]
 
        set psxcmoviesize [file size $psxc(IMDBMOVIE)]
 
        if {$psxcmoviesize == $psxcimdb(movie)} {
                return 0
        }
 
        if {$psxcmoviesize  < $psxcimdb(movie)} {
                set psxcimdb(movie) 0
        }
 
        if {[catch {open $psxc(IMDBMOVIE) r} fp] != 0} {
                return 0
        }
 
        seek $fp $psxcimdb(movie)
 
        while {![eof $fp]} {
                set psxcline [gets $fp]
 
                if {[string equal $psxcline ""]} {
                        continue
                }
 
                set psxcthr [lindex $psxcline 7]
                set psxcdst [lindex $psxcline 8]
 
                if {[string equal $psxcdst ""]} {
                        foreach psxcimdbchan $psxc(IMDBCHANNEL) {
                                foreach psxcln [split $psxcthr "$psxc(NEWLINE)"] {
                                        putserv "PRIVMSG $psxcimdbchan :$psxc(IMDBTRIG) $psxcln"
                                }
                        }
                } else {
                        foreach psxcln [split $psxcthr "$psxc(NEWLINE)"] {
                                putserv "PRIVMSG $psxcdst :$psxc(IMDBTRIG) $psxcln"
                        }
                }
        }
 
        close $fp
 
        set psxcimdb(movie) [file size $psxc(IMDBMOVIE)]
 
        return 0
}
 
proc ::ngBot::psxc-IMDb::PreIMDb {} {
        global psxcimdb
 
        variable psxc
 
        utimer 5 [list [namespace current]::PreIMDb]
 
        set psxcpresize [file size $psxc(PRELOG)]
 
        if {$psxcpresize == $psxcimdb(prelog)} {
                return 0
        }
 
        if {$psxcpresize < $psxcimdb(prelog)} {
                set psxcimdb(prelog) 0
        }
 
        if {[catch {exec $psxc(PRESCRIPT)} psxccout] != 0} {
                [namespace current]::Error $psxccout
                return 
        }
 
        set psxcimdb(prelog) [file size $psxc(PRELOG)]
 
        return 0
}
 
proc ::ngBot::psxc-IMDb::Search {nick uhost handle chan text} {
        variable psxc
 
        if {[catch {exec $psxc(FINDSCRIPT) $chan $text} psxcimdbfindlog] != 0} {
                [namespace current]::Error $psxcimdbfindlog
                putserv "PRIVMSG $chan :Error..."
                return
        }
 
        foreach psxcline [split $psxcimdbfindlog "\n"] {
                putserv "PRIVMSG $chan :$psxcline"
        }
}
 
proc ::ngBot::psxc-IMDb::SearchDcc {nick uhost handle text} {
        variable psxc
 
        set psxcexec $psxc(FINDSCRIPT)
        if {![string equal $text ""]} {
                #set result [catch {exec $psxcexec $nick $text -p -l1} psxcimdbfindlog]
                set result [catch {exec $psxcexec $nick $text} psxcimdbfindlog]
        } else {
                set result [catch {exec $psxcexec} psxcimdbfindlog]
        }
 
        if {$result != 0} {
                [namespace current]::Error $psxcimdbfindlog
                putserv "PRIVMSG $nick :Error..."
                return
        }
 
        foreach psxcline [split $psxcimdbfindlog "\n"] {
                putserv "PRIVMSG $nick :$psxcline"
        }
}
 
::ngBot::psxc-IMDb::Init

