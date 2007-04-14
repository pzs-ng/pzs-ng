// File_Flv - Info for Flash files
// Copyright (C) 2005-2007 Jerome Martinez, Zen@MediaArea.net
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
// Information about Flash files
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef MediaInfo_File_FlvH
#define MediaInfo_File_FlvH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/File__Base.h"
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Class File_Flv
//***************************************************************************

class File_Flv : public File__Base
{
protected :
    //Format
    void Read_Buffer_Continue ();
    void Read_Buffer_Finalize ();

    //Information
    void HowTo (stream_t StreamKind);

private :
    bool Buffer_Parse();
    bool Element_Parse();
    size_t Element_Size;
    int8u  Element_Name;
    size_t Element_Next;

    //Elements
    void header();
    void video();
    void video_H263();
    void video_ScreenVideo();
    void video_VP6(bool WithAlpha);
    void audio();
    void meta();
    void meta_Parse();
    void meta_Parse_Value(int32u &Stream_Pos, stream_t &StreamKind, Ztring &Name, Ztring &Value);

    //Temp
    int32u Flags;
};

} //NameSpace

#endif
