#!/bin/sh
file="sitewho.c"

echo -n "Compiling $file : "
gcc -O2 -static -Wall -o ${file%.c} $file 2> /dev/null
if [ $? -gt 0 ] ; then
 echo "[FAIL]"
 exit 2
fi
echo "[OK]"

echo ""
echo "Installing...";
conf="${file%.c}.conf"
for v in rootpath headerfile footerfile; do
 eval $v=$(grep "^$v" $conf | cut -d'=' -f2 | sed 's/^ \+//g')
done

cp -f ${headerfile##*/} $rootpath$headerfile
cp -f ${footerfile##*/} $rootpath$footerfile
for f in ${file%.c} $conf bandwidth.sh; do
 cp $f $rootpath/bin/
done

echo ""
echo "Check README, if you want to learn how this works!"
echo ""
