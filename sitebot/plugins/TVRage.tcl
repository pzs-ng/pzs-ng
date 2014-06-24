################################################################################
#                                                                              #
#                TVRage - TV Show & Episode Pzs-ng Plug-in                     #
#                       by Meij <meijie@gmail.com>                             #
#     maintained by the community <irc://irc.efnet.org/#pzs-ng,#glhelp>        #
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
# If you want to use egghttp instead of the tcllib http package, make sure that
# the egghttp.tcl is sourced before this script in your eggdrop.conf.
#
# Changelog:
# - 20140315 - melange: Now does http cleanup where necessary.
# - 20140314 - melange:	No longer hangs eggdrop event loop while getting data.
# - 20140314 - melange: Option to use egghttp.
# - 20140314 - melange:	Option to store episode information in a file.
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
	set tvrage(sections) { "/site/incoming/tv-rip-hd/" "/site/incoming/tv-retail-hd/" "/site/incoming/tv-rip-sd/" "/site/incoming/tv-retail-sd/" }
#	set tvrage(sections) { "/site/incoming/tvxvid/" "/site/incoming/tvx264/" }
	##
	## Timeout in milliseconds. (default: 3000)
	set tvrage(timeout)  3000
	##
	## Announce when no data was found. (default: false)
	## Note: this also effects output to "nfo-file".
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
	## If you want to store the information for an epsiode in a file under the
	## release path, then specify the filename to use here (Leave empty/unset
	## to disable). You can use these cookies in the filename:
	##		%rlsname		The release directory name.
	##		%rlsname_lower	As %rlsname, but lowercase.
	##		%rlsname_upper	As %rlsname, but uppercase.
	##		%rlsname_title	As %rlsname, but uses [string totitle].
	set tvrage(nfo-file) ".tvrage"
	##################################################

	## Version
	set tvrage(version) "20140315"

	variable events [list "NEWDIR" "PRE"]

	variable scriptFile [info script]
	variable scriptName ${ns}::LogEvent

	variable http
	set http(useragent) "Mozilla/5.0 (Windows; U; Windows NT 6.0; en-US; rv:1.9.0.5) Gecko/2008120122 Firefox/3.0.5"

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
		variable http
		variable scriptName
		variable scriptFile

		set http(ns) ::http::
		set http(use_egghttp) [expr { [info exists http(use_egghttp)] && [${np}::istrue $http(use_egghttp)] }]

		if {$http(use_egghttp)} {
			if {[info exists ::egghttp(version)]} {
				${ns}::Debug "Using egghttp instead of the tcllib http package for http connections."
			} {
				${ns}::Error "\"egghttp\" package not found, unloading script."
				return -code -1
			}
			set http(ns) ::egghttp:
		}
		# Egghttp does not have a command to encode a query, so we still use
		# the tcllib http package for the formatQuery procedure and assume
		# UTF-8 encoding.
		if {[catch {package require http}] != 0} {
			${ns}::Error "\"http\" package not found, unloading script."
			return -code -1
		}

		set variables(TVRAGE-MSGFULL) "%tvrage_show_name %tvrage_show_id %tvrage_show_genres %tvrage_show_country %tvrage_show_network %tvrage_show_status %tvrage_show_latest_title %tvrage_show_latest_episode %tvrage_show_latest_airdate %tvrage_show_next_title %tvrage_show_next_episode %tvrage_show_next_airdate %tvrage_show_url %tvrage_show_classification %tvrage_show_premiered %tvrage_show_started %tvrage_show_ended %tvrage_show_airtime %tvrage_show_runtime %tvrage_episode_url %tvrage_episode_season_episode %tvrage_episode_season %tvrage_episode_number %tvrage_episode_original_airdate %tvrage_episode_title"
		set variables(TVRAGE) "$variables(NEWDIR) $variables(TVRAGE-MSGFULL)"
		set variables(TVRAGE-PRE) "$variables(PRE) $variables(TVRAGE-MSGFULL)"
		set variables(TVRAGE-MSGSHOW) $variables(TVRAGE-MSGFULL)
		set variables(TVRAGE-INFOFILE) $variables(TVRAGE-MSGFULL)

		# Handle the script being sourced with an absolute path.
		if {[string index $scriptFile 0] eq "/"} {
			set theme_file [file normalize "[file rootname $scriptFile].zpt"]
		} {
			set theme_file [file normalize "[pwd]/[file rootname $scriptFile].zpt"]
		}
		if {[file isfile $theme_file]} {
			${np}::loadtheme $theme_file true
		} {
			Error "Could not find theme file \"$theme_file\", but continuing anyway."
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

	proc InternalError {{target {}} {msg {}} {event {}} {section {}}} {
		variable np

		set pub_msg "TVRage Error :: An internal error occured. Please ask a sysop to check the eggdrop.log for details of the error."

		if {[string length $target]} {
			${np}::sndone $target $pub_msg
		} elseif {[string length $event]} {
			${np}::sndall $event $section $pub_msg
		} {
			${np}::sndall "SYSOP" "DEFAULT" $pub_msg
		}
		Error [expr { [string length $msg] ? $msg : $::errorInfo}]
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

	proc WriteInfoFile {event section path logData} {
		variable np
		variable tvrage

		set release [file tail $path]
		set path [file join [set ${np}::glroot] {*}[lrange [file split [file dirname $path]] 1 end]]
		set file_name [string map {"%rlsname_lower" [string toupper $release} $tvrage(nfo-file)]
		set file_name [string map {"%rlsname_upper" [string tolower $release} $tvrage(nfo-file)]
		set file_name [string map {"%rlsname_title" [string totitle $release} $tvrage(nfo-file)]
		set file_name [string map {"%rlsname" $release} $tvrage(nfo-file)]

		if {[catch { set file_id [open [file join $path $release $file_name] "w"] }]} {
			InternalError "" "" $event $section
			return 0
		}
		puts $file_id [${np}::ng_format "TVRAGE-INFOFILE" "none" $logData]
		close $file_id

		return 1
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

		if {[catch { set show_args [ShowArgs $text false] } fail]} {
			${np}::sndone $target "TVRage Error :: $fail"
			return 1
		}
		if {[catch { ${ns}::GetShowAndEpisode $target {*}$show_args [list ${ns}::TriggerCallback] }]} {
			InternalError $target
			return 0
		}
	}
	proc TriggerCallback {token target data_array} {
		variable np
		variable http
		set logData [LogData $data_array]

		## Display full series/episode info if episode_url exists
		set event [expr { [string equal [lindex $logData 19] ""] ? "TVRAGE-MSGSHOW" : "TVRAGE-MSGFULL" }]
		${np}::sndone $target [${np}::ng_format $event "none" $logData]

		$http(ns)cleanup $token

		return 1
	}

	proc LogEvent {event section logData} {
		variable ns
		variable np
		variable tvrage

		if {[string compare -nocase $event "NEWDIR"] == 0} {
			set event "TVRAGE"

			set release [lindex $logData 0]
		} else {
			set event "TVRAGE-PRE"

			if {(![info exists tvrage(pre-regexp)]) || (![info exists tvrage(pre-path)])} {
				InternalError "" "Your pre-regexp or pre-path variables are not set." $event $section
				return 0
			}

			if {[catch { set values [regexp -inline -nocase -- $tvrage(pre-regexp) $logData] }]} {
				InternalError "" "" $event $section
				return 0
			}

			if {[set cookies [regexp -inline -all -- {%([0-9]+)} $tvrage(pre-path)]] == ""} {
				InternalError "" "Your pre-path contains no valid cookies." $event $section
				return 0
			}

			set release $tvrage(pre-path)
			foreach {cookie number} $cookies {
				regsub -- $cookie $release [lindex $values $number] release
			}
		}

		## Check if the release directory is ignored.
		foreach ignore [split $tvrage(ignore_dirs) " "] {
			if {[string match -nocase $ignore [file tail $release]]} {
				return 1
			}
		}

		foreach path $tvrage(sections) {
			if {[string match -nocase "$path*" $release]} {

				if {[catch { set show_args [ShowArgs [file tail $release]] }]} {
					InternalError "" "" $event $section
					return 0
				}
				if {[catch { ${ns}::GetShowAndEpisode "" {*}$show_args [list ${ns}::LogEventCallback $event $section $release $logData] }]} {
					InternalError "" "" $event $section
					return 0
				}

				break
			}
		}

		return 1
	}
	proc LogEventCallback {event section release logData token target data_array} {
		variable np
		variable tvrage
		variable http

		set logData [concat $logData [LogData $data_array]]
		set empty 0

		foreach element [lrange $logData [llength $logData] end] {
			if {![string length $element]} {
				set empty 1
				break
			}
		}
		if {!$empty || [string is true -strict $tvrage(announce-empty)]} {
			${np}::sndall $event $section [${np}::ng_format $event $section $logData]

			if {[info exists tvrage(nfo-file)] && [string length $tvrage(nfo-file)]} {
				WriteInfoFile $event $section $release $logData
			}
		}
		$http(ns)cleanup $token
	}

	proc ShowArgs {release_dir {strict true}} {

		if {![string length $release_dir]} {
			return -code error "No show name specified."
		}
		if {![llength [set regexp_list [regexp -inline -- {^(.*)(?:(?:(\d+)x(\d+))|(?:[sS](\d+)[eE](\d+)))} $release_dir]]]} {
			if {[string is true -strict $strict]} {
				return -code error "Unable to parse show name, season and/or episode info from \"$release_dir\"."
			}
			# Assume that we are responding to an irc trigger.
			lappend regexp_list "" $release_dir
		}

		set show_str [string trim [string map {. " " _ " "} [lindex $regexp_list 1]]]
		set episode [split [concat [join [lrange $regexp_list 2 end]]]]

		return [list $show_str [lindex $episode 0] [lindex $episode 1]]
	}

	proc LogData {data_array} {

		set logData [list]
		array set data $data_array
		lappend output_order show_name show_id show_genres show_country show_network show_status
		lappend output_order show_latest_title show_latest_episode show_latest_airdate
		lappend output_order show_next_title show_next_episode show_next_airdate
		lappend output_order show_url show_classification show_premiered
		lappend output_order show_started show_ended show_airtime show_runtime
		lappend output_order episode_url episode_season_episode episode_season episode_number
		lappend output_order episode_original_airdate episode_title

		foreach key $output_order {
			if {![info exists data($key)] || [string equal -nocase $data($key) "&nbsp;"]} {
				set data($key) ""
			}
			lappend logData $data($key)
		}

		return $logData
	}

	proc GetShowAndEpisode {target show season epnumber callback {token {}}} {
		variable ns
		variable tvrage
		variable http

		set event_args [expr { [lindex $callback 0] eq "${ns}::LogEventCallback" ? [lrange $callback 1 2] : [list] }]

		if {![string length $token]} {
			lappend command ${ns}::GetShowAndEpisode $target $show $season $epnumber $callback

			# Egghttp doesn't have it's own query encoder and i'm too lazy
			# to write one :(
			set url "http://services.tvrage.com/tools/quickinfo.php?show=[::http::formatQuery $show]&ep=${season}x$epnumber"

			if {!$http(use_egghttp)} {
				$http(ns)config -useragent $http(useragent)
				$http(ns)geturl $url -command $command -timeout $tvrage(timeout)
			} {
				$http(ns)geturl $url $command -useragent $http(useragent) -timeout $tvrage(timeout)
			}
			return
		}

		set status [expr { $http(use_egghttp) ? [$http(ns)errormsg $token] : [$http(ns)status $token] }]

		if {![string equal -nocase $status "ok"]} {
			InternalError $target "Connection $status" {*}$event_args
			$http(ns)cleanup $token
			return
		}

		## Its pointless checking the http status, its always 200. Errors are
		## redirected on the server to an 'Unknown Page' error page.
		##
		## We probably should check it anyway, as we can not guarantee
		## behaviour, if the server gives an unexpected response (API change,
		## bad upgrade, etc). You'd hope they would test before commiting to
		## their production environment and ensure backwards compatibility for
		## clients, but they have made these mistakes in the past. At the very
		## least, we should handle redirect correctly. Maybe fix it one day :p

		## Removal of the <pre>-tag and splitting in key-value pairs, taking into account that value might be empty
		set data [string replace [$http(ns)data $token] 0 4]
		set matches [regexp -inline -nocase -all -- {(.[^@]+)@([^\n\r]*)(?:[\n\r]*)} $data]

		if {[string length $matches] == 0} {
			InternalError $target "No results found for \"$show\""
			$http(ns)cleanup $token
			return
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
			InternalError $target "Invalid results found for \"$show\"" {*}$event_args
			$http(ns)cleanup $token
		} {
			{*}$callback $token $target [array get info]
		}
	}
}

if {[string equal "" $::ngBot::plugin::TVRage::np]} {
	::ngBot::plugin::TVRage::init
}
