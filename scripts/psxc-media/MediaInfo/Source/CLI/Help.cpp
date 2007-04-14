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
#include "CLI_Main.h"
using namespace std;
using namespace MediaInfoLib;
//---------------------------------------------------------------------------

void Out(Core &MI)
{
    MI.Data_Prepare();
    STRINGOUT(MI.Text_Get());
}

//***************************************************************************
//
//***************************************************************************

//---------------------------------------------------------------------------
int Help()
{
    TEXTOUT("Usage: \"MediaInfo [-Options...] FileName1 [Filename2...]\"\n");
    TEXTOUT("\n");
    TEXTOUT("Options:\n");
    TEXTOUT("-f  Display full information (all internal tags)\n");
    TEXTOUT("--Info-Parameters  Display available parameters\n");
    TEXTOUT("--Inform=...  Specify a template\n");
    TEXTOUT("             (--Help-Inform for more info)\n");
    TEXTOUT("--Help  Display this help and exit\n");
    TEXTOUT("--Language=raw  Display Non-translated words (internal text)\n");
    TEXTOUT("--Output=HTML  Display with HTML tags for parsing by a browser\n");
    TEXTOUT("--Version  Display version information and exit\n");
    TEXTOUT("\n");
    TEXTOUT("--Help-AnOption  Display more details about \"AnOption\"");

    return -1;
}

//---------------------------------------------------------------------------
int Help_Nothing()
{
    TEXTOUT("Usage: \"MediaInfo [-Options...] FileName1 [Filename2...]\"\n");
    TEXTOUT("\"MediaInfo --Help\" for displaying more information");

    return -1;
}

//---------------------------------------------------------------------------
int Help_Inform()
{
    TEXTOUT("--Inform=...  Specify a template (BETA)\n");
    TEXTOUT("Usage: \"MediaInfo --Inform=xxx;Text FileName\"\n");
    TEXTOUT("\n");
    TEXTOUT("xxx can be: General, Video, Audio, Text, Chapter, Image, Menu\n");
    TEXTOUT("Text can be the template text, or a filename\n");
    TEXTOUT("     Filename must be in the form file://filename\n");
    TEXTOUT("\n");
    TEXTOUT("See --Info-Parameters for available parameters in the text\n");
    TEXTOUT("(Parameters must be surrounded by \"%%\" sign)\n");
    TEXTOUT("\n");
    TEXTOUT("Example: \"MediaInfo -Inform=Video;%%AspectRatio%% FileName\"\n");
    TEXTOUT("Example: \"MediaInfo -Inform=Video;file://Video.txt FileName\"\n");
    TEXTOUT("and Video.txt contains \"%%AspectRatio%%\"\n");
    TEXTOUT("for having Video Aspect Ratio.");

    return -1;
}
