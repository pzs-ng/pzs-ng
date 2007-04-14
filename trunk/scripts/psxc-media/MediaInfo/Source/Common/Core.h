// Core - Main functions
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
//
// Core functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef CoreH
#define CoreH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifdef MEDIAINFO_DLL
    #include "MediaInfoDLL/MediaInfoDLL.h"
#else
    #include "MediaInfo/MediaInfoList.h"
#endif
using namespace MediaInfoLib;
//---------------------------------------------------------------------------

//***************************************************************************
// Class Core
//***************************************************************************

class Core
{
public:
    //Constructor/Destructor
    Core();
    ~Core();

    //Menu
    void    Menu_File_Open_File             (const String &FileName);
    void    Menu_File_Open_Files_Begin      ();
    void    Menu_File_Open_Files_Continue   (const String &FileName);
    void    Menu_File_Open_Directory        (const String &DirectoryName);
    void    Menu_View_Tree                  ();
    void    Menu_View_HTML                  ();
    void    Menu_View_Text                  ();
    void    Menu_Option_Preferences_Inform  (const String &Inform);
    void    Menu_Option_Preferences_Option  (const String &Param, const String &Value);
    void    Menu_Debug_Complete             (bool Value);
    void    Menu_Debug_Details              (float Value);
    String &Menu_Help_Version               ();
    String &Menu_Help_Info_Formats          ();
    String &Menu_Help_Info_Codecs           ();
    String &Menu_Help_Info_Parameters       ();
    void    Menu_Language                   (const String &Language);

    //Datas
    void    Data_Prepare();
    String &Text_Get    ();

private:
    MediaInfoLib::MediaInfoList* MI;
    String  Text;
};

#endif
