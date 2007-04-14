// GUI_Main - WxWidgets GUI for MediaInfo
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
// WxWidgets GUI for MediaInfo
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef GUI_MainH
#define GUI_MainH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "wx/wxprec.h"
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// the application icon (under Windows and OS/2 it is in resources)
#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)
    //#include "mondrian.xpm"
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifdef MEDIAINFO_DLL
    #include "MediaInfoDLL/MediaInfoDLL.h"
#else
    #include "MediaInfo/MediaInfoList.h"
#endif
using namespace MediaInfoLib;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
class FileDrop;
class Core;
class wxTreeCtrl;
class wxHtmlWindow;
class wxTextCtrl;
//---------------------------------------------------------------------------

//***************************************************************************
// GUI_Main
//***************************************************************************

class GUI_Main : public wxFrame
{
public:
    GUI_Main (const wxPoint& pos, const wxSize& size, long style = wxDEFAULT_FRAME_STYLE);
    ~GUI_Main ();

    //Menu - List
    wxMenuBar*  Menu;
    wxMenu*     Menu_File;
    wxMenu*     Menu_File_Open;
    wxMenuItem* Menu_File_Open_Files;
    wxMenuItem* Menu_File_Open_Directory;
    wxMenuItem* Menu_File_Quit;
    wxMenu*     Menu_View;
    wxMenuItem* Menu_View_Tree;
    wxMenuItem* Menu_View_HTML;
    wxMenuItem* Menu_View_Text;
    wxMenu*     Menu_Debug;
    wxMenuItem* Menu_Debug_Details;
    wxMenu*     Menu_Help;
    wxMenuItem* Menu_Help_About;
    wxMenuItem* Menu_Help_Info_Codecs;
    wxMenuItem* Menu_Help_Info_Formats;
    wxMenuItem* Menu_Help_Info_Parameters;

    //Menu - Actions
    void Menu_Create();
    void OnMenu_File_Open_Files     (wxCommandEvent &event);
    void OnMenu_File_Open_Directory (wxCommandEvent &event);
    void OnMenu_File_Quit           (wxCommandEvent &event);
    void OnMenu_View_Tree           (wxCommandEvent &event);
    void OnMenu_View_HTML           (wxCommandEvent &event);
    void OnMenu_View_Text           (wxCommandEvent &event);
    void OnMenu_Debug_Details       (wxCommandEvent &event);
    void OnMenu_Help_About          (wxCommandEvent &event);
    void OnMenu_Help_Info_Codecs    (wxCommandEvent &event);
    void OnMenu_Help_Info_Formats   (wxCommandEvent &event);
    void OnMenu_Help_Info_Parameters(wxCommandEvent &event);
    void OnSize                     (wxSizeEvent    &event);

    //ToolBar - List
    wxToolBar*  ToolBar;

    //ToolBar - Actions
    void ToolBar_Create();

    //Helpers
    void Refresh_View               ();
    void Resize_View                ();
    void HTML_Hide                  ();
    void HTML_Show                  (const String &ToShow);
    void Text_Hide                  ();
    void Text_Show                  (const String &ToShow);

private:
    //Non-GUI Elements
    FileDrop* FD;
    Core* C;

    //GUI
    wxTreeCtrl* Tree;
    wxHtmlWindow* HTML;
    wxTextCtrl* Text;

    //wxWidgets
    DECLARE_EVENT_TABLE() // any class wishing to process wxWindows events must use this macro
};

#endif


