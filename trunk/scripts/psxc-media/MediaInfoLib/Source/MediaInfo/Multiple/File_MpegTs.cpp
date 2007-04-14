// File_Mpegts - Info for MPEG Transport Stream files
// Copyright (C) 2006-2007 Jerome Martinez, Zen@MediaArea.net
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
#if defined(MEDIAINFO_MPEGTS_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Multiple/File_MpegTs.h"
#include "MediaInfo/Multiple/File_MpegPs.h"
#include "ZenLib/BitStream.h"
#include "ZenLib/Utils.h"
#include "wx/file.h"
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Constants
//***************************************************************************

//---------------------------------------------------------------------------
char* MpegTs_ProgramAssociationTable_TableID(int32u ID)
{
    switch (ID)
    {
        case 0x00 : return "program_association_section";
        case 0x01 : return "conditional_access_section";
        case 0x02 : return "TS_program_map_section";
        case 0x03 : return "TS_description_section";
        case 0x04 : return "ISO_IEC_14496_scene_description_section";
        case 0x05 : return "ISO_IEC_14496_object_descriptor_section";
        default :
            if (ID>=0x06
             && ID<=0x37) return "ITU-T Rec. H.222.0 | ISO/IEC 13818-1 reserved";
            if (ID>=0x38
             && ID<=0x3F) return "Defined in ISO/IEC 13818-6";
            if (ID>=0x40
             && ID<=0xFE) return "User private";
            if (ID==0xFF) return "forbidden";
            return "forbidden";
    }
}

//---------------------------------------------------------------------------
char* MpegTs_TS_program_map_section_StreamID(int32u ID)
{
    switch (ID)
    {
        case 0x00 : return "ITU-T | ISO/IEC Reserved";
        case 0x01 : return "ISO/IEC 11172 Video";
        case 0x02 : return "ITU-T Rec. H.262 | ISO/IEC 13818-2 Video or ISO/IEC 11172-2 constrained parameter video stream";
        case 0x03 : return "ISO/IEC 11172 Audio";
        case 0x04 : return "ISO/IEC 13818-3 Audio";
        case 0x05 : return "ITU-T Rec. H.222.0 | ISO/IEC 13818-1 private_sections";
        case 0x06 : return "ITU-T Rec. H.222.0 | ISO/IEC 13818-1 PES packets containing private data";
        case 0x07 : return "ISO/IEC 13522 MHEG";
        case 0x08 : return "ITU-T Rec. H.222.0 | ISO/IEC 13818-1 Annex A DSM-CC";
        case 0x09 : return "ITU-T Rec. H.222.1";
        case 0x0A : return "ISO/IEC 13818-6 type A";
        case 0x0B : return "ISO/IEC 13818-6 type B";
        case 0x0C : return "ISO/IEC 13818-6 type C";
        case 0x0D : return "ISO/IEC 13818-6 type D";
        case 0x0E : return "ITU-T Rec. H.222.0 | ISO/IEC 13818-1 auxiliary";
        case 0x0F : return "ISO/IEC 13818-7 Audio with ADTS transport syntax";
        case 0x10 : return "ISO/IEC 14496-2 Visual";
        case 0x11 : return "ISO/IEC 14496-3 Audio with the LATM transport syntax as defined in ISO/IEC 14496-3 / AMD 1";
        case 0x12 : return "ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in PES packets";
        case 0x13 : return "ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in ISO/IEC14496_sections.";
        case 0x14 : return "ISO/IEC 13818-6 Synchronized Download Protocol";
        case 0x1B : return "H.264";
        default :
            if (ID>=0x15
             && ID<=0x7F) return "ITU-T Rec. H.222.0 | ISO/IEC 13818-1 reserved";
            if (ID>=0x80
             && ID<=0xFF) return "User Private";
            return "forbidden";
    }
}

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
File_MpegTs::File_MpegTs()
{
    TS_Packet_Count=0;
}

//---------------------------------------------------------------------------
File_MpegTs::~File_MpegTs()
{
    for (size_t Pos=0; Pos<TS_Handle.size(); Pos++)
        delete TS_Handle[Pos];
}

//***************************************************************************
// Base
//***************************************************************************

//---------------------------------------------------------------------------
void File_MpegTs::Read_Buffer_Init()
{
    Buffer_MinimumSize=32768*4;
    Streams_Count=Error;
    PES_Needed.resize(0x2000); //13 bits
    PES_Needed[0x0000]=true; //Program
    TS_Needed.resize(0x2000); //13 bits
    TS_Needed[0x0000]=true; //Program
    TimeStamp_Start.resize(0x2000); //13 bits
    TimeStamp_End.resize(0x2000); //13 bits
    Synched=false;
    Network_PID=0;
}

