################################################################################
#                                                                              #
#                TVRage - TV Show & Episode Pzs-ng Plug-in                     #
#                       by Meij <meijie@gmail.com>                             #
#                                                                              #
################################################################################
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
# Changelog:
# - 20140723 - Sked:	Add socks5 proxy support, needs tclcurl (based on code by crx)
# - 20110630 - Sked:	Now tested and working with ngBot - thx to mok_ for reporting
# - 20110624 - Sked:	Change parsing to work with quickinfo API solely
#			Removed episode votes, rating and prodnumber as not available or useless
#			Added show_id, _network, _runtime, _airtime, _started and _ended vars
#			Backwards compatible with dZSbot via first configsetting
#
#################################################################################

namespace eval ::ngBot::plugin::TVRage {
	variable ns [namespace current]

	variable tvrage

	## Config Settings ###############################
	##
	## Choose one of two settings, the first when using ngBot, the second when using dZSbot
	variable np [namespace qualifiers [namespace parent]]
	#variable np ""
	##
	## Proxy settings
	## If you set proxy host it will use proxy. Keep it "" for no proxy.
	## For the type, options are http/socks4/socks5 or others depending on the TclCurl version
	set tvrage(proxytype) "socks5"
	set tvrage(proxyhost) ""
	set tvrage(proxyport) 8080
	set tvrage(proxyuser) "username"
	set tvrage(proxypass) "password"
	##
	set tvrage(sections) { "/site/incoming/tvxvid/" "/site/incoming/tvx264/" }
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
	set tvrage(ignore_dirs) {cd[0-9] dis[ck][0-9] dvd[0-9] codec cover covers extra extras sample subs vobsub vobsubs proof}
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
	## TVRAGE is used on NEWDIR, TVRAGE-PRE on PRE,
	## TVRAGE-MSGFULL is used with !tv-trigger with specific episode
	## and TVRAGE-MSGSHOW is used with !tv-trigger with only a showname or invalid episode number
	set ${np}::disable(TVRAGE)         0
	set ${np}::disable(TVRAGE-PRE)     0
	set ${np}::disable(TVRAGE-MSGFULL) 0
	set ${np}::disable(TVRAGE-MSGSHOW) 0
	##
	## Convert empty or zero variables into something else.
	set ${np}::zeroconvert(%tvrage_show_name)                 "N/A"
	set ${np}::zeroconvert(%tvrage_show_id)                   "N/A"
	set ${np}::zeroconvert(%tvrage_show_genres)               "N/A"
	set ${np}::zeroconvert(%tvrage_show_country)              "N/A"
	set ${np}::zeroconvert(%tvrage_show_network)              "N/A"
	set ${np}::zeroconvert(%tvrage_show_status)               "N/A"
	set ${np}::zeroconvert(%tvrage_show_latest_title)         "N/A"
	set ${np}::zeroconvert(%tvrage_show_latest_episode)       "N/A"
	set ${np}::zeroconvert(%tvrage_show_latest_airdate)       "N/A"
	set ${np}::zeroconvert(%tvrage_show_next_title)           "N/A"
	set ${np}::zeroconvert(%tvrage_show_next_episode)         "N/A"
	set ${np}::zeroconvert(%tvrage_show_next_airdate)         "N/A"
	set ${np}::zeroconvert(%tvrage_show_url)                  "N/A"
	set ${np}::zeroconvert(%tvrage_show_classification)       "N/A"
	set ${np}::zeroconvert(%tvrage_show_premiered)            "N/A"
	set ${np}::zeroconvert(%tvrage_show_started)              "N/A"
	set ${np}::zeroconvert(%tvrage_show_ended)                "N/A"
	set ${np}::zeroconvert(%tvrage_show_airtime)              "N/A"
	set ${np}::zeroconvert(%tvrage_show_runtime)              "N/A"
	set ${np}::zeroconvert(%tvrage_episode_url)               "N/A"
	set ${np}::zeroconvert(%tvrage_episode_season_episode)    "N/A"
	set ${np}::zeroconvert(%tvrage_episode_season)            "N/A"
	set ${np}::zeroconvert(%tvrage_episode_number)            "N/A"
	set ${np}::zeroconvert(%tvrage_episode_original_airdate)  "N/A"
	set ${np}::zeroconvert(%tvrage_episode_title)             "N/A"
	##
	##################################################

	## Version
	set tvrage(version) "20140723"
	## Useragent
	set tvrage(useragent) "Mozilla/5.0 (Windows; U; Windows NT 6.0; en-US; rv:1.9.0.5) Gecko/2008120122 Firefox/3.0.5"

	variable events [list "NEWDIR" "PRE"]

	variable scriptFile [info script]
	variable scriptName ${ns}::LogEvent

	if {[string equal "" $np]} {
		bind evnt -|- prerehash ${ns}::deinit
	}

