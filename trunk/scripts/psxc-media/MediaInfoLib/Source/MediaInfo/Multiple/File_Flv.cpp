// File_Flv - Info for Flash files
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

//---------------------------------------------------------------------------
//
// Examples:
// http://samples.mplayerhq.hu/FLV/
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Compilation condition
#include <MediaInfo/Setup.h>
#if defined(MEDIAINFO_FLV_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Multiple/File_Flv.h"
#include "ZenLib/Utils.h"
#include "ZenLib/BitStream.h"
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//---------------------------------------------------------------------------
int8u  Flv_Channels[]=
{
    1,
    2,
};

int8u  Flv_Resolution[]=
{
    8,
    16,
};

int32u Flv_SamplingRate[]=
{
    5500,
    11025,
    22050,
    44100,
    8000, //Special case for Nellymoser 8kHz mono
};

Char* Flv_Codec_Audio[]=
{
    _T("Uncompressed"),
    _T("ADPCM"),
    _T("MPEG-1L3"),
    _T(""),
    _T(""),
    _T("Nellymoser 8kHz mono"),
    _T("Nellymoser"),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
};

Char* Flv_Codec_Video[]=
{
    _T(""),
    _T(""),
    _T("H263"),
    _T("Screen video"),
    _T("On2 VP6"),
    _T("On2 VP6 with alpha channel"),
    _T("Screen video 2"),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
};

const Char* Flv_H263_PictureSize[]=
{
    _T("custom, 1 byte"),
    _T("custom, 2 bytes"),
    _T("CIF (352x288)"),
    _T("QCIF (176x144)"),
    _T("SQCIF (128x96)"),
    _T("320x240"),
    _T("160x120"),
    _T(""),
};

const int32u Flv_H263_WidthHeight[8][2]=
{
    {  0,   0},
    {  0,   0},
    {352, 288},
    {176, 144},
    {128, 96},
    {320, 240},
    {160, 120},
    {0, 0},
};

const Char* Flv_H263_PictureType[]=
{
    _T("IntraFrame"),
    _T("InterFrame"),
    _T("InterFrame (Disposable)"),
    _T(""),
};
const Char* Flv_VP6_FrameMode[]=
{
    _T("IntraFrame"),
    _T(""),
};

const Char* Flv_VP6_Marker[]=
{
    _T("VP6.1/6.2"),
    _T("VP6.0"),
};

const Char* Flv_VP6_Version[]=
{
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T("VP6.0/6.1"),
    _T("VP6.0 (Electronic Arts)"),
    _T("VP6.2"),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
};

const Char* Flv_VP6_Version2[]=
{
    _T("VP6.0"),
    _T(""),
    _T(""),
    _T("VP6.1/6.2"),
};

const Char* Flv_FrameType[]=
{
    _T(""),
    _T("KeyFrame"),
    _T("InterFrame"),
    _T("InterFrame (Disposable)"),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
    _T(""),
};

const Char* Flv_TagType[]=
{
    _T("Number"),
    _T("Boolean"),
    _T("String"),
    _T("Object"),
    _T("MovieClip"),
    _T("Null"),
    _T("Undefined"),
    _T("Reference"),
    _T("ECMA array"),
    _T(""),
    _T("Strict array"),
    _T("Date"),
    _T("Long string"),
};

//---------------------------------------------------------------------------

//***************************************************************************
// Format
//***************************************************************************

//---------------------------------------------------------------------------
void File_Flv::Read_Buffer_Continue()
{
    if (File_Offset==0)
    {
        if (Buffer_Size<9 || CC3(Buffer)!=CC3("FLV"))
        {
            File_Offset=File_Size;
            return;
        }

        header();
        Buffer_Offset+=Element_Size;
    }

    while (Buffer_Parse());

    //No need of more
    File_Offset=File_Size;
}