//---------------------------------------------------------------------------
void File_MpegTs::Read_Buffer_Unsynched()
{
    if (Config.Details_Get())
    {
        Details_Add_Error("MPEG-TS, Will resync...");
    }

    Synched=false;
    Streams_Count=Error;
    PES_Needed.clear();
    PES_Needed.resize(0x2000); //13 bits
    for (size_t Pos=0; Pos<TS_UsefulPID.size(); Pos++)
        PES_Needed[TS_UsefulPID[Pos]]=true;
    TS_Needed.clear();
    TS_Needed.resize(0x2000); //13 bits
    for (size_t Pos=0; Pos<TS_UsefulPID.size(); Pos++)
        TS_Needed[TS_UsefulPID[Pos]]=true;
}

//---------------------------------------------------------------------------
void File_MpegTs::Read_Buffer_Continue()
{
    //Look for first Sync word
    if (!Synched && !Synchronize())
        return;

    //Fill buffers with demuxed streams
    while (Buffer_Parse());
}

//---------------------------------------------------------------------------
void File_MpegTs::Read_Buffer_Finalize()
{
    //Streams
    for (size_t Element_Name=0; Element_Name<TS_Handle.size(); Element_Name++)
        if (TS_Handle[Element_Name])
        {
            TS_Handle[Element_Name]->Open_Buffer_Finalize();
            Merge (*TS_Handle[Element_Name]);
            Fill("ID", Element_Name, 16);
            //TimeStamp
            int64s PlayTime=TimeStamp_End[Element_Name]-TimeStamp_Start[Element_Name];
            if (PlayTime)
            {
                Fill("PlayTime", "");
                Fill("PlayTime", PlayTime/90);
            }
        }

    //Fill General
    if (General.empty())
        Stream_Prepare(Stream_General);
         if (!Video.empty() && Video[0](_T("Codec"))==_T("MPEG-2V"))
        Fill(Stream_General, 0, "Format", "MPEG-2TS");
    else if (!Video.empty() && Video[0](_T("Codec"))==_T("MPEG-4V"))
        Fill(Stream_General, 0, "Format", "MPEG-4TS");
    else if (!Video.empty() && Video[0](_T("Codec"))==_T("AVC"))
        Fill(Stream_General, 0, "Format", "MPEG-4TS");
    else
        Fill(Stream_General, 0, "Format", "MPEG-1TS");

    return;
}

//***************************************************************************
// Buffer
//***************************************************************************

#define NOT_NEEDED(_TEST) \
    if (_TEST) \
    { \
        if (Config.Details_Get()) \
        { \
            Details_Add_Element_Purge(); \
        } \
        return; \
    } \

#define NOT_NEEDED_BOOL(_TEST) \
    if (_TEST) \
    { \
        if (Config.Details_Get()) \
        { \
            Details_Add_Element_Purge(); \
        } \
        return true; \
    } \

#define DETAIL_ADD(_VALUE) \
    Details_Add_Info(_VALUE); \

#define BEGIN \
    BitStream BS(Buffer+Buffer_Offset, Element_Size); \

#define END \
    { \
        BS.Byte_Align(); \
        INTEGRITY(BS.Offset_Get()) \
    } \

