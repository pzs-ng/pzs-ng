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
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "CommandLine_Parser.h"
#include "Help.h"
using namespace std;
using namespace MediaInfoLib;
//---------------------------------------------------------------------------

//Parse Command Line
#define LAUNCH(_METHOD) \
    { \
        String Option, Value; \
        int Return=_METHOD(MI, Argument, Option, Value); \
        if (Return<0) \
            return Return; \
    } \

#define OPTION(_TEXT, _TOLAUNCH) \
    else if (Argument.find(_T(_TEXT))==0)        LAUNCH(_TOLAUNCH) \

#define OPTION2(_TEXT, _TOLAUNCH) \
    else if (Argument.find(_T(_TEXT))==0)        _TOLAUNCH(); \


//***************************************************************************
// Main
//***************************************************************************

int Parse(Core &MI, MediaInfoLib::String &Argument)
{
    if (0);
    OPTION("-f",                                            f)
    OPTION("--Help-",                                       Help_xxx)
    OPTION("--Help",                                        Help)
    OPTION("--Info-Parameters",                             Info_Parameters)
    OPTION("--Inform=",                                     Inform)
    OPTION("--Language",                                    Language)
    OPTION("--Output=HTML",                                 Output_HTML)
    OPTION("--Version",                                     Version)
    //Obsolete
    OPTION("-lang=raw",                                     Language)
    else
        return 1;

    return 0;
}

//---------------------------------------------------------------------------
CL_OPTION(f)
{
    MI.Menu_Debug_Complete(1);

    return 0;
}

//---------------------------------------------------------------------------
CL_OPTION(Help)
{
    TEXTOUT("MediaInfo Command line, ");
    STRINGOUT(MI.Menu_Help_Version());
    TEXTOUT("\n");
    return Help();
}

//---------------------------------------------------------------------------
CL_OPTION(Help_xxx)
{
    if (0);
    OPTION2("--Help-Inform",                                Help_Inform)
    else
        TEXTOUT("No help available");

    return -1;
}

//---------------------------------------------------------------------------
CL_OPTION(Info_Parameters)
{
    STRINGOUT(MI.Menu_Help_Info_Parameters());

    return -1;
}

//---------------------------------------------------------------------------
CL_OPTION(Inform)
{
    //Form : --Inform=Text
    size_t Egal_Pos=Argument.find(_T('='));
    if (Egal_Pos==String::npos)
        return Help_Inform();

    MI.Menu_Option_Preferences_Inform(Argument.substr(Egal_Pos+1));

    return 0;
}

//---------------------------------------------------------------------------
CL_OPTION(Language)
{
    MI.Menu_Language(_T("raw"));

    return 0;
}

//---------------------------------------------------------------------------
CL_OPTION(Output_HTML)
{
    MI.Menu_View_HTML();

    return 0;
}

//---------------------------------------------------------------------------
CL_OPTION(Version)
{
    TEXTOUT("MediaInfo Command line, ");
    STRINGOUT(MI.Menu_Help_Version());
    TEXTOUT("\n");

    return -1;
}
