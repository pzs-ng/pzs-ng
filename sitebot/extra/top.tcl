# Small eggdrop script to announce the $top_users weektop uploaders of
# current week for $top_sect section to $top_chan each $top_interval seconds
set top_interval    [expr 2 * 60 * 60]
set top_stats       "/glftpd/bin/stats"
set top_sect        0
set top_users       10
set top_chan        "#pzs-ng"


proc show_usertop {} {
    global top_interval top_sect top_stats top_users top_chan toptimer

    set bold "\002"
    set top_line " Week Top (Up)"
    set cnt 0

    set toptimer [utimer $top_interval "show_usertop"]
    foreach line [split [exec $top_stats -u -w -x $top_users -s $top_sect] "\n"] {
        set cnt [expr $cnt + 1]
        if {$cnt > 4} {
            set pos      [string range $line 1 2]
            set username [string trimright [string range $line 5 14]]
            set tagline  [string trimright [string range $line 16 46]]
            set files    [string trimleft [string range $line 48 55]]
            set bytes    [expr [string range $line 57 65] + 0]
            set speed    [expr [string range $line 69 [expr [string length $line] - 4]] + 0]

            append top_line " \[$pos. $username $bold$bytes$bold\M\]"
        }
    }
    puthelp "PRIVMSG $top_chan :[string range $top_line 1 end]"
}

if {[info exists toptimer]} {
    if {[catch {killutimer $toptimer} err]} {
        putlog "top.tcl: killutimer failed ($err)"
    }
}
set toptimer [utimer $top_interval "show_usertop"]
