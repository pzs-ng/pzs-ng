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

# Some Very Important Settings #
################################
set glroot			"/glftpd/"
set sitename			"pzs-ng"
set cmdpre			"!pn"
set announce(THEMEFILE)		"themes/default.zst"
set mainchan			"#pzs-ng"

# some bins and paths - edit to match your config/os #
######################################################
set location(PASSWD)		"$glroot/etc/passwd"
set location(USERS)		"$glroot/ftp-data/users/"
set location(GLCONF)		"/etc/glftpd.conf"
set location(GROUP)		"$glroot/etc/group"
set glftpdlog(GLLOG01)		"$glroot/ftp-data/logs/glftpd.log"
set loginlog(LOGINLOG01)	"$glroot/ftp-data/logs/login.log"
set binary(CAT)			"$glroot/bin/cat"
set binary(WHO)			"$glroot/bin/sitewho"
set binary(STATS)		"$glroot/bin/stats"
set binary(PASSCHK)		"$glroot/bin/passchk"
set binary(INCOMPLETE)		"$glroot/bin/incomplete-list.sh"
set binary(PING)		"/bin/ping"
set binary(NCFTPLS)		"/usr/local/bin/ncftpls"
set binary(DF)			"/bin/df"
set binary(GLFTPD)		"$glroot/bin/glftpd"
set binary(SHOWLOG)		"$glroot/bin/showlog"

# important settings #
######################
set bindnopre			"YES"
set disable(TRIGINALLCHAN)	0
set bnc(ENABLED)		"NO"
set bnc(LIST)			"uk:msn.test1.com:123 de:msn.test2.com:345 se:msn.test3.com:567"
set bnc(USER)			"bncuser"
set bnc(PASS)			"bncpass"
set speed(INCOMING)		"12500"
set speed(OUTGOING)		"12500"
set device(0)			"/dev/da0s1f MP3"
set device(1)			"/dev/ad0s1e ISO"

# Your sections #
#################
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
set chanlist(TEST)		"$mainchan"
set chanlist(GAMES)		"$mainchan"
set chanlist(APPS)		"$mainchan"
set chanlist(MP3)		"$mainchan"
set chanlist(0DAY)		"$mainchan"
set chanlist(MV)		"$mainchan"
set chanlist(DEFAULT)		"$mainchan"
set chanlist(WELCOME)		"$mainchan"
set statsection(0)		"MP3"
set statsection(1)		"0DAY"
set statsection(2)		"ISO"

# Stuff you may need to change #
################################
set splitter(CHAR)		"|"
set enable_irc_invite		"NO"
set chanlist(INVITE)		"$mainchan"
set invite_channels		$chanlist(INVITE)
set disable(BADBITRATE)		0
set disable(BADGENRE)		0
set disable(BADMSGINVITE)	0
set disable(BADYEAR)		0
set disable(BAD_FILE_CRC)	0
set disable(BAD_FILE_0SIZE)	0
set disable(BAD_FILE_ZIP)	0
set disable(BAD_FILE_WRONGDIR)	0
set disable(BAD_FILE_SFV)	0
set disable(BAD_FILE_NOSFV)	0
set disable(BAD_FILE_GENRE)	1
set disable(BAD_FILE_YEAR)	1
set disable(BAD_FILE_BITRATE)	1
set disable(BAD_FILE_DISALLOWED)	0
set disable(DEBUG)		1
set disable(DEFAULT)		1
set disable(DELDIR)		0
set disable(LOGIN)		1
set disable(LOGOUT)		1
set disable(TIMEOUT)		1
set disable(TAGLINE)		1
set disable(WIPE)		0
set disable(WIPE-r)		0
set disable(FAILLOGIN)		0
set disable(IPNOTADDED)		1
set disable(BADUSERHOST)	1
set disable(BADPASSWD)		1

