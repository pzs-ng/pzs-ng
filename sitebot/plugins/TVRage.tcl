################################################################################
#                                                                              #
#             TVRage - TV Show & Episode Pzs-ng Plug-in v2.0                   #
#                       by Meij <meijie@gmail.com>                             #
#                                                                              #
################################################################################
#
# Report bugs to: http://bugs.pzs-ng.com/view.php?id=407
#
# Description:
# - Announce information obtained from tvrage.com on pre and new releases.
#
# Installation:
# 1. Copy this file (TVRage.tcl) and the plugin theme (TVRage.zpt) into your
#    pzs-ng sitebots 'plugins' folder.
#
# 2. Edit the configuration options below.
#
# 3. Add the following to your eggdrop.conf:
#    source pzs-ng/plugins/TVRage.tcl
#
# 4. Rehash or restart your eggdrop for the changes to take effect.
#
#################################################################################

namespace eval ::ngBot::TVRage {
	global zeroconvert disable

	variable tvrage

	## Config Settings ###############################
	##
	set tvrage(sections) { "/site/incoming/tv" }
	##
	## Timeout in milliseconds. (default: 3000)
	set tvrage(timeout)  3000
	##
	## Announce when no data was found. (default: false)
	set tvrage(announce-empty) false
	##
	## Channel trigger. (Leave blank to disable)
	set tvrage(ctrigger) "!tv"
	##
	## Private message trigger. (Leave blank to disable)
	set tvrage(ptrigger) ""
	##
	## Date format. (URL: http://tcl.tk/man/tcl8.4/TclCmd/clock.htm)
	set tvrage(date)     "%Y-%m-%d"
	##
	## Skip announce for these directories.
	set tvrage(ignore_dirs) {cd[0-9] dis[ck][0-9] dvd[0-9] codec cover covers extra extras sample subs vobsub vobsubs}
	##
	## Genre splitter.
	set tvrage(splitter) " / "
	##
	## Pre line regexp.
	##  We need to reconstruct the full path to the release. Since not all
	##  pre scripts use the same format we'll use regexp to extract what we
	##  need from the pre logline and reconstuct it ourselves.
	##
	## Default f00-pre example:
	set tvrage(pre-regexp) {^"(.[^"]+)" ".[^"]*" ".[^"]*" "(.[^"]+)"}
	set tvrage(pre-path)   "%2/%1"
	##
	## Default eur0-pre example:
	#set tvrage(pre-regexp) {^"(.[^"]+)"}
	#set tvrage(pre-path)   "%1"
	##
	## Disable announces. (0 = No, 1 = Yes)
	set disable(TVRAGE)         0
	set disable(TVRAGE-PRE)     0
	set disable(TVRAGE-MSGFULL) 0
	set disable(TVRAGE-MSGSHOW) 0
	##
	## Convert empty or zero variables into something else.
	set zeroconvert(%tvrage_show_name)                 "N/A"
	set zeroconvert(%tvrage_show_genres)               "N/A"
	set zeroconvert(%tvrage_show_country)              "N/A"
	set zeroconvert(%tvrage_show_status)               "N/A"
	set zeroconvert(%tvrage_show_latest_title)         "N/A"
	set zeroconvert(%tvrage_show_latest_episode)       "N/A"
	set zeroconvert(%tvrage_show_latest_airdate)       "N/A"
	set zeroconvert(%tvrage_show_next_title)           "N/A"
	set zeroconvert(%tvrage_show_next_episode)         "N/A"
	set zeroconvert(%tvrage_show_next_airdate)         "N/A"
	set zeroconvert(%tvrage_show_url)                  "N/A"
	set zeroconvert(%tvrage_show_classification)       "N/A"
	set zeroconvert(%tvrage_show_premiered)            "N/A"
	set zeroconvert(%tvrage_episode_url)               "N/A"
	set zeroconvert(%tvrage_episode_season_episode)    "N/A"
	set zeroconvert(%tvrage_episode_season)            "N/A"
	set zeroconvert(%tvrage_episode_number)            "N/A"
	set zeroconvert(%tvrage_episode_original_airdate)  "N/A"
	set zeroconvert(%tvrage_episode_title)             "N/A"
	set zeroconvert(%tvrage_episode_production_number) "N/A"
	set zeroconvert(%tvrage_episode_score)             "-"
	set zeroconvert(%tvrage_episode_votes)             "0"
	##
	##################################################

	variable events [list "NEWDIR" "PRE"]

	variable scriptFile [info script]
	variable scriptName [namespace current]::LogEvent

	bind evnt -|- prerehash [namespace current]::DeInit
}

