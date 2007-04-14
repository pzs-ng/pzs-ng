#! /bin/sh

#############################################################################
# THIS SCRIPT MUST BE IN THE ROOT DIRECTORY OF MEDIAINFO
#############################################################################


#############################################################################
# Info
echo MediaInfo compile from sources.
echo NOTE : This is a BETA software, mainly on Linux platform
echo This script will download source of third-party librairies,
echo and compile them before compile MediaInfo itself.
echo [later, it will use dependancies from your distrib]
sleep 10

#############################################################################
# chmod because compressing is sometimes done with Windows
chmod 755 Shared/Release/Source_Get.sh
chmod 755 Shared/Release/Build_GCC_Linux_i386.sh
chmod 755 Shared/Release/GCC_Linux_i386/Make.sh

#############################################################################
# Shared
if test -e Shared/Library/GCC_Linux_i386/libZLib.a \
&& test -e Shared/Library/GCC_Linux_i386/libWxWidgets.a \
&& test -e Shared/Library/GCC_Linux_i386/libEbml.a \
&& test -e Shared/Library/GCC_Linux_i386/libMatroska.a \
&& test -e Shared/Library/GCC_Linux_i386/libFLAC.a \
&& test -e Shared/Library/GCC_Linux_i386/libFLAC++.a \
&& test -e Shared/Library/GCC_Linux_i386/libSndFile.a \
; then
 #Already done
 echo Shared libraries are already compiled.
else
 #Build
 cd Shared/Release
 ./Build_GCC_Linux_i386.sh
 cd ../..
 if test -e Shared/Library/GCC_Linux_i386/libZLib.a \
 && test -e Shared/Library/GCC_Linux_i386/libWxWidgets.a \
 && test -e Shared/Library/GCC_Linux_i386/libEbml.a \
 && test -e Shared/Library/GCC_Linux_i386/libMatroska.a \
 && test -e Shared/Library/GCC_Linux_i386/libFLAC.a \
 && test -e Shared/Library/GCC_Linux_i386/libFLAC++.a \
 && test -e Shared/Library/GCC_Linux_i386/libMpcDec.a \
 && test -e Shared/Library/GCC_Linux_i386/libSndFile.a \
 ; then
  echo Shared libraries are compiled
 else
  echo Problem while compiling shared libraries, stop.
  exit
 fi
fi

#############################################################################
# ZenLib
if test -d Shared/Library/GCC_Linux_i386/libZenLib.a; then
 #Already done
 echo ZenLib library is already compiled
else
 #Build
 cd ZenLib
 cd Project/GCC/Library
 mkdir -p .deps/Source/ZenLib
 mkdir -p .objs/Source/ZenLib
 $make
 cd ../../..
 cp -r Include/ZenLib ../Shared/Include/
 cp Release/GCC_Linux_i386/Library/libZenLib.a ../Shared/Library/GCC_Linux_i386/
 cd ..
 if test -e Shared/Library/GCC_Linux_i386/libZenLib.a; then
  echo ZenLib library is compiled
 else
  echo Problem while compiling ZenLib, stop.
  exit
 fi
fi

#############################################################################
# MediaInfoLib
cd MediaInfoLib/Project/GCC/Library
$make
cd ../../../..
if test -e MediaInfoLib/Release/GCC_Linux_i386/Library/libMediaInfo.a; then
 echo MediaInfo library is compiled
else
 echo Problem while compiling MediaInfo Library, stop.
 exit
fi

#############################################################################
# MediaInfo
cd MediaInfo/Project/GCC/CLI
$make
cd ../../../..
cp MediaInfo/Release/GCC_Linux_i386/CLI/MediaInfo MediaInfo.exe
