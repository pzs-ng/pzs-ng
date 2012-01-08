################################################################################
#                                                                              #
#                   GameFAQs - Game Info Pzs-ng Plug-in                        #
#                       by Meij <meijie@gmail.com>                             #
#                                                                              #
################################################################################
#
# Description:
# - Announce information obtained from gamefaqs.com on new releases.
#
# Installation:
# 1. Copy this file (GameFAQs.tcl) and the plugin theme (GameFAQs.zpt) into your
#    pzs-ng sitebots 'plugins' folder.
#
# 2. Edit the configuration options below.
#
# 3. Add the following to your eggdrop.conf:
#    source pzs-ng/plugins/GameFAQs.tcl
#
# 4. Rehash or restart your eggdrop for the changes to take effect.
#
# Changelog:
# - 20120107 - Sked:	Updated systemnames & numbers, ignore_dirs and
#			 banned words in dirnames (thanks to bwqbbq and K999 for the latter)
#			Added gamefaqs_description (Thanks to bwqbbq and K999)
#			Added region specific (esrb)rating
#			Several other fixes (Thanks to bwqbbq and K999)
#
################################################################################
#
# System Names & Numbers:
#
# Num  System               | Num  System                | Num  System
#------------------------------------------------------------------------------------
# 0    All Platforms        | 1    Dreamcast             | 2    Game Boy
# 3    NeoGeo Pocket Color  | 4    Nintendo 64           | 5    PC
# 6    PlayStation          | 7    PlayStation 2         | 8    Saturn
# 9    Game Boy Color       | 10   Genesis               | 11   GameCube
# 12   Game Boy Advance     | 13   Xbox                  | 14   Nuon
# 15   3DO                  | 16   GameGear              | 17   Jaguar
# 18   NeoGeo               | 19   NES                   | 20   Sega Master System
# 21   SNES                 | 22   TurboGrafx-16         | 23   WonderSwan Color
# 24   Amiga                | 25   Apple II              | 26   Arcade Games
# 27   Arcadia 2001         | 28   Atari 2600            | 29   Atari 5200
# 30   Atari 7800           | 31   Atari 8-bit           | 32   BBS Door
# 33   CD-I                 | 34   Channel F             | 35   Colecovision
# 36   Commodore 64         | 37   GP32                  | 38   Game.com
# 39   Intellivision        | 40   Lynx                  | 41   MSX
# 42   Macintosh            | 43   Microvision           | 44   Odyssey^2
# 45   PC-FX                | 46   Playdia               | 47   Unix/Linux
# 48   VIC-20               | 48   Vectrex               | 50   VirtualBoy
# 1002 Sega CD              | 1003 Sega 32X              | 1004 Jaguar CD
# 1005 Turbo CD             | 1006 N-Gage                | 1007 Neo-Geo CD
# 1008 Famicom Disk System  | 1009 Pinball               | 1010 Online/Browser
# 1011 Odyssey              | 1012 Nintendo 64DD         | 1013 e-Reader
# 1014 Windows Mobile       | 1015 Palm OS Classic       | 1017 SG-1000
# 1018 OS/2                 | 1019 Redemption            | 1020 APF-*1000/IM
# 1021 Astrocade            | 1022 WonderSwan            | 1023 DVD Player
# 1024 PSP                  | 1025 Mobile                | 1026 DS
# 1027 Zodiac               | 1028 PlayStation 3         | 1029 Xbox 360
# 1030 Gizmondo             | 1031 Wii                   | 1032 Sharp X68000
# 1033 NEC PC98             | 1034 Atari ST              | 1035 Amstrad CPC
# 1036 Tandy Color Computer | 1037 Oric 1/Atmos          | 1038 Commodore PET
# 1039 Mattel Aquarius      | 1040 EACA Colour Genie 2000| 1041 FM Towns
# 1042 Adventurevision      | 1043 SuperVision           | 1044 CPS Changer
# 1045 RCA Studio II        | 1046 LaserActive           | 1047 Interton VC4000
# 1048 CreatiVision         | 1049 iPhone/iPod           | 1050 Sinclair ZX81/Spectrum
# 1051 TI-99/4A             | 1052 Bandai Pippin         | 1053 BBC Micro
# 1054 Sord M5              | 1055 FM-7                  | 1056 Cassette Vision
# 1057 Super Cassette Vision| 1058 Acorn Archimedes      | 1059 Amiga CD32
# 1060 Casio Loopy          | 1061 Android               | 1062 Palm webOS
# 1063 BlackBerry           | 1064 Flash                 | 1065 NEC PC88
# 1066 Sharp X1             | 1067 Zeebo                 | 1068 3DS
# 1069 PlayStation Vita     | 1070 Wii U
#
################################################################################


