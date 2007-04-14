#! /bin/sh

#-----------------------------------------------------------------------
# Clean up
rm    MediaInfo_CLI_Linux_i386.tar
rm    MediaInfo_CLI_Linux_i386.tar.bz2
rm -r MediaInfo_CLI_Linux_i386
mkdir MediaInfo_CLI_Linux_i386

#-----------------------------------------------------------------------
# Copying : Exe
cp GCC_Linux_i386/CLI/MediaInfo MediaInfo_CLI_Linux_i386/MediaInfo

#-----------------------------------------------------------------------
# Copying : Licence files
cp ../Licence.txt MediaInfo_CLI_Linux_i386/

#-----------------------------------------------------------------------
# Copying : History
cp ../History.CLI.txt MediaInfo_CLI_Linux_i386/

#-----------------------------------------------------------------------
# Copying : Information file
cp Readme.Linux.txt MediaInfo_CLI_Linux_i386/

#-----------------------------------------------------------------------
# Compressing Archive
tar jcf MediaInfo_CLI_Linux_i386.tar.bz2 MediaInfo_CLI_Linux_i386
rm MediaInfo_CLI_Linux_i386.tar

#-----------------------------------------------------------------------
# Clean up
#if "%1"=="SkipCleanUp" goto SkipCleanUp
rm -r MediaInfo_CLI_Linux_i386
#:SkipCleanUp
