syst=`uname -s`
glver="`grep -r "^#define for_glftpd2" sitewho.c | awk '{print $3}'`"
if [ $glver -eq 1 ]; then
 glver="Glftpd v2.xx"
else
 glver="Glftpd v1.xx"
fi
echo -n "Compiling sitewho.c for $glver on $syst:"
case $syst in
 [Ll][Ii][Nn][Uu][Xx])
  gcc -O2 sitewho.c -O2 -o sitewho 2> /dev/null
  ;;
 *[Bb][Ss][Dd]*)
  gcc -O2 sitewho.c -O2 -static -o sitewho 2> /dev/null
  ;;
esac

if [ $? -gt 0 ] ; then
 echo " [FAIL]"
 exit 2
fi
echo " [OK]"
echo ""
echo "Check README, if you want to learn how this works!"
echo ""
