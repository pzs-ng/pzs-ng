// File_Mpega - Info for MPEG Audio files
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
#if defined(MEDIAINFO_MPEGA_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#if defined(MEDIAINFO_ID3_YES)
    #include "MediaInfo/Audio/File_Id3.h"
#endif
#include "MediaInfo/Audio/File_Mpega.h"
#include "ZenLib/BitStream.h"
#include <ZenLib/Utils.h>
using namespace ZenLib;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
inline static size_t Mpega_SynchSafeInt (int8u* B) {return ((B[0]<<21)
                                                          | (B[1]<<14)
                                                          | (B[2]<< 7)
                                                          | (B[3]<< 0));}
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//---------------------------------------------------------------------------
const Char* Mpega_Version[4]=
{
    _T("MPEG-2.5A"),
    _T(""),
    _T("MPEG-2A"),
    _T("MPEG-1A")
};
const Char* Mpega_Version_String[4]=
{
    _T("MPEG-2.5 Audio"),
    _T(""),
    _T("MPEG-2 Audio"),
    _T("MPEG-1 Audio")
};
const Char* Mpega_Layer[4]=
{
    _T(""),
    _T(" L3"),
    _T(" L2"),
    _T(" L1")
};
const Char* Mpega_Layer_String[4]=
{
    _T(""),
    _T(" layer 3"),
    _T(" layer 2"),
    _T(" layer 1")
};
const size_t Mpega_BitRate[4][4][16]=
{
    {{0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},  //MPEG Audio 2.5 layer X
     {0,   8,  16,  24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160,   0},  //MPEG Audio 2.5 layer 3
     {0,   8,  16,  24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160,   0},  //MPEG Audio 2.5 layer 2
     {0,  32,  48,  56,  64,  80,  96, 112, 128, 144, 160, 176, 192, 224, 256,   0}}, //MPEG Audio 2.5 layer 1
    {{0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},  //MPEG Audio X layer X
     {0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},  //MPEG Audio X layer 3
     {0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},  //MPEG Audio X layer 2
     {0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0}}, //MPEG Audio X layer 1
    {{0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},  //MPEG Audio 2 layer X
     {0,   8,  16,  24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160,   0},  //MPEG Audio 2 layer 3
     {0,   8,  16,  24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160,   0},  //MPEG Audio 2 layer 2
     {0,  32,  48,  56,  64,  80,  96, 112, 128, 144, 160, 176, 192, 224, 256,   0}}, //MPEG Audio 2 layer 1
    {{0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},  //MPEG Audio 1 layer X
     {0,  32,  40,  48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320,   0},  //MPEG Audio 1 layer 3
     {0,  32,  48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 384,   0},  //MPEG Audio 1 layer 2
     {0,  32,  64,  96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448,   0}}  //MPEG Audio 1 layer 1
};
const size_t Mpega_SamplingRate[4][4]=
{
    {11025, 12000,  8000, 0}, //MPEG Audio 2.5
    {    0,     0,     0, 0}, //MPEG Audio X
    {22050, 24000, 16000, 0}, //MPEG Audio 2
    {44100, 48000, 32000, 0}  //MPEG Audio 1
};
const size_t Mpega_Channels[4]=
{
    2,
    2,
    2,
    1
};
const Char* Mpega_Codec_Profile[4]=
{
    _T(""),
    _T("Joint stereo"),
    _T("Dual mono"),
    _T("")
};
//---------------------------------------------------------------------------

//***************************************************************************
// Functions
//***************************************************************************

size_t Mpega_Frame_Size(int32u Header);

#define NAME(ELEMENT_NAME) \
    if (Config.Details_Get()) \
    { \
        Details_Add_Element(ELEMENT_NAME); \
    } \

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
File_Mpega::File_Mpega()
{
    //Config
    Frame_Count_Valid=16;
    Frame_Count_Limit=1000;
}

//***************************************************************************
// Format
//***************************************************************************

//---------------------------------------------------------------------------
void File_Mpega::Read_Buffer_Init()
{
    Frame_Count=0;
    Frame_Size_First=0;
    BitRate_Mode.clear();
    Frame_Size=0;
    VBR_Frames=0;
    VBR_FileSize=File_Size;
    Delay=0;
    ID3=NULL;
    BitRate_Mode_More.clear();
    BitRate_Mode_More.resize(16); //Bitrate index size
    Id3v2_Size=0;
}

