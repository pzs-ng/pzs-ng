#!/bin/bash

# config #
##########

# Where do I find your logs?
logdir=/glftpd/ftp-data/logs

# Enter here the names of the logs you wish trimmed.
# The format is: <logname>:<# of lines to keep>
logfiles="error.log:500 glftpd.log:1000 login.log:100 request.log:100 xferlog:1000"
today="`date +"%a %b %d %H:%M:%S %Y"`"

#################
# end of config #

for logline in $logfiles; do
  logfile=`echo $logline | cut -d ':' -f 1`
  loglines=`echo $logline | cut -d ':' -f 2`
  tail -n $loglines ${logdir}/${logfile} > ${logdir}/${logfile}.temp
  echo "$today -- Logfile turned over --" >> ${logdir}/${logfile}.temp
  cat ${logdir}/${logfile}.temp > ${logdir}/${logfile}
  rm -f ${logdir}/${logfile}.temp
done

