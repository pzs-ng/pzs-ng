// File_Avc - Info for AVC Visual files
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
#if defined(MEDIAINFO_MPEG4V_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Video/File_Mpeg4v.h"
#include "ZenLib/BitStream.h"
#include "ZenLib/Utils.h"
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//---------------------------------------------------------------------------
//Constants
const size_t Frame_Count_Limit=16;

//---------------------------------------------------------------------------
const char* Mpeg4v_PictureStructure[]=
{
    "Interlaced",
    "TFF",
    "BFF",
    "PPF",
};

const char* Mpeg4v_Chroma[]=
{
    "",
    "4:2:0",
    "",
    "",
};

const char* Mpeg4v_visual_object_type[]=
{
    "",
    "video",
    "still texture",
    "mesh",
    "FBA",
    "3D mesh",
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

const char* Mpeg4v_verid[]=
{
    "",
    "ISO/IEC 14496-2",
    "ISO/IEC 14496-2 AMD 1",
    "ISO/IEC 14496-2 AMD 2",
    "ISO/IEC 14496-2 AMD 3 (Studio)",
    "ISO/IEC 14496-2 AMD 4 (SVP)",
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

/* *** helper macros *** */

/* turn a numeric literal into a hex constant
   (avoids problems with leading zeroes)
   8-bit constants max value 0x11111111, always fits in unsigned long
*/
#define HEX__(n) 0x##n##LU

/* 8-bit conversion function */
#define B8__(x) ((x&0x0000000FLU)?1:0)      \
               +((x&0x000000F0LU)?2:0)      \
               +((x&0x00000F00LU)?4:0)      \
               +((x&0x0000F000LU)?8:0)      \
               +((x&0x000F0000LU)?16:0)     \
               +((x&0x00F00000LU)?32:0)     \
               +((x&0x0F000000LU)?64:0)     \
               +((x&0xF0000000LU)?128:0)

/* *** user macros *** */

/* for upto 8-bit binary constants */
#define B8(d) ((unsigned char)B8__(HEX__(d)))

/* for upto 16-bit binary constants, MSB first */
#define B16(dmsb,dlsb) (((unsigned short)B8(dmsb)<<8)     \
                        + B8(dlsb))

/* for upto 32-bit binary constants, MSB first */
#define B32(dmsb,db2,db3,dlsb) (((unsigned long)B8(dmsb)<<24)      \
                                  + ((unsigned long)B8(db2)<<16) \
                                  + ((unsigned long)B8(db3)<<8)    \
                                  + B8(dlsb))