//---------------------------------------------------------------------------
void File_Flv::Read_Buffer_Finalize()
{
    if (Video.size()>0 && Get(Stream_Video, 0, _T("Codec")).empty())
        Video[0].clear(); //Said there is video, but there is not!
    if (Audio.size()>0 && Get(Stream_Audio, 0, _T("Codec")).empty())
        Audio[0].clear(); //Said there is audio, but there is not!
}

//***************************************************************************
// Buffer
//***************************************************************************

#define ELEMENT(_NAME) \
    if (Config.Details_Get()) \
    { \
        Details_Add_Element(_NAME); \
    } \

#define BEGIN_I \
    size_t Stream_Pos=0; \

#define BEGIN \
    BitStream BS(Buffer+Buffer_Offset, Element_Size); \

#define END_I \
    { \
        Element_Size=Stream_Pos; \
        FLUSH(); \
    } \

#define END \
    { \
        BS.Byte_Align(); \
        INTEGRITY(BS.Offset_Get()) \
    } \

#define INTEGRITY(ELEMENT_SIZE) \
     if (Element_Size<ELEMENT_SIZE) \
        return; \

#define INTEGRITY_E(ELEMENT_SIZE) \
     if (Element_Size<ELEMENT_SIZE) \
        return false; \

#define GET_I1(_INFO, _NAME) \
    { \
        INTEGRITY(Stream_Pos+1) \
        _INFO=BigEndian2int8u(Buffer+Buffer_Offset+Stream_Pos); \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Stream_Pos, #_NAME, _INFO); \
        } \
        Stream_Pos+=1; \
    }

#define GET_I1_E(_INFO, _NAME) \
    { \
        _INFO=BigEndian2int8u(Buffer+Buffer_Offset+Stream_Pos); \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Element_Info(Stream_Pos, #_NAME, _INFO); \
        } \
        Stream_Pos+=1; \
    }

#define GET_I2(_INFO, _NAME) \
    { \
        INTEGRITY(Stream_Pos+2) \
        _INFO=BigEndian2int16u(Buffer+Buffer_Offset+Stream_Pos); \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Stream_Pos, #_NAME, _INFO); \
        } \
        Stream_Pos+=2; \
    }

#define GET_I3(_INFO, _NAME) \
    { \
        INTEGRITY(Stream_Pos+2) \
        _INFO=BigEndian2int8u(Buffer+Buffer_Offset+Stream_Pos)*0x10000+BigEndian2int16u(Buffer+Buffer_Offset+Stream_Pos+1); \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Stream_Pos, #_NAME, _INFO); \
        } \
        Stream_Pos+=3; \
    }

#define GET_I3_E(_INFO, _NAME) \
    { \
        _INFO=BigEndian2int8u(Buffer+Buffer_Offset+Stream_Pos)*0x10000+BigEndian2int16u(Buffer+Buffer_Offset+Stream_Pos+1); \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Element_Info(Stream_Pos, #_NAME, _INFO); \
        } \
        Stream_Pos+=3; \
    }

#define GET_I4(_INFO, _NAME) \
    { \
        INTEGRITY(Stream_Pos+4) \
        _INFO=BigEndian2int32u(Buffer+Buffer_Offset+Stream_Pos); \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Stream_Pos, #_NAME, _INFO); \
        } \
        Stream_Pos+=4; \
    }

#define GET_CH(_BYTES, _INFO, _NAME) \
    { \
        INTEGRITY(Stream_Pos+_BYTES) \
        _INFO.From_Local((const char*)(Buffer+Buffer_Offset+Stream_Pos), _BYTES); \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Stream_Pos, #_NAME, _INFO); \
        } \
        Stream_Pos+=_BYTES; \
    }

#define GET(_BITS, _INFO, _NAME) \
    { \
        INTEGRITY(BS.Offset_Get()) \
        _INFO=BS.Get(_BITS); \
        if (Config.Details_Get()) \
        { \
            Details_Add_Info(Error, #_NAME, _INFO); \
        } \
    }

#define SKIP_I1(_NAME) \
    { \
        INTEGRITY(Stream_Pos+1) \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Stream_Pos, #_NAME, BigEndian2int8u(Buffer+Buffer_Offset+Stream_Pos)); \
        } \
        Stream_Pos+=1; \
    } \

