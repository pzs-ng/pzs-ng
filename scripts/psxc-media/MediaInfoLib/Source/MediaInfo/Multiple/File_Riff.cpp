// File_Riff - Info for RIFF files
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
#if defined(MEDIAINFO_RIFF_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Multiple/File_Riff.h"
#if defined(MEDIAINFO_MPEG4V_YES)
    #include "MediaInfo/Video/File_Mpeg4v.h"
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
#if defined(MEDIAINFO_AAC_YES)
    #include "MediaInfo/Audio/File_Aac.h"
#endif
#include <ZenLib/Utils.h>
#include <ZenLib/Ztring.h>
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//---------------------------------------------------------------------------
extern MediaInfo_Config Config;
//---------------------------------------------------------------------------

//***************************************************************************
// Format
//***************************************************************************

//---------------------------------------------------------------------------
void File_Riff::Read_Buffer_Init()
{
    //Buffer
    Element_Level=0;
    Element_Name[0]=0;
    Element_Next[0]=(int64u)-1; //Unlimited
    Element_ID=0;
    AVI__movi_Size=0;
    Rec_Present=false;
    TotalFrame=0;
    Trusted=10;
}

//---------------------------------------------------------------------------
void File_Riff::Read_Buffer_Continue()
{
    while (File_GoTo==0 && Buffer_Parse());
}

void File_Riff::Read_Buffer_Finalize ()
{
    //Finalizing streams
    for (Element_ID=0; (size_t)Element_ID<Codec_External.size(); Element_ID++)
    {
        if (Codec_External[Element_ID]!=NULL)
        {
            //Details
            if (Config.Details_Get())
            {
                Details_Add_Info(0, "Stream", Element_ID);
                Details_Level_Last++;
            }

            //Finalizing and Merging (except Video codec)
            Open_Buffer_Finalize(Codec_External[Element_ID]);
            SetLastByID(Element_ID);
            Ztring Codec_Temp;
            if (StreamKind_Last==Stream_Video)
                Codec_Temp=Get(StreamKind_Last, StreamPos_Last, _T("Codec")); //We want to keep the 4Cc of AVI
            Merge(*Codec_External[Element_ID], StreamKind_Last, 0, StreamPos_Last);
            if (StreamKind_Last==Stream_Video)
            {
                Fill("Codec", "");
                Fill("Codec", Codec_Temp);
            }
            if (Codec_External_StreamSize[Element_ID]>0)
                Fill("StreamSize", Codec_External_StreamSize[Element_ID]);
            Codec_External_PacketCount[Element_ID]=Error;

            //Specific
            #if defined(MEDIAINFO_MPEG4V_YES)
            if (StreamKind_Last==Stream_Video && Config.Codec_Get(Codec_External_Codec[Element_ID], InfoCodec_KindofCodec).find(_T("MPEG-4"))==0)
            {
                if (((File_Mpeg4v*)Codec_External[Element_ID])->RIFF_VOP_Count_Max>1)
                {
                    Fill("Codec_Settings/PacketBitStream", "Yes");
                    Fill("Codec_Settings", "Packed Bitstream");
                }
                else
                    Fill("Codec_Settings/PacketBitStream", "No");
            }
            #endif

            //Details
            if (Config.Details_Get())
                Details_Level_Last--;

            //Delete parsers
            delete Codec_External[Element_ID]; Codec_External[Element_ID]=NULL;
        }
    }

    //Some work on the first video stream
    if (!Video.empty())
    {
        //ODML
        if (TotalFrame!=0)
        {
            Fill(Stream_Video, 0, "FrameCount", "");
            Fill(Stream_Video, 0, "FrameCount", TotalFrame);

            float32 FrameRate=Get(Stream_Video, 0, _T("FrameRate")).To_float32();
            if (FrameRate!=0)
            {
                Fill(Stream_Video, 0, "PlayTime", "");
                Fill(Stream_Video, 0, "PlayTime", (int64u)(TotalFrame*1000/FrameRate));
            }
        }

        //Calculation of missing information
        int64u PlayTime=Get(Stream_Video, 0, _T("PlayTime")).To_int64u();
        if (PlayTime>0)
        {
            int32u BitRate=(int32u)(AVI__movi_Size*8*1000/PlayTime);
            Fill(Stream_General, 0, "BitRate", BitRate);
            if (BitRate>100000) //BitRate is > 100 000, to avoid strange behavior
            {
                int32u VideoBitRate=BitRate-5000; //5000 bps because of AVI OverHead (from GordianKnot)
                for (size_t Pos=0; Pos<Audio.size(); Pos++)
                {
                    int OverHead=8000; //Classic OverHead (for MP3) (from GordianKnot)
                    if (Audio[Pos](_T("Codec"))==_T("2000")) //AC3
                        OverHead=4750; //OverHead of AC3 in AVI (from GordianKnot)
                    VideoBitRate-=Audio[Pos](_T("BitRate")).To_int32s()+OverHead;
                }
                Fill(Stream_Video, 0, "BitRate", VideoBitRate);
            }
        }
    }

    //Rec
    if (Rec_Present)
        Fill(Stream_General, 0, "Codec_Settings", "rec");
}

//***************************************************************************
// Buffer
//***************************************************************************

//---------------------------------------------------------------------------
bool File_Riff::Buffer_Parse()
{
    //Integrity
    if (Element_Level>10)
    {
        File_Offset=File_Size;
        return false;
    }

    //End of this level?
    while (File_Offset+Buffer_Offset>=Element_Next[Element_Level])
        Element_Level--; //To next element

    //Going in a lower level
    Element_Level++;

    //Element Name
    if (!Element_Name_Get())
    {
        Element_Level--;
        return false;
    }

    //Element Size
    if (!Element_Size_Get())
    {
        Element_Level--;
        return false;
    }

    //Parsing
    if (!Element_Parse())
        return false; //Not enough bytes, Element_Level-- is done in Element_Parse() if needed

    //If no need of more
    if (File_Offset==File_Size || File_GoTo>0)
        return false;

    //Next element
    Buffer_Offset=Element_Next[Element_Level]-File_Offset;
    Element_Name[Element_Level]=0;
    Element_Next[Element_Level]=0;
    Element_Level--;
    return true;
}

//***************************************************************************
// Format
//***************************************************************************

#define DETAILLEVEL_SET(_VALUE) \
    Details_Level_Min=_VALUE; \

#define ATOM_BEGIN \
    if (Level!=Element_Level) \
    { \
        Level++; \
        switch (Element_Name[Level]) \
        {

    #define SKIP(_ATOM) \
            case Riff::_ATOM : \
                       break; \

    #define ATOM(_ATOM) \
            case Riff::_ATOM : \
                    if (Level==Element_Level) \
                    { \
                        if (Element_Next[Element_Level]-File_Offset<=Buffer_Size) \
                            _ATOM(); \
                        else \
                        { \
                            Buffer_Offset-=Element_HeaderSize; \
                            Element_Level--; \
                            PURGE(); \
                            return false; \
                        } \
                    } \
                    break; \

    #define ATOM_DEFAULT(_ATOM) \
            default : \
                    if (Level==Element_Level) \
                    { \
                        if (Element_Next[Element_Level]-File_Offset<=Buffer_Size) \
                            _ATOM(); \
                        else \
                        { \
                            Buffer_Offset-=Element_HeaderSize; \
                            Element_Level--; \
                            PURGE(); \
                            return false; \
                        } \
                    } \
                    break; \

    #define LIST(_ATOM) \
            case Riff::_ATOM : \
                    if (Level==Element_Level) \
                        _ATOM(); \

    #define LIST_DEFAULT(_ATOM) \
            default : \
                    if (Level==Element_Level) \
                        _ATOM(); \

#define ATOM_END \
        } \
    } \
    break; \

