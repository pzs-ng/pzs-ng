// File_Mpega - Info for MPEG Audio files
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
// Information about MPEG Audio files
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef MediaInfo_File_MpegaH
#define MediaInfo_File_MpegaH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/File__Base.h"
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

class File_Id3;

//***************************************************************************
// Class File_Mpega
//***************************************************************************

class File_Mpega : public File__Base
{
public :
    File_Mpega ();

    //Configuration
    size_t Frame_Count_Valid;
    size_t Frame_Count_Limit;

protected :
    //Format
    void Read_Buffer_Init ();
    void Read_Buffer_Continue ();
    void Read_Buffer_Finalize ();

    //Information
    void HowTo (stream_t StreamKind);

private :
    //Buffer
    bool Buffer_Parse();
    bool Element_Parse();

    //Element
    void Frame();
    void Id3v1();

    //Technical info
    size_t Mpeg;
    size_t Layer;
    size_t Protection;
    size_t BitRate;
    size_t SamplingRate;
    size_t Padding;
    size_t Private;
    size_t Channels;
    size_t Frame_Size;
    size_t Frame_Size_First;

    //VBR handling
    int32u VBR_Frames;
    int64u VBR_FileSize;
    Ztring BitRate_Mode;
    std::vector<size_t> BitRate_Mode_More;

    //Tags
    Ztring Encoded_Library;

    //Frames
    size_t NextFrame();
    size_t Header_Xing();
    size_t Header_VBRI();
    size_t Frame_Count;
    size_t Delay;

    //ID3
    File_Id3* ID3;
    int32u Id3v2_Size;
    Ztring Id3v1_Title;
    Ztring Id3v1_Artist;
    Ztring Id3v1_Album;
    Ztring Id3v1_Year;
    Ztring Id3v1_Comment;
    Ztring Id3v1_Track;
    Ztring Id3v1_Genre;
};

} //NameSpace

#endif
