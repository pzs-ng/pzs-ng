################################################################################
#                                                                              #
#             TVRage - TV Show & Episode Pzs-ng Plug-in v1.1                   #
#                       by Meij <meijie@gmail.com>                             #
#                                                                              #
################################################################################
# Step 4. below is done (changes done in dZSbot.defaults.tcl and dZSbot.vars)
# Step 5. is done in the default.zst theme *only*.
# Any questions must be relayed to the author.
# Updates and info can be found on http://bugs.pzs-ng.com/view.php?id=407
#
# Description:
# - Announce information obtained from tvrage.com on pre and new releases.
#
# Installation:
# 1. Copy this file (TVRage.tcl) into your pzs-ng sitebots 'plugins' folder.
#
# 2. Edit the configuration options below.
#
# 3. Add the following to your eggdrop.conf:
#    source pzs-ng/plugins/TVRage.tcl
#
# 4. Add the following to dZSbot.conf:
#    set disable(TVRAGE)       0
#    set disable(TVRAGE-PRE)   0
#    set variables(TVRAGE)     "%pf %u_name %g_name %u_tagline %tvrage_show_name %tvrage_show_genres %tvrage_show_country %tvrage_show_status %tvrage_show_latest_title %tvrage_show_latest_episode %tvrage_show_latest_airdate %tvrage_show_next_title %tvrage_show_next_episode %tvrage_show_next_airdate %tvrage_show_url %tvrage_show_classification %tvrage_show_premiered %tvrage_episode_url %tvrage_episode_season_episode %tvrage_episode_season %tvrage_episode_number %tvrage_episode_original_airdate %tvrage_episode_title %tvrage_episode_production_number"
#    set variables(TVRAGE-PRE) "$variables(PRE) %tvrage_show_name %tvrage_show_genres %tvrage_show_country %tvrage_show_status %tvrage_show_latest_title %tvrage_show_latest_episode %tvrage_show_latest_airdate %tvrage_show_next_title %tvrage_show_next_episode %tvrage_show_next_airdate %tvrage_show_url %tvrage_show_classification %tvrage_show_premiered %tvrage_episode_url %tvrage_episode_season_episode %tvrage_episode_season %tvrage_episode_number %tvrage_episode_original_airdate %tvrage_episode_title %tvrage_episode_production_number"
#
#    set zeroconvert(%tvrage_show_name)                 "N/A"
#    set zeroconvert(%tvrage_show_genres)               "N/A"
#    set zeroconvert(%tvrage_show_country)              "N/A"
#    set zeroconvert(%tvrage_show_status)               "N/A"
#    set zeroconvert(%tvrage_show_latest_title)         "N/A"
#    set zeroconvert(%tvrage_show_latest_episode)       "N/A"
#    set zeroconvert(%tvrage_show_latest_airdate)       "N/A"
#    set zeroconvert(%tvrage_show_next_title)           "N/A"
#    set zeroconvert(%tvrage_show_next_episode)         "N/A"
#    set zeroconvert(%tvrage_show_next_airdate)         "N/A"
#    set zeroconvert(%tvrage_show_url)                  "N/A"
#    set zeroconvert(%tvrage_show_classification)       "N/A"
#    set zeroconvert(%tvrage_show_premiered)            "N/A"
#    set zeroconvert(%tvrage_episode_url)               "N/A"
#    set zeroconvert(%tvrage_episode_season_episode)    "N/A"
#    set zeroconvert(%tvrage_episode_season)            "N/A"
#    set zeroconvert(%tvrage_episode_number)            "N/A"
#    set zeroconvert(%tvrage_episode_original_airdate)  "N/A"
#    set zeroconvert(%tvrage_episode_title)             "N/A"
#    set zeroconvert(%tvrage_episode_production_number) "N/A"
#
# 5. Add the following to your theme file (.zst).
#    announce.TVRAGE           = "[%b{TV-INFO}][%section] %b{%tvrage_show_name}: %b{%tvrage_episode_title} (%tvrage_show_genres) Aired: %tvrage_episode_original_airdate\n[%b{TV-INFO}][%section] URL: %tvrage_episode_url"
#
#    announce.TVRAGE-PRE       = "[%b{TV-INFO}][%section] %b{%tvrage_show_name}: %b{%tvrage_episode_title} (%tvrage_show_genres) Aired: %tvrage_episode_original_airdate\n[%b{TV-INFO}][%section] URL: %tvrage_episode_url"
#
#
# 6. Rehash or restart your eggdrop for the changes to take effect.
#
#################################################################################

