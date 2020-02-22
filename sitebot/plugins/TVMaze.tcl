################################################################################
#                                                                              #
#                TVMaze - TV Show & Episode Pzs-ng Plug-in                     #
#              Initial code for TVRage by Meij <meijie@gmail.com>              #
#                           TVMaze rework by MrCode                            #
#                                                                              #
# APIs https://www.tvmaze.com/api                                              #
#                                                                              #
################################################################################
#
# Description:
# - Announce information obtained from tvmaze.com on pre and new releases.
#
# Installation:
# 1. Copy this file (TVMaze.tcl) and the plugin theme (TVMaze.zpt) into your
#    pzs-ng sitebots 'plugins' folder.
#
# 2. Edit the configuration options below.
#
# 3. Add the following to your eggdrop.conf:
#    source pzs-ng/plugins/TVMaze.tcl
#
# 4. Rehash or restart your eggdrop for the changes to take effect.
#
# Changelog:
# - 20200222 - Sked:	Use https (thanks to teqnodude)
# - 20190815 - Sked:	Fix finding shows with newer Tcl packages
# - 20160310 - Sked:	Fix show_network
# - 20160117 - Sked:	Cleanup for inclusion in pzs-ng
# - 20160115 - MrCode:	Refactoring
# - 20151222 - MrCode:	Revamp TVRage to TVMaze
#
#################################################################################

namespace eval ::ngBot::plugin::TVMaze {
	variable ns [namespace current]

	variable tvmaze