//---------------------------------------------------------------------------
void File_Mpega::Read_Buffer_Continue()
{
    //Integrity
    if (Buffer_Size<10)
        return;

    //ID3v2 present?
    if (CC3(Buffer)==CC3("ID3"))
    {
        //ID3v2, must skip it here
        Id3v2_Size=Mpega_SynchSafeInt(Buffer+6)+10;

        //Details
        if (Config.Details_Get())
        {
            Details_Add_Element(0, "Id3v2");
            FLUSH();
        }

        #if defined(MEDIAINFO_ID3_YES)
            ID3=new File_Id3;
            Open_Buffer_Init(ID3, Id3v2_Size);
        #endif //ID3

        VBR_FileSize=File_Size-Id3v2_Size;
    }

    //If we continue the tag
    #if defined(MEDIAINFO_ID3_YES)
    if (Id3v2_Size>0 && File_Offset<Id3v2_Size)
    {
        if (Id3v2_Size-File_Offset<=Buffer_Size)
        {
            Open_Buffer_Continue(ID3, Buffer, Id3v2_Size-File_Offset);
            Buffer_Offset+=Id3v2_Size-File_Offset;
            Open_Buffer_Finalize(ID3);
        }
        else
        {
            Open_Buffer_Continue(ID3, Buffer, Buffer_Size);
            Buffer_Offset=Buffer_Size;
        }
    }
    #else
        Buffer_Offset+=Id3v2_Size;
    #endif //ID3

    if (File_Offset==File_Size-128 && Buffer_Size==128)
    {
        if (CC3(Buffer+Buffer_Size-128)==CC3("TAG"))
            Id3v1();
    }

    while (Buffer_Parse());
}

//---------------------------------------------------------------------------
void File_Mpega::Read_Buffer_Finalize()
{
    if (Count_Get(Stream_Audio)==0)
        return;

    //VBR detection without header
    if (VBR_Frames==0)
    {
        //How much kinds of bitrates?
        size_t BitRate_Mode_More_Count=0;
        for (size_t Pos=0; Pos<BitRate_Mode_More.size(); Pos++)
            if (BitRate_Mode_More[Pos]==0)
                BitRate_Mode_More_Count++;
        if (BitRate_Mode_More_Count<BitRate_Mode_More.size()-1) //To have only one kind of bitrare
            BitRate_Mode=_T("VBR");
    }

    //VBR header
    if (VBR_Frames==0 && BitRate_Mode!=_T("VBR"))
    {
        BitRate=Mpega_BitRate[Mpeg][Layer][BitRate]*1000;
        BitRate_Mode=_T("CBR");
    }
    else if (VBR_Frames>0)
    {
        float32 FrameLength=(VBR_FileSize?VBR_FileSize:File_Size)/VBR_Frames;
        size_t Divider;
        if (Mpeg==3 && Layer==3) //MPEG 1 Layer 1
             Divider=384/8;
        else if ((Mpeg==2 || Mpeg==0) && Layer==1) //MPEG 2 or 2.5 Layer 3
            Divider=576/8;
        else
            Divider=1152/8;
        BitRate=(size_t)(FrameLength*Mpega_SamplingRate[Mpeg][SamplingRate]/Divider);
        BitRate_Mode=_T("VBR");
    }
    else
        BitRate=0;

    //Filling
    if (BitRate>0 && !CompleteFileName.empty())
        Fill(Stream_General, 0, "PlayTime", VBR_FileSize*8*1000/BitRate);
    Fill(Stream_General, 0, "Encoded_Library", Encoded_Library);
    if (BitRate>0)
        Fill(Stream_Audio, 0, "BitRate", BitRate);
    Fill(Stream_Audio, 0, "BitRate_Mode", BitRate_Mode);
    Fill(Stream_Audio, 0, "Encoded_Library", Encoded_Library);
    if (Delay>100 && BitRate!=0 && CompleteFileName.empty())
        Fill("Delay", Delay*1000*8/BitRate);

    //ID3
    #if defined(MEDIAINFO_ID3_YES)
    if (ID3!=NULL)
    {
        if (ID3->Count_Get(Stream_General)>0)
            Merge(*ID3, Stream_General, 0, 0);
    }
    #endif //ID3
    if (Get(Stream_General, 0, _T("Title")).empty() && !Id3v1_Title.empty()) Fill(Stream_General, 0, "Title", Id3v1_Title);
    if (Get(Stream_General, 0, _T("Album")).empty() && !Id3v1_Album.empty()) Fill(Stream_General, 0, "Album", Id3v1_Album);
    if (Get(Stream_General, 0, _T("Performer")).empty() && !Id3v1_Artist.empty()) Fill(Stream_General, 0, "Performer", Id3v1_Artist);
    if (Get(Stream_General, 0, _T("Recorded_Date")).empty() && !Id3v1_Year.empty()) Fill(Stream_General, 0, "Recorded_Date", Id3v1_Year);
    if (Get(Stream_General, 0, _T("Comment")).empty() && !Id3v1_Comment.empty()) Fill(Stream_General, 0, "Comment", Id3v1_Comment);
    if (Get(Stream_General, 0, _T("Track/Position")).empty() && !Id3v1_Track.empty()) Fill(Stream_General, 0, "Track/Position", Id3v1_Track);
    if (Get(Stream_General, 0, _T("Genre")).empty() && !Id3v1_Genre.empty()) Fill(Stream_General, 0, "Genre", Id3v1_Genre);
}

