// File_MpegPs - Info for MPEG files
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
#if defined(MEDIAINFO_MPEGPS_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Multiple/File_MpegPs.h"
#if defined(MEDIAINFO_MPEGV_YES)
    #include "MediaInfo/Video/File_Mpegv.h"
#endif
#if defined(MEDIAINFO_MPEG4V_YES)
    #include "MediaInfo/Video/File_Mpeg4v.h"
#endif
#if defined(MEDIAINFO_AVC_YES)
    #include "MediaInfo/Video/File_Avc.h"
#endif
#if defined(MEDIAINFO_MPEGA_YES)
    #include "MediaInfo/Audio/File_Mpega.h"
#endif
#if defined(MEDIAINFO_AC3_YES)
    #include "MediaInfo/Audio/File_Ac3.h"
#endif
#if defined(MEDIAINFO_DTS_YES)
    #include "MediaInfo/Audio/File_Dts.h"
#endif
#include <ZenLib/Utils.h>
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Constants
//***************************************************************************

//---------------------------------------------------------------------------
const char* MpegPs_Version[]=
{
    "MPEG-1PS",
    "MPEG-2PS",
    "???",
    "???",
};

//---------------------------------------------------------------------------
extern char* MpegTs_TS_program_map_section_StreamID(int32u ID);

//***************************************************************************
// Format
//***************************************************************************

//---------------------------------------------------------------------------
void File_MpegPs::Read_Buffer_Init()
{
    MPEG_Version=0;
    Video_Unlimited=false;
    Element_Name=0;
    Element_Size=0;
    Element_HeaderSize=0;
    Element_Next=0;
    Buffer_MinimumSize=100;
    TimeStamp.PTS=0;
    TimeStamp.DTS=0;
    FromTS=0; //No info
    Synched=false;
    Video_Count=(int8u)-1;
    Audio_Count=(int8u)-1;
}

//---------------------------------------------------------------------------
void File_MpegPs::Read_Buffer_Unsynched()
{
    if (Config.Details_Get())
    {
        Details_Add_Error("Will resync...");
    }

    Synched=false;
    Video_Unlimited=false;
    Video_Count=(int8u)-1;
    Audio_Count=(int8u)-1;
}

//---------------------------------------------------------------------------
void File_MpegPs::Read_Buffer_Continue()
{
    //Integrity
    if (File_Offset==0 && Detect_NonMPEGPS())
    {
        File_Offset=File_Size;
        return;
    }

    //Video unlimited specific, we didn't wait for the end (because this is... unlimited)
    if (Video_Unlimited)
    {
        Element_Size_Get_Unlimited();
        Element_Parse();
        Buffer_Offset+=Element_Size;
    }

    //Look for first Sync word
    if (!Synched && !Synchronize())
        return;

    //Parse with PES parser
    while (Buffer_Parse());
}

//---------------------------------------------------------------------------
void File_MpegPs::Read_Buffer_Finalize()
{
    //Video
    Video.clear();
    for (size_t Stream_Pos=0; Stream_Pos<Video_Handle.size(); Stream_Pos++)
        if (Video_Handle[Stream_Pos])
        {

            Open_Buffer_Finalize(Video_Handle[Stream_Pos]);
            if (Merge(*Video_Handle[Stream_Pos])>0)
            {
                //Delay
                if (Video_TimeStamp[Stream_Pos].Have_PTS)
                {
                    Fill("Delay", "");
                    Fill("Delay", Video_TimeStamp[Stream_Pos].PTS/90);
                }

                //TimeStamp
                int64s PlayTime=Video_TimeStamp_End[Stream_Pos].PTS-Video_TimeStamp[Stream_Pos].PTS;
                if (PlayTime && Get(Stream_Video, StreamPos_Last, _T("PlayTime")).empty())
                    Fill("PlayTime", PlayTime/90);

                //Tags in MPEG Video
                Fill(Stream_General, 0, "Encoded_Library", Get(Stream_Video, StreamPos_Last, _T("Encoded_Library")));
            }

            delete Video_Handle[Stream_Pos];
            Video_Handle[Stream_Pos]=NULL;
        }

    //Audio
    Audio.clear();
    for (size_t Stream_Pos=0; Stream_Pos<Audio_Handle.size(); Stream_Pos++)
        if (Audio_Handle[Stream_Pos])
        {

            Open_Buffer_Finalize(Audio_Handle[Stream_Pos]);
            if (Merge(*Audio_Handle[Stream_Pos])>0)
            {
                //Delay
                if (Audio_TimeStamp[Stream_Pos].Have_PTS)
                {
                    Fill("Delay", "");
                    Fill("Delay", (Audio_TimeStamp[Stream_Pos].PTS)/90);
                }

                //TimeStamp
                int64s PlayTime=Audio_TimeStamp_End[Stream_Pos].PTS-Audio_TimeStamp[Stream_Pos].PTS;
                if (PlayTime && Get(Stream_Audio, StreamPos_Last, _T("PlayTime")).empty())
                    Fill("PlayTime", PlayTime/90);
            }

            delete Audio_Handle[Stream_Pos];
            Audio_Handle[Stream_Pos]=NULL;
        }

    //Private1
    for (size_t Stream_Pos=0; Stream_Pos<Private1_Handle.size(); Stream_Pos++)
        if (Private1_Handle[Stream_Pos])
        {

            Open_Buffer_Finalize(Private1_Handle[Stream_Pos]);
            if (Merge(*Private1_Handle[Stream_Pos])>0)
            {
                //Delay
                if (Private1_TimeStamp[Stream_Pos].Have_PTS)
                {
                    Fill("Delay", "");
                    Fill("Delay", Private1_TimeStamp[Stream_Pos].PTS/90);
                }

                //TimeStamp
                int64s PlayTime=Private1_TimeStamp_End[Stream_Pos].PTS-Private1_TimeStamp[Stream_Pos].PTS;
                if (PlayTime && Get(Stream_Audio, StreamPos_Last, _T("PlayTime")).empty())
                    Fill("PlayTime", PlayTime/90);
            }

            delete Private1_Handle[Stream_Pos];
            Private1_Handle[Stream_Pos]=NULL;
        }

    //Fill General
    if (General.empty())
        Stream_Prepare(Stream_General);

         if (!Video.empty() && Video[0](_T("Codec"))==_T("MPEG-2V"))
        Fill(Stream_General, 0, "Format", "MPEG-2PS");
    else if (!Video.empty() && Video[0](_T("Codec"))==_T("MPEG-4V"))
        Fill(Stream_General, 0, "Format", "MPEG-4PS");
    else if (MPEG_Version==2)
        Fill(Stream_General, 0, "Format", "MPEG-2PS");
    else if (MPEG_Version==1)
        Fill(Stream_General, 0, "Format", "MPEG-1PS");
}