	## Config Settings ###############################
	##
	## Choose one of two settings, the first when using ngBot, the second when using dZSbot
	variable np [namespace qualifiers [namespace parent]]
	#variable np ""
	##
	## Proxy settings
	## If you set proxy host it will use proxy. Keep it "" for no proxy.
	## For the type, options are http/socks4/socks5 or others depending on the TclCurl version
	set tvmaze(proxytype) "socks5"
	set tvmaze(proxyhost) ""
	set tvmaze(proxyport) 8080
	set tvmaze(proxyuser) "username"
	set tvmaze(proxypass) "password"
	##
	set tvmaze(sections) { "/site/incoming/tvxvid/" "/site/incoming/tvx264/" }
	##
	## Timeout in milliseconds. (default: 3000)
	set tvmaze(timeout)  3000
	##
	## Announce when no data was found. (default: false)
	set tvmaze(announce-empty) false
	##
	## Channel trigger. (Leave blank to disable)
	set tvmaze(ctrigger) "!tv"
	##
	## Private message trigger. (Leave blank to disable)
	set tvmaze(ptrigger) ""
	##
	## Date format. (URL: http://tcl.tk/man/tcl8.4/TclCmd/clock.htm)
	set tvmaze(date)     "%Y-%m-%d"
	##
	## Skip announce for these directories.
	set tvmaze(ignore_dirs) {cd[0-9] dis[ck][0-9] dvd[0-9] codec cover covers extra extras sample subs vobsub vobsubs proof}
	##
	## Genre splitter.
	set tvmaze(splitter) " / "
	##
	## Pre line regexp.
	##  We need to reconstruct the full path to the release. Since not all
	##  pre scripts use the same format we'll use regexp to extract what we
	##  need from the pre logline and reconstuct it ourselves.
	##
	## Default f00-pre example:
	set tvmaze(pre-regexp) {^"(.[^"]+)" ".[^"]*" ".[^"]*" "(.[^"]+)"}
	set tvmaze(pre-path)   "%2/%1"
	##
	## Default eur0-pre example:
	#set tvmaze(pre-regexp) {^"(.[^"]+)"}
	#set tvmaze(pre-path)   "%1"
	##
	## Disable announces. (0 = No, 1 = Yes)
	## TVMAZE is used on NEWDIR, TVMAZE-PRE on PRE,
	## TVMAZE-MSGFULL is used with !tv-trigger with specific episode
	## and TVMAZE-MSGSHOW is used with !tv-trigger with only a showname or invalid episode number
	set ${np}::disable(TVMAZE)         0
	set ${np}::disable(TVMAZE-PRE)     0
	set ${np}::disable(TVMAZE-MSGFULL) 0
	set ${np}::disable(TVMAZE-MSGSHOW) 0
	##
	## Convert empty or zero variables into something else.
	set ${np}::zeroconvert(%tvmaze_show_name)                 "N/A"
	set ${np}::zeroconvert(%tvmaze_show_id)                   "N/A"
	set ${np}::zeroconvert(%tvmaze_show_genres)               "N/A"
	set ${np}::zeroconvert(%tvmaze_show_country)              "N/A"
	set ${np}::zeroconvert(%tvmaze_show_network)              "N/A"
	set ${np}::zeroconvert(%tvmaze_show_status)               "N/A"
	set ${np}::zeroconvert(%tvmaze_show_latest_title)         "N/A"
	set ${np}::zeroconvert(%tvmaze_show_latest_episode)       "N/A"
	set ${np}::zeroconvert(%tvmaze_show_latest_airdate)       "N/A"
	set ${np}::zeroconvert(%tvmaze_show_next_title)           "N/A"
	set ${np}::zeroconvert(%tvmaze_show_next_episode)         "N/A"
	set ${np}::zeroconvert(%tvmaze_show_next_airdate)         "N/A"
	set ${np}::zeroconvert(%tvmaze_show_url)                  "N/A"
	set ${np}::zeroconvert(%tvmaze_show_type)                 "N/A"
	set ${np}::zeroconvert(%tvmaze_show_premiered)            "N/A"
	set ${np}::zeroconvert(%tvmaze_show_started)              "N/A"
	set ${np}::zeroconvert(%tvmaze_show_ended)                "N/A"
	set ${np}::zeroconvert(%tvmaze_show_airtime)              "N/A"
	set ${np}::zeroconvert(%tvmaze_show_runtime)              "N/A"
	set ${np}::zeroconvert(%tvmaze_episode_url)               "N/A"
	set ${np}::zeroconvert(%tvmaze_episode_season_episode)    "N/A"
	set ${np}::zeroconvert(%tvmaze_episode_season)            "N/A"
	set ${np}::zeroconvert(%tvmaze_episode_number)            "N/A"
	set ${np}::zeroconvert(%tvmaze_episode_original_airdate)  "N/A"
	set ${np}::zeroconvert(%tvmaze_episode_title)             "N/A"
	##
	##################################################

	## Version
	set tvmaze(version) "20200222"
	## Useragent
	set tvmaze(useragent) "Mozilla/5.0 (Windows; U; Windows NT 6.0; en-US; rv:1.9.0.5) Gecko/2008120122 Firefox/3.0.5"

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
		variable tvmaze
		variable scriptName
		variable scriptFile

		# If string length nonzero, we require TclCurl
		if {[string length $tvmaze(proxyhost)]} {
			if {[catch {package require TclCurl}]} {
				${ns}::Error "\"TclCurl\" package not found, unloading script."
				return -code -1
			}
		} else {
			if {[catch {package require http 2}]} {
				${ns}::Error "\"http\" package not found, unloading script."
				return -code -1
			}
			# We want at least protocol TLS 1.X
			if {[catch {package require tls 1.7}]} {
				${ns}::Error "\"tls\" package not found, unloading script."
				return -code -1
			}
		}

		set variables(TVMAZE-MSGFULL) "%tvmaze_show_name %tvmaze_show_id %tvmaze_show_genres %tvmaze_show_country %tvmaze_show_network %tvmaze_show_status %tvmaze_show_latest_title %tvmaze_show_latest_episode %tvmaze_show_latest_airdate %tvmaze_show_next_title %tvmaze_show_next_episode %tvmaze_show_next_airdate %tvmaze_show_url %tvmaze_show_type %tvmaze_show_premiered %tvmaze_show_started %tvmaze_show_ended %tvmaze_show_airtime %tvmaze_show_runtime %tvmaze_episode_url %tvmaze_episode_season_episode %tvmaze_episode_season %tvmaze_episode_number %tvmaze_episode_original_airdate %tvmaze_episode_title"
		set variables(TVMAZE) "$variables(NEWDIR) $variables(TVMAZE-MSGFULL)"
		set variables(TVMAZE-PRE) "$variables(PRE) $variables(TVMAZE-MSGFULL)"
		set variables(TVMAZE-MSGSHOW) $variables(TVMAZE-MSGFULL)

		set theme_file [file normalize "[pwd]/[file rootname $scriptFile].zpt"]
		if {[file isfile $theme_file]} {
			${np}::loadtheme $theme_file true
		}

		## Register the event handler.
		foreach event $events {
			lappend postcommand($event) $scriptName
		}

		if {([info exists tvmaze(ctrigger)]) && (![string equal $tvmaze(ctrigger) ""])} {
			bind pub -|- $tvmaze(ctrigger) ${ns}::Trigger
		}
		if {([info exists tvmaze(ptrigger)]) && (![string equal $tvmaze(ptrigger) ""])} {
			bind msg -|- $tvmaze(ptrigger) ${ns}::Trigger
		}

		${ns}::Debug "Loaded successfully (Version: $tvmaze(version))."

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
		putlog "\[ngBot\] TVMaze :: $msg"
	}

	proc Error {error} {
		putlog "\[ngBot\] TVMaze Error :: $error"
	}

	proc ConvertDate {string} {
		variable tvmaze

		set tmp [string map { "/" " " } $string]
		if {![regexp {^\d} $tmp] && [set i [string last " " $tmp]] != -1} {
			set tmp [string replace $tmp $i $i ", "]
		}

		if {![string equal "$tmp" ""] && [catch {clock format [clock scan $tmp] -format $tvmaze(date)} result] == 0} {
			set string $result
		}

		return $string
	}

	proc Trigger {args} {
		variable ns
		variable np
		variable tvmaze

		if {[llength $args] == 5} {
			${np}::checkchan [lindex $args 2] [lindex $args 3]

			set trigger $tvmaze(ctrigger)
		} else {
			set trigger $tvmaze(ptrigger)
		}

		set text [lindex $args [expr { [llength $args] - 1 }]]
		set target [lindex $args [expr { [llength $args] - 2 }]]

		if {[string equal $text ""]} {
			${np}::sndone $target "TVMaze Syntax :: $trigger <string> (eg: $trigger Stargate SG-1 S02E10)"
			return 1
		}

		if {[catch {${ns}::FindInfo $text [list] "false"} logData] != 0} {
			${np}::sndone $target "TVMaze Error :: $logData"
			return 0
		}

		## Display full series/episode info if episode_url exists
		if {![string equal [lindex $logData 19] ""]} {
			${np}::sndone $target [${np}::ng_format "TVMAZE-MSGFULL" "none" $logData]
		} else {
			${np}::sndone $target [${np}::ng_format "TVMAZE-MSGSHOW" "none" $logData]
		}

		return 1
	}

	proc LogEvent {event section logData} {
		variable ns
		variable np
		variable tvmaze

		if {[string compare -nocase $event "NEWDIR"] == 0} {
			set target "TVMAZE"

			set release [lindex $logData 0]
		} else {
			set target "TVMAZE-PRE"

			if {(![info exists tvmaze(pre-regexp)]) || (![info exists tvmaze(pre-path)])} {
				${ns}::Error "Your pre-regexp or pre-path variables are not set"
				return 0
			}

			if {[catch {regexp -inline -nocase -- $tvmaze(pre-regexp) $logData} error] != 0} {
				${ns}::Error $error
				return 0
			}

			if {[set cookies [regexp -inline -all -- {%([0-9]+)} $tvmaze(pre-path)]] == ""} {
				${ns}::Error "Your pre-path contains no valid cookies"
				return 0
			}

			set release $tvmaze(pre-path)
			foreach {cookie number} $cookies {
				regsub -- $cookie $release [lindex $error $number] release
			}
		}

		## Check the release directory is ignored.
		foreach ignore [split $tvmaze(ignore_dirs) " "] {
			if {[string match -nocase $ignore [file tail $release]]} {
				return 1
			}
		}

		foreach path $tvmaze(sections) {
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

				if {($empty == 0) || ([string is true -strict $tvmaze(announce-empty)])} {
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
						show_url show_type show_premiered \
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
		variable tvmaze

		set data [GetFromApi "https://api.tvmaze.com/singlesearch/shows?embed%5b%5d=previousepisode&embed%5b%5d=nextepisode&q=" $show]
		if {[string equal "Connection" [string range $data 0 9]]} {
			return -code error $data
		}

		set matches [regexp -inline -nocase -all -- {\"name\":\"(.*?)\"} $data]
		if {[string length $matches] == 0} {
			return -code error "No results found for \"$show\""
		}

		regexp {\"id\":(\d+)} $data -> info(show_id)
		regexp {\"name\":\"(.*?)\"} $data -> info(show_name)
		regexp {\"url\":\"(.*?)\"} $data -> info(show_url)
		set info(show_url) [regsub "http" $info(show_url) "https"]
		regexp {\"status\":\"(.*?)\"} $data -> info(show_status)
		regexp {\"country\":.*?\"code\":\"(.*?)\"} $data -> info(show_country)
		regexp {\"premiered\":\"(.*?)\"} $data -> info(show_premiered)
		regexp {\"type\":\"(.*?)\"} $data -> info(show_type)
		regexp {\"runtime\":(\d+)} $data -> info(show_runtime)

		# use webchan or network as "show_network"
		if {[regexp {\"network\":null,} $data] && ![regexp {\"webChannel\":null,} $data]} {
			regexp {\"webChannel\":.*?\"name\":\"(.*?)\",\"} $data -> info(show_network)
		} elseif {![regexp {\"network\":null,} $data] && [regexp {\"webChannel\":null,} $data]} {
			regexp {\"network\":.*?\"name\":\"(.*?)\"} $data -> info(show_network)
		}

		# genre(s)
		regexp {\"genres\":\[(\".+?)\]} $data -> show_genres
		if {[info exists show_genres]} {
			set info(show_genres) [list]
			foreach genre [split $show_genres ","] {
				lappend info(show_genres) [string range [string trim $genre] 1 end-1]
			}
			set info(show_genres) [join $info(show_genres) $tvmaze(splitter)]
		}

		# air day(s) time
		regexp {\"schedule\":\{\"time\":\"(\d.*?)\"} $data -> show_airtime
		regexp {\"schedule\":.*?\"days\":\[(\".*?)\]} $data -> show_airdays

		# day(s) present
		if {[info exists show_airdays]} {
			set info(show_airtime) [list]
			foreach day [split $show_airdays ","] {
				lappend info(show_airtime) [string range [string trim $day] 1 end-1]
			}

			# with time available
			if {[info exists show_airtime]} {
				set info(show_airtime) "[join $info(show_airtime) $tvmaze(splitter)] $show_airtime"
			# no time available, join only days
			} else {
				set info(show_airtime) "[join $info(show_airtime) $tvmaze(splitter)]"
			}
		# no day(s)
		} elseif {[info exists show_airtime]} {
			# in case of time
			set info(show_airtime) $show_airtime
		}

		# started year
		if {[info exists info(show_premiered)]} {
			set info(show_started) [string range $info(show_premiered) 0 3]
		}

		# ended year
		set show_embedded ""
		regexp {\"_embedded\":(.*)} $data -> show_embedded

		# latest episode
		regexp {\"previousepisode\":.*?\"name\":\"(.*?)\",\"} $show_embedded -> info(show_latest_title)
		regexp {\"previousepisode\":.*?\"airdate\":\"(.*?)\"} $show_embedded -> info(show_latest_airdate)

		# merge episode Ep/Season
		regexp {\"previousepisode\":.*?\"season\":(\d+),} $show_embedded -> show_latest_episode_season
		regexp {\"previousepisode\":.*?\"number\":(\d+),} $show_embedded -> show_latest_episode_number
		if {[info exists show_latest_episode_season] && [info exists show_latest_episode_number]} {
			set show_latest_episode_season [format S%02d $show_latest_episode_season]
			set show_latest_episode_number [format E%02d $show_latest_episode_number]
			set info(show_latest_episode) "${show_latest_episode_season}${show_latest_episode_number}"
		}

		regexp {\"nextepisode\":.*?\"name\":\"(.*?)\",\"} $show_embedded -> info(show_next_title)
		regexp {\"nextepisode\":.*?\"airdate\":\"(.*?)\"} $show_embedded -> info(show_next_airdate)

		# merge episode Ep/Season
		regexp {\"nextepisode\":.*?\"season\":(\d+),} $show_embedded -> show_next_episode_season
		regexp {\"nextepisode\":.*?\"number\":(\d+),} $show_embedded -> show_next_episode_number
		if {[info exists show_next_episode_season] && [info exists show_latest_episode_number]} {
			set show_next_episode_season [format S%02d $show_next_episode_season]
			set show_next_episode_number [format E%02d $show_next_episode_number]
			set info(show_next_episode) "${show_next_episode_season}${show_next_episode_number}"
		}

		if {[info exists info(show_status)] && [string equal "$info(show_status)" "Ended"]} {
			if {[info exists info(show_next_airdate)]} {
				set info(show_ended) [string range $info(show_next_airdate) 0 3]
			} elseif {[info exists info(show_latest_airdate)]} {
				set info(show_ended) [string range $info(show_latest_airdate) 0 3]
			}
		}

		# in case of SXXEXX
		if {![string equal "$season" ""] && ![string equal "$epnumber" ""]} {
			set data [GetFromApi "https://api.tvmaze.com/shows/${info(show_id)}/episodebynumber?season=${season}&number=${epnumber}" ""]
			if {[string equal "Connection" [string range $data 0 9]]} {
				return -code error $data
			}

			regexp {\"name\":\"(.*?)\"} $data -> info(episode_title)
			regexp {\"url\":\"(.*?)\"} $data -> info(episode_url)
			set info(episode_url) [regsub "http" $info(episode_url) "https"]
			regexp {\"airdate\":\"(.*?)\"} $data -> info(episode_original_airdate)

			regexp {\"season\":(\d+)} $data -> info(episode_season)
			regexp {\"number\":(\d+)} $data -> info(episode_number)

			if {[info exists info(episode_season)] && [info exists info(episode_number)]} {
				set episode_season [format S%02d $info(episode_season)]
				set episode_number [format E%02d $info(episode_number)]
				set info(episode_season_episode) "${episode_season}${episode_number}"
			}
		}

		return [array get info]
	}

	proc GetFromApi {uri query} {
		variable tvmaze

		# init data
		set data ""

		if {[string length $tvmaze(proxyhost)]} {
			if {![string equal "" "$query"]} {
				set uri "$uri[curl::escape $query]"
			}
			curl::transfer -url "$uri" -proxy $tvmaze(proxyhost):$tvmaze(proxyport) -proxytype $tvmaze(proxytype) -proxyuserpwd $tvmaze(proxyuser):$tvmaze(proxypass) -useragent $tvmaze(useragent) -bodyvar token -timeoutms $tvmaze(timeout)
			set data $token
		} else {
			if {![string equal "" "$query"]} {
				# Verify if we can use quoteString or the older mapReply
				# else fallback to the original formatQuery
				# Use "commands" as quoteString is an alias (of mapReply)
				if {[string length [info commands ::http::quoteString]]} {
					set uri "$uri[::http::quoteString $query]"
				} elseif {[string length [info procs ::http::mapReply]]} {
					set uri "$uri[::http::mapReply $query]"
				} else {
					set uri "$uri[::http::formatQuery $query]"
				}
			}
			::http::config -useragent $tvmaze(useragent)
			::http::register https 443 [list ::tls::socket -autoservername true]
			set token [::http::geturl "$uri" -timeout $tvmaze(timeout)]

			if {![string equal -nocase [::http::status $token] "ok"]} {
				return "Connection [::http::status $token]"
			}

			## Its pointless checking the http status, its always 200. Errors are
			## redirected on the server to an 'Unknown Page' error page.

			set data [::http::data $token]
		}

		return $data
	}
}

if {[string equal "" $::ngBot::plugin::TVMaze::np]} {
	::ngBot::plugin::TVMaze::init
}