//---------------------------------------------------------------------------
bool File_Mpega::Buffer_Parse()
{
    //Search for header
    size_t Junk=NextFrame();
    if (Junk==Error)
        return false;
    if (Frame_Count==0)
        Delay+=Buffer_Offset;

    //Details
    if (Junk>0)
    {
        if (Config.Details_Get())
        {
            Details_Add_Info(0, "Junk (bytes)", Junk);
        }
    }

    if (!CompleteFileName.empty() && Junk>1000)
    {
        //This is not a MP3 file!
        File_Offset=File_Size;
        return false;
    }

    //Parsing
    if (!Element_Parse())
        return false; //Not enough bytes

    //To the next frame
    Buffer_Offset+=Frame_Size;
    return true;
}

//---------------------------------------------------------------------------
bool File_Mpega::Element_Parse()
{
    //Enough buffer?
    if (Buffer_Offset+5>Buffer_Size)
        return false;

    // AAAAAAAA AAABBCCD EEEEFFGH IIJJKLMM {ZZZZ}
    //
    // Label (Position) Description
    // A (31-21) Frame sync
    //           All bits set (1)
    // B (20,19) MPEG Audio version ID
    //           00 - MPEG Ver 2.5, 01 - reserved, 10 - Ver 2, 11 - Ver 1
    //           Note: MPEG Ver 2.5 is not official; bit # 20 indicates 2.5
    // C (18,17) Layer description
    //           00 - reserved, 01 - Layer III, 10 - Layer II, 11 - Layer I
    // D    (16) Protection bit
    //           0 - None, 1 - Protected by CRC (16bit crc follows header)
    // E (15,12) Bitrate index
    // F (11,10) Sampling rate
    // G     (9) Padding bit
    //           0 - frame not padded, 1 - frame padded with one extra slot
    // H     (8) Private bit
    //           0 - not private, 1 - private
    //           Note: May be freely used for other needs of an application.
    // I   (7,6) Channel Mode
    //           00 - Stereo, 01 - Joint stereo, 10 - Dual (Stereo), 11 - Mono
    // J   (5,4) Mode extension (Only if Joint stereo)
    //           Used to join data; bits dynamically generated by an encoder.
    // K     (3) Copyright
    //           0 - Audio is not copyrighted, 1 - Audio is marked copyrighted
    // L     (2) Original
    //           0 - Copy of original media, 1 - Original media
    // M   (1,0) Emphasis
    //           00 - none, 01 - 50/15 ms, 10 - reserved, 11 - CCIT J.17
    // Z (32-35) CRC  !!OPTIONAL!!
    //           Note: NOT part of header, just appended on end when needed
    //

    BitStream BS(Buffer+Buffer_Offset, Buffer_Size-Buffer_Offset);
    BS.Skip(11); //Frame Sync, All bits set (1)
    Mpeg=BS.Get(2); //Version ID
    Layer=BS.Get(2); //Layer ID
    Protection=BS.Get(1); //Protection
    BitRate=BS.Get(4); //BitRate
    SamplingRate=BS.Get(2); //SamplingRate
    Padding=BS.Get(1); //Padding
    Private=BS.Get(1); //Private
    Channels=BS.Get(2); //Channel Mode

    //Enough buffer?
    if (Buffer_Offset+Frame_Size>Buffer_Size)
        return false;

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Element(0, _T("Frame"));
        Details_Add_Element(Ztring::ToZtring(Frame_Count));
    }

    //VBR headers
    if (Frame_Count<10) //No need to do it too much
    {
        Header_Xing();
        Header_VBRI();
    }

    //Parsing
    Frame();

    //Stopping Analysis if requested
    if (Frame_Count>Frame_Count_Limit && !CompleteFileName.empty())
    {
        File_GoTo=File_Size-128; //Looking for ID3 tag
        return false;
    }

    //No more need?
    if (File_GoTo>0)
        return false;

    return true;
}

