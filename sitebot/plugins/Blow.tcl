#################################################################################
# ngBot - Blow plugin v0.1 original concept by poci                             #
#################################################################################
# These peeps have my thanks: neoxed, meij, god-emper, al gore and jesus
#
# FEATURES:
# - split lines if they become too long
# - enable/disable key exchange, or just allow it for a selected group of people
# - prevent the bot from sending unencrypted PRIVMSG/NOTICE.
# - set/read topic from irc (!topic) or set it from site (site topic #chan topic)
# - integrates perfectly with pzs-ng and NickDb
#
# DOCS:
# 1. Edit the plugin theme (Blow.zpt) for the *TOPIC announces.
# 
# 2. Add the following to your glftpd.conf:
# site_cmd        TOPIC           EXEC            /bin/ng-topic.sh
# custom-topic 1 =siteops !*
#
# 3. Config the stuff below
#
#################################################################################

namespace eval ::ngBot::plugin::Blow {
	variable ns [namespace current]
	variable np [namespace qualifiers [namespace parent]]

	## Config Settings ###############################
	variable blowkey
	##
	## Set the blowfish keys for each channel here. You can have as many
	## targets as you want.
	set blowkey(#chan1) "mYkeY1"
	set blowkey(#chan2) "MyKey2"
	set blowkey(#chan3) "mykey"
	##
	## Use the blowfish key of the channel listed in mainChan for unknown
	## targets. Doesn't work when key exchanged is enabled. Case sensetive.
	## (Set to "" to disable)
	##    Example: variable mainChan "#chan1"
	variable mainChan "#chan1"
	##
	## Max character length unencrypted. 305 is a safe bet for both UnrealIrcd
	## and Hybrid (EFnet).
	variable maxLength 305
	##
	## Split at this character. You probably want to split at spaces.
	variable splitChar " "
	##
	## Respond to unencrypted messages. (1/true or 0/false)
	## This includes CTCP and also DCC. This means you will not
	## be able to get on the partyline via IRC.
	## Set this to false if you dont want to deal with them. (Recommended)
	variable allowUnencrypted false
	##
	## Key Exchange Settings #########################
	##
	## Note: All keys are stored in memory and are forgotten on
	##       rehash/reset/etc.
	##
	## Enable key exchange. (1/true or 0/false)
	variable keyx true
	##
	## Restrict which users allowed to key exchange with the bot.
	## (Set to "" to disable)
	##    Example: variable keyxUsers "=siteops"
	variable keyxUsers ""
	##
	## Respond to unencrypted private messages and initiate a key exchange
	## before replying. The data sent back will be encrypted.
	## (1/true or 0/false)
	variable keyxAllowUnencrypted true
	##
	## Time to wait in seconds for a client to complete a key exchange
	## handshake. Some older version of FiSH for mIRC can take upwards of
	## 60 seconds to reply.
	variable keyxTimeout 120
	##
	## Path to "DH1080_tcl.so". This file is REQUIRED for key exchange to work.
	## Get it from http://fish.secure.la/. Compile it yourself if you're a
	## paranoid geek (Recommended).
	variable blowso "scripts/blow/DH1080_tcl.so"
	##
	## NickDb Settings ###############################
	##
	## NickDb allows you to link IRC users to their FTP accounts. With this
	## enabled you can restrict IRC commands only to respond to specific users
	## based on their groups.
	##
	## If the following options are disabled, NickDb is NOT required.
	##
	## Only respond to trusted users. (Set to "" to disable)
	##    Example: variable trustedUsers "!=notthisgroup *"
	variable trustedUsers ""
	##
	## Only allow these users to get/set new topics via the
	## GETTOPIC/SETTOPIC ftpd commands. (Set to "" to disable)
	##    Example: variable topicUsers "=siteops"
	#variable topicUsers "=siteops"
	variable topicUsers ""
	##
	## TOPIC Settings ###############################
	##
	## Above setting also related
	##
	## Set disable(SETTOPIC) to 1 if you dont want the bot to message the
	## chan with the new topic
	set ${np}::disable(SETTOPIC)    1
	set ${np}::disable(GETTOPIC)    0
	##
	## Trigger (Leave blank to disable)
	variable topictrigger "!topic"
	##
	##################################################

	variable blowversion "20171120"

	variable events [list "SETTOPIC" "GETTOPIC"]

	variable scriptFile [info script]

	interp alias {} IsTrue {} string is true -strict
	interp alias {} IsFalse {} string is false -strict

	####
	# Blow::Debug
	#
	# Pretty self-explanatory
	#
	proc Debug {msg} {
		variable np
		variable ${np}::debugmode
		if {[IsTrue [set ${np}::debugmode]]} {
			putlog "\[ngBot\] Blow :: $msg"
		}
	}

	####
	# Blow::Error
	#
	# Pretty self-explanatory
	#
	proc Error {error} {
		putlog "\[ngBot\] Blow Error :: $error"
	}

	####
	# Blow::SetTopic
	#
	# Sets a new topic for a channel
	#
	proc SetTopic {channel topic} {
		variable ns
		#set topic [join $topic]
		set key [${ns}::getKey $channel]
		if {[IsTrue [${ns}::matchChan $channel]]} {
			putquick "TOPIC $channel :$topic"
			return True
		}
		return False
	}

	####
	# Blow::GetTopic
	#
	# Returns the current topic for $channel
	#
	proc GetTopic {channel} {
		variable ns
		set topic [topic $channel]
		set key [${ns}::getKey $channel]
		if {[string equal $key ""]} { return $topic }
		
		if {[IsTrue [${ns}::matchChan $channel]] && [string equal [lindex $topic 0] "+OK"]} {
			set topic [decrypt $key [lindex $topic 1]]
		}
		return $topic
	}

	####
	# Blow::breakLine
	#
	# Breaks input into parts
	#
	proc breakLine {line lineArr} {
		variable ns
		variable maxLength
		variable splitChar
		upvar $lineArr broken
		#${ns}::debug "line: $line"
		set length [string length $line]

		set pos 0
		## round UP
		set runs [expr round([expr $length/$maxLength]+0.5)]
		## length of each new line
		set partSize [expr round([expr $length/$runs]+0.5)]
		#${ns}::debug "maxLength: $maxLength, length: $length, runs: $runs, partsize: $partSize"

		for {set i 0} {$i<$runs} {incr i} {
			## heavy stuff
			set newPart [string range $line $pos [expr $pos + $partSize]]

			set broken($i) [string range $line $pos [expr [string last " " $newPart]+$pos]]
			set pos [string last " " [string range $line $pos [expr $pos + $partSize]]]; incr pos
			#${ns}::debug "$i: $runs :: $pos"
		}
		return True
	}

	####
	# Blow::isMsgEncrypted
	#
	# Call this function to see if a message is encrypted
	#
	proc isMsgEncrypted {} {
		global blowEncryptedMessage
		if {[info exists blowEncryptedMessage]} {return True}
		return False
	}

	####
	# Blow::reEscape
	#
	# Escapes all non-alfanumeric for use in a regexp
	#
	proc reEscape {str} {
		regsub -all {\W} $str {\\&}
	}


	####
	# Blow::getKey
	#
	# Returns key associated with $target
	#
	proc getKey {target} {
		variable keyx
		variable blowkey
		variable mainChan

		set names [array names blowkey]
		# Old configs had example blowkeys as blowkey("#chan1") etc, accommodate for this.
		if {[set index [lsearch -regexp $names "(?i)^\"?[reEscape $target]\"?$"]] == -1} {
			if {![string equal $mainChan ""] && [info exists blowkey($mainChan)] && ![IsTrue $keyx]} {
				return $blowkey($mainChan)
			}

			return
		}

		return $blowkey([lindex $names $index])
	}

	####
	# Blow::matchChan
	#
	# checks if $chan is defined in the blowkey array
	#
	proc matchChan {target} {
		variable blowkey

		# Old configs had example blowkeys as blowkey("#chan1") etc, accommodate for this.
		if {[lsearch -regexp [array names blowkey] "(?i)^\"?[reEscape $target]\"?$"] != -1} { return 1 }

		return 0
	}

	####
	# Blow::encryptThis
	#
	# checks if the outgoing command matches PRIVMSG or NOTICE
	#
	proc encryptThis {text} {
		foreach type [list "PRIVMSG " "NOTICE " "TOPIC "] {
			if {[string equal -nocase -length [string length $type] $text $type]} {
				return 1
			}
		}

		return 0
	}

	proc is_trustedusers {} {
		variable trustedUsers

		if {[info exists trustedUsers]} {
			if {[regexp -- {^([*]|)$} $trustedUsers]} { return 1 }
		}

		return 0
	}

	proc is_topicusers {} {
		variable topicUsers

		if {[info exists topicUsers]} {
			if {[regexp -- {^([*]|)$} $topicUsers]} { return 1 }
		}

		return 0
	}

	####
	# Blow::keyx_nick
	#
	# Called on nick events. Moves the blowkey from the old nickname to the new nickname, if one exists.
	#
	proc keyx_nick {nick host hand chan newnick} {
		variable blowkey
		variable keyxinit
		variable keyxtimer
		variable keyxqueue

		foreach var [list "blowkey" "keyxinit" "keyxtimer" "keyxqueue"] {
			if {[info exists $var($nick)]} {
				set $var($newnick) $var($nick)
				unset $var($nick)
			}
		}
	}

	proc keyx_generate {target name_public {name_private ""}} {
		variable blowinit

		if {![info exists blowinit($target)]} {
			upvar $name_public key_public
			if {$name_private != ""} { upvar $name_private key_private }

			## IMPORTANT: the 2 variables should be at least 200 bytes each!
			## You might run into a crash, if they are too small!
			set key_private [string repeat x 300]
			set key_public [string repeat x 300]

			# Overwrites the variables with the generated values.
			DH1080gen $key_private $key_public

			# Only set blowinit if we're initiating the handshake.
			if {$name_private == ""} { set blowinit($target) $key_private }

			return 1
		}

		return 0
	}

	proc keyx_init {target} {
		variable ns
		variable np

		if {[${ns}::keyx_generate $target my_key_pub]} {
			putquick2 "NOTICE $target :DH1080_INIT $my_key_pub"
			${ns}::Debug "keyx_init: Sending DH1080 public key to $target."
		}
	}

	proc keyx_bind {nick host handle text dest} {
		variable np
		variable ns
		variable keyx
		variable blowkey
		variable blowinit
		variable keyxUsers

		if {![IsTrue $keyx]} {
			${ns}::Debug "Key exchange is disabled!"
			return
		}

		if {![is_trustedusers]} {
			set ftp_user [${ns}::GetFtpUser $nick]

			if {![${ns}::GetInfo $ftp_user ftp_group ftp_flags]} {
				return
			}

			if {![${np}::rightscheck $keyxUsers $ftp_user $ftp_group $ftp_flags]} {
				return
			}
		}

		set text [split $text]
		set len [string length [lindex $text 1]]
		switch -- [string toupper [lindex $text 0]] {
			DH1080_INIT {
				if { ($len > 178) || ($len < 182) } {
					if {[${ns}::keyx_generate $nick my_key_pub my_key_prv]} {
						putquick2 "NOTICE $nick :DH1080_FINISH $my_key_pub"
						set his_key_pub [lindex $text 1]
						DH1080comp $my_key_prv $his_key_pub
						set blowkey($nick) $his_key_pub
						${ns}::Debug "keyx_bind: Received DH1080 public key from $nick. Sending DH1080 plublic key to $nick."

						${ns}::keyx_queue_flush $nick

						return 1
					}
				} else {

				}
			}
			DH1080_FINISH {
				if { ($len > 178) || ($len < 182) } {
					if {[info exists blowinit($nick)]} {
						set his_key_pub [lindex $text 1]
						DH1080comp $blowinit($nick) $his_key_pub
						set blowkey($nick) $his_key_pub
						unset blowinit($nick)
						${ns}::Debug "keyx_bind: Received DH1080 public key from $nick."

						${ns}::keyx_queue_flush $nick

						return 1
					}
				} else {

				}
			}
			DH1024_INIT {
				# NOT SUPPORTED!
			}
			DH1024_FINISH {
				# NOT SUPPORTED!
			}
		}
	}

	proc keyx_queue {target type command text {option ""}} {
		variable ns
		variable keyxtimer
		variable keyxqueue
		variable keyxTimeout

		# Clean up the queue variable if the keyx handshake isnt completed
		# within 120 seconds.
		if {![info exists keyxtimer($target)]} {
			set timeout [expr { [string is integer $keyxTimeout] ? $keyxTimeout : 120 }]
			set keyxtimer($target) [utimer $timeout [list ${ns}::keyx_queue_delete $target]]
		}

		lappend keyxqueue($target) $type $command $text $option
	}

	proc keyx_queue_delete {target} {
		variable keyxqueue
		variable keyxtimer

		catch {killutimer $keyxtimer($target)}
		catch {unset keyxtimer($target)}
		catch {unset keyxqueue($target)}
	}

	proc keyx_queue_flush {target} {
		variable ns
		variable blowkey
		variable keyxqueue

		if {![info exists keyxqueue($target)] || ![info exists blowkey($target)]} { return }

		foreach {type command text option} $keyxqueue($target) {
			${ns}::put_encrypted $target $type $command $text $blowkey($target) $option
		}

		${ns}::keyx_queue_delete $target
	}

	####
	# Blow::IrcTopic
	#
	# Wraps up the arguments of !topic nicely and shows topic OR sends new topic to chan
	proc IrcTopic {nick host handle channel text} {
		variable ns
		variable np
		variable topicUsers
		if {[string equal $text ""] } {
			set topic [${ns}::GetTopic $channel]
			${np}::sndall GETTOPIC DEFAULT [${np}::ng_format "GETTOPIC" "DEFAULT" \"$topic\"]
		} else {
			if {[${ns}::is_topicusers]} {
				if {[IsTrue [${ns}::SetTopic $channel "$text"]]} {
					${ns}::Debug "Topic for $channel set: $text"
				}

				return
			}
			set ftpUser [${ns}::GetFtpUser $nick]
			if {[${ns}::GetInfo $ftpUser ftpGroup ftpFlags] && [${np}::rightscheck $topicUsers $ftpUser $ftpGroup $ftpFlags] && [IsTrue [${ns}::SetTopic $channel "$text"]]} {
				${ns}::Debug "Topic for $channel set: $text"
			} else {
				${ns}::Debug "Unauthorized user: $nick"
			}
		}
	}

	####
	# Blow::encryptedIncomingHandler
	#
	# Takes care of incoming messages and checks if they are bound to any command
	#
	proc encryptedIncomingHandler {nick uhost handle args} {
		variable ns
		variable keyx
		variable blowkey
		variable blowinit

		global lastbind blowEncryptedMessage

		# Find out if its a PUB or MSG bind.
		if {[set ispub [expr { [llength $args] == 2 ? 1 : 0 }]]} {
			# From the eggdrop Tcl manual: "PUBM binds are processed before PUB binds."
			set bind { "pubm" "pub" }
			set text [lindex $args 1]
			set target [lindex $args 0]
		} else {
			# From the eggdrop Tcl manual: "MSGM binds are processed before MSG binds."
			set bind { "msgm" "msg" }
			set text [lindex $args 0]
			set target $nick
		}

		set key [${ns}::getKey $target]

		# If we received an encrypted private message, don't have a key
		# for the user and it was trigger by a MSG bind, reinit a key exchange.
		if {[IsTrue $keyx] && ![IsTrue [${ns}::matchChan $target]] && !$ispub} {
			if {![info exists blowinit($target)]} {
				${ns}::keyx_init $target

				putserv "PRIVMSG $target :Unable to decrypt last message, redid key exchange. Please try again."
			}
		}

		if {[string equal $key ""]} { return }

		set tmp [split [decrypt $key $text]]
		# From the eggdrop server help: "exclusive-binds:
		#   This setting configures PUBM and MSGM binds to be exclusive of PUB and MSG binds."
		set mExecuted 0
		#${ns}::debug "received encrypted message: $tmp"
		foreach bindtype $bind {
			foreach item [binds $bindtype] {
				if {[string equal [lindex $item 2] "+OK"]} { continue }
				if {![string equal [lindex $item 1] "-|-"] && ![matchattr $handle [lindex $item 1] $target]} { continue }
				set blowEncryptedMessage 1
				set lastbind [lindex $item 2]
				set targchan "*"
				if {[string equal "$bindtype" "pubm"]} {
					set targchan [lindex "$lastbind" 0]
					if {[string equal "$targchan" "%"]} {
						set targchan "*"
					}
					set lastbind [lrange "$lastbind" 1 end]
				}
				## execute bound proc
				if {[string match "$targchan" "$target"]} {
					if {([string equal "$bindtype" "pubm"] || [string equal "$bindtype" "msgm"]) && [string match "$lastbind" "$tmp"]} {
						if {[info exists exclusive-binds] && ${::exclusive-binds}} {
							set mExecuted 1
						}
						if {$ispub} {
							eval [lindex $item 4] \$nick \$uhost \$handle \$target \$tmp
						} else {
							eval [lindex $item 4] \$nick \$uhost \$handle \$tmp
						}
					} elseif {!$mExecuted && [string equal "$lastbind" [lindex $tmp 0]]} {
						# Use "eval" to expand the callback script, for example:
						# bind pub -|- !something [list PubCommand MyEvent]
						# proc PubCommand {event nick uhost handle chan text} {...}
						if {$ispub} {
							eval [lindex $item 4] \$nick \$uhost \$handle \$target {[join [lrange $tmp 1 end]]}
						} else {
							eval [lindex $item 4] \$nick \$uhost \$handle {[join [lrange $tmp 1 end]]}
						}
					}
				}
				unset blowEncryptedMessage
			}
		}
	}

	####"
	# Blow::unencryptedIncomingHandler
	#
	# Takes care of unencrypted incoming messages. Ignores *ALL* input from unknown users if $trustedUsers is set.
	#
	proc unencryptedIncomingHandler {from keyword text} {
		variable ns
		variable np
		variable keyx
		variable trustedUsers
		variable allowUnencrypted
		variable keyxAllowUnencrypted

		#${ns}::debug "unencryptedIncomingHandler {$from $keyword $text} allowUnencrypted: $allowUnencrypted"

		set nick [lindex [split $from "!"] 0]
		#set uhost [lindex [split $from "!"] 1]
		set target [lindex [split $text] 0]
		#set handle [nick2hand $nick]

		if { [IsTrue $allowUnencrypted] || [string match ":+OK *" [string range $text [string first : $text] end]] } {
			if {![${ns}::is_trustedusers]} {
				if {[catch {set ftpUser [${ns}::GetFtpUser $nick]} error]} {
					${ns}::Debug "Error while grabbing \$ftpuser by nick $nick, ftpUser: $ftpUser, error: $error"
				}
				if {[${ns}::GetInfo $ftpUser ftpGroup ftpFlags]} {
					if {[${np}::rightscheck $trustedUsers $ftpUser $ftpGroup $ftpFlags]} { 
						return 0
					}
					${ns}::Debug "Rightscheck failed, user: $ftpUser, data: [lindex [split $text ":"] 1]"
					return 1
				}
				${ns}::Debug "GetInfo failed, user: $ftpUser"
				return 1
			}
		} else {
			# If we received an unencrypted private message and its target isn't a
			# channel, evaluate it. The output commands will deal with the
			# key exchange initiation and encryption.
			if {[IsTrue $keyx] && [regexp -- {^[^&#]} $target] && [IsTrue $keyxAllowUnencrypted]} {
				catch {unset blowkey($target)}

				return 0
			}

			return 1
		}
	}

	####
	# Blow::GetInfo
	#
	# gets $group and $flags from the userfile
	#
	proc GetInfo {ftpUser groupVar flagsVar} {
		variable ns
		variable np
		variable ${np}::location
		upvar $groupVar group $flagsVar flags

		set file "$location(USERS)/$ftpUser"
		# Linux will give an error if you open a directory and try to read from it.
		if {![file isfile $file]} {
			${ns}::Error "Invalid user file for \"$ftpUser\" ($file)."
			return 0
		}

		set group ""; set flags ""
		if {[catch {set handle [open $file r]} error] == 0} {
			set data [read $handle]
			close $handle
			foreach line [split $data "\n"] {
				switch -exact -- [lindex $line 0] {
					"FLAGS" {set flags [lindex $line 1]}
					"GROUP" {set group [lindex $line 1]}
				}
			}
			return 1
		} else {
			${ns}::Error "Unable to open user file for \"$ftpUser\" ($error)."
			return 0
		}
	}

	# Outgoing messages
	proc put_bind {type text {option ""}} {
		variable ns
		variable keyx
		variable blowinit
		variable maxLength
		variable allowUnencrypted

		# Only allow valid procs to be called.
		if {[lsearch -regexp [list "quick" "serv" "help"] "(?i)^$type$"] == -1} { return }

		set ltext [split $text]
		set target [lindex $ltext 1]
		set key [${ns}::getKey $target]

		# Key exchange is turned on, message is not being sent to a channel,
		# we're supposed to encrypt this type of message and there isnt
		# already already a key for this target.
		if {[IsTrue $keyx] && [regexp -- {^[^&#]} $target] && \
			[IsTrue [${ns}::encryptThis $text]] && ![IsTrue [${ns}::matchChan $target]]} {
			 # Initiate if we haven't already.
			if {![info exists blowinit($target)]} {
				# Will set blowinit($target)
				${ns}::keyx_init $target
			}
		}

		if {([IsTrue [${ns}::encryptThis $text]] && ![string equal $key ""]) || [info exists blowinit($target)]} {
			if {![string equal $key ""] || [info exists blowinit($target)]} {
				## we have to encrypt this
				set command [lindex $ltext 0]

				set pos [string first : $text]
				if {$pos == -1 || [string equal $pos " "]} {
					${ns}::Error "BOGUS MESSAGE"
					return
				}
				set message [string range $text [incr pos] end]

				if {[string length $message] > $maxLength} {
					## start splitting
					${ns}::breakLine $message lineArr
					foreach {{} msg} [array get lineArr] {
						${ns}::put_encrypted $target $type $command $msg $key $option
					}
				} else {
					## message is short enough to fit on one line
					${ns}::put_encrypted $target $type $command $message $key $option
				}
			} elseif {[IsTrue $allowUnencrypted]} {
				## message for undefined channel
				${ns}::put_unencrypted $type $text $option
			}
		} else {
			## we're sending a command (MODE/JOIN/WHATEVER)
			${ns}::put_unencrypted $type $text $option
		}
	}

	proc put_unencrypted {type text {option ""}} {
		if {$option == ""} {
			put${type}2 $text
		} else {
			put${type}2 $text $option
		}
	}

	proc put_encrypted {target type command text key {option ""}} {
		variable ns

		if {![string equal $key ""]} {
			if {$option == ""} {
				put${type}2 "$command $target :+OK [encrypt $key $text]"
			} else {
				put${type}2 "$command $target :+OK [encrypt $key $text]" $option
			}
		} else {
			${ns}::keyx_queue $target $type $command $text $option
		}
	}

	####
	# Blow::LogEvent
	#
	# Called on logevents (facilitated by ngBot)
	#
	proc LogEvent {event section logData} {
		variable ns
		${ns}::Debug "LogEvent {$event $section $logData} called"
		if {![string equal "SETTOPIC" $event]} {return 1}
			set channel [lindex $logData 0]
			set topic [lindex $logData 1]
			if {[IsFalse [${ns}::SetTopic $channel $topic]]} {
			${ns}::Debug "Unable to set topic"
		}
		return 1
	}

	####
	# Blow::Init
	#
	# Called on initialization
	#
	proc init {args} {
		variable ns
		variable np
		variable keyx
		variable blowso
		variable events
		variable topicUsers
		variable trustedUsers
		variable allowUnencrypted
		variable scriptFile
		variable blowversion
		variable topictrigger
		variable ${np}::cmdpre
		variable ${np}::variables
		variable ${np}::disable
		variable ${np}::msgtypes
		variable ${np}::precommand

		if {![${ns}::is_trustedusers] || ![${ns}::is_topicusers]} {
			if {![namespace exists [namespace parent]::NickDb]} {
				${ns}::Error "Unable to find NickDb plugin."
				return -code -1
			}

			namespace import [namespace parent]::NickDb::*
		}

		if {[IsTrue $keyx]} {
			if {[catch {load $blowso} error]} {
				${ns}::Error "$error"
				return -code -1
			}
		}

		set aliases [interp aliases]
		foreach cmd {putquick putserv puthelp} {
			if {[lsearch $aliases $cmd] != -1 || [catch {info args $cmd}] == 0 || [info commands $cmd] == ""} {
				${ns}::Error "Output procs have already been renamed. Make sure no other blowfish scripts are loaded and \002.restart\002."
				return -code -1
			}
		}

		## Intercept putquick, putserv and puthelp, and replace it with our own version
		catch {rename ::putquick ::putquick2}
		catch {rename ::putserv ::putserv2}
		catch {rename ::puthelp ::puthelp2}

		interp alias {} putquick {} ${ns}::put_bind "quick"
		interp alias {} putserv {} ${ns}::put_bind "serv"
		interp alias {} puthelp {} ${ns}::put_bind "help"

		set variables(SETTOPIC)  "%channel %topic"
		set variables(GETTOPIC)  "%topic"

		set theme_file [file normalize "[pwd]/[file rootname $scriptFile].zpt"]
                if {[file isfile $theme_file]} {
                        ${np}::loadtheme $theme_file true
                }

		## Register the event handler.
		foreach event $events {
			lappend msgtypes(DEFAULT) "$event"
		}

		## Initialize our encrypted incoming handler
		## Binds to input from irc
		bind pub - +OK ${ns}::encryptedIncomingHandler
		bind msg - +OK ${ns}::encryptedIncomingHandler
		bind raw - PRIVMSG ${ns}::unencryptedIncomingHandler
		if {[IsTrue $keyx]} {
			bind nick - * ${ns}::keyx_nick
			bind notc - "DH1080_INIT *" ${ns}::keyx_bind
			bind notc - "DH1080_FINISH *" ${ns}::keyx_bind
		}
		if {([info exists topictrigger]) && (![string equal $topictrigger ""])} {
			bind pub - $topictrigger ${ns}::IrcTopic
		}

		## Binds to input from the ftp
		lappend precommand(SETTOPIC) ${ns}::LogEvent

		putlog "\[ngBot\] Blow :: Loaded successfully (Version: $blowversion)."
	}

	####
	# Blow::DeInit
	#
	# Called on rehash; unregisters the event handler.
	#
	proc deinit {args} {
		variable ns
		variable np
		variable events
		variable keyxtimer
		variable topictrigger
		variable keyx
		variable ${np}::cmdpre
		variable ${np}::msgtypes

		## Remove event callbacks.
		if {[info commands "putquick2"] != ""} {
			interp alias {} putquick {}
			catch {rename ::putquick2 ::putquick}
		}
		if {[info commands "putserv2"] != ""} {
			interp alias {} putserv {}
			catch {rename ::putserv2  ::putserv}
		}
		if {[info commands "puthelp2"] != ""} {
			interp alias {} puthelp {}
			catch {rename ::puthelp2  ::puthelp}
		}

		foreach timer [array names keyxtimer] {
			catch {killutimer $keyxtimer($timer)}
		}

		## Remove the SETTOPIC and GETTOPIC values from msgtypes(DEFAULT)
		foreach event $events {
			if {[info exists msgtypes(DEFAULT)] && [set pos [lsearch -exact $msgtypes(DEFAULT) $event]] !=  -1} {
				set msgtypes(DEFAULT) [lreplace $msgtypes(DEFAULT) $pos $pos]
			}
		}

		# Remove binds
		catch {unbind pub - +OK ${ns}::encryptedIncomingHandler}
		catch {unbind msg - +OK ${ns}::encryptedIncomingHandler}
		catch {unbind raw - PRIVMSG ${ns}::unencryptedIncomingHandler}
		if {[IsTrue $keyx]} {
			catch {unbind nick - * ${ns}::keyx_nick}
			catch {unbind notc - "DH1080_INIT *" ${ns}::keyx_bind}
			catch {unbind notc - "DH1080_FINISH *" ${ns}::keyx_bind}
		}
		if {([info exists topictrigger]) && (![string equal $topictrigger ""])} {
			catch {unbind pub - $topictrigger ${ns}::IrcTopic}
		}

		namespace delete $ns
		return
	}
}
