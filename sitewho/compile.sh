echo -n "Compiling sitewho.c :"
gcc -O2 sitewho.c -O2 -static -o sitewho 2> /dev/null
if [ $? -gt 0 ] ; then
 echo " [FAIL]"
 exit 2
fi
echo " [OK]"

echo ""
echo "Check README, if you want to learn how this works!"
echo ""