//---------------------------------------------------------------------------
void File_Mpega::Frame()
{
    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(0, "Frame Sync", "11111111111");
        Details_Add_Info(11, "Version ID", Mpeg);
        Details_Add_Info(13, "Layer ID", Layer);
        Details_Add_Info(15, "Protection", Protection);
        Details_Add_Info(16, "BitRate", Mpega_BitRate[Mpeg][Layer][BitRate]*1000);
        Details_Add_Info(20, "SamplingRate", Mpega_SamplingRate[Mpeg][SamplingRate]);
        Details_Add_Info(22, "Padding", Padding);
        Details_Add_Info(23, "Private", Private);
        Details_Add_Info(24, "Channel Mode", Mpega_Codec_Profile[Channels]);
    }

    //Filling
    if (Count_Get(Stream_Audio)==0 && Frame_Count>Frame_Count_Valid-1)
    {
        //Filling
        Stream_Prepare(Stream_General);
        Fill("Format", Ztring(Mpega_Version[Mpeg]));
        Stream_Prepare(Stream_Audio);
        Fill("Codec", Ztring(Mpega_Version[Mpeg])+Mpega_Layer[Layer]);
        Fill("Codec/String", Ztring(Mpega_Version_String[Mpeg])+Mpega_Layer_String[Layer]);
        Fill("SamplingRate", Mpega_SamplingRate[Mpeg][SamplingRate]);
        Fill("Channel(s)", Mpega_Channels[Channels]);
        Fill("Codec_Profile", Mpega_Codec_Profile[Channels]);
        Fill("Resolution", 16);

        //Details
        if (Config.Details_Get())
        {
            Details_Add_Error("------------------------------------------");
            Details_Add_Error("---   MPEG-A, Jumping to end of file   ---");
            Details_Add_Error("------------------------------------------");
        }
        //No more need data
        if (CompleteFileName.empty())
            File_Offset=File_Size; //Finnished
        else
            File_GoTo=File_Size-128; //Looking for ID3 tag
    }

    //Counting
    Frame_Count++;
    BitRate_Mode_More[BitRate]++;
}

//---------------------------------------------------------------------------
void File_Mpega::Id3v1()
{
    //Details
    if (Config.Details_Get())
    {
        Details_Add_Element(0, "Id3v1");
    }

    char* BufferC=(char*)Buffer+Buffer_Size-128;

    Id3v1_Title.From_Local     (BufferC+  3, 30);
    Id3v1_Artist.From_Local    (BufferC+ 33, 30);
    Id3v1_Album.From_Local     (BufferC+ 63, 30);
    Id3v1_Year.From_Local      (BufferC+ 93, 4);
    Id3v1_Comment.From_Local   (BufferC+ 97, 30);
    if (BufferC+125=='\0' && BufferC+126!='\0')
        Id3v1_Track.From_Number((int8u)BufferC[126]);
    Id3v1_Genre.From_Number    ((int8u)BufferC[127]);
    if (Id3v1_Genre==_T("0") || Id3v1_Genre==_T("255"))
        Id3v1_Genre.clear();

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(Error, "Title", Id3v1_Title);
        Details_Add_Info(Error, "Artist", Id3v1_Artist);
        Details_Add_Info(Error, "Album", Id3v1_Album);
        Details_Add_Info(Error, "Year", Id3v1_Year);
        Details_Add_Info(Error, "Comment", Id3v1_Comment);
        Details_Add_Info(Error, "Track", Id3v1_Track);
        Details_Add_Info(Error, "Genre", Id3v1_Genre);
    }

}