const char* Mpeg4v_Profile_Level(int32u Profile_Level)
{
    switch (Profile_Level)
    {
        case B8(00000001) : return "Simple Profile/Level 1";
        case B8(00000010) : return "Simple Profile/Level 2";
        case B8(00000011) : return "Simple Profile/Level 3";
        case B8(00010001) : return "Simple Scalable Profile/Level 1";
        case B8(00010010) : return "Simple Scalable Profile/Level 2";
        case B8(00100001) : return "Core Profile/Level 1";
        case B8(00100010) : return "Core Profile/Level 2";
        case B8(00110010) : return "Main Profile/Level 2";
        case B8(00110011) : return "Main Profile/Level 3";
        case B8(00110100) : return "Main Profile/Level 4";
        case B8(01000010) : return "N-bit Profile/Level 2";
        case B8(01010001) : return "Scalable Texture Profile/Level 1";
        case B8(01100001) : return "Simple Face Animation Profile/Level 1";
        case B8(01100010) : return "Simple Face Animation Profile/Level 2";
        case B8(01100011) : return "Simple FBA Profile/Level 1";
        case B8(01100100) : return "Simple FBA Profile/Level 2";
        case B8(01110001) : return "Basic Animated Texture Profile/Level 1";
        case B8(01110010) : return "Basic Animated Texture Profile/Level 2";
        case B8(10000001) : return "Hybrid Profile/Level 1";
        case B8(10000010) : return "Hybrid Profile/Level 2";
        case B8(10010001) : return "Advanced Real Time Simple Profile/Level 1";
        case B8(10010010) : return "Advanced Real Time Simple Profile/Level 2";
        case B8(10010011) : return "Advanced Real Time Simple Profile/Level 3";
        case B8(10010100) : return "Advanced Real Time Simple Profile/Level 4";
        case B8(10100001) : return "Core Scalable Profile/Level 1";
        case B8(10100010) : return "Core Scalable Profile/Level 2";
        case B8(10100011) : return "Core Scalable Profile/Level 3";
        case B8(10110001) : return "Advanced Coding Efficiency Profile/Level 1";
        case B8(10110010) : return "Advanced Coding Efficiency Profile/Level 2";
        case B8(10110011) : return "Advanced Coding Efficiency Profile/Level 3";
        case B8(10110100) : return "Advanced Coding Efficiency Profile/Level 4";
        case B8(11000001) : return "Advanced Core Profile/Level 1";
        case B8(11000010) : return "Advanced Core Profile/Level 2";
        case B8(11010001) : return "Advanced Scalable Texture/Level 1";
        case B8(11010010) : return "Advanced Scalable Texture/Level 2";
        case B8(11010011) : return "Advanced Scalable Texture/Level 3";
        case B8(11100001) : return "Simple Studio Profile/Level 1";
        case B8(11100010) : return "Simple Studio Profile/Level 2";
        case B8(11100011) : return "Simple Studio Profile/Level 3";
        case B8(11100100) : return "Simple Studio Profile/Level 4";
        case B8(11100101) : return "Core Studio Profile/Level 1";
        case B8(11100110) : return "Core Studio Profile/Level 2";
        case B8(11100111) : return "Core Studio Profile/Level 3";
        case B8(11101000) : return "Core Studio Profile/Level 4";
        case B8(11110001) : return "Simple Streaming Video Profile/Level 1";
        case B8(11110010) : return "Simple Streaming Video Profile/Level 2";
        case B8(11110011) : return "Simple Streaming Video Profile/Level 3";
        case B8(11110100) : return "Simple Streaming Video Profile/Level 4";
        case B8(11110101) : return "Streaming Video Profile/Level 1";
        case B8(11110110) : return "Streaming Video Profile/Level 2";
        case B8(11110111) : return "Streaming Video Profile/Level 3";
        case B8(11111000) : return "Streaming Video Profile/Level 4";
        case B8(11111111) : return "None";
        default :           return "Unknown";
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
Ztring Mpeg4v_Quant_Mat_Read(BitStream &BS);
//---------------------------------------------------------------------------

//***************************************************************************
// Format
//***************************************************************************

//---------------------------------------------------------------------------
void File_Mpeg4v::Read_Buffer_Init()
{
    IVOP_Count=0;
    PVOP_Count=0;
    BVOP_Count=0;
    SVOP_Count=0;
    NVOP_Count=0;
    Chroma=Error;
    Standard=Error;
    PictureStructure=Error;
    interlaced=Error;
    newpred_enable=Error;
    time_size=Error;
    reduced_resolution_vop_enable=Error;
    shape=Error;
    sprite_enable=Error;
    scalability=Error;
    enhancement_type=Error;
    complexity_estimation_disable=Error;
    RIFF_VOP_Count=0;
    RIFF_VOP_Count_Max=0;
    Frame_Count=0;
}

//---------------------------------------------------------------------------
void File_Mpeg4v::Read_Buffer_Continue()
{
    //Integrity
    if (File_Offset==0 && Detect_NonMPEG4V())
    {
        File_Offset=File_Size;
        return;
    }

    //Look for first Sync word
    while (Buffer_Offset+3<=Buffer_Size && CC3(Buffer+Buffer_Offset)!=0x000001)
        Buffer_Offset++;
    if (Buffer_Offset+3>Buffer_Size)
        return;

    //We want to count the number of VOP in this bufffer
    RIFF_VOP_Count=0;

    //Parse
    while (Buffer_Parse());
}

//---------------------------------------------------------------------------
void File_Mpeg4v::Read_Buffer_Finalize()
{
    //Libray
    if (!Library.empty())
        Fill("Encoded_Library", Library);
}

//***************************************************************************
// Buffer
//***************************************************************************

//---------------------------------------------------------------------------
bool File_Mpeg4v::Buffer_Parse()
{
    //Enough data?
    if (Buffer_Offset+4>Buffer_Size)
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

    //Next Frame
    Buffer_Offset=Element_Next;
    return true;
}

//---------------------------------------------------------------------------
// Element parse
//
bool File_Mpeg4v::Element_Parse()
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
        case 0xB0: visual_object_sequence_start(); break;
        case 0xB1: visual_object_sequence_end(); break;
        case 0xB2: user_data_start(); break;
        case 0xB3: group_of_vop_start(); break;
        case 0xB4: video_session_error(); break;
        case 0xB5: visual_object_start(); break;
        case 0xB6: vop_start(); break;
        case 0xB7: slice_start(); break;
        case 0xB8: extension_start(); break;
        case 0xB9: fgs_vop_start(); break;
        case 0xBA: fba_object_start(); break;
        case 0xBB: fba_object_plane_start(); break;
        case 0xBC: mesh_object_start(); break;
        case 0xBD: mesh_object_plane_start(); break;
        case 0xBE: still_texture_object_start(); break;
        case 0xBF: texture_spatial_layer_start(); break;
        case 0xC0: texture_snr_layer_start(); break;
        case 0xC1: texture_tile_start(); break;
        case 0xC2: texture_shape_layer_start(); break;
        default:
                 if (Element_Name<=0x1F) video_object_start();
            else if (Element_Name>=0x20
                  && Element_Name<=0x2F) video_object_layer_start();
            else if (Element_Name>=0x40
                  && Element_Name<=0x4F) fgs_bp_start();
            else if (Element_Name>=0xC6) system_start();
            else
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

#define INTEGRITY_GENERAL() \
    if (Count_Get(Stream_General)==0) \
    { \
        TRUSTED_ISNOT("Header is missing"); \
        return; \
    } \

#define INTEGRITY(ELEMENT_SIZE) \
    if (Element_Size!=ELEMENT_SIZE) \
    { \
        TRUSTED_ISNOT("Size is wrong"); \
        return; \
    } \

#define DETAIL(DETAIL) \
    if (Config.Details_Get()) \
    { \
        Details_Add_Info(BS.Offset_Get(), #DETAIL, DETAIL); \
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
// Packet "00"
//
void File_Mpeg4v::video_object_start()
{
    NAME("video_object_start")
    NEXTFRAME_CLEAR();

    //Reading
    BEGIN
    END

    //Filling
    if (Count_Get(Stream_General)==0)
        Stream_Prepare(Stream_General);
}

//---------------------------------------------------------------------------
// Packet "20"
//
void File_Mpeg4v::video_object_layer_start()
{
    NAME("video_object_layer_start")
    NEXTFRAME_CLEAR();

    //Integrity
    if (Element_Size>512)
    {
        Element_Size=Error;
        return;
    }

    //Analyzing
    size_t Width=0;
    size_t Height=0;
    size_t PixelAspectRatio_Width=0;
    size_t PixelAspectRatio_Height=0;
    size_t QPel=0;
    size_t GMC=0;
    size_t BVOP=0;
    size_t load_intra_quant_mat=0;
    size_t load_nonintra_quant_mat=0;
    size_t load_intra_quant_mat_grayscale=0;
    size_t load_nonintra_quant_mat_grayscale=0;

    BitStream BS(Buffer+Buffer_Offset, Element_Size);
    BS.Skip(1);                                     //random_accessible_vol
    BS.Skip(8);                                     //object_type_id
    size_t verid=1;
    if (BS.Get(1))                                  //is_object_layer_id
    {
        verid=BS.Get(4);                            //object_layer_verid
        BS.Skip(3);                                 //object_layer_priority
    }
    DETAIL(verid) INFO(Mpeg4v_verid[verid]);
    size_t PixelAspectRatio=BS.Get(4);              //aspect_ratio_info
    DETAIL(PixelAspectRatio)
    if (PixelAspectRatio==0x0F)
    {
        PixelAspectRatio_Width=BS.Get(8);           //par_width
        PixelAspectRatio_Height=BS.Get(8);          //par_height
        DETAIL(PixelAspectRatio_Width)
        DETAIL(PixelAspectRatio_Height)
    }
    if (BS.Get(1))                                  //vol_control_parameters
    {
        Chroma=BS.Get(2);                           //chroma_format
        DETAIL(Chroma)
        BVOP=!BS.Get(1);                            //low_delay
        DETAIL(BVOP)
        if (BS.Get(1))                              //vbv_parameters
        {
            BS.Skip(15);                            //first_half_bit_rate
            BS.Skip(1);                             //marker_bit
            BS.Skip(15);                            //latter_half_bit_rate
            BS.Skip(1);                             //marker_bit
            BS.Skip(15);                            //first_half_vbv_Element_Size
            BS.Skip(1);                             //marker_bit
            BS.Skip(3);                             //latter_half_vbv_Element_Size
            BS.Skip(11);                            //first_half_vbv_occupancy
            BS.Skip(1);                             //marker_bit
            BS.Skip(15);                            //latter_half_vbv_occupancy
            BS.Skip(1);                             //marker_bit
        }
    }
    shape=BS.Get(2);                                //object_layer_shape
    DETAIL(shape)
    size_t shape_extension=0;
    if (shape==3 && verid!=1) //Shape=GrayScale
        shape_extension=BS.Get(4);                  //object_layer_shape_extension
    DETAIL(shape_extension)
    size_t aux_comp_count=0;
    DETAIL(aux_comp_count)
    if (shape_extension==0 && shape_extension==1 && shape_extension==5 && shape_extension==7 && shape_extension==8) aux_comp_count=1;
    if (shape_extension==2 && shape_extension==3 && shape_extension==6 && shape_extension==9 && shape_extension==11) aux_comp_count=2;
    if (shape_extension==4 && shape_extension==10 && shape_extension==12) aux_comp_count=3;
    BS.Skip(1);                                     //marker_bit
    size_t Increment=BS.Get(16);                    //vop_time_increment_resolution
    DETAIL(Increment)
    size_t PowerOf2=1;
    for (time_size=0; time_size<=16; time_size++)
    {
        if (Increment<PowerOf2)
            break;
        PowerOf2<<=1;
    }
    DETAIL(PowerOf2)
    BS.Skip(1);                                     //marker_bit
    if (BS.Get(1))                                  //fixed_vop_rate
        BS.Skip(time_size);                         //fixed_vop_time increment
    size_t Resolution=8;
    if (shape!=2) //Shape!=BinaryOnly
    {
        if (shape==0) //Shape=Rectangular
        {
            BS.Skip(1);                             //marker_bit
            Width=BS.Get(13);                       //object_layer_width
            DETAIL(Width)
            BS.Skip(1);                             //marker_bit
            Height=BS.Get(13);                      //object_layer height
            DETAIL(Height)
            BS.Skip(1);                             //marker_bit
        }
        interlaced=BS.Get(1);                       //interlaced
        DETAIL(interlaced)
        if (interlaced)
            PictureStructure=0;
        else
            PictureStructure=3;
        DETAIL(PictureStructure)
        BS.Skip(1);                                 //obmc_disable
        if (verid==1)
            sprite_enable=BS.Get(1);                //sprite_enable
        else
            sprite_enable=BS.Get(2);                //sprite_enable
        DETAIL(sprite_enable)
        if (sprite_enable==1 || sprite_enable==2 )  //static or GMC
        {
            if (sprite_enable!=2) //No GMC
            {
                BS.Skip(13);                        //sprite_width
                BS.Skip(1);                         //marker_bit
                BS.Skip(13);                        //sprite_height
                BS.Skip(1);                         //marker_bit
                BS.Skip(13);                        //sprite_top_coordinate
                BS.Skip(1);                         //marker_bit
                BS.Skip(13);                        //sprite_left_coordinate
                BS.Skip(1);                         //marker_bit
            }
            GMC=BS.Get(6);                          //no_of_sprite_warping_points
            DETAIL(GMC)
            BS.Skip(2);                             //sprite_warping_accuracy
            BS.Skip(1);                             //sprite_brightness_change
            if (sprite_enable!=2) //No GMC
                BS.Skip(1);                         //low_latency_sprite_enable
        }
        if (verid==1 && shape!=0) //Shape!=Rectangular
            BS.Skip(1);                             //sadct_disable
        if (BS.Get(1))                              //bits_per_pixel not_8_bit
        {
            BS.Skip(4);                             //quant_precision
            Resolution=BS.Get(4);                   //bits_per_pixel
            DETAIL(Resolution)
        }
        if (shape==3) //Shape=GrayScale
        {
            BS.Skip(1);                             //no_gray_quant_update
            BS.Skip(1);                             //composition_method
            BS.Skip(1);                             //linear_composition
        }
        size_t quant_type=BS.Get(1);                //quant_type
        DETAIL(quant_type)
        if (quant_type)
        {
            load_intra_quant_mat=BS.Get(1);         //load_intra_quant_mat
            DETAIL(load_intra_quant_mat)
            if (load_intra_quant_mat)
            {
                size_t Pos;
                for (Pos=0; Pos<64; Pos++)
                    if (BS.Get(8)==0)               //intra_quant_mat
                        Pos=Error-1;
            }
            load_nonintra_quant_mat=BS.Get(1);      //load_nonintra_quant_mat
            DETAIL(load_nonintra_quant_mat)
            if (load_nonintra_quant_mat)
            {
                size_t Pos;
                for (Pos=0; Pos<64; Pos++)
                    if (BS.Get(8)==0)               //nonintra_quant_mat
                        Pos=Error-1;
            }
            if(shape==3) //Shape=GrayScale
            {
                for(size_t Pos=0; Pos<aux_comp_count; Pos++)
                {
                    load_intra_quant_mat_grayscale=BS.Get(1);//load_intra_quant_mat_grayscale
                    DETAIL(load_intra_quant_mat_grayscale)
                    if (load_intra_quant_mat_grayscale)
                    {
                        size_t Pos;
                        for (Pos=0; Pos<64; Pos++)
                            if (BS.Get(8)==0)       //intra_quant_mat_grayscale
                                Pos=Error-1;
                    }
                    load_nonintra_quant_mat_grayscale=BS.Get(1);//load_nonintra_quant_mat_grayscale
                    DETAIL(load_nonintra_quant_mat_grayscale)
                    if (load_nonintra_quant_mat_grayscale)
                    {
                        size_t Pos;
                        for (Pos=0; Pos<64; Pos++)
                            if (BS.Get(8)==0)       //nonintra_quant_mat_grayscale
                                Pos=Error-1;
                    }
                }
            }
        }
        if (verid!=1)
        {
            QPel=BS.Get(1);                         //quarter_sample
            DETAIL(QPel)
        }
        complexity_estimation_disable=BS.Get(1);    //complexity_estimation_disable
        DETAIL(complexity_estimation_disable)
        if (!complexity_estimation_disable)
        {
            size_t estimation_method=BS.Get(2);     //estimation_method
            DETAIL(estimation_method)
            if (estimation_method==0 || estimation_method==1)
            {
                size_t shape_complexity_estimation_disable=BS.Get(1); //shape_complexity_estimation_disable
                DETAIL(shape_complexity_estimation_disable)
                if (shape_complexity_estimation_disable)
                {
                    BS.Skip(1);                     //opaque
                    BS.Skip(1);                     //transparent
                    BS.Skip(1);                     //intra_cae
                    BS.Skip(1);                     //inter_cae
                    BS.Skip(1);                     //no_update
                    BS.Skip(1);                     //upsampling
                }
                size_t texture_complexity_estimation_set_1_disable=BS.Get(1); //texture_complexity_estimation_set_1_disable
                DETAIL(texture_complexity_estimation_set_1_disable)
                if (texture_complexity_estimation_set_1_disable)
                {
                    BS.Skip(1);                     //intra_blocks
                    BS.Skip(1);                     //inter_blocks
                    BS.Skip(1);                     //inter4v_blocks
                    BS.Skip(1);                     //not_coded_blocks
                }
                BS.Skip(1);                         //marker_bit
                size_t texture_complexity_estimation_set_2_disable=BS.Get(1); //texture_complexity_estimation_set_2_disable
                DETAIL(texture_complexity_estimation_set_2_disable)
                if (texture_complexity_estimation_set_2_disable)
                {
                    BS.Skip(1);                     //dct_coefs
                    BS.Skip(1);                     //dct_lines
                    BS.Skip(1);                     //vlc_symbols
                    BS.Skip(1);                     //vlc_bits
                }
                size_t motion_compensation_complexity_disable=BS.Get(1); //motion_compensation_complexity_disable
                DETAIL(motion_compensation_complexity_disable)
                if (motion_compensation_complexity_disable)
                {
                    BS.Skip(1);                     //apm
                    BS.Skip(1);                     //npm
                    BS.Skip(1);                     //interpolate_mc_q
                    BS.Skip(1);                     //forw_back_mc_q
                    BS.Skip(1);                     //halfpel2
                    BS.Skip(1);                     //halfpel4
                }
                BS.Skip(1);                         //marker_bit
                if (estimation_method==1)
                {
                    size_t version2_complexity_estimation_disable=BS.Get(1); //version2_complexity_estimation_disable
                    DETAIL(version2_complexity_estimation_disable)
                    if (version2_complexity_estimation_disable)
                    {
                        BS.Skip(1);                 //sadct
                        BS.Skip(1);                 //quarterpel
                    }
                }
            }
        }
        BS.Skip(1);                                 //resync_marker_disable
        if (BS.Get(1)==1)                           //data_partitioned
            BS.Skip(1);                             //reversible_vlc
        if (verid!=1)
        {
            newpred_enable=BS.Get(1);               //newpred_enable
            DETAIL(newpred_enable)
            if (newpred_enable)
            {
                BS.Skip(2);                         //requested_upstream_message_type
                BS.Skip(1);                         //newpred_segment_type
            }
            reduced_resolution_vop_enable=BS.Get(1);//reduced_resolution_vop_enable
            DETAIL(reduced_resolution_vop_enable)
        }
        scalability=BS.Get(1);                      //scalability
        DETAIL(scalability)
        if (scalability==1)
        {
            size_t Hierarchy=BS.Get(1);             //hierarchy_type
            DETAIL(Hierarchy)
            BS.Skip(4);                             //ref_layer_id
            BS.Skip(1);                             //ref_layer_sampling_direc
            BS.Skip(5);                             //hor_sampling_factor_n
            BS.Skip(5);                             //hor_sampling_factor_m
            BS.Skip(5);                             //vert_sampling_factor_n
            BS.Skip(5);                             //vert_sampling_factor_m
            enhancement_type=BS.Get(1);             //enhancement_type
            DETAIL(enhancement_type)
            if (shape==1 && Hierarchy==0) //Shape=Binary
            {
                BS.Skip(1);                         //use_ref_shape
                BS.Skip(1);                         //use_ref_texture
                BS.Skip(5);                         //shape_hor_sampling_factor_n
                BS.Skip(5);                         //shape_hor_sampling_factor_m
                BS.Skip(5);                         //shape_vert_sampling_factor_n
                BS.Skip(5);                         //shape_vert_sampling_factor_m
            }
        }
    }
    else
    {
        if (verid!=1)
        {
            if (BS.Get(1)==1)                       //scalability
            {
                BS.Skip(4);                         //ref_layer_id
                BS.Skip(5);                         //shape_hor_sampling_factor_n
                BS.Skip(5);                         //shape_hor_sampling_factor_m
                BS.Skip(5);                         //shape_vert_sampling_factor_n
                BS.Skip(5);                         //shape_vert_sampling_factor_m
            }
        }
        BS.Skip(1);                                 //resync_marker_disable
        //TODO: ...
    }

    //Coherancy test
    //BS.Byte_Align();
    //Element_Size=BS.Offset_Get(); //Not possible because all stream analysis is not done

    //Coherancy
    if (Width==0 || Height==0 || Width/Height<0.2 || Width/Height>5)
    {
        Element_Size=0;
        return;
    }

    //Continue only if not already done
    if (Count_Get(Stream_Video)>0)
        return;

    //Filling
    if (Count_Get(Stream_General))
        Stream_Prepare(Stream_General);
    Fill("Codec", "MPEG-4V");
    Stream_Prepare(Stream_Video);
    Fill("Codec", "MPEG-4V");

    if (Height)
    {
        Fill("Width", Width);
        Fill("Height", Height);
        float PixelAspectRatio_Value=1;
             if (PixelAspectRatio==0x01) PixelAspectRatio=1;
             if (PixelAspectRatio==0x02) PixelAspectRatio=12/11;
        else if (PixelAspectRatio==0x03) PixelAspectRatio=10/11;
        else if (PixelAspectRatio==0x04) PixelAspectRatio=16/11;
        else if (PixelAspectRatio==0x05) PixelAspectRatio=40/13;
        else if (PixelAspectRatio==0x0F && PixelAspectRatio_Height) PixelAspectRatio_Value=((float)PixelAspectRatio_Width)/PixelAspectRatio_Height;
        Fill("AspectRatio", ((float)Width)/Height*PixelAspectRatio_Value);
    }
    Fill("Resolution", Resolution);
    if (Chroma<4)
        Fill("Chroma", Mpeg4v_Chroma[Chroma]);
    if (PictureStructure<4)
        Fill("Interlacement", Mpeg4v_PictureStructure[PictureStructure]);
    if (BVOP)
    {
        Fill("Codec_Settings", "BVOP");
        Fill("Codec_Settings/BVOP", "Yes");
    }
    else
        Fill("Codec_Settings/BVOP", "No");
    if (GMC)
    {
        Fill("Codec_Settings", Ztring(_T("GMC"))+Ztring::ToZtring(GMC));
        Fill("Codec_Settings/GMC", GMC);
    }
    else
        Fill("Codec_Settings/GMC", 0);
    if (QPel)
    {
        Fill("Codec_Settings", "QPel");
        Fill("Codec_Settings/QPel", "Yes");
    }
    else
        Fill("Codec_Settings/QPel", "No");
    if (load_intra_quant_mat_grayscale || load_nonintra_quant_mat_grayscale)
    {
        Fill("Codec_Settings", "Custom Matrix (Gray)");
        Fill("Codec_Settings/Matrix", "Custom (Gray)");
    }
    else if (load_intra_quant_mat || load_nonintra_quant_mat)
    {
        Fill("Codec_Settings", "Custom Matrix");
        Fill("Codec_Settings/Matrix", "Custom");
    }
    else
        Fill("Codec_Settings/Matrix", "Default");
}

//---------------------------------------------------------------------------
// Packet "40"
//
void File_Mpeg4v::fgs_bp_start()
{
    NAME("fgs_bp_start")
    NEXTFRAME_CLEAR();
}

//---------------------------------------------------------------------------
// Packet "B0"
void File_Mpeg4v::visual_object_sequence_start()
{
    NAME("visual_object_sequence_start");
    NEXTFRAME_CLEAR();

    //Reading
    int32u Profile_Level;
    BEGIN
    GET (8, Profile_Level,                                      profile_and_level_indication) INFO(Mpeg4v_Profile_Level(Profile_Level));
    END
}

//---------------------------------------------------------------------------
// Packet "B1"
void File_Mpeg4v::visual_object_sequence_end()
{
    NAME("visual_object_sequence_end");
    NEXTFRAME_CLEAR();
    INTEGRITY_GENERAL()
}

//---------------------------------------------------------------------------
// Packet "B2", User defined size, this is often used of library name
// Data                             X bytes, Pos=0
//
void File_Mpeg4v::user_data_start()
{
    NAME("user_data_start");
    NEXTFRAME_CLEAR();
    INTEGRITY_GENERAL()

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
    Ztring Temp;
    Temp.From_Local((const char*)Buffer+Buffer_Offset+Library_Offset, Library_Size);

    //Special cases
         if (Temp.find(_T("build"))==0) Library+=Ztring(_T(" "))+Temp;
    else Library=Temp;

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(Buffer_Offset, "Library", Library);
    }

}

//---------------------------------------------------------------------------
// Packet "B3"
void File_Mpeg4v::group_of_vop_start()
{
    NAME("group_of_vop_start");
    NEXTFRAME_CLEAR();
    INTEGRITY_GENERAL()
}

//---------------------------------------------------------------------------
// Packet "B4"
//
void File_Mpeg4v::video_session_error()
{
    NAME("video_session_error");
    NEXTFRAME_CLEAR();
}

//---------------------------------------------------------------------------
// Packet "B5"
//
void File_Mpeg4v::visual_object_start()
{
    NAME("visual_object_start");
    NEXTFRAME_CLEAR();

    //Reading
    int32u FrameType, VersionID;
    BEGIN
    TEST_SKIP( 1,                                               is_visual_object_identifier)
        GET ( 4, VersionID,                                     visual_object_verid)  INFO(Mpeg4v_verid[VersionID]);
        SKIP( 3,                                                visual_object_priority)
        TEST_END
    GET ( 4, FrameType,                                         visual_object_type) INFO(Mpeg4v_visual_object_type[FrameType]);
    if (FrameType==1)
    {
        TEST_SKIP(1,                                            video_signal_type)
            SKIP(3,                                             video_format)
            SKIP(1,                                             video_range)
            TEST_SKIP(1,                                        colour_description)
                SKIP(8,                                         colour_primaries)
                SKIP(8,                                         transfer_characteristics)
                SKIP(8,                                         matrix_coefficients)
            TEST_END
        TEST_END
        END //Other FrameType need some work
    }
}

//---------------------------------------------------------------------------
// Packet "B6"
//
void File_Mpeg4v::vop_start()
{
    NAME("vop_start");
    NEXTFRAME_CLEAR();
    INTEGRITY_GENERAL()
    Frame_Count++;

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Element(Ztring(_T("Frame ")+Ztring::ToZtring(Frame_Count)));
    }

    BitStream BS(Buffer+Buffer_Offset, Element_Size);
    size_t vop_coding_type=BS.Get(2);               //vop_coding_type
    size_t modulo_time_base=0;
    while (BS.Get(1)==1)
        modulo_time_base++;                         //modulo_time_base
    BS.Skip(1);                                     //marker_bit
    BS.Get(time_size);                              //time_increment
    BS.Skip(1);                                     //marker_bit
    size_t vop_coded=BS.Get(1);                     //vop_coding
    if (vop_coded)
    {
        if (newpred_enable)
        {
            BS.Skip(time_size);                     //vop_id
            if (BS.Get(1)==1)                       //vop_id_for_prediction_indication
                BS.Skip(time_size);                 //vop_id_for_prediction
            BS.Skip(1);                             //marker_bit
        }

        if (shape!=2              //Shape!=BinaryOnly
         && (vop_coding_type==1   //Type=P
          || (vop_coding_type==3  //Type=S
           && sprite_enable==2))) //Sprite_Enable=GMC
            BS.Skip(1);
        if (reduced_resolution_vop_enable==1
         && shape==0                         //Shape=Rectangular
         && (vop_coding_type==1              //Type=P
          || vop_coding_type==0))            //Type=I
            BS.Skip(1);
        if (shape!=0) //Shape!=Rectangular
        {
            if (sprite_enable==1    //Sprite_Enable=Static
             && vop_coding_type==0) //Type=I
            {
                BS.Skip(13);                        //vop_width
                BS.Skip(1);                            //marker_bit
                BS.Skip(13);                        //vop_height
                BS.Skip(1);                            //marker_bit
                BS.Skip(13);                        //vop_horizontal_mc_spatial_ref
                BS.Skip(1);                            //marker_bit
                BS.Skip(13);                        //vop_vertical_mc_spatial_ref
                BS.Skip(1);                            //marker_bit
            }
            if (shape!=2             //Shape=BinaryOnly
             && scalability==1
             && enhancement_type==1)
                BS.Skip(1);                         //background_composition
            BS.Skip(1);                             //change_conv_ratio_disable
            if (BS.Get(1)==1)                       //vop_constant_alpha
                BS.Skip(8);                         //vop_constant_alpha_value
        }

        if (shape!=2) //Shape=BinaryOnly
            if (complexity_estimation_disable==0)
                return; //TODO: parse it, but this value is not common
        if (shape!=2) //Shape=BinaryOnly
        {
            BS.Skip(3);                             //intra_dc_vlc_thr
            if (interlaced)
            {
                if (BS.Get(1)==1)                   //top_field_first
                    PictureStructure=1; //Top first
                else
                    PictureStructure=2; //Bottom first
                BS.Skip(1);                         //alternate_vertical_scan_flag
            }
        }
        //...
    }

    if (!vop_coded)              NVOP_Count++; //VOP with no data
    else if (vop_coding_type==0) IVOP_Count++; //Type I
    else if (vop_coding_type==1) PVOP_Count++; //Type P
    else if (vop_coding_type==2) BVOP_Count++; //Type B
    else if (vop_coding_type==3) SVOP_Count++; //Type S

    //Specific
    if (CompleteFileName.empty())
    {
        RIFF_VOP_Count++;
        if (RIFF_VOP_Count>RIFF_VOP_Count_Max)
            RIFF_VOP_Count_Max=RIFF_VOP_Count;
    }

    //Jumping to the end of the file
    if (Frame_Count>Frame_Count_Limit-1)
    {
        //Details
        if (Config.Details_Get())
        {
            Details_Add_Error("-------------------------------------------");
            Details_Add_Error("---   MPEG-4V, Jumping to end of file   ---");
            Details_Add_Error("-------------------------------------------");
        }

        //Jumping
        File_GoTo=File_Size;
    }
}

//---------------------------------------------------------------------------
// Packet "B7"
//
void File_Mpeg4v::slice_start()
{
    NAME("slice_start");
    NEXTFRAME_CLEAR();
}

//---------------------------------------------------------------------------
// Packet "B8"
//
void File_Mpeg4v::extension_start()
{
    NAME("extension_start");
    NEXTFRAME_CLEAR();
}

//---------------------------------------------------------------------------
// Packet "B9"
//
void File_Mpeg4v::fgs_vop_start()
{
    NAME("fgs_vop_start");
    NEXTFRAME_CLEAR();
}

//---------------------------------------------------------------------------
// Packet "BA"
//
void File_Mpeg4v::fba_object_start()
{
    NAME("fba_object_start");
    NEXTFRAME_CLEAR();
}

//---------------------------------------------------------------------------
// Packet "BB"
//
void File_Mpeg4v::fba_object_plane_start()
{
    NAME("fba_object_plane_start");
    NEXTFRAME_CLEAR();
}

//---------------------------------------------------------------------------
// Packet "BC"
//
void File_Mpeg4v::mesh_object_start()
{
    NAME("mesh_object_start");
    NEXTFRAME_CLEAR();
}

//---------------------------------------------------------------------------
// Packet "BD"
//
void File_Mpeg4v::mesh_object_plane_start()
{
    NAME("mesh_object_plane_start");
    NEXTFRAME_CLEAR();
}

//---------------------------------------------------------------------------
// Packet "BE"
//
void File_Mpeg4v::still_texture_object_start()
{
    NAME("still_texture_object_start");
    NEXTFRAME_CLEAR();
}

//---------------------------------------------------------------------------
// Packet "BF"
//
void File_Mpeg4v::texture_spatial_layer_start()
{
    NAME("texture_spatial_layer_start");
    NEXTFRAME_CLEAR();
}

//---------------------------------------------------------------------------
// Packet "C0"
//
void File_Mpeg4v::texture_snr_layer_start()
{
    NAME("texture_snr_layer_start");
    NEXTFRAME_CLEAR();
}

//---------------------------------------------------------------------------
// Packet "C1"
//
void File_Mpeg4v::texture_tile_start()
{
    NAME("texture_tile_start");
    NEXTFRAME_CLEAR();
}

//---------------------------------------------------------------------------
// Packet "C2"
//
void File_Mpeg4v::texture_shape_layer_start()
{
    NAME("texture_shape_layer_start");
    NEXTFRAME_CLEAR();
}

//---------------------------------------------------------------------------
// Packet "C3"
//
void File_Mpeg4v::system_start()
{
    NAME("system_start");
    NEXTFRAME_CLEAR();
}

//***************************************************************************
// Helpers
//***************************************************************************

//---------------------------------------------------------------------------
bool File_Mpeg4v::Detect_NonMPEG4V ()
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
bool File_Mpeg4v::Element_Size_Get()
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
            //Details
            if (Config.Details_Get())
            {
                Details_Level_Last--;
                Details_Add_Info(Error, "Next element not found!", Element_Next-Buffer_Offset);
                Details_Level_Last++;
            }

            File_Offset=File_Size;
            return false;
        }
        else
            return false; //Other byte will come
    }
    //Filling
    Element_Size=Element_Next-Buffer_Offset-4;

    //Testing
    if (Element_Size==0) //Not normal
        TRUSTED_ISNOT("Next element is too small, not normal");

    return true;
}

