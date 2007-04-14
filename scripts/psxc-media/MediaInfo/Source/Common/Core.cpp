// Main - Main functions
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
#include "Common/Core.h"
//---------------------------------------------------------------------------

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
Core::Core()
{
    MI=new MediaInfoLib::MediaInfoList;
}

Core::~Core()
{
    delete MI;
}

//***************************************************************************
// Show Info
//***************************************************************************

//---------------------------------------------------------------------------
void Core::Data_Prepare()
{
    //Inform
    Text=MI->Inform((size_t)-1).c_str();
}

//***************************************************************************
// Menu
//***************************************************************************

//---------------------------------------------------------------------------
void Core::Menu_File_Open_File (const String& FileName)
{
    Menu_File_Open_Files_Begin();
    Menu_File_Open_Files_Continue(FileName);
}

//---------------------------------------------------------------------------
void Core::Menu_File_Open_Files_Begin ()
{
    MI->Close(Error);
}

//---------------------------------------------------------------------------
void Core::Menu_File_Open_Files_Continue (const String &FileName)
{
    MI->Open(FileName);
}

//---------------------------------------------------------------------------
void Core::Menu_File_Open_Directory (const String &DirectoryName)
{
    MI->Open(DirectoryName, FileOption_Recursive);
}

//---------------------------------------------------------------------------
void Core::Menu_View_Tree ()
{
    Text=_T("Root\n Item");
}

//---------------------------------------------------------------------------
void Core::Menu_View_HTML ()
{
    MI->Option(_T("Inform"), _T("HTML"));
}

//---------------------------------------------------------------------------
void Core::Menu_View_Text ()
{
    MI->Option(_T("Inform"), _T(""));
}

//---------------------------------------------------------------------------
void Core::Menu_Option_Preferences_Inform (const String& Inform)
{
    MI->Option(_T("Inform"), Inform);
}

//---------------------------------------------------------------------------
void Core::Menu_Option_Preferences_Option (const String& Param, const String& Value)
{
    MI->Option(Param, Value);
}

//---------------------------------------------------------------------------
void Core::Menu_Debug_Complete (bool Value)
{
    if (Value)
        MI->Option(_T("Complete"), _T("1"));
    else
        MI->Option(_T("Complete"), _T("0"));
}

//---------------------------------------------------------------------------
void Core::Menu_Debug_Details (float Value)
{
    StringStream Details;
    Details<<_T("Details;");
    Details<<Value;

    MI->Option(_T("Inform"), String(_T("Details;0.9")));
}

//---------------------------------------------------------------------------
String& Core::Menu_Help_Version ()
{
    Text=MI->Option(_T("Info_Version"));
    return Text;
}

//---------------------------------------------------------------------------
String& Core::Menu_Help_Info_Formats ()
{
    Text=MI->Option(_T("Info_Capacities"));
    return Text;
}

//---------------------------------------------------------------------------
String& Core::Menu_Help_Info_Codecs ()
{
    Text=MI->Option(_T("Info_Codecs"));
    return Text;
}

//---------------------------------------------------------------------------
String& Core::Menu_Help_Info_Parameters ()
{
    Text=MI->Option(_T("Info_Parameters"));
    return Text;
}

//---------------------------------------------------------------------------
void Core::Menu_Language (const String& Language)
{
    MI->Option(_T("Language"), Language);
}

//***************************************************************************
// Helpers
//***************************************************************************

//---------------------------------------------------------------------------
String& Core::Text_Get ()
{
    return Text;
}
