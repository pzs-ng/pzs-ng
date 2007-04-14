// File_Aac - Info for AAC files
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
// Compilation condition
#include <MediaInfo/Setup.h>
#if defined(MEDIAINFO_AAC_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Audio/File_Aac.h"
#if defined(MEDIAINFO_ID3_YES) || (!defined(MEDIAINFO_AUDIO_NO) && !defined(MEDIAINFO_ID3_NO))
    #include "MediaInfo/Audio/File_Id3.h"
#endif
#include "ZenLib/Utils.h"
#include "ZenLib/BitStream.h"
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//---------------------------------------------------------------------------
const size_t ADTS_SamplingRate[]=
{96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050,
 16000, 12000, 11025,  8000, 7350, Error, Error, Error,};

const char* ADTS_Profile[]=
{
    "A_AAC/MPEG4/MAIN",
    "A_AAC/MPEG4/LC",
    "A_AAC/MPEG4/SSR",
    "A_AAC/MPEG4/LTP",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
};

const char* MP4_Profile[]=
{
    "Reserved",
    "A_AAC/MPEG4/MAIN",
    "A_AAC/MPEG4/LC",
    "A_AAC/MPEG4/SSR",
    "A_AAC/MPEG4/LTP",
    "AAC SBR",
    "AAC Scalable",
    "TwinVQ",
    "CELP",
    "HVXC",
    "Reserved",
    "Reserved"
    "TTSI",
    "Main synthetic",
    "Wavetable synthesis",
    "General MIDI",
    "Algorithmic Synthesis and Audio FX",
    "ER AAC LC",
    "Reserved",
    "ER AAC LTP",
    "ER AAC scalable",
    "ER TwinVQ",
    "ER BSAC",
    "ER AAC LD",
    "ER CELP",
    "ER HVXC",
    "ER HILN",
    "ER Parametric",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
};
//---------------------------------------------------------------------------

//***************************************************************************
// Format
//***************************************************************************

//---------------------------------------------------------------------------
void File_Aac::Read_Buffer_Continue ()
{
    //Tags
    if (File_Offset==0)
    {
        if (!Skip_ID3v2())
            return;
    }

    //Parsing headers
    if (Buffer_Size<=8)                                  ES();   //AAC in ES (.MP4)
    else if (CC4(Buffer)==CC4("ADIF"))                   ADIF(); //ADIF header
    else if ((CC2(Buffer+Buffer_Offset)&0xFFF6)==0xFFF0) ADTS(); //ADTS header (12 bits + ID + 2 bits)
    else if (CC3(Buffer)==CC3("AAC"))                    AAC();  //AAC header

    //No need of more
    File_Offset=File_Size;

    //Tags
    #if defined(MEDIAINFO_ID3_YES)
        /*
        File_Id3 Id3;
        if (Id3.Open(CompleteFileName)<0)
            Id3.Open(Buffer, Id3v2_Size);
        Merge(Id3);
        */
    #endif //ID3
}

//***************************************************************************
// Elements
//***************************************************************************

//---------------------------------------------------------------------------
//
void File_Aac::AAC ()
{
}

