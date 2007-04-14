#! /bin/sh

#-----------------------------------------------------------------------
# Clean up
rm    MediaInfo_CLI_MacIntel.tar
rm    MediaInfo_CLI_MacIntel.tar.bz2
rm -r MediaInfo_CLI_MacIntel
mkdir MediaInfo_CLI_MacIntel

#-----------------------------------------------------------------------
# Copying : Exe
cp GCC_Linux_i386/CLI/MediaInfo MediaInfo_CLI_MacIntel/MediaInfo

#-----------------------------------------------------------------------
# Copying : Licence files
cp ../Licence.txt MediaInfo_CLI_MacIntel/

#-----------------------------------------------------------------------
# Copying : History
cp ../History.CLI.txt MediaInfo_CLI_MacIntel/

#-----------------------------------------------------------------------
# Copying : Information file
cp Readme.Linux.txt MediaInfo_CLI_MacIntel/

#-----------------------------------------------------------------------
# Compressing Archive
tar jcf MediaInfo_CLI_MacIntel.tar.bz2 MediaInfo_CLI_MacIntel
rm MediaInfo_CLI_MacIntel.tar

#-----------------------------------------------------------------------
# Clean up
#if "%1"=="SkipCleanUp" goto SkipCleanUp
rm -r MediaInfo_CLI_MacIntel
#:SkipCleanUp
