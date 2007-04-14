@echo off

rem --- Clean up ---
del ZenLib_GCC_MinGW32.7z
rmdir ZenLib_GCC_MinGW32 /S /Q
mkdir ZenLib_GCC_MinGW32


rem --- Copying : Include ---
xcopy ..\Source\ZenLib\*.h ZenLib_GCC_MinGW32\Include\ZenLib\

rem --- Copying : Documentation ---
mkdir Doc
cd ..\Source\Doc
..\..\..\Shared\Binary\Doxygen Doxygen
cd ..\..\Release
mkdir ZenLib_GCC_MinGW32\Doc\
xcopy Doc\*.*  ZenLib_GCC_MinGW32\Doc\
rmdir Doc /S /Q
xcopy ..\Source\Doc\*.html ZenLib_GCC_MinGW32\ /S

rem --- Copying : Library ---
copy GCC_MinGW32\Library\ZenLib.a ZenLib_GCC_MinGW32\

rem --- Copying : Information files ---
copy ..\*.txt ZenLib_GCC_MinGW32\


rem --- Compressing Archive ---
cd ZenLib_GCC_MinGW32
..\..\..\Shared\Binary\Win32\7-Zip\7z a -r -t7z -mx9 ..\ZenLib_GCC_MinGW32.7z *
cd ..

rem --- Clean up ---
rmdir ZenLib_GCC_MinGW32 /S /Q