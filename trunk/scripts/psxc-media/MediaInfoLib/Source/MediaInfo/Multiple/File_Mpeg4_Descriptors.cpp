// File_Mpeg4 - Info for MPEG-4 files
// Copyright (C) 2005-2007 Jerome Martinez, Zen@MediaArea.net
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
// Information about MPEG-4 files
// Descriptors part
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
// Compilation condition
#include <MediaInfo/Setup.h>
#if defined(MEDIAINFO_MPEG4_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Multiple/File_Mpeg4.h"
#if defined(MEDIAINFO_MPEG4V_YES)
    #include "MediaInfo/Video/File_Mpeg4v.h"
#endif
#if defined(MEDIAINFO_AAC_YES)
    #include "MediaInfo/Audio/File_Aac.h"
#endif
#include "ZenLib/Utils.h"
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Format
//***************************************************************************

//---------------------------------------------------------------------------
// Descriptors
//
void File_Mpeg4::Descriptors(size_t Esds_Offset, size_t Esds_Size)
{
    while (Esds_Size>0)
    {
        //Integrity
        if (Esds_Size<2) //Size of atom
            return;

        //Type
        int8u Descriptor_Type=BigEndian2int8u(Buffer+Buffer_Offset+Esds_Offset);

        Esds_Offset++;
        Esds_Size--;

        //Size
        size_t Size=0;
        int8u Size_ToAdd;
        int8u Size_Bytes=0;
        do
        {
            Size_ToAdd=BigEndian2int8u(Buffer+Buffer_Offset+Esds_Offset+Size_Bytes);
            Size=(Size<<7) | (Size_ToAdd&0x7F);
            Size_Bytes++;
        }
        while ((Size_ToAdd&0x80) && Size_Bytes<4);
        Esds_Offset+=Size_Bytes;
        Esds_Size-=Size_Bytes;

        //Corrupted sizes
        if (Size>Esds_Size)
            Size=Esds_Size;

        //Details
        if (Config.Details_Get())
        {
            Element_Level++;
            Buffer_Offset+=Esds_Offset-Size_Bytes;
            Details_Add_Element(Element_Level, Ztring::ToZtring(Descriptor_Type, 16), Size);
            Buffer_Offset-=Esds_Offset-Size_Bytes;
        }

        //Parsing
            switch (Descriptor_Type)
        {
            case  0x3 : Descriptors_ES          (Esds_Offset, Size); break;
            case  0x4 : Descriptors_DecConfig   (Esds_Offset, Size); break;
            case  0x5 : Descriptors_DecSpecific (Esds_Offset, Size); break;
            case  0x6 : Descriptors_SLConfig    (Esds_Offset, Size); break;
            case 0x10 : Descriptors_Profiles    (Esds_Offset, Size); break;
        }
        FLUSH();

        //Details
        if (Config.Details_Get())
        {
            Element_Level--;
        }

        //Next
        Esds_Offset+=Size;
        Esds_Size-=Size;
    }
}

//***************************************************************************
// Elements
//***************************************************************************

#define INTEGRITY(ELEMENT_SIZE) \
    if (Esds_Size<ELEMENT_SIZE) \
    { \
        TRUSTED_ISNOT("Size is wrong"); \
        return; \
    } \

#define BEGIN \
    size_t Stream_Pos=0; \

#define END \
    { \
    } \

#define GET_I1(_INFO, _NAME) \
    { \
        INTEGRITY(Stream_Pos+1) \
        _INFO=BigEndian2int8u(Buffer+Buffer_Offset+Esds_Offset+Stream_Pos); \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Stream_Pos, #_NAME, _INFO); \
        } \
        Stream_Pos+=1; \
    }

#define GET_I2(_INFO, _NAME) \
    { \
        INTEGRITY(Stream_Pos+2) \
        _INFO=BigEndian2int16u(Buffer+Buffer_Offset+Esds_Offset+Stream_Pos); \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Stream_Pos, #_NAME, _INFO); \
        } \
        Stream_Pos+=2; \
    }

#define GET_I4(_INFO, _NAME) \
    { \
        INTEGRITY(Stream_Pos+4) \
        _INFO=BigEndian2int32u(Buffer+Buffer_Offset+Esds_Offset+Stream_Pos); \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Stream_Pos, #_NAME, _INFO); \
        } \
        Stream_Pos+=4; \
    }

#define SKIP_I1(_NAME) \
    { \
        INTEGRITY(Stream_Pos+1) \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Stream_Pos, #_NAME, BigEndian2int8u(Buffer+Buffer_Offset+Esds_Offset+Stream_Pos)); \
        } \
        Stream_Pos+=1; \
    } \

#define SKIP_I2(_NAME) \
    { \
        INTEGRITY(Stream_Pos+2) \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Stream_Pos, #_NAME, BigEndian2int16u(Buffer+Buffer_Offset+Esds_Offset+Stream_Pos); \
        } \
        Stream_Pos+=2; \
    } \

#define SKIP_I4(_NAME) \
    { \
        INTEGRITY(Stream_Pos+4) \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Stream_Pos, #_NAME, BigEndian2int32u(Buffer+Buffer_Offset+Esds_Offset+Stream_Pos)); \
        } \
        Stream_Pos+=4; \
    } \

