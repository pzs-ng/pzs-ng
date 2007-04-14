@echo off

rem --- Clean up ---
del ZenLib_MSVC.7z
rmdir ZenLib_MSVC /S /Q
mkdir ZenLib_MSVC


rem --- Copying : Include ---
xcopy ..\Source\ZenLib\*.h ZenLib_MSVC\Include\ZenLib\

rem --- Copying : Documentation ---
mkdir Doc
cd ..\Source\Doc
..\..\..\Shared\Binary\Doxygen Doxygen
cd ..\..\Release
mkdir ZenLib_MSVC\Doc\
xcopy Doc\*.*  ZenLib_MSVC\Doc\
rmdir Doc /S /Q
xcopy ..\Source\Doc\*.html ZenLib_MSVC\ /S

rem --- Copying : Library ---
copy MSVC\Library\ZenLib.lib ZenLib_MSVC\

rem --- Copying : Information files ---
copy ..\*.txt ZenLib_MSVC\


rem --- Compressing Archive ---
cd ZenLib_MSVC
..\..\..\Shared\Binary\Win32\7-Zip\7z a -r -t7z -mx9 ..\ZenLib_MSVC.7z *
cd ..

rem --- Clean up ---
rmdir ZenLib_MSVC /S /Q