//---------------------------------------------------------------------------
bool File_Riff::Element_Parse()
{

    //Details
    if (Config.Details_Get())
    {
        DETAILLEVEL_SET(0)
        std::string S1;
        S1.append(1, (char)((Element_Name[Element_Level]&0xFF000000)>>24));
        S1.append(1, (char)((Element_Name[Element_Level]&0x00FF0000)>>16));
        S1.append(1, (char)((Element_Name[Element_Level]&0x0000FF00)>> 8));
        S1.append(1, (char)((Element_Name[Element_Level]&0x000000FF)>> 0));
        Ztring Z1;
        Z1.From_Local(S1.c_str());
        if (Z1.empty())
            Details_Add_Element(Element_Level-1, _T("(empty)"));
        else
            Details_Add_Element(Element_Level-1, Z1);
    }

    //Init
    Buffer_Offset+=Element_HeaderSize;
    int8u Level=0;
    List=Element_HeaderSize>8?true:false;

    ATOM_BEGIN
    LIST(AVI_)
        ATOM_BEGIN
        ATOM(AVI__cset)
        LIST(AVI__exif)
            ATOM_BEGIN
            ATOM_DEFAULT(AVI__exif_xxxx)
            ATOM_END
        LIST(AVI__goog)
            ATOM_BEGIN
            ATOM(AVI__goog_GDAT)
            ATOM_END
        ATOM(AVI__GMET)
        LIST(AVI__hdlr)
            ATOM_BEGIN
            ATOM(AVI__hdlr_avih)
            ATOM(AVI__hdlr_JUNK)
            LIST(AVI__hdlr_strl)
                ATOM_BEGIN
                ATOM(AVI__hdlr_strl_indx)
                ATOM(AVI__hdlr_strl_JUNK)
                ATOM(AVI__hdlr_strl_strf)
                ATOM(AVI__hdlr_strl_strh)
                ATOM(AVI__hdlr_strl_strn)
                ATOM(AVI__hdlr_strl_vprp)
                ATOM_END
            LIST(AVI__hdlr_odml)
                ATOM_BEGIN
                ATOM(AVI__hdlr_odml_dmlh)
                ATOM_END
            ATOM_DEFAULT(AVI__hdlr_xxxx)
            ATOM_END
        ATOM(AVI__idx1)
        LIST(AVI__INFO)
            ATOM_BEGIN
            ATOM(AVI__INFO_JUNK)
            ATOM_DEFAULT(AVI__INFO_xxxx)
            ATOM_END
        ATOM(AVI__JUNK)
        LIST(AVI__movi)
            ATOM_BEGIN
            LIST(AVI__movi_rec_)
                ATOM_BEGIN
                ATOM_DEFAULT(AVI__movi_xxxx)
                ATOM_END
            ATOM_DEFAULT(AVI__movi_xxxx)
            ATOM_END
        ATOM_END
    LIST(AVIX) //OpenDML
        ATOM_BEGIN
        ATOM(AVIX_idx1)
        LIST(AVIX_movi)
            ATOM_BEGIN
            LIST(AVIX_movi_rec_)
                ATOM_BEGIN
                ATOM_DEFAULT(AVIX_movi_xxxx)
                ATOM_END
            ATOM_DEFAULT(AVIX_movi_xxxx)
            ATOM_END
        ATOM_END
    LIST(menu)
        ATOM_BEGIN
        ATOM_END
    LIST(PAL_)
        ATOM_BEGIN
        ATOM_END
    LIST(RDIB)
        ATOM_BEGIN
        ATOM_END
    LIST(RMID)
        ATOM_BEGIN
        ATOM_END
    LIST(RMMP)
        ATOM_BEGIN
        ATOM_END
    LIST(WAVE)
        ATOM_BEGIN
        ATOM(WAVE_fmt_)
        ATOM(WAVE_fact)
        //This atom is too large, but we need only first bytes
        case Riff::WAVE_data :
                if (Level==Element_Level)
                {
                    if (Buffer_Size-Buffer_Offset>=1000)
                    {
                        WAVE_data();
                        return true;
                    }
                    else
                    {
                        Buffer_Offset-=Element_HeaderSize;
                        Element_Level--;
                        PURGE();
                        return false;
                    }
                }
        ATOM_END
    default : ;
    }}
    FLUSH();

    //List
    if (List)
    {
        Read_Buffer_Continue();
        if (Element_Next[Element_Level]-File_Offset>Buffer_Size)
            return false; //Not enough bytes
    }

    return true;
}

//***************************************************************************
// Elements
//***************************************************************************

#define NAME(ELEMENT_NAME) \
    if (Config.Details_Get()>=Details_Level_Min) \
    { \
        Details_Add_Element(ELEMENT_NAME); \
    } \

#define INTEGRITY(ELEMENT_SIZE) \
     if (Element_Size<ELEMENT_SIZE) \
        return; \

#define BEGIN \
    size_t Stream_Pos=0; \

#define END \
    { \
    } \

#define GET_I1(_INFO, _NAME) \
    { \
        INTEGRITY(Stream_Pos+1) \
        _INFO=LittleEndian2int8u(Buffer+Buffer_Offset+Stream_Pos); \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Error, #_NAME, _INFO); \
        } \
        Stream_Pos+=1; \
    }

#define GET_I2(_INFO, _NAME) \
    { \
        INTEGRITY(Stream_Pos+2) \
        _INFO=LittleEndian2int16u(Buffer+Buffer_Offset+Stream_Pos); \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Error, #_NAME, _INFO); \
        } \
        Stream_Pos+=2; \
    }

#define GET_I4(_INFO, _NAME) \
    { \
        INTEGRITY(Stream_Pos+4) \
        _INFO=LittleEndian2int32u(Buffer+Buffer_Offset+Stream_Pos); \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Error, #_NAME, _INFO); \
        } \
        Stream_Pos+=4; \
    }

#define GET_I8(_INFO, _NAME) \
    { \
        INTEGRITY(Stream_Pos+8) \
        _INFO=LittleEndian2int64u(Buffer+Buffer_Offset+Stream_Pos); \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Error, #_NAME, _INFO); \
        } \
        Stream_Pos+=8; \
    }

#define GET_CH(_BYTES, _INFO, _NAME) \
    { \
        INTEGRITY(Stream_Pos+_BYTES) \
        _INFO.assign((const char*)(Buffer+Buffer_Offset+Stream_Pos), _BYTES); \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Error, #_NAME, _INFO); \
        } \
        Stream_Pos+=_BYTES; \
    }

#define GET_C4(_INFO, _NAME) \
    { \
        INTEGRITY(Stream_Pos+4) \
        _INFO=CC4(Buffer+Buffer_Offset+Stream_Pos); \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Stream_Pos, #_NAME, Buffer+Buffer_Offset+Stream_Pos, 4); \
        } \
        Stream_Pos+=4; \
    }

#define SKIP_I1(_NAME) \
    { \
        INTEGRITY(Stream_Pos+1) \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Error, #_NAME, LittleEndian2int8u(Buffer+Buffer_Offset+Stream_Pos)); \
        } \
        Stream_Pos+=1; \
    } \

#define SKIP_I2(_NAME) \
    { \
        INTEGRITY(Stream_Pos+2) \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Error, #_NAME, LittleEndian2int16u(Buffer+Buffer_Offset+Stream_Pos)); \
        } \
        Stream_Pos+=2; \
    } \

#define SKIP_I4(_NAME) \
    { \
        INTEGRITY(Stream_Pos+4) \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Error, #_NAME, LittleEndian2int32u(Buffer+Buffer_Offset+Stream_Pos)); \
        } \
        Stream_Pos+=4; \
    } \

#define SKIP_I8(_NAME) \
    { \
        INTEGRITY(Stream_Pos+8) \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Error, #_NAME, LittleEndian2int64u(Buffer+Buffer_Offset+Stream_Pos)); \
        } \
        Stream_Pos+=8; \
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

#define SKIP_C4(_NAME) \
    { \
        INTEGRITY(Stream_Pos+4) \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Stream_Pos, #_NAME, Buffer+Buffer_Offset+Stream_Pos, 4); \
        } \
        Stream_Pos+=4; \
    } \

//---------------------------------------------------------------------------
// Chunk "AVI "
//
void File_Riff::AVI_()
{
    NAME("Format: AVI")

    //Test if there is only one AVI
    if (Count_Get(Stream_General)>0)
    {
        //Details
        if (Config.Details_Get())
        {
            Details_Add_Info(0, "Problem", "2 AVI chunks, this is not normal");
        }

        //Problem, we go to the end
        File_GoTo=File_Size;
        return;
    }

    //Filling
    Stream_Prepare(Stream_General);
    Fill("Format", "AVI");
}

//---------------------------------------------------------------------------
// Chunk "CSET", 8 bytes
// CodePage                         2 bytes, Pos=0
// CountryCode                      2 bytes, Pos=2
// LanguageCode                     2 bytes, Pos=4
// Dialect                          2 bytes, Pos=6
//
void File_Riff::AVI__cset()
{
    //TODO: take a look about IBM/MS RIFF/MCI Specification 1.0
}

//---------------------------------------------------------------------------
// List of Exif atoms
//
void File_Riff::AVI__exif()
{
    NAME ("Exchangeable Image File Format")
}

//---------------------------------------------------------------------------
// Exif atom
// Name                             X bytes, Pos=0
//
void File_Riff::AVI__exif_xxxx()
{
    NAME ("Value")

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(0, "Value", Buffer+Buffer_Offset+0, Element_Size);
    }

    //Coherancy test
    if (Count_Get(Stream_General)==0)
        return;

    switch (Element_Name[Element_Level])
    {
        case Riff::AVI__exif_ecor : Fill(Stream_General, 0, "Make", (const char*)(Buffer+Buffer_Offset), Element_Size); break;
        case Riff::AVI__exif_emdl : Fill(Stream_General, 0, "Model", (const char*)(Buffer+Buffer_Offset), Element_Size); break;
        case Riff::AVI__exif_emnt : Fill(Stream_General, 0, "MakerNotes", (const char*)(Buffer+Buffer_Offset), Element_Size); break;
        case Riff::AVI__exif_erel : Fill(Stream_General, 0, "RelatedImageFile", (const char*)(Buffer+Buffer_Offset), Element_Size); break;
        case Riff::AVI__exif_etim : Fill(Stream_General, 0, "Written_Date", (const char*)(Buffer+Buffer_Offset), Element_Size); break;
        case Riff::AVI__exif_eucm : Fill(Stream_General, 0, "Comment", (const char*)(Buffer+Buffer_Offset), Element_Size); break;
        case Riff::AVI__exif_ever : break; //Exif version
        default: Fill(Stream_General, 0, Ztring().From_Local((char*)Buffer+Buffer_Offset-8, 4).To_Local().c_str(), (const char*)(Buffer+Buffer_Offset), Element_Size);
    }
}

//---------------------------------------------------------------------------
// Google specific
//
void File_Riff::AVI__goog()
{
    NAME ("Google specific")

    Fill("Format", "");
    Fill("Format", "GVI");
}

//---------------------------------------------------------------------------
// Google specific
//
void File_Riff::AVI__goog_GDAT()
{
    NAME ("Google datas")

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(0, "Unknown", "Unknown");
    }
}

//---------------------------------------------------------------------------
// Google Metadata
//
void File_Riff::AVI__GMET()
{
    NAME ("Google Metadatas")

    Ztring Value; Value.From_Local((const char*)(Buffer+Buffer_Offset+0), Element_Size);
    ZtringListList List;
    List.Separator_Set(0, _T("\n"));
    List.Separator_Set(1, _T(":"));
    List.Max_Set(1, 2);
    List.Write(Value);

    //Details
    if (Config.Details_Get())
    {
        for (size_t Pos=0; Pos<List.size(); Pos++)
            Details_Add_Info(Pos, List(Pos, 0).To_Local().c_str(), List(Pos, 1));
    }

    for (size_t Pos=0; Pos<List.size(); Pos++)
    {
        if (List(Pos, 0)==_T("gvp_version"));
        if (List(Pos, 0)==_T("duration"));
        if (List(Pos, 0)==_T("title")) Fill(Stream_General, 0, "Title", List(Pos, 1));
        if (List(Pos, 0)==_T("description")) Fill(Stream_General, 0, "Title/More", List(Pos, 1));
        if (List(Pos, 0)==_T("url")) Fill(Stream_General, 0, "Title/Url", List(Pos, 1));
        if (List(Pos, 0)==_T("docid")) Fill(Stream_General, 0, "UniqueID", List(Pos, 1));
    }
}

//---------------------------------------------------------------------------
// Chunk "hdlr"
//
void File_Riff::AVI__hdlr()
{
    NAME("Header")
}

//---------------------------------------------------------------------------
// Chunk "avih", 56 bytes
// MicrosecPerFrame                 4 bytes, Pos=0
// MaxBytesPerSec                   4 bytes, Pos=4
// PaddingGranularity               4 bytes, Pos=8
// Flags                            4 bytes, Pos=12
// TotalFrames                      4 bytes, Pos=16 (Only for the first AVI chunk)
// InitialFrames                    4 bytes, Pos=20
// StreamsCount                     4 bytes, Pos=24
// SuggestedBufferSize              4 bytes, Pos=28
// Width                            4 bytes, Pos=32
// Height                           4 bytes, Pos=36
// Reserved                         16 bytes, Pos=40
//
// Flags:
// Has index                        0x00000010
// Must use index                   0x00000020
// Is interleaved                   0x00000100
// Use CK type to find key frames   0x00000800
// Was capture file                 0x00010000
// Copyrighted                      0x00020000
//
void File_Riff::AVI__hdlr_avih()
{
    NAME("AVI header")
    INTEGRITY(56)

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(0, "MicrosecPerFrame", LittleEndian2int32u(Buffer+Buffer_Offset+0));
        Details_Add_Info(4, "MaxBytesPerSec", LittleEndian2int32u(Buffer+Buffer_Offset+4));
        Details_Add_Info(8, "PaddingGranularity", LittleEndian2int32u(Buffer+Buffer_Offset+8));
        Details_Add_Info(12, "Flags", LittleEndian2int32u(Buffer+Buffer_Offset+12), 16);
        Details_Add_Info(16, "TotalFrames", LittleEndian2int32u(Buffer+Buffer_Offset+16));
        Details_Add_Info(20, "InitialFrames", LittleEndian2int32u(Buffer+Buffer_Offset+20));
        Details_Add_Info(24, "StreamsCount", LittleEndian2int32u(Buffer+Buffer_Offset+24));
        Details_Add_Info(28, "SuggestedBufferSize", LittleEndian2int32u(Buffer+Buffer_Offset+28));
        Details_Add_Info(32, "Width", LittleEndian2int32u(Buffer+Buffer_Offset+32));
        Details_Add_Info(36, "Height", LittleEndian2int32u(Buffer+Buffer_Offset+36));
        Details_Add_Info(40, "Reserved", "(16 bytes)");
    }

    //Keep some info in memory
    avih_TotalFrame=LittleEndian2int32u(Buffer+Buffer_Offset+16); //TotalFrames
    avih_FrameRate=1000000.0/LittleEndian2int32u(Buffer+Buffer_Offset+0);

    //Filling
    //Fill(Stream_General, 0, "PlayTime", (int32u)(avih_TotalFrame*1000/avih_FrameRate)); //not trustable
}

//---------------------------------------------------------------------------
void File_Riff::AVI__hdlr_JUNK()
{
    NAME("Garbage")
}

//---------------------------------------------------------------------------
// Chunk "odml", this is a list
void File_Riff::AVI__hdlr_odml()
{
    NAME("OpenDML")
}

//---------------------------------------------------------------------------
// Chunk "dmlh", 4 bytes for fccType "auds"
// GrandFrames                      2 bytes, Pos=0
//
void File_Riff::AVI__hdlr_odml_dmlh()
{
    NAME("OpenDML header")
    INTEGRITY(2)
    TotalFrame=LittleEndian2int32u(Buffer+Buffer_Offset+0); //GrandFrames

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(0, "GrandFrames", LittleEndian2int16u(Buffer+Buffer_Offset+0));
    }

}

//---------------------------------------------------------------------------
// Chunk "strl", this is a list
void File_Riff::AVI__hdlr_strl()
{
    NAME("Stream info")

    //Reset internal datas
    AVI__hdlr_strl_strh_StreamType=0;
    AVI__hdlr_strl_strh_Scale=0;
    AVI__hdlr_strl_strh_Rate=0;
    AVI__hdlr_strl_strh_Length=0;
}

//---------------------------------------------------------------------------
void File_Riff::AVI__hdlr_strl_indx()
{
    NAME("Index")

    int16u Entry_Size, Entry_Count; int8u Type, SubType;
    BEGIN
    GET_I2 (Entry_Size,                                         LongsPerEntry) //Size of each entry in aIndex array
    GET_I1 (SubType,                                            IndexSubType) //Future use. must be 0
    //#define AVI_INDEX_2FIELD     0x01 // when fields within frames are also indexed
    GET_I1 (Type,                                               IndexType) //One of AVI_INDEX_* codes
    //#define AVI_INDEX_OF_INDEXES 0x00 // when each entry in aIndex array points to an index chunk
    //#define AVI_INDEX_OF_CHUNKS  0x01 // when each entry in aIndex array points to a chunk in the file
    //#define AVI_INDEX_IS_DATA    0x80 // when each entry is aIndex is really the data bIndexSubtype codes for INDEX_OF_CHUNKS
    GET_I4 (Entry_Count,                                        EntriesInUse) //Index of first unused member in aIndex array
    SKIP_C4(                                                    ChunkId) //FCC of what is indexed

    //Depends of size of structure...
          if (Entry_Size==2)
    {
        //Index of Indexes
             if (Type==0x00)
        {
            //Standard
                 if (SubType==0x00)
            {
                //Standard Index Chunk
                SKIP_I8 (                                       Offset)
                SKIP_I4 (                                       Reserved2)
                for (int16u Pos=0; Pos<Entry_Count; Pos++)
                {
                    SKIP_I4 (                                   Offset) //BaseOffset + this is absolute file offset
                    SKIP_I4 (                                   Size) //Bit 31 is set if this is NOT a keyframe
                }
            }
        }
        //Index of Chunks
        else if (Type==0x01)
        {
            //Index to Fields
                 if (SubType==0x01)
            {
                //Field Index Chunk
                SKIP_I8 (                                       Offset)
                SKIP_I4 (                                       Reserved2)
                for (int16u Pos=0; Pos<Entry_Count; Pos++)
                {
                    SKIP_I4 (                                   Offset) //BaseOffset + this is absolute file offset
                    SKIP_I4 (                                   Size) //Bit 31 is set if this is NOT a keyframe
                    SKIP_I4 (                                   OffsetField2) //Offset to second field
                }
            }
        }
    }
    //Super Index Chunk
    else if (Entry_Size==4)
    {
        //Index of Indexes
             if (Type==0x00)
        {
            //Standard or Index to Fields
                 if (SubType==0x00 || SubType==0x01)
            {
                SKIP_I4 (                                       Reserved0)
                SKIP_I4 (                                       Reserved1)
                SKIP_I4 (                                       Reserved2)
                for (int16u Pos=0; Pos<Entry_Count; Pos++)
                {
                    SKIP_I8 (                                   Offset)
                    SKIP_I4 (                                   Size) //Size of index chunk at this offset
                    SKIP_I4 (                                   Duration) //time span in stream ticks
                }
            }
        }
    }
}

//---------------------------------------------------------------------------
void File_Riff::AVI__hdlr_strl_JUNK()
{
    NAME("Garbage")
}

//---------------------------------------------------------------------------
void File_Riff::AVI__hdlr_strl_strd()
{
    NAME("Stream datas")
}

//---------------------------------------------------------------------------
void File_Riff::AVI__hdlr_strl_strf()
{
    NAME("Stream format")

    //Coherancy test
    if (AVI__hdlr_strl_strh_StreamType==0)
        return;

    //Add the codec to the codec list
    Codec_External.push_back(NULL);
    Codec_External_PacketCount.push_back(0);
    Codec_External_StreamSize.push_back(0);
    Element_ID=Codec_External_Codec.size();

    //Parse depending of kind of stream
    switch (AVI__hdlr_strl_strh_StreamType)
    {
        case Riff::AVI__hdlr_strl_strh_auds : AVI__hdlr_strl_strf_auds(); break;
        case Riff::AVI__hdlr_strl_strh_iavs : AVI__hdlr_strl_strf_iavs(); break;
        case Riff::AVI__hdlr_strl_strh_mids : AVI__hdlr_strl_strf_mids(); break;
        case Riff::AVI__hdlr_strl_strh_txts : AVI__hdlr_strl_strf_txts(); break;
        case Riff::AVI__hdlr_strl_strh_vids : AVI__hdlr_strl_strf_vids(); break;
        default :;
    }

    //Add the codec to the codec list
    Codec_External_Codec.push_back(Get(StreamKind_Last, StreamPos_Last, _T("Codec")));//After because we don't have the codec name
    Fill("ID", Element_ID);
}

//---------------------------------------------------------------------------
// Chunk "strf", at least 16 bytes for fccType "auds"
// FormatTag                        2 bytes, Pos=0
// Channels                         2 bytes, Pos=2
// SamplesPerSec                    4 bytes, Pos=4
// AvgBytesPerSec                   4 bytes, Pos=8
// BlockAlign                       2 bytes, Pos=12
// BitsPerSample                    2 bytes, Pos=14
// cbSize                           2 bytes, Pos=16
// Options
//
// Options could be:
// ValidBitsPerSample               2 bytes, Pos=18 (Optional)
// SamplesPerBlock                  2 bytes, Pos=20 (Optional)
// Reserved                         2 bytes, Pos=22 (Optional)
// ChannelMask                      4 bytes, Pos=24 (Optional)
// SubFormat                        16 bytes, Pos=28 (Optional)
//
// For FormatTag=0x0055
// ID                               2 bytes
// Flags                            4 bytes
// BlockSize                        2 bytes
// FramesPerBlock                   2 bytes
// CodecDelay                       2 bytes
//
void File_Riff::AVI__hdlr_strl_strf_auds()
{
    NAME("Audio")
    INTEGRITY(16)

    //Coherancy test
    if (Element_Size<16) //Size of atom
        return;

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(0, "FormatTag", LittleEndian2int16u(Buffer+Buffer_Offset+0), 16);
        Details_Add_Info(2, "Channels", LittleEndian2int16u(Buffer+Buffer_Offset+2));
        Details_Add_Info(4, "SamplesPerSec", LittleEndian2int32u(Buffer+Buffer_Offset+4));
        Details_Add_Info(8, "AvgBytesPerSec", LittleEndian2int32u(Buffer+Buffer_Offset+8));
        Details_Add_Info(12, "BlockAlign", LittleEndian2int16u(Buffer+Buffer_Offset+12));
        Details_Add_Info(14, "BitsPerSample", LittleEndian2int16u(Buffer+Buffer_Offset+14));
    }

    //Fill Audio datas
    Stream_Prepare(Stream_Audio);
    Ztring Codec; Codec.From_Number(LittleEndian2int16u(Buffer+Buffer_Offset+0), 16);
    Codec.MakeUpperCase();
    Fill("Codec", Codec); //FormatTag
    Fill("Channel(s)", LittleEndian2int16u(Buffer+Buffer_Offset+2)!=5?LittleEndian2int16u(Buffer+Buffer_Offset+2):6); //Channels, some workaround about AC3 (for example) with subwoofer
    Fill("SamplingRate", LittleEndian2int16u(Buffer+Buffer_Offset+4));
    Fill("BitRate", LittleEndian2int32u(Buffer+Buffer_Offset+8)*8); //AvgBytesPerSec
    if (LittleEndian2int16u(Buffer+Buffer_Offset+14))
        Fill("Resolution", LittleEndian2int16u(Buffer+Buffer_Offset+14)); //BitsPerSample

    //From stream header
    if (AVI__hdlr_strl_strh_Rate)
    {
        int64u PlayTime=0;
        float32 FrameRate=((float)AVI__hdlr_strl_strh_Rate)/AVI__hdlr_strl_strh_Scale;
        if (FrameRate)
            PlayTime=float32_int64s((1000*(float)AVI__hdlr_strl_strh_Length)/FrameRate);
        if (avih_TotalFrame>0 //avih_TotalFrame is here because some files have a wrong Audio Playtime if TotalFrame==0 (which is a bug, of course!)
        && (avih_FrameRate==0 || PlayTime<((float)avih_TotalFrame)/avih_FrameRate*1000*1.10) //Some file have a nearly perfect header, except that the value is false, trying to detect it (false if 10% more than 1st video)
        && (avih_FrameRate==0 || PlayTime>((float)avih_TotalFrame)/avih_FrameRate*1000*0.90)) //Some file have a nearly perfect header, except that the value is false, trying to detect it (false if 10% less than 1st video)
            Fill("PlayTime", PlayTime);
        else
            Fill("Coherency/PlayTime", PlayTime);
    }

    //Creating the parser
    Codec.MakeUpperCase();
         if (0);
    #if defined(MEDIAINFO_MPEGA_YES)
    else if (Config.Codec_Get(Codec, InfoCodec_KindofCodec).find(_T("MPEG-1"))==0)
        Codec_External[Element_ID]=new File_Mpega;
    #endif
    #if defined(MEDIAINFO_AC3_YES)
    else if (Codec==_T("2000"))
        Codec_External[Element_ID]=new File_Ac3;
    #endif
    #if defined(MEDIAINFO_DTS_YES)
    else if (Codec==_T("2001") || Codec==_T("1")) //Some DTS streams are coded "1"
        Codec_External[Element_ID]=new File_Dts;
    #endif
    #if defined(MEDIAINFO_AAC_YES)
    else if (Codec==_T("AAC") || Codec==_T("FF"))
        Codec_External[Element_ID]=new File_Aac;
    #endif

    //Options
    INTEGRITY(18)

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(16, "cbSize", LittleEndian2int16u(Buffer+Buffer_Offset+16));
    }

    //Analysing
    int16u Option_Size=LittleEndian2int16u(Buffer+Buffer_Offset+16);
    if (Option_Size>0)
    {
        size_t Element_Size_Old=Element_Size;
        Element_Size=Option_Size;
        Buffer_Offset+=18;
        if (Config.Details_Get())
        {
            Details_Add_Element(Element_Level, _T("Options"));
        }
        AVI__hdlr_strl_strf_auds_Options();
        Buffer_Offset-=18;
        Element_Size=Element_Size_Old;
    }
}

void File_Riff::AVI__hdlr_strl_strf_auds_Options()
{
    if (Element_Size==0)
        return; //No options

    const Ztring& Codec=Get(StreamKind_Last, StreamPos_Last, _T("Codec"));

    //MP3
    if (Codec==_T("55"))
    {
        NAME("MPEG Audio")
        INTEGRITY(12)

        //Details
        if (Config.Details_Get())
        {
            Details_Add_Info(0, "ID", LittleEndian2int16u(Buffer+Buffer_Offset+0));
            Details_Add_Info(2, "Flags", LittleEndian2int32u(Buffer+Buffer_Offset+2), 16);
            Details_Add_Info(6, "BlockSize", LittleEndian2int16u(Buffer+Buffer_Offset+6));
            Details_Add_Info(8, "FramesPerBlock", LittleEndian2int16u(Buffer+Buffer_Offset+8));
            Details_Add_Info(10, "CodecDelay", LittleEndian2int16u(Buffer+Buffer_Offset+10));
        }
    }

    //AAC
    #if defined(MEDIAINFO_AAC_YES)
        if (Codec==_T("AAC") || Codec==_T("FF"))
        {
            NAME("AAC")

            //Reading
            File_Aac MI;
            Open_Buffer_Init(&MI, File_Size, File_Offset+Buffer_Offset);
            Open_Buffer_Continue(&MI, Buffer+Buffer_Offset, Element_Size);
            Open_Buffer_Finalize(&MI);

            //Filling
            Merge(MI, StreamKind_Last, 0, StreamPos_Last);
        }
    #endif

    //Template
    /* Must be confirmed
    if (Element_Size==10)
    {
        int32u Position=LittleEndian2int32u(Buffer+Buffer_Offset+8);

        //Details
        if (Config.Details_Get())
        {
            Details_Add_Info(0, "Resolution", LittleEndian2int16u(Buffer+Buffer_Offset+0));
            Details_Add_Info(2, "Unknown", "");
            Details_Add_Info(8, "ChannelPositions", Position, 16);
        }

        Fill("Resolution", LittleEndian2int16u(Buffer+Buffer_Offset+0));
        Ztring Front, Middle, Rear, Top, Surround, ChannelPositions;
        if (Position&0x00000001) Front   +=_T(" L");
        if (Position&0x00000004) Front   +=_T(" C");
        if (Position&0x00000040) Front   +=_T(" C"); //Center Left
        if (Position&0x00000080) Front   +=_T(" C"); //Center Rigth
        if (Position&0x00000002) Front   +=_T(" R");
        if (Position&0x00000200) Middle  +=_T(" L");
        if (Position&0x00000400) Middle  +=_T(" R");
        if (Position&0x00000010) Rear    +=_T(" L");
        if (Position&0x00000100) Rear    +=_T(" C");
        if (Position&0x00000020) Rear    +=_T(" R");
        if (Position&0x00000008) Surround+=_T(" C");
        if (Position&0x00000800) Top     +=_T(" C");
        if (Position&0x00000800) Top     +=_T(" FL");
        if (Position&0x00002000) Top     +=_T(" FC");
        if (Position&0x00001000) Top     +=_T(" FR");
        if (Position&0x00000800) Top     +=_T(" RL");
        if (Position&0x00010000) Top     +=_T(" RC");
        if (Position&0x00020000) Top     +=_T(" RR");
        if (!Front.empty()   ) {ChannelPositions+=_T("Front"); ChannelPositions+=_T(": "); ChannelPositions+=Front   ; ChannelPositions+=_T(", ");}
        if (!Middle.empty()  ) {ChannelPositions+=_T("Front"); ChannelPositions+=_T(": "); ChannelPositions+=Middle  ; ChannelPositions+=_T(", ");}
        if (!Rear.empty()    ) {ChannelPositions+=_T("Front"); ChannelPositions+=_T(": "); ChannelPositions+=Rear    ; ChannelPositions+=_T(", ");}
        if (!Surround.empty()) {ChannelPositions+=_T("Front"); ChannelPositions+=_T(": "); ChannelPositions+=Surround; ChannelPositions+=_T(", ");}
        if (!Top.empty()     ) {ChannelPositions+=_T("Front"); ChannelPositions+=_T(": "); ChannelPositions+=Top     ; ChannelPositions+=_T(", ");}
        if (ChannelPositions.size()>2)
            ChannelPositions.resize(ChannelPositions.size()-2);
        Fill("ChannelPositions", ChannelPositions);
    }
    */
}