namespace eval ::ngBot::TVRage {
	variable events
	variable tvrage

	## Config Settings ###############################
	##
	set tvrage(sections) { "/site/incoming/tv" }
	##
	## Timeout in milliseconds.
	set tvrage(timeout)  10000
	##
	## Date format (URL: http://tcl.tk/man/tcl8.3/TclCmd/clock.htm)
	set tvrage(date)     "%Y-%m-%d"
	##
	## Genre splitter
	set tvrage(splitter) " / "
	##
	## Pre line regexp
	##  We need to reconstruct the full path to the release. Since not all
	##  pre scripts use the same format we'll use regexp to extract what we
	##  need from the pre logline and reconstuct it ourselves.
	##
	##  Default f00-pre example:
	set tvrage(pre-regexp) {^"(.[^"]+)" ".[^"]*" ".[^"]*" "(.[^"]+)"}
	set tvrage(pre-path)   "%2/%1"
	##
	##  Default eur0-pre example:
	#set tvrage(pre-regexp) {^"(.[^"]+)"}
	#set tvrage(pre-path)   "%1"
	##
	##################################################

	set events [list "NEWDIR" "PRE"]

	variable scriptName [namespace current]::LogEvent
	bind evnt -|- prerehash [namespace current]::DeInit
}

proc ::ngBot::TVRage::Init {args} {
	global postcommand

	variable events
	variable tvrage
	variable scriptName

	if {[catch {package require http}] != 0} {
		[namespace current]::Error "\"http\" package not found, unloading script."
		::ngBot::TVRage::DeInit
		return
	}

	## Register the event handler.
	foreach event $events {
		lappend postcommand($event) $scriptName
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

	catch {unbind evnt -|- prerehash [namespace current]::DeInit}

	namespace delete [namespace current]
}

proc ::ngBot::TVRage::Error {error} {
	putlog "\[ngBot\] TVRage :: Error: $error"
}

proc ::ngBot::TVRage::LogEvent {event section logData} {
	variable tvrage

	set output_order [list show_name show_genre show_country show_status show_latest_title show_latest_episode show_latest_airdate show_next_title show_next_episode show_next_airdate show_url show_classification show_premiered episode_url episode_season_episode episode_season episode_number episode_original_airdate episode_title episode_production_number]

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

	foreach path $tvrage(sections) {
		if {[string match -nocase "$path*" $release]} {
			if {[regexp -- {^(.*?)(\d+x\d+|[sS]\d+[eE]\d+).*$} [file tail $release] -> show_str episode_str]} {
				regexp -- {^(\d+)x(\d+)$} $episode_str -> episode_season episode_number
				regexp -- {^[sS](\d+)[eE](\d+)$} $episode_str -> episode_season episode_number

				regsub -all -- {[\._]} $show_str " " show_str
				set show_str [string trim $show_str]

				::http::config -useragent "Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.0.7) Gecko/20060909 Firefox/1.5.0.7"

				if {[catch {[namespace current]::GetShow $show_str} error] != 0} {
					[namespace current]::Error "$error ($release)."
					return 0
				}

				array set info $error

				if {[catch {[namespace current]::FindEpisode $info(show_url) $episode_season $episode_number} error]} {
					[namespace current]::Error "$error ($release)."
					return 0
				}

				array set info $error

				if {[catch {[namespace current]::GetEpisode $info(episode_url)} error]} {
					[namespace current]::Error "$error ($release)."
					return 0
				}

				array set info $error

				foreach key $output_order {
					if {![info exists info($key)]} {
						set info($key) ""
					}

					lappend logData $info($key)
				}

				sndall $target $section [ng_format $target $section $logData]
			} else {
				[namespace current]::Error "Unable to parse TV info from \"$release\"."
			}

			break
		}


	}

	return 1
}

proc ::ngBot::TVRage::GetShow {show} {
	variable tvrage

	set token [::http::geturl "http://www.tvrage.com/quickinfo.php?show=[::http::formatQuery $show]" -timeout $tvrage(timeout)]

	# Its pointless checking the http status, its always 200. Errors are
	# redirected on the server to an 'Unknown Page' error page.

	set data [::http::data $token]
	set matches [regexp -inline -nocase -all -- {(.[^@]+)@(.[^\n\r]+)(?:[\n\r]*)} $data]

	if {[string length $matches] == 0} {
		error "No results found for \"$show\""
	}

	foreach {junk key value} $matches {
		set key [string map { " " "_" } [string tolower $key]]

		switch -glob $key {
			latest_episode -
			next_episode {
				regsub -- {_\w+} $key "" key

				regexp -- {(.[^\^]+)\^(.[^\^]+)\^(.*)} $value -> info(show_$key\_episode) info(show_$key\_title) info(show_$key\_airdate)

				if {[regexp -- {[0-9]{2}/\w+/[0-9]{2}} $info(show_$key\_airdate)]} {
					set info(show_$key\_airdate) [string map { "/" "" } $info(show_$key\_airdate)]

					set info(show_$key\_airdate) [clock format [clock scan $info(show_$key\_airdate)] -format $tvrage(date)]
				}
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
		error "Invalid results found for \"$show\""
	}

	return [array get info]
}

proc ::ngBot::TVRage::FindEpisode {url season episode} {
	variable tvrage

	set token [::http::geturl "$url/episode_list/$season" -timeout $tvrage(timeout)]

	# Its pointless checking the http status, its always 200. Errors are
	# redirected on the server to an 'Unknown Page' error page.

	set data [::http::data $token]

	regexp -- {<font size=.?4.?>Season (\d+)</font>} $data -> tmp_season

	if {$season != $tmp_season} {
		error "Invalid season information. Website returned season $tmp_season, we were expecting $season"
	}

	set matches [regexp -inline -nocase -all -- {<tr bgcolor='#.*?'>.+?</tr>} $data]

	if {[string length $matches] == 0} {
		error "No results found for \"$season\x$episode\""
	}

	foreach junk $matches {
		regexp -nocase -- {<td width='40'.[^>]+><a href='(.[^']+)'>\d+x(\d+)</i></a></td>} $junk -> tmp_url tmp_episode

		if {$episode == $tmp_episode} {
			set info(episode_url) "http://www.tvrage.com$tmp_url"

			break
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

	# Its pointless checking the http status, its always 200. Errors are
	# redirected on the server to an 'Unknown Page' error page.

	set data [::http::data $token]

	if {[set edata [regexp -inline -nocase -- {<table align='left' cellpadding='2'>.+?</table>} $data]] != ""} {
		set matches [regexp -inline -nocase -all -- {<b>(.+?): </b></td><td.*?>(.+?)</td>} $edata]

		foreach {junk key value} $matches {
			set key [string trim [string map { "#" "" } $key]]
			set key [string map { " " "_" } [string tolower $key]]

			switch -glob $key {
				original_airdate {
					set info(episode_$key) $value

					if {[regexp -- {[0-9]{2}/\w+/[0-9]{2}} $info(episode_$key)]} {
						set info(episode_$key) [string map { "/" "" } $info(episode_$key)]

						set info(episode_$key) [clock format [clock scan $info(episode_$key)] -format $tvrage(date)]
					}
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
				default {
					set info(episode_$key) $value
				}
			}
		}
	}

	if {![info exists info]} {
		error "Invalid results found"
	}

	return [array get info]
}

::ngBot::TVRage::Init