//---------------------------------------------------------------------------
// ADIF header
// "ADIF"                           32 bits
// Copyright_Present                1 bit
// Copyright_ID                     70 bits (only if Copyright_Present=true)
// Original                         1 bit
// Home                             1 bit
// Home                             1 bit
// BitStream_Type                   1 bit (0=CBR, 1=VBR)
// BitRate                          23 bits (For CBR: bitrate, for VBR: peak bitrate, 0 means unknown)
// num_program_config_elements      4 bits (the next 2 fields come num_program_config_elements+1 times)
// buffer_fullness                  20 bits (only if bitstream_type==0)
// program_config_element           VAR
// ...
//
void File_Aac::ADIF ()
{
    //Parsing
    BitStream BS(Buffer+Buffer_Offset, Buffer_Size-Buffer_Offset);
    BS.Skip(32); //ID: "ADIF"
    size_t Copyright_Present=(bool)BS.Get(1);       //Copyright_Present
    if (Copyright_Present)
        {BS.Skip(32); BS.Skip(32); BS.Skip(8);}     //Copyright_ID
    BS.Skip(1); //Original
    BS.Skip(1); //Home
    size_t BitStream_Type=BS.Get(1);                //BitStream_Type
    size_t BitRate=BS.Get(23); //

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Element(0, _T("AAC"));
        Details_Add_Info(0, "Copyright_Present", Copyright_Present);
        Details_Add_Info(0, "BitStream_Type", BitStream_Type);
        Details_Add_Info(0, "BitRate", BitRate);
    }

    //Filling
    Stream_Prepare(Stream_General);
    Fill ("Format", "AAC");

    Stream_Prepare(Stream_Audio);
    Fill ("Codec", "AAC");
    if (BitStream_Type==0)
        Fill("BitRate_Mode", "CBR");
    else
        Fill("BitRate_Mode", "VBR");
    if (BitRate>0)
    {
        if (BitStream_Type==0) //CBR
            Fill("BitRate", BitRate);
        else //VBR
            Fill("BitRate_Max", BitRate);
    }
    Fill("Resolution", 16);
}

//---------------------------------------------------------------------------
// ADIF header, 7 or 9 bytes
// syncword                         12 bits
// id (Mpeg version)                1 bit (0=MPEG-4, 1=MPEG-2)
// layer                            2 bits (should be 0)
// protection_absent                1 bit
// profile                          2 bits
// sampling_frequency_index         4 bits
// private                          1 bit
// channel_configuraton             3 bits
// original                         1 bit
// home                             1 bit
// copyright_id                     1 bit
// copyright_id_start               1 bit
// aac_frame_length                 13 bits
// adts_buffer_fullness             11 bits
// num_raw_data_blocks              2 bits
// CRC_check                        16 bits (if protection_absent==0)
void File_Aac::ADTS ()
{
    //Parsing
    BitStream BS(Buffer+Buffer_Offset, Buffer_Size-Buffer_Offset);
    BS.Skip(12);                                //syncword, 0xFFF
    size_t id=BS.Get(1);                        //id
    BS.Skip(2);                                 //layer
    size_t protection_absent=(bool)BS.Get(1);   //protection_absent
    size_t profile=BS.Get(2);                   //profile
    size_t sampling_frequency_index=BS.Get(4);  //sampling_frequency_index
    BS.Skip(1);                                 //private
    size_t channel_configuraton=BS.Get(3);      //Channels
    BS.Skip(1);                                 //original
    BS.Skip(1);                                 //home
    BS.Skip(1);                                 //copyright_id
    BS.Skip(1);                                 //copyright_id_start
    size_t aac_frame_length=BS.Get(13);         //aac_frame_length
    size_t adts_buffer_fullness=BS.Get(11);     //adts_buffer_fullness
    BS.Skip(2);                                 //num_raw_data_blocks
    if (protection_absent)
        BS.Skip(16);                            //CRC_check

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Element(0, _T("AAC"));
        Details_Add_Info(0, "id", id);
        Details_Add_Info(0, "protection_absent", protection_absent);
        Details_Add_Info(0, "profile", profile);
        Details_Add_Info(0, "sampling_frequency_index", sampling_frequency_index);
        Details_Add_Info(0, "channel_configuraton", channel_configuraton);
        Details_Add_Info(0, "aac_frame_length", aac_frame_length);
        Details_Add_Info(0, "adts_buffer_fullness", adts_buffer_fullness);
    }

    //Filling
    Stream_Prepare(Stream_General);
    Fill("Format", "AAC");

    Stream_Prepare(Stream_Audio);
    Fill("Codec", ADTS_Profile[profile]);
    Fill("SamplingRate", ADTS_SamplingRate[sampling_frequency_index]);
    Fill("Channel(s)", channel_configuraton);
    if (adts_buffer_fullness==0x7FF)
        Fill("BitRate_Mode", "VBR");
    else
        Fill("BitRate_Mode", "CBR");
    Fill("Resolution", 16);

    //size_t FramePerSecond=(float)SamplingRate/1024; //For raw AAC, there are always 1024 samples per frame (but 960 in MP4?)
    //size_t BytesPerFrame=FileSize/(float)(FrameCount*1000);
    //BitRate=8*BytesPerFrame*FramePerSecond+0.5);
}