//---------------------------------------------------------------------------
void File_Mpega::HowTo(stream_t StreamKind)
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
        Audio[0](_T("ChannelPositions"), Info_HowTo)=_T("R");
        Audio[0](_T("SamplingRate"), Info_HowTo)=_T("R");
    }
}

//---------------------------------------------------------------------------
size_t File_Mpega::NextFrame()
{
    size_t Level=0;
    size_t Offset_Header=Buffer_Offset;
    size_t Malformed=0;
    while (Offset_Header+4<Buffer_Size) //No frame should be so big
    {
        if (Level==3)
        {
        }
             if (Level==2)
        {
            //Reset the Buffer_Offset
            Offset_Header-=2;
            Offset_Header-=Malformed;

            //Frame_Size
              int32u Header=BigEndian2int32u(Buffer+Offset_Header);
            Frame_Size=Mpega_Frame_Size(Header);

            //Testing if we can believe in the value
            bool OK=true;
            //-Frame size
            if (Frame_Size<72 || Frame_Size==Error)
                OK=false;
            //-Testing if next Frame is here
            else if (Frame_Count==0)
            {
                //Size
                if (Offset_Header+Frame_Size>Buffer_Size)
                    OK=false;
                //Next bytes after FrameSize (or FrameSize+1) must be FFF
                else if (Offset_Header+Frame_Size+2<Buffer_Size && !(
                         (Buffer[Offset_Header+Frame_Size+0]==0xFF && (Buffer[Offset_Header+Frame_Size+1]&0xF0)==0xF0)  //Normal
                      || (Buffer[Offset_Header+Frame_Size+1]==0xFF && (Buffer[Offset_Header+Frame_Size+2]&0xF0)==0xF0)  //For buggy files
                        ))
                    OK=false;
            }
            if (!OK)
            {
                Malformed=Error;
                Level=0;
            }
            else
            {
                //Junk
                size_t Junk=Offset_Header-Buffer_Offset;
                if (Junk==Error) //Malformed
                    Junk=0;

                //Return value
                Buffer_Offset=Offset_Header;
                return Junk;
            }
        }
        else if (Level==1)
        {
            if ((Buffer[Offset_Header]&0xE0)==0xE0
             && (Buffer[Offset_Header]&0x18)!=0x08
             && (Buffer[Offset_Header]&0x06)!=0x00)
                Level=2;
            else
                Level=0;
        }
        else if (Level==0)
        {
            if (Buffer[Offset_Header]==0xFF)
            {
                Malformed=0;
                Level=1;
            }
            //MPEG Audio malformed! Sometimes the padding bit is set but the padding is not here.
            else if (Offset_Header>0 && Malformed!=Error
                  && Buffer[Offset_Header-1]==0xFF //Level 0
                  && (Buffer[Offset_Header]&0xE0)==0xE0 //Level 1
                  && (Buffer[Offset_Header]&0x18)!=0x08
                  && (Buffer[Offset_Header]&0x06)!=0x00)
            {
                Malformed=1;
                Level=2;
            }
        }
        Offset_Header++;
    }
    return Error;
}

