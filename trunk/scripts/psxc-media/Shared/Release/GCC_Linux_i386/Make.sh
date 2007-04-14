#! /bin/sh

#############################################################################
# Go on the top level
cd ../..

#############################################################################
# Preparing Directories
test -d Include || mkdir -p Include
test -d Library/GCC_Linux_i386 || mkdir -p Library/GCC_Linux_i386

#############################################################################
# ZLib
if test -e Library/GCC_Linux_i386/libZLib.a; then
 echo ZLib is already compiled
else
 cd Source/System/ZLib
 ./configure
 $make
 cp libz.a ../../../Library/GCC_Linux_i386/libZLib.a
 test -d ../../../Include/zlib || mkdir -p ../../../Include/zlib
 cp zconf.h ../../../Include/zlib/
 cp zlib.h ../../../Include/zlib/
 cd ../../..
fi

#############################################################################
# WxWidgets
if test -e Library/GCC_Linux_i386/libWxWidgets.a; then
 echo WxWidgets is already compiled
else
 cd Source/System/WxWidgets
 ./configure --disable-shared --disable-gui --enable-unicode --enable-monolithic
 $make
 cp lib/libwx_baseu-2.6.a ../../../Library/GCC_Linux_i386/libWxWidgets.a
 cp -r include/wx ../../../Include/
 cp lib/wx/include/base-unicode-release-static-2.6/wx/setup.h ../../../Include/wx
 cd ../../..
fi

#############################################################################
# Ebml
if test -e Library/GCC_Linux_i386/libEbml.a; then
 echo Ebml is already compiled
else
 cd Source/MediaInfo/Ebml/make/linux/
 $make staticlib
 cd ../..
 cp -r ebml ../../../Include/
 cp make/linux/libebml.a ../../../Library/GCC_Linux_i386/libEbml.a
 cd ../../..
fi

#############################################################################
# Matroska
if test -e Library/GCC_Linux_i386/libMatroska.a; then
 echo Matroska is already compiled
else
 cd Source/MediaInfo/Matroska/make/linux/
 cp -r ../../../Ebml/ebml ../../
 $make staticlib
 cd ../..
 cp -r matroska ../../../Include/
 cp make/linux/libmatroska.a ../../../Library/GCC_Linux_i386/libMatroska.a
 cd ../../..
fi

#############################################################################
# Flac
if test -e Library/GCC_Linux_i386/libFLAC.a && test -e Library/GCC_Linux_i386/libFLAC++.a; then
 echo Flac is already compiled
else
 cd Source/MediaInfo/Flac
 #./configure --disable-shared --disable-xmms-plugin --disable-oggtest --disable-rpath --without-ogg --without-ogg-libraries --without-ogg-includes 
 ./configure --disable-shared --without-ogg --disable-asm-optimizations
 ln -s ../../Ogg/include/ogg include/ogg
 cd src/libFLAC
 $make
 cd ../..
 cd src/libFLAC++
 $make
 cd ../..
 cp -r include/FLAC ../../../Include/
 cp -r include/FLAC++ ../../../Include/
 cp src/libFLAC/.libs/libFLAC.a ../../../Library/GCC_Linux_i386/libFLAC.a
 cp src/libFLAC++/.libs/libFLAC++.a ../../../Library/GCC_Linux_i386/libFLAC++.a
 cd ../../..
fi

#############################################################################
# Musepack
if test -e Library/GCC_Linux_i386/libMpcDec.a; then
 echo MpcDec is already compiled
else
 cd Source/MediaInfo/Musepack
 ./configure --disable-shared
 $make
 cp -r include/mpcdec ../../../Include/
 cp src/.libs/libmpcdec.a ../../../Library/GCC_Linux_i386/libMpcDec.a
 cd ../../..
fi

#############################################################################
# SndFile
if test -e Library/GCC_Linux_i386/libSndFile.a; then
 echo ZSndFile is already compiled
else
 cd Source/MediaInfo/SndFile
 ./configure --disable-shared --disable-sqlite --disable-flac --disable-alsa
 $make
 test -d ../../../Include/SndFile || mkdir -p ../../../Include/SndFile
 cp -r src/sndfile.h ../../../Include/SndFile/
 cp src/.libs/libsndfile.a ../../../Library/GCC_Linux_i386/libSndFile.a
 cd ../../..
fi

#############################################################################
# ZenLib
#cd ZenLib/Project/GCC/Library
#mkdir -p .deps/Source/ZenLib
#mkdir -p .objs/Source/ZenLib
#make
#cd ../../..
#cp -r Include/ZenLib ../Shared/Include/
#cp Release/GCC_Linux_i386/Library/libZenLib.a ../Shared/Library/GCC_Linux_i386/
#cd ..

#############################################################################
# Go back to home
cd Release/GCC_Linux_i386