//---------------------------------------------------------------------------
// AAC in ES, 3+ bytes
// AudioObjectType                  5 bits
// SamplingRateIndex                4 bits
// SamplingRate                     24 bits (only if SamplingRateIndex == 0xF)
// ChannelConfiguration             4 bits
// FrameLengthFlag                  1 bit (0-->1024, 1-->960)
// DependsOnCoreCoder               1 bit (always 0)
// ExtensionFlag                    1 bit (always 0)
// SamplingRateIndex                4 bits  (only if AudioObjectType==5)
// SamplingRate                     24 bits (only if AudioObjectType==5 && SamplingRateIndex==0xF)
// SBR extension (optional)...
//
void File_Aac::ES ()
{
    //Parsing
    BitStream BS(Buffer, Buffer_Size);
    size_t AudioObjectType=BS.Get(5);
    size_t SamplingRate_Index=BS.Get(4);
    size_t SamplingRate;
    if (SamplingRate_Index==0xF)
        SamplingRate=BS.Get(24);
    else
        SamplingRate=ADTS_SamplingRate[SamplingRate_Index];
    size_t ChannelConfiguration=BS.Get(4);
    size_t FrameLengthFlag=BS.Get(1);
    size_t FrameLength;
    if (FrameLengthFlag)
        FrameLength=960;
    else
        FrameLength=1024;
    int32u DependsOnCoreCoder=BS.Get(1);
    int32u ExtensionFlag=BS.Get(1);

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Element(0, _T("AAC"));
        Details_Add_Info(0, "AudioObjectTypeIndex", AudioObjectType);
        Details_Add_Info(0, "SamplingRateIndex", SamplingRate_Index);
        if (SamplingRate_Index==0xF)
            Details_Add_Info(0, "SamplingRate", SamplingRate);
        Details_Add_Info(0, "ChannelConfiguration", ChannelConfiguration);
        Details_Add_Info(0, "FrameLengthFlag", FrameLengthFlag);
        Details_Add_Info(0, "FrameLength", FrameLength);
        Details_Add_Info(0, "DependsOnCoreCoder", DependsOnCoreCoder);
        Details_Add_Info(0, "ExtensionFlag", ExtensionFlag);
    }

    //Parsing SBR stuff with AAC Scalable
    if (AudioObjectType==5)
    {
        size_t SamplingRate_Index=BS.Get(4);
        size_t SamplingRate;
        if (SamplingRate_Index==0xF)
            SamplingRate=BS.Get(24);
        else
            SamplingRate=ADTS_SamplingRate[SamplingRate_Index];

        //Details
        if (Config.Details_Get())
        {
            Details_Add_Info(0, "SamplingRateIndex", SamplingRate_Index);
            if (SamplingRate_Index==0xF)
                Details_Add_Info(0, "SamplingRate", SamplingRate);
        }
    }

    //Filling
    Stream_Prepare(Stream_General);
    Fill("Format", "MPEG-4 AAC");

    Stream_Prepare(Stream_Audio);
    Fill("Codec", MP4_Profile[AudioObjectType]);
    Fill("SamplingRate", SamplingRate);
    if (ChannelConfiguration)
        Fill("Channel(s)", ChannelConfiguration);
    Fill("Resolution", 16);

    //Next element
    Buffer_Offset+=BS.Offset_Get();

    //SBR stuff
    if (Buffer_Size-Buffer_Offset>=3)
    {
        ES_SBR();
    }
}