//---------------------------------------------------------------------------
// DecConfig
// ObjectTypeId                     1 bytes, Pos=0
// Flags                            1 bytes, Pos=1
// BufferSizeDB                     3 bytes, Pos=2
// MaxBitrate                       4 bytes, Pos=5
// AvgBitrate                       4 bytes, Pos=9
// Descriptors                      X bytes, Pos=13
//
void File_Mpeg4::Descriptors_DecConfig(size_t Esds_Offset, size_t Esds_Size)
{
    NAME("DecConfig");
    INTEGRITY(13);

    //Reading
    int8u  ObjectTypeId =BigEndian2int8u (Buffer+Buffer_Offset+Esds_Offset+0);
    int32u BitRate_Max  =BigEndian2int32u(Buffer+Buffer_Offset+Esds_Offset+5);
    int32u BitRate_Avg  =BigEndian2int32u(Buffer+Buffer_Offset+Esds_Offset+9);

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(0, "Size", Esds_Size);
        Details_Add_Info(0, "ObjectTypeId", ObjectTypeId);
        Details_Add_Info(1, "Flags", BigEndian2int8u(Buffer+Buffer_Offset+Esds_Offset+1));
        Details_Add_Info(2, "BufferSizeDB", BigEndian2int32u(Buffer+Buffer_Offset+Esds_Offset+1)&0x00FFFFFF);
        Details_Add_Info(5, "MaxBitrate", BitRate_Max);
        Details_Add_Info(9, "AvgBitrate", BitRate_Avg);
    }

    //Filling
    switch (ObjectTypeId)
    {
        case 0x01 : Fill("Codec", ""); Fill("Codec", "System"); break;
        case 0x02 : Fill("Codec", ""); Fill("Codec", "System Core"); break;
        case 0x20 : Fill("Codec", ""); Fill("Codec", "MPEG-4V"); break;
        case 0x21 : Fill("Codec", ""); Fill("Codec", "H264"); break;
        case 0x40 : Fill("Codec", ""); Fill("Codec", "AAC"); break; //MPEG-4 AAC
        case 0x60 : Fill("Codec", ""); Fill("Codec", "MPEG-2V"); break; //MPEG-2V Simple
        case 0x61 : Fill("Codec", ""); Fill("Codec", "MPEG-2V"); break; //MPEG-2V Main
        case 0x62 : Fill("Codec", ""); Fill("Codec", "MPEG-2V"); break; //MPEG-2V SNR
        case 0x63 : Fill("Codec", ""); Fill("Codec", "MPEG-2V"); break; //MPEG-2V Spatial
        case 0x64 : Fill("Codec", ""); Fill("Codec", "MPEG-2V"); break; //MPEG-2V High
        case 0x65 : Fill("Codec", ""); Fill("Codec", "MPEG-2V"); break; //MPEG-2V 4:2:2
        case 0x66 : Fill("Codec", ""); Fill("Codec", "AAC"); break; //MPEG-2 AAC Main
        case 0x67 : Fill("Codec", ""); Fill("Codec", "AAC"); break; //MPEG-2 AAC LC
        case 0x68 : Fill("Codec", ""); Fill("Codec", "AAC"); break; //MPEG-2 AAC SSR
        case 0x69 : Fill("Codec", ""); Fill("Codec", "MPEG-2A L3"); break;
        case 0x6A : Fill("Codec", ""); Fill("Codec", "MPEG-1V"); break;
        case 0x6B : Fill("Codec", ""); Fill("Codec", "MPEG-1A"); break;
        case 0x6C : Fill("Codec", ""); Fill("Codec", "JPEG"); break;
        case 0xA0 : Fill("Codec", ""); Fill("Codec", "EVRC"); Fill("SamplingRate", "8000"); Fill("Channel(s)", "1"); break;
        case 0xA1 : Fill("Codec", ""); Fill("Codec", "SMV"); Fill("SamplingRate", "8000"); Fill("Channel(s)", "1");  break;
        case 0xD1 : Fill("Codec", ""); Fill("Codec", "EVRC"); Fill("SamplingRate", "8000"); Fill("Channel(s)", "1");  break;
        case 0xDD : Fill("Codec", ""); Fill("Codec", "Ogg"); break;
        case 0xDE : Fill("Codec", ""); Fill("Codec", "Ogg"); break;
        case 0xE1 : Fill("Codec", ""); Fill("Codec", "QCELP"); Fill("SamplingRate", "8000"); Fill("Channel(s)", "1");  break;
        default: ;
    }

    if (BitRate_Avg>0)
    {
        Fill("BitRate", BitRate_Avg);
        if (BitRate_Max<=BitRate_Avg*1.1)
            Fill("BitRate_Mode", "CBR");
        else
            Fill("BitRate_Mode", "VBR");
    }

    //Other descriptors
    Descriptors(Esds_Offset+13, Esds_Size-13);
}