//---------------------------------------------------------------------------
// Chunk "strf", 32 bytes for fccType "iavs"
// DVAAuxSrc                        4 bytes, Pos=0
// DVAAuxCtl                        4 bytes, Pos=4
// DVAAuxSrc1                       4 bytes, Pos=8
// DVAAuxCtl1                       4 bytes, Pos=12
// DVVAuxSrc                        4 bytes, Pos=16
// DVVAuxCtl                        4 bytes, Pos=20
// DVReserved                       8 bytes, Pos=24
//
void File_Riff::AVI__hdlr_strl_strf_iavs()
{
    //Standard video header before Iavs?
    size_t Iavs_Offset=0;
    if (Element_Size==72)
    {
        AVI__hdlr_strl_strf_vids();
        Iavs_Offset=40;
    }
    else
    {
        NAME("Interleaved Audio/Video")
        INTEGRITY(32)
    }

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(0+Iavs_Offset, "DVAAuxSrc", LittleEndian2int32u(Buffer+Buffer_Offset+Iavs_Offset+0), 16);
        Details_Add_Info(4+Iavs_Offset, "DVAAuxCtl", LittleEndian2int32u(Buffer+Buffer_Offset+Iavs_Offset+4), 16);
        Details_Add_Info(8+Iavs_Offset, "DVAAuxSrc1", LittleEndian2int32u(Buffer+Buffer_Offset+Iavs_Offset+8), 16);
        Details_Add_Info(12+Iavs_Offset, "DVAAuxCtl1", LittleEndian2int32u(Buffer+Buffer_Offset+Iavs_Offset+12), 16);
        Details_Add_Info(16+Iavs_Offset, "DVVAuxSrc", LittleEndian2int32u(Buffer+Buffer_Offset+Iavs_Offset+16), 16);
        Details_Add_Info(20+Iavs_Offset, "DVVAuxCtl", LittleEndian2int32u(Buffer+Buffer_Offset+Iavs_Offset+20), 16);
        Details_Add_Info(24+Iavs_Offset, "DVReserved", LittleEndian2int32u(Buffer+Buffer_Offset+Iavs_Offset+24), 16);
    }

    //Filling - Video
    if (Iavs_Offset==0)
    {
        Stream_Prepare(Stream_Video);
        Fill("Codec", AVI__hdlr_strl_strh_Codec);
    }

    //FrameRate
    float FrameRate=0;
    if (AVI__hdlr_strl_strh_Scale!=0)
    {
        FrameRate=((float)AVI__hdlr_strl_strh_Rate)/AVI__hdlr_strl_strh_Scale;
        if (avih_FrameRate==0)
            avih_FrameRate=FrameRate; //Keep it in Memory
        Fill("FrameRate", FrameRate, 3);
        if (FrameRate)
            Fill("PlayTime", (1000*(float)AVI__hdlr_strl_strh_Length)/FrameRate);
    }

    //Fill datas depend of the codec
         if (AVI__hdlr_strl_strh_Codec==_T("dvsd") || AVI__hdlr_strl_strh_Codec==_T("dvsl"))
    {
                                    Fill("Width",  720);
             if (FrameRate==25.000) Fill("Height", 576);
        else if (FrameRate==29.970) Fill("Height", 480);
        Fill("AspectRatio", ((float)4)/3);
    }
    else if (AVI__hdlr_strl_strh_Codec==_T("dvhd"))
    {
                                    Fill("Width",  1440);
             if (FrameRate==25.000) Fill("Height", 1152);
        else if (FrameRate==30.000) Fill("Height",  960);
        Fill("AspectRatio", ((float)4)/3);
    }

    //Filling - Audio
    if (Iavs_Offset==0)
    {
        Stream_Prepare(Stream_Audio);
        Fill("Codec", AVI__hdlr_strl_strh_Codec);
    }
}

//---------------------------------------------------------------------------
// Chunk "strf", unknown size for fccType "mids"
// Unknown format
//
void File_Riff::AVI__hdlr_strl_strf_mids()
{
    NAME("Midi")

    Stream_Prepare(Stream_Audio);
    Fill("Codec", "Midi");
}

//---------------------------------------------------------------------------
// Chunk "strf", unknown size for fccType "txts"
// Unknown format
//
void File_Riff::AVI__hdlr_strl_strf_txts()
{
    NAME("Text")

    Stream_Prepare(Stream_Text);
    Fill("Codec", "SRT");
}

//---------------------------------------------------------------------------
// Chunk "strf", 40 bytes for fccType "vids"
// Size                             4 bytes, Pos=0
// Width                            4 bytes, Pos=4
// Height                           4 bytes, Pos=8
// Planes                           2 bytes, Pos=12
// BitCount                         2 bytes, Pos=14
// Compression                      4 bytes, Pos=16
// SizeImage                        4 bytes, Pos=20
// XPelsPerMeter                    4 bytes, Pos=24
// YPelsPerMeter                    4 bytes, Pos=28
// ClrUsed                          4 bytes, Pos=32
// ClrImportant                     4 bytes, Pos=36
void File_Riff::AVI__hdlr_strl_strf_vids()
{
    NAME("Video")
    INTEGRITY(40)

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(0, "Size", LittleEndian2int32u(Buffer+Buffer_Offset+0), 16);
        Details_Add_Info(4, "Width", LittleEndian2int32u(Buffer+Buffer_Offset+4));
        Details_Add_Info(8, "Height", LittleEndian2int32u(Buffer+Buffer_Offset+8));
        Details_Add_Info(12, "Planes", LittleEndian2int16u(Buffer+Buffer_Offset+12));
        Details_Add_Info(14, "BitCount", LittleEndian2int16u(Buffer+Buffer_Offset+14));
        Details_Add_Info(16, "Compression", (const char*)(Buffer+Buffer_Offset+16), 4);
        Details_Add_Info(20, "SizeImage", LittleEndian2int32u(Buffer+Buffer_Offset+20));
        Details_Add_Info(24, "XPelsPerMeter", LittleEndian2int32u(Buffer+Buffer_Offset+24));
        Details_Add_Info(28, "YPelsPerMeter", LittleEndian2int32u(Buffer+Buffer_Offset+28));
        Details_Add_Info(32, "ClrUsed", LittleEndian2int32u(Buffer+Buffer_Offset+32));
        Details_Add_Info(36, "ClrImportant", LittleEndian2int32u(Buffer+Buffer_Offset+36));
    }

    //Filling
    int32u Compression=CC4(Buffer+Buffer_Offset+16); //Compression
    Ztring Codec;
    if (((Compression&0x000000FF)>=0x00000020 && (Compression&0x000000FF)<=0x0000007E
      && (Compression&0x0000FF00)>=0x00002000 && (Compression&0x0000FF00)<=0x00007E00
      && (Compression&0x00FF0000)>=0x00200000 && (Compression&0x00FF0000)<=0x007E0000
      && (Compression&0xFF000000)>=0x20000000 && (Compression&0xFF000000)<=0x7E000000)
     ||   Compression==0x00000000
       ) //Sometimes this value is wrong, we have to test this
    {
        //Divx.com hack for subtitle, this is a text stream in a DivX container
        if (Compression==CC4("DXSB"))
        {
            Fill(Stream_General, 0, "Format", "");
            Fill(Stream_General, 0, "Format", "DivX");
            Stream_Prepare(Stream_Text);
        }
        else
            Stream_Prepare(Stream_Video);

        //Filling
        Codec.From_UTF8((const char*)(Buffer+Buffer_Offset+16), 4);
        if (Compression==0x00000000)
            Fill("Codec", "RGB"); //Raw RGB, not handled by automatic codec mapping
        else
            Fill("Codec", Codec); //Compression
        Fill("Width", LittleEndian2int32u(Buffer+Buffer_Offset+4)); //Width
        Fill("Height", LittleEndian2int32u(Buffer+Buffer_Offset+8)); //Height
    }
    else
    {
        //Some Stream headers are broken, must use AVISTREAMINFOA structure instead of AVIFILEINFOA
        Stream_Prepare(Stream_Video);
        Codec=AVI__hdlr_strl_strh_Codec;
        Fill("Codec", AVI__hdlr_strl_strh_Codec); //StreamHeader fcchandler
        Fill("Width", AVI__hdlr_strl_strh_Width?AVI__hdlr_strl_strh_Width:LittleEndian2int32u(Buffer+Buffer_Offset+4)); //StreamHeader Width if exists
        Fill("Height", AVI__hdlr_strl_strh_Height?AVI__hdlr_strl_strh_Height:LittleEndian2int32u(Buffer+Buffer_Offset+8)); //StreamHeader Height if exists
    }

    //Don't calculate video information for DXSB, this is text
    if (Compression==CC4("DXSB"))
        return;

    //Framerate, Playtime
    if (AVI__hdlr_strl_strh_Scale!=0)
    {
        Fill("FrameCount", AVI__hdlr_strl_strh_Length);
        float32 FrameRate=((float)AVI__hdlr_strl_strh_Rate)/AVI__hdlr_strl_strh_Scale;
        if (avih_FrameRate==0)
            avih_FrameRate=FrameRate; //Keep it in Memory
        Fill("FrameRate", FrameRate, 3);
        if (FrameRate)
            Fill("PlayTime", (int32u)(1000*(float)AVI__hdlr_strl_strh_Length)/FrameRate);
    }

    //Creating the parser
         if (0);
    #if defined(MEDIAINFO_MPEG4V_YES)
    else if (Config.Codec_Get(Codec, InfoCodec_KindofCodec).find(_T("MPEG-4"))==0)
        Codec_External[Element_ID]=new File_Mpeg4v;
    #endif
}