proc ::ngBot::TVRage::Init {args} {
	global postcommand variables

	variable events
	variable tvrage
	variable scriptName
	variable scriptFile

	if {[catch {package require http}] != 0} {
		[namespace current]::Error "\"http\" package not found, unloading script."
		[namespace current]::DeInit
		return
	}

	set variables(TVRAGE) "$variables(NEWDIR) %tvrage_show_name %tvrage_show_genres %tvrage_show_country %tvrage_show_status %tvrage_show_latest_title %tvrage_show_latest_episode %tvrage_show_latest_airdate %tvrage_show_next_title %tvrage_show_next_episode %tvrage_show_next_airdate %tvrage_show_url %tvrage_show_classification %tvrage_show_premiered %tvrage_episode_url %tvrage_episode_season_episode %tvrage_episode_season %tvrage_episode_number %tvrage_episode_original_airdate %tvrage_episode_title %tvrage_episode_production_number %tvrage_episode_score %tvrage_episode_votes"
	set variables(TVRAGE-PRE) "$variables(PRE) %tvrage_show_name %tvrage_show_genres %tvrage_show_country %tvrage_show_status %tvrage_show_latest_title %tvrage_show_latest_episode %tvrage_show_latest_airdate %tvrage_show_next_title %tvrage_show_next_episode %tvrage_show_next_airdate %tvrage_show_url %tvrage_show_classification %tvrage_show_premiered %tvrage_episode_url %tvrage_episode_season_episode %tvrage_episode_season %tvrage_episode_number %tvrage_episode_original_airdate %tvrage_episode_title %tvrage_episode_production_number %tvrage_episode_score %tvrage_episode_votes"
	set variables(TVRAGE-MSGFULL) "%tvrage_show_name %tvrage_show_genres %tvrage_show_country %tvrage_show_status %tvrage_show_latest_title %tvrage_show_latest_episode %tvrage_show_latest_airdate %tvrage_show_next_title %tvrage_show_next_episode %tvrage_show_next_airdate %tvrage_show_url %tvrage_show_classification %tvrage_show_premiered %tvrage_episode_url %tvrage_episode_season_episode %tvrage_episode_season %tvrage_episode_number %tvrage_episode_original_airdate %tvrage_episode_title %tvrage_episode_production_number %tvrage_episode_score %tvrage_episode_votes"
	set variables(TVRAGE-MSGSHOW) $variables(TVRAGE-MSGFULL)

	set theme_file [file normalize "[pwd]/[file rootname $scriptFile].zpt"]
	if {[file isfile $theme_file]} {
		loadtheme $theme_file true
	}

	## Register the event handler.
	foreach event $events {
		lappend postcommand($event) $scriptName
	}

	if {([info exists tvrage(ctrigger)]) && (![string equal $tvrage(ctrigger) ""])} {
		bind pub -|- $tvrage(ctrigger) [namespace current]::Trigger
	}
	if {([info exists tvrage(ptrigger)]) && (![string equal $tvrage(ptrigger) ""])} {
		bind msg -|- $tvrage(ptrigger) [namespace current]::Trigger
	}

	putlog "\[ngBot\] TVRage :: Loaded successfully."
}

proc ::ngBot::TVRage::DeInit {args} {
	global postcommand

	variable events
	variable tvrage
	variable scriptName

	## Remove the script event from postcommand.
	foreach event $events {
		if {[info exists postcommand($event)] && [set pos [lsearch -exact $postcommand($event) $scriptName]] !=  -1} {
			set postcommand($event) [lreplace $postcommand($event) $pos $pos]
		}
	}

	catch {unbind pub -|- $tvrage(ctrigger) [namespace current]::Trigger}
	catch {unbind msg -|- $tvrage(ptrigger) [namespace current]::Trigger}
	catch {unbind evnt -|- prerehash [namespace current]::DeInit}

	namespace delete [namespace current]
}

proc ::ngBot::TVRage::Error {error} {
	putlog "\[ngBot\] TVRage :: Error: $error"
}

proc ::ngBot::TVRage::ConvertDate {string} {
	variable tvrage

	set tmp [string map { "/" " " } $string]
	if {[set i [string last " " $tmp]] != -1} {
		set tmp [string replace $tmp $i $i ", "]
	}

	if {[catch {clock format [clock scan $tmp] -format $tvrage(date)} result] == 0} {
		set string $result
	}

	return $string
}

