// MediaInfo_CLI - A Command Line Interface for MediaInfoLib
// Copyright (C) 2002-2007 Jerome Martinez, Zen@MediaArea.net
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
//Args
#if !defined(_WIN32) && defined(UNICODE)
    #include <wx/strconv.h>
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//Unicode
#ifdef _WIN32
    #include <windows.h>
    #include <tchar.h>
    #define _TO(__x) _T(__x)
#else
    //Ubuntu does not support output in Unicode
    #undef  _tprintf
    #define _tprintf printf
    #define _TO(__x) __x
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#if !defined(_WIN32) && defined(UNICODE)
static char* Buffer=NULL;
static const char* cWC2MB(const wchar_t* Text)
{
    delete Buffer;
    size_t Buffer_Size=wxConvCurrent->WC2MB(NULL, Text, 0)+1;
    char* Buffer=new char[Buffer_Size];
    wxConvCurrent->WC2MB(Buffer, Text, Buffer_Size);
    return Buffer;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#else
static const MediaInfoLib::Char* cWC2MB(const MediaInfoLib::Char* Text)
{
    return Text;
}
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//Get command line args in main()
#if defined(_WIN32) && defined(UNICODE)
    #define GETCOMMANDLINE() \
        argv=CommandLineToArgvW(GetCommandLineW(), &argc); \

#elif !defined(_WIN32) && defined(UNICODE)
    #define GETCOMMANDLINE() \
        vector<String> argv; \
        for (int Pos=0; Pos<argc; Pos++) \
        { \
                size_t Buffer_Size=(wxConvCurrent->MB2WC(NULL, argv_ansi[Pos], 0)+1)*sizeof(MediaInfoLib::Char); \
                MediaInfoLib::Char* Buffer=new MediaInfoLib::Char[Buffer_Size]; \
                wxConvCurrent->MB2WC(Buffer, argv_ansi[Pos], Buffer_Size); \
                argv.push_back(Buffer); \
                delete Buffer; \
        } \

#else
    #define GETCOMMANDLINE() \

#endif

//---------------------------------------------------------------------------
//Write to terminal
#define TEXTOUT(_TEXT) \
{ \
    _tprintf(_TO(_TEXT)); \
} \

#define STRINGOUT(_TEXT) \
{ \
    _tprintf(cWC2MB(_TEXT.c_str())); \
} \


