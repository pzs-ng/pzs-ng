// File_Dts - Info for DTS files
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
// Information about DTS files
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
// Compilation condition
#include <MediaInfo/Setup.h>
#if defined(MEDIAINFO_DTS_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Audio/File_Dts.h"
#include <ZenLib/Utils.h>
#include <ZenLib/BitStream.h>
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// DTS - Format
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  StartCode                        4 bytes - 0x7FFE8001
//  Frame type                       1 bit
//  Deficit semple count?            5 bits
//  CRC present                      1 bit
//  Number of PCM sample block?      7 bits
//  Primary frame byte size?        14 bits
//  Channels                         6 bits
//  Sampling rate                    4 bits
//  Bit rate                         5 bits
//  ...                             14 bits
//  Resolution                       3 bits
//  ...
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
const size_t DTS_SamplingRate[]=
{     0,  8000,  16000,  32000,      0,      0,  11025,  22050,
  44100,     0,      0,  12000,  24000,  48000,  96000, 192000};
const size_t DTS_BitRate[]=
{   32000,   56000,   64000,   96000,  112000,  128000,  192000,  224000,
   256000,  320000,  384000,  448000,  512000,  576000,  640000,  768000,
   960000, 1024000, 1152000, 1280000, 1344000, 1408000, 1411200, 1472000,
  1536000, 1920000, 2048000, 3072000, 3840000};
const size_t DTS_Channels[]=
{1, 2, 2, 2, 2, 3, 3, 4,
 4, 5, 6, 6, 6, 7, 8, 8};
const size_t DTS_Resolution[]=
{16, 16, 20, 20, 24, 24, 0, 0};
const Char*  DTS_ChannelPositions[]=
{
    _T("Mono"),
    _T("Dual mono"),
    _T("L R"),
    _T("L R"),
    _T("L R"),
    _T("L C R"),
    _T("Front: L R, Surround: C"),
    _T("Front: L C R, Surround: C"),
    _T("Front: L R, Surround: L R"),
    _T("Front: L C R, Surround: L R"),
    _T("Front: L C C R, Surround: L R"),
    _T("Front: L C R, Rear: L R"),
    _T("Front: L R, Middle: L R, Rear: L R"),
    _T("Front: L C R, Middle: L R, surround: L R"),
    _T("Front: L R, Middle: L R, Surround: L C C R"),
    _T("Front: L C R, Middle: L R, Surround: L C R"),
};
//---------------------------------------------------------------------------

//***************************************************************************
// Format
//***************************************************************************

//---------------------------------------------------------------------------
void File_Dts::Read_Buffer_Init()
{
    Delay=0;
}

#define BEGIN \
    BitStream BS(Buffer+Buffer_Offset, Element_Size); \

#define END \
    { \
        BS.Byte_Align(); \
        while(BS.Remain()>0 && BS.Get(8)==0); \
        INTEGRITY(BS.Offset_Get()) \
    } \

#define GET(_BITS, _INFO, _NAME) \
    { \
        int32u _NAME=BS.Get(_BITS); \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Error, #_NAME, _NAME); \
        } \
        _INFO=_NAME; \
    }

#define SKIP(_BITS, _NAME) \
    { \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Error, #_NAME, BS.Get(_BITS)); \
        } \
        else \
            BS.Skip(_BITS); \
    } \

