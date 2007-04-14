MediaInfo.Dll - http://mediainfo.sourceforge.net
-------------
This Dll is freeware
You can distribute as you want (Internet, CD in books...)
MediaInfo.dll is the only one important file, which you must put in your software folder


WARNING FOR Windows95, Windows98 and Windows Milenium USERS:
------------------------------------------------------------
The following has to be installed, it enables the Unicode functionality for those OSes.
Microsoft Layer for Unicode on Windows 95/98/ME Systems (MSLU)
http://www.microsoft.com/downloads/details.aspx?FamilyId=73BA7BD7-ED06-4F0D-80A4-2A7EEAEE17E2&displaylang=en


For software developers
-----------------------
You can use it as you want (example: without this text file, without sources),
but a reference to "http://mediainfo.sourceforge.net" in your software would be appreciated.

There are examples for:
- BCB: Borland C++ Builder 6
- Delphi: Borland Delphi 10 (aka 2005)
- MSCS: Microsoft C# 8 (aka 2005)
- MSJS: Microsoft J# 8 (aka 2005)
- MSVB: Microsoft Visual Basic 8 (aka 2005)
- MSVC: Microsoft Visual C++ 8 (aka 2005)
Don't forget to put MediaInfo.Dll and Example.ogg in your executable folder.

Note: for C++ users, .lib files can be found in "Developpers/Release" Directory

Note: for Visual Studio 7 (aka 2002) or 7.1 (aka 2003), you can edit .sln and .xxproj to be compatible.
- .sln: "...Format Version 9.00" to "7.00"
- .xxproj: "Version="8.00" to "7.00"
This is not always tested, but examples files are only a template, you can easily adapt them to your compiler
Note: I can't handle all versions of all compilers, so I look for maintainers for examples.

Note: versioning method, for people who develop with LoadLibrary method
- if one of 2 first numbers change, there is no guaranties that the DLL is compatible with old one. You should verify with MediaInfo_Option("Version") if you are compatible
- if one of 2 last numbers change, there is a guaranty that the DLL is compatible with old one.
So you should test the version of the DLL, and if one of the 2 first numbers change, not load it.

Contributions (in the "Contrib" directory):
- MSVB5: Microsoft Visual Basic 5 and 6 example, from Ingo Brueckl
- ActiveX: ActiveX wrapper for MediaInfoDLL, with Internet Explorer and VB Script examples, from Ingo Brueckl