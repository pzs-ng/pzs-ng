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
    set blowkey("#chan1") "mYkeY1"
    set blowkey("#chan2") "MyKey2"
    set blowkey("#chan3") "mykey"
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
    ## set to "*" to disable
    variable keyxUsers "=siteops"
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
        if { $index==-1 && ![string equal $mainChan ""]} { return blowkey($mainChan) }
        return $blowkey([lindex [array names blowkey] $index])
    }

    ####
    # Blow::matchChan
    #
    # checks if $chan is defined in the blowkey array
    #
    proc matchChan {chan} {
        variable blowkey
        if { [string match *{"$chan"}* [array names blowkey]] } { return True }
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

    ####
    # Blow::keyxNick
    #
    # Called on nick events. Moves the blowkey from the old nickname to the new nickname, if one exists.
    #
    proc keyxNick {nick host hand chan newnick} {
        if {[info exists blowkey($nick)]} {
            set blowkey($newnick) blowkey($nick)
            unset blowkey($nick)
        }
    }

    ####
    # !!! BROKEN !!!
    # Blow::keyxInit
    # 
    # Called when someone initializes a key exchange
    #
    proc keyxInit { nick host handle text dest } {
        variable ns
        variable np
        variable keyx
        variable keyxUsers
        variable publicKey
        variable privateKey
        variable blowkey
        if {![IsTrue $keyx]} { 
            ${ns}::PutLog "keyxchange is disabled!"
            return
        }
        
        set ftpUser [${ns}::GetFtpUser $nick]
        if {[${ns}::GetInfo $ftpUser ftpGroup ftpFlags]} {
            if {[${np}::rightscheck $keyxUsers $ftpUser $ftpGroup $ftpFlags]} {
                ## keyx
                set arg_len [string length $text]
                if { ($arg_len < 192) || ($arg_len > 193) } { return }
                ## IMPORTANT: the 2 variables should be at least 200 bytes each!
                ## You might run into a crash, if they are too small!
                set myprivkey [string repeat x 300]
                set mypubkey [string repeat x 300]
                DH1080gen "$myprivkey" "$mypubkey"
                set hispubkey [lindex $text 1]
                DH1080comp "$myprivkey" "$hispubkey"
                set blowkey($nick) $hispubkey
                unset hispubkey
            } else {
                ## Comment this if you dont want your bot to respond at all
                #putserv "NOTICE $nick :no thanks"
            }
        } else {
            ## Comment this if you dont want your bot to respond at all
            #putserv "NOTICE $nick :no thanks"
        }
    }

    ####
    # !!! BROKEN !!!
    # Blow::keyxFinish
    #
    # Finalizes the key exchange
    #
    proc keyxFinish { nick host handle text dest } {
        variable ns
        variable np
        variable keyx
        variable keyxUsers
        variable publicKey
        variable privateKey
        variable blowkey
        
        set ftpUser [${ns}::GetFtpUser "$nick"]
        if {[${ns}::GetInfo $ftpUser ftpGroup ftpFlags]} {
            if {[IsTrue $keyx] && ![${np}::rightscheck $keyxUsers $ftpUser $ftpGroup $ftpFlags]} {
                set arg_len [string length $text]
                if { ($arg_len < 194) || ($arg_len > 195) } { return }
                set hispubkey [lindex $text 1]
                DH1080comp "$myprivkey" "$hispubkey"
                ## $hispubkey now contains the secret key!
                set blowkey($nick) $hispubkey
                unset hispubkey
            } else {
                ## Comment this if you dont want your bot to respond at all
                #putserv "NOTICE $nick :no thanks"
            }
        } else {
            ## Comment this if you dont want your bot to respond at all
            #putserv "NOTICE $nick :no thanks"
        }
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
            if {[string equal $topicUsers "*"]} {
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
    proc encryptedIncomingHandler {nick host hand chan arg} {
        variable ns
        variable blowkey
        global lastbind blowEncryptedMessage
        set key [${ns}::getKey $chan]
        if {[string equal $key ""]} {return}
        set tmp [split [decrypt $key $arg]]
        #${ns}::PutLog "received encrypted message: $tmp"
        foreach item [binds pub] {
            if {[string equal [lindex $item 2] "+OK"]} { continue }
            if {![string equal [lindex $item 1] "-|-"] && ![matchattr $hand [lindex $item 1] $chan]} { continue }
            set blowEncryptedMessage 1
            set lastbind [lindex $item 2]
            ## execute bound proc
            if {[string equal [lindex $item 2] [lindex $tmp 0]]} { 
                # Use "eval" to expand the callback script, for example:
                # bind pub -|- !something [list PubCommand MyEvent]
                # proc PubCommand {event nick host hand chan text} {...}
                eval [lindex $item 4] \$nick \$host \$hand \$chan {[join [lrange $tmp 1 end]]}
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
        variable allowUnencrypted
        variable trustedUsers
        #${ns}::PutLog "unencryptedIncomingHandler {$from $keyword $text} allowUnencrypted: $allowUnencrypted"

        if { [IsTrue $allowUnencrypted] || [string match ":+OK *" [string range $text [string first : $text] end]] } {
            if {![string equal $trustedUsers "*"]} {
                set nick [lindex [split $from "!"] 0]
                set host [lindex [split $from "!"] 1]
                set chan [lindex $text 0]
                set hand [nick2hand $nick $chan]

                if {[catch {set ftpUser [${ns}::GetFtpUser [set nick]]} error]} {
                    ${ns}::PutLog "Error while grabbing \$ftpuser by nick $nick, ftpUser: $ftpUser, error: $error"
                }
                if {[${ns}::GetInfo $ftpUser ftpGroup ftpFlags]} {
                    if {[${np}::rightscheck $trustedUsers $ftpUser $ftpGroup $ftpFlags]} { 
                        return 0
                    }
                    ${ns}::PutLog "Rightscheck failed, user: $ftpUser, data: [lindex [split $text ":"] 1]"
                }
                ${ns}::PutLog "GetInfo failed, user: $ftpUser"
                return 1
            }
        } else {
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
        set group ""; set flags ""

        if {![catch {set handle [open "$location(USERS)/$ftpUser" r]} error]} {
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

    proc PutHelp {text {option ""}} {
        variable ns
        variable maxLength
        variable allowUnencrypted

        set channel [lindex [split $text] 1]
        set key [${ns}::getKey $channel]
        if {[IsTrue [${ns}::encryptThis $text]] && ![string equal $key ""]} {
            if {[IsTrue [${ns}::matchChan $channel]]} {
                ## we have to encrypt this
                set command [lindex $text 0]
                set pos [string first : $text]
                if {$pos==-1 || [string equal $pos " "]} {putlog "\[ngBot\] Blow Error :: BOGUS MESSAGE";return}
                incr pos
                set message [string range $text $pos end]

                if {[expr {[string length $message] > $maxLength}]} {
                    ## start splitting
                    ${ns}::breakLine $message lineArr
                    foreach {i msg} [array get lineArr] {
                        if {[string equal $option ""]} {
                            puthelp2 "$command [lindex $text 1] :+OK [encrypt $key $msg]"
                        } else {
                            puthelp2 "$command [lindex $text 1] :+OK [encrypt $key $msg]" $option
                        }
                    }
                } else {
                    ## message is short enough to fit on one line
                    if {[string equal $option ""]} {
                        puthelp2 "$command [lindex $text 1] :+OK [encrypt $key $message]"
                    } else { puthelp2 "$command [lindex $text 1] :+OK [encrypt $key $message]" $option }
                }
            } elseif {[IsTrue $allowUnencrypted]} {
                ## message for undefined channel
                if {[string equal $option ""]} { puthelp2 "$text" } else { puthelp2 "$text" $option}
            }
        } else {
            ## we're sending a command (MODE/JOIN/WHATEVER)
            if {[string equal $option ""]} { puthelp2 "$text"} else { puthelp2 "$text" $option}
        }
    }

    proc PutServ {text {option ""}} {
        variable ns
        variable maxLength
        variable allowUnencrypted

        set channel [lindex [split $text] 1]
        set key [${ns}::getKey $channel]
        if {[IsTrue [${ns}::encryptThis $text]] && ![string equal $key ""]} {
            if {[IsTrue [${ns}::matchChan $channel]]} {
                ## we have to encrypt this
                set command [lindex $text 0]
                set pos [string first : $text]
                if {$pos==-1 || [string equal $pos " "]} {putlog "\[ngBot\] Blow Error :: BOGUS MESSAGE";return}
                incr pos
                set message [string range $text $pos end]

                if {[expr {[string length $message] > $maxLength}]} {
                    ## start splitting
                    ${ns}::breakLine $message lineArr
                    foreach {i msg} [array get lineArr] {
                        if {[string equal $option ""]} {
                            putserv2 "$command [lindex $text 1] :+OK [encrypt $key $msg]"
                        } else {
                            putserv2 "$command [lindex $text 1] :+OK [encrypt $key $msg]" $option
                        }
                    }
                } else {
                    ## message is short enough to fit on one line
                    if {[string equal $option ""]} {
                        putserv2 "$command [lindex $text 1] :+OK [encrypt $key $message]"
                    } else { putserv2 "$command [lindex $text 1] :+OK [encrypt $key $message]" $option }
                }
            } elseif {[IsTrue $allowUnencrypted]} {
                ## message for undefined channel
                if {[string equal $option ""]} { putserv2 "$text" } else { putserv2 "$text" $option}
            }
        } else {
            ## we're sending a command (MODE/JOIN/WHATEVER)
            if {[string equal $option ""]} { putserv2 "$text"} else { putserv2 "$text" $option}
        }
    }

    proc PutQuick {text {option ""}} {
        variable ns
        variable maxLength
        variable allowUnencrypted

        set channel [lindex [split $text] 1]
        set key [${ns}::getKey $channel]
        if {[IsTrue [${ns}::encryptThis $text]] && ![string equal $key ""]} {
            if {[IsTrue [${ns}::matchChan $channel]]} {
                ## we have to encrypt this
                set command [lindex $text 0]
                set pos [string first : $text]
                if {$pos==-1 || [string equal $pos " "]} {putlog "\[ngBot\] Blow Error :: BOGUS MESSAGE";return}
                incr pos
                set message [string range $text $pos end]

                if {[expr {[string length $message] > $maxLength}]} {
                    ## start splitting
                    ${ns}::breakLine $message lineArr
                    foreach {i msg} [array get lineArr] {
                        if {[string equal $option ""]} {
                            putquick2 "$command [lindex $text 1] :+OK [encrypt $key $msg]"
                        } else {
                            putquick2 "$command [lindex $text 1] :+OK [encrypt $key $msg]" $option
                        }
                    }
                } else {
                    ## message is short enough to fit on one line
                    if {[string equal $option ""]} { 
                        putquick2 "$command [lindex $text 1] :+OK [encrypt $key $message]"
                    } else { putquick2 "$command [lindex $text 1] :+OK [encrypt $key $message]" $option }
                }
            } elseif {[IsTrue $allowUnencrypted]} {
                ## message for undefined channel
                if {[string equal $option ""]} { putquick2 "$text" } else { putquick2 "$text" $option}
            }
        } else {
            ## we're sending a command (MODE/JOIN/WHATEVER)
            if {[string equal $option ""]} { putquick2 "$text"} else { putquick2 "$text" $option}
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

        if {![string equal $trustedUsers "*"] || ![string equal $topicUsers "*"]} {
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

        interp alias {} putquick {} ${ns}::PutQuick
        interp alias {} putserv {} ${ns}::PutServ
        interp alias {} puthelp {} ${ns}::PutHelp
        #catch {rename ${ns}::PutQuick ::putquick}
        #catch {rename ${ns}::PutServ  ::putserv}
        #catch {rename ${ns}::PutHelp  ::puthelp}


        ## Set disable(SETTOPIC) to 1 if you dont want the bot to message the chan with the new topic
        set disable(SETTOPIC)    1
        set disable(GETTOPIC)    0
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
            bind nick - * ${ns}::keyxNick
            #bind notc - "DH1080_INIT *" ${ns}::keyxInit
            #bind notc - "DH1080_FINISH *" ${ns}::keyxFinish
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

        ## Remove the SETTOPIC and GETTOPIC values from msgtypes(DEFAULT)
        foreach event $events {
            if {[info exists msgtypes(DEFAULT)] && [set pos [lsearch -exact $msgtypes(DEFAULT) $event]] !=  -1} {
                set msgtypes(DEFAULT) [lreplace $msgtypes(DEFAULT) $pos $pos]
            }
        }

        #catch {unbind evnt -|- prerehash ${ns}::deinit}
        catch {unbind pub - +OK ${ns}::encryptedIncomingHandler}
        catch {unbind pub - ${cmdpre}topic ${ns}::IrcTopic}
        catch {unbind raw - PRIVMSG ${ns}::unencryptedIncomingHandler}
        catch {unbind msg - !bckeydel bckeydel}
        catch {unbind nick - * ${ns}::keyxNick}
        #catch {unbind notc - "DH1080_INIT" ${ns}::keyxInit}
        #catch {unbind notc - "DH1080_FINISH" ${ns}::keyxFinish}

        namespace delete $ns
        return
    }
}