#define SKIP_I1_E(_NAME) \
    { \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Element_Info(Stream_Pos, #_NAME, BigEndian2int8u(Buffer+Buffer_Offset+Stream_Pos)); \
        } \
        Stream_Pos+=1; \
    } \

#define SKIP_I2(_NAME) \
    { \
        INTEGRITY(Stream_Pos+2) \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Stream_Pos, #_NAME, BigEndian2int16u(Buffer+Buffer_Offset+Stream_Pos)); \
        } \
        Stream_Pos+=2; \
    } \

#define SKIP_I3(_NAME) \
    { \
        INTEGRITY(Stream_Pos+3) \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Stream_Pos, #_NAME, BigEndian2int8u(Buffer+Buffer_Offset+Stream_Pos)*0x10000+BigEndian2int16u(Buffer+Buffer_Offset+Stream_Pos+1)); \
        } \
        Stream_Pos+=3; \
    } \

#define SKIP_I3_E(_NAME) \
    { \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Element_Info(Stream_Pos, #_NAME, BigEndian2int8u(Buffer+Buffer_Offset+Stream_Pos)*0x10000+BigEndian2int16u(Buffer+Buffer_Offset+Stream_Pos+1)); \
        } \
        Stream_Pos+=3; \
    } \

#define SKIP_I4(_NAME) \
    { \
        INTEGRITY(Stream_Pos+4) \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Stream_Pos, #_NAME, BigEndian2int32u(Buffer+Buffer_Offset+Stream_Pos)); \
        } \
        Stream_Pos+=4; \
    } \

#define SKIP_I4_E(_NAME) \
    { \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Element_Info(Stream_Pos, #_NAME, BigEndian2int32u(Buffer+Buffer_Offset+Stream_Pos)); \
        } \
        Stream_Pos+=4; \
    } \

#define SKIP_CH(_BYTES, _NAME) \
    { \
        INTEGRITY(Stream_Pos+_BYTES) \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Stream_Pos, #_NAME, Buffer+Buffer_Offset+Stream_Pos, _BYTES); \
        } \
        Stream_Pos+=_BYTES; \
    } \

