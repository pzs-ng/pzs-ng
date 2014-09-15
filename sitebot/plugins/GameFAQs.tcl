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
# - 20140915 - Sked:	Adjusted regexes for rating and description
#			Only user and internet ratings remain, but added amount of votes
# - 20140124 - Sked:	Updated systemnames & numbers
#			Fixed regexes and more for the new site layout
#			Added ESRB rating (K-A) and extra var _esrb_descriptor
#			Fixed date conversion
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
# Systemname			Abbreviation	Number
# All Platforms			n/a		0
# 3DO				3do		61
# 3DS				3ds		116
# Acorn Archimedes		arch		48
# Adventurevision		avision		32
# Amiga				amiga		39
# Amiga CD32			cd32		70
# Amstrad CPC			cpc		46
# Android			android		106
# APF-*1000/IM			apf1000		12
# Apple II			appleii		8
# Arcade Games			arcade		2
# Arcadia 2001			a2k1		28
# Astrocade			astrocade	7
# Atari 2600			atari2600	6
# Atari 5200			atari5200	20
# Atari 7800			atari7800	51
# Atari 8-bit			atari8bit	13
# Atari ST			ast		38
# Bandai Pippin			pippin		81
# BBC Micro			bbc		22
# BBS Door			bbs		50
# BlackBerry			blackberry	107
# Casio Loopy			loopy		80
# Cassette Vision		ecv		26
# CD-I				cdi		60
# Channel F			channelf	4
# Colecovision			colecovision	29
# Commodore 64			c64		24
# Commodore PET			pet		15
# CPS Changer			cps		75
# CreatiVision			cvision		23
# Dreamcast			dreamcast	67
# DS				ds		108
# DVD Player			dvd		87
# EACA Colour Genie 2000	cg2000		31
# e-Reader			ereader		101
# Famicom Disk System		famicomds	47
# Flash				flash		102
# FM-7				fm7		30
# FM Towns			fmtowns		55
# Game Boy			gameboy		59
# Game Boy Advance		gba		91
# Game Boy Color		gbc		57
# Game.com			gamecom		86
# GameCube			gamecube	99
# GameGear			gamegear	62
# Genesis			genesis		54
# Gizmondo			gizmondo	110
# GP32				gp32		100
# Intellivision			intellivision	16
# Interton VC4000		vc4000		10
# iPhone/iPod			iphone		112
# Jaguar			jaguar		72
# Jaguar CD			jaguarcd	82
# LaserActive			laser		71
# Lynx				lynx		58
# Macintosh			mac		27
# Mattel Aquarius		aquarius	36
# Microvision			microvision	17
# Mobile			mobile		85
# MSX				msx		40
# NEC PC88			pc88		21
# NEC PC98			pc98		42
# NeoGeo			neo		64
# Neo-Geo CD			neogeocd	68
# NeoGeo Pocket Color		ngpc		89
# NES				nes		41
# N-Gage			ngage		105
# Nintendo 64			n64		84
# Nintendo 64DD			n64dd		92
# Nuon				nuon		93
# Odyssey			odyssey		3
# Odyssey^2			odyssey2	9
# Online/Browser		webonly		69
# Oric 1/Atmos			oric1		44
# OS/2				os2		73
# Ouya				ouya		119
# Palm OS Classic		palmos		96
# Palm webOS			palm-webos	97
# PC				pc		19
# PC-FX				pcfx		79
# Pinball			pinball		1
# Playdia			playdia		77
# PlayStation			ps		78
# PlayStation 2			ps2		94
# PlayStation 3			ps3		113
# PlayStation 4			ps4		120
# PlayStation Vita		vita		117
# PSP				psp		109
# RCA Studio II			studio2		5
# Redemption			redemption	104
# Saturn			saturn		76
# Sega 32X			sega32x		74
# Sega CD			segacd		65
# Sega Master System		sms		49
# SG-1000			sg1000		43
# Sharp X1			x1		37
# Sharp X68000			x68000		52
# Sinclair ZX81/Spectrum	sinclair	35
# Sord M5			sordm5		25
# Super Cassette Vision		scv		45
# Super Nintendo		snes		63
# SuperVision			svision		66
# Tandy Color Computer		coco		18
# TI-99/4A			ti99		14
# Turbo CD			turbocd		56
# TurboGrafx-16			tg16		53
# Unix/Linux			unixlinux	33
# Vectrex			vectrex		34
# VIC-20			vic20		11
# Virtual Boy			virtualboy	83
# Wii				wii		114
# Wii U				wii-u		118
# Windows Mobile		windows-mobile	88
# WonderSwan			wonderswan	90
# WonderSwan Color		wsc		95
# Xbox				xbox		98
# Xbox 360			xbox360		111
# Xbox One			xboxone		121
# Zeebo				zeebo		115
# Zodiac			zod		103
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
	set gamefaqs(sections) { {91 /site/incoming/gba/} {98 /site/incoming/xbox} {94 /site/incoming/ps2} {0 /site/requests/} }
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
	## Date format. (URL: http://tcl.tk/man/tcl8.5/TclCmd/clock.htm)
	set gamefaqs(date)     "%Y-%m-%d"
	##
	## Skip announce for these directories.
	set gamefaqs(ignore_dirs) {cd[0-9] dis[ck][0-9] dvd[0-9] codec cover covers extra extras sample subs vobsub vobsubs proof}
	##
	## Max chars in the description, if more, it will be trimmed
	## to 3 less and have '...' added at the end. (default: 300)
	set gamefaqs(desctrimlength)  250
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
	set ${np}::zeroconvert(%gamefaqs_rating_uservotes) "-"
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
	set ${np}::zeroconvert(%gamefaqs_esrb_descriptor)  "N/A"
	set ${np}::zeroconvert(%gamefaqs_description)      "N/A"
	##
	##################################################
	set gamefaqs(version) "20140124"

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

		set variables(GAMEFAQS-MSG) "%gamefaqs_title %gamefaqs_rating_users %gamefaqs_rating_internet %gamefaqs_rating_uservotes %gamefaqs_genre %gamefaqs_players %gamefaqs_developer {%gamefaqs_region_title %gamefaqs_region_publisher %gamefaqs_region_date %gamefaqs_region_rating %gamefaqs_region} %gamefaqs_url %gamefaqs_system %gamefaqs_esrb_descriptor %gamefaqs_esrb %gamefaqs_description"
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

		#Default is "MM/DD/YY"
		#Variations: "Monthname YYYY" and "YYYY" can not be processed
		set format "%m/%d/%y"
		if {[regexp -- {^[01][0-9]/[0-3][0-9]/[0-9][0-9]$} $string]} {
			if {[catch {clock format [clock scan $string -format $format -locale en] -format $gamefaqs(date)} result] == 0} {
				set string $result
			}
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
		set output_order [list title rating_users rating_internet rating_uservotes genre \
							   players developer release_data url system esrb_descriptor esrb description]

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

		regexp -nocase -- {<li class="crumb top-crumb"><a href=".*?">(.*?)</a>} $data -> info(system)

		regexp -nocase -- {<span class="esrb_logo [a-z_]+">(.*?) - </span>} $data -> info(esrb)

		if {[regexp -nocase -- {<h2 class="title">Title Data</h2></div><div class="body"><dl>(.*?)</dl>} $data -> title_data]} {
			set title_details [regexp -inline -nocase -all -- {<dt>(.*?):</dt><dd>(.*?)</dd>} $title_data]

			if {[llength $title_details] > 0} {
				foreach {{} name value} $title_details {
					switch -exact [string tolower $name] {
						"genre" {
							set value [string map { "&gt;" ">" } $value]

							set info(genre) [${ns}::ToTitle $value]
						}
						"developer" {
							regexp -nocase -- {<a href=".*?">(.*?)</a>} $value -> info(developer)
						}
						"number of players" {
							set info(players) [${ns}::ToTitle $value]
						}
						"esrb descriptor(s)" {
							set info(esrb_descriptor) $value
						}
					}
				}
			}
		}

		if {[regexp -nocase -- {<h2 class="title">Release Data</h2></div><div class="body"><table class="contrib">.*?</thead><tbody>(.*?)</tbody></table>} $data -> release_data]} {
			set release_details [regexp -inline -nocase -all -- {<tr><td class="cbox" rowspan="2">.*?</td><td class="ctitle" colspan="6"><b>(.*?)</b></td></tr><tr><td class="cregion">(.*?)</td><td class="datacompany"><a href=".*?">(.*?)</a></td><td class="datapid">(.*?)</td><td class="datapid">(.*?)</td><td class="cdate">(.*?)</td><td class="datarating">(.*?)</td></tr>} $release_data]

			if {[llength $release_details] > 0} {
				foreach {{} title region publisher serial barcode date rating} $release_details {
					if {[string length $date] > 4} {
						set date [${ns}::ConvertDate $date]
					}
					if {[string equal -nocase $rating "&nbsp;"]} {
						set rating ""
					}
					if {[string equal -nocase $serial "&nbsp;"]} {
						set serial ""
					}
					if {[string equal -nocase $barcode "&nbsp;"]} {
						set barcode ""
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

		regexp -- {<div class="subsection-title">Rating:<br/><div .*?><a href.*?>([0-5]\.[0-9]{2}) / 5</a></div><p class="rate">([0-9]+) total votes</p></div></div>} $data -> info(rating_users) info(rating_uservotes)
		regexp -- {<div class="title">MetaCritic MetaScore</div><div .*?>([0-9]+)</div>} $data -> info(rating_internet)

		foreach name [array names info] {
			if {![string is double $info($name)] && ![string is integer $info($name)]} {
				unset info($name)
			}
		}

		regexp -- {<h2 class="title">Description</h2></div><div class="body game_desc"><div class="desc">(.*?)</div></div></div>} $data -> info(description)
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
		# Ratings used on GameFAQs:
		# AU (Classification Board): n/a < E (Exempt) < G (General) < PG (Parental Guidance) < M (Mature) < MA (15+ Mature Accompanied) < R (18+ Restricted)
		# EU (PEGI): n/a < 3+ < 7+ < 12+ < 16+ < 18+
		# US (ESRB): n/a < RP (Rating Pending) < E (Everyone) = K-A (Kids to Adult) < EC (Early Childhood) < E10+ (Everyone 10+) < T (Teen) < M (Mature) < AO (Adults Only)
		# JP (CERO): n/a < EDB (Education/Database) < A (All Ages) < B (Ages 12+) < C (Ages 15+) < D (Ages 17+) < Z (Ages 18+)
		# SG (Media Development Authority): n/a < NAR < AGE (Age Advisory) < M18
		# KO (Game Rating Board): n/a < ALL (All Ages) < 12 (Ages 12+) < 15 (Ages 15+) < 18 (Ages 18+) < NO (Refused Classification)
		# As it's not easy to crosscompare, only ESRB ratings are compared
		array set esrbratings { "" 0 "RP" 1 "E" 2 "K-A" 2 "EC" 3 "E10+" 4 "T" 5 "M" 6 "AO" 7 }
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
			3do 61		3ds 116		arch 48		avision 32		amiga 39
			cd32 70		cpc 46		android 106	apf1000 12		appleii 8
			arcade 2	a2k1 28		astrocade 7	atari2600 6		atari5200 20
			atari7800 51	atari8bit 13	ast 38		pippin 81		bbc 22
			bbs 50		blackberry 107	loopy 80	ecv 26			cdi 60
			channelf 4	colecovision 29	c64 24		pet 15			cps 75
			cvision 23	dreamcast 67	ds 108		dvd 87			cg2000 31
			ereader 101	famicomds 47	flash 102	fm7 30			fmtowns 55
			gameboy 59	gba 91		gbc 57		gamecom 86		gamecube 99
			gamegear 62	genesis 54	gizmondo 110	gp32 100		intellivision 16
			vc4000 10	iphone 112	jaguar 72	jaguarcd 82		laser 71
			lynx 58		mac 27		aquarius 36	microvision 17		mobile 85
			msx 40		pc88 21		pc98 42		neo 64			neogeocd 68
			ngpc 89		nes 41		ngage 105	n64 84			n64dd 92
			nuon 93		odyssey 3	odyssey2 9	webonly 69		oric1 44
			os2 73		ouya 119	palmos 96	palm-webos 97		pc 19
			pcfx 79		pinball 1	playdia 77	ps 78			ps2 94
			ps3 113		ps4 120		vita 117	psp 109			studio2 5
			redemption 104	saturn 76	sega32x 74	segacd 65		sms 49
			sg1000 43	x1 37		x68000 52	sinclair 35		sordm5 25
			scv 45		snes 63		svision 66	coco 18			ti99 14
			turbocd 56	tg16 53		unixlinux 33	vectrex 34		vic20 11
			virtualboy 83	wii 114		wii-u 118	windows-mobile 88	wonderswan 90
			wsc 95		xbox 98		xbox360 111	xboxone 121		zeebo 115
			zod 103
		}

		set string [string tolower $string]

		set platform 0
		if {[info exists platforms($string)]} {
			set platform $platforms($string)
		}

		return $platform
	}
}
