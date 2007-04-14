// File_Mpeg - Info for MPEG files
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
// Information about MPEG files
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef MediaInfo_MpegH
#define MediaInfo_MpegH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/File__Base.h"
#include "ZenLib/BitStream.h"
#include <ZenLib/Conf.h>
#include <vector>
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//---------------------------------------------------------------------------
typedef struct
{
  int64u PTS;
  int64u DTS;
  bool Have_PTS;
  bool Have_DTS;
} Mpeg_TimeStamp;

//---------------------------------------------------------------------------

//***************************************************************************
// Class File_Mpeg
//***************************************************************************

class File_MpegPs : public File__Base
{
protected :
    //Format
    void Read_Buffer_Init ();
    void Read_Buffer_Unsynched ();
    void Read_Buffer_Continue ();
    void Read_Buffer_Finalize ();

    //Information
    void HowTo (stream_t StreamKind);

public :
    //Where this file comes from?
    int32u FromTS; //ID
    int8u MPEG_Version;

private :
    //Buffer
    bool Buffer_Parse ();
    bool Buffer_Parse_Header ();
    bool Buffer_Parse_Header_Data ();
    bool Buffer_Parse_Header_Data_MPEG1 ();
    bool Buffer_Parse_Header_Data_MPEG2 ();
    bool Element_Parse();
    size_t Element_Size;
    size_t Element_HeaderSize;
    int8u  Element_Name;
    size_t Element_Next;

    //Packet
    void MPEG_program_end();    //0xB9
    void pack_start();          //0xBA
    void system_header_start(); //0xBB
    void program_stream_map();  //0xBC
    void private_stream_1();    //0xBD
    void padding_stream();      //0xBE
    void private_stream_2();    //0xBF
    void audio_stream();        //0xC0 --> 0xDF
    void video_stream();        //0xE0 --> 0xEF
    int8u Video_Count;
    int8u Audio_Count;

    //Helpers
    File__Base* Private1_ChooseParser(int8u Private1_ID);

    bool   Detect_NonMPEGPS();
    void   Detect_MPEG_Version();
    bool   Detect_EOF_Needed();
    bool   RemoveZeros();
    bool   Synchronize();
    bool   Element_Name_Get();
    bool   Element_Size_Get();
    bool   Element_Size_Get_Unlimited();
    bool   Element_Size_Get_Element_Next();
    bool   Element_Name_IsOK();


    size_t MPEG_PES_Data   ();
    size_t MPEG_Stream     ();
    int64u MPEG_TimeStamp_Read (BitStream &BS);

    //Temp
    Mpeg_TimeStamp TimeStamp;
    std::vector<File__Base*>    Video_Handle;
    std::vector<File__Base*>    Audio_Handle;
    std::vector<File__Base*>    Private1_Handle;
    std::vector<Mpeg_TimeStamp> Video_TimeStamp;
    std::vector<Mpeg_TimeStamp> Video_TimeStamp_End;
    std::vector<size_t>         Video_Handle_Error;
    std::vector<Mpeg_TimeStamp> Audio_TimeStamp;
    std::vector<Mpeg_TimeStamp> Audio_TimeStamp_End;
    std::vector<Mpeg_TimeStamp> Private1_TimeStamp;
    std::vector<Mpeg_TimeStamp> Private1_TimeStamp_End;
    bool Video_Unlimited;

    //Error controls
    std::vector<int8u> StreamKind;
};

} //NameSpace

#endif
