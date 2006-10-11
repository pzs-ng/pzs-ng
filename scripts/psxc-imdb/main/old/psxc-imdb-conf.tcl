#######################
# PSXC-IMDB bot-addon #
# -------------------
# Tcl is really not my strong suit, so do not be surprised if your computer
# blows up when you run this script.
#
# This little thingy can be used instead of adding stuff to crontab. It does
# basically the same thing, only this will react instantly on .nfo uploads.
# It may also be used instead of your normal bot config to output stuff in
# (one) channel.
#
# edit the (not so) advanced config below, then copy this script to your bot's
# script directory and add the following to your eggdrop.conf:
#
#   source scripts/psxc-imdb-bot.tcl
#

###################
##### config ######

# location of psxc-imdb.log file (aka IMDBLOG)
set psxc(IMDBLOG)     "/glftpd/ftp-data/logs/psxc-imdb.log"

# location of the imdb-script
set psxc(IMDBSCRIPT)  "/glftpd/bin/psxc-imdb.sh"

# location of psxc-moviedata.log file (aka GLLOG)
# used only in "full" mode, where this addon will do all 
# imdb-output to channel.
# !! DO NOT USE glftpd.log HERE !!
set psxc(IMDBMOVIE)   "/glftpd/ftp-data/logs/psxc-moviedata.log"

# announce-channel(s) - separate by space
# used only in "full" mode.
set psxc(IMDBCHANNEL) "#changethis"

# should the lines start with something?
# used only in "full" mode.
set psxc(IMDBTRIG) "\002IMDB:\002"

# location of the .log file used for pre'ing
set psxc(PRELOG)     "/glftpd/ftp-data/logs/glftpd.log"
#set psxc(PRELOG)     "/glftpd/ftp-data/logs/prelog"

# location of the imdb-pre-script. Normally, this is a symlink
# to psxc-imdb.sh.
set psxc(PRESCRIPT)  "/glftpd/bin/psxc-imdb-pre.sh"

# What do you wish to use as a channel trigger to react to
# questions about a movie?
set psxc(FINDTRIG) "!imdb"

# Where is the find script (full path)
set psxc(FINDSCRIPT) "/glftpd/bin/psxc-imdb-find.sh"

# This char is used to split lines on output. It should be the
# same variable you have in psxc-imdb-conf (NEWLINE).
set psxc(NEWLINE) "|"

#####
# this is where you enable/disable the parts of the script.

# Should the script be used to output imdb-info? Normally
# you set this to "YES"
set psxc(USEBOT)  "YES"

# Should the script handle imdb-output? Normally dZsbot.tcl
# handles output, but in some cases that doesn't work. This was
# before known as "full" mode.
# You also set this to "YES" if you wish this addon to handle
# all !imdb requests.
set psxc(USEFULL) "NO"

# Is pre-support wanted?
set psxc(USEPRE)  "YES"

# Do you wish to answer to !imdb requests? Please read
# psxc-imdb-find.README before setting this to YES.
# Also REMOVE psxc-imdb-find.tcl from your eggdrop.conf if
# you used it!
set psxc(USEFIND) "YES"
#

#####
# I know a lot of admins dislike the "<[psxc-imdb.sh] <defunct>"
# process - I strongly suggest you get used to it, but if you
# cannot, set this variable to YES - it will force the bot to wait
# for the script to finish. FYI - this can lead to a slow
# responding/freezing bot, it may die on occation, it may become
# very unstable in fact. But, you're free to test.
set psxc(NODEFUNCT) "NO"

##### config done #####
#######################

