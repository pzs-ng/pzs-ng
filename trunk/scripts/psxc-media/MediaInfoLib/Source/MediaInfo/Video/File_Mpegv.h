// File_Mpegv - Info for MPEG Video files
// Copyright (C) 2004-2007 Jerome Martinez, Zen@MediaArea.net
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
// Information about MPEG Video files
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef MediaInfo_MpegvH
#define MediaInfo_MpegvH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/File__Base.h"
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Class File_Mpegv
//***************************************************************************

class File_Mpegv : public File__Base
{
protected :
    //Format
    void Read_Buffer_Init ();
    void Read_Buffer_Continue ();
    void Read_Buffer_Finalize ();

    //Information
    void HowTo (stream_t StreamKind);

public :
    //Public parameters
    int MPEG_Version;

private :
    //Buffer
    bool Buffer_Parse();
    bool Element_Parse();
    int8u  Element_Name;
    size_t Element_Size;
    size_t Element_Next;

    //Elements
    void Picture_Start();
    void Slice_Start();
    void User_Start();
    void Sequence_Header();
    void Sequence_Error();
    void Extension_Start();
    void Sequence_End();
    void Group_Start();
    size_t Frame_Count;

    //Temp
    size_t Time_Begin_Seconds;
    size_t Time_Begin_Frames;
    size_t Time_End_Seconds;
    size_t Time_End_Frames;
    size_t Time_End_Old;
    bool   Time_End_NeedComplete;
    size_t Width;
    size_t Height;
    size_t RatioValue;
    float32 FrameRate;
    size_t BitRate;
    Ztring Library;

    //Error controls
    size_t User_Start_Count;
    std::vector<int8u> Frame_ShouldBe;

    //Helpers
    bool Detect_NonMPEGV();
    bool Element_Size_Get();
    bool Element_Name_IsOK();
    bool NextFrame();
};

} //NameSpace

#endif
