// GUI_Main_FileDrop - FileDrop function
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
// FileDrop function
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef FileDropH
#define FileDropH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "GUI/WxWidgets/GUI_Main.h"
#include <wx/dnd.h>
class Core;
//----------------------------------------------------------------------------

//***************************************************************************
// FileDrop
//***************************************************************************

class FileDrop : public wxFileDropTarget
{
public:
    FileDrop(Core* C_, GUI_Main* GUI_) {C=C_; GUI=GUI_;};
    bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);

private:
    Core* C;
    GUI_Main* GUI;
};

#endif
