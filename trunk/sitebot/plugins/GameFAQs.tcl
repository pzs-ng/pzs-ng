################################################################################
#                                                                              #
#                 GameFAQs - Game Info Pzs-ng Plug-in v1.4                     #
#                       by Meij <meijie@gmail.com>                             #
#                                                                              #
################################################################################
# Included with pzs-ng by psxc
# Step 4. below is done (changes done in dZSbot.defaults.tcl and dZSbot.vars)
# Step 5. is done in the default.zst theme *only*.
# Any questions must be relayed to the author.
# Updates and info can be found on http://bugs.pzs-ng.com/view.php?id=385
#
# Description:
# - Announce information obtained from gamefaqs.com on new releases.
#
# Installation:
# 1. Copy this file (GameFAQs.tcl) into your pzs-ng sitebots 'plugins' folder.
#
# 2. Edit the configuration options below.
#
# 3. Add the following to your eggdrop.conf:
#    source pzs-ng/plugins/GameFAQs.tcl
#
# 4. Add the following to dZSbot.conf:
#    set disable(GAMEFAQS)       0
#    set disable(GAMEFAQS-PRE)   0
#    set variables(GAMEFAQS)     "%pf %u_name %g_name %u_tagline %gamefaqstitle %gamefaqsrating %gamefaqsgenre %gamefaqsplayers %gamefaqsdeveloper {%gamefaqsalttitle %gamefaqspublisher %gamefaqsdate %gamefaqsregion} %gamefaqsurl %gamefaqssystem %gamefaqsesrb"
#    set variables(GAMEFAQS-PRE) "$variables(PRE) %gamefaqstitle %gamefaqsrating %gamefaqsgenre %gamefaqsplayers %gamefaqsdeveloper {%gamefaqsalttitle %gamefaqspublisher %gamefaqsdate %gamefaqsregion} %gamefaqsurl %gamefaqssystem %gamefaqsesrb"
#
#    set zeroconvert(%gamefaqstitle)     "N/A"
#    set zeroconvert(%gamefaqsrating)    "-"
#    set zeroconvert(%gamefaqsgenre)     "N/A"
#    set zeroconvert(%gamefaqsplayers)   "?"
#    set zeroconvert(%gamefaqsdeveloper) "N/A"
#    set zeroconvert(%gamefaqsalttitle)  "N/A"
#    set zeroconvert(%gamefaqspublisher) "N/A"
#    set zeroconvert(%gamefaqsdate)      "N/A"
#    set zeroconvert(%gamefaqsregion)    "N/A"
#    set zeroconvert(%gamefaqsurl)       "N/A"
#    set zeroconvert(%gamefaqssystem)    "N/A"
#
# 5. Add the following to your theme file (.zst).
#    announce.GAMEFAQS           = "[%b{GAME-INFO}][%section] %b{%g_title} (%g_rating)/10 URL: %g_url\n[%b{GAME-INFO}][%section] Release Date: %loop1"
#    announce.GAMEFAQS_LOOP1     = "%g_region: %g_date%splitter"
#
#    announce.GAMEFAQS-PRE       = "[%b{GAME-INFO}][%section] %b{%g_title} (%g_rating)/10 URL: %g_url\n[%b{GAME-INFO}][%section] Release Date: %loop1"
#    announce.GAMEFAQS-PRE_LOOP1 = "%g_region: %g_date%splitter"
#
# 6. Rehash or restart your eggdrop for the changes to take effect.
#
#
# System Names & Numbers:
#
# Num  Abbrev  System                | Num  Abbrev  System
#--------------------------------------------------------------------------------
# 0            All Platforms         | 1    DC      Dreamcast
# 2    GB      Game Boy              | 3    NGPC    NeoGeo Pocket Color
# 4    N64     Nintendo 64           | 5    PC      PC
# 6    PS      PlayStation           | 7    PS2     PlayStation 2
# 8    SAT     Saturn                | 9    GBC     Game Boy Color
# 10   GEN     Genesis               | 11   GC      GameCube
# 12   GBA     Game Boy Advance      | 13   XBOX    Xbox
# 14   NUON    Nuon                  | 15   3DO     3DO
# 16   GG      GameGear              | 17   JAG     Jaguar
# 18   NEO     NeoGeo                | 19   NES     NES
# 20   SMS     Sega Master System    | 21   SNES    SNES
# 22   TG16    TurboGrafx-16         | 23   WSC     WonderSwan Color
# 24   AMI     Amiga                 | 25   APL2    Apple II
# 26   ARC     Arcade Games          | 27   A2K1    Arcadia 2001
# 28   2600    Atari 2600            | 29   5200    Atari 5200
# 30   7800    Atari 7800            | 31   A800    Atari 8-bit
# 32   BBS     BBS Door              | 33   CDI     CD-I
# 34   FAIR    Channel F             | 35   CVIS    Colecovision
# 36   C64     Commodore 64          | 37   GP32    GP32
# 38   GCOM    Game.com              | 39   INTV    Intellivision
# 40   LYNX    Lynx                  | 41   MSX     MSX
# 42   MAC     Macintosh             | 43   MVIS    Microvision
# 44   O2      Odyssey^2             | 45   PCFX    PC-FX
# 46   PLD     Playdia               | 47   UNIX    Unix/Linux
# 48   VC20    VIC-20                | 49   VECX    Vectrex
# 50   VBOY    VirtualBoy            | 1000         All
# 1002 SCD     Sega CD               | 1003 32X     Sega 32X
# 1004 JCD     Jaguar CD             | 1005 TCD     Turbo CD
# 1006 NGE     N-Gage                | 1007 NGCD    Neo-Geo CD
# 1008 FDS     Famicom Disc System   | 1009 PBL     Pinball
# 1010 WEB     Online-Only/Web       | 1011 ODY     Odyssey
# 1012 N64DD   Nintendo 64DD         | 1013 ERDR    e-Reader
# 1014 PPC     Pocket PC             | 1015 POS     Palm OS
# 1017 SG1     SG-1000               | 1018 OS2     OS/2
# 1019 RED     Redemption            | 1020 APF     APF-*1000/IM
# 1021 AST     Astrocade             | 1022 WS      WonderSwan
# 1023 DVD     DVD Player            | 1024 PSP     PSP
# 1025 MOBILE  Mobile                | 1026 DS      DS
# 1027 ZOD     Zodiac                | 1028 PS3     PlayStation 3
# 1029 X360    Xbox 360              | 1030 GIZ     Gizmondo
# 1031 WII     Wii                   | 1032 X68     Sharp X68000
# 1033 PC98    NEC PC98              | 1034 ST      Atari ST
# 1035 CPC     Amstrad CPC           | 1036 COCO    Tandy Color Computer
# 1037 ORIC    Oric 1/Atmos          | 1038 PET     Commodore PET
# 1039 AQU     Mattel Aquarius       | 1040 CG      EACA Colour Genie 2000
# 1041 FMT     FM Towns              | 1042 ADV     Adventurevision
# 1043 SV      SuperVision           | 1044 CPS     CPS Changer
# 1045 S2      RCA Studio II         | 1046 LA      LaserActive
# 1047 IVC     Interton VC4000       | 1048 VCV     CreatiVision
#
#################################################################################