	proc init {} {
		variable ns
		variable np
		variable ${np}::postcommand
		variable ${np}::variables

		variable events
		variable tvrage
		variable scriptName
		variable scriptFile

		# If string length nonzero, we require TclCurl
		if {[string length $tvrage(proxyhost)]} {
			if {[catch {package require TclCurl}]} {
				${ns}::Error "\"TclCurl\" package not found, unloading script."
				return -code -1
			}
		} else {
			if {[catch {package require http}]} {
				${ns}::Error "\"http\" package not found, unloading script."
				return -code -1
			}
		}

		set variables(TVRAGE-MSGFULL) "%tvrage_show_name %tvrage_show_id %tvrage_show_genres %tvrage_show_country %tvrage_show_network %tvrage_show_status %tvrage_show_latest_title %tvrage_show_latest_episode %tvrage_show_latest_airdate %tvrage_show_next_title %tvrage_show_next_episode %tvrage_show_next_airdate %tvrage_show_url %tvrage_show_classification %tvrage_show_premiered %tvrage_show_started %tvrage_show_ended %tvrage_show_airtime %tvrage_show_runtime %tvrage_episode_url %tvrage_episode_season_episode %tvrage_episode_season %tvrage_episode_number %tvrage_episode_original_airdate %tvrage_episode_title"
		set variables(TVRAGE) "$variables(NEWDIR) $variables(TVRAGE-MSGFULL)"
		set variables(TVRAGE-PRE) "$variables(PRE) $variables(TVRAGE-MSGFULL)"
		set variables(TVRAGE-MSGSHOW) $variables(TVRAGE-MSGFULL)

		set theme_file [file normalize "[pwd]/[file rootname $scriptFile].zpt"]
		if {[file isfile $theme_file]} {
			${np}::loadtheme $theme_file true
		}

		## Register the event handler.
		foreach event $events {
			lappend postcommand($event) $scriptName
		}

		if {([info exists tvrage(ctrigger)]) && (![string equal $tvrage(ctrigger) ""])} {
			bind pub -|- $tvrage(ctrigger) ${ns}::Trigger
		}
		if {([info exists tvrage(ptrigger)]) && (![string equal $tvrage(ptrigger) ""])} {
			bind msg -|- $tvrage(ptrigger) ${ns}::Trigger
		}

		${ns}::Debug "Loaded successfully (Version: $tvrage(version))."

	}

	proc deinit {args} {
		variable ns
		variable np
		variable ${np}::postcommand

		variable events
		variable scriptName

		## Remove the script event from postcommand.
		foreach event $events {
			if {[info exists postcommand($event)] && [set pos [lsearch -exact $postcommand($event) $scriptName]] !=  -1} {
				set postcommand($event) [lreplace $postcommand($event) $pos $pos]
			}
		}

		namespace delete $ns
	}

	proc Debug {msg} {
		putlog "\[ngBot\] TVRage :: $msg"
	}

	proc Error {error} {
		putlog "\[ngBot\] TVRage Error :: $error"
	}

	proc ConvertDate {string} {
		variable tvrage

		set tmp [string map { "/" " " } $string]
		if {![regexp {^\d} $tmp] && [set i [string last " " $tmp]] != -1} {
			set tmp [string replace $tmp $i $i ", "]
		}

		if {![string equal "$tmp" ""] && [catch {clock format [clock scan $tmp] -format $tvrage(date)} result] == 0} {
			set string $result
		}

		return $string
	}

	proc Trigger {args} {
		variable ns
		variable np
		variable tvrage

		if {[llength $args] == 5} {
			${np}::checkchan [lindex $args 2] [lindex $args 3]

			set trigger $tvrage(ctrigger)
		} else {
			set trigger $tvrage(ptrigger)
		}

		set text [lindex $args [expr { [llength $args] - 1 }]]
		set target [lindex $args [expr { [llength $args] - 2 }]]

		if {[string equal $text ""]} {
			${np}::sndone $target "TVRage Syntax :: $trigger <string> (eg: $trigger Stargate SG-1 S02E10)"
			return 1
		}

		if {[catch {${ns}::FindInfo $text [list] "false"} logData] != 0} {
			${np}::sndone $target "TVRage Error :: $logData"
			return 0
		}

		## Display full series/episode info if episode_url exists
		if {![string equal [lindex $logData 19] ""]} {
			${np}::sndone $target [${np}::ng_format "TVRAGE-MSGFULL" "none" $logData]
		} else {
			${np}::sndone $target [${np}::ng_format "TVRAGE-MSGSHOW" "none" $logData]
		}

		return 1
	}

