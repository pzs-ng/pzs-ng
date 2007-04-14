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

//---------------------------------------------------------------------------
// Compilation condition
#include <MediaInfo/Setup.h>
#if defined(MEDIAINFO_MPEGV_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Video/File_Mpegv.h"
#include <ZenLib/BitStream.h>
#include <ZenLib/Utils.h>
using namespace ZenLib;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
namespace MediaInfoLib
{
//---------------------------------------------------------------------------

//***************************************************************************
// Constants
//***************************************************************************

//---------------------------------------------------------------------------
const float Mpegv_FrameRate[]=
{
    0,
    23.976,
    24,
    25,
    29.97,
    30,
    50,
    59.94,
    60,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

//---------------------------------------------------------------------------
const float Mpegv_Ratio1[]=
{
    0,
    1,
    0.6735,
    0.7031, //16/9 PAL
    0.7615,
    0.8055,
    0.8437, //16/9 NTSC
    0.8935,
    0.9157, //4/3 PAL
    0.9815,
    1.0255,
    1.0695,
    1.0950, //4/3 NTSC
    1.1575,
    1.2015,
    0,
};

//---------------------------------------------------------------------------
const float Mpegv_Ratio2[]=
{
    0,
    1,
    (float)4/3,
    (float)16/9,
    2.21,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

//---------------------------------------------------------------------------
const char* Mpegv_Chroma[]=
{
    "",
    "4:2:0",
    "4:2:2",
    "4:4:4",
};

//---------------------------------------------------------------------------
const char* Mpegv_Standard[]=
{
    "Composite",
    "PAL",
    "NTSC",
    "SECAM",
    "MAC",
    "",
    "",
    "",
};

//---------------------------------------------------------------------------
const char* Mpegv_Interlacement[]=
{
    "Interlaced",
    "TFF",
    "BFF",
    "PPF",
};

//---------------------------------------------------------------------------
const char* Mpegv_Profile[]=
{
    "",
    "High",
    "Spatial",
    "SNR",
    "Main",
    "Simple",
    "",
    "",
};

//---------------------------------------------------------------------------
const char* Mpegv_Level[]=
{
    "",
    "",
    "",
    "",
    "High",
    "",
    "High-1440",
    "",
    "Main",
    "",
    "Low",
    "",
    "",
    "",
    "",
    "",
};

//---------------------------------------------------------------------------
const char* Mpegv_FrameType[]=
{
    "",
    "I",
    "P",
    "B",
    "D",
    "",
    "",
    "",
};

//***************************************************************************
// Format
//***************************************************************************

//---------------------------------------------------------------------------
void File_Mpegv::Read_Buffer_Init()
{
    //Elements
    Element_Size=0;
    Element_Next=0;
    Frame_Count=0;

    //Temp
    Time_Begin_Seconds=Error;
    Time_Begin_Frames=Error;
    Time_End_Seconds=Error;
    Time_End_Frames=Error;
    Time_End_NeedComplete=false;
    Width=0;
    Height=0;
    RatioValue=0;
    FrameRate=0;
    BitRate=0;

    //Error controls
    User_Start_Count=0;
    Synched=false;
}

//---------------------------------------------------------------------------
void File_Mpegv::Read_Buffer_Continue()
{
    //Integrity
    if (File_Offset==0 && Detect_NonMPEGV())
    {
        File_Offset=File_Size;
        return;
    }

    //Parse
    Trusted=Buffer_Size/512; //Pb with Test China 1.mpg if /1024
    while (Buffer_Parse());
}

//---------------------------------------------------------------------------
void File_Mpegv::Read_Buffer_Finalize()
{
    //Tags with multiple location
    //-AspectRatio
    float AspectRatio=0;
    if (MPEG_Version==2)
    {
        if (RatioValue==1 && Height!=0)
            AspectRatio=Width/Height;
        else
            AspectRatio=Mpegv_Ratio2[RatioValue];
    }
    else
    {
        if (Height!=0 && Mpegv_Ratio1[RatioValue]!=0)
            AspectRatio=(float)Width/Height/Mpegv_Ratio1[RatioValue];
    }

    //-Version
    if (MPEG_Version==2)
    {
        Fill(Stream_General, 0, "Format", "MPEG-2V");
        Fill("Codec", "MPEG-2V");
        Fill("Codec/String", "MPEG-2 Video");
    }
    else
    {
        Fill(Stream_General, 0, "Format", "MPEG-1V");
        Fill("Codec", "MPEG-1V");
        Fill("Codec/String", "MPEG-1 Video");
        Fill("Interlacement", "PPF");
    }
    Fill("Width", Width);
    Fill("Height", Height);
    Fill("AspectRatio", AspectRatio);
    Fill("FrameRate", FrameRate);

    if (BitRate==0x3FFFF)
        Fill("BitRate_Mode", "VBR");
    else
    {
        Fill("BitRate_Mode", "CBR");
        Fill("BitRate", BitRate*400);
    }

    if (Library.size()>=8)
    {
        Fill("Encoded_Library", Library);
        Fill(Stream_General, 0, "Encoded_Library", Library);
    }

    //Calculate InitTime with Framerate
    if (Time_End_NeedComplete && Config.ParseSpeed_Get()!=1)
        Time_End_Seconds=Error;
    if (Time_End_Seconds!=Error)
    {
        size_t Time_Begin=Time_Begin_Seconds*1000;
        size_t Time_End =Time_End_Seconds*1000;
        Time_End_Frames++; //+1 to count the last frame
        if (FrameRate)
        {
            Time_Begin+=(size_t)(Time_Begin_Frames*1000/FrameRate);
            Time_End  +=(size_t)(Time_End_Frames  *1000/FrameRate);
        }
        if (!Video.empty() && Time_End>Time_Begin)
            Video[0](_T("PlayTime")).From_Number(Time_End-Time_Begin);
    }
}

//***************************************************************************
// Buffer
//***************************************************************************

//---------------------------------------------------------------------------
bool File_Mpegv::Buffer_Parse()
{
    //Next frame
    if (!NextFrame())
        return false;

    //Element name
    Element_Name=BigEndian2int8u(Buffer+Buffer_Offset+3);

    //Element size
    if (!Element_Size_Get())
        return false;

    //Is OK?
    if (Element_Name_IsOK())
        Element_Parse();

    //If no need of more
    if (File_Offset==File_Size || File_GoTo>0)
        return false;

    //Next frame
    Buffer_Offset=Element_Next;
    return true;
}

//---------------------------------------------------------------------------
// Element parse
//
bool File_Mpegv::Element_Parse()
{
    //Details
    if (Config.Details_Get())
    {
        Details_Add_Element(0, Element_Name, Element_Size-4);
    }

    //Header
    Buffer_Offset+=4;
    Element_Size -=4;

    //Parse
    switch (Element_Name)
    {
        case 0x00: Picture_Start(); break;
        case 0xB2: User_Start(); break;
        case 0xB3: Sequence_Header(); break;
        case 0xB4: Sequence_Error(); break;
        case 0xB5: Extension_Start(); break;
        case 0xB7: Sequence_End(); break;
        case 0xB8: Group_Start(); break;
        default:
            if (Element_Name>=0x01
             && Element_Name<=0xAF) Slice_Start();
            else
                TRUSTED_ISNOT("Unattended element!");
    }

    FLUSH();
    return true;
}

//***************************************************************************
// Elements
//***************************************************************************

#define NOT_NEEDED(_TEST) \
    if (_TEST) \
    { \
        if (Config.Details_Get()) \
        { \
            Details_Add_Info("(Not needed)"); \
        } \
        return; \
    } \

#define NEXTFRAME_TEST() \
    { \
        if (!Frame_ShouldBe.empty()) \
        { \
            bool OK=false; \
            for (size_t Pos=0; Pos<Frame_ShouldBe.size(); Pos++) \
                if (Frame_ShouldBe[Pos]==Element_Name) \
                    OK=true; \
            if (!OK) \
                TRUSTED_ISNOT("Frames are not in the right order"); \
        } \
    } \

#define NEXTFRAME_CLEAR() \
    { \
        Frame_ShouldBe.clear(); \
    } \

#define NEXTFRAME_ADD(_Frame) \
    { \
        Frame_ShouldBe.push_back(_Frame); \
    } \

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

#define TEST(_BITS, _INFO, _TEST, _NAME) \
    { \
        GET (_BITS, _INFO, _NAME) \
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
// Packet "00", Picture Start
//
void File_Mpegv::Picture_Start()
{
    DETAILLEVEL_SET(0);
    NAME("Picture_Start");
    NEXTFRAME_CLEAR();
    NEXTFRAME_ADD(0x01);
    NEXTFRAME_ADD(0xB2);
    NEXTFRAME_ADD(0xB5);

    //Frame counts
    Frame_Count++;
    if (Config.Details_Get())
    {
        Details_Add_Element(Ztring::ToZtring(Frame_Count));
    }

    //Reading
    int32u FrameType;
    BEGIN
    SKIP(10,                                                temporal_reference)
    GET ( 3, FrameType,                                     picture_coding_type)

    //Time
    if (Time_End_Seconds!=Error)
        Time_End_Frames++;

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(Mpegv_FrameType[FrameType]);
        if (Time_End_Seconds!=Error)
        {
            size_t Time_Begin=Time_Begin_Seconds*1000;
            size_t Time_End =Time_End_Seconds*1000;
            if (FrameRate)
            {
                Time_Begin+=(size_t)(Time_Begin_Frames*1000/FrameRate);
                Time_End  +=(size_t)(Time_End_Frames  *1000/FrameRate);
            }
            Details_Add_Info(Error, "Time", Time_End-Time_Begin);
        }
    }
}

//---------------------------------------------------------------------------
// Packet "01" --> "AF", Slice Start
//
void File_Mpegv::Slice_Start()
{
    DETAILLEVEL_SET(0.7);
    NAME("Slice_Start");
    INTEGRITY_GENERAL();
    NEXTFRAME_TEST();
    NEXTFRAME_CLEAR();
    NEXTFRAME_ADD(0x00);
    NEXTFRAME_ADD(Element_Name);
    NEXTFRAME_ADD(Element_Name+1);
    NEXTFRAME_ADD(Element_Name+2);
    NEXTFRAME_ADD(0xB3);
    NEXTFRAME_ADD(0xB8);

    //Detection is finnished
    if (File_Size>=File_Offset+Buffer_Size+4*1024*1024 && File_Size!=(int64u)-1 && Config.ParseSpeed_Get()<=0.01 && (
       (Count_Get(Stream_Video)>0)
    ))
    {
        DETAILLEVEL_SET(0);
        //Details
        if (Config.Details_Get())
        {
            Details_Add_Error("------------------------------------------");
            Details_Add_Error("---   MPEG-V, Jumping to end of file   ---");
            Details_Add_Error("------------------------------------------");
        }

        //Jumping
        File_GoTo=File_Size-4*1024*1024;
        Synched=false;
        NEXTFRAME_CLEAR();
        Time_End_Seconds=Error;
        Time_End_Frames=Error;
    }
}

//---------------------------------------------------------------------------
// Packet "B2", User Start
//
void File_Mpegv::User_Start()
{
    DETAILLEVEL_SET(0);
    NAME("User_Start");

    //In case of User start is not used for library name
    if (User_Start_Count>2)
        return;
    User_Start_Count++;

    //Reading
    size_t Library_Offset=0;
    size_t Library_Size=Element_Size;

    //-Reject junk after the name
    while (Library_Size>0 && (Buffer[Buffer_Offset+Library_Offset+Library_Size-1]<0x20 || Buffer[Buffer_Offset+Library_Offset+Library_Size-1]>0x7D))
        Library_Size--;
    if (Library_Size==0)
        return;

    //-Reject junk before the name
    size_t Library_Offset_Final=Library_Offset+Library_Size-1;
    while (Buffer_Offset+Library_Offset_Final>=Buffer_Offset+Library_Offset && Buffer[Buffer_Offset+Library_Offset_Final]>=0x20 && Buffer[Buffer_Offset+Library_Offset_Final]<=0x7D)
        Library_Offset_Final--;
    if (Buffer_Offset+Library_Offset_Final>=Buffer_Offset+Library_Offset)
    {
        Library_Size -=Library_Offset_Final-Library_Offset;
        Library_Offset=Library_Offset_Final;
    }

    //Filling
    Library.From_Local((const char*)Buffer+Buffer_Offset+Library_Offset, Library_Size);

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(Library_Offset, "Datas", Library);
    }
}

//---------------------------------------------------------------------------
// Packet "B3", Sequence Header
//
void File_Mpegv::Sequence_Header()
{
    DETAILLEVEL_SET(0);
    NAME("Sequence_Header");
    NEXTFRAME_CLEAR();
    NEXTFRAME_ADD(0xB2);
    NEXTFRAME_ADD(0xB5);
    NEXTFRAME_ADD(0xB8);
    NOT_NEEDED(!General.empty());

    //Reading
    int32u FrameRate_Code, Intra, Intra_No;
    BEGIN
    GET (12, Width,                                             horizontal_size_value)
    GET (12, Height,                                            vertical_size_value)
    GET ( 4, RatioValue,                                        aspect_ratio_information)
    GET ( 4, FrameRate_Code,                                    frame_rate_code)
    GET (18, BitRate,                                           bit_rate_value)
    SKIP( 1,                                                    marker_bit)
    SKIP(10,                                                    vbv_buffer_size_value)
    SKIP( 1,                                                    constrained_parameters_flag)
    TEST( 1, Intra, Intra,                                      load_intra_quantiser_matrix)
        for (size_t Pos=0; Pos<64; Pos++)
            SKIP(8,                                             intra_quantiser_matrix)
        TEST_END
    TEST( 1, Intra_No, Intra_No,                                load_non_intra_quantiser_matrix)
        for (size_t Pos=0; Pos<64; Pos++)
            SKIP(8,                                             non_intra_quantiser_matrix)
        TEST_END
    END

    //Calculations
    FrameRate=Mpegv_FrameRate[FrameRate_Code];

    //Filling
    Stream_Prepare(Stream_General);
    Stream_Prepare(Stream_Video);
    if (Intra || Intra_No)
    {
        Fill("Codec_Settings", "CustomMatrix");
        Fill("Codec_Settings/Matrix", "Custom");
    }
    else
        Fill("Codec_Settings/Matrix", "Standard");
}

//---------------------------------------------------------------------------
// Packet "B4", Sequence Error
//
void File_Mpegv::Sequence_Error()
{
    DETAILLEVEL_SET(0);
    NAME("Sequence_Error");
    INTEGRITY_GENERAL();
}

//---------------------------------------------------------------------------
// Packet "B5", Extension Start
//
void File_Mpegv::Extension_Start()
{
    DETAILLEVEL_SET(0);
    NAME("Extension_Start");
    INTEGRITY_GENERAL();
    MPEG_Version=2; //Extension_Start only exists in MPEG-2 specs

    //Reading
    int32u ID;
    BEGIN
    GET ( 4, ID,                                           extension_start_code_identifier)

    //sequence_extension
         if (ID==1)
    {
        INFO("Sequence Extension");

        //Reading
        int32u Profile, Level, Progressive, Chroma, Width_Ext, Height_Ext,
               BitRate_Ext, FrameRate_N, FrameRate_D;
        SKIP( 1,                                                profile_and_level_indication_escape)
        GET ( 3, Profile,                                       profile_and_level_indication_profile) INFO(Mpegv_Profile[Profile]);
        GET ( 4, Level,                                         profile_and_level_indication_level) INFO(Mpegv_Level[Level]);
        GET ( 1, Progressive,                                   progressive_sequence)
        GET ( 2, Chroma,                                        chroma_format) INFO(Mpegv_Chroma[Chroma]);
        GET ( 2, Width_Ext,                                     horizontal_size_extension)
        GET ( 2, Height_Ext,                                    vertical_size_extension)
        GET (12, BitRate_Ext,                                   bit_rate_extension)
        SKIP( 1,                                                marker_bit)
        SKIP( 8,                                                vbv_buffer_size_extension)
        SKIP( 1,                                                low_delay)
        GET ( 2, FrameRate_N,                                   frame_rate_extension_n)
        GET ( 5, FrameRate_D,                                   frame_rate_extension_d)
        END

        //Filling
        Width+=0x1000*Width_Ext;
        Height+=0x1000*Height_Ext;
        BitRate+=0x4000*BitRate_Ext;
        if (FrameRate_D!=0)
            FrameRate=FrameRate_N/FrameRate_D;
        if (Progressive && Get(Stream_Video, 0, _T("Interlacement")).empty())
            Fill("Interlacement", Mpegv_Interlacement[3]);
        if (Get(Stream_Video, 0, _T("Chroma")).empty())
            Fill("Chroma", Mpegv_Chroma[Chroma]);
        if (Profile<8 && Level<16 && Get(Stream_Video, 0, _T("Codec_Profile")).empty())
            Fill("Codec_Profile", Ztring().From_Local(Mpegv_Profile[Profile])+_T("@")+Ztring().From_Local(Mpegv_Level[Level]));
    }

    //sequence_display_extension
    else if (ID==2)
    {
        INFO("Sequence Display Extension");

        //Reading
        int32u Standard;

        GET ( 3, Standard,                                      video_format) INFO(Mpegv_Standard[Standard]);
        TEST_SKIP( 1,                                           load_intra_quantiser_matrix)
            SKIP( 8,                                            colour_primaries)
            SKIP( 8,                                            transfer_characteristics)
            SKIP( 8,                                            matrix_coefficients)
            TEST_END
        SKIP(14,                                                display_horizontal_size)
        SKIP( 1,                                                marker_bit)
        SKIP(14,                                                display_vertical_size)
        END

        //Filling
        if (Standard<8 && Get(Stream_Video, 0, _T("Standard")).empty())
            Fill("Standard", Mpegv_Standard[Standard]);
    }
    //
    else if (ID==3)
    {
        INFO("Quant Matrix Extension");
    }
    //
    else if (ID==4)
    {
        INFO("Copyright");
    }
    //
    else if (ID==5)
    {
        INFO("Sequence Scalable");
    }
    //
    else if (ID==7)
    {
        INFO("Picture Display");
    }
    //
    else if (ID==8)
    {
        INFO("Picture Coding");
        //Reading
        int32u Standard, PictureStructure, TopField;
        GET ( 4, Standard,                                      video_format)
        SKIP( 4,                                                f_code_forward_horizontal)
        SKIP( 4,                                                f_code_forward_vertical)
        SKIP( 4,                                                f_code_backward_horizontal)
        SKIP( 4,                                                f_code_backward_vertical)
        SKIP( 2,                                                intra_dc_precision)
        GET ( 2, PictureStructure,                              picture_structure)
        GET ( 1, TopField,                                      top_field_first)
        SKIP( 1,                                                frame_pred_frame_dct)
        SKIP( 1,                                                concealment_motion_vectors)
        SKIP( 1,                                                q_scale_type)
        SKIP( 1,                                                intra_vlc_format)
        SKIP( 1,                                                alternate_scan)
        SKIP( 1,                                                repeat_first_field)
        SKIP( 1,                                                chroma_420_type)
        SKIP( 1,                                                progressive_frame)
        TEST_SKIP( 1,                                           composite_display_flag)
            SKIP( 1,                                            v_axis)
            SKIP( 3,                                            field_sequence)
            SKIP( 1,                                            sub_carrier)
            SKIP( 7,                                            burst_amplitude)
            SKIP( 8,                                            sub_carrier_phase)
            TEST_END
        END

        //Filling
        size_t Interlacement;
        if (PictureStructure==3)
        {
            if (TopField)
                Interlacement=1; //TopField
            else
                Interlacement=2; //BottomField
        }
        else
            Interlacement=PictureStructure; //Depend of 1st field found
        if (Get(Stream_Video, 0, _T("Interlacement")).empty())
            Fill("Interlacement", Mpegv_Interlacement[Interlacement]);
    }
    //
    else if (ID==9)
    {
        INFO("Picture Spatial Scalable");
    }
    //
    else if (ID==10)
    {
        INFO("Picture Temporal Scalable");
    }
    //
    else
    {
        INFO("Reserved");
    }
}

//---------------------------------------------------------------------------
// Packet "B7", Sequence_End
//
void File_Mpegv::Sequence_End()
{
    DETAILLEVEL_SET(0);
    NAME("Sequence_End");
    INTEGRITY_GENERAL();
}

//---------------------------------------------------------------------------
// Packet "B8", Group Start
//
void File_Mpegv::Group_Start()
{
    DETAILLEVEL_SET(0);
    NAME("Group_Start");
    NEXTFRAME_CLEAR();
    NEXTFRAME_ADD(0x00);
    NEXTFRAME_ADD(0xB2);
    NEXTFRAME_ADD(0xB5);
    INTEGRITY_GENERAL();

    //Reading
    BitStream BS(Buffer+Buffer_Offset, Element_Size);
    int32u Hours, Minutes, Seconds, Frames;
    SKIP( 1,                                                    time_code_drop_frame_flag)
    GET ( 5, Hours,                                             time_code_time_code_hours)
    GET ( 6, Minutes,                                           time_code_time_code_minutes)
    SKIP( 1,                                                    time_code_marker_bit)
    GET ( 6, Seconds,                                           time_code_time_code_seconds)
    GET ( 6, Frames,                                            time_code_time_code_pictures)
    SKIP( 1,                                                    closed_gop)
    SKIP( 1,                                                    broken_link)
    END

    //Calculating
    if (Time_Begin_Seconds==Error)
    {
        //Save begin time before
        Time_Begin_Seconds=60*60*Hours+60*Minutes+Seconds;
        Time_Begin_Frames =Frames;
    }
    else
    {
        //Is it always the same?
        if (60*60*Hours+60*Minutes+Seconds==Time_Begin_Seconds && Frames==Time_Begin_Frames)
            Time_End_NeedComplete=true; //Will always be empty...
    }
    if (!Time_End_NeedComplete)
    {
        Time_End_Seconds=60*60*Hours+60*Minutes+Seconds;
        Time_End_Frames =Frames-1; //We must not count the next picture
    }
}

//***************************************************************************
// Helpers
//***************************************************************************

//---------------------------------------------------------------------------
bool File_Mpegv::Detect_NonMPEGV ()
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

    //Seems OK
    return false;
}

//---------------------------------------------------------------------------
bool File_Mpegv::NextFrame()
{
    //Look for first Sync word
    size_t Buffer_Offset_Temp=Buffer_Offset;
    while (Buffer_Offset_Temp+4<=Buffer_Size && !(CC3(Buffer+Buffer_Offset_Temp)==0x000001 && CC1(Buffer+Buffer_Offset_Temp+3)<=0xB8)) //0xB8 is the upper value for MPEG-V
        Buffer_Offset_Temp++;

    //Not synched case
    if (!Synched && Buffer_Offset_Temp+4>=Buffer_Size)
    {
        Buffer_Offset_Temp++; //keep only 3 bytes
        Buffer_Offset=Buffer_Offset_Temp;
        return false;
    }

    //Error in stream?
    if (Synched && Buffer_Offset_Temp-Buffer_Offset>0)
    {
        TRUSTED_ISNOT("Sync error");
        Synched=false;
        return false;
    }

    //Must wait more data?
    if (Buffer_Offset_Temp+4>Buffer_Size)
        return false;

    //OK, we continue
    Synched=true;
    Buffer_Offset=Buffer_Offset_Temp;
    return true;
}

//---------------------------------------------------------------------------
bool File_Mpegv::Element_Size_Get()
{
    //Element with known size
    if (Element_Name==0xB7) //End
    {
        Element_Size=0;
        Element_Next=Buffer_Offset+4;
        return true;
    }

    //Searching Element_Next
    Element_Next=Buffer_Offset+4;
    while(Element_Next+3<=Buffer_Size && !(CC3(Buffer+Element_Next)==0x000001 && CC1(Buffer+Element_Next+3)<=0xB8)) //0xB8 is the upper value for MPEG-V
        Element_Next++;
    if (Element_Next+3>Buffer_Size)
    {
        //Not found
        //Maybe next byte is not good too
        if (CC2(Buffer+Element_Next)!=0x0000)
        {
            Element_Next++;
            //Maybe next byte is not good too
            if (CC1(Buffer+Element_Next)!=0x00)
                Element_Next++;
        }

        //Testing
        if (Element_Next-Buffer_Offset>=128*1024) //False positive, no element should have more than this size
        {
            TRUSTED_ISNOT("Next element is too big, not normal");
            File_Offset=File_Size;
        }
        Element_Next-=Buffer_Offset; //Because buffer will be moved by File__Base
        return false;
    }

    //Filling
    Element_Size=Element_Next-Buffer_Offset;

    //Testing
    if (Element_Size==0) //Not normal
        TRUSTED_ISNOT("Next element is too small, not normal");

    return true;
}

bool File_Mpegv::Element_Name_IsOK()
{
    //Is it the start of a MPEG-V?
    if (Count_Get(Stream_General)==0
     && !(Element_Name==0xB3)
    )
    {
        //MPEG V didn't begin, skipping
        //Details
        if (Config.Details_Get()>0.9)
        {
            Details_Add_Element(0, Element_Name, Element_Size-4);
            Details_Add_Element("Has not began, skipping");
            FLUSH();
        }
        return false;
    }

    return true;
}

//***************************************************************************
// Information
//***************************************************************************

//---------------------------------------------------------------------------
void File_Mpegv::HowTo(stream_t StreamKind)
{
    switch (StreamKind)
    {
        case (Stream_General) :
            Fill_HowTo("Format", "R");
            Fill_HowTo("Encoded_Application", "R");
            break;
        case (Stream_Video) :
            Fill_HowTo("Codec", "R");
            Fill_HowTo("BitRate", "R");
            Fill_HowTo("Width", "R");
            Fill_HowTo("Height", "R");
            Fill_HowTo("AspectRatio", "R");
            Fill_HowTo("FrameRate", "R");
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

#endif //MEDIAINFO_MPEGV_YES

