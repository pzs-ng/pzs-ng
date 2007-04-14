// File_Avc - Info for AVC Video files
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
#if defined(MEDIAINFO_AVC_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Video/File_Avc.h"
#include <ZenLib/BitStream.h>
#include <ZenLib/Utils.h>
#include <math.h>
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Constants
//***************************************************************************

//---------------------------------------------------------------------------
const size_t Avc_Errors_MaxCount=32;

const char* Avc_PicType[]=
{
    "I",
    "I, P",
    "I, P, B",
    "SI",
    "SI, SP",
    "I, SI",
    "I, SI, P, SP",
    "I, SI, P, SP, B",
};

const char* Avc_SliceType[]=
{
    "P",
    "B",
    "I",
    "SP",
    "SI",
    "P",
    "B",
    "I",
    "SP",
    "SI",
};
//---------------------------------------------------------------------------


//***************************************************************************
// Format
//***************************************************************************

//---------------------------------------------------------------------------
void File_Avc::Read_Buffer_Init()
{
}

//---------------------------------------------------------------------------
void File_Avc::Read_Buffer_Continue()
{
    //Look for first Sync word
    while (Buffer_Offset+3<Buffer_Size && CC3(Buffer+Buffer_Offset)!=0x000001)
        Buffer_Offset++;
    if (Buffer_Offset+3>=Buffer_Size)
        return;

    //Parse with PES parser
    while (Buffer_Parse());
}

//***************************************************************************
// Buffer
//***************************************************************************

#define BEGIN \
    BitStream BS(Buffer+Buffer_Offset, Element_Size); \

#define END \
    { \
        BS.Byte_Align(); \
        INTEGRITY(BS.Offset_Get()) \
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

#define ZERO_E(_BITS, _NAME) \
    { \
        int32u Temp=BS.Get(_BITS); \
        if (Config.Details_Get()) \
        { \
            Details_Add_Element_Info(Error, #_NAME, Temp); \
        } \
        if (Temp!=0) \
        { \
            TRUSTED_ISNOT("Zero bit is wrong"); \
            return false; \
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

//---------------------------------------------------------------------------
bool File_Avc::Buffer_Parse()
{
    //Enough data?
    if (Buffer_Offset+4>Buffer_Size)
        return false;

    //Element name
    if (!Element_Name_Get())
        return false;

    //Element size
    if (!Element_Size_Get())
        return false;

    //Parse
    Element_Parse();

    //If no need of more
    if (File_Offset==File_Size || File_GoTo>0)
        return false;

    //Next Frame
    Buffer_Offset=Element_Next;
    return true;
}

//---------------------------------------------------------------------------
//
bool File_Avc::Element_Parse()
{
    //Details
    if (Config.Details_Get())
    {
        Details_Add_Element(0, Element_Name, Element_Size);
    }

    //Header
    Buffer_Offset+=4;

    //Parsing
    switch (Element_Name)
    {
        case 0x01 : slice_layer_without_partitioning_non_IDR(); break;
        case 0x02 : ; break;
        case 0x03 : ; break;
        case 0x04 : ; break;
        case 0x05 : slice_layer_without_partitioning_IDR(); break;
        case 0x06 : sei(); break;
        case 0x07 : seq_parameter_set(); break;
        case 0x08 : pic_parameter_set(); break;
        case 0x09 : access_unit_delimiter(); break;
        case 0x10 : ; break;
        case 0x11 : ; break;
        case 0x12 : ; break;
        default :
            TRUSTED_ISNOT("Unattended element!");
    }

    FLUSH();
    return true;
}

//***************************************************************************
// Elements
//***************************************************************************

#define NAME(ELEMENT_NAME) \
    if (Config.Details_Get()) \
    { \
        Details_Add_Element(ELEMENT_NAME); \
    } \

#define SKIP_U(_NAME) \
    { \
        int LeadingZeroBits=0; \
        while(BS.Remain()>0 && BS.Get(1)==0) \
            LeadingZeroBits++; \
        if (Config.Details_Get()) \
        { \
            Details_Add_Info(Error, #_NAME, (int32u)pow(2, (float)LeadingZeroBits)-1+BS.Get(LeadingZeroBits)); \
        } \
        else \
            BS.Skip(LeadingZeroBits); \
    } \

#define SKIP_S(_NAME) \
    { \
        int LeadingZeroBits=0; \
        while(BS.Remain()>0 && BS.Get(1)==0) \
            LeadingZeroBits++; \
        if (Config.Details_Get()) \
        { \
            double Temp=pow(2, (float)LeadingZeroBits)-1+BS.Get(LeadingZeroBits); \
            Details_Add_Info(Error, #_NAME, (int32u)pow(-1, Temp+1)*ceil(Temp/2)); \
        } \
        else \
            BS.Skip(LeadingZeroBits); \
    } \

#define GET_U(_INFO, _NAME) \
    { \
        int LeadingZeroBits=0; \
        while(BS.Remain()>0 && BS.Get(1)==0) \
            LeadingZeroBits++; \
        _INFO=(int32u)pow(2, (float)LeadingZeroBits)-1+BS.Get(LeadingZeroBits); \
        if (Config.Details_Get()) \
        { \
            Details_Add_Info(Error, #_NAME, _INFO, 16); \
        } \
    }

#define GET_S(_INFO, _NAME) \
    { \
        int LeadingZeroBits=0; \
        while(BS.Remain()>0 && BS.Get(1)==0) \
            LeadingZeroBits++; \
        double Temp=(int32u)pow(2, (float)LeadingZeroBits)-1+BS.Get(LeadingZeroBits); \
        _INFO=pow(-1, Temp+1)*ceil(Temp/2); \
        if (Config.Details_Get()) \
        { \
            Details_Add_Info(Error, #_NAME, _INFO); \
        } \
    }

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
//
void File_Avc::slice_layer_without_partitioning_non_IDR()
{
    NAME("slice_layer_without_partitioning (non-IDR)");

    slice_header();
}

//---------------------------------------------------------------------------
//
void File_Avc::slice_layer_without_partitioning_IDR()
{
    NAME("slice_layer_without_partitioning (IDR)");

    slice_header();
}

//---------------------------------------------------------------------------
//
void File_Avc::slice_header()
{
    int32u SliceType;
    BEGIN
    SKIP_U(                                                     first_mb_in_slice)
    GET_U (SliceType,                                           slice_type) if (SliceType<9) INFO(Avc_SliceType[SliceType]);
    SKIP_U(                                                     pic_parameter_set_id)
    SKIP  (log2_max_frame_num_minus4+4,                         frame_num)
    if (!frame_mbs_only_flag)
    {
        TEST_SKIP  (1,                                          field_pic_flag)
            SKIP  (1,                                           bottom_field_flag);
        TEST_END
    }
}

//---------------------------------------------------------------------------
//
void File_Avc::seq_parameter_set()
{
    NAME("seq_parameter_set");

    int32u Profile, Level, Width, Height;
    int32u pic_order_cnt_type, frame_cropping_flag, vui_parameters_present_flag;
    BEGIN
    GET ( 8, Profile,                                           profile_idc)
    SKIP( 1,                                                    constraint_set0_flag)
    SKIP( 1,                                                    constraint_set1_flag)
    SKIP( 1,                                                    constraint_set2_flag)
    ZERO( 5,                                                    reserved_zero_5bits)
    GET ( 8, Level,                                             level_idc)
    SKIP_U(                                                     seq_parameter_set_id)
    GET_U (log2_max_frame_num_minus4,                           log2_max_frame_num_minus4)
    GET_U (pic_order_cnt_type,                                  pic_order_cnt_type)
    if (pic_order_cnt_type==0)
    {
        SKIP_U(                                                 log2_max_pic_order_cnt_lsb_minus4)
    }
    else if (pic_order_cnt_type==1)
    {
        int32u num_ref_frames_in_pic_order_cnt_cycle;
        SKIP(1,                                                 delta_pic_order_always_zero_flag)
        SKIP_S(                                                 offset_for_non_ref_pic)
        SKIP_S(                                                 offset_for_top_to_bottom_field)
        GET_U (num_ref_frames_in_pic_order_cnt_cycle,           num_ref_frames_in_pic_order_cnt_cycle)
        for(int32u Pos=0; Pos<num_ref_frames_in_pic_order_cnt_cycle; Pos++)
        {
            SKIP_S(                                             offset_for_ref_frame)
        }
    }
    SKIP_U(                                                     num_ref_frames)
    SKIP( 1,                                                    gaps_in_frame_num_value_allowed_flag)
    GET_U (Width,                                               pic_width_in_mbs_minus1)
    GET_U (Height,                                              pic_height_in_map_units_minus1)
    GET ( 1, frame_mbs_only_flag,                               frame_mbs_only_flag)
    if (!frame_mbs_only_flag)
    {
        SKIP( 1,                                                mb_adaptive_frame_field_flag)
    }
    SKIP( 1,                                                    direct_8x8_inference_flag)
    GET ( 1, frame_cropping_flag,                               frame_cropping_flag)
    if (frame_cropping_flag)
    {
        SKIP_U(                                                 frame_crop_left_offset)
        SKIP_U(                                                 frame_crop_right_offset)
        SKIP_U(                                                 frame_crop_top_offset)
        SKIP_U(                                                 frame_crop_bottom_offset)
    }
    GET ( 1, vui_parameters_present_flag,                       vui_parameters_present_flag)
    if (vui_parameters_present_flag)
    {
    }

    //Profile
    Ztring ProfileS;
    switch (Profile)
    {
        case 0x42 : ProfileS=_T("Baseline"); break;
        case 0x4D : ProfileS=_T("Main"); break;
        case 0x58 : ProfileS=_T("Extended"); break;
        case 0x64 : ProfileS=_T("High"); break;
        case 0x6E : ProfileS=_T("High 10"); break;
        case 0x7A : ProfileS=_T("High 4:2:2"); break;
        case 0x90 : ProfileS=_T("High 4:4:4"); break;
        default   : ProfileS.From_Number(Profile); break;
    }

    //Level
    Ztring LevelS;
    LevelS.From_Number(((float)Level)/10, 1); //Level is Value*10, can have one digit example 5.1

    //Continue only if not already done
    if (Count_Get(Stream_Video)>0)
        return;

    //Filling
    Stream_Prepare(Stream_General);
    Fill("Format", "AVC");
    Stream_Prepare(Stream_Video);
    Fill("Codec", "AVC");

    Fill("Codec_Profile", ProfileS+_T("@L")+LevelS);
    Fill("Width", (Width+1)*16);
    Fill("Height", (Height+1)*(2-frame_mbs_only_flag)*16);
}

//---------------------------------------------------------------------------
//
void File_Avc::pic_parameter_set()
{
    NAME("pic_parameter_set");
}

//---------------------------------------------------------------------------
//
void File_Avc::sei()
{
    NAME("sei");
}

//---------------------------------------------------------------------------
//
void File_Avc::access_unit_delimiter()
{
    NAME("access_unit_delimiter");

    int32u PicType;
    BEGIN
    GET ( 3, PicType,                                           primary_pic_type) INFO(Avc_PicType[PicType]);
}

//***************************************************************************
// Helpers
//***************************************************************************

//---------------------------------------------------------------------------
bool File_Avc::Element_Name_Get()
{
    //Enough data?
    if (Buffer_Offset+4>Buffer_Size)
        return false;

    BitStream BS(Buffer+Buffer_Offset+3, 1);
    ZERO_E( 1,                                                  forbidden_zero_bit)
    SKIP_E( 2,                                                  nal_ref_idc)
    GET_E ( 5, Element_Name,                                    nal_unit_type)

    return true;
}

//---------------------------------------------------------------------------
bool File_Avc::Element_Size_Get()
{
    Element_Next=Buffer_Offset+4;
    while(Element_Next+3<Buffer_Size && CC3(Buffer+Element_Next)!=0x000001)
        Element_Next++;
    if (Element_Next+3>=Buffer_Size)
    {
        //No next element
        if (File_Offset+Element_Next+3>=File_Size)
            Element_Next=Buffer_Size; //This is the last frame
        else if (Element_Next-Buffer_Offset>=65536) //False positive, no element should have more than this size
        {
            TRUSTED_ISNOT("Next element is too big, not normal");
            File_Offset=File_Size;
            return false;
        }
        else
            return false; //Other byte will come
    }
    Element_Size=Element_Next-Buffer_Offset-4;
    return true;
}

//***************************************************************************
// Information
//***************************************************************************

//---------------------------------------------------------------------------
void File_Avc::HowTo(stream_t StreamKind)
{
    switch (StreamKind)
    {
        case (Stream_General) :
            Fill_HowTo("Format", "R");
            break;
        case (Stream_Video) :
            Fill_HowTo("Codec", "R");
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

#endif //MEDIAINFO_AVC_YES
