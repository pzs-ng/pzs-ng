// GUI_Main_Menu - WxWidgets GUI for MediaInfo, Menu
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
#include "GUI/WxWidgets/GUI_Main.h"
#include "Common/Core.h"
#include <wx/image.h>
//---------------------------------------------------------------------------

//***************************************************************************
// Events
//***************************************************************************

//---------------------------------------------------------------------------
enum
{
    ID_Menu_File_Open,
    ID_Menu_File_Open_Files,
    ID_Menu_File_Open_Directory,
    ID_Menu_File_Quit,
    ID_Menu_View_HTML,
    ID_Menu_View_Text,
    ID_Menu_Debug_Details,
    ID_Menu_Help_About,
    ID_Menu_Help_Info_Formats,
    ID_Menu_Help_Info_Codecs,
    ID_Menu_Help_Info_Parameters,
};

//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(GUI_Main, wxFrame)
    //Menu
    EVT_MENU(ID_Menu_File_Open_Files,       GUI_Main::OnMenu_File_Open_Files)
    EVT_MENU(ID_Menu_File_Open_Directory,   GUI_Main::OnMenu_File_Open_Directory)
    EVT_MENU(wxID_EXIT,                     GUI_Main::OnMenu_File_Quit)
    EVT_MENU(ID_Menu_View_HTML,             GUI_Main::OnMenu_View_HTML)
    EVT_MENU(ID_Menu_View_Text,             GUI_Main::OnMenu_View_Text)
    EVT_MENU(ID_Menu_Debug_Details,         GUI_Main::OnMenu_Debug_Details)
    EVT_MENU(wxID_ABOUT,                    GUI_Main::OnMenu_Help_About)
    EVT_MENU(ID_Menu_Help_Info_Formats,     GUI_Main::OnMenu_Help_Info_Formats)
    EVT_MENU(ID_Menu_Help_Info_Codecs,      GUI_Main::OnMenu_Help_Info_Codecs)
    EVT_MENU(ID_Menu_Help_Info_Parameters,  GUI_Main::OnMenu_Help_Info_Parameters)

    //GUI
    EVT_SIZE(                               GUI_Main::OnSize)
END_EVENT_TABLE()


//---------------------------------------------------------------------------
#ifndef __WXMSW__
    #include "Ressource/Image/Menu/File_Open_File.xpm"
    #include "Ressource/Image/Menu/File_Open_Directory.xpm"
    #include "Ressource/Image/Menu/Help_About.xpm"
#endif

//***************************************************************************
// Menu creation
//***************************************************************************

//---------------------------------------------------------------------------
void GUI_Main::Menu_Create()
{
    //File - Open
    Menu_File_Open              =new wxMenu;
    Menu_File_Open_Files        =Menu_File_Open->Append(ID_Menu_File_Open_Files, _T("Open &file(s)"));
    Menu_File_Open_Directory    =Menu_File_Open->Append(ID_Menu_File_Open_Directory, _T("Open &directory"));

    //File
    Menu_File=new wxMenu;
                                 Menu_File->Append(ID_Menu_File_Open, _T("Open"), Menu_File_Open);
                                 Menu_File->AppendSeparator();
    Menu_File_Quit              =Menu_File->Append(wxID_EXIT, _T("E&xit\tAlt-X"), _T("Quit this program"));

    //View
    Menu_View                   =new wxMenu;
    Menu_View_HTML              =Menu_View->AppendRadioItem(ID_Menu_View_HTML, _T("HTML"), _T("HTML view"));
    Menu_View_Text              =Menu_View->AppendRadioItem(ID_Menu_View_Text, _T("Text"), _T("Text view"));

    //Debug
    Menu_Debug                  =new wxMenu;
    Menu_Debug_Details          =Menu_Debug->AppendCheckItem(ID_Menu_Debug_Details, _T("Details"), _T("Details text view"));

    //Help
    Menu_Help                   =new wxMenu;
    Menu_Help_About             =Menu_Help->Append(wxID_ABOUT, _T("&About...\tF1"), _T("Show about dialog"));
                                 Menu_Help->AppendSeparator();
    Menu_Help_Info_Formats      =Menu_Help->Append(ID_Menu_Help_Info_Formats, _T("Known formats"));
    Menu_Help_Info_Codecs       =Menu_Help->Append(ID_Menu_Help_Info_Codecs, _T("Known codecs"));
    Menu_Help_Info_Parameters   =Menu_Help->Append(ID_Menu_Help_Info_Parameters, _T("Known parameters"));

    //Main
    Menu=new wxMenuBar();
    Menu->Append(Menu_File,     _T("&File"));
    Menu->Append(Menu_View,     _T("&View"));
    Menu->Append(Menu_Debug,    _T("&Debug"));
    Menu->Append(Menu_Help,     _T("&Help"));
    SetMenuBar(Menu);
}

