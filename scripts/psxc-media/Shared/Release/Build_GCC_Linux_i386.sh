#! /bin/sh

#############################################################################
# Download sources if needed
./Source_Get.sh
if test -d ../Source/System/ZLib \
&& test -d ../Source/System/WxWidgets \
&& test -d ../Source/MediaInfo/Ebml \
&& test -d ../Source/MediaInfo/Matroska \
&& test -d ../Source/MediaInfo/Flac \
&& test -d ../Source/MediaInfo/Musepack \
&& test -d ../Source/MediaInfo/SndFile \
; then
 echo All is downloaded
else
 echo Problem while downloading files, stop.
 exit
fi

#############################################################################
# Make
cd GCC_Linux_i386
./Make.sh
cd ..