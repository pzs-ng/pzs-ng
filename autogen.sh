#!/bin/sh

# Add valid autoconf versions here
autoconf_versions="2.59 2.57 2.5"

for p in `echo $PATH | tr ':' ' '`
do
	a=`ls $p/autoconf* 2> /dev/null`
	autoconf="$autoconf $a"
done

if [ -n "$autoconf" ]
then
	
	for ac in $autoconf
	do
		acv=`$ac --version | awk '/^[Aa]utoconf/{print $NF}'`
		for ver in $autoconf_versions
		do
			if [ "$acv" = "$ver" ]
			then
				use_autoconf=$ac
				break 2
			fi
		done
	done

	# run valid autoconf version
	if [ -n "$use_autoconf" ]
	then
		echo -n "Running $use_autoconf "
		$use_autoconf
		
		if [ $? -eq 0 ]; then
			echo "[OK]"
		else
			echo "[FAIL]"
		fi

		exit 0
	fi
	
fi

echo "No usable autoconf version found"