proc ::ngBot::TVRage::Trigger {args} {
	variable tvrage

	if {[llength $args] == 5} {
		checkchan [lindex $args 2] [lindex $args 3]

		set trigger $tvrage(ctrigger)
	} else {
		set trigger $tvrage(ptrigger)
	}

	set text [lindex $args [expr { [llength $args] - 1 }]]
	set target [lindex $args [expr { [llength $args] - 2 }]]

	if {[string equal $text ""]} {
		sndone $target "TVRage Syntax :: $trigger <string> (eg: $trigger Stargate SG-1 S02E10)"
		return 1
	}

	if {[catch {[namespace current]::FindInfo $text [list] "false"} logData] != 0} {
		sndone $target "TVRage Error :: $logData"
		return 0
	}

	## Display full series/episode info if episode_url exists
	if {![string equal [lindex $logData 13] ""]} {
		sndone $target [ng_format "TVRAGE-MSGFULL" "none" $logData]
	} else {
		sndone $target [ng_format "TVRAGE-MSGSHOW" "none" $logData]
	}

	return 1
}

proc ::ngBot::TVRage::LogEvent {event section logData} {
	variable tvrage

	if {[string compare -nocase $event "NEWDIR"] == 0} {
		set target "TVRAGE"

		set release [lindex $logData 0]
	} else {
		set target "TVRAGE-PRE"

		if {(![info exists tvrage(pre-regexp)]) || (![info exists tvrage(pre-path)])} {
			[namespace current]::Error "Your pre-regexp or pre-path variables are not set"
			return 0
		}

		if {[catch {regexp -inline -nocase -- $tvrage(pre-regexp) $logData} error] != 0} {
			[namespace current]::Error $error
			return 0
		}

		if {[set cookies [regexp -inline -all -- {%([0-9]+)} $tvrage(pre-path)]] == ""} {
			[namespace current]::Error "Your pre-path contains no valid cookies"
			return 0
		}

		set release $tvrage(pre-path)
		foreach {cookie number} $cookies {
			regsub -- $cookie $release [lindex $error $number] release
		}
	}

	## Check the release directory is ignored.
	foreach ignore [split $tvrage(ignore_dirs) " "] {
		if {[string match -nocase $ignore [file tail $release]]} {
			return 1
		}
	}

	foreach path $tvrage(sections) {
		if {[string match -nocase "$path*" $release]} {
			set logLen [llength $logData]

			if {[catch {[namespace current]::FindInfo [file tail $release] $logData} logData] != 0} {
				[namespace current]::Error "$logData. ($release)"
				return 0
			}

			set empty 1
			foreach piece [lrange $logData $logLen end] {
				if {![string equal $piece ""]} {
					set empty 0

					break
				}
			}

			if {($empty == 0) || ([string is true -strict $tvrage(announce-empty)])} {
				sndall $target $section [ng_format $target $section $logData]
			}

			break
		}


	}

	return 1
}