#define SKIP(_BITS, _NAME) \
    { \
        INTEGRITY(BS.Offset_Get()) \
        if (Config.Details_Get()) \
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
bool File_Flv::Buffer_Parse()
{
    if (Buffer_Offset+15>Buffer_Size)
        return false; //Not enough buffer
    Element_Size=15;

    //Parsing
    BEGIN_I
    SKIP_I4_E(                                                  PreviousTagSize)
    GET_I1_E (Element_Name,                                     Type) //INFO(Element_Name<19?Flv_Type[Element_Name]:_T("Unknown"));
    GET_I3_E (Element_Size,                                     BodyLength)
    SKIP_I3_E(                                                  Timestamp_Base) //in ms
    SKIP_I1_E(                                                  Timestamp_Extended) //TimeStamp = Timestamp_Extended*0x01000000+Timestamp_Base
    SKIP_I3_E(                                                  StreamID)

    Element_Next=Buffer_Offset+15+Element_Size;

    Element_Parse();

    //If no need of more
    if (Flags==0x00)
    {
        File_Offset=File_Size;
        return false;
    }

    Buffer_Offset=Element_Next;

    return true;
}

//---------------------------------------------------------------------------
bool File_Flv::Element_Parse()
{
    //Details
    if (Config.Details_Get())
    {
        Details_Add_Element(0, Element_Name, Element_Size);
    }

    //Positionning
    Buffer_Offset+=15;

    switch (Element_Name)
    {
        case 0x08 : audio(); break;
        case 0x09 : video(); break;
        case 0x12 : meta(); break;
        default : break;
    }

    FLUSH();
    return true;
}

//***************************************************************************
// Elements
//***************************************************************************

//---------------------------------------------------------------------------
void File_Flv::header()
{
    //Details
    if (Config.Details_Get())
    {
        Details_Add_Element(0, "FLV header", 9);
    }

    if (Buffer_Offset+9>Buffer_Size)
        return; //Not enough buffer
    Element_Size=9;

    //Parsing
    int32u Version;
    BEGIN_I
    SKIP_CH(3,                                                  Signature)
    GET_I1 (Version,                                            Version)
    GET_I1 (Flags,                                              Flags)
    if (Config.Details_Get()>0.5)
    {
        Details_Add_Info(Stream_Pos, "Video", (bool)((Flags&0x00000001)>0));
        Details_Add_Info(Stream_Pos, "Audio", (bool)((Flags&0x00000004)>2));
    }
    GET_I4 (Element_Size,                                       Size)
    END_I

    //Integrity
    if (Version==0 || Version>1)
        return;
    if (Element_Size<9)
        return;

    //Filling
    Stream_Prepare(Stream_General);
    Fill("Format", "FLV");
    if (Flags&0x00000001) Stream_Prepare(Stream_Video);
    if (Flags&0x00000004) Stream_Prepare(Stream_Audio);
    FLUSH();
}

//---------------------------------------------------------------------------
void File_Flv::video()
{
    ELEMENT("Video");

    //Needed?
    if (!(Flags&0x01))
        return; //No more need of Aufio stream

    //Parsing
    int32u Codec, FrameType;
    BEGIN
    GET (4, FrameType,                                          frameType) INFO(Flv_FrameType[FrameType]);
    GET (4, Codec,                                              codecID) INFO(Flv_Codec_Video[Codec]);
    END

    if (FrameType!=1) //Not a KeyFrame
        return;

    //Filling
    if (Get(Stream_Video, 0, _T("Codec")).empty())
    {
        //Filling
        if (Count_Get(Stream_Video)==0)
            Stream_Prepare(Stream_Video);
        if (Codec<6)
            Fill(Stream_Video, 0, "Codec", Flv_Codec_Video[Codec]);
        Flags&=0xFE; //No more need of Video stream

        //Parsing video data
        Buffer_Offset+=1;
        Element_Size+=1;
        switch (Codec)
        {
            case 2 : video_H263(); break;
            case 3 : video_ScreenVideo(); break;
            case 4 : video_VP6(false); break;
            case 5 : video_VP6(true); break;
            case 6 : video_ScreenVideo(); break;
        }
    }
}

//---------------------------------------------------------------------------
void File_Flv::video_H263()
{
    //Parsing
    int32u Version, PictureSize, Width, Height, PictureType;
    BEGIN
    SKIP(17,                                                    PictureStartCode)
    GET (5, Version,                                            Version)
    if (Version>1)
        return;
    SKIP(8,                                                     TemporalReference)
    GET (3, PictureSize,                                        PictureSize) INFO(Flv_H263_PictureSize[PictureSize]);
    switch (PictureSize)
    {
        case 0 :
            GET ( 8, Width,                                     Width)
            GET ( 8, Height,                                    Height)
            break;
        case 1 :
            GET (16, Width,                                     Width)
            GET (16, Height,                                    Height)
            break;
        default :
            Width=Flv_H263_WidthHeight[PictureSize][0];
            Height=Flv_H263_WidthHeight[PictureSize][1];
    }
    GET (2, PictureType,                                        PictureSize) INFO(Flv_H263_PictureType[PictureType]);
    SKIP(1,                                                     DeblockingFlag)
    SKIP(5,                                                     Quantizer)

    //Filling
    Fill(Stream_Video, 0, "Width", _T(""));
    Fill(Stream_Video, 0, "Width", Width);
    Fill(Stream_Video, 0, "Height", _T(""));
    Fill(Stream_Video, 0, "Height", Height);
}

//---------------------------------------------------------------------------
void File_Flv::video_ScreenVideo()
{
    //Parsing
    int32u Width, Height;
    BEGIN
    SKIP(4,                                                     PixelWidth)
    GET (12, Width,                                             Width)
    SKIP(4,                                                     PixelHeight)
    GET (12, Height,                                            Height)

    //Filling
    //Fill(Stream_Video, 0, "Width", _T(""));
    //Fill(Stream_Video, 0, "Width", Width&0x3FFF);
    //Fill(Stream_Video, 0, "Height", _T(""));
    //Fill(Stream_Video, 0, "Height", Height&0x3FFF);
}

//---------------------------------------------------------------------------
// From: http://wiki.multimedia.cx/index.php?title=On2_VP6
//
void File_Flv::video_VP6(bool WithAlpha)
{
    //Parsing
    int32u HorizontalAdjustment, VerticalAdjustment, Version, Version2, FrameMode, Marker, Width=0, Height=0;
    BEGIN
    GET ( 4, HorizontalAdjustment,                              HorizontalAdjustment) INFO(Ztring(_T("Remove "))+Ztring::ToZtring(HorizontalAdjustment)+_T(" pixels"));
    GET ( 4, VerticalAdjustment,                                VerticalAdjustment) INFO(Ztring(_T("Remove "))+Ztring::ToZtring(VerticalAdjustment)+_T(" pixels"));
    if (WithAlpha)
        SKIP(24,                                                OffsetToAlpha)
    GET ( 1, FrameMode,                                         FrameMode) INFO(Flv_VP6_FrameMode[FrameMode]);
    SKIP( 6,                                                    Quantization)
    GET ( 1, Marker,                                            Marker) INFO(Flv_VP6_Marker[Marker]);
    if (FrameMode==0)
    {
        GET ( 5, Version,                                       Version)
        GET ( 2, Version2,                                      Version2)
        SKIP( 1,                                                Interlace)
        if (Marker==1 || Version2==0)
            SKIP(16,                                            Offset)
        SKIP( 8,                                                MacroBlock_Height)
        SKIP( 8,                                                MacroBlock_Width)
        GET ( 8, Height,                                        Height) INFO(Ztring::ToZtring(Height*16)+_T(" pixels"));
        GET ( 8, Width,                                         Width) INFO(Ztring::ToZtring(Width*16)+_T(" pixels"));
    }
    else
    {
        if (Marker==1)
            SKIP( 16,                                           Offset)
    }

    //Filling
    if (Width && Height)
    {
        Fill(Stream_Video, 0, "Width",  "");
        Fill(Stream_Video, 0, "Width",  Width*16-HorizontalAdjustment);
        Fill(Stream_Video, 0, "Height", "");
        Fill(Stream_Video, 0, "Height", Height*16-VerticalAdjustment);
    }
}

//---------------------------------------------------------------------------
void File_Flv::audio()
{
    ELEMENT("Audio");

    //Needed?
    if (!(Flags&0x04))
        return; //No more need of Aufio stream

    //Parsing
    int32u Channels, Resolution, SamplingRate, Codec;
    BEGIN
    GET (4, Codec,                                           codec) INFO(Flv_Codec_Audio[Codec]);
    GET (2, SamplingRate,                                    sampling_rate) INFO(Ztring::ToZtring(Flv_SamplingRate[SamplingRate])+_T(" Hz"));
    GET (1, Resolution,                                      is_16bit) INFO(Ztring::ToZtring(Flv_Resolution[Resolution])+_T(" bits"));
    GET (1, Channels,                                        is_stereo) INFO(Ztring::ToZtring(Flv_Channels[Channels])+_T(" channel(s)"));
    END

    //Special case
    if (Codec==5) //Nellymoser 8kHz mono
    {
        SamplingRate=5; //8000 Hz forced
        Channels=0; //Mono forced
    }

    //Filling
    if (Get(Stream_Audio, 0, _T("Codec")).empty())
    {
        //Filling
        if (Count_Get(Stream_Audio)==0)
            Stream_Prepare(Stream_Audio);
        Fill(Stream_Audio, 0, "Channel(s)", Flv_Channels[Channels]);
        Fill(Stream_Audio, 0, "Resolution", Flv_Resolution[Resolution]);
        Fill(Stream_Audio, 0, "SamplingRate", Flv_SamplingRate[SamplingRate]);
        if (Codec<6)
            Fill(Stream_Audio, 0, "Codec", Flv_Codec_Audio[Codec]);
        Flags&=0xFB; //No more need of Aufio stream
    }
}

//---------------------------------------------------------------------------
void File_Flv::meta()
{
    ELEMENT("Meta");

    BEGIN_I
    SKIP_I1(                                                    Unknown)

    //Parsing
    Buffer_Offset+=Stream_Pos;
    Element_Size-=Stream_Pos;
    meta_Parse();
}

//---------------------------------------------------------------------------
void File_Flv::meta_Parse()
{
    int32u Name_Size;
    Ztring Name, Value;
    stream_t StreamKind=Stream_General;
    BEGIN_I
    GET_I2 (Name_Size,                                          Name_Size)
    GET_CH (Name_Size, Name,                                    Name)

    //Parsing Value
    meta_Parse_Value(Stream_Pos, StreamKind, Name, Value);

    //Filling
    if (!Name.empty())
        Fill(StreamKind, 0, Name.To_UTF8().c_str(), Value);

    //Positioning after the frame
    Buffer_Offset+=Stream_Pos;
    Element_Size-=Stream_Pos;
}

//---------------------------------------------------------------------------
void File_Flv::meta_Parse_Value(int32u &Stream_Pos, stream_t &StreamKind, Ztring &Name, Ztring &Value)
{
    int32u Type, ECMA;
    GET_I1 (Type,                                               Type) if (Type<13) INFO(Flv_TagType[Type]);
    switch (Type)
    {
        case 0x00 : //Double
            {
                double Value_Double;
                int8u Double0, Double1, Double2, Double3, Double4, Double5, Double6, Double7;
                GET_I1 (Double0,                                DoublePart1)
                GET_I1 (Double1,                                DoublePart2)
                GET_I1 (Double2,                                DoublePart3)
                GET_I1 (Double3,                                DoublePart4)
                GET_I1 (Double4,                                DoublePart5)
                GET_I1 (Double5,                                DoublePart6)
                GET_I1 (Double6,                                DoublePart7)
                GET_I1 (Double7,                                DoublePart8)
                int8u* Double=(int8u*)&Value_Double;
                Double[0]=Double7;
                Double[1]=Double6;
                Double[2]=Double5;
                Double[3]=Double4;
                Double[4]=Double3;
                Double[5]=Double2;
                Double[6]=Double1;
                Double[7]=Double0;
                INFO(Ztring::ToZtring(Value_Double));
                     if (0) ;
                else if (Name==_T("width")) {Name=_T("Width"); StreamKind=Stream_Video; Value.From_Number(Value_Double, 0);}
                else if (Name==_T("height")) {Name=_T("Height"); StreamKind=Stream_Video; Value.From_Number(Value_Double, 0);}
                else if (Name==_T("duration")) {Name=_T("PlayTime"); Value.From_Number(Value_Double*1000, 0);}
                else if (Name==_T("audiodatarate")) {Name=_T("BitRate"); StreamKind=Stream_Audio; Value.From_Number(Value_Double*1000, 0);}
                else if (Name==_T("framerate")) {Name=_T("FrameRate"); StreamKind=Stream_Video; Value.From_Number(Value_Double, 3);}
                else if (Name==_T("datasize")) {Name=_T("");}
                else if (Name==_T("lasttimestamp")) {Name=_T("");}
                else if (Name==_T("audiosize")) {Name=_T("StreamSize"); StreamKind=Stream_Audio; Value.From_Number(Value_Double, 0);}
                else if (Name==_T("videosize")) {Name=_T("StreamSize"); StreamKind=Stream_Video; Value.From_Number(Value_Double, 0);}
                else if (Name==_T("videodatarate")) {Name=_T("BitRate"); StreamKind=Stream_Video; Value.From_Number(Value_Double*1000, 0);}
                else if (Name==_T("lastkeyframetimestamp")) {Name=_T("");}
                else if (Name==_T("lastkeyframelocation")) {Name=_T("");}
                else if (Name==_T("videocodecid")) {Name=_T("");}
                else if (Name==_T("audiocodecid")) {Name=_T("");}
                else if (Name==_T("audiodelay")) {Name=_T("Delay"); StreamKind=Stream_Audio; if (Value_Double>0) Value.From_Number(Value_Double*1000, 0);}
                else if (Name==_T("canSeekToEnd")) {Name=_T("");}
                else if (Name==_T("")) {Name=_T(""); StreamKind=Stream_General; Value.From_Number(Value_Double*1000, 0);}
                else
                    Value.From_Number(Value_Double);
            }
            break;
        case 0x01 : //Boolean
            int32u Value_Bool;
            GET_I1 (Value_Bool,                                 Value)
            Value=Value_Bool?_T("Yes"):_T("No");
                 if (0) ;
            else if (Name==_T("haskeyframes")) {Name=_T("");}
            else if (Name==_T("hasmetadata")) {Name=_T("");}
            break;
        case 0x02 : //String
            {
                int32u Value_Size;
                GET_I2 (Value_Size,                             Value_Size)
                if (Value_Size)
                    GET_CH (Value_Size, Value,                      Value)
                else
                    Value.clear();
            }
                 if (0) ;
            else if (Name==_T("creator")) {Name=_T("Encoded_Application");}
            else if (Name==_T("metadatacreator")) {Name=_T("Tagged_Application");}
            else if (Name==_T("creationdate")) {Name=_T("Encoded_Date"); Value.Date_From_String(Value.To_UTF8().c_str());}
            break;
        case 0x03 : //Object
            {
                int32u Value_Size;
                GET_I2 (Value_Size,                                 Value_Size)
                if (Value_Size)
                    GET_CH (Value_Size, Value,                      Value)
                else
                    Value.clear();
            }
            meta_Parse_Value(Stream_Pos, StreamKind, Name, Value);
            Value.clear();
            Stream_Pos=Element_Size; //Forcing the end of parsing, there is a bug somewhere...
            break;
        case 0x07 : //Reference
            int32u Value_U16;
            GET_I2 (Value_U16,                                  Value)
            break;
        case 0x08 : //ECMA
            GET_I4 (ECMA,                                       ECMA)
            Buffer_Offset+=Stream_Pos;
            Element_Size-=Stream_Pos;
            Details_Level_Last++;
            for (int32u Pos=0; Pos<ECMA; Pos++)
                meta_Parse();
            Details_Level_Last--;
            break;
        case 0x0A : //Array
            {
                int32u Value_Size;
                GET_I2 (Value_Size,                                 Value_Size)
                if (Value_Size)
                    GET_CH (Value_Size, Value,                      Value)
                else
                    Value.clear();
            }
            while (Stream_Pos<Element_Size)
                meta_Parse_Value(Stream_Pos, StreamKind, Name, Value);
            Value.clear();
            Stream_Pos=Element_Size; //Forcing the end of parsing, there is a bug somewhere...
            break;
        case 0x0B : //Date
            {
                int32u DateTime_1, DateTime_2;
                GET_I4 (DateTime_1,                         DateTime_1)
                GET_I4 (DateTime_2,                         DateTime_2)
                Value.Date_From_Seconds_1970((DateTime_1*0x100000000LL+DateTime_2)/1000);
                INFO(Value);
                Value.clear(); //There is a bug, where?
                SKIP_I2(                                    Local_Offset_Minutes)
            }
            break;
        default :
            Stream_Pos=Element_Size; //Forcing the end of parsing
    }
}


//***************************************************************************
// Information
//***************************************************************************

//---------------------------------------------------------------------------
void File_Flv::HowTo(stream_t StreamKind)
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

#endif //MEDIAINFO_FLV_YES

