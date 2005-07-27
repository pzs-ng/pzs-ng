#!/bin/sh

aclocal
autoheader
libtoolize --automake
automake --add-missing

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
		$use_autoconf > autogen.log 2>&1
		
		if [ $? -eq 0 ]; then
			echo "[OK]"
		else
			echo "[FAIL]"
		fi

		echo ""
		echo "NOTE:"
		echo "  This version is NOT considered usable. You will get NO HELP from the devs using this."
		echo "  Please consider downloading the v1.0 stable version from http://www.pzs-ng.com"
		echo ""
		exit 0
	fi
	
fi

echo "No usable autoconf version found"