//***************************************************************************
// Menu actions
//***************************************************************************

//---------------------------------------------------------------------------
void GUI_Main::OnMenu_File_Quit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

//---------------------------------------------------------------------------
void GUI_Main::OnMenu_File_Open_Files(wxCommandEvent& WXUNUSED(event))
{
    //User interaction
    wxFileDialog* Dialog=new wxFileDialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"), wxOPEN|wxFILE_MUST_EXIST|wxMULTIPLE);
    if (Dialog->ShowModal()!=wxID_OK)
        return;
    wxArrayString FileNames;
    Dialog->GetPaths(FileNames);
    delete Dialog;

    //Showing
    C->Menu_File_Open_Files_Begin();
    for (size_t Pos=0; Pos<FileNames.size(); Pos++)
        C->Menu_File_Open_Files_Continue(FileNames[Pos].c_str());
    Refresh_View();
}

//---------------------------------------------------------------------------
void GUI_Main::OnMenu_File_Open_Directory(wxCommandEvent& WXUNUSED(event))
{
    //User interaction
    wxDirDialog* Dialog=new wxDirDialog(this, _T("Choose a directory"));
    if (Dialog->ShowModal()!=wxID_OK)
        return;
    wxString DirName=Dialog->GetPath();
    delete Dialog;

    //Showing
    C->Menu_File_Open_Files_Begin();
    C->Menu_File_Open_Files_Continue(DirName.c_str());
    Refresh_View();
}

//---------------------------------------------------------------------------
void GUI_Main::OnMenu_View_HTML(wxCommandEvent& WXUNUSED(event))
{
    //GUI
    Menu_Debug_Details->Check(false);

    //Showing
    C->Menu_View_HTML();
    Refresh_View();
}

//---------------------------------------------------------------------------
void GUI_Main::OnMenu_View_Text(wxCommandEvent& WXUNUSED(event))
{
    //GUI
    Menu_Debug_Details->Check(false);

    //Showing
    C->Menu_View_Text();
    Refresh_View();
}

//---------------------------------------------------------------------------
void GUI_Main::OnMenu_Debug_Details(wxCommandEvent& WXUNUSED(event))
{
    //GUI
    Menu_View_HTML->Enable(!Menu_Debug_Details->IsChecked());
    Menu_View_Text->Enable(!Menu_Debug_Details->IsChecked());

    //Showing
    if (Menu_Debug_Details->IsChecked())
    {
        C->Menu_Debug_Details(1);
    }
    else
    {
        if (Menu_View_HTML->IsChecked())
            C->Menu_View_HTML();
        else
            C->Menu_View_Text();
    }
    Refresh_View();
}

//---------------------------------------------------------------------------
void GUI_Main::OnMenu_Help_About(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("This is the About dialog of the minimal MediaInfo sample.\n"), _T("About Minimal"), wxOK | wxICON_INFORMATION, this);
}

//---------------------------------------------------------------------------
void GUI_Main::OnMenu_Help_Info_Formats(wxCommandEvent& WXUNUSED(event))
{
    Text_Show(C->Menu_Help_Info_Formats());
}

//---------------------------------------------------------------------------
void GUI_Main::OnMenu_Help_Info_Codecs(wxCommandEvent& WXUNUSED(event))
{
    Text_Show(C->Menu_Help_Info_Codecs());
}

//---------------------------------------------------------------------------
void GUI_Main::OnMenu_Help_Info_Parameters(wxCommandEvent& WXUNUSED(event))
{
    Text_Show(C->Menu_Help_Info_Parameters());
}

//***************************************************************************
// ToolBar creation
//***************************************************************************

//---------------------------------------------------------------------------
void GUI_Main::ToolBar_Create()
{
    //ToolBar
    wxToolBar* ToolBar=CreateToolBar(wxTB_VERTICAL);
    ToolBar->SetToolBitmapSize(wxSize(32, 32));
    ToolBar->AddTool(ID_Menu_File_Open_Files, _T("Open &file(s)"), wxICON(File_Open_File));
    ToolBar->AddTool(ID_Menu_File_Open_Directory, _T("Open &directory"), wxICON(File_Open_Directory));
    ToolBar->AddSeparator();
    ToolBar->AddTool(ID_Menu_Help_About, _T("About"), wxICON(Help_About));
    ToolBar->Realize();
}
