#!/bin/sh
file="sitewho.c"

echo -n "Compiling $file : "
case `uname -s` in
 *[Bb][Ss][Dd]*)
  STATIC="-static"
  ;;
esac
gcc -O2 $STATIC -Wall -o ${file%.c} $file 2> /dev/null
if [ $? -gt 0 ]; then
 echo "[FAIL]"
 exit 2
fi
echo "[OK]"

echo ""
echo "Installing...";
conf="${file%.c}.conf"
for v in glrootpath headerfile footerfile; do
 eval $v=$(grep "^$v" $conf | awk {'print $3'})
done

cp -f ${headerfile##*/} $glrootpath$headerfile
cp -f ${footerfile##*/} $glrootpath$footerfile
for f in ${file%.c} $conf bandwidth.sh; do
 cp $f $glrootpath/bin/
done

echo ""
echo "Check README, if you want to learn how this works!"
echo ""