//---------------------------------------------------------------------------
// AAC in ES, SBR part, 3 bytes
// Sync                             11 bits (0x2B7)
// sbr_object_type                  5 bits
// has_sbr                          1 bit
// SamplingRateIndex                4 bits  (only if has_sbr==0)
// SamplingRate                     24 bits (only if has_sbr==0 && SamplingRateIndex==0xF)
void File_Aac::ES_SBR ()
{
    BitStream BS(Buffer+Buffer_Offset, Buffer_Size-Buffer_Offset);
    size_t Sync=BS.Get(11);
    if (Sync!=0x2B7)
        return;
    size_t ObjectType=BS.Get(5);
    size_t HasSBR=BS.Get(1);
    size_t SamplingRate_Index=0;
    size_t SamplingRate=0;
    if (HasSBR)
    {
        SamplingRate_Index=BS.Get(4);
        if (SamplingRate_Index==0xF)
            SamplingRate=BS.Get(24);
        else
            SamplingRate=ADTS_SamplingRate[SamplingRate_Index];
    }

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(0, "Sync", Sync, 16);
        Details_Add_Info(0, "ObjectType", ObjectType);
        Details_Add_Info(0, "HasSBR", HasSBR);
        if (HasSBR)
        {
            Details_Add_Info(0, "SamplingRateIndex", SamplingRate_Index);
            if (SamplingRate_Index==0xF)
                Details_Add_Info(0, "SamplingRate", SamplingRate);
        }
    }

    //Filling
    if (HasSBR)
    {
        (Stream[StreamKind_Last]->at(StreamPos_Last))(_T("Codec")).append(_T("/SBR"));
        Fill("SamplingRate", ""); //Clear it
        Fill("SamplingRate", ADTS_SamplingRate[SamplingRate_Index]);
    }

    //Next element
    Buffer_Offset+=BS.Offset_Get();

    //SBR-PS stuff
    if (Buffer_Size-Buffer_Offset>=2)
    {
        ES_SBR_PS();
    }
}

//---------------------------------------------------------------------------
// AAC in ES, SBR part, 2 bytes?
// Format is unknown
void File_Aac::ES_SBR_PS ()
{
    //Possible only if 1 Channel
    if (Get(Stream_Audio, 0, _T("Channel(s)"))==_T("1"))
    {
        Fill("Channel(s)", ""); //Clear it
        Fill("Channel(s)", 2);
        (Stream[StreamKind_Last]->at(StreamPos_Last))(_T("Codec")).append(_T("/PS"));
    }

    //Next element
    Buffer_Offset+=2;
}

//***************************************************************************
// Helpers
//***************************************************************************

//---------------------------------------------------------------------------
//
bool File_Aac::Skip_ID3v2 ()
{
    //Integrity
    if (Buffer_Size<10)
        return true; //Not possible to have ID3v2

    //ID3 v2 present?
    if (CC3(Buffer+Buffer_Offset)==CC3("ID3"))
    {
        //ID3v2, must skip it here
        size_t Id3v2_Size=(Buffer[Buffer_Offset+6]<<21) | (Buffer[Buffer_Offset+7]<<14) | (Buffer[Buffer_Offset+8]<<7) | (Buffer[Buffer_Offset+9]<<0)+10;
        if (Id3v2_Size+4>Buffer_Size)
            return false;

        //Skipping it
        Buffer_Offset+=Id3v2_Size;
    }

    return true;
}

//***************************************************************************
// Information
//***************************************************************************

//---------------------------------------------------------------------------
void File_Aac::HowTo(stream_t StreamKind)
{
        if (StreamKind==Stream_General)
    {
        General[0](_T("Format"), Info_HowTo)=_T("R");
        General[0](_T("BitRate"), Info_HowTo)=_T("R");
        General[0](_T("PlayTime"), Info_HowTo)=_T("R");
    }
    else if (StreamKind==Stream_Audio)
    {
        Audio[0](_T("Codec"), Info_HowTo)=_T("R");
        Audio[0](_T("BitRate"), Info_HowTo)=_T("R");
        Audio[0](_T("Channel(s)"), Info_HowTo)=_T("R");
        Audio[0](_T("SamplingRate"), Info_HowTo)=_T("R");
        Audio[0](_T("Resolution"), Info_HowTo)=_T("R");
    }
}

} //NameSpace

#endif //MEDIAINFO_AAC_YES

