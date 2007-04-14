@echo off

rem --- Clean up ---
del ZenLib_Source.7z
rmdir ZenLib_Source /S /Q
mkdir ZenLib_Source


rem --- Copying : Include ---
xcopy ..\Source\ZenLib\*.h ZenLib_Source\Include\ZenLib\

rem --- Copying : Documentation ---
mkdir Doc
cd ..\Source\Doc
..\..\..\Shared\Binary\Doxygen Doxygen
cd ..\..\Release
mkdir ZenLib_Source\Doc\
xcopy Doc\*.*  ZenLib_Source\Doc\
rmdir Doc /S /Q
xcopy ..\Source\Doc\*.html ZenLib_Source\ /S

@rem --- Copying : Sources ---
xcopy ..\Source\*.h ZenLib_Source\Source\ /S
xcopy ..\Source\*.cpp ZenLib_Source\Source\ /S
xcopy ..\Source\Doc\* ZenLib_Source\Source\Doc\ /S

@rem --- Copying : Projects ---
xcopy ..\Project\*.bpg ZenLib_Source\Project\ /S
xcopy ..\Project\*.bpf ZenLib_Source\Project\ /S
xcopy ..\Project\*.bpr ZenLib_Source\Project\ /S
xcopy ..\Project\*.res* ZenLib_Source\Project\ /S
xcopy ..\Project\*.dfm ZenLib_Source\Project\ /S
xcopy ..\Project\*.cpp ZenLib_Source\Project\ /S
xcopy ..\Project\*.bdsgroup ZenLib_Source\Project\ /S
xcopy ..\Project\*.bdsproj ZenLib_Source\Project\ /S
xcopy ..\Project\*.dpr ZenLib_Source\Project\ /S
xcopy ..\Project\*.pas ZenLib_Source\Project\ /S
xcopy ..\Project\*.dof ZenLib_Source\Project\ /S
xcopy ..\Project\*.dev ZenLib_Source\Project\ /S
xcopy ..\Project\*.sln ZenLib_Source\Project\ /S
xcopy ..\Project\*.cs ZenLib_Source\Project\ /S
xcopy ..\Project\*.csproj ZenLib_Source\Project\ /S
xcopy ..\Project\*.jsl ZenLib_Source\Project\ /S
xcopy ..\Project\*.vjsproj ZenLib_Source\Project\ /S
xcopy ..\Project\*.vb ZenLib_Source\Project\ /S
xcopy ..\Project\*.vbproj ZenLib_Source\Project\ /S
xcopy ..\Project\*.vc ZenLib_Source\Project\ /S
xcopy ..\Project\*.vcproj ZenLib_Source\Project\ /S
xcopy ..\Project\*.ico ZenLib_Source\Project\ /S
xcopy ..\Project\*.workspace ZenLib_Source\Project\ /S
xcopy ..\Project\*.cbp ZenLib_Source\Project\ /S
xcopy ..\Project\BCB\*.h ZenLib_Source\Project\BCB\ /S
xcopy ..\Project\MSVC\*.h ZenLib_Source\Project\MSVC\ /S
xcopy ..\Project\MSVC\*.rc ZenLib_Source\Project\MSVC\ /S
xcopy ..\Project\GCC\Make* ZenLib_Source\Project\GCC\ /S
xcopy ..\Project\GCC\*.Mak ZenLib_Source\Project\GCC\ /S

@rem --- Copying : Release ---
xcopy *.txt ZenLib_Source\Release\
xcopy *.bat ZenLib_Source\Release\
xcopy BCB\*.txt ZenLib_Source\Release\BCB\ /S
xcopy MSVC\*.txt ZenLib_Source\Release\MSVC\ /S
xcopy GCC_MinGW32\*.txt ZenLib_Source\Release\GCC_MinGW32\ /S
xcopy GCC_Linux_i386\*.txt ZenLib_Source\Release\GCC_Linux_i386\ /S

rem --- Copying : Information files ---
copy ..\*.txt ZenLib_Source\

rem --- Copying : CVS files ---
copy ..\*.cvsignore ZenLib_Source\


rem --- Compressing Archive ---
if "%2"=="SkipCompression" goto SkipCompression
move ZenLib_Source ZenLib
..\..\Shared\Binary\Win32\7-Zip\7z a -r -t7z -mx9 ZenLib_Source.7z ZenLib\*
move ZenLib ZenLib_Source
:SkipCompression

rem --- Clean up ---
if "%1"=="SkipCleanUp" goto SkipCleanUp
rmdir ZenLib_Source /S /Q
:SkipCleanUp
