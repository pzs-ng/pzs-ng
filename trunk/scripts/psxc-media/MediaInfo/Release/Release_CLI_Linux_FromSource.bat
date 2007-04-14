@echo off

rem --- Clean up ---
del MediaInfo_CLI_Linux_FromSource.tar
del MediaInfo_CLI_Linux_FromSource.tar.bz2
rmdir MediaInfo_CLI_Linux_FromSource /S /Q
mkdir MediaInfo_CLI_Linux_FromSource

rem --- Shared ---
mkdir MediaInfo_CLI_Linux_FromSource\Shared
mkdir MediaInfo_CLI_Linux_FromSource\Shared\Release
copy ..\..\Shared\Release\Build_GCC_Linux_i386.sh MediaInfo_CLI_Linux_FromSource\Shared\Release\
copy ..\..\Shared\Release\Source_Get.sh MediaInfo_CLI_Linux_FromSource\Shared\Release\
mkdir MediaInfo_CLI_Linux_FromSource\Shared\Release\GCC_Linux_i386
copy ..\..\Shared\Release\GCC_Linux_i386\Make.sh MediaInfo_CLI_Linux_FromSource\Shared\Release\GCC_Linux_i386\
copy ..\Source\Install\Linux_Compile_All.sh MediaInfo_CLI_Linux_FromSource\compile.sh

rem --- ZenLib ---
cd ..\..\ZenLib\Release\
call Release_Source.bat SkipCleanUp SkipCompression
cd ..\..\MediaInfo\Release\
move ..\..\ZenLib\Release\ZenLib_Source MediaInfo_CLI_Linux_FromSource\ZenLib

rem --- MediaInfoLib ---
cd ..\..\MediaInfoLib\Release\
call Release_Source.bat SkipCleanUp SkipCompression
cd ..\..\MediaInfo\Release\
move ..\..\MediaInfoLib\Release\MediaInfo_Lib_Source MediaInfo_CLI_Linux_FromSource\MediaInfoLib

rem --- MediaInfo ---
call Release_Source.bat SkipCleanUp SkipCompression
move MediaInfo_Source MediaInfo_CLI_Linux_FromSource\MediaInfo