//---------------------------------------------------------------------------
// Chunk "strh", 56 bytes
// fccType                          4 bytes, Pos=0
// fccHandler                       4 bytes, Pos=4
// Flags                            4 bytes, Pos=8
// Priority                         2 bytes, Pos=12
// Language                         2 bytes, Pos=14
// InitialFrames                    4 bytes, Pos=16
// Scale                            4 bytes, Pos=20
// Rate                             4 bytes, Pos=24 (Rate/Scale is stream tick rate in ticks/sec)
// Start                            4 bytes, Pos=28
// Length                           4 bytes, Pos=32
// SuggestedBufferSize              4 bytes, Pos=36
// Quality                          4 bytes, Pos=40
// SampleSize                       4 bytes, Pos=44
// Frame_Left                       2 bytes, Pos=48
// Frame_Top                        2 bytes, Pos=50
// Frame_Rigth                      2 bytes, Pos=52
// Frame_Bottom                     2 bytes, Pos=54
//
// Flags:
// AVISF_DISABLED                   0x???????? Stream should not be activated by default
// AVISF_VIDEO_PALCHANGES           0x???????? Stream is a video stream using palettes where the palette is changing during playback.
//
void File_Riff::AVI__hdlr_strl_strh()
{
    NAME("Stream header")
    INTEGRITY(56)

    //Save info for later use
    //Strh_Offset=Offset;
    AVI__hdlr_strl_strh_StreamType=CC4(Buffer+Buffer_Offset+0); //fccType
    AVI__hdlr_strl_strh_Scale=LittleEndian2int32u(Buffer+Buffer_Offset+20); //Scale
    AVI__hdlr_strl_strh_Rate=LittleEndian2int32u(Buffer+Buffer_Offset+24); //Rate
    AVI__hdlr_strl_strh_Length=LittleEndian2int32u(Buffer+Buffer_Offset+32); //Length
    AVI__hdlr_strl_strh_Codec.From_Local((const char*)(Buffer+Buffer_Offset+4), 4); //fccHandler
    AVI__hdlr_strl_strh_Width=LittleEndian2int16u(Buffer+Buffer_Offset+52)-LittleEndian2int16u(Buffer+Buffer_Offset+48); //Frame_Rigth-Frame_Left
    AVI__hdlr_strl_strh_Height=LittleEndian2int16u(Buffer+Buffer_Offset+54)-LittleEndian2int16u(Buffer+Buffer_Offset+50); //Frame_Bottom-Frame_Top

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(0, "fccType", Buffer+Buffer_Offset+0, 4);
        switch (AVI__hdlr_strl_strh_StreamType)
        {
            case Riff::AVI__hdlr_strl_strh_auds : Details_Add_Info(4, "fccHandler", LittleEndian2int32u(Buffer+Buffer_Offset+8), 16); break;
            case Riff::AVI__hdlr_strl_strh_iavs : Details_Add_Info(4, "fccHandler", LittleEndian2int32u(Buffer+Buffer_Offset+8), 16); break;
            case Riff::AVI__hdlr_strl_strh_mids : Details_Add_Info(4, "fccHandler", LittleEndian2int32u(Buffer+Buffer_Offset+8), 16); break;
            case Riff::AVI__hdlr_strl_strh_vids : Details_Add_Info(4, "fccHandler", Buffer+Buffer_Offset+4, 4); break;
            case Riff::AVI__hdlr_strl_strh_txts : Details_Add_Info(4, "fccHandler", LittleEndian2int32u(Buffer+Buffer_Offset+8), 16); break;
        }
        Details_Add_Info(8, "Flags", LittleEndian2int32u(Buffer+Buffer_Offset+8), 16);
        Details_Add_Info(12, "Priority", LittleEndian2int16u(Buffer+Buffer_Offset+12));
        Details_Add_Info(14, "Language", LittleEndian2int16u(Buffer+Buffer_Offset+14));
        Details_Add_Info(16, "InitialFrames", LittleEndian2int32u(Buffer+Buffer_Offset+16));
        Details_Add_Info(20, "Scale", LittleEndian2int32u(Buffer+Buffer_Offset+20));
        Details_Add_Info(24, "Rate", LittleEndian2int32u(Buffer+Buffer_Offset+24));
        Details_Add_Info(28, "Start", LittleEndian2int32u(Buffer+Buffer_Offset+28));
        Details_Add_Info(32, "Length", LittleEndian2int32u(Buffer+Buffer_Offset+32));
        Details_Add_Info(36, "SuggestedBufferSize", LittleEndian2int32u(Buffer+Buffer_Offset+36));
        Details_Add_Info(40, "Quality", LittleEndian2int32s(Buffer+Buffer_Offset+40));
        Details_Add_Info(44, "SampleSize", LittleEndian2int32u(Buffer+Buffer_Offset+44));
        Details_Add_Info(48, "Frame_Left", LittleEndian2int16u(Buffer+Buffer_Offset+48));
        Details_Add_Info(50, "Frame_Top", LittleEndian2int16u(Buffer+Buffer_Offset+50));
        Details_Add_Info(52, "Frame_Rigth", LittleEndian2int16u(Buffer+Buffer_Offset+52));
        Details_Add_Info(54, "Frame_Bottom", LittleEndian2int16u(Buffer+Buffer_Offset+54));
    }
}

//---------------------------------------------------------------------------
// Chunk "strn", user defined size
// StreamName                       X bytes, Pos=0
//
void File_Riff::AVI__hdlr_strl_strn()
{
    NAME("Stream name")
    Fill("Title", Buffer+Buffer_Offset+0, Element_Size); //StreamName
}

//---------------------------------------------------------------------------
// Chunk "vprp", at least 36 bytes
// VideoFormatToken                 4 bytes, Pos=0
// VideoStandard                    4 bytes, Pos=4
// VerticalRefreshRate              4 bytes, Pos=8
// HTotalInT                        4 bytes, Pos=12
// VTotalInLines                    4 bytes, Pos=16
// FrameAspectRatio                 4 bytes, Pos=20
// FrameWidthInPixels               4 bytes, Pos=24
// FrameHeightInLines               4 bytes, Pos=28
// FieldPerFrame                    4 bytes, Pos=32
// FieldInfo[1]                     32 bytes, Pos=36 (Optional)
// ...
// FieldInfo[FieldPerFrame]         32 bytes, Pos=36+32x(FieldPerFrame-1) (Optional)
//
// FieldInfo, 32 bytes:
// CompressedBMHeight               4 bytes, Pos=0
// CompressedBMWidth                4 bytes, Pos=4
// ValidBMHeight                    4 bytes, Pos=8
// ValidBMWidth                     4 bytes, Pos=12
// ValidBMXOffset                   4 bytes, Pos=16
// ValidBMYOffset                   4 bytes, Pos=20
// VideoXOffsetInT                  4 bytes, Pos=24
// VideoYValidStartLine             4 bytes, Pos=28
//
void File_Riff::AVI__hdlr_strl_vprp()
{
    NAME("Video properties")
}

//---------------------------------------------------------------------------
void File_Riff::AVI__hdlr_xxxx()
{
    AVI__INFO_xxxx();
}

//---------------------------------------------------------------------------
void File_Riff::AVI__idx1()
{
    NAME("Index (old)")

    DETAILLEVEL_SET(0.9)
    int8u StreamID1, StreamID2; int32u Size;
    BEGIN
    size_t Pos=0;
    while (Pos<Element_Size)
    {
        GET_I1 (StreamID1,                                       Stream ID)
        GET_I1 (StreamID2,                                       Stream ID)
        SKIP_CH(2,                                              Stream Kind)
        SKIP_I4(                                                Flags)
        //KeyFrame(+)
        //Midpart
        //Firstpart
        //Lastpart
        //NoTime
        SKIP_I4(                                                Chunk offset)
        GET_I4 (Size,                                           Chunk length)

        size_t StreamID=(StreamID1-'0')*10+(StreamID2-'0')*1;
        if (StreamID<Codec_External_StreamSize.size() && Size>8)
            Codec_External_StreamSize[StreamID]+=Size-8;
        Pos+=0x10;
    }
}

//---------------------------------------------------------------------------
void File_Riff::AVI__INFO()
{
    NAME("Tags")
}

//---------------------------------------------------------------------------
void File_Riff::AVI__INFO_JUNK()
{
    NAME("Garbage")
}