//---------------------------------------------------------------------------
void File_Dts::Read_Buffer_Continue()
{
    if (Buffer_Size<=12)
        return;
    if (Count_Get(Stream_Audio)>0)
    {
        File_Offset=File_Size;
        return; //Already done!
    }

    //False positive
    if (CC4(Buffer)==CC4("RIFF"))
    {
        //File_Offset=File_Size;
        //return;
    }

    while (Buffer_Offset<=Buffer_Size-12
      && !(BigEndian2int16u   (Buffer+Buffer_Offset+0)==0x7FFE && BigEndian2int16u   (Buffer+Buffer_Offset+2)==0x8001) //16 bits and big    endian bitstream
      && !(LittleEndian2int16u(Buffer+Buffer_Offset+0)==0x7FFE && LittleEndian2int16u(Buffer+Buffer_Offset+2)==0x8001) //16 bits and little endian bitstream
      && !(BigEndian2int16u   (Buffer+Buffer_Offset+0)==0x1FFF && BigEndian2int16u   (Buffer+Buffer_Offset+2)==0xE800 && (BigEndian2int16u   (Buffer+Buffer_Offset+4)&0xFFF0)==0x07F0) //14 bits and big    endian bitstream
      && !(LittleEndian2int16u(Buffer+Buffer_Offset+0)==0x1FFF && LittleEndian2int16u(Buffer+Buffer_Offset+2)==0xE800 && (LittleEndian2int16u(Buffer+Buffer_Offset+4)&0xFFF0)==0x07F0) //14 bits and little endian bitstream
      )
        Buffer_Offset++;
    Delay+=Buffer_Offset;
    if (Buffer_Offset>Buffer_Size-12)
    {
        if (!CompleteFileName.empty())
            File_Offset=File_Size;
        File_Offset=File_Size;
        return;
    }

    //Fill stream
    Stream_Prepare(Stream_General);
    General[0](_T("Format"))=_T("DTS");
    General[0](_T("Format/String"))=_T("DTS");
    General[0](_T("Format/Extensions"))=_T("DTS");
    Stream_Prepare(Stream_Audio);
    Audio[0](_T("Codec"))=_T("DTS");


    //Configuring
    int Word=1;
    int BigEndian=1;
    if (0) ;
    else if (BigEndian2int16u   (Buffer+Buffer_Offset)==0x7FFE) //16 bits and big    endian bitstream
    {
    }
    else if (LittleEndian2int16u(Buffer+Buffer_Offset)==0x7FFE) //16 bits and little endian bitstream
    {
        BigEndian=0;
    }
    else if (BigEndian2int16u   (Buffer+Buffer_Offset)==0x1FFF) //14 bits and big    endian bitstream
    {
        Word=0;
    }
    else if (LittleEndian2int16u(Buffer+Buffer_Offset)==0x1FFF) //14 bits and little endian bitstream
    {
        Word=0;
        BigEndian=0;
    }

    //14-bits and Little endian are not yet supported
    if (!Word || !BigEndian)
    {
        File_Offset=File_Size;
        return;
    }

    //Parsing
    Element_Size=12;
    int32u Frame_Length, Frame_Size, Flags, Channels, SamplingRate, BitRate, LFE, Resolution;
    BEGIN
    SKIP(32,                                                    start_code)
    SKIP( 1,                                                    frame_type)
    SKIP( 5,                                                    samples_deficit)
    SKIP( 1,                                                    crc_present)
    GET ( 7, Frame_Length,                                      frame_length) INFO(Ztring::ToZtring((Frame_Length+1)*32)+_T(" bytes"));
    GET (14, Frame_Size,                                        frame_size) INFO(Ztring::ToZtring(Frame_Size+1)+_T(" bytes"));
    Frame_Size+=1;
    if (!Word) Frame_Size=Frame_Size*8/14*2; //Word is on 14 bits!
    GET ( 6, Channels,                                          channels) INFO(Ztring::ToZtring(DTS_Channels[Channels])+_T(" channels"));
    GET ( 4, SamplingRate,                                      sampling_rate) INFO(Ztring::ToZtring(DTS_SamplingRate[SamplingRate])+_T(" Hz"));
    GET ( 5, BitRate,                                           bit_rate) INFO(Ztring::ToZtring(DTS_BitRate[BitRate])+_T(" bps"));
    SKIP(10,                                                    unknown)
    GET ( 2, LFE,                                               lfe)
    SKIP( 2,                                                    unknown)
    GET ( 3, Resolution,                                        resolution) INFO(Ztring::ToZtring(DTS_Resolution[Resolution])+_T(" bits"));

    //-SamplingRate
    Audio[0](_T("SamplingRate")).From_Number(DTS_SamplingRate[SamplingRate]);
    //-BitRate
    if (BitRate<29)
        Audio[0](_T("BitRate")).From_Number(DTS_BitRate[BitRate]);
    else if (BitRate==29)
        Audio[0](_T("BitRate"))=_T("Open");
    else if (BitRate==30)
        Audio[0](_T("BitRate"))=_T("Variable");
    else if (BitRate==31)
        Audio[0](_T("BitRate"))=_T("LossLess");
    if (Delay>100 && BitRate<29 && CompleteFileName.empty())
        Audio[0](_T("Delay")).From_Number((float)Delay*1000/(DTS_BitRate[BitRate]/8), 0);
    //-Channels
    if (Channels<16)
    {
        Audio[0](_T("Channel(s)")).From_Number(DTS_Channels[Channels]+(LFE>0?1:0));
        Audio[0](_T("ChannelPositions"))=DTS_ChannelPositions[Channels];
    }
    else
    {
        Audio[0](_T("Channel(s)")).From_Number(8);
        Audio[0](_T("ChannelPositions"))=_T("User defined");
    }
    //LFE
    if (LFE)
        Audio[0](_T("ChannelPositions"))+=_T(", Subwoofer");
    //-Resolution
    Audio[0](_T("Resolution")).From_Number(DTS_Resolution[Resolution]);

    //Finnished
    File_Offset=File_Size;
}

//---------------------------------------------------------------------------
void File_Dts::HowTo(stream_t StreamKind)
{
    switch (StreamKind)
    {
        case (Stream_General) :
            Fill_HowTo("Format", "R");
            break;
        case (Stream_Video) :
            break;
        case (Stream_Audio) :
            Fill_HowTo("Codec", "R");
            Fill_HowTo("BitRate", "R");
            Fill_HowTo("Channel(s)", "R");
            Fill_HowTo("ChannelPositions", "R");
            Fill_HowTo("SamplingRate", "R");
            Fill_HowTo("Resolution", "N");
            break;
        case (Stream_Text) :
            break;
        case (Stream_Chapters) :
            break;
        case (Stream_Image) :
            break;
        case (Stream_Menu) :
            break;
        case (Stream_Max) :
            break;
    }
}

} //NameSpace

#endif //MEDIAINFO_DTS_YES