//---------------------------------------------------------------------------
size_t File_Mpega::Header_Xing()
{
    int32u Xing_Header_Offset;
    if (Mpeg==3) //MPEG-1
        if (Channels==3) //Mono
            Xing_Header_Offset=21;
        else
            Xing_Header_Offset=36;
    else //MPEG-2 or 2.5
        if (Channels==3) //Mono
            Xing_Header_Offset=13;
        else
            Xing_Header_Offset=21;
    if (Buffer_Offset+Xing_Header_Offset+128<Buffer_Size)
    {
        const int8u* Xing_Header=Buffer+Buffer_Offset+Xing_Header_Offset;
        if (CC4(Xing_Header)==CC4("Xing") || CC4(Xing_Header)==CC4("Info"))
        {
            size_t Xing_Flags=0;
            Xing_Header+=4; // "Xing"
            Xing_Flags=BigEndian2int32s((char*)Xing_Header);
            Xing_Header+=4; //Flags
            int32u Xing_Header_Size=8
                                   +((Xing_Flags&0x1) ? 4:0)    //FrameCount
                                   +((Xing_Flags&0x2) ? 4:0)    //FileSize
                                   +((Xing_Flags&0x4) ? 100:0)  //TOC
                                   +((Xing_Flags&0x8) ? 4:0)    //Scale
                                   +((Xing_Flags&0x10)? 348:0); //Lame

            if (Xing_Header_Offset+Xing_Header_Size<Buffer_Size)
            {
                if (Xing_Flags&0x1) //FrameCount
                {
                    VBR_Frames=BigEndian2int32s((char*)Xing_Header);
                    Xing_Header+=4;
                }

                if (Xing_Flags&0x2) //FileSize
                {
                    VBR_FileSize=BigEndian2int32s((char*)Xing_Header);
                    Xing_Header+=4;
                }

                if (Xing_Flags&0x4) //TOC
                    Xing_Header+=100;

                if (Xing_Flags&0x8) //Scale
                    Xing_Header+=4;

                if (Xing_Flags&0xF) //LAME
                {
                    Encoded_Library.From_Local((char*)Xing_Header, 9);
                    if (Encoded_Library>=_T("LAME3.90") || Encoded_Library<_T("LAME9.99"))
                    {
                        if ((CC1(Xing_Header+9)&0x0F)==1 || (CC1(Xing_Header+9)&0x0F)==8) //2 possible values for CBR
                            VBR_Frames=0;
                        //TODO : Lame tag, http://gabriel.mp3-tech.org/mp3infotag.html
                    }
                    else
                        Encoded_Library.From_Local((char*)Xing_Header, 20); //Long tag version, if version<3.90
                }
            }

            if (CC4(Xing_Header)==CC4("Info"))
                VBR_Frames=0; //This is not a VBR file

            return 1;
        }
    }
    return Error;
}

//---------------------------------------------------------------------------
size_t File_Mpega::Header_VBRI()
{
    const size_t Fraunhofer_Header_Offset=36;
    if (Buffer_Offset+Fraunhofer_Header_Offset+32<Buffer_Size)
    {
        const int8u* Fraunhofer_Header=Buffer+Buffer_Offset+Fraunhofer_Header_Offset;
        if (CC4(Fraunhofer_Header)==CC4("VBRI") && CC2(Fraunhofer_Header+4)==0x0001) //VBRI v1 only
        {
            VBR_FileSize=BigEndian2int32s(Fraunhofer_Header+10);
            VBR_Frames=BigEndian2int32s(Fraunhofer_Header+14);
            return 1;
        }
    }
    return Error;
}

//---------------------------------------------------------------------------
size_t Mpega_Frame_Size(int32u I0)
{
    size_t Mpeg=(I0>>19)&0x3;
    size_t Layer=(I0>>17)&0x3;
    size_t BitRate=(I0>>12)&0xF;
    size_t SamplingRate=(I0>>10)&0x3;
    size_t Padded=(I0>>9)&0x1;
    size_t Frame_Size=0;

    if (Mpega_SamplingRate[Mpeg][SamplingRate]==0)
        return Error;

    if (Layer==3) //Layer 1
    {
        if (Mpeg==3) //MPEG-1
            Frame_Size=(12*Mpega_BitRate[Mpeg][Layer][BitRate]*1000/Mpega_SamplingRate[Mpeg][SamplingRate]+Padded)*4;
        else //MPEG-2 and 2.5
            Frame_Size=(6*Mpega_BitRate[Mpeg][Layer][BitRate]*1000/Mpega_SamplingRate[Mpeg][SamplingRate]+Padded)*4;
    }
    else //Layer 2 and 3
    {
        if (Mpeg==3) //MPEG-1
            Frame_Size=144*Mpega_BitRate[Mpeg][Layer][BitRate]*1000/Mpega_SamplingRate[Mpeg][SamplingRate]+Padded;
        else //MPEG-2 and 2.5
            Frame_Size=72*Mpega_BitRate[Mpeg][Layer][BitRate]*1000/Mpega_SamplingRate[Mpeg][SamplingRate]+Padded;
    }

    return Frame_Size;
}

} //NameSpace

#endif //MEDIAINFO_MPEGA_YES

