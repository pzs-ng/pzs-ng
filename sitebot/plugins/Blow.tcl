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
# 1. Add the following to your themefile:
# GETTOPIC = "%b{[topic ]} %topic"
# SETTOPIC = "%b{[topic ]} %channel %topic"
# 
# 2. Add the following to your glftpd.conf:
# site_cmd        TOPIC           EXEC            /bin/ng-topic.sh
# custom-topic 1 =siteops !*
#
# 3. Config the stuff below
#
# TODO:
# - write docs. ;)
# - keyxchange, i dont need this so this is not a top priority ;)
#
#################################################################################

namespace eval ::ngBot::plugin::Blow {
	variable ns [namespace current]
	variable np [namespace qualifiers [namespace parent]]

	## Config Settings ###############################
	variable events
	variable blowkey
	##
	## Path to the DH1080_tcl.so lib. Get it from http://fish.sekure.us/
	## Compile it yourself if you're a paranoid geek (recommended)
	variable blowso "scripts/blow/DH1080_tcl.so"
	##
	## Set the blowkeys here. You can have as many targets as you want.
	set blowkey(#chan1) "mYkeY1"
	set blowkey(#chan2) "MyKey2"
	set blowkey(#chan3) "mykey"
	##
	## Max character length unencrypted. 305 is a safe bet for both UnrealIrcd and Hybrid (EFnet)
	variable maxLength 305
	##
	## Split at this character. You probably want to split at spaces.
	variable splitChar " "
	##
	## Send unencrypted data if the target isnt listed in blowkey? Set to either True or False
	## Respond to unencrypted data?
	## (set this to False if you dont want to deal with unencrypted data (Recommended)
	variable allowUnencrypted False
	##
	## Do you want to enable the key-exchange for all users? Set to either True or False
	variable keyx True
	##
	## Which users are allowed to key-exchange? (Only works when you've enabled keyx)
	## set to "*" to disable (UNIMPLEMENTED)
	#variable keyxUsers "=siteops"
	##
	## Use key of mainChan for unknown targets when keyx = False
	## Set to "" to disable
	variable mainChan "#chan1"
	##
	## NickDB: only respond trustedUsers
	## set to "*" to disable
	variable trustedUsers "!=notthisgroup *"
	##
	## NickDB/TOPIC: only these users are allowed to set a new topic
	## set to "*" to disable
	variable topicUsers "=siteops"
	##
	##################################################
	
	set events [list "SETTOPIC" "GETTOPIC"]
	#bind evnt -|- prerehash ${ns}::deinit

	interp alias {} IsTrue {} string is true -strict
	interp alias {} IsFalse {} string is false -strict

	####
	# Blow::PutLog
	#
	# Pretty self-explanitory
	#
	proc PutLog {msg} {
		putlog "\[ngBot\] Blow :: $msg"
	}

	####
	# Blow::SetTopic
	#
	# Sets a new topic for a channel
	#
	proc SetTopic {channel topic} {
		variable ns
		set topic [join $topic]
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
		#${ns}::PutLog "line: $line"
		set length [string length $line]

		set pos 0
		## round UP
		set runs [expr round([expr $length/$maxLength]+0.5)]
		## length of each new line
		set partSize [expr round([expr $length/$runs]+0.5)]
		#${ns}::PutLog "maxLength: $maxLength, length: $length, runs: $runs, partsize: $partSize"

		for {set i 0} {$i<$runs} {incr i} {
			## heavy stuff
			set newPart [string range $line $pos [expr $pos + $partSize]]

			set broken($i) [string range $line $pos [expr [string last " " $newPart]+$pos]]
			set pos [string last " " [string range $line $pos [expr $pos + $partSize]]]; incr pos
			#${ns}::PutLog "$i: $runs :: $pos"
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
	# Blow::getKey
	#
	# Returns key associated with $target
	#
	proc getKey {target} {
		variable mainChan
		variable blowkey

		set index [lsearch -exact [string tolower [join [array names blowkey] " "]] [string tolower $target]]
		if {$index == -1 && ![string equal $mainChan ""]} { return blowkey($mainChan) }

		return $blowkey([lindex [array names blowkey] $index])
	}

	####
	# Blow::matchChan
	#
	# checks if $chan is defined in the blowkey array
	#
	proc matchChan {target} {
		variable blowkey

		if {[lsearch -regexp [array names blowkey] "^\[\"\]?$target\[\"\]?$"] != -1} { return True }

		return False
	}

	####
	# Blow::encryptThis
	#
	# checks if the outgoing command matches PRIVMSG or NOTICE
	#
	proc encryptThis {text} {
		if { ![string compare -nocase -length 7 $text "PRIVMSG"] || ![string compare -nocase -length 7 $text "NOTICE"] || ![string compare -nocase -length 5 $text "TOPIC"]} { return True }
		return False
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
			${np}::debug "keyx_init: Sending DH1080 public key to $target."
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
			${np}::debug "Key exchange is disabled!"
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
		switch -nocase -- [lindex $text 0] {
			DH1080_INIT {
				if { ($len > 178) || ($len < 182) } {
					if {[${ns}::keyx_generate $nick my_key_pub my_key_prv]} {
						putquick2 "NOTICE $nick :DH1080_FINISH $my_key_pub"
						set his_key_pub [lindex $text 1]
						DH1080comp $my_key_prv $his_key_pub
						set blowkey($nick) $his_key_pub
						${np}::debug "keyx_bind: Received DH1080 public key from $nick. Sending DH1080 plublic key to $nick."

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
						${np}::debug "keyx_bind: Received DH1080 public key from $nick."

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

		# Clean up the queue variable if the keyx handshake isnt completed
		# within 120 seconds.
		if {![info exists keyxtimer($target)]} {
			set keyxtimer($target) [utimer 120 [list ${ns}::keyx_queue_delete $target]]
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
			${np}::sndall GETTOPIC DEFAULT [ng_format "GETTOPIC" "DEFAULT" \"$topic\"]
		} else {
			set ftpUser [${ns}::GetFtpUser $nick]
			if {[${ns}::is_topicusers]} {
				if {[IsTrue [${ns}::SetTopic $channel "$text"]]} {
					${ns}::PutLog "Topic for $channel set: $text"
				}
			} elseif {[${ns}::GetInfo $ftpUser ftpGroup ftpFlags] && [${np}::rightscheck $topicUsers $ftpUser $ftpGroup $ftpFlags] && [IsTrue [${ns}::SetTopic $channel "$text"]]} {
				${ns}::PutLog "Topic for $channel set: $text"
			} else {
				${ns}::PutLog "Unauthorized user: $nick"
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
			set bind "pub"
			set text [lindex $args 1]
			set target [lindex $args 0]
		} else {
			set bind "msg"
			set text [lindex $args 0]
			set target $nick
		}

		set key [${ns}::getKey $target]

		# If we received an encrypted private message, don't have a key
		# for the user and it was trigger by a MSG bind, reinit a key exchange.
		if {[IsTrue $keyx] && ![IsTrue [${ns}::matchChan $target]] && !$ispub} {
			# ${ns}::getKey can set key to $mainChan, make sure its empty.
			set key ""
			if {![info exists blowinit($target)]} {
				${ns}::keyx_init $target

				putserv "PRIVMSG $target :Unable to decrypt last message, redid key exchange. Please try again."
			}
		}

		if {[string equal $key ""]} { return }

		set tmp [split [decrypt $key $text]]
		#${ns}::PutLog "received encrypted message: $tmp"
		foreach item [binds $bind] {
			if {[string equal [lindex $item 2] "+OK"]} { continue }
			if {![string equal [lindex $item 1] "-|-"] && ![matchattr $handle [lindex $item 1] $target]} { continue }
			set blowEncryptedMessage 1
			set lastbind [lindex $item 2]
			## execute bound proc
			if {[string equal [lindex $item 2] [lindex $tmp 0]]} { 
				# Use "eval" to expand the callback script, for example:
				# bind pub -|- !something [list PubCommand MyEvent]
				# proc PubCommand {event nick uhost handle chan text} {...}
				if {$ispub} {
					eval [lindex $item 4] \$nick \$uhost \$handle \$target {[join [lrange $tmp 1 end]]}
				} else {
					eval [lindex $item 4] \$nick \$uhost \$handle {[join [lrange $tmp 1 end]]}
				}
			}
			unset blowEncryptedMessage
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

		#${ns}::PutLog "unencryptedIncomingHandler {$from $keyword $text} allowUnencrypted: $allowUnencrypted"

		set nick [lindex [split $from "!"] 0]
		#set uhost [lindex [split $from "!"] 1]
		set target [lindex [split $text] 0]
		#set handle [nick2hand $nick]

		if { [IsTrue $allowUnencrypted] || [string match ":+OK *" [string range $text [string first : $text] end]] } {
			if {![${ns}::is_trustedusers]} {
				if {[catch {set ftpUser [${ns}::GetFtpUser $nick]} error]} {
					${ns}::PutLog "Error while grabbing \$ftpuser by nick $nick, ftpUser: $ftpUser, error: $error"
				}
				if {[${ns}::GetInfo $ftpUser ftpGroup ftpFlags]} {
					if {[${np}::rightscheck $trustedUsers $ftpUser $ftpGroup $ftpFlags]} { 
						return 0
					}
					${ns}::PutLog "Rightscheck failed, user: $ftpUser, data: [lindex [split $text ":"] 1]"
					return 1
				}
				${ns}::PutLog "GetInfo failed, user: $ftpUser"
				return 1
			}
		} else {
			# If we received an unencrypted private message and its target isn't a
			# channel, evaluate it. The output commands will deal with the
			# encryption.
			if {[IsTrue $keyx] && [regexp -- {^[^&#]} $target]} {
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
		variable np
		variable ${np}::location
		upvar $groupVar group $flagsVar flags

		set file "$location(USERS)/$ftpUser"
		# Linux will give an error if you open a directory and try to read from it.
		if {![file isfile $file]} { return }

		set group ""; set flags ""
		if {![catch {set handle [open $file r]} error]} {
			set data [read $handle]
			close $handle
			foreach line [split $data "\n"] {
				switch -exact -- [lindex $line 0] {
					"FLAGS" {set flags [lindex $line 1]}
					"GROUP" {set group [lindex $line 1]}
				}
			}
			return True
		} else {
			putlog "\[ngBot\] Blow Error :: Unable to open user file for \"$ftpUser\" ($error)."
			return False
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
			# ${ns}::getKey can set key to $mainChan, make sure its empty.
			set key ""
			 # Initiate if we haven't already.
			if {![info exists blowinit($target)]} {
				# Will set blowinit($target)
				${ns}::keyx_init $target
			}
		}

		if {([IsTrue [${ns}::encryptThis $text]] && ![string equal $key ""]) || [info exists blowinit($target)]} {
			if {[IsTrue [${ns}::matchChan $target]] || [info exists blowinit($target)]} {
				## we have to encrypt this
				set command [lindex $ltext 0]

				set pos [string first : $text]
				if {$pos == -1 || [string equal $pos " "]} {
					putlog "\[ngBot\] Blow Error :: BOGUS MESSAGE"
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
		variable blowkey

		if {[info exists blowkey($target)]} {
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
		${ns}::PutLog "LogEvent {$event $section $logData} called"
		if {![string equal "SETTOPIC" $event]} {return 1}
			set channel [lindex $logData 0]
			set topic [lindex $logData 1]
			if {[IsFalse [${ns}::SetTopic $channel $topic]]} {
			${ns}::PutLog "Unable to set topic"
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
		variable ${np}::cmdpre
		variable ${np}::variables
		variable ${np}::disable
		variable ${np}::msgtypes
		variable ${np}::precommand

		if {![${ns}::is_trustedusers] || ![${ns}::is_topicusers]} {
			if {![namespace exists [namespace parent]::NickDb]} {
				putlog "\[ngBot\] Blow Error :: Unable to find NickDb plugin."
				return -code -1
			}

			namespace import [namespace parent]::NickDb::*
		}

		if {[catch {load $blowso} error]} {
			putlog "\[ngBot\] Blow Error :: $error"
			return -code -1
		}

		## Intercept putquick, putserv and puthelp, and replace it with our own version
		catch {rename ::putquick ::putquick2}
		catch {rename ::putserv ::putserv2}
		catch {rename ::puthelp ::puthelp2}

		interp alias {} putquick {} ${ns}::put_bind "quick"
		interp alias {} putserv {} ${ns}::put_bind "serv"
		interp alias {} puthelp {} ${ns}::put_bind "help"
		#interp alias {} putquick {} ${ns}::PutQuick
		#interp alias {} putserv {} ${ns}::PutServ
		#interp alias {} puthelp {} ${ns}::PutHelp
		#catch {rename ${ns}::PutQuick ::putquick}
		#catch {rename ${ns}::PutServ  ::putserv}
		#catch {rename ${ns}::PutHelp  ::puthelp}


		## Set disable(SETTOPIC) to 1 if you dont want the bot to message the chan with the new topic
		set disable(SETTOPIC)	1
		set disable(GETTOPIC)	0
		set variables(SETTOPIC)  "%channel %topic"
		set variables(GETTOPIC)  "%topic"

		## Register the event handler.
		foreach event $events {
			lappend msgtypes(DEFAULT) "$event"
		}

		## Initialize our encrypted incoming handler
		## Binds to input from irc
		bind pub - +OK ${ns}::encryptedIncomingHandler
		bind pub - ${cmdpre}topic ${ns}::IrcTopic
		bind raw - PRIVMSG ${ns}::unencryptedIncomingHandler
		if {[IsTrue $keyx]} {
			bind msg - !bckeydel bckeydel
			bind nick - * ${ns}::keyx_nick
			bind notc - "DH1080_INIT *" ${ns}::keyx_bind
			bind notc - "DH1080_FINISH *" ${ns}::keyx_bind
			bind msg  - "+OK" ${ns}::encryptedIncomingHandler
		}

		## Binds to input from the ftp
		lappend precommand(SETTOPIC) ${ns}::LogEvent

		return
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
		variable ${np}::cmdpre
		variable ${np}::msgtypes

		## Remove event callbacks.
		if {[llength [info commands "putquick2"]] == 1} {
			interp alias {} putquick {}
			#catch {rename ::putquick  {}}
			catch {rename ::putquick2 ::putquick}
		}
		if {[llength [info commands "putserv2"]] == 1} {
			interp alias {} putserv {}
			#catch {rename ::putserv   {}}
			catch {rename ::putserv2  ::putserv}
		}
		if {[llength [info commands "puthelp2"]] == 1} {
			interp alias {} puthelp {}
			#catch {rename ::puthelp   {}}
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

		#catch {unbind evnt -|- prerehash ${ns}::deinit}
		#catch {unbind pub - +OK ${ns}::encryptedIncomingHandler}
		#catch {unbind pub - ${cmdpre}topic ${ns}::IrcTopic}
		#catch {unbind raw - PRIVMSG ${ns}::unencryptedIncomingHandler}
		#catch {unbind msg - !bckeydel bckeydel}
		#catch {unbind nick - * ${ns}::keyx_nick}
		#catch {unbind notc - "DH1080_INIT" ${ns}::keyx_bind}
		#catch {unbind notc - "DH1080_FINISH" ${ns}::keyx_bind}

		namespace delete $ns
		return
	}
}
