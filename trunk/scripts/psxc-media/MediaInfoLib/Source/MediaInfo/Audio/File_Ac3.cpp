// File_Ac3 - Info for AC3 files
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

//---------------------------------------------------------------------------
// Compilation condition
#include <MediaInfo/Setup.h>
#if defined(MEDIAINFO_AC3_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Audio/File_Ac3.h"
#include <ZenLib/Utils.h>
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// AC3 - Format
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  StartCode                        2 bytes - 0x0B77
//  CRC                              2 bytes
//  Sampling rate                    2 bits
//  Bit rate / word per syncframe    6 bits
//  BitStream ID                     5 bits - 01000
//  BitStream Mode (FX, voice...)    3 bits
//  Channels                         3 bits
//  (option) Central amplification   2 bits - if Channels & 001 && Channels!=1
//  (option) Back amplification      2 bits - if Channels & 100
//  (option) Dolby                   2 bits - if Channels == 2
//  Sub woofer present               1 bit
//  ...
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
const size_t Ac3_Errors_MaxCount=10;

const size_t AC3_SamplingRate[]=
{ 48000,  44100,  32000,      0,};
const size_t AC3_BitRate_Size=19;
const size_t AC3_BitRate[]=
{  32000,   40000,   48000,   56000,   64000,   80000,   96000,  112000,
  128000,  160000,  192000,  224000,  256000,  320000,  384000,  448000,
  512000,  576000,  640000};
const size_t AC3_Channels[]=
{2, 1, 2, 3, 3, 4, 4, 5};
const Char*  AC3_ChannelPositions[]=
{
    _T("L R"),
    _T("C"),
    _T("L R"),
    _T("L C R"),
    _T("Front: L R,   Rear: C"),
    _T("Front: L C R, Rear: C"),
    _T("Front: L R,   Rear: L R"),
    _T("Front: L C R, Rear: L R")
};

//---------------------------------------------------------------------------

//***************************************************************************
// Format
//***************************************************************************

//---------------------------------------------------------------------------
void File_Ac3::Read_Buffer_Init()
{
    Delay=0;
    Element_Size_Save=0;
    Trusted=Ac3_Errors_MaxCount;
}

//---------------------------------------------------------------------------
void File_Ac3::Read_Buffer_Continue()
{
    if (Buffer_Size<=4)
        return;
    if (Count_Get(Stream_Audio)>0)
    {
        File_Offset=File_Size;
        return; //Already done!
    }

    //Synchronizing
    while (Buffer_Offset<=Buffer_Size-4 && !(CC2(Buffer+Buffer_Offset)==0x0B77))
        Buffer_Offset++;
    Delay+=Buffer_Offset;
    if (Buffer_Offset>Buffer_Size-4)
    {
        if (!CompleteFileName.empty())
            File_Offset=File_Size;
        return;
    }

    //Coherancy test : for example, to not find AC3 in a file with some "0B77" block
    size_t Offset_Save=Buffer_Offset;
    while(Buffer_Parse());
    if (File_Offset==File_Size)
        return;
    Buffer_Offset=Offset_Save;

    //Processing
    int32u I0=BigEndian2int32u((char*)Buffer+Buffer_Offset+4);
    size_t SamplingRate=I0>>30;
    size_t BitRate=(I0&0x3E000000)>>25;
    if (BitRate>=AC3_BitRate_Size)
    {
        File_Offset=File_Size;
        return;
    }
    size_t Channels=(I0&0x0000E000)>>13;

    size_t Decalage=0; bool Dolby=false;
    if (Channels&0x01 && Channels!=0x01) Decalage+=2;
    if (Channels&0x04) Decalage+=2;
    if (Channels==0x02) // Dolby Digital?
    {
        size_t DD=(I0>>(12-Decalage))&0x03;
        if (DD==0x02)
            Dolby=true;
        Decalage+=2;
    }
    int SubWoofer=(I0>>(12-Decalage))&0x01;

    //Filling
    Stream_Prepare(Stream_General);
    General[0](_T("Format"))=_T("AC3");

    Stream_Prepare(Stream_Audio);
    Audio[0](_T("Codec"))=_T("AC3");
    Audio[0](_T("SamplingRate")).From_Number(AC3_SamplingRate[SamplingRate]);
    Audio[0](_T("BitRate")).From_Number(AC3_BitRate[BitRate]);
    if (Delay>100 && CompleteFileName.empty())
        Audio[0](_T("Delay")).From_Number((float)Delay*1000/(AC3_BitRate[BitRate]/8), 0);
    Audio[0](_T("Channel(s)")).From_Number(AC3_Channels[Channels]);
    if (Channels==0)
        Audio[0](_T("Codec_Profile"))=_T("Dual Mono");
    Audio[0](_T("ChannelPositions"))=AC3_ChannelPositions[Channels];
    if (SubWoofer)
    {
        Audio[0](_T("Channel(s)")).From_Number(AC3_Channels[Channels]+1);
        Audio[0](_T("ChannelPositions"))+=_T(", Subwoofer");
    }
    if (Dolby)
        Audio[0](_T("ChannelPositions"))+=_T(" (Dolby Digital)");
    Audio[0](_T("BitRate_Mode"))=_T("CBR");

    //No more need data
    File_Offset=File_Size;
}

//---------------------------------------------------------------------------
bool File_Ac3::Buffer_Parse()
{
    //Element size
    if (!Element_Size_Get())
        return false;

    //Next frame
    Buffer_Offset=Element_Next;
    return true;
}

//---------------------------------------------------------------------------
bool File_Ac3::Element_Size_Get()
{
    //Searching Element_Next
    Element_Next=Buffer_Offset+2;
    while(Element_Next+2<=Buffer_Size && CC2(Buffer+Element_Next)!=0x0B77)
        Element_Next++;
    if (Element_Next+2>Buffer_Size)
        return false;

    //Filling
    Element_Size=Element_Next-Buffer_Offset;

    //Testing
    if (Element_Size_Save==0)
        Element_Size_Save=Element_Size;
    if (Element_Size!=Element_Size_Save) //Not normal
    {
        TRUSTED_ISNOT("Should be constant Element size");
        if (File_Offset==File_Size)
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------
void File_Ac3::HowTo(stream_t StreamKind)
{
    switch (StreamKind)
    {
        case (Stream_General) :
            Fill_HowTo("Format", "R");
            Fill_HowTo("OveralBitRate", "R");
            Fill_HowTo("PlayTime", "R");
            break;
        case (Stream_Video) :
            break;
        case (Stream_Audio) :
            Fill_HowTo("BitRate", "R");
            Fill_HowTo("BitRate_Mode", "R");
            Fill_HowTo("Channel(s)", "R");
            Fill_HowTo("ChannelPositions", "R");
            Fill_HowTo("SamplingRate", "R");
            Fill_HowTo("Codec", "R");
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

#endif //MEDIAINFO_AC3_YES

