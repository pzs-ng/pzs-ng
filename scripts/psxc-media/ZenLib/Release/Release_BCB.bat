@echo off

rem --- Clean up ---
del ZenLib_BCB.7z
rmdir ZenLib_BCB /S /Q
mkdir ZenLib_BCB


rem --- Copying : Include ---
xcopy ..\Source\ZenLib\*.h ZenLib_BCB\Include\ZenLib\

rem --- Copying : Documentation ---
mkdir Doc
cd ..\Source\Doc
..\..\..\Shared\Binary\Doxygen Doxygen
cd ..\..\Release
mkdir ZenLib_BCB\Doc\
xcopy Doc\*.*  ZenLib_BCB\Doc\
rmdir Doc /S /Q
xcopy ..\Source\Doc\*.html ZenLib_BCB\ /S

rem --- Copying : Library ---
copy BCB\Library\ZenLib.lib ZenLib_BCB\

rem --- Copying : Information files ---
copy ..\*.txt ZenLib_BCB\


rem --- Compressing Archive ---
cd ZenLib_BCB
..\..\..\Shared\Binary\Win32\7-Zip\7z a -r -t7z -mx9 ..\ZenLib_BCB.7z *
cd ..

rem --- Clean up ---
rmdir ZenLib_BCB /S /Q