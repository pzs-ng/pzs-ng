#!/bin/tclsh
# version: 9/07022002
## fix up puts "#0bleh"

if {[llength $argv] != 2} {puts "Invalid arguments passed to script!" ; exit 2}

set misc(echo) 1
set dir(glroot) ""
set dir(exempt) "/site/requests/* /site/private/*"
set dir(ziponly) "/site/0day/*"
set files(exempt) ""
set files(delete) "*.diz"
set files(unique) "{SFV *.\[ss\]\[fF\]\[vV\]}"
set files(chksfv) "*.\[rR\]\[aA\]\[rR\] *.\[mM\]\[pP\]3 *.\[rR0-9\]\[0-9\]\[0-9\]"
set files(naming) "{4 *.\[zZ\]\[iI\]\[pP\]}"

proc checkpath {path} {
   if {![file readable $path]} {
      echo "Bitch at the sysop, because he has\nabsolutely no idea how to setup this script!" ; exit 0
   }
}

proc checksfv {sfvfile upfile} {
  set value 0
  if {[catch {set sfvfile [open $sfvfile r]}] != 0} {exit 0}
  foreach line [split [read $sfvfile] "\n"] {
     set entry [lindex [join [join [split [split $line "\$"] "\""]]] 0]
     if {[string index $entry 0] == ";"} {continue}
     if {[string match -nocase $entry $upfile]} {set value 1 ; break}
  }
  close $sfvfile
  return $value
}

proc echo {string} {
 global misc
  if {$misc(echo)} {puts $string}
}
proc findfiles {pattern} {return [glob -nocomplain -type f $pattern]}

set file [lindex $argv 0] ; set path [lindex $argv 1] ; set fullpath "$dir(glroot)$path"
checkpath $fullpath

if {[info exists dir(exempt)]} {
   foreach entry $dir(exempt) {
      if {[string match -nocase $entry $path]} {exit 0}
   }
}

if {[info exists files(exempt)]} {
   foreach entry $files(exempt) {
      if {[string match -nocase $entry $file]} {exit 2}
   }
}

if {[info exists files(delete)]} {
   foreach entry $files(delete) {
      if {[string match -nocase $entry $file]} {exit 2}
   }
}

if {[info exists dir(ziponly)]} {
   foreach entry $dir(ziponly) {
      if {[string match -nocase $entry $fullpath] && ![string match -nocase "*.zip" $file]} {exit 2}
   }
}

if {[info exists files(unique)]} {
   foreach entry $files(unique) {
      if {[llength $entry] != 2} {continue}
      set type [lindex $entry 0] ; set pattern [lindex $entry 1]
      if {![string match -nocase $pattern $file]} {continue}
      set matches [findfiles $fullpath/$pattern]
      if {[llength $matches] == 0} {break}
      foreach entry $matches {
         if {[file size $entry] == 0} {file delete --force $entry ; break}
      }
      echo "File rejected! $type file already exist!" ; exit 2
   }
}

if {[info exists files(chksfv)]} {
   foreach entry $files(chksfv) {
      if {![string match -nocase $entry $file]} {continue}
      set mainfile [findfiles $fullpath/*.\[ss\]\[fF\]\[vV\]]
      if {[llength $mainfile] == 0} {echo "You must upload a sfv first!" ; exit 2}
      if {[llength $mainfile] != 1} {echo "Can't check the sfv since multiple files were found!" ; exit 0}
      if {[checksfv $mainfile $file]} {exit 0}
      echo "File does not exist in sfv!" ; exit 2
   }
}

if {[info exists files(naming)]} {
   foreach entry $files(naming) {
      if {[llength $entry] != 2} {continue}
      set chars [lindex $entry 0] ; set pattern [lindex $entry 1]
      if {![string match -nocase $pattern $file]} {continue}
      set mainfile [file tail [lindex [findfiles $fullpath/$pattern] 0]]
      if {$mainfile == ""} {exit 0}
      set x 0 ; set pattern ""
      while {$x < $chars} {
         append pattern [string index $mainfile $x]
         incr x
      }
      append pattern "*"
      if {[string match -nocase $pattern $file]} {exit 0}
      echo "Filename does not match with existing disks" ; exit 2
   }
}

exit 0
