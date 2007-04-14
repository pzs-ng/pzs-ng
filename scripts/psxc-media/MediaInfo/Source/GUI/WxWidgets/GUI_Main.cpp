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

//---------------------------------------------------------------------------
#include "GUI/WxWidgets/GUI_Main.h"
#include "Common/Core.h"
#include "GUI/WxWidgets/GUI_Main_FileDrop.h"
#include <wx/html/htmlwin.h>
#include <wx/file.h>
#include <wx/dnd.h>
#include <wx/fs_mem.h>
#include <wx/treectrl.h>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef __WXMSW__
    #include "Ressource/Image/MediaInfo.xpm"
#endif
//---------------------------------------------------------------------------

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
// Constructor
GUI_Main::GUI_Main(const wxPoint& pos, const wxSize& size, long style)
:wxFrame(NULL, -1, _T("MediaInfo"), pos, size, style)
{
    //Set the frame icon
    SetIcon(wxICON(aaaaaaaa));

    //Menu and ToolBar
    Menu_Create();
    ToolBar_Create();

    //Status bar
    CreateStatusBar();

    //GUI
    //Tree=new wxTreeCtrl(this, -1);
    //Tree->SetSize(GetClientSize());
    //Tree->Hide();
    Tree=NULL;
    HTML=NULL;
    Text=NULL;
    CenterOnScreen();

    //Core
    C=new Core;

    //Drag n drop
    FD=new FileDrop(C, this);
    SetDropTarget(FD);

    //Defaults
    C->Menu_View_HTML(); //Default to HTML with GUI.
    Refresh_View();
    //HTML->Show();
    //C->Menu_View_Tree();
    //Tree->Show();
    //wxTreeItemId RootID=Tree->AddRoot(wxT("Item"));
    //Tree->AppendItem(RootID, wxT("Item"));
}

//---------------------------------------------------------------------------
GUI_Main::~GUI_Main()
{
    SetDropTarget(NULL);
}

//***************************************************************************
// Events
//***************************************************************************

//---------------------------------------------------------------------------
void GUI_Main::OnSize(wxSizeEvent& WXUNUSED(event))
{
    if (IsShown())
        Resize_View();
}

//---------------------------------------------------------------------------
void GUI_Main::Resize_View()
{
    if (HTML)
        HTML->SetSize(1, 1, GetClientSize().GetWidth()-2, GetClientSize().GetHeight()-2);
    if (Text)
        Text->SetSize(0, 0, GetClientSize().GetWidth()-0, GetClientSize().GetHeight()-0);
}

//***************************************************************************
// Helpers
//***************************************************************************

//---------------------------------------------------------------------------
void GUI_Main::Refresh_View()
{
    C->Data_Prepare();
    if (C->Text_Get().empty() || C->Text_Get().find(_T("<html>"))<4)
        HTML_Show(C->Text_Get());
    else
        Text_Show(C->Text_Get());
}

//---------------------------------------------------------------------------
void GUI_Main::HTML_Hide()
{
    if (!HTML)
        return;

    delete HTML; HTML=NULL;
}

//---------------------------------------------------------------------------
void GUI_Main::HTML_Show(const String &ToShow)
{
    Text_Hide();

    if (!HTML)
    {
        HTML=new wxHtmlWindow(this, -1, wxDefaultPosition, wxDefaultSize);
        Resize_View();
        int Size[7]={ 7, 8, 10, 12, 16, 22, 30};
        HTML->SetFonts(wxEmptyString, wxEmptyString, Size);
    }

    wxFileSystem::AddHandler(new wxMemoryFSHandler);
    wxMemoryFSHandler::AddFile(_T("index.html"), ToShow.c_str());
    HTML->LoadPage(_T("memory:index.html"));
    wxMemoryFSHandler::RemoveFile(_T("index.html"));
}

//---------------------------------------------------------------------------
void GUI_Main::Text_Hide()
{
    if (!Text)
        return;

    delete Text; Text=NULL;
}

//---------------------------------------------------------------------------
void GUI_Main::Text_Show(const String &ToShow)
{
    HTML_Hide();

        //Text->SetDefaultStyle(Attr);

    if (!Text)
    {
        Text=new wxTextCtrl(this, -1, L"", wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_MULTILINE|wxTE_RICH|wxTE_RICH2);
        Resize_View();
        wxFont Font;
        Font.SetFamily(wxFONTFAMILY_MODERN);
        wxTextAttr Attr;
        Attr.SetFont(Font);
        Text->SetDefaultStyle(Attr);
    }
    else
        Text->Clear();

    Text->WriteText(ToShow.c_str());
}

