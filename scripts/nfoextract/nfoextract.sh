#!/bin/bash
#####################################################
# PSXC's GetNFO v0.5a (get_nfo.sh)                   #
#                                                    #
# History:                                           #
# v0.5a - added support for a different file than    #
#         the default .message                       #
# v0.5 - more bugs fixed                             #
# v0.4 - small bug fixed (hopefully)                 #
#        added debug support for sfv releases        #
# v0.3 - removed support for chown'ing of file.      #
#        added support for extraction of file_id.diz #
#        added chmod'ing of file_id.diz              #
#        fixed a bug re: subdir in unzip list        #
# v0.2 - added support for a better nfo extraction.  #
#        added comments and how-to-use.              #
#        added some config.                          #
# v0.1 - First release.                              #
####################################################################################################
#
# How to use:
#
# This script should be run after the release is complete. Meant for d1's
# Project-ZS v0.6.x, edit the following in zsconfig.h:
#
# #define enable_complete_script  TRUE
# #define complete_script         "/bin/get_nfo.sh"
#
# Then deny *.nfo uploads in your 0day dirs. Here is an example with the
# default precheck.sh (replace 0DAY with the name of your 0day dir):
#
#  *.[nN][fF][oO])
#   if [ ! "`echo $2 | grep 0DAY`" = "" ]; then
#    echo -e ".---=== GetNFO v0.5a =====--- psxc (C)2003 --====--.\n| NFO file uploads denied.                         |\n| The .nfo will instead be extracted from the zip. |\n\`--------=======-----------------=====-------------'\n"
#    exit 2
#   fi
#  ;;
#
# copy get_nfo.sh to glftpd's bin directory (/glftpd/bin) and make sure you have the following
# in glftpd's bin directory: bash chmod chown unzip ls echo cat
#
####################################################################################################

##########
# config #
##########
# do you want the .nfo extracted to .message?
copy_to_message=1

# name of .message
name_message=.message

# add debug info?
# 0 = disable debug
# 1 = use file
# 2 = use .message
debug=0

#################
# end of config #
#################

found=0
nfocount=`ls -1 2>/dev/null | grep "\.[Nn][Ff][Oo]$"`
if [ -z "$nfocount" ] ; then
 files=`ls -1 2> /dev/null | grep "\.[Zz][Ii][Pp]$"`
 for file in $files; do
  nfocount=`ls -1 2> /dev/null | grep "\.[Nn][Ff][Oo]$"`
  if [ -z "$nfocount" ] ; then 
   /bin/unzip -l $file | {
    while read Length Date Time Name; do
     if [ ! -z "`echo $Name | grep "\.[Nn][Ff][Oo]$"`" ]; then
      if [ ! $found = 1 ]; then
       found=1
       plain=`echo $Name | /bin/sed "s|/| |g"`
       for plain_name in $plain; do
        New_Name=`echo $plain_name | tr '[:upper:]' '[:lower:]'`
       done
       /bin/unzip -Cqqp $file $Name > $New_Name
       if [ ! "$copy_to_message" = "0" ]; then
        echo "" >> $name_message
        echo "content of $New_Name:" >> $name_message
        echo "" >> $name_message
        cat $New_Name >> $name_message
       fi
       /bin/chmod 644 $New_Name
       if [ -z `ls -1 2>/dev/null | grep "[Ff][Ii][Ll][Ee]_[Ii][Dd]\.[Dd][Ii][Zz]"` ]; then
        /bin/unzip -Cqqp $file file_id.diz > file_id.diz
       fi
       if [ -e file_id.diz ]; then
        /bin/chmod 644 file_id.diz
       fi
      fi
     fi
    done
   }
  fi
 done
fi
if [ ! $debug -eq 0 ]; then
 if [ ! -z "`ls -1 2>/dev/null | grep "\.[Ss][Ff][Vv]$"`" ]; then
  a=0
  sfv_file=`ls -1 2>/dev/null | grep "\.[Ss][Ff][Vv]$" | head -1`
  for b in `cat $sfv_file | cut -d " " -f1`; do
   if [ -e $b ]; then
    let "a = a + 1"
   else
    noop=1
   fi
  done
  if [ $debug -eq 1 ]; then
   if [ ! "`ls -1 | grep "^\.files_uploaded"`" = "" ]; then
    rm -f .files_uploaded*
   fi
   touch .files_uploaded_$a
  else
   if [ ! "`cat $name_message | grep "^files_uploaded"`" = "" ]; then
    cat $name_message | awk '{ if ($1 == "files_uploaded") print "debug: num of files has changed"; else print }' > $name_message.tmp
    rm -f $name_message
    mv $name_message.tmp $name_message
    echo "files_uploaded : $a" >> $name_message
   else
    echo " " >> $name_message
    echo "files_uploaded : $a" >> $name_message
   fi
  fi
#  echo -e "Files uploaded so far: $a"
 fi
fi
exit 0

