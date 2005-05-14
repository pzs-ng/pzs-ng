#######################
# PSXC-IMDB bot-addon #
# -------------------
# Tcl is really not my strong suit, so do not be surprised if your computer
# blows up when you run this script.

###################
##### config ######

# Version number. Do not change.
set psxc(VERSION) "2.7"

# relative path and name of the config.
# make sure you edit the conf-file before starting the bot.
set psxcbotconf "scripts/psxc-imdb-conf.tcl"

#
##### config done #####
#######################

catch {
  source "${psxcbotconf}"
}

proc readimdb {} {
 global psxc psxclog
 utimer 5 "readimdb"
 set psxclogsize [file size $psxc(IMDBLOG)]
 if { $psxclogsize == $psxclog } { return 0 }
 if { $psxclogsize  < $psxclog } { set psxclog 0 }
 set psxcascript $psxc(IMDBSCRIPT)
 if { $psxc(NODEFUNCT) == "YES" } { set psxcaout [exec $psxcascript ]; return 0 }
 set psxcaout [exec $psxcascript &]
 return 0
}

proc showimdb {} {
 global psxc psxcmovie
 utimer 5 "showimdb"
 set psxcmoviesize [file size $psxc(IMDBMOVIE)]
 if { $psxcmoviesize == $psxcmovie } { return 0 }
 if { $psxcmoviesize  < $psxcmovie } { set psxcmovie 0 }
 if { [catch { set psxcoffset [open $psxc(IMDBMOVIE) r] } ] } { return 0 }
 seek $psxcoffset $psxcmovie
 while {![eof $psxcoffset]} {
  set psxcline [gets $psxcoffset]
  if {$psxcline == ""} {continue}
  set psxcthr [lindex $psxcline 7]
  set psxcdst [lindex $psxcline 8]
  if {$psxcdst == ""} { foreach psxcimdbchan $psxc(IMDBCHANNEL) { foreach psxcln [split $psxcthr "$psxc(NEWLINE)"] { putserv "PRIVMSG $psxcimdbchan :$psxc(IMDBTRIG) $psxcln" } } }
  if {$psxcdst != ""} { foreach psxcln [split $psxcthr "$psxc(NEWLINE)"] { putserv "PRIVMSG $psxcdst :$psxc(IMDBTRIG) $psxcln" } }
 }
 close $psxcoffset
 set psxcmovie [file size $psxc(IMDBMOVIE)]
 return 0
}

proc preimdb {} {
 global psxc psxcprelog
 utimer 5 "preimdb"
 set psxcpresize [file size $psxc(PRELOG)]
 if { $psxcpresize == $psxcprelog } { return 0 }
 if { $psxcpresize  < $psxcprelog } { set psxcprelog 0 }
 set psxccscript $psxc(PRESCRIPT)
 set psxccout [ exec $psxccscript ]
 set psxcprelog [file size $psxc(PRELOG)]
 return 0
}

proc psxc-imdbsearch {nick uhost hand chan args} {
   global psxc
   catch {exec $psxc(FINDSCRIPT) $chan $args} psxcimdbfindlog
   if { $psxcimdbfindlog == "child process exited abnormally" } { putserv "privmsg $chan :error..."; return }
   foreach psxcline [split $psxcimdbfindlog "\n" ] { putserv "PRIVMSG $chan :$psxcline" }
}

proc psxc-imdbsearchdcc {nick host hand args} {
   global psxc
   if { $args == "" } { catch {exec $psxc(FINDSCRIPT) } psxcimdbfindlog }
#   if { $args != "" } { catch {exec $psxc(FINDSCRIPT) $nick $args -p -l1 } psxcimdbfindlog }
   if { $args != "" } { catch {exec $psxc(FINDSCRIPT) $nick $args } psxcimdbfindlog }
   if { $psxcimdbfindlog == "child process exited abnormally" } { putserv "privmsg $nick :error..."; return }
   foreach psxcline [split $psxcimdbfindlog "\n" ] { putserv "PRIVMSG $nick :$psxcline" }
}

