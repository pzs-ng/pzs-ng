######################################################################
#      Project Zipscript Next Generation - Sitebot Configuration     #
#                        (originally by Dark0n3)                     #
#                                                                    #
######################################################################
#                                                                    #
#                          IMPORTANT!!                               #
#                          -----------                               #
# This file contains overrides only. You may find that some settings #
# are missing - search in dZSbconf.defaults.tcl for the settings you #
# need and COPY THOSE INTO THIS FILE - do not edit the defaults file #
# - it will be changed! This file, however, will not, which means    #
# that when you upgrade your bot, you can keep this file, replace    #
# the defaults file, and be done with it.                            #
#                                                                    #
######################################################################

set sitename			"pzs-ng"
set cmdpre			"!pn"
set bindnopre			"YES"
set bnc(LIST)			"uk:msn.test1.com:123 de:msn.test2.com:345 se:msn.test3.com:567"
set bnc(USER)			"bncuser"
set bnc(PASS)			"bncpass"
set speed(INCOMING)		"12500"
set speed(OUTGOING)		"12500"
set statsection(0)		"MP3"
set statsection(1)		"0DAY"
set statsection(2)		"ISO"
set binary(PING)		"/bin/ping"
# ping in freebsd is in sbin
#set binary(PING)		"/sbin/ping"
set device(0)			"/dev/da0s1f MP3"
set device(1)			"/dev/ad0s1e ISO"
set disable(TRIGINALLCHAN)	0
set splitter(CHAR)		"|"
set announce(THEMEFILE)		"themes/default.zst"

# Your sections
set sections			"TEST GAMES APPS MP3 0DAY MV"
set paths(TEST)			"/site/test/*"
set type(TEST)			"RACE"
set paths(GAMES)		"/site/incoming/games/*"
set type(GAMES)			"RACE"
set paths(APPS)			"/site/incoming/apps/*"
set type(APPS)			"RACE"
set paths(MP3)			"/site/incoming/mp3/*"
set type(MP3)			"RACE"
set paths(0DAY)			"/site/incoming/0day/*"
set type(0DAY)			"RACE"
set paths(MV)			"/site/incoming/musicvideos/*"
set type(MV)			"RACE"
set mainchan			"#pzs-ng"
set chanlist(TEST)		"$mainchan"
set chanlist(GAMES)		"$mainchan"
set chanlist(APPS)		"$mainchan"
set chanlist(MP3)		"$mainchan"
set chanlist(0DAY)		"$mainchan"
set chanlist(MV)		"$mainchan"
set chanlist(DEFAULT)		"$mainchan"
set chanlist(INVITE)		"$mainchan"
set chanlist(WELCOME)		"$mainchan"
set invite_channels		$chanlist(INVITE)
