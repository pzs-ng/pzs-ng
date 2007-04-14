#! /bin/sh

#############################################################################
# How to get http
if test -x /usr/bin/wget -o -x /usr/local/bin/wget; then
 echo wget is found
 wgetBin='wget'
else
 echo wget is NOT found
 if test -x /usr/bin/curl -o -x /usr/local/bin/curl; then
  echo Curl is found
  wgetBin='curl -O'
 else
  echo Curl is NOT found, assuming default : wget
  wgetBin='wget'
 fi
fi

#############################################################################
# Versions
ZLib_Version=1.2.3
WxWidgets_Version=2.6.3
Ebml_Version=0.7.7
Matroska_Version=0.8.0
Ogg_Version=1.1.3
Flac_Version=1.1.3
Musepack_Version=1.2.2

#############################################################################
# Go on the top level
cd ..
rm *.gz
rm *.bz2

#############################################################################
# Preparing Directories
test -d Source/System || mkdir -p Source/System
test -d Source/MediaInfo || mkdir -p Source/MediaInfo

#############################################################################
# ZLib
if test -d Source/System/ZLib; then
 echo ZLib is present
else
 $wgetBin http://www.zlib.net/zlib-$ZLib_Version.tar.gz
 gunzip zlib-$ZLib_Version.tar.gz
 tar xf zlib-$ZLib_Version.tar
 test -d Source/System/ZLib && rm -r Source/System/ZLib
 mv zlib-$ZLib_Version Source/System/ZLib
 rm zlib-$ZLib_Version.tar
fi

#############################################################################
# WxWidgets
if test -d Source/System/WxWidgets; then
 echo WxWidgets is present
else
 test -f wxWidgets-$WxWidgets_Version.tar.gz || $wgetBin http://ovh.dl.sourceforge.net/wxwindows/wxWidgets-$WxWidgets_Version.tar.bz2
 bunzip2 wxWidgets-$WxWidgets_Version.tar.bz2
 tar xf wxWidgets-$WxWidgets_Version.tar
 test -d Source/System/WxWidgets && rm -r Source/System/WxWidgets
 mv wxWidgets-$WxWidgets_Version Source/System/WxWidgets
 rm wxWidgets-$WxWidgets_Version.tar
fi

#############################################################################
# Ebml
if test -d Source/MediaInfo/Ebml; then
 echo Ebml is present
else
 $wgetBin http://dl.matroska.org/downloads/libebml/libebml-$Ebml_Version.tar.bz2
 bunzip2 libebml-$Ebml_Version.tar.bz2
 tar xf libebml-$Ebml_Version.tar
 test -d Source/MediaInfo/Ebml && rm -r Source/MediaInfo/Ebml
 mv libebml-$Ebml_Version Source/MediaInfo/Ebml
 rm libebml-$Ebml_Version.tar
fi

#############################################################################
# Matroska
if test -d Source/MediaInfo/Matroska; then
 echo Matroska is present
else
 $wgetBin http://dl.matroska.org/downloads/libmatroska/libmatroska-$Matroska_Version.tar.bz2
 bunzip2 libmatroska-$Matroska_Version.tar.bz2
 tar xf libmatroska-$Matroska_Version.tar
 test -d Source/MediaInfo/Matroska && rm -r Source/MediaInfo/Matroska
 mv libmatroska-$Matroska_Version Source/MediaInfo/Matroska
 rm libmatroska-$Matroska_Version.tar
fi

#############################################################################
# Flac
if test -d Source/MediaInfo/Flac; then
 echo Flac is present
else
 $wgetBin http://ovh.dl.sourceforge.net/sourceforge/flac/flac-$Flac_Version.tar.gz
 gunzip flac-$Flac_Version.tar.gz
 tar xf flac-$Flac_Version.tar
 test -d Source/MediaInfo/Flac && rm -r Source/MediaInfo/Flac
 mv flac-$Flac_Version Source/MediaInfo/Flac
 rm flac-$Flac_Version.tar
 test -d Project/MediaInfo/Flac/prj && cp Project/MediaInfo/Flac/prj/* Source/MediaInfo/Flac/prj/
fi

#############################################################################
# Musepack
if test -d Source/MediaInfo/Musepack; then
 echo Musepack is present
else
 $wgetBin http://files.musepack.net/source/libmpcdec-$Musepack_Version.tar.bz2
 bunzip2 libmpcdec-$Musepack_Version.tar.bz2
 tar xf libmpcdec-$Musepack_Version.tar
 test -d Source/MediaInfo/Musepack && rm -r Source/MediaInfo/Musepack
 mv libmpcdec-$Musepack_Version Source/MediaInfo/Musepack
 rm libmpcdec-$Musepack_Version.tar
fi

#############################################################################
# SndFile
if test -d Source/MediaInfo/SndFile; then
 echo SndFile is present
else
 $wgetBin http://www.mega-nerd.com/libsndfile/libsndfile-1.0.17.tar.gz
 gunzip libsndfile-1.0.17.tar.gz
 tar xf libsndfile-1.0.17.tar
 test -d Source/MediaInfo/SndFile && rm -r Source/MediaInfo/SndFile
 mv libsndfile-1.0.17 Source/MediaInfo/SndFile
 rm libsndfile-1.0.17.tar
fi

#############################################################################
# Go back to home
cd Release