//---------------------------------------------------------------------------
// DecSpecific
// Info                             X bytes, Pos=0
//
void File_Mpeg4::Descriptors_DecSpecific(size_t Esds_Offset, size_t Esds_Size)
{
    //Details
    if (Config.Details_Get())
    {
        Details_Add_Element("DecSpecific");
    }

    File__Base* MI=NULL;

    //Creating the parser
         if (0);
    #if defined(MEDIAINFO_MPEG4V_YES)
    else if (Get(StreamKind_Last, StreamPos_Last, _T("Codec"))==_T("MPEG-4V"))
        MI=new File_Mpeg4v;
    #endif
    #if defined(MEDIAINFO_AAC_YES)
    else if (Get(StreamKind_Last, StreamPos_Last, _T("Codec"))==_T("AAC"))
        MI=new File_Aac;
    #endif
    else
        return;

    //Parsing
    Open_Buffer_Init(MI, File_Offset+Buffer_Offset+Esds_Offset+Esds_Size, File_Offset+Buffer_Offset+Esds_Offset);
    Open_Buffer_Continue(MI, Buffer+Buffer_Offset+Esds_Offset, Esds_Size);
    Open_Buffer_Finalize(MI);

    //Filling
    Merge(*MI, StreamKind_Last, 0, StreamPos_Last);
}

//---------------------------------------------------------------------------
// ES
// ESID                             2 bytes, Pos=0
// Flags                            1 bytes, Pos=2 (7=DependsOnESID, 6=URL, 5=OCRESID 4-0=streamPriority)
// DependsOnESID                    2 bytes, Pos= (Optional!)
// URL                              1+X bytes, Pos= (Optional!)
// OCRESID                          2 bytes, Pos= (Optional!)
// Descriptors                      X bytes, Pos=
//
void File_Mpeg4::Descriptors_ES(size_t Esds_Offset, size_t Esds_Size)
{
    //Integrity
    if (Esds_Size<3) //Size of atom
        return;

    //Reading
    int8u Flags=BigEndian2int8u(Buffer+Buffer_Offset+Esds_Offset+2);
    bool DependsOnESID_Flag=(Flags&0x80)>>7;
    bool URL_Flag          =(Flags&0x40)>>6;
    bool OCRESID_Flag      =(Flags&0x20)>>5;

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Element("ES");
        Details_Add_Info(0, "ESID", BigEndian2int16u(Buffer+Buffer_Offset+Esds_Offset+0));
        Details_Add_Info(2, "DependsOnESID_Flag", (int32u)DependsOnESID_Flag);
        Details_Add_Info(2, "URL_Flag", (int32u)URL_Flag);
        Details_Add_Info(2, "OCRESID_Flag", (int32u)OCRESID_Flag);
        Details_Add_Info(2, "StreamPriority", Flags&0x1F);
    }

    //Reading
    size_t Pos=3;
    if (DependsOnESID_Flag)
    {
        //Details
        if (Config.Details_Get())
            Details_Add_Info(Pos, "DependsOnESID", BigEndian2int16u(Buffer+Buffer_Offset+Esds_Offset+Pos));
        //Jumping
        Pos+=2;
    }
    if (URL_Flag)
    {
        //Reading
        int8u Size=BigEndian2int8u(Buffer+Buffer_Offset+Esds_Offset+Pos);
        //Details
        if (Config.Details_Get())
        {
            Details_Add_Info(Pos, "URL Size", Size);
            Details_Add_Info(Pos+1, "URL", Buffer+Buffer_Offset+Esds_Offset+Pos+1, Size);
        }
        //Jumping
        Pos+=1+Size;
    }
    if (OCRESID_Flag)
    {
        //Details
        if (Config.Details_Get())
            Details_Add_Info(Pos, "OCRESID", BigEndian2int16u(Buffer+Buffer_Offset+Esds_Offset+Pos));
        //Jumping
        Pos+=2;
    }

    //Integrity
    if (Pos>=Esds_Size) //Size of atom
        return;

    //Other descriptors
    Descriptors(Esds_Offset+Pos, Esds_Size-Pos);
}

//---------------------------------------------------------------------------
// SLConfig
// Predefined                       1 bytes, Pos=0
//
void File_Mpeg4::Descriptors_SLConfig(size_t Esds_Offset, size_t Esds_Size)
{
    NAME("SLConfig");
    INTEGRITY(1);

    //Reading
    BEGIN
    SKIP_I1 (                                                   Predefined)
    END
}

//---------------------------------------------------------------------------
//
void File_Mpeg4::Descriptors_Profiles(size_t Esds_Offset, size_t Esds_Size)
{
    NAME("Profiles");
    INTEGRITY(5);

    //Reading
    BEGIN
    SKIP_I1 (                                                   Unknown)
    SKIP_I1 (                                                   ID)
    SKIP_I1 (                                                   ODProfileLevel)
    SKIP_I1 (                                                   SceneProfileLevel)
    SKIP_I1 (                                                   AudioProfileLevel)
    SKIP_I1 (                                                   VideoProfileLevel)
    SKIP_I1 (                                                   GraphicsProfileLevel)
    END
}

//***************************************************************************
// C++
//***************************************************************************

} //NameSpace

#endif //MEDIAINFO_MPEG4_*
