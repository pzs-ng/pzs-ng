#########################################################
# This script is part of zipscript-c made by dark0n3    #
# Modified by (c) daxxar <daxxar@mental.mine.nu>        #
# This is configuration for dZSbot.tcl                  #
#########################################################
# BASIC CONFIG                                          #
#########################################################
# Locations of all the usual files, change is usually only required
# if you have another glroot than /glftpd/ =)
set location(GLLOG)         "/glftpd/ftp-data/logs/glftpd.log"
set location(PASSWD)        "/glftpd/etc/passwd"
set location(USERS)         "/glftpd/ftp-data/users/"

# Are you using glftpd2? Set to 1 if you are, 0 if not.
set output_glftpd2          "1"

# What types of messages are counted as the different types of output
# (for disabling etc)
set msgtypes(RACE)          "NEWDIR DELDIR INCOMPLETE NUKE UNNUKE PRE RACE SFV UPDATE HALFWAY NEWLEADER COMPLETE STATS"
set msgtypes(DEFAULT)       "MSGINVITE BADMSGINVITE INVITE LOGIN LOGOUT"

# Sitename is for the %sitename-cookie in announces :)
set sitename                "dS"
# Prefix on commands (f. x. !dsfree if cmdpre is !ds, !free if cmdpre is !, etc)
set cmdpre                  "!ds"
# List of bnc's (used for BNC-testing with the <prefix>bnc command in channel)
set bnc(LIST)               "msn.test1.com:123 msn.test2.com:345 msn.test3.com:567"
# Username and password to use for testing bncs (must exist with this box's ip added)
set bnc(USER)               "bncuser"
set bnc(PASS)               "bncpass"
# Number of seconds to wait before giving up on connecting to the BNCs
set bnc(TIMEOUT)            "2"

# What sections are we announcing for? (Remeber to define path and type below)
set sections                "GAMES APPS MP3 0DAY MV"

# The sections we allow checking of stats on, remember that the number in 
# parantheses must be the same as the number of the section in glftpd config =)
set statsection(0)          "MP3"
set statsection(1)          "0DAY"
set statsection(2)          "ISO"

# These are the binaries the script will use. They are usually best left
# as their default values :) (btw, check that they all are where they should
# and chmodded to eXecutable :) (chmod 755 <file>)
set binary(CAT)             "/glftpd/bin/cat"
set binary(WHO)             "/glftpd/bin/sitewho"
set binary(BW)              "/glftpd/bin/bandwidth.sh"
set binary(STATS)           "/glftpd/bin/stats"
set binary(PASSCHK)         "/glftpd/bin/passchk"
set binary(NCFTPLS)         "/usr/bin/ncftpls"
set binary(BNCTEST)         "/glftpd/bin/bnctest.sh"
set binary(INCOMPLETE)      "/glftpd/bin/incomplete-list.sh"
set binary(DF)              "/glftpd/bin/df"

# Definition of hd devices (for freespace) Syntax is "disk sections".
# 'sections' is purely for cosmetic uses (output)
set device(0)               "/dev/hda1 ISO"
set device(1)               "/dev/hdc1 MP3"

# Set up paths for all the sections (wildcards) and their
# types. Types are to decide if things get shown or not ;)
set paths(GAMES)            "/site/incoming/games/*"
set type(GAMES)             "RACE"
set paths(APPS)             "/site/incoming/apps/*"
set type(APPS)              "RACE"
set paths(MP3)              "/site/incoming/mp3/*"
set type(MP3)               "RACE"
set paths(0DAY)             "/site/incoming/0day/*"
set type(0DAY)              "RACE"
set paths(MV)               "/site/incoming/musicvideos/*"
set type(MV)                "RACE"

# Which channels we announce the different things to.
# Space separated lists, you can have more than one channel :)
set chanlist(GAMES)         "#dserver"
set chanlist(APPS)          "#dserver"
set chanlist(MP3)           "#dserver"
set chanlist(0DAY)          "#dserver"
set chanlist(MV)            "#dserver"
set chanlist(DEFAULT)       "#dserver"
set chanlist(INVITE)        "#dserver"
set chanlist(WELCOME)       "#dserver"

# Are we disabling the announce of this stuff?
# If its set to 1, we wont show it :)
set disable(NEWDIR)         0
set disable(DELDIR)         0
set disable(PRE)            0
set disable(INVITE)         0
set disable(NUKE)           0
set disable(UNNUKE)         0
set disable(MSGINVITE)      0
set disable(BADMSGINVITE)   0
set disable(LOGIN)          1
set disable(LOGOUT)         1
set disable(INCOMPLETE)     0
set disable(RACE)           0
set disable(SFV)            1
set disable(UPDATE)         0
set disable(HALFWAY)        0
set disable(NEWLEADER)      0
set disable(COMPLETE)       0
set disable(STATS)          0
set disable(WELCOME)        0
set disable(DEFAULT)        1