//---------------------------------------------------------------------------
// List of information atoms
// Name                             X bytes, Pos=0
//
void File_Riff::AVI__INFO_xxxx()
{
    //Coherancy test
    if (Count_Get(Stream_General)==0)
        return;

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(0, "Value", Buffer+Buffer_Offset+0, Element_Size);
    }

    switch (Element_Name[Element_Level])
    {
        case Riff::AVI__INFO_IARL : Fill(Stream_General, 0, "Archival_Location", (const char*)(Buffer+Buffer_Offset), Element_Size, false); break;
        case Riff::AVI__INFO_IART : Fill(Stream_General, 0, "Artist", (const char*)(Buffer+Buffer_Offset), Element_Size, false); break;
        case Riff::AVI__INFO_ICMS : Fill(Stream_General, 0, "CommissionedBy", (const char*)(Buffer+Buffer_Offset), Element_Size, false); break;
        case Riff::AVI__INFO_ICMT : Fill(Stream_General, 0, "Comment", (const char*)(Buffer+Buffer_Offset), Element_Size, false); break;
        case Riff::AVI__INFO_ICOP : Fill(Stream_General, 0, "Copyright", (const char*)(Buffer+Buffer_Offset), Element_Size, false); break;
        case Riff::AVI__INFO_ICRD : Fill(Stream_General, 0, "Written_Date", Ztring().Date_From_String((const char*)(Buffer+Buffer_Offset), Element_Size)); break;
        case Riff::AVI__INFO_ICRP : Fill(Stream_General, 0, "Cropped", (const char*)(Buffer+Buffer_Offset), Element_Size, false); break;
        case Riff::AVI__INFO_IDIM : Fill(Stream_General, 0, "Dimensions", (const char*)(Buffer+Buffer_Offset), Element_Size, false); break;
        case Riff::AVI__INFO_IDIT : Fill(Stream_General, 0, "Mastered_Date", Ztring().Date_From_String((const char*)(Buffer+Buffer_Offset), Element_Size)); break;
        case Riff::AVI__INFO_IDPI : Fill(Stream_General, 0, "DotsPerInch", (const char*)(Buffer+Buffer_Offset), Element_Size, false); break;
        case Riff::AVI__INFO_IENG : Fill(Stream_General, 0, "Engineer", (const char*)(Buffer+Buffer_Offset), Element_Size, false); break;
        case Riff::AVI__INFO_IGNR : Fill(Stream_General, 0, "Genre", (const char*)(Buffer+Buffer_Offset), Element_Size, false); break;
        case Riff::AVI__INFO_IKEY : Fill(Stream_General, 0, "Keywords", (const char*)(Buffer+Buffer_Offset), Element_Size, false); break;
        case Riff::AVI__INFO_ILGT : Fill(Stream_General, 0, "Ligthness", (const char*)(Buffer+Buffer_Offset), Element_Size, false); break;
        case Riff::AVI__INFO_ILNG : Fill(Stream_Audio  , 0, "Language", (const char*)(Buffer+Buffer_Offset), Element_Size, false); break;
        case Riff::AVI__INFO_IMED : Fill(Stream_General, 0, "Medium", (const char*)(Buffer+Buffer_Offset), Element_Size, false); break;
        case Riff::AVI__INFO_INAM : Fill(Stream_General, 0, "Title", (const char*)(Buffer+Buffer_Offset), Element_Size, false); break;
        case Riff::AVI__INFO_IPLT : Fill(Stream_General, 0, "NumColors", (const char*)(Buffer+Buffer_Offset), Element_Size, false); break;
        case Riff::AVI__INFO_IPRD : Fill(Stream_General, 0, "Product", (const char*)(Buffer+Buffer_Offset), Element_Size, false); break;
        case Riff::AVI__INFO_IRTD : Fill(Stream_General, 0, "LawRating", (const char*)(Buffer+Buffer_Offset), Element_Size, false); break;
        case Riff::AVI__INFO_ISBJ : Fill(Stream_General, 0, "Subject", (const char*)(Buffer+Buffer_Offset), Element_Size, false); break;
        case Riff::AVI__INFO_ISFT : Fill(Stream_General, 0, "Encoded_Application", (const char*)(Buffer+Buffer_Offset), Element_Size, false); break;
        case Riff::AVI__INFO_ISHP : Fill(Stream_General, 0, "Sharpness", (const char*)(Buffer+Buffer_Offset), Element_Size, false); break;
        case Riff::AVI__INFO_ISRC : Fill(Stream_General, 0, "Encoded_Original/DistributedBy", (const char*)(Buffer+Buffer_Offset), Element_Size, false); break;
        case Riff::AVI__INFO_ISRF : Fill(Stream_General, 0, "Encoded_Original", (const char*)(Buffer+Buffer_Offset), Element_Size, false); break;
        case Riff::AVI__INFO_ITCH : Fill(Stream_General, 0, "EncodedBy", (const char*)(Buffer+Buffer_Offset), Element_Size, false); break;
        default: Fill(Stream_General, 0, Ztring().From_Local((char*)Buffer+Buffer_Offset-8, 4).To_Local().c_str(), (const char*)(Buffer+Buffer_Offset), Element_Size, false);
    }
}

//---------------------------------------------------------------------------
// Chunk "JUNK", library defined size for padding, often used to store library name
// Junk                             X bytes, Pos=0
//
void File_Riff::AVI__JUNK()
{
    NAME("Garbage")
    INTEGRITY(12)

    //Coherancy test
    if (Count_Get(Stream_General)==0)
        return;

    //Detect DivX files
         if (CC5(Buffer+Buffer_Offset)==CC5("DivX "))
    {
        Fill(Stream_General, 0, "Format", "");
        Fill(Stream_General, 0, "Format", "DivX");
    }
    //MPlayer
    else if (CC8(Buffer+Buffer_Offset)==CC8("[= MPlay") && Get(Stream_General, 0, _T("Encoded_Library")).empty())
        Fill(Stream_General, 0, "Encoded_Library", "MPlayer");
    //FFMpeg broken files detection
    else if (CC8(Buffer+Buffer_Offset)==CC8("odmldmlh"))
        TotalFrame=0; //this is not normal to have this string in a JUNK block!!! and in files tested, in this case TotalFrame is broken too
    //VirtualDubMod
    else if (CC8(Buffer+Buffer_Offset)==CC8("INFOISFT"))
    {
        int32u Size=LittleEndian2int32u(Buffer+Buffer_Offset+8);
        if (Size>Element_Size-12)
            Size=Element_Size-12;
        Fill(Stream_General, 0, "Encoded_Library", (const char*)(Buffer+Buffer_Offset+12), Size);
    }
    else if (CC8(Buffer+Buffer_Offset)==CC8("INFOIENG"))
    {
        int32u Size=LittleEndian2int32u(Buffer+Buffer_Offset+8);
        if (Size>Element_Size-12)
            Size=Element_Size-12;
        Fill(Stream_General, 0, "Encoded_Library", (const char*)(Buffer+Buffer_Offset+12), Size);
    }
    //Other libraries?
    else if (CC1(Buffer+Buffer_Offset)>=CC1("A") && CC1(Buffer+Buffer_Offset)<=CC1("z") && Get(Stream_General, 0, _T("Encoded_Library")).empty())
        Fill(Stream_General, 0, "Encoded_Library", (const char*)(Buffer+Buffer_Offset), Element_Size);
}

//---------------------------------------------------------------------------
void File_Riff::AVI__movi()
{
    NAME("Datas")

    //Filling
    AVI__movi_Size+=Element_Size;

    //We must parse moov?
    bool Skip=true;
    for (size_t Pos=0; Pos<Codec_External_PacketCount.size(); Pos++)
    {
        if (Codec_External[Pos]!=NULL && Codec_External_PacketCount[Pos]<Error)
            Skip=false;
    }
    if (Skip)
    {
        //Jumping
        File_GoTo=Element_Next[Element_Level-1];
    }
}

//---------------------------------------------------------------------------
void File_Riff::AVI__movi_rec_()
{
    NAME("Syncronisation")

    Rec_Present=true;
}

//---------------------------------------------------------------------------
void File_Riff::AVI__movi_rec__xxxx()
{
    AVI__movi_xxxx();
}

//---------------------------------------------------------------------------
void File_Riff::AVI__movi_xxxx()
{
    //ID
    char ID1=(char)((Element_Name[Element_Level]&0xFF000000)>>24);
    char ID2=(char)((Element_Name[Element_Level]&0x00FF0000)>>16);
    if (ID1<'0' || ID1>'9' || ID2<'0' || ID2>'9')
        return;
    Element_ID=(ID1-'0')*10+(ID2-'0');
    if ((size_t)Element_ID>=Codec_External.size())
        return;

    //Details
    if (Config.Details_Get())
    {
        switch (Element_Name[Element_Level]&0x0000FFFF) //2 last bytes
        {
            case Riff::AVI__movi_xxxx___db : NAME("Video"); break;
            case Riff::AVI__movi_xxxx___dc : NAME("Video"); break;
            case Riff::AVI__movi_xxxx___tx : NAME("Text"); break;
            case Riff::AVI__movi_xxxx___wb : NAME("Audio"); break;
            default :                        NAME("Data"); break;
        }
    }

    //Finished?
    if (Codec_External[Element_ID]!=NULL && Codec_External_PacketCount[Element_ID]!=Error)
    {
        Codec_External_PacketCount[Element_ID]++;

        //Details
        if (Config.Details_Get())
        {
            Details_Add_Element(Ztring(_T("Packet number ")+Ztring::ToZtring(Codec_External_PacketCount[Element_ID])));
        }

        //Parsing
        Open_Buffer_Init(Codec_External[Element_ID], File_Size, File_Offset+Buffer_Offset);
        Open_Buffer_Continue(Codec_External[Element_ID], Buffer+Buffer_Offset, Element_Size);

        //Some specific stuff
        switch (Element_Name[Element_Level]&0x0000FFFF) //2 last bytes
        {
            case Riff::AVI__movi_xxxx___db : AVI__movi_xxxx___db(); break;
            case Riff::AVI__movi_xxxx___dc : AVI__movi_xxxx___dc(); break;
            case Riff::AVI__movi_xxxx___tx : AVI__movi_xxxx___tx(); break;
            case Riff::AVI__movi_xxxx___wb : AVI__movi_xxxx___wb(); break;
            default :                        AVI__movi_xxxx___xx(); break;
        }
    }
    else
    {
        //Details
        if (Config.Details_Get())
        {
            Details_Add_Element("Skipped");
        }
    }

    //We must always parse moov?
    bool Skip=true;
    for (size_t Pos=0; Pos<Codec_External_PacketCount.size(); Pos++)
    {
        if (Codec_External[Pos]!=NULL && Codec_External_PacketCount[Pos]<Error)
            Skip=false;
    }
    if (Skip)
    {
        //Details
        if (Config.Details_Get())
        {
            Details_Add_Error("----------------------------------------");
            Details_Add_Error("---   Jumping to end of movi chunk   ---");
            Details_Add_Error("----------------------------------------");
        }

        //Jumping
        if (Rec_Present)
            File_GoTo=Element_Next[Element_Level-2];
        else
            File_GoTo=Element_Next[Element_Level-1];
    }
}

//---------------------------------------------------------------------------
void File_Riff::AVI__movi_xxxx___db()
{
    AVI__movi_xxxx___dc();
}

//---------------------------------------------------------------------------
void File_Riff::AVI__movi_xxxx___dc()
{
    //Finalize (if requested)
    if (Codec_External[Element_ID]->File_Offset==File_Size || Codec_External_PacketCount[Element_ID]>=300)
        Codec_External_PacketCount[Element_ID]=Error;

    //Specific
    #if defined(MEDIAINFO_MPEG4V_YES)
        if (Config.Codec_Get(Codec_External_Codec[Element_ID], InfoCodec_KindofCodec).find(_T("MPEG-4"))==0 && ((File_Mpeg4v*)Codec_External[Element_ID])->RIFF_VOP_Count>1)
            Codec_External_PacketCount[Element_ID]=Error;
    #endif
}

