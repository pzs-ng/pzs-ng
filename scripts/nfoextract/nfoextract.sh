#!/bin/bash

nfocount=`ls -1 2> /dev/null | grep -xE *.[nN][fF][oO]`
if [ -z "$nfocount" ] ; then
	nfolist=`/bin/unzip -Clqq $1 *.nfo | head -n 1 | cut -c29-`         
	if [ "$nfolist" != "" ] ; then
		/bin/unzip -Cqq $1 $nfolist
		fi
	fi