putlog "psxc-imdb: Loading bot-addon v$psxc(VERSION)..."

set psxc(ERROR) "0"

if { [info exists psxc(ONBOT)] == 1 } {
 putlog "psxc-imdb: rehash detected - imdb-logging continue ON."
}
if { $psxc(USEBOT) == "YES" } {
 if { [file exist $psxc(IMDBLOG)] == 0 } { putlog "psxc-imdb: !!ERROR!! IMDBLOG: $psxc(IMDBLOG) not found."; set psxc(ERROR) "1" }
 if { [file exist $psxc(IMDBSCRIPT)] == 0 } { putlog "psxc-imdb: !!ERROR!! IMDBSCRIPT: $psxc(IMDBSCRIPT) not found."; set psxc(ERROR) "1" }
 if { $psxc(ERROR) == "0" } { 
  if { [info exists psxc(ONBOT)] == 0 } {
   set psxc(ONBOT) "ON"
   set psxclog 0
   putlog "psxc-imdb: imdb-logging is ON."
   readimdb
   }
  }
 }

if { [info exists psxc(ONFULL)] == 1 } {
 putlog "psxc-imdb: rehash detected - imdb-fullmode continue ON."
}
if { $psxc(USEFULL) == "YES" } {
 if { [file exist $psxc(IMDBMOVIE)] == 0 } { putlog "psxc-imdb: !!ERROR!! IMDBMOVIE: $psxc(IMDBMOVIE) not found."; set psxc(ERROR) "1" }
 if { $psxc(ERROR) == "0" } { 
  if { [info exists psxc(ONFULL)] == 0 } {
   set psxc(ONFULL) "ON"
   set psxcmovie [file size $psxc(IMDBMOVIE)]
   putlog "psxc-imdb: imdb-fullmode is ON."
   showimdb
   }
  }
 }

if { [info exists psxc(ONPRE)] == 1 } {
 putlog "psxc-imdb: rehash detected - imdb-premode continue ON."
}
if { $psxc(USEPRE) == "YES" } {
 if { [file exist $psxc(PRELOG)] == 0 } { putlog "psxc-imdb: !!ERROR!! PRELOG $psxc(PRELOG) not found."; set psxc(ERROR) "1" }
 if { [file exist $psxc(PRESCRIPT)] == 0 } { putlog "psxc-imdb: !!ERROR!! PRESCRIPT: $psxc(PRESCRIPT) not found."; set psxc(ERROR) "1" }
 if { $psxc(ERROR) == "0" } { 
  if { [info exists psxc(ONPRE)] == 0 } {
   set psxc(ONPRE) "ON"
   set psxcprelog 0
   putlog "psxc-imdb: imdb-premode is ON."
   preimdb
   }
  }
 }

if { [info exists psxc(ONFIND)] == 1 } {
 putlog "psxc-imdb: rehash detected - $psxc(FINDTRIG)-mode continue ON."
}
if { $psxc(USEFIND) == "YES" } {
 if { [file exist $psxc(FINDSCRIPT)] == 0 } { putlog "psxc-imdb: !!ERROR!! FINDSCRIPT: $psxc(FINDSCRIPT) not found."; set psxc(ERROR) "1" }
 if { $psxc(ERROR) == "0" } { 
  if { [info exists psxc(ONFIND)] == 0 } {
   set psxc(ONFIND) "ON"
   bind pub -|- $psxc(FINDTRIG) psxc-imdbsearch
   bind msg -|- $psxc(FINDTRIG) psxc-imdbsearchdcc
   putlog "psxc-imdb: $psxc(FINDTRIG)-mode is ON."
   }
  }
 }

if { $psxc(ERROR) == "0" } {  putlog "psxc-imdb: Ready." }
if { $psxc(ERROR) != "0" } {  putlog "psxc-imdb: Errors in config - please check." }

