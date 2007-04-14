// GUI_About - About interface of MediaInfo
// Copyright (C) 2002-2005 Jerome Martinez, Zen@MediaArea.net
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef GUI_AboutH
#define GUI_AboutH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <TntComCtrls.hpp>
#include <TntStdCtrls.hpp>
#include <TntForms.hpp>
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include <ToolWin.hpp>
#include <ZenLib/Ztring.h>
//---------------------------------------------------------------------------

//***************************************************************************
// Class
//***************************************************************************

class TAboutF : public TTntForm
{
__published:    // IDE-managed Components
    TTntLabel *Translator;
    TTntMemo *Memo;
    TTntButton *OK;
    TTntButton *WebSite;
    TTntToolBar *ToolBar1;
    TToolButton *ToolButton1;
    TTntButton *NewVersion;
    TTntButton *Donate;
    TToolBar *ToolBar2;
    TToolButton *ToolButton2;
    TTntButton *WriteMe;
    TTntButton *WriteToTranslator;
    TImageList *ImageList1;
    TImageList *ImageList2;
    void __fastcall WebSiteClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall NewVersionClick(TObject *Sender);
    void __fastcall DonateClick(TObject *Sender);
    void __fastcall WriteMeClick(TObject *Sender);
    void __fastcall WriteToTranslatorClick(TObject *Sender);
private:    // User declarations
public:        // User declarations
    __fastcall TAboutF(TComponent* Owner);
    ZenLib::Ztring Translator_Url;
};
//---------------------------------------------------------------------------
extern PACKAGE TAboutF *AboutF;
//---------------------------------------------------------------------------
#endif
