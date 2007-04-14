// File_Rm - Info for Real Media files
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
// Information about Real Media files
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef MediaInfo_File_RmH
#define MediaInfo_File_RmH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/File__Base.h"
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Classe File_Rm
//***************************************************************************

class File_Rm : public File__Base
{
protected :
    //Formats
    void Read_Buffer_Continue ();

    //Information
    void HowTo (stream_t StreamKind);

private :
    //Buffer
    size_t Size;
    bool ShouldStop; //Offset is too far, should stop analyse

    //Chunks
    void Cont ();
    void Data ();
    void Data_Entry ();
    void Mdpr ();
    void Mdpr_FileInfo (size_t Offset, size_t Size);
    void Mdpr_FileInfo_Info (size_t Offset, size_t Size);
    void Mdpr_Audio (size_t Offset, size_t Size);
    void Mdpr_Video (size_t Offset, size_t Size);
    void Prop ();
    void Rjmd ();
    void Rjmd_Property (size_t Offset, size_t Size, Ztring CompleteName);
    void Rmf  ();
};

} //NameSpace

#endif