namespace eval ::ngBot::GameFAQs {
	variable events
	variable gamefaqs

	## Config Settings ###############################
	## Sections to be active in:
	##   Format: {Number Abbreviation Path}
	##
	##  Where Number is the system number from the above graph, the abbreviation
	##  used on gamefaqs and the path is path of the section
	##  (wildcards accepted).
	set gamefaqs(sections) { {12 gba /site/incoming/gba/} {13 xbox /site/incoming/xbox} {7 ps2 /site/incoming/ps2} {0 "" /site/requests/} }
	##
	## Timeout in milliseconds.
	set gamefaqs(timeout)  10000
	##
	## Date format (URL: http://tcl.tk/man/tcl8.3/TclCmd/clock.htm)
	set gamefaqs(date)     "%Y-%m-%d"
	##
	## Pre line regexp
	##  We need to reconstruct the full path to the release. Since not all
	##  pre scripts use the same format we'll use regexp to extract what we
	##  need from the pre logline and reconstuct it ourselves.
	##
	##  Default f00-pre example:
	set gamefaqs(pre-regexp) {^"(.[^"]+)" ".[^"]*" ".[^"]*" "(.[^"]+)"}
	set gamefaqs(pre-path)   "%2/%1"
	##
	##  Default eur0-pre example:
	#set gamefaqs(pre-regexp) {^"(.[^"]+)"}
	#set gamefaqs(pre-path)   "%1"
	##
	##################################################

	set events [list "NEWDIR" "PRE"]

	variable scriptName [namespace current]::LogEvent
	bind evnt -|- prerehash [namespace current]::DeInit
}

