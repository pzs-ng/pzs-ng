#!/bin/bash

# Set system type
case $(uname -s) in
    Linux) os=linux ;;
    *[oO][pP][eE][nN][bB][sS][dD]*) os=openbsd ;;
    *[Dd][Aa][Rr][Ww][Ii][Nn]*) os=darwin ;;
    *[Nn][Ee][Tt][Bb][Ss][Dd]*) os=netbsd ;;
    *[fF][rR][eE][eE][bB][sS][dD]*)
        bsdversion=`uname -r | tr -cd '0-9' | cut -b 1-2`
        if [ $bsdversion -ge 52 ]; then
           os=freebsd5
        else
           os=freebsd4
        fi
        ;;
    *)
        echo "Sorry, but this util does not support the $(uname -s) platform."
        exit 1
        ;;
esac

# Ensure we have all useful paths in our $PATH
PATH="$PATH:/bin:/usr/bin:/usr/local/bin:/sbin:/usr/sbin:/usr/local/sbin:\
/usr/libexec:/usr/compat/linux/bin:/usr/compat/linux/sbin:/usr/compat/linux/usr/bin:\
/usr/compat/linux/usr/sbin"

if [ $# -eq 0 ]; then
	echo ""
	echo "This util will try to copy the necessary libs into your"
	echo "glftpd environment."
	echo ""
	echo "Usage: $0 /path/to/glftpd-root-dir"
	echo ""
	exit 0
else
	glroot=$1
	if [ ! -e /$glroot/bin/glftpd ]; then
		echo ""
		echo "Oops! I think you wrote wrong path. I can't find"
		echo "$glroot/bin/glftpd - Exiting."
		exit 0
	fi
fi

lddsequence() {
    echo -n "   $(basename $lib): "
    libdir="$(dirname $lib)"
    if [ -f "$lib" ]; then
       mkdir -p "$glroot$libdir"
       cp -f "$lib" "$glroot$libdir"
       echo "$libdir" >> "$glroot/etc/ld.so.conf"
       echo "OK"
    elif [ -f "/usr/compat/linux/$lib" ]; then
       mkdir -p "$glroot/usr/compat/linux"
       cp -f "/usr/compat/linux/$lib" "$glroot/usr/compat/linux/"
       echo "$libdir" >> "$glroot/etc/ld.so.conf"
       echo "OK"
    else
       echo -e '\033[1;31mFailed!\033[0m'" You must find & copy $(basename $lib) to $glroot$libdir manually."
    fi
}

echo -e "\nCopying required shared library files:"
echo -n "" > "$glroot/etc/ld.so.conf"
case $os in
    openbsd)
      openrel=`uname -r | tr -cd '0-9' | cut -b 1-2`
      if [ $openrel -ge 34 ]; then
        ldd $glroot/bin/* 2>/dev/null | awk '{print $5, $1}' | grep -e "^/" | grep -v "00000000$" | awk '{print $1}' |
        sort | uniq | while read lib; do
            lddsequence
        done
      else
        ldd $glroot/bin/* 2>/dev/null | awk '{print $3, $4}' | grep -e "^/" | grep -v "00000000)$" | awk '{print $1}' |
        sort | uniq | while read lib; do
            lddsequence
        done
      fi
    ;;
    darwin)
        otool -L $glroot/bin/* 2>/dev/null | awk '{print $1}' | grep -e "^/" | grep -v "$glroot/bin/" |
        sort | uniq | while read lib; do
            lddsequence
        done
    ;;
    *)
        ldd $glroot/bin/* 2>/dev/null | awk '{print $3}' | grep -e "^/" | 
        sort | uniq | while read lib; do
            lddsequence
        done
esac

case $os in *bsd*)
    echo -e "\nCopying your system's run-time library linker(s):"
    echo "(NOTE: Searches can take a couple of minutes, please be patient.)"
    case $os in
        freebsd4)
            bsdlibs="/usr/libexec/ld-elf.so.1"
            ;;
        freebsd5)
            bsdlibs="/libexec/ld-elf.so.1"
            ;;
        openbsd)
            bsdlibs="/usr/libexec/ld.so"
            ;;
        netbsd)
            bsdlibs="/usr/libexec/ld.so /usr/libexec/ld.elf_so"
            ;;
        esac
        libfailed=0
        for bsdlib in $bsdlibs; do
            bsdlibdir=${bsdlib%/*}
            mkdir -p "$glroot$bsdlibdir"
            echo -n "   $(basename $bsdlib): "
            if [ -e "$bsdlib" ]; then
                cp -f "$bsdlib" "$glroot$bsdlibdir"
                echo "OK"
            else
                echo -n "Searching . . . " 
                file=$(find / -name $(basename $bsdlib) | head -1)
                if [ -n "$file" ]; then
                    cp -f "$file" "$glroot$bsdlibdir"
                    echo "OK"
                else
                    echo -e '\033[1;31mFailed!\033[0m'
                    libfailed="1"
                fi
            fi
        done
        [ $libfailed -eq 1 ] && echo "You must install and copy the missing libraries to $glroot$bsdlibdir manually." 
    ;;
esac   

echo -ne "\nConfiguring the shared library cache . . . "
sort "$glroot/etc/ld.so.conf" | uniq >"$glroot/etc/ld.so.temp" && mv "$glroot/etc/ld.so.temp" "$glroot/etc/ld.so.conf"
lddlist="`cat $glroot/etc/ld.so.conf | tr '\n' ' '`"
case $os in
    linux)
        chroot "$glroot" /bin/ldconfig
        ;;
    *bsd*)
        mkdir -p "$glroot/usr/lib"
        mkdir -p "$glroot/var/run"
        chroot "$glroot" /bin/ldconfig $lddlist
        ;;
    esac
echo "Done."

exit 0