#########################################################
# ADVANCED CONFIG, EDIT CAREFULLY                       #
#########################################################

# Space separated list of wildcards that we match against
# paths to see if we announce it or not =) 
set denypost "*Complete* *COMPLETE*"
# Do not show it if this user gets nuked
set hidenuke "UNKNOWN"

## Defining variables for announce
#
# Example:
#  set variables(PRE) "%pf %user %group %pregroup %files %mbytes"
#  set announce(PRE)  "-%sitename- \[%section\] %user@%group launches new %pregroup-pre called %release (%mbytesM in %filesF)"
#
# Special variables:
#  %pf      = path filter, must be the first parameter and contain full path of the release, it defines:
#   %release = Last directory in path ( /site/xxx/marissa-exxtasy/cd1 => cd1 )
#   %path    = Second last directory in path ( /site/xxx/marissa-exxtasy/cd1 => marissa-exxtasy )
#   %relname = all directories after those defined in paths
#              ( paths(ISO) = "/site/xxx/" does: /site/xxx/marissa-exxtasy/cd1 => marissa-exxtasy/cd1 )
#
#  %bold    = bolds text
#  %uline   = underlines text
#  %section = current section name
#
#  %nuker   = name of nuker
#  %mult    = nuke multiplier
#  %reason  = nuke reason
#  %nukee   = ppl that got nuked

set variables(NEWDIR)       "%pf %user %group %tagline"
set variables(DELDIR)       "%pf %user %group %tagline"
set variables(LOGIN)        "%hostmask %ip %user %group %tagline"
set variables(LOGOUT)       "%hostmask %ip %user %group %tagline"
set variables(INVITE)       "%ircnick %user %group"
set variables(DEFAULT)      "%pf %msg"

## Converts empty or zero variable to something else if defined
#
# Example:
#  set zeroconvert(%user) "NoOne"

set zeroconvert(%group) "NoGroup"

## Splits output line to smaller pieces
#
# To disable set it to "\n"

set splitter(CHAR) "\n"


## RANDOMIZING OUTPUT
#
# Example:
#  set random(NEWDIR-0)       "-%sitename- \[%section\] + %user@%group creates a directory called %release"
#  set random(NEWDIR-1)       "-%sitename- \[%section\] + %user@%group makes a directory called %release"
#  set random(NEWDIR-2)       "-%sitename- \[%section\] + %user@%group does mkdir %release"
#   TYPE --------^   ^
#         ID --------^
#
#  set announce(NEWDIR) "random 3"
#   TYPE ---------^        ^    ^
#         RANDOM ----------^    ^
#             # OF IDS ---------^

set announce(NEWDIR)        "-%sitename- \[%section\] + New Release: %path/%bold%release%bold (%relname) by %bold%user%bold@%group"
set announce(DELDIR)        "-%sitename- \[%section\] - Directory deleted: %path/%bold%release%bold by %bold%user%bold@%group"
set announce(BW)            "-%sitename- \[%section\] - %bold%uploads%bold uploads @ %bold%upspeed%boldkb/sec - %bold%downloads%bold downloads @ %bold%dnspeed%bold kb/sec - total: %bold%transfers%bold transfers @ %bold%totalspeed%boldkb/sec"
set announce(LOGIN)         "-%sitename- \[LOGIN\] + %bold%user%bold@%group has logged in"
set announce(LOGOUT)        "-%sitename- \[LOGOUT\] - %bold%user%bold@%group has logged out"
set announce(NUKE)          "-%sitename- \[%section\] + %path/%bold%release%bold was %ulinenuked %mult%ulinex by %bold%nuker%bold - reason: %reason - nukees: %nukees"
set announce(UNNUKE)        "-%sitename- \[%section\] + %path/%bold%release%bold was %ulineunnuked %mult%ulinex by %bold%nuker%bold - reason: %reason - nukees: %nukees"
set announce(INVITE)        "-%sitename- \[INVITE\] + %bold%user%bold@%group invited himself as %bold%ircnick%bold"
set announce(MSGINVITE)     "-%sitename- \[INVITE\] + %bold%user%bold@%group invited himself as %bold%ircnick%bold"
set announce(BADMSGINVITE)  "-%sitename- \[INTRUDER\] - %bold%ircnick%bold (%host) tried to invite himself with invalid login!"
set announce(WELCOME)       "Welcome to dS's sitechan. Type !dshelp for help."
set announce(DEFAULT)       "-%sitename- \[%section\] - %msg"

# In this announce you can use %free, %total and %used for the free/total/used sum of all disks
# and you can use %devices. Each device specified (see above (set device(0) etc)) will be inserted
# with stats as specified in announce(FREE-DEV) (see below)
set announce(FREE)          "-%sitename- \[FREE\] - %devices"
# Stats for each device (Replaces %devices in announce(FREE) for each device), here you can use
# %free, %total and %used :)
set announce(FREE-DEV)      "%bold%free%bold of %bold%total%bold (used %percentage%) "