//***************************************************************************
// Buffer
//***************************************************************************

//---------------------------------------------------------------------------
bool File_MpegPs::Buffer_Parse()
{
    //Remove zeros
    if (!RemoveZeros())
        return false;

    //Element name
    if (!Element_Name_Get())
        return false;

    //Element size
    if (!Element_Size_Get())
        return false;

    //Is OK?
    if (Element_Name_IsOK())
        Element_Parse();

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
//
bool File_MpegPs::Element_Parse()
{
    //Details
    if (Config.Details_Get())
    {
        Details_Add_Element(0, Element_Name, Element_Size);
    }

    //Header
    Buffer_Offset+=Element_HeaderSize;

    //Parsing
    switch (Element_Name)
    {
        case 0xB9 : MPEG_program_end(); break;
        case 0xBA : pack_start(); break;
        case 0xBB : system_header_start(); break;
        case 0xBC : program_stream_map(); break;
        case 0xBD : private_stream_1(); break;
        case 0xBE : padding_stream(); break;
        case 0xBF : private_stream_2(); break;
        default:
                 if (Element_Name>=0xC0
                  && Element_Name<=0xDF) audio_stream();
            else if (Element_Name>=0xE0
                  && Element_Name<=0xEF) video_stream();
            else
                TRUSTED_ISNOT("Unattended element!");
    }

    FLUSH();
    return true;
}

//***************************************************************************
// Elements
//***************************************************************************

#define INTEGRITY_GENERAL() \
    if (Count_Get(Stream_General)==0) \
    { \
        TRUSTED_ISNOT("Should not be here"); \
        return; \
    } \

#define INTEGRITY(ELEMENT_SIZE) \
    if (Element_Size!=ELEMENT_SIZE) \
    { \
        TRUSTED_ISNOT("Size is wrong"); \
        return; \
    } \

#define NOT_NEEDED(_TEST) \
    if (_TEST) \
    { \
        NOT_NEEDED(); \
        return; \
    } \

#define BEGIN \
    BitStream BS(Buffer+Buffer_Offset, Element_Size); \

#define END \
    { \
        BS.Byte_Align(); \
        INTEGRITY(BS.Offset_Get()); \
    } \

#define GET(_BITS, _INFO, _NAME) \
    { \
        _INFO=BS.Get(_BITS); \
        if (Config.Details_Get()) \
        { \
            Details_Add_Info(Error, #_NAME, _INFO); \
        } \
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

#define TEST(_BITS, _INFO, _TEST, _NAME) \
    { \
        GET (_BITS, _INFO, _NAME); \
        if (_TEST) \
        { \

#define TEST_SKIP(_BITS, _NAME) \
    { \
        int32u _NAME=BS.Get(_BITS); \
        if (Config.Details_Get()) \
        { \
            Details_Add_Info(Error, #_NAME, _NAME); \
        } \
        if (_NAME) \
        { \

#define TEST_END \
        } \
    } \

//---------------------------------------------------------------------------
// Packet "B9"
void File_MpegPs::MPEG_program_end()
{
    NAME("MPEG_program_end");
    INTEGRITY(0);
}

//---------------------------------------------------------------------------
// Packet "BA"
void File_MpegPs::pack_start()
{
    NAME("pack_start");
    NOT_NEEDED(Count_Get(Stream_General)>0);

    //Reading
    int32u Version, SysClock32, SysClock29, SysClock14, Padding;
    BEGIN
    GET ( 2, Version,                                           version) INFO(MpegPs_Version[Version]);
    if (Version==1)
    {
        //MPEG-2
        MPEG_Version=2;
        GET ( 3, SysClock32,                                    system_clock_reference_base32)
        MARK( 1,                                                marker_bit)
        GET (15, SysClock29,                                    system_clock_reference_base29)
        MARK( 1,                                                marker_bit)
        GET (15, SysClock14,                                    system_clock_reference_base14)
        MARK( 1,                                                marker_bit)
        SKIP( 9,                                                system_clock_reference_extension)
        MARK( 1,                                                marker_bit)
        SKIP(22,                                                program_mux_rate)
        MARK( 1,                                                marker_bit)
        MARK( 1,                                                marker_bit)
        SKIP( 5,                                                reserved)
        GET ( 3, Padding,                                       pack_stuffing_length)

        //Integrity
        INTEGRITY(10+(size_t)Padding)

        //int64u system_clock_reference_base=SysClock32<<30
        //                                || SysClock29<<15
        //                                || SysClock14<<00
        //                                ;
    }
    else
    {
        //MPEG-1
        MPEG_Version=1;
        INTEGRITY(8)
        SKIP( 1,                                              sync)
        SKIP( 1,                                              sync)
        GET ( 3, SysClock32,                                  system_clock_reference_base32)
        MARK( 1,                                              marker_bit)
        GET (15, SysClock29,                                  system_clock_reference_base29)
        MARK( 1,                                              marker_bit)
        GET (15, SysClock14,                                  system_clock_reference_base14)
        MARK( 1,                                              marker_bit)
        MARK( 1,                                              marker_bit)
        SKIP(22,                                              program_mux_rate)
        MARK( 1,                                              marker_bit)
    }

    //Filling
    if (Count_Get(Stream_General)==0)
        Stream_Prepare(Stream_General);
}

//---------------------------------------------------------------------------
// Packet "BB"
void File_MpegPs::system_header_start()
{
    NAME("system_header_start");
    INTEGRITY_GENERAL();

    int32u Audio_Count_Temp, Video_Count_Temp;
    BEGIN
    MARK( 1,                                                    marker_bit)
    SKIP(22,                                                    rate_bound)
    MARK( 1,                                                    marker_bit)
    GET ( 6, Audio_Count_Temp,                                  audio_bound)
    SKIP( 1,                                                    fixed_flag)
    SKIP( 1,                                                    CSPS_flag)
    SKIP( 1,                                                    system_audio_lock_flag)
    SKIP( 1,                                                    system_video_lock_flag)
    MARK( 1,                                                    marker_bit)
    GET ( 5, Video_Count_Temp,                                  video_bound)
    SKIP( 1,                                                    packet_rate_restriction_flag)
    SKIP( 7,                                                    reserved_byte)
    while (BS.Peek(1)==1)
    {
        int32u Scale, Size;
        SKIP( 8,                                                stream_id)
        MARK( 1,                                                marker_bit)
        MARK( 1,                                                marker_bit)
        GET ( 1, Scale,                                         STD_buffer_bound_scale)
        GET (13, Size,                                          STD_buffer_size_bound) INFO(Ztring::ToZtring(Size*(Scale?1024:128)) + _T(" bytes"));
    }
    END

    //Filling
    if (Audio_Count==(int8u)-1) //0xBB may be multipart
        Audio_Count=0;
    Audio_Count+=Audio_Count_Temp;
    if (Video_Count==(int8u)-1) //0xBB may be multipart
        Video_Count=0;
    Video_Count+=Video_Count_Temp;
}

//---------------------------------------------------------------------------
// Packet "BC"
void File_MpegPs::program_stream_map()
{
    NAME("program_stream_map");
    MPEG_Version=2;
    StreamKind.resize(0x100); //Init

    int32u PSI_Size;
    BEGIN
    SKIP( 1,                                                    current_next_indicator)
    SKIP( 2,                                                    reserved)
    SKIP( 5,                                                    program_stream_map_version)
    SKIP( 7,                                                    reserved)
    MARK( 1,                                                    marker_bit)
    GET (16, PSI_Size,                                          program_stream_info_length)
    for (int32u Descriptor_Pos=0; Descriptor_Pos<PSI_Size; Descriptor_Pos++)
    {
        //Descriptor
        //SKIP( 8,                                                descriptor)
        BS.Skip(8);
    }
    int32u ESM_Size, ES_Size, ESID, StreamID;
    size_t Streams_Count_Temp=0;
    int32u Pos=0;
    GET (16, ESM_Size,                                          elementary_stream_map_length)
    while (Pos<ESM_Size)
    {
        GET ( 8, StreamID,                                      stream_type) INFO(MpegTs_TS_program_map_section_StreamID(StreamID));
        GET ( 8, ESID,                                          elementary_stream_id)
        GET (16, ES_Size,                                       ES_info_length)
        for (int32u Descriptor_Pos=0; Descriptor_Pos<ES_Size; Descriptor_Pos++)
        {
            //Descriptor
            //SKIP( 8,                                            descriptor)
            BS.Skip(8);
        }

        StreamKind[ESID]=StreamID;
        Pos+=4+ES_Size;
        Streams_Count_Temp++;
    }
    SKIP(32,                                                    CRC_32)
    END
}

//---------------------------------------------------------------------------
// Packet "BD"
void File_MpegPs::private_stream_1()
{
    NAME("private_stream_1");

    //For TS streams in debug mode, which does not have Start chunk
    if (General.empty() && FromTS>0 && (Element_Size>10000 || Buffer_Offset+Element_Size+10000>=Buffer_Size)) //Trying to detect false positive
    {
        Stream_Prepare(Stream_General);
        Stream_Prepare(Stream_Audio);
        Video_Count=0;
        Audio_Count=1;
    }

    //Internal Stream_Pos
    int8u Private1_ID;
    if (FromTS==0)
    {
        //Pure MPEG-PS, ID is the first byte
        if (Element_Size<1)
            return;
        Private1_ID=Buffer[Buffer_Offset];
        Buffer_Offset++;
        Element_Size--;
    }
    else
    {
        //MPEG-TS, ID come from TS header
        Private1_ID=FromTS;
    }

    //-Details
    if (Config.Details_Get())
    {
        Details_Add_Element(Ztring::ToZtring(Private1_ID, 16).MakeUpperCase());
    }

    //New stream if needed
    int8u Stream_Pos=Private1_ID;
    if (Stream_Pos>=Private1_Handle.size())
    {
        Private1_Handle.resize(Stream_Pos+1);
        Private1_TimeStamp.resize(Stream_Pos+1);
        Private1_TimeStamp_End.resize(Stream_Pos+1);
    }
    if (Private1_Handle[Stream_Pos]==NULL)
    {
        Private1_Handle[Stream_Pos]=Private1_ChooseParser(Stream_Pos);
        if (Private1_Handle[Stream_Pos]==NULL)
            return;
        Private1_TimeStamp[Stream_Pos]=TimeStamp;
    }

    //TimeStamp
    Private1_TimeStamp_End[Stream_Pos]=TimeStamp;

    //Open
    Open_Buffer_Init(Private1_Handle[Stream_Pos], File_Size, File_Offset+Buffer_Offset);
    Open_Buffer_Continue(Private1_Handle[Stream_Pos], Buffer+Buffer_Offset, Element_Size);
    if (Private1_Handle[Stream_Pos] && Audio_Count>0 && (Private1_Handle[Stream_Pos]->File_GoTo>0 || Private1_Handle[Stream_Pos]->File_Offset==Private1_Handle[Stream_Pos]->File_Size))
        Audio_Count--;
}

//---------------------------------------------------------------------------
File__Base* File_MpegPs::Private1_ChooseParser(int8u Private1_ID)
{
    //Subtitles
         if (Private1_ID>=0x20 && Private1_ID<=0x3F)
    {
        //Details
        if (Config.Details_Get())
        {
            Details_Add_Element("Text");
        }

        //Filling
        File__Base* Handle=new File__Base();
        Handle->Stream_Prepare(Stream_Text);
        Handle->Fill("Codec", "2-bit RLE");
        return Handle;
    }

    //AC3
    else if (Private1_ID>=0x80 && Private1_ID<=0x87)
    {
        //Details
        if (Config.Details_Get())
        {
            Details_Add_Element("AC3");
        }

        //Filling
        #if defined(MEDIAINFO_AC3_YES)
            return new File_Ac3();
        #else
            //Filling
            File__Base* Handle=new File__Base();
            Handle->Stream_Prepare(Stream_Audio);
            Handle->Fill("Codec", "AC3");
            return Handle;
        #endif
    }

    //DTS
    else if (Private1_ID>=0x88 && Private1_ID<=0x8F) //7F for the beginning of a DTS stream (audio in MPEG-TS)
    {
        //Details
        if (Config.Details_Get())
        {
            Details_Add_Element("DTS");
        }

        //Filling
        #if defined(MEDIAINFO_DTS_YES)
            return new File_Dts();
        #else
            //Filling
            File__Base* Handle=new File__Base();
            Handle->Stream_Prepare(Stream_Audio);
            Handle->Fill("Codec", "DTS");
            return Handle;
        #endif
    }

    //PCM
    else if (Private1_ID>=0xA0 && Private1_ID<=0xA8)
    {
        //Details
        if (Config.Details_Get())
        {
            Details_Add_Element("PCM");
        }

        //Filling
        File__Base* Handle=new File__Base();
        Handle->Stream_Prepare(Stream_Audio);
        Handle->Fill("Codec", "PCM");

        //Resolution
        int Resolution=Buffer[Buffer_Offset+4]>>6;
        if (Resolution==0) Handle->Fill("Resolution", 16);
        if (Resolution==1) Handle->Fill("Resolution", 20);
        if (Resolution==2) Handle->Fill("Resolution", 24);
        int SamplingRate=(Buffer[Buffer_Offset+4]&0x30)>>4;
        if (SamplingRate==0) Handle->Fill("SamplingRate", 48000);
        if (SamplingRate==1) Handle->Fill("SamplingRate", 96000);
        int Channels=Buffer[Buffer_Offset+4]&0x07;
        Handle->Fill("Channel(s)", Channels+1);
        return Handle;
    }
    else
    {
        //Managing Transport Streams with ID from TS but with first byte indicating kind.
        if (Element_Size<1)
            return NULL;
        int8u Private1_ID=Buffer[Buffer_Offset];
        Buffer_Offset++;
        Element_Size--;
        return Private1_ChooseParser(Private1_ID);
    }
}

//---------------------------------------------------------------------------
// Packet "BE"
void File_MpegPs::padding_stream()
{
    NAME("padding_stream");
}

//---------------------------------------------------------------------------
// Packet "BF"
void File_MpegPs::private_stream_2()
{
    NAME("private_stream_2");
}

//---------------------------------------------------------------------------
void File_MpegPs::audio_stream()
{
    NAME("Audio");

    //For TS streams in debug mode, which does not have Start chunk
    if (FromTS>0 && General.empty() && (Element_Size>10000 || Buffer_Offset+Element_Size+10000>=Buffer_Size)) //Trying to detect false positive
    {
        Stream_Prepare(Stream_General);
        Stream_Prepare(Stream_Audio);
        Video_Count=0;
        Audio_Count=1;
        StreamKind.resize(0x100);
        StreamKind[Element_Name]=FromTS;
    }

    //New stream if needed
    int8u Stream_Pos=Element_Name-0xC0;
    if (Stream_Pos>=Audio_Handle.size())
    {
        Audio_Handle.resize(Stream_Pos+1);
        Audio_TimeStamp.resize(Stream_Pos+1);
        Audio_TimeStamp_End.resize(Stream_Pos+1);
    }
    if (Audio_Handle[Stream_Pos]==NULL)
    {
             if (StreamKind.empty() || StreamKind[Element_Name]==0x03 || StreamKind[Element_Name]==0x04)
        {
            #if defined(MEDIAINFO_MPEGA_YES)
                Audio_Handle[Stream_Pos]=new File_Mpega;
                ((File_Mpega*)Audio_Handle[Stream_Pos])->Frame_Count_Valid=1;
            #else
                //Filling
                Audio_Handle[Stream_Pos]=new File__Base();
                Audio_Handle[Stream_Pos]->Stream_Prepare(Stream_Audio);
                Audio_Handle[Stream_Pos]->Fill("Codec", "MPEG Audio");
            #endif
        }
        else
        {
            Audio_Handle[Stream_Pos]=new File__Base();
            Audio_Handle[Stream_Pos]->Stream_Prepare(Stream_Video);
            Audio_Handle[Stream_Pos]->Fill("Codec", "Unsupported");
        }

        Audio_TimeStamp[Stream_Pos]=TimeStamp;
    }

    //TimeStamp
    Audio_TimeStamp_End[Stream_Pos]=TimeStamp;

    //Open Mpega
    Open_Buffer_Init(Audio_Handle[Stream_Pos], File_Size, File_Offset+Buffer_Offset);
    Open_Buffer_Continue(Audio_Handle[Stream_Pos], Buffer+Buffer_Offset, Element_Size);
    if (Audio_Count>0 && (Audio_Handle[Stream_Pos]->File_GoTo>0 || Audio_Handle[Stream_Pos]->File_Offset==Audio_Handle[Stream_Pos]->File_Size))
        Audio_Count--;
}

//---------------------------------------------------------------------------
void File_MpegPs::video_stream()
{
    NAME ("Video");

    if (!CompleteFileName.empty())
    {
        //INTEGRITY_GENERAL()
    }

    //For TS streams in debug mode, which does not have Start chunk
    if (FromTS>0 && General.empty() && (Element_Size>10000 || Buffer_Offset+Element_Size+10000>=Buffer_Size)) //Trying to detect false positive
    {
        Stream_Prepare(Stream_General);
        Stream_Prepare(Stream_Video);
        Video_Count=1;
        Audio_Count=0;
        StreamKind.resize(0x100);
        StreamKind[Element_Name]=FromTS;
    }

    //New stream if needed
    int8u Stream_Pos=Element_Name-0xE0;
    if (Stream_Pos>=Video_Handle.size())
    {
        Video_Handle.resize(Stream_Pos+1);
        Video_Handle_Error.resize(Stream_Pos+1);
        Video_TimeStamp.resize(Stream_Pos+1);
        Video_TimeStamp_End.resize(Stream_Pos+1);
    }
    if (Video_Handle[Stream_Pos]==NULL)
    {
             if (StreamKind.empty() || StreamKind[Element_Name]<=0x02)
        {
            #if defined(MEDIAINFO_MPEGV_YES)
                Video_Handle[Stream_Pos]=new File_Mpegv;
                ((File_Mpegv*)Video_Handle[Stream_Pos])->MPEG_Version=MPEG_Version;
            #else
                //Filling
                Video_Handle[Stream_Pos]=new File__Base();
                Video_Handle[Stream_Pos]->Stream_Prepare(Stream_Video);
                Video_Handle[Stream_Pos]->Fill("Codec", "MPEG Video");
            #endif
        }
        else if (StreamKind[Element_Name]==0x10)
        {
            #if defined(MEDIAINFO_MPEG4V_YES)
                Video_Handle[Stream_Pos]=new File_Mpeg4v;
            #else
                //Filling
                Video_Handle[Stream_Pos]=new File__Base();
                Video_Handle[Stream_Pos]->Stream_Prepare(Stream_Video);
                Video_Handle[Stream_Pos]->Fill("Codec", "MPEG-4V");
            #endif
        }
        else if (StreamKind[Element_Name]==0x1B)
        {
            #if defined(MEDIAINFO_AVC_YES)
                Video_Handle[Stream_Pos]=new File_Avc;
            #else
                //Filling
                Video_Handle[Stream_Pos]=new File__Base();
                Video_Handle[Stream_Pos]->Stream_Prepare(Stream_Video);
                Video_Handle[Stream_Pos]->Fill("Codec", "AVC");
            #endif
        }
        else
        {
            Video_Handle[Stream_Pos]=new File__Base();
            Video_Handle[Stream_Pos]->Stream_Prepare(Stream_Video);
            Video_Handle[Stream_Pos]->Fill("Codec", "Unsupported");
        }

        Video_TimeStamp[Stream_Pos]=TimeStamp;
    }

    //TimeStamp
    Video_TimeStamp_End[Stream_Pos]=TimeStamp;

    //Open Mpegv
    Open_Buffer_Init(Video_Handle[Stream_Pos], File_Size, File_Offset+Buffer_Offset);
    Open_Buffer_Continue(Video_Handle[Stream_Pos], Buffer+Buffer_Offset, Element_Size);
    if (Video_Count>0 && Video_Handle[Stream_Pos]->File_GoTo>0)
        Video_Count--;
}

//***************************************************************************
// Helpers
//***************************************************************************

//---------------------------------------------------------------------------
bool File_MpegPs::Detect_NonMPEGPS ()
{
    //Element_Size
    if (Buffer_Size<8)
    {
        File_Offset=File_Size;
        return true;
    }

    //Detect DAT files, and the parser is not enough precise to detect them later
    if (CC4(Buffer)==CC4("RIFF"))
    {
        File_Offset=File_Size;
        return true;
    }

    //Detect TS files, and the parser is not enough precise to detect them later
    if (Buffer_Size>=188*4)
    {
        //Look for first Sync word
        while (Buffer_Offset<188 && CC1(Buffer+Buffer_Offset)!=0x47)
            Buffer_Offset++;
        if (Buffer_Offset<188 && CC1(Buffer+Buffer_Offset+188)==0x47 && CC1(Buffer+Buffer_Offset+188*2)==0x47 && CC1(Buffer+Buffer_Offset+188*3)==0x47)
        {
            File_Offset=File_Size;
            return true;
        }
        Buffer_Offset=0;
    }

    //Detect MPEG Version
    if (MPEG_Version==0)
        Detect_MPEG_Version();

    //Seems OK
    return false;
}

//---------------------------------------------------------------------------
void File_MpegPs::Detect_MPEG_Version ()
{
    /*
    size_t B5_Offset=Buffer_Offset;
    while (B5_Offset+4<=Buffer_Size && !(CC3(Buffer+B5_Offset)==0x000001 && CC1(Buffer+B5_Offset+3)==0xB5))
        B5_Offset++;
    if (B5_Offset+4>Buffer_Size)
        MPEG_Version=1;
    else
    */ //Currently default is MPEG-2, if Header says MPEG-1, this will be MPEG-1, no more.
        MPEG_Version=2;
}

//---------------------------------------------------------------------------
bool File_MpegPs::Detect_EOF_Needed ()
{
    //Jumping to the end of the file
    if (File_Size>=File_Offset+Buffer_Size+4*1024*1024 && File_Size!=(int64u)-1 && (
       (File_Offset>=2*1024*1024)
    || (Synched && Video_Count==0 && Audio_Count==0)
    ))
    {
        //Details
        if (Config.Details_Get())
        {
            Details_Add_Error("-------------------------------------------");
            Details_Add_Error("---   MPEG-PS, Jumping to end of file   ---");
            Details_Add_Error("-------------------------------------------");
        }

        //Jumping
        File_GoTo=File_Size-4*1024*1024;
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------
bool File_MpegPs::RemoveZeros ()
{
    //Keep out unuseful 0x00 in the stream
    while (               Buffer_Offset+3<=Buffer_Size
            && CC1(Buffer+Buffer_Offset+0)==0x00
            && CC1(Buffer+Buffer_Offset+1)==0x00
            && CC1(Buffer+Buffer_Offset+2)==0x00)
        Buffer_Offset++;
    if (Buffer_Offset+3>Buffer_Size)
        return false;

    //Integrity
    if (CC1(Buffer+Buffer_Offset+0)!=0x00
     || CC1(Buffer+Buffer_Offset+1)!=0x00)
    {
        TRUSTED_ISNOT("Sync error");
        Synched=false;
        return Synchronize();
    }

    return true;
}

//---------------------------------------------------------------------------
bool File_MpegPs::Synchronize ()
{
    //Look for first Sync word
    while (Buffer_Offset+4<=Buffer_Size && !(CC3(Buffer+Buffer_Offset)==0x000001 && CC1(Buffer+Buffer_Offset+3)>=0xB9)) //0xB9 is the lower value for MPEG-PS
        Buffer_Offset++;

    //Must wait more data?
    if (Buffer_Offset+4>Buffer_Size)
    {
        if (Count_Get(Stream_General)==0)
        {
            //We can purge buffer
            if (CC3(Buffer+Buffer_Offset)!=0x000001)
            {
                Buffer_Offset++;
                if (CC2(Buffer+Buffer_Offset)!=0x0000)
                {
                    Buffer_Offset++;
                    if (CC1(Buffer+Buffer_Offset)!=0x00)
                        Buffer_Offset++;
                }
            }
        }
        Synched=false;
        return false;
    }

    //OK, we continue
    Synched=true;
    return true;
}

//---------------------------------------------------------------------------
bool File_MpegPs::Element_Name_Get()
{
    //Enough data?
    if (Buffer_Offset+4>Buffer_Size)
        return false;

    //Element name
    Element_Name=BigEndian2int8u(Buffer+Buffer_Offset+3);

    return true;
}

//---------------------------------------------------------------------------
bool File_MpegPs::Element_Size_Get()
{
    switch (Element_Name)
    {
        //Fixed size
        case 0xB9 : //MPEG_program_end
            Element_HeaderSize=4; //MPEG start code + Element name
            Element_Size=0;
            Element_Next=Buffer_Offset+4; //MPEG start code + Element name
            break;

        //Not possible to know the size directly
        case 0xBA : //pack_start
            //Searching Element_Next
            Element_Next=Buffer_Offset+4;
            if (!Element_Size_Get_Element_Next())
                return false;
            //Filling
            Element_HeaderSize=4; //MPEG start code + Element name
            Element_Size=Element_Next-Buffer_Offset-4;
            break;

        //Header is only Size
        case 0xBB : //system_header_start
        case 0xBC : //program_stream_map
        case 0xBE : //padding_stream
        case 0xBF : //private_stream_2
        case 0xF0 : //ECM
        case 0xF1 : //EMM
        case 0xF2 : //DSMCC stream
        case 0xF8 : //ITU-T Rec. H .222.1 type E
        case 0xFF : //Program stream directory
            //Enough data?
            if (Buffer_Offset+6>Buffer_Size)
                return false;
            //Filling
            Element_HeaderSize=6; //MPEG start code + Element name + Element size
            Element_Size=BigEndian2int16u(Buffer+Buffer_Offset+4);
            Element_Next=Buffer_Offset+6+Element_Size; //MPEG start code + Element name + Element size
            break;

        //Element with PES Header (with Size in Header)
        default :
            if (!Buffer_Parse_Header()) //Header
                return false;
    }

    //Testing
    if (Element_Size==0) //Not normal
        TRUSTED_ISNOT("Next element is too small, not normal");

    return true;
}

//---------------------------------------------------------------------------
bool File_MpegPs::Element_Size_Get_Unlimited()
{
    //Searching Element_Next
    Element_Next=Buffer_Offset;
    if (!Element_Size_Get_Element_Next())
        Element_Next=Buffer_Size; //The whole buffer

    Element_HeaderSize=0;
    Element_Size=Element_Next-Buffer_Offset;
    Synched=true;
    return true;
}

//---------------------------------------------------------------------------
bool File_MpegPs::Element_Size_Get_Element_Next()
{
    //Integrity
    if (Element_Next<Buffer_Offset)
        Element_Next=Buffer_Size;
    if (Element_Next==Buffer_Size && CC3(Buffer+Element_Next)==0x000001 && CC1(Buffer+Element_Next+3)>=0xB9) //0xB9 is the lower value for MPEG-PS
        Element_Next+=4; //Not normal, risk of infinite loop

    //Searching Element_Next
    while (Element_Next+4<=Buffer_Size && !(CC3(Buffer+Element_Next)==0x000001 && CC1(Buffer+Element_Next+3)>=0xB9)) //0xB9 is the lower value for MPEG-PS
        Element_Next++;
    if (Element_Next+4>Buffer_Size)
    {
        if (File_Offset+Element_Next+4<=File_Size)
            return false; //This is not the end of file, so we must wait for more data

        //Managing special cases
        Element_Next=Buffer_Size; //End of file
    }

    return true;
}

//---------------------------------------------------------------------------
bool File_MpegPs::Element_Name_IsOK()
{
    //Is it the start of a MPEG PS?
    if (Count_Get(Stream_General)==0
     &&   Element_Name!=0xBA
     && !(Element_Name>=0xBD && Element_Name<=0xEF)
     )
    {
        //MPEG PS didn't begin, skipping
        //Details
        if (Config.Details_Get()>0.9)
        {
            Details_Add_Element(0, Element_Name, Element_Size-4);
            Details_Add_Element("Has not began, skipping");
        }
        Buffer_Offset+=Element_HeaderSize+Element_Size;
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------
// Packet header
bool File_MpegPs::Buffer_Parse_Header()
{
    //There is a PES Header with size and long packet header
    if (Buffer_Offset+6>=Buffer_Size)
        return false; //No enough bytes

    //Element size
    Element_Size=BigEndian2int16u(Buffer+Buffer_Offset+4); //Element size
    Element_HeaderSize=6;

    //Element_Next
    if (Element_Size==0)
    {
        //Video stream in Transport Stream, Element_Size may be unlimited
        Video_Unlimited=true;
        Element_Next=Buffer_Offset+6; ////MPEG start code + Element name + Element size
        if (!Element_Size_Get_Element_Next())
            Element_Next=Buffer_Size; //The whole buffer
        if (Element_Next>Buffer_Offset && Element_Next-Buffer_Offset>Element_HeaderSize)
            Element_Size=Element_Next-Buffer_Offset-Element_HeaderSize;
        else
            Element_Size=0; //There is a problem...
    }
    else
    {
        Element_Next=Buffer_Offset+6+Element_Size;

        //Testing end of the file before the end of the PES
        if (File_Offset+Element_Next>=File_Size) //This is the end of the file
            Element_Next=Buffer_Size;

        //Testing if we have enough data
        if (Element_Next>Buffer_Size)
            return false;
    }

    //Parsing header data
    if (!Buffer_Parse_Header_Data()) //Header
        return false;

    return true;
}

//---------------------------------------------------------------------------
// Packet header data
bool File_MpegPs::Buffer_Parse_Header_Data()
{
    TimeStamp.Have_PTS=false;
    TimeStamp.Have_DTS=false;

    //MPEG-2
    switch (MPEG_Version)
    {
        case 1  : if (!Buffer_Parse_Header_Data_MPEG1()) return false; break;
        case 2  : if (!Buffer_Parse_Header_Data_MPEG2()) return false; break;
        default : return true; //Change nothing
    }

    Element_Size=Element_Next-Buffer_Offset-Element_HeaderSize;
    return true;
}

//---------------------------------------------------------------------------
// Packet header data - MPEG-1
bool File_MpegPs::Buffer_Parse_Header_Data_MPEG1()
{
    //Keep out firsts 0xFF
    while (Buffer_Offset+Element_HeaderSize<Element_Next && Buffer[Buffer_Offset+Element_HeaderSize]==0xFF)
        Element_HeaderSize++;
    if (Buffer_Offset+Element_HeaderSize>=Element_Next)
        return true;

    //Buffer scale and size
    if ((Buffer[Buffer_Offset+Element_HeaderSize]&0xC0)==0x40)
    {
        Element_HeaderSize+=2;
    }

    //With PTS only
    if ((Buffer[Buffer_Offset+Element_HeaderSize]&0xF0)==0x20)
    {
        if (Element_Size<5)
            return false;
        BitStream BS(Buffer+Buffer_Offset+Element_HeaderSize, Element_Size);
        TimeStamp.Have_PTS=true;
        TimeStamp.Have_DTS=false;
        TimeStamp.PTS=MPEG_TimeStamp_Read(BS);
        Element_HeaderSize+=5;
    }

    //With PTS and DTS
    else if ((Buffer[Buffer_Offset+Element_HeaderSize]&0xF0)==0x30)
    {
        if (Element_Size<10)
            return false;
        BitStream BS(Buffer+Buffer_Offset+Element_HeaderSize, Element_Size);
        TimeStamp.Have_PTS=true;
        TimeStamp.Have_DTS=true;
        TimeStamp.PTS=MPEG_TimeStamp_Read(BS);
        Element_HeaderSize+=10;
    }

    return true;
}

//---------------------------------------------------------------------------
// Packet header data - MPEG-2
bool File_MpegPs::Buffer_Parse_Header_Data_MPEG2()
{
    if (Element_Size<3)
        return false;

    //Reading
    BitStream BS(Buffer+Buffer_Offset+6, Element_Size-6);
    int32u Flags, Size;
    BS.Skip(2);                                 //01
    BS.Skip(2);                                 //PES_scrambling_control
    BS.Skip(1);                                 //PES_priority
    BS.Skip(1);                                 //data_alignment_indicator
    BS.Skip(1);                                 //copyright
    BS.Skip(1);                                 //original_or_copy
    Flags=BS.Get(2);                            //PTS_DTS_flags
    BS.Skip(1);                                 //ESCR_flag
    BS.Skip(1);                                 //ES_rate_flag
    BS.Skip(1);                                 //DSM_trick_mode_flag
    BS.Skip(1);                                 //additional_copy_info_flag
    BS.Skip(1);                                 //PES_CRC_flag
    BS.Skip(1);                                 //PES_extension_flag
    Size=BS.Get(8);                             //PES_header_data_length

    Element_HeaderSize+=3;
    if (3+Size>Element_Size)
        return false;

    //Options
    if (Flags==0x2)
    {
        TimeStamp.Have_PTS=true;
        TimeStamp.Have_DTS=false;
        TimeStamp.PTS=MPEG_TimeStamp_Read(BS);
    }
    if (Flags==0x3)
    {
        TimeStamp.Have_PTS=true;
        TimeStamp.Have_DTS=true;
        TimeStamp.PTS=MPEG_TimeStamp_Read(BS);
        TimeStamp.DTS=MPEG_TimeStamp_Read(BS);
    }

    //Positioning Packet data start
    Element_HeaderSize+=Size;
    return true;
}

//---------------------------------------------------------------------------
int64u File_MpegPs::MPEG_TimeStamp_Read (BitStream &BS)
{
    int64u TimeStamp;

    BS.Skip(4);
    TimeStamp=BS.Get(3);
    BS.Skip(1);
    TimeStamp<<=15;
    TimeStamp|=BS.Get(15);
    BS.Skip(1);
    TimeStamp<<=15;
    TimeStamp|=BS.Get(15);
    BS.Skip(1);
    return TimeStamp;
}

//***************************************************************************
// Information
//***************************************************************************

//---------------------------------------------------------------------------
void File_MpegPs::HowTo(stream_t StreamKind)
{
    switch (StreamKind)
    {
        case (Stream_General) :
            Fill_HowTo("Format", "R");
            Fill_HowTo("OveralBitRate", "R");
            Fill_HowTo("PlayTime", "R");
            Fill_HowTo("Encoded_Application", "R");
            break;
        case (Stream_Video) :
            Fill_HowTo("Codec", "R");
            Fill_HowTo("BitRate", "R");
            Fill_HowTo("Width", "R");
            Fill_HowTo("Height", "R");
            Fill_HowTo("AspectRatio", "R");
            Fill_HowTo("FrameRate", "R");
            Fill_HowTo("Delay", "R");
            break;
        case (Stream_Audio) :
            Fill_HowTo("Codec", "R");
            Fill_HowTo("BitRate", "R");
            Fill_HowTo("Channel(s)", "R");
            Fill_HowTo("SamplingRate", "R");
            Fill_HowTo("Resolution", "R");
            Fill_HowTo("Delay", "R");
            break;
        case (Stream_Text) :
            Fill_HowTo("Codec", "R");
            Fill_HowTo("Delay", "R");
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

#endif //MEDIAINFO_MPEGPS_YES