//---------------------------------------------------------------------------
bool File_Mpeg4v::Element_Name_IsOK()
{
    //Is it the start of a MPEG-4V?
    if (Count_Get(Stream_General)==0
     && !(Element_Name==0x00
       || Element_Name==0xB0
       || Element_Name==0xB5)
    )
    {
        //MPEG-4V didn't begin, skipping
        //Details
        if (Config.Details_Get()>0.9)
        {
            Details_Add_Element(0, Element_Name, Element_Size-4);
            Details_Add_Element("Has not began, skipping");
        }
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------
//"xxx_quant_mat" reading
Ztring Mpeg4v_Quant_Mat_Read(BitStream &BS)
{
    Ztring ToReturn; ToReturn.reserve(64);
    int32u Value;
    size_t Pos=0;

    //Reading from BitStream
    do
    {
        Value=BS.Get(8);
        ToReturn+=Ztring::ToZtring(Value, 16);
        Pos++;
    }
    while (Pos<64);

    //Completing with automatic values

    //Returning
    ToReturn.MakeUpperCase();
    return ToReturn;
}

//***************************************************************************
// Information
//***************************************************************************

//---------------------------------------------------------------------------
void File_Mpeg4v::HowTo(stream_t StreamKind)
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

//***************************************************************************
// C++
//***************************************************************************

} //NameSpace

#endif //MEDIAINFO_MPEG4V_*