namespace eval ::ngBot::plugin::GameFAQs {
	variable ns [namespace current]
	variable np [namespace qualifiers [namespace parent]]
	#variable ${np}::disable
	#variable ${np}::zeroconvert

	variable gamefaqs

	## Config Settings ###############################
	## Sections to be active in:
	##   Format: {Num Path}
	##
	##  Where Num is the system number from the above graph, and the Path
	##  is path of the section (wildcards accepted).
	set gamefaqs(sections) { {12 /site/incoming/gba/} {13 /site/incoming/xbox} {7 /site/incoming/ps2} {0 /site/requests/} }
	##
	## Timeout in milliseconds. (default: 3000)
	set gamefaqs(timeout)  3000
	##
	## Announce when no data was found. (default: false)
	set gamefaqs(announce-empty) false
	##
	## Channel trigger. (Leave blank to disable)
	set gamefaqs(ctrigger) "!game"
	##
	## Private message trigger. (Leave blank to disable)
	set gamefaqs(ptrigger) ""
	##
	## Date format. (URL: http://tcl.tk/man/tcl8.3/TclCmd/clock.htm)
	set gamefaqs(date)     "%Y-%m-%d"
	##
	## Skip announce for these directories.
	set gamefaqs(ignore_dirs) {cd[0-9] dis[ck][0-9] dvd[0-9] codec cover covers extra extras sample subs vobsub vobsubs proof}
	##
	## Max chars in the description, if more, it will be trimmed
	## to 3 less and have '...' added at the end. (default: 300)
	set gamefaqs(desctrimlength)  300
	##
	## Pre line regexp.
	##  We need to reconstruct the full path to the release. Since not all
	##  pre scripts use the same format we'll use regexp to extract what we
	##  need from the pre logline and reconstuct it ourselves.
	##
	## Default f00-pre example:
	set gamefaqs(pre-regexp) {^"(.[^"]+)" ".[^"]*" ".[^"]*" "(.[^"]+)"}
	set gamefaqs(pre-path)   "%2/%1"
	##
	## Default eur0-pre example:
	#set gamefaqs(pre-regexp) {^"(.[^"]+)"}
	#set gamefaqs(pre-path)   "%1"
	##
	## Disable announces. (0 = No, 1 = Yes)
	set ${np}::disable(GAMEFAQS)     0
	set ${np}::disable(GAMEFAQS-PRE) 0
	set ${np}::disable(GAMEFAQS-MSG) 0
	##
	## Convert empty or zero variables into something else.
	set ${np}::zeroconvert(%gamefaqs_title)            "N/A"
	set ${np}::zeroconvert(%gamefaqs_rating_users)     "-"
	set ${np}::zeroconvert(%gamefaqs_rating_internet)  "-"
	set ${np}::zeroconvert(%gamefaqs_rating)           "-"
	set ${np}::zeroconvert(%gamefaqs_genre)            "N/A"
	set ${np}::zeroconvert(%gamefaqs_players)          "?"
	set ${np}::zeroconvert(%gamefaqs_developer)        "N/A"
	set ${np}::zeroconvert(%gamefaqs_region_title)     "N/A"
	set ${np}::zeroconvert(%gamefaqs_region_publisher) "N/A"
	set ${np}::zeroconvert(%gamefaqs_region_date)      "N/A"
	set ${np}::zeroconvert(%gamefaqs_region_rating)    "-"
	set ${np}::zeroconvert(%gamefaqs_region)           "N/A"
	set ${np}::zeroconvert(%gamefaqs_url)              "N/A"
	set ${np}::zeroconvert(%gamefaqs_system)           "N/A"
	set ${np}::zeroconvert(%gamefaqs_esrb)             "-"
	set ${np}::zeroconvert(%gamefaqs_description)      "N/A"
	##
	##################################################
	set gamefaqs(version) "20120109"

	variable events [list "NEWDIR" "PRE"]

	variable scriptFile [info script]
	variable scriptName ${ns}::LogEvent

	#bind evnt -|- prerehash ${ns}::deinit


	proc init {args} {
		variable ns
		variable np
		variable ${np}::variables
		variable ${np}::postcommand

		variable events
		variable gamefaqs
		variable scriptName
		variable scriptFile

		if {[catch {package require http}] != 0} {
			${ns}::Error "\"http\" package not found."
			return -code -1
		}

		set variables(GAMEFAQS-MSG) "%gamefaqs_title %gamefaqs_rating_users %gamefaqs_rating_internet %gamefaqs_rating %gamefaqs_genre %gamefaqs_players %gamefaqs_developer {%gamefaqs_region_title %gamefaqs_region_publisher %gamefaqs_region_date %gamefaqs_region_rating %gamefaqs_region} %gamefaqs_url %gamefaqs_system %gamefaqs_esrb %gamefaqs_description"
		set variables(GAMEFAQS) "$variables(NEWDIR) $variables(GAMEFAQS-MSG)"
		set variables(GAMEFAQS-PRE) "$variables(PRE) $variables(GAMEFAQS-MSG)"

		set theme_file [file normalize "[pwd]/[file rootname $scriptFile].zpt"]
		if {[file isfile $theme_file]} {
			${np}::loadtheme $theme_file true
		}

		## Register the event handler.
		foreach event $events {
			lappend postcommand($event) $scriptName
		}

		if {([info exists gamefaqs(ctrigger)]) && (![string equal $gamefaqs(ctrigger) ""])} {
			bind pub -|- $gamefaqs(ctrigger) ${ns}::Trigger
		}
		if {([info exists gamefaqs(ptrigger)]) && (![string equal $gamefaqs(ptrigger) ""])} {
			bind msg -|- $gamefaqs(ptrigger) ${ns}::Trigger
		}

		${ns}::Debug "Loaded successfully (Version: $gamefaqs(version))."
	}

	proc deinit {args} {
		variable ns
		variable np
		variable ${np}::postcommand

		variable events
		variable gamefaqs
		variable scriptName

		## Remove the script event from postcommand.
		foreach event $events {
			if {[info exists postcommand($event)] && [set pos [lsearch -exact $postcommand($event) $scriptName]] !=  -1} {
				set postcommand($event) [lreplace $postcommand($event) $pos $pos]
			}
		}

		catch {unbind pub -|- $gamefaqs(ctrigger) ${ns}::Trigger}
		catch {unbind msg -|- $gamefaqs(ptrigger) ${ns}::Trigger}
		#catch {unbind evnt -|- prerehash ${ns}::deinit}

		namespace delete $ns
	}

	proc Debug {msg} {
		putlog "\[ngBot\] GameFAQs :: $msg"
	}

	proc Error {error} {
		putlog "\[ngBot\] GameFAQs :: Error: $error"
	}

	proc ConvertDate {string} {
		variable gamefaqs

		if {[catch {clock format [clock scan $string] -format $gamefaqs(date)} result] == 0} {
			set string $result
		}

		return $string
	}

	proc Trigger {args} {
		variable ns
		variable np
		variable gamefaqs

		if {[llength $args] == 5} {
			${np}::checkchan [lindex $args 2] [lindex $args 3]

			set trigger $gamefaqs(ctrigger)
		} else {
			set trigger $gamefaqs(ptrigger)
		}

		set text [lindex $args [expr { [llength $args] - 1 }]]
		set target [lindex $args [expr { [llength $args] - 2 }]]

		set platform [${ns}::SelectPlatform [lindex [split $text] 0]]

		if {$platform != 0} {
			set text [join [lrange [split $text] 1 end]]
		}

		if {[string equal $text ""]} {
			${np}::sndone $target "GameFAQs Syntax :: $trigger \[platform\] <string> (eg: $trigger pc diablo 2)"
			return 1
		}

		if {[catch {${ns}::FindInfo $text $platform [list]} logData] != 0} {
			${np}::sndone $target "GameFAQs Error :: $logData"
			return 0
		}

		${np}::sndone $target [${np}::ng_format "GAMEFAQS-MSG" "none" $logData]

		return 1
	}

	proc LogEvent {event section logData} {
		variable ns
		variable np
		variable gamefaqs

		if {[string compare -nocase $event "NEWDIR"] == 0} {
			set target "GAMEFAQS"

			set release [lindex $logData 0]
		} else {
			set target "GAMEFAQS-PRE"

			if {(![info exists gamefaqs(pre-regexp)]) || (![info exists gamefaqs(pre-path)])} {
				${ns}::Error "Your pre-regexp or pre-path variables are not set"
				return 0
			}

			if {[catch {regexp -inline -nocase -- $gamefaqs(pre-regexp) $logData} error] != 0} {
				${ns}::Error $error
				return 0
			}

			if {[set cookies [regexp -inline -all -- {%([0-9]+)} $gamefaqs(pre-path)]] == ""} {
				${ns}::Error "Your pre-path contains no valid cookies"
				return 0
			}

			set release $gamefaqs(pre-path)
			foreach {cookie number} $cookies {
				regsub -- $cookie $release [lindex $error $number] release
			}
		}

		## Check the release directory is ignored.
		foreach ignore [split $gamefaqs(ignore_dirs) " "] {
			if {[string match -nocase $ignore [file tail $release]]} {
				return 1
			}
		}

		foreach {platform path} [join $gamefaqs(sections)] {
			if {[string match -nocase "$path*" $release]} {
				set logLen [llength $logData]

				if {[catch {${ns}::FindInfo [file tail $release] $platform $logData} logData] != 0} {
					${ns}::Error "$logData. ($release)"
					return 0
				}

				set empty 1
				foreach piece [lrange $logData $logLen end] {
					if {![string equal $piece ""]} {
						set empty 0

						break
					}
				}

				if {($empty == 0) || ([string is true -strict $gamefaqs(announce-empty)])} {
					${np}::sndall $target $section [${np}::ng_format $target $section $logData]
				}

				break
			}
		}

		return 1
	}

	proc FindInfo {string platform logData} {
		variable ns
		set output_order [list title rating_users rating_internet rating genre \
							   players developer release_data url system esrb description]

		set string [${ns}::Cleanup $string]

		::http::config -useragent "Mozilla/5.0 (Windows NT 5.1; rv:9.0.1) Gecko/20100101 Firefox/9.0.1"

		array set info [${ns}::FindGame $string $platform]

		if {[info exists info(url)]} {
			set info(url) "http://www.gamefaqs.com$info(url)"

			array set info [${ns}::GetRating $info(url)]
			array set info [${ns}::GetGame "$info(url)/data"]
		}

		foreach key $output_order {
			if {(![info exists info($key)]) || \
				([string equal -nocase $info($key) "&nbsp;"])} {
				set info($key) ""
			}

			lappend logData $info($key)
		}

		return $logData
	}

	proc FindGame {string platform} {
		variable gamefaqs

		set token [::http::geturl "http://www.gamefaqs.com/search/index.html?[::http::formatQuery game $string platform $platform]" -timeout $gamefaqs(timeout)]

		if {![string equal -nocase [::http::status $token] "ok"]} {
			return -code error "Connection [::http::status $token]"
		}

		set data [::http::data $token]
		if {![regexp -indices -nocase -- {<div class="head"><h2 class="title">Best Matches</h2></div>} $data pos]} {
			return -code error "No results found for \"$string\""
		}

		array set info [list]

		if {[regexp -nocase -start [lindex $pos 1] -- {<a href="(.*?)".*?>(.*?)</a></td>} $data -> info(url) info(title)]} {
			regsub -all -- {[\t\n\r]+} $info(title) "" info(title)
		}

		return [array get info]
	}

	proc GetGame {url {loop 0}} {
		variable ns
		variable gamefaqs

		set token [::http::geturl $url -timeout $gamefaqs(timeout)]

		upvar #0 $token state
		array set meta $state(meta)

		if {[info exists meta(Location)]} {
			set token [::http::geturl $meta(Location) -timeout $gamefaqs(timeout)]

			set info(url) $meta(Location)
		}

		set data [::http::data $token]

		array set info [list]

		regexp -nocase -- {<div class="crumbs"><a href=".*?">(.*?)</a>} $data -> info(system)

		regexp -nocase -- {<div class="logo">(.*?)</div>} $data -> info(esrb)

		if {[regexp -nocase -- {<ul class="details title_data">(.*?)</ul>} $data -> title_data]} {
			set title_details [regexp -inline -nocase -all -- {<li><span class="label">(.*?):</span><span class="data">(.*?)</span></li>} $title_data]

			if {[llength $title_details] > 0} {
				foreach {{} name value} $title_details {
					switch -exact [string tolower $name] {
						"genre" {
							set value [string map { "&gt;" " > " } $value]

							set info(genre) [${ns}::ToTitle $value]
						}
						"developer" {
							regexp -nocase -- {<a href=".*?">(.*?)</a>} $value -> info(developer)
						}
						## has this been removed ?
						"number of players" {
							set info(players) [${ns}::ToTitle $value]
						}
						## has this been removed ?
						"esrb rating" {
							set info(esrb) $value
						}
					}
				}
			}
		}

		if {[regexp -nocase -- {<table class="release">(.*?)</table>} $data -> release_data]} {
			set release_details [regexp -inline -nocase -all -- {<th scope="row">(.*?)</th><td><a href=".*?">(.*?)</a></td><td>(.*?)</td><td>(.*?)</td><td>(.*?)</td><td>(.*?)</td></tr>} $release_data]

			if {[llength $release_details] > 0} {
				foreach {{} title publisher serial date region rating} $release_details {
					if {[string length $date] > 4} {
						set date [${ns}::ConvertDate $date]
					}
					if {[string equal -nocase $rating "&nbsp;"]} {
						set rating ""
					}

					lappend info(release_data) $title $publisher $date $rating $region
				}
			}
		}

		# Get the highest ESRB rating from the region releases if no global was found
		if {![info exists info(esrb)]} {
			set info(esrb) ""
			foreach {{} {} {} rating {}} $info(release_data) {
				if {[${ns}::CompareESRB $info(esrb) $rating] < 0} {
					set info(esrb) $rating
				}
			}
		}

		return [array get info]
	}

	proc GetRating {url} {
		variable gamefaqs

		set token [::http::geturl $url -timeout $gamefaqs(timeout)]

		upvar #0 $token state
		array set meta $state(meta)

		array set info [list]

		if {[info exists meta(Location)]} {
			set token [::http::geturl $meta(Location) -timeout $gamefaqs(timeout)]
		}

		set data [::http::data $token]

		regexp -- {<tr><td.*?([0-9]{1,2}\.?[0-9]?|[\w/]+)</span>.*?([0-9]{1,2}\.?[0-9]?|[\w/]+)</span>.*?([0-9]{1,2}\.?[0-9]?|[\w/]+)</span>.*?</tr>} $data -> info(rating_users) info(rating) info(rating_internet)

		foreach name [array names info] {
			if {![string is double $info($name)]} {
				unset info($name)
			}
		}

		regexp -- {<div class="details">(.*?)</div>} $data -> info(description)
		if {[string length $info(description)] > $gamefaqs(desctrimlength)} {
			set desctrimmed [string range $info(description) 0 [expr $gamefaqs(desctrimlength) - 4]]
			set info(description) $desctrimmed...
		}

		return [array get info]
	}

	proc ToTitle {text} {
		set result [list]

		foreach word [split $text " "] {
			if {![string is upper $word]} {
				set nword [list]

				foreach sword [split $word "-"] {
					lappend nword [string totitle $sword]
				}

				set word [join $nword "-"]
			}

			lappend result $word
		}

		return [join $result " "]
	}

	proc Cleanup {string} {
		set string [split [string tolower $string] "-"]

		if {[llength $string] > 1} {
			set string [lrange $string 0 [expr { [llength $string] - 2 }]]
		}

		set string [join [split [join $string] "._"]]

		set banned {
			"multi??" "multi?" "mu?" "euro" "eur" "jpn" "jap" "usa" "internal"
			"trainer" "gba" "beta" "cracked" "nokia" "gba" "symbianos?" "proper"
			"multilanguage" "pal" "ntsc" "ps2" "xbox" "readnfo" "plus??" "plus?"
			"repack" "gba" "patch" "ngage" "retail" "arena" "crack" "french" "psp"
			"nds" "gbc" "wsx" "ws" "ngp" "ngpx" "dvd" "xboxdvd" "ps2dvd" "ps2rip"
			"xboxrip" "dutch" "german" "nds" "psp" "clonecd" "sfclone" "clonedvd"
			"update" "dlc" "unlocker" "update.?" "pack?dlc" "dlc" "hotfix" "nfo"
			"read?nfo" "incomplete" "keygen" "nodvd" "nocd" "dvd?rip" "fix" "crackfix"
			"update.??" "nfofix" "bonus?cd" "bonus" "dsi" "clean" "ps3" "jb" "dirfix"
			"spanish" "wii" "wiiware" "vc" "snes" "ng" "uncut" "xbox360" "x360" "final"
			"rf" "wave??"
		}

		set i [expr { [llength $string] - 1 }]
		foreach match $banned {
			if {([set result [lsearch -glob $string $match]] < $i) && ($result != -1)} {
				set i $result
				incr i -1
			}
		}

		return [lrange $string 0 $i]
	}

	# Returns 0 if ESRB's are equal, -1 if rating1 is lower than rating2
	# and 1 if rating1 is higher than rating2
	proc CompareESRB {rating1 rating2} {
		set flag 1
		array set esrbratings { "" 0 "RP" 1 "EC" 2 "E" 3 "E10+" 4 "T" 5 "M" 6 "AO" 7 }
		if {[info exists esrbratings($rating1)]} {
			if {[info exists esrbratings($rating2)]} {
				if {$esrbratings($rating1) < $esrbratings($rating2)} {
					set flag -1
				} elseif {$esrbratings($rating1) == $esrbratings($rating2)} {
					set flag 0
				}
			}
		} else {
			set flag -1
			if {![info exists esrbratings($rating2)]} {
				set flag 0
			}
		}

		return $flag
	}

	proc SelectPlatform {string} {
		array set platforms {
			dc       1	gb      2	ngpc      3	n64     4	pc      5
			ps       6	ps2     7	sat       8	gbc     9	gen    10
			gc      11	gba    12	xbox     13	nuon   14	3do    15
			gg      16	jag    17	neo      18	nes    19	sms    20
			snes    21	tg16   22	wsc      23	ami    24	apl2   25
			arc     26	a2k1   27	2600     28	5200   29	7800   30
			a800    31	bbs    32	cdi      33	fair   34	cvis   35
			c64     36	gp32   37	gcom     38	intv   39	lynx   40
			msx     41	mac    42	mvis     43	o2     44	pcfx   45
			pld     46	unix   47	vc20     48	vecx   49	vboy   50
			scd   1002	32x  1003	jcd    1004	tcd  1005	nge  1006
			ngcd  1007	fds  1008	pbl    1009	web  1010	ody  1011
			n64dd 1012	erdr 1013	winm   1014	pos  1015	sg1  1017
			os2   1018	red  1019	apf    1020	ast  1021	ws   1022
			dvd   1023	psp  1024	mobile 1025	ds   1026	zod  1027
			ps3   1028	x360 1029	giz    1030	wii  1031	x68  1032
			pc98  1033	st   1034	cpc    1035	coco 1036	oric 1037
			pet   1038	aqu  1039	cg     1040	fmt  1041	adv  1042
			sv    1043	cps  1044	s2     1045	la   1046	ivc  1047
			vcv   1048	ip   1049	zx     1050	ti   1051	pip  1052
			bbc   1053	m5   1054	fm7    1055	ecv  1056	svc  1057
			arch  1058	cd32 1059	cl     1060	and  1061	wos  1062
			bb    1063	fla  1064	pc88   1065	x1   1066	zb   1067
			3ds   1068	vita 1069	wiiu   1070
		}

		set string [string tolower $string]

		set platform 0
		if {[info exists platforms($string)]} {
			set platform $platforms($string)
		}

		return $platform
	}
}