proc ::ngBot::TVRage::FindInfo {string logData {strict true}} {
	set output_order [list show_name show_genre show_country show_status \
	                       show_latest_title show_latest_episode \
	                       show_latest_airdate show_next_title \
	                       show_next_episode show_next_airdate show_url \
	                       show_classification show_premiered episode_url \
	                       episode_season_episode episode_season \
	                       episode_number episode_original_airdate \
	                       episode_title episode_production_number \
	                       episode_score episode_votes]

	set show_str $string
	if {(![regexp -- {^(.*?)(\d+x\d+|[sS]\d+[eE]\d+).*$} $string -> show_str episode_str]) && \
	    ([string is true -strict $strict])} {
		error "Unable to parse season and episode info from \"$string\""
	}

	catch {regexp -- {^(\d+)x(\d+)$} $episode_str -> episode_season episode_number}
	catch {regexp -- {^[sS](\d+)[eE](\d+)$} $episode_str -> episode_season episode_number}

	regsub -all -- {[\._]} $show_str " " show_str
	set show_str [string trim $show_str]

	::http::config -useragent "Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.0.7) Gecko/20060909 Firefox/1.5.0.7"

	array set info [[namespace current]::GetShow $show_str]
	if {([info exists episode_season]) && ([info exists episode_number]) && \
	    ([info exists info(show_url)])} {
		array set info [[namespace current]::FindEpisode $info(show_url) $episode_season $episode_number]

		if {[info exists info(episode_url)]} {
			array set info [[namespace current]::GetEpisode $info(episode_url)]
		}
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

proc ::ngBot::TVRage::GetShow {show} {
	variable tvrage

	set token [::http::geturl "http://www.tvrage.com/quickinfo.php?show=[::http::formatQuery $show]" -timeout $tvrage(timeout)]

	if {![string equal -nocase [::http::status $token] "ok"]} {
		return -code error "Connection [::http::status $token]"
	}

	## Its pointless checking the http status, its always 200. Errors are
	## redirected on the server to an 'Unknown Page' error page.

	set data [::http::data $token]
	set matches [regexp -inline -nocase -all -- {(.[^@]+)@(.[^\n\r]+)(?:[\n\r]*)} $data]

	if {[string length $matches] == 0} {
		return -code error "No results found for \"$show\""
	}

	foreach {junk key value} $matches {
		set key [string map { " " "_" } [string tolower $key]]

		switch -glob $key {
			latest_episode -
			next_episode {
				regsub -- {_\w+} $key "" key

				regexp -- {(.[^\^]+)\^(.[^\^]+)\^(.*)} $value -> info(show_$key\_episode) info(show_$key\_title) info(show_$key\_airdate)

				set info(show_$key\_airdate) [[namespace current]::ConvertDate $info(show_$key\_airdate)]
			}
			genres {
				set info(show_genre) [list]

				foreach genre [split $value "|"] {
					lappend info(show_genre) [string trim $genre]
				}

				set info(show_genre) [join $info(show_genre) $tvrage(splitter)]
			}
			show_* {
				set info($key) $value
			}
			default {
				set info(show_$key) $value
			}
		}
	}

	if {![info exists info(show_url)]} {
		return -code error "Invalid results found for \"$show\""
	}

	return [array get info]
}

proc ::ngBot::TVRage::FindEpisode {url season episode} {
	variable tvrage

	set token [::http::geturl "$url/episode_list/$season" -timeout $tvrage(timeout)]

	## Its pointless checking the http status, its always 200. Errors are
	## redirected on the server to an 'Unknown Page' error page.

	set data [::http::data $token]

	regexp -- {<font size=.?4.?>Season (\d+)</font>} $data -> tmp_season

	if {[string trimleft $season "0"] != $tmp_season} {
		error "Invalid season information. Website returned season $tmp_season, we were expecting $season"
	}

	set matches [regexp -inline -nocase -all -- {<tr bgcolor='#.*?'>.+?</tr>} $data]

	if {[string length $matches] == 0} {
		error "No results found for \"$season\x$episode\""
	}

	foreach junk $matches {
		if {[regexp -nocase -- {<td width='40'.[^>]+><a href='(.[^']+)'>\d+x(\d+)</i></a></td>} $junk -> tmp_url tmp_episode]} {
			if {$episode == $tmp_episode} {
				set info(episode_url) "http://www.tvrage.com$tmp_url"

				break
			}
		}

		catch {unset tmp_url tmp_episode}
	}

	if {![info exists info(episode_url)]} {
		error "Invalid results found for \"$season\x$episode\""
	}

	return [array get info]
}

proc ::ngBot::TVRage::GetEpisode {url} {
	variable tvrage

	set token [::http::geturl $url -timeout $tvrage(timeout)]

	## Its pointless checking the http status, its always 200. Errors are
	## redirected on the server to an 'Unknown Page' error page.

	set data [::http::data $token]

	if {[set edata [regexp -inline -nocase -- {<table class='b' width='100%' cellspacing='0'>.+?</table>} $data]] != ""} {
		set matches [regexp -inline -nocase -all -- {<b>(.+?): </b></td><td.*?>(.+?)</td>} $edata]

		foreach {junk key value} $matches {
			set key [string trim [string map { "#" "" } $key]]
			set key [string map { " " "_" } [string tolower $key]]
			set key [string trimright $key "._"]

			switch -glob $key {
				original_airdate {
					set info(episode_$key) [[namespace current]::ConvertDate $value]
				}
				writer -
				director {
					set tmp [regexp -inline -nocase -all -- {<a  href='(.+?)' >(.+?)</a><br>} $value]

					foreach {tmp_junk tmp_url tmp_name} $tmp {
						lappend info(episode_$key) $tmp_name $tmp_url
					}

					catch {unset tmp tmp_junk tmp_url tmp_name}
				}
				episode_* {
					set info($key) $value
				}
				score {
					regexp -nocase -- {(\d+) vote(?:s|) \(([\d\.]+) / 10\)<br>} $value -> info(episode_votes) info(episode_score)
				}
				default {
					set info(episode_$key) $value
				}
			}
		}
	}

	if {![info exists info]} {
		error "Unable to parse episode information"
	}

	return [array get info]
}

::ngBot::TVRage::Init