	proc LogEvent {event section logData} {
		variable ns
		variable np
		variable tvrage

		if {[string compare -nocase $event "NEWDIR"] == 0} {
			set target "TVRAGE"

			set release [lindex $logData 0]
		} else {
			set target "TVRAGE-PRE"

			if {(![info exists tvrage(pre-regexp)]) || (![info exists tvrage(pre-path)])} {
				${ns}::Error "Your pre-regexp or pre-path variables are not set"
				return 0
			}

			if {[catch {regexp -inline -nocase -- $tvrage(pre-regexp) $logData} error] != 0} {
				${ns}::Error $error
				return 0
			}

			if {[set cookies [regexp -inline -all -- {%([0-9]+)} $tvrage(pre-path)]] == ""} {
				${ns}::Error "Your pre-path contains no valid cookies"
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

				if {[catch {${ns}::FindInfo [file tail $release] $logData} logData] != 0} {
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

				if {($empty == 0) || ([string is true -strict $tvrage(announce-empty)])} {
					${np}::sndall $target $section [${np}::ng_format $target $section $logData]
				}

				break
			}


		}

		return 1
	}

	proc FindInfo {string logData {strict true}} {
		variable ns
		set output_order [list show_name show_id show_genres show_country show_network show_status \
						show_latest_title show_latest_episode show_latest_airdate \
						show_next_title show_next_episode show_next_airdate \
						show_url show_classification show_premiered \
						show_started show_ended show_airtime show_runtime \
						episode_url episode_season_episode episode_season episode_number \
						episode_original_airdate episode_title]

		set show_str $string
		if {(![regexp -- {^(.*?)(\d+x\d+|[sS]\d+[eE]\d+).*$} $string -> show_str episode_str]) && ([string is true -strict $strict])} {
			return -code error "Unable to parse season and episode info from \"$string\""
		}

		set episode_season ""
		set episode_number ""
		catch {regexp -- {^(\d+)x(\d+)$} $episode_str -> episode_season episode_number}
		catch {regexp -- {^[sS](\d+)[eE](\d+)$} $episode_str -> episode_season episode_number}

		regsub -all -- {[\._]} $show_str " " show_str
		set show_str [string trim $show_str]

		array set info [${ns}::GetShowAndEpisode $show_str $episode_season $episode_number]

		foreach key $output_order {
			if {(![info exists info($key)]) || \
				([string equal -nocase $info($key) "&nbsp;"])} {
				set info($key) ""
			}

			lappend logData $info($key)
		}

		return $logData
	}

	proc GetShowAndEpisode {show season epnumber} {
		variable ns
		variable tvrage

		# init data
		set data ""
		if {[string length $tvrage(proxyhost)]} {
			curl::transfer -url "http://services.tvrage.com/tools/quickinfo.php?show=[curl::escape $show]&ep=${season}x$epnumber" -proxy $tvrage(proxyhost):$tvrage(proxyport) -proxytype $tvrage(proxytype) -proxyuserpwd $tvrage(proxyuser):$tvrage(proxypass) -useragent $tvrage(useragent) -bodyvar token -timeoutms $tvrage(timeout)
			## Removal of the <pre>-tag and splitting in key-value pairs, taking into account that value might be empty
			set data [string replace $token 0 4]
		} else {
			::http::config -useragent $tvrage(useragent)
			set token [::http::geturl "http://services.tvrage.com/tools/quickinfo.php?show=[::http::formatQuery $show]&ep=${season}x$epnumber" -timeout $tvrage(timeout)]

			if {![string equal -nocase [::http::status $token] "ok"]} {
				return -code error "Connection [::http::status $token]"
			}

			## Its pointless checking the http status, its always 200. Errors are
			## redirected on the server to an 'Unknown Page' error page.

			## Removal of the <pre>-tag and splitting in key-value pairs, taking into account that value might be empty
			set data [string replace [::http::data $token] 0 4]
		}

		set matches [regexp -inline -nocase -all -- {(.[^@]+)@([^\n\r]*)(?:[\n\r]*)} $data]

		if {[string length $matches] == 0} {
			return -code error "No results found for \"$show\""
		}

		foreach {{} key value} $matches {
			set key [string map { " " "_" } [string tolower $key]]

			switch -glob $key {
				latest_episode -
				next_episode {
					regsub -- {_\w+} $key "" key

					regexp -- {(.[^\^]+)\^(.[^\^]+)\^(.*)} $value -> info(show_$key\_episode) info(show_$key\_title) info(show_$key\_airdate)

					set info(show_$key\_airdate) [${ns}::ConvertDate $info(show_$key\_airdate)]
				}
				genres {
					set info(show_genres) [list]

					foreach genre [split $value "|"] {
						lappend info(show_genres) [string trim $genre]
					}

					set info(show_genres) [join $info(show_genres) $tvrage(splitter)]
				}
				episode_url -
				show_* {
					set info($key) $value
				}
				episode_info {
					regsub -- {_\w+} $key "" key

					regexp -- {(.[^\^]+)\^(.[^\^]+)\^(.*)} $value -> info(episode_season_episode) info(episode_title) info(episode_original_airdate)

					regexp -- {^(\d+)x(\d+)$} $info(episode_season_episode) -> info(episode_season) info(episode_number)

					set info(episode_original_airdate) [${ns}::ConvertDate $info(episode_original_airdate)]
				}
				ended -
				started {
					set info(show_$key) $value

					set info(show_$key) [${ns}::ConvertDate $info(show_$key)]
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

}

if {[string equal "" $::ngBot::plugin::TVRage::np]} {
	::ngBot::plugin::TVRage::init
}
