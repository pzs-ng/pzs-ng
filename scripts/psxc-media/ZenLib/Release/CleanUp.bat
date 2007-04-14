@rem echo off
@rem --- General ---
cd ..
del *.~* *.obj *.o *.tds *.dcu *.ddp *.opt *.ncb *.suo *.plg *.aps *.user *.win *.layout *.local *.depend *.identcache *.tgs *.tgw /Q /S
del *.~* *.obj *.o *.tds *.dcu *.ddp *.opt *.ncb *.suo *.plg *.aps *.user *.win *.layout *.local *.depend  *.identcache *.tgs *.tgw /AH /Q /S
cd Release

@rem --- In Project files ---
cd ..\Project
del *.exe *.dll *.a *.lib *.exp *.class *.zip *.7z /Q /S
del *.exe *.dll *.a *.lib *.exp *.class *.zip *.7z /AH /Q /S
cd ..\Release

@rem MS Visual Studio specific ---
cd ..\Project
rmdir MSVC\Test\Release /Q /S
rmdir MSVC\Test\Release_Ansi /Q /S
rmdir MSVC\Library\Release /Q /S
rmdir MSVC\Library\Release_Ansi /Q /S
rmdir MSVC\Example\Release /Q /S
rmdir MSVC\Example\Release_Ansi /Q /S
cd ..\Release

@rem Borland Developper Studio specific ---
cd ..\Source
rmdir ZenLib\__history /Q /S
rmdir Example\__history /Q /S
cd ..\Release
cd ..\Project
rmdir BCB\__history /Q /S
rmdir BCB\Example\__history /Q /S
rmdir BCB\Example\Debug_Build /Q /S
rmdir BCB\Example\Release_Build /Q /S
rmdir BCB\Library\__history /Q /S
rmdir BCB\Library\Debug_Build /Q /S
rmdir BCB\Library\Release_Build /Q /S
cd ..\Release

@rem Code::Blocks specific ---
cd ..\Project
rmdir CodeBlocks\Library\.objs /Q /S
cd ..\Release

@rem GCC specific ---
cd ..\Project
rmdir GCC\Library\.deps /Q /S
rmdir GCC\Library\.objs /Q /S
cd ..\Release

@rem Release specific ---
del *.zip *.gz *.bz2 *.lib *.dll *.exp *.a *.so *.7z /Q /S