proc ::ngBot::GameFAQs::Init {args} {
	global postcommand

	variable events
	variable gamefaqs
	variable scriptName

	if {[catch {package require http}] != 0} {
		[namespace current]::Error "\"http\" package not found, unloading script."
		::ngBot::GameFAQs::DeInit
		return
	}

	## Register the event handler.
	foreach event $events {
		lappend postcommand($event) $scriptName
	}

	putlog "\[ngBot\] GameFAQs :: Loaded successfully."
}

proc ::ngBot::GameFAQs::DeInit {args} {
	global postcommand

	variable events
	variable gamefaqs
	variable scriptName

	## Remove the script event from postcommand.
	foreach event $events {
		if {[info exists postcommand($event)] && [set pos [lsearch -exact $postcommand($event) $scriptName]] !=  -1} {
			set postcommand($event) [lreplace $postcommand($event) $pos $pos]
		}
	}

	catch {unbind evnt -|- prerehash [namespace current]::DeInit}

	namespace delete [namespace current]
}

proc ::ngBot::GameFAQs::Error {error} {
	putlog "\[ngBot\] GameFAQs :: Error: $error"
}

proc ::ngBot::GameFAQs::LogEvent {event section logData} {
	variable gamefaqs

	if {[string compare -nocase $event "NEWDIR"] == 0} {
		set target "GAMEFAQS"

		set release [lindex $logData 0]
	} else {
		set target "GAMEFAQS-PRE"

		if {(![info exists gamefaqs(pre-regexp)]) || (![info exists gamefaqs(pre-path)])} {
			[namespace current]::Error "Your pre-regexp or pre-path variables are not set"
			return 0
		}

		if {[catch {regexp -inline -nocase -- $gamefaqs(pre-regexp) $logData} error] != 0} {
			[namespace current]::Error $error
			return 0
		}

		if {[set cookies [regexp -inline -all -- {%([0-9]+)} $gamefaqs(pre-path)]] == ""} {
			[namespace current]::Error "Your pre-path contains no valid cookies"
			return 0
		}

		set release $gamefaqs(pre-path)
		foreach {cookie number} $cookies {
			regsub -- $cookie $release [lindex $error $number] release
		}
	}

	foreach {num abbrev path} [join $gamefaqs(sections)] {
		if {[string match -nocase "$path*" $release]} {
			set search [[namespace current]::Cleanup [file tail $release]]
			set result [[namespace current]::Search $num $abbrev $search]

			foreach data $result {
				lappend logData $data
			}

			sndall $target $section [ng_format $target $section $logData]

			break
		}
	}

	return 1
}

proc ::ngBot::GameFAQs::Search {num abbrev search} {
	variable gamefaqs

	array set game [list rating "" release [list] developer "" system "" title "" url "" genre "" players "" esrb ""]

	::http::config -useragent "Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.0.6) Gecko/20060728 Firefox/1.5.0.6"
	set token [::http::geturl "http://www.gamefaqs.com/search/?[::http::formatQuery platform $num game $search]" -timeout $gamefaqs(timeout)]

	if {[string length $abbrev] == 0} {
		set abbrev ".*?"
	}

	if {[regexp -nocase -- "<td>($abbrev)</td><td><a href=\"(.*?)\">(.*?)</a></td>" [::http::data $token] -> game(system) tmp_url game(title)]} {
		set token [::http::geturl $tmp_url -timeout $gamefaqs(timeout)]

		upvar #0 $token state
		foreach {name value} $state(meta) {
			if {[string match -nocase "Location" $name]} {
				set game(url) $value

				set token [::http::geturl $value -timeout $gamefaqs(timeout)]
				set data [::http::data $token]

				if {[regexp -nocase -- {<div class="details">(.*?)</div>} $data -> tmp_data]} {
					set tmp_data [regexp -inline -nocase -all -- {<p>(.*?):.*?<b>(.*?)</b>.[^<]*</p>} $tmp_data]

					if {[llength $tmp_data] > 0} {
						foreach {tmp_junk tmp_name tmp_value} $tmp_data {
							switch -exact [string tolower $tmp_name] {
								"genre" {
									set game(genre) [string map { "&gt;" " > " "rpg " "RPG " " rpg" " RPG" } [[namespace current]::ToTitle $tmp_value]]
								}
								"developer" {
									regexp -nocase -- {<a href=".*?">(.*?)</a>} $tmp_value -> game(developer)
								}
								# has this been removed ?
								"number of players" {
									set game(players) [[namespace current]::ToTitle $tmp_value]
								}
								"esrb rating" {
									set game(esrb) $tmp_value
								}
							}
						}
					}
				}

				if {[regexp -nocase -- {<table class="release">(.*?)</table>} $data -> tmp_data]} {
					set tmp_data [regexp -inline -nocase -all -- {<th>(.[^<]*)</th>.[^<]*<td>.[^<]*<a href=".[^"]*">(.[^<]*)</a></td>.[^<]*<td.[^>]*>.[\s\t\r\n]*(.[^<]*)</td>.[^<]*<td>.[\s\t\r\n]*(.[^<]*)</td>} $tmp_data]

					if {[llength $tmp_data] > 0} {
						foreach {tmp_junk tmp_title tmp_publisher tmp_date tmp_region} $tmp_data {
							if {[regexp -- {[0-9]{2}/[0-9]{2}/[0-9]{2}} $tmp_date]} {
								set tmp_date [clock format [clock scan $tmp_date] -format $gamefaqs(date)]
							}

							lappend game(release) $tmp_title $tmp_publisher $tmp_date $tmp_region
						}
					}
				}

				# Get info from gamerankings.com
				if {[regexp -nocase -- {<a href="(.[^"]+)" target="_blank">Game Rankings</a>} $data -> tmp_url]} {
					set token [::http::geturl $tmp_url -timeout $gamefaqs(timeout)]

					set tmp_data [::http::data $token]
					if {[regexp -nocase -- {</b></font><font.*?> - \((.*?)\)} $tmp_data -> tmp_rating]} {
						set game(rating) [expr [lindex [split [string map {"%" ""} $tmp_rating] "\."] 0].0 / 10]
					} elseif {[regexp -nocase -- {<font color="#415497" style="font-size:30px;"><b>(.*?)</b></font>} $tmp_data -> tmp_rating]} {
						set game(rating) $tmp_rating
					}
				}
			}
		}
	}

	return [list $game(title) $game(rating) $game(genre) $game(players) $game(developer) $game(release) $game(url) $game(system) $game(esrb)]
}

proc ::ngBot::GameFAQs::ToTitle {text} {
	set ntext ""
	set list { "rpg" }
	foreach word [split $text " "] {
		set nword ""

		foreach sword [split $word "-"] {
			lappend nword [string totitle $sword]
		}

		foreach check $list {
			if {[string match -nocase $check $nword]} {
				set nword [string toupper $nword]
			}
		}
		lappend ntext [join $nword "-"]
	}

	return [join $ntext " "]
}


proc ::ngBot::GameFAQs::Cleanup {release} {
	set release [split $release "-"]

	if {[llength $release] > 1} {
		set release [lrange $release 0 [expr { [llength $release] - 2 }]]
	}
	set release [join [split [join [split [join $release] "."]] "_"]]
	set search ""
	set list {
		"multi??" "multi?" "mu?" "euro" "eur" "jpn" "jap" "usa" "internal" "trainer" "gba" "beta"
		"cracked" "nokia" "gba" "symbianos?" "proper" "multilanguage" "pal" "ntsc" "ps2" "xbox"
		"readnfo" "plus??" "plus?" "repack" "gba" "patch" "ngage" "retail" "arena" "crack" "french"
		"psp" "nds" "gbc" "wsx" "ws" "ngp" "ngpx" "dvd" "xboxdvd" "ps2dvd" "ps2rip" "xboxrip"
	}

	set search [list]
	foreach segment $release {
		set retval [catch { foreach check $list { if {[string match -nocase $check $segment]} { error beep } } }]
		if {$retval == 1} { break }
		lappend search $segment
	}

	return [join $search " "]
}

::ngBot::GameFAQs::Init