//---------------------------------------------------------------------------
// Text
// "GAB2"               4 bytes
// 0x00                 1 byte
// 0x0002               2 bytes, this is Unicode
// Name_Size (bytes)    4 bytes
// Name (UTF-16)        Name_Size bytes
// 0x04                 2 bytes
// File_Size            4 bytes
// File                 File_Size bytes, entire SRT/SS file
//
void File_Riff::AVI__movi_xxxx___tx()
{
    //Skip it
    Codec_External_PacketCount[Element_ID]=Error;
}

//---------------------------------------------------------------------------
void File_Riff::AVI__movi_xxxx___wb()
{
    //Finalize (if requested)
    if (Codec_External[Element_ID]->File_Offset==File_Size || Codec_External_PacketCount[Element_ID]>=1000)
        Codec_External_PacketCount[Element_ID]=Error;
}

//---------------------------------------------------------------------------
void File_Riff::AVI__movi_xxxx___xx()
{
    //Skip it
    Codec_External_PacketCount[Element_ID]=true;
}

//---------------------------------------------------------------------------
void File_Riff::AVIX()
{
}

//---------------------------------------------------------------------------
void File_Riff::AVIX_idx1()
{
    AVI__idx1();
}

//---------------------------------------------------------------------------
void File_Riff::AVIX_movi()
{
    AVI__movi();
}

//---------------------------------------------------------------------------
void File_Riff::AVIX_movi_rec_()
{
    AVI__movi_rec_();
}

//---------------------------------------------------------------------------
void File_Riff::AVIX_movi_rec__xxxx()
{
    AVIX_movi_xxxx();
}

//---------------------------------------------------------------------------
void File_Riff::AVIX_movi_xxxx()
{
    AVI__movi_xxxx();
}

//---------------------------------------------------------------------------
void File_Riff::menu()
{
    NAME("DivX Menu")

    Stream_Prepare(Stream_Menu);
    Fill("Codec", "DivX");
}

//---------------------------------------------------------------------------
void File_Riff::PAL_()
{
    NAME("Format: RIFF Palette")
    Stream_Prepare(Stream_General);
    Fill("Format", "RIFF Palette");
}

//---------------------------------------------------------------------------
void File_Riff::RDIB()
{
    NAME("Format: RIFF DIB")
    Stream_Prepare(Stream_General);
    Fill("Format", "RIFF DIB");
}

//---------------------------------------------------------------------------
void File_Riff::RMID()
{
    NAME("Format: RIFF MIDI")
    Stream_Prepare(Stream_General);
    Fill("Format", "RIFF MIDI");
}

//---------------------------------------------------------------------------
void File_Riff::RMMP()
{
    NAME("Format: RIFF MMP")
    Stream_Prepare(Stream_General);
    Fill("Format", "RIFF MMP");
}

//---------------------------------------------------------------------------
void File_Riff::WAVE()
{
    NAME("Format: Wave")
    Stream_Prepare(Stream_General);
    Fill("Format", "Wave");
}

//---------------------------------------------------------------------------
void File_Riff::WAVE_data()
{
    NAME("Raw datas")

    //Parsing
    Element_Name[Element_Level]=CC4("00wb");
    Element_Size=Buffer_Size-Buffer_Offset;
    AVI__movi_xxxx();
}

//---------------------------------------------------------------------------
// Chunk "fact", at least bytes
// SamplesCount                     2 bytes, Pos=0
//
void File_Riff::WAVE_fact()
{
}

//---------------------------------------------------------------------------
void File_Riff::WAVE_fmt_()
{
    AVI__hdlr_strl_strh_Rate=0;

    //Add the codec to the codec list
    Codec_External.push_back(NULL);
    Codec_External_PacketCount.push_back(0);
    Codec_External_StreamSize.push_back(0);
    Element_ID=Codec_External_Codec.size();

    AVI__hdlr_strl_strf_auds();

    //Add the codec to the codec list
    Codec_External_Codec.push_back(Get(StreamKind_Last, StreamPos_Last, _T("Codec")));//After because we don't have the codec name
    Fill("ID", Element_ID);
}

//***************************************************************************
// Helpers
//***************************************************************************

//---------------------------------------------------------------------------
bool File_Riff::Element_Name_Get()
{
    //Enough data?
    if (Buffer_Offset+8>Buffer_Size)
        return false;

    //Element name
    Element_Name[Element_Level]=CC4(Buffer+Buffer_Offset);
    Element_HeaderSize=8;

    //Special cases
    if (Element_Name[Element_Level]==Riff::LIST || Element_Name[Element_Level]==Riff::RIFF)
    {
        //Enough data?
        if (Buffer_Offset+12>Buffer_Size)
            return false;

        //Reading real name
        Element_Name[Element_Level]=CC4(Buffer+Buffer_Offset+8);
        Element_HeaderSize=12;
    }

    //Integrity
    if (Element_Name[Element_Level]==0x00000000)
        TRUSTED_ISNOT("Empty name");

    return true;
}

//---------------------------------------------------------------------------
bool File_Riff::Element_Size_Get()
{
    //Enough data?
    if (Buffer_Offset+8>Buffer_Size)
        return false;

    //Element size
    Element_Size=LittleEndian2int32u(Buffer+Buffer_Offset+4);

    //Special cases
    if (Element_HeaderSize==12)
    {
        if (Element_Size>=4)
            Element_Size-=4;
        else
        {
            Element_Size=(int64u)-1; //Size is broken
            Element_Next[Element_Level]=(int64u)-1;
            return true;
        }
    }

    //Element Next
    Element_Next[Element_Level]=File_Offset+Buffer_Offset+Element_HeaderSize+Element_Size;

    //Integrity of element
    if (Element_Next[Element_Level]>Element_Next[Element_Level-1])
    {
        TRUSTED_ISNOT("Atom is too big, resizing...");
        Element_Next[Element_Level]=Element_Next[Element_Level-1];
        if (Element_Next[Element_Level]>(File_Offset+Buffer_Offset)+Element_HeaderSize)
            Element_Size=Element_Next[Element_Level]-(File_Offset+Buffer_Offset)-Element_HeaderSize;
        else
        {
            TRUSTED_ISNOT("Not enough place to have an atom");
            Element_Size=0; //Not enough place to have an atom
        }
    }

    //2 byte-aligned
    if (Element_Size%2)
        Element_Next[Element_Level]+=1;
    return true;
}

//---------------------------------------------------------------------------
// Set StreamXXXX_Last with the stream corresponding to the ID
void File_Riff::SetLastByID(int8u ID)
{
    for (StreamKind_Last=(stream_t)(Stream_General+1); StreamKind_Last<Stream_Max; StreamKind_Last=(stream_t)(StreamKind_Last+1))
        for (StreamPos_Last=0; StreamPos_Last<Stream[StreamKind_Last]->size(); StreamPos_Last++)
            if (Get(StreamKind_Last, StreamPos_Last, _T("ID")).To_int32s()==ID)
                return; //Found

    //Not found
    StreamKind_Last=Stream_General;
    StreamPos_Last=0;
}

//***************************************************************************
// Information
//***************************************************************************

//---------------------------------------------------------------------------
void File_Riff::HowTo(stream_t StreamKind)
{
    switch (StreamKind)
    {
        case (Stream_General) :
            Fill_HowTo("Format", "R");
            Fill_HowTo("BitRate", "R");
            Fill_HowTo("PlayTime", "R");
            Fill_HowTo("Movie", "R|INAM");
            Fill_HowTo("Track", "N|INAM");
            Fill_HowTo("Track/Position", "N|IPRT");
            Fill_HowTo("Album/Track_Total", "N|IFRM");
            Fill_HowTo("Composer", "R|IMUS");
            Fill_HowTo("WrittenBy", "R|IWRI");
            Fill_HowTo("Director", "R|IART");
            Fill_HowTo("DirectorOfPhotography", "R|ICNM");
            Fill_HowTo("EditedBy", "R|IEDT");
            Fill_HowTo("Producer", "R|IPRO");
            Fill_HowTo("ProductionDesigner", "R|IPDS");
            Fill_HowTo("CostumeDesigner", "R|ICDS");
            Fill_HowTo("MasteredBy", "R|IENG");
            Fill_HowTo("ProductionStudio", "R|ISTD");
            Fill_HowTo("DistributedBy", "R|IDST");
            Fill_HowTo("EncodedBy", "R|ITCH");
            Fill_HowTo("CommissionedBy", "R|ICMS");
            Fill_HowTo("Encoded_Original/DistributedBy", "R|ISRC");
            Fill_HowTo("Subject", "R|ISBJ");
            Fill_HowTo("Keywords", "R|IKEY");
            Fill_HowTo("LawRating", "R|IRTD");
            Fill_HowTo("Language", "R|ILNG");
            Fill_HowTo("Medium", "R|IMED");
            Fill_HowTo("Product", "R|IPRD");
            Fill_HowTo("Country", "R|ICNT");
            Fill_HowTo("Written_Date", "R|ICRD");
            Fill_HowTo("Mastered_Date", "R|IDIT");
            Fill_HowTo("Archival_Location", "R|IARL");
            Fill_HowTo("Genre", "R|IGNR");
            Fill_HowTo("Comment", "R|ICMT");
            Fill_HowTo("Encoded_Application", "R|ISFT");
            Fill_HowTo("Encoded_Original", "R|ISRF");
            Fill_HowTo("Copyright", "R|ICOP");
            break;
        case (Stream_Video) :
            Fill_HowTo("Codec", "R");
            Fill_HowTo("FrameRate", "R");
            Fill_HowTo("FrameCount", "R");
            Fill_HowTo("Width", "R");
            Fill_HowTo("Height", "R");
            Fill_HowTo("AspectRatio", "R");
            Fill_HowTo("BitRate", "R");
            break;
        case (Stream_Audio) :
            Fill_HowTo("BitRate", "R");
            Fill_HowTo("Channel(s)", "R");
            Fill_HowTo("SamplingRate", "R");
            Fill_HowTo("Codec", "R");
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

#endif //MEDIAINFO_RIFF_YES