#define GET(_BITS, _INFO, _NAME) \
    { \
        int32u _NAME=BS.Get(_BITS); \
        if (Config.Details_Get()) \
        { \
            Details_Add_Info(Error, #_NAME, _NAME); \
        } \
        _INFO=_NAME; \
    }

#define SKIP(_BITS, _NAME) \
    { \
        if (Config.Details_Get()) \
        { \
            Details_Add_Info(Error, #_NAME, BS.Get(_BITS)); \
        } \
        else \
            BS.Skip(_BITS); \
    } \

#define MARK(_BITS, _NAME) \
    { \
        int32u Temp=BS.Get(_BITS); \
        if (Config.Details_Get()) \
        { \
            Details_Add_Info(Error, #_NAME, Temp); \
        } \
        if (Temp==0) \
        { \
            TRUSTED_ISNOT("Marker bit is wrong"); \
            return; \
        } \
    }

#define ZERO(_BITS, _NAME) \
    { \
        int32u Temp=BS.Get(_BITS); \
        if (Config.Details_Get()) \
        { \
            Details_Add_Info(Error, #_NAME, Temp); \
        } \
        if (Temp!=0) \
        { \
            TRUSTED_ISNOT("Marker bit is wrong"); \
            return; \
        } \
    }

#define GET_E(_BITS, _INFO, _NAME) \
    { \
        int32u _NAME=BS.Get(_BITS); \
        if (Config.Details_Get()) \
        { \
            Details_Add_Element_Info(Error, #_NAME, _NAME); \
        } \
        _INFO=_NAME; \
    }

#define SKIP_E(_BITS, _NAME) \
    { \
        if (Config.Details_Get()) \
        { \
            Details_Add_Element_Info(Error, #_NAME, BS.Get(_BITS)); \
        } \
        else \
            BS.Skip(_BITS); \
    } \

#define INTEGRITY(ELEMENT_SIZE) \
    if (0 && Element_Size!=ELEMENT_SIZE) \
    { \
        TRUSTED_ISNOT("Size is wrong"); \
        return; \
    } \

//---------------------------------------------------------------------------
bool File_MpegTs::Buffer_Parse()
{
    //Element size
    Element_Next=Buffer_Offset+188;

    //Parsing
    if (!Frame_Parse())
        return false;

    //Jumping to the end of the file if needed
    if (Detect_EOF_Needed())
        return false;

    //If no need of more
    if (File_Offset==File_Size || File_GoTo>0)
        return false;

    Buffer_Offset=Element_Next;
    return true;
}

//---------------------------------------------------------------------------
bool File_MpegTs::Frame_Parse()
{
    //Enough data?
    if (Buffer_Offset+188>Buffer_Size)
        return false;

    //Header
    Element_Size=188;

    //Parsing
    int32u Adaptation, Scrambling;
    BEGIN
    SKIP_E( 8,                                                  sync_byte)
    SKIP_E( 1,                                                  transport_error_indicator)
    GET_E ( 1, PayLoad_Start,                                   payload_unit_start_indicator)
    SKIP_E( 1,                                                  transport_priority)
    GET_E (13, Element_Name,                                    pid)
    if (!TS_Needed[Element_Name])
    {
        //We don't need this element (TS level)
        PURGE();
        return true;
    }
    GET_E ( 2, Scrambling,                                      transport_scrambling_control)
    if (Scrambling>0)
    {
        //Encrypted
        PURGE();
        Frame_Parse_Scrambled();
        Buffer_Offset+=188;
        return true;
    }
    GET_E ( 2, Adaptation,                                      adaptation_field_control)
    SKIP_E( 4,                                                  continuity_counter)

    //Adaptation
    if (Adaptation&0x2)
        AdaptationField();
    else
        Element_HeaderSize=4;

    //Needed?
    if (!PES_Needed[Element_Name])
    {
        //We don't need this element (PES level)
        PURGE();
        Buffer_Offset+=188;
        return true;
    }

    //Parsing
    if (Adaptation&0x1)
        Element_Parse();
    else
    {
        FLUSH();
        Buffer_Offset+=188;
    }

    return true;
}

//---------------------------------------------------------------------------
bool File_MpegTs::Element_Parse()
{
    if (Config.Details_Get())
    {
        Details_Add_Element(0, Element_Name);
    }

    Buffer_Offset+=Element_HeaderSize;
    Element_Size=188-Element_HeaderSize;
    TS_Packet_Count++;

    //PIDs
    if (Element_HeaderSize<188)
    {
        if (Buffer_Offset+Element_Size>Buffer_Size) ;
        else if (Element_Name==0x0000) ProgramAssociationTable();
        else if (Element_Name==0x0001) ConditionalAccessTable();
        //else if (Element_Name==ProgramMap_PID) TS_Program_Map_Section();
        else if (Element_Name==Network_PID) TS_Network_Section();
        else if (Element_Name>=0x0002
              && Element_Name<=0x000F) Reserved();
        else if (Element_Name==0x1FFF) Null();
        else
        {
            bool IsProgramMap=false;
            for (size_t Pos=0; Pos<ProgramMap_PID.size(); Pos++)
                if (Element_Name==ProgramMap_PID[Pos])
                    IsProgramMap=true;
            if (IsProgramMap)
                TS_Program_Map_Section();
            else
                PES();
        }
    }
    FLUSH();

    return true;
}

//***************************************************************************
// Helpers
//***************************************************************************

//---------------------------------------------------------------------------
bool File_MpegTs::Synchronize()
{
    while (           Buffer_Offset+188*4<Buffer_Size
      && !(CC1(Buffer+Buffer_Offset+188*0)==0x47
        && CC1(Buffer+Buffer_Offset+188*1)==0x47
        && CC1(Buffer+Buffer_Offset+188*2)==0x47
        && CC1(Buffer+Buffer_Offset+188*3)==0x47))
        Buffer_Offset++;
    if (Buffer_Offset+188*4>=Buffer_Size)
    {
        if (Synched)
        {
            //Sync lost
            if (Config.Details_Get())
            {
                Details_Add_Error("-------------------------------------------");
                Details_Add_Error("---   MPEG-TS, Synchronisation lost     ---");
                Details_Add_Error("-------------------------------------------");
            }
            Synched=false;
        }

        //Managing first Synch attempt
        if (Count_Get(Stream_General)==0)
            File_Offset=File_Size; //End

        return false;
    }

    //There is no start code, so Stream_General is filled here
    if (Count_Get(Stream_General)==0)
        Stream_Prepare(Stream_General);

    Synched=true;
    return true;
}

//---------------------------------------------------------------------------
bool File_MpegTs::Detect_EOF_Needed()
{
    //Jump to the end of the file
    if (File_Size>=File_Offset+Buffer_Size+1*1024*1024 && File_Size!=(int64u)-1 && (
       (File_Offset>=2*1024*1024)
    || (Streams_Count==0)
    ))
    {
        //Details
        if (Config.Details_Get())
        {
            Details_Add_Error("-------------------------------------------");
            Details_Add_Error("---   MPEG-TS, Jumping to end of file   ---");
            Details_Add_Error("-------------------------------------------");
        }

        File_GoTo=File_Size-1*1024*1024;
        return true;
    }

    return false;
}

//---------------------------------------------------------------------------
//
void File_MpegTs::AdaptationField()
{
    //Go where Adapation field is
    Buffer_Offset+=4;
    Element_Size=188-4;

    int32u Adaptation_Size, PCR, OPCR, Splicing, Transport, Adapatation_Extension;
    BEGIN
    GET_E ( 8, Adaptation_Size,                                 adaptation_field_length)
    if (Adaptation_Size>0)
    {
        SKIP_E( 1,                                              discontinuity_indicator)
        SKIP_E( 1,                                              random_access_indicator)
        SKIP_E( 1,                                              elementary_stream_priority_indicator)
        GET_E ( 1, PCR,                                         PCR_flag)
        GET_E ( 1, OPCR,                                        OPCR_flag)
        GET_E ( 1, Splicing,                                    splicing_point_flag)
        GET_E ( 1, Transport,                                   transport_private_data_flag)
        GET_E ( 1, Adapatation_Extension,                       adaptation_field_extension_flag)
        if (PCR)
        {
            SKIP_E( 1,                                          program_clock_reference_base)
            int32u TimeStamp;
            GET_E (32, TimeStamp,                               program_clock_reference_base)
            if (TimeStamp_Start[Element_Name])
                TimeStamp_End[Element_Name]=TimeStamp;
            else
                TimeStamp_Start[Element_Name]=TimeStamp;
            SKIP_E( 6,                                          reserved)
            SKIP_E( 9,                                          program_clock_reference_extension)
        }
        if (OPCR)
        {
            SKIP_E( 1,                                          original_program_clock_reference_base)
            SKIP_E(32,                                          original_program_clock_reference_base)
            SKIP_E( 6,                                          reserved)
            SKIP_E( 9,                                          original_program_clock_reference_extension)
        }
        if (Splicing)
        {
            SKIP_E(8,                                           splice_countdown)
        }
        if (Transport)
        {
            int32u Transport_Size;
            GET_E ( 8, Transport_Size,                          transport_private_data_length)
            for (int32u Pos=0; Pos<=Transport_Size; Pos++)
                BS.Skip(8);
        }
        if (Adapatation_Extension)
        {
            int32u Adapatation_Extension_Size;
            GET_E ( 8, Adapatation_Extension_Size,              adaptation_field_extension_length)
            for (int32u Pos=0; Pos<=Adapatation_Extension_Size; Pos++)
                BS.Skip(8);
        }
    }

    //Jump after the adaptation field
    Buffer_Offset-=4;
    Element_HeaderSize=4+1+Adaptation_Size;
}

//---------------------------------------------------------------------------
void File_MpegTs::Frame_Parse_Scrambled()
{
    NOT_NEEDED(!PES_Needed[Element_Name])

    switch (TS_UsefulPID_StreamID[Element_Name])
    {
        case 0x01 : Stream_Prepare(Stream_Video); Fill("Codec", "MPEG-1V"); break;
        case 0x02 : Stream_Prepare(Stream_Video); Fill("Codec", "MPEG-2V"); break;
        case 0x03 : Stream_Prepare(Stream_Audio); Fill("Codec", "MPEG-1A"); break;
        case 0x04 : Stream_Prepare(Stream_Audio); Fill("Codec", "MPEG-2A"); break;
        case 0x0F : Stream_Prepare(Stream_Audio); Fill("Codec", "AAC"); break;
        case 0x10 : Stream_Prepare(Stream_Video); Fill("Codec", "MPEG-4V"); break;
        case 0x11 : Stream_Prepare(Stream_Audio); Fill("Codec", "AAC"); break;
        case 0x1B : Stream_Prepare(Stream_Video); Fill("Codec", "H264"); break;
        default :
            if (TS_UsefulPID_StreamID[Element_Name]>=0x80
             && TS_UsefulPID_StreamID[Element_Name]<=0xFF) Stream_Prepare(Stream_Audio); break;
    }

    Fill("Encryption", "Encrypted");
    PES_Needed[Element_Name]=false;
}

//***************************************************************************
// Elements
//***************************************************************************

#define NAME(ELEMENT_NAME) \
    if (Config.Details_Get()) \
    { \
        Details_Add_Element(ELEMENT_NAME); \
    } \

//---------------------------------------------------------------------------
void File_MpegTs::ProgramAssociationTable()
{
    NAME("Program Association Table")
    int32u Jump, ID;
    BEGIN
    if (PayLoad_Start)
        GET ( 8, Jump,                                          pointer_field)
    GET ( 8, ID,                                                table_id) DETAIL_ADD(MpegTs_ProgramAssociationTable_TableID(ID))

    Buffer_Offset+=2;
    Element_Size-=2;

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Element(1, (int8u)ID, Element_Size);
    }

    if (ID==0) ProgramAssociationTable_program_association_section();
    Details_Level_Last--;
}

//---------------------------------------------------------------------------
void File_MpegTs::ProgramAssociationTable_program_association_section()
{
    NAME("Program Association Section")
    int32u PAS_Size;
    BEGIN
    MARK( 1,                                                    section_syntax_indicator)
    ZERO( 1,                                                    zero)
    SKIP( 2,                                                    reserved)
    GET (12, PAS_Size,                                          section_length)
    SKIP(16,                                                    transport_stream_id)
    SKIP( 2,                                                    reserved)
    SKIP( 5,                                                    version_number)
    SKIP( 1,                                                    current_next_indicator)
    SKIP( 8,                                                    section_number)
    SKIP( 8,                                                    last_section_number)
    if (PAS_Size<5) return;

    int32u ProgNumber;
    for (int32u Pos=0; Pos<(PAS_Size-5-4)/3; Pos++)
    {
        GET (16, ProgNumber,                                    program_number)
        SKIP( 3,                                                reserved)
        if (ProgNumber==0)
        {
            GET ( 13, Network_PID,                              network_PID)
        }
        else
        {
            int32u ProgramMap_PID_Temp;
            GET ( 13, ProgramMap_PID_Temp,                      program_map_PID)
            TS_Needed[ProgramMap_PID_Temp]=true;
            PES_Needed[ProgramMap_PID_Temp]=true;
            ProgramMap_PID.push_back(ProgramMap_PID_Temp);
        }
    }
    SKIP(32,                                                    CRC_32)
    TS_Needed[0]=false;
}

//---------------------------------------------------------------------------
void File_MpegTs::TS_Program_Map_Section()
{
    NAME("TS Program Map Section")

    int32u Jump, PMS_Size, PSI_Size;
    BEGIN
    if (PayLoad_Start)
        GET ( 8, Jump,                                          pointer_field)
    SKIP( 8,                                                    table_id)
    SKIP( 1,                                                    section_syntax_indicator)
    SKIP( 1,                                                    zero)
    SKIP( 2,                                                    reserved)
    GET (12, PMS_Size,                                          section_length)
    if (PMS_Size<13)
        return;
    SKIP(16,                                                    transport_stream_id)
    SKIP( 2,                                                    reserved)
    SKIP( 5,                                                    version_number)
    SKIP( 1,                                                    current_next_indicator)
    SKIP( 8,                                                    section_number)
    SKIP( 8,                                                    last_section_number)
    SKIP( 3,                                                    reserved)
    SKIP(13,                                                    PCR_PID)
    SKIP( 4,                                                    reserved)
    GET (12, PSI_Size,                                          program_info_length)
    for (int32u Descriptor_Pos=0; Descriptor_Pos<PSI_Size; Descriptor_Pos++)
    {
        //Descriptor
        //SKIP( 8,                                                descriptor)
        BS.Skip(8);
    }
    int32u ES_Size, EPID, StreamID;
    size_t Streams_Count_Temp=0;
    int32u Pos=9+PSI_Size;
    while (Pos<PMS_Size-4)
    {
        GET ( 8, StreamID,                                      stream_type) DETAIL_ADD(MpegTs_TS_program_map_section_StreamID(StreamID))
        SKIP( 3,                                                reserved)
        GET (13, EPID,                                          elementary_PID)
        SKIP( 4,                                                reserved)
        GET (12, ES_Size,                                       ES_info_length)
        for (int32u Descriptor_Pos=0; Descriptor_Pos<ES_Size; Descriptor_Pos++)
        {
            //Descriptor
            //SKIP( 8,                                            descriptor)
            BS.Skip(8);
        }

        //Enable what we know parsing
        if (StreamID==0x01
         || StreamID==0x02
         || StreamID==0x03
         || StreamID==0x04
         || StreamID==0x05
         || StreamID==0x06
         || StreamID==0x10
         || StreamID==0x1B
         || (StreamID>=0x80 && StreamID<=0xFF)
        )
        {
            TS_Needed[EPID]=true;
            TS_UsefulPID.push_back(EPID);
            TS_UsefulPID_StreamID[EPID]=StreamID;
            PES_Needed[EPID]=true;
        }

        Pos+=5+ES_Size;
        Streams_Count_Temp++;
    }
    SKIP(32,                                                    CRC_32)
    END

    //Filling
    if (Streams_Count==Error)
        Streams_Count=0;
    Streams_Count+=Streams_Count_Temp;
    TS_Needed[Element_Name]=false;
}

//---------------------------------------------------------------------------
void File_MpegTs::TS_Network_Section()
{
    NAME("Network Information Table")
}

//---------------------------------------------------------------------------
void File_MpegTs::ConditionalAccessTable()
{
    NAME("Conditional Access Table")
}

//---------------------------------------------------------------------------
void File_MpegTs::Reserved()
{
    NAME("Reserved")
}

//---------------------------------------------------------------------------
void File_MpegTs::Null()
{
    NOT_NEEDED(1)
    NAME("Null")
}

//---------------------------------------------------------------------------
void File_MpegTs::PES()
{
    NAME("Packet Stream")

    //Enough place?
    if (Element_Name>=TS_Handle.size())
        TS_Handle.resize(Element_Name+1);

    //Is it the start of a PES?
    NOT_NEEDED(TS_Handle[Element_Name]==NULL && !PayLoad_Start)

    //Allocating an handle if needed
    if (TS_Handle[Element_Name]==NULL)
        TS_Handle[Element_Name]=new File_MpegPs;

    //Parsing is needed?
    NOT_NEEDED(!PES_Needed[Element_Name])

    //Open MPEG-PS (PES)
    if (TS_Handle[Element_Name] && TS_Handle[Element_Name]->File_GoTo<File_Offset+Buffer_Offset && TS_Handle[Element_Name]->File_Offset!=TS_Handle[Element_Name]->File_Size)
    {
        Open_Buffer_Init(TS_Handle[Element_Name], File_Size, File_Offset+Buffer_Offset);
        TS_Handle[Element_Name]->FromTS=TS_UsefulPID_StreamID[Element_Name];
        TS_Handle[Element_Name]->MPEG_Version=2;
        Open_Buffer_Continue(TS_Handle[Element_Name], Buffer+Buffer_Offset, Element_Size);

        //Need anymore?
        if (TS_Handle[Element_Name]->File_GoTo!=0 || TS_Handle[Element_Name]->File_Offset==TS_Handle[Element_Name]->File_Size)
        {
            PES_Needed[Element_Name]=false;
            Streams_Count--;
        }
    }
}

//---------------------------------------------------------------------------
void File_MpegTs::HowTo(stream_t StreamKind)
{
    switch (StreamKind)
    {
        case (Stream_General) :
            Fill_HowTo("Format", "R");
            break;
        case (Stream_Video) :
            break;
        case (Stream_Audio) :
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

#endif //MEDIAINFO_MPEGTS_YES


