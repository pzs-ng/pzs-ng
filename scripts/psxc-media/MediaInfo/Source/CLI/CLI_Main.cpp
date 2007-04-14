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
#ifndef __BORLANDC__
#include "Common/Core.h"
#else
#include "Common/Core.cpp" //Borland Crashes if we use .h, I don't know why!
#endif
#include "CommandLine_Parser.h"
#include "Help.h"
using namespace std;
using namespace MediaInfoLib;
//---------------------------------------------------------------------------

//***************************************************************************
// Main
//***************************************************************************

#if !defined(_WIN32) && defined(UNICODE)
int main(int argc, char* argv_ansi[])
#else
int main(int argc, MediaInfoLib::Char* argv[])
#endif
{
    GETCOMMANDLINE();

    Core MI;
    MI.Menu_View_Text(); //Default to text with CLI.

    vector<String> List;
    for (int Pos=1; Pos<argc; Pos++)
    {
        String Argument(argv[Pos]);
        int Return=Parse (MI, Argument);
        if (Return<0)
            return Return;
        if (Return>0)
            List.push_back(Argument);
    }

    //No filenames
    if (List.empty())
        return Help_Nothing();

    //Output
    for (size_t Pos=0; Pos<List.size(); Pos++)
    {
        MI.Menu_File_Open_Files_Continue(List[Pos]);
    }

    //Output
    Out(MI);
    //cin>>DoNotAnalyse_Pos;
    return 0;
}
//---------------------------------------------------------------------------

