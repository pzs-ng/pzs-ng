// File_Rm - Info for Real Media files
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
#if defined(MEDIAINFO_RM_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <wx/file.h>
#include <ZenLib/ZtringListList.h>
#include <ZenLib/Utils.h>
#include "MediaInfo/Multiple/File_Rm.h"
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

// https://common.helixcommunity.org/nonav/2003/HCS_SDK_r5/htmfiles/rmff.htm

//***************************************************************************
// Functions
//***************************************************************************

void File_Rm::Read_Buffer_Continue()
{
    //Init
    Size=0;

    //Integrity
    if (Buffer_Size<4)
        return;
    if (CC4(Buffer)!=CC4(".RMF"))
    {
        File_Offset=File_Size;
        return;
    }

    ShouldStop=false;
    while (!ShouldStop)
    {
        int32u Code=CC4(Buffer+Buffer_Offset);
        Size=BigEndian2int32u(Buffer+Buffer_Offset+4);
        if (Size>=8 && Buffer_Offset+Size<Buffer_Size)
        {
            Buffer_Offset+=8;
            Size-=8;
            if (Size==0) ;
            else if (Code==CC4(".RMF")) Rmf();
            else if (Code==CC4("PROP")) Prop();
            else if (Code==CC4("MDPR")) Mdpr();
            else if (Code==CC4("CONT")) Cont();
            else if (Code==CC4("DATA")) ;
            else if (Code==CC4("INDX")) ;
            //Go after the last frame
            Buffer_Offset+=Size;
            if (Buffer_Offset+8>=Buffer_Size)
                ShouldStop=true;
        }
        else
            ShouldStop=true;
    }

    /*
    //End of the file, we search for metadata
    Buffer_Offset=End_Size-4;
    while (Buffer_Offset>0 && CC4(End+Buffer_Offset)!=CC4("RJMD"))
        Buffer_Offset--;
    Size=End_Size-Buffer_Offset;
    if (Buffer_Offset>0) Rjmd();
    */
}

//---------------------------------------------------------------------------
// Atom "CONT", CONTent
// ObjectVersion                    2 bytes, Pos=0
// TitleLength                      2 bytes, Pos=2
// Title                            TitleLength bytes
// ArtistLength                     2 bytes, Pos=2
// Artist                           ArtistLength bytes
// CopyrightLength                  2 bytes, Pos=2
// Copyright                        CopyrightLength bytes
// CommentLength                    2 bytes, Pos=2
// Comment                          CommentLength bytes
//
void File_Rm::Cont()
{
    char* Tags=(char*)Buffer+Buffer_Offset;
    int16u Size;
    Tags+=2; //Version
    Size=BigEndian2int16u(Tags);
    Tags+=2; //Size
    Fill(Stream_General, 0, "PlayTime", Tags, Size);
    Tags+=Size; //XXX_Size
    Size=BigEndian2int16u(Tags);
    Tags+=2; //Size
    Fill(Stream_General, 0, "Performer", Tags, Size);
    Tags+=Size; //XXX_Size
    Size=BigEndian2int16u(Tags);
    Tags+=2; //Size
    Fill(Stream_General, 0, "Copyright", Tags, Size);
    Tags+=Size; //XXX_Size
    Size=BigEndian2int16u(Tags);
    Tags+=2; //Size
    Fill(Stream_General, 0, "Comment", Tags, Size);
}

//---------------------------------------------------------------------------
// Atom "DATA"
// ObjectVersion                    2 bytes, Pos=0
// NumPackets                       4 bytes, Pos=2
// NextDataHeader                   4 bytes, Pos=6
// Entry_Size                       4 bytes
// Entry                            Entry_Size-4 bytes;
//
void File_Rm::Data()
{
}

//---------------------------------------------------------------------------
// Atom "DATA", entry
// StreamNumber                     2 bytes, Pos=0
// Timestamp                        4 bytes, Pos=2
// PacketGroup                      1 bytes, Pos=6
// Flags                            1 bytes, Pos=7
// Datas                                     Pos=8
//
// Flags:
// bit0: Reliable
// bit1: Keyframe
//
void File_Rm::Data_Entry()
{
}

//---------------------------------------------------------------------------
// Atom "MDPR", MeDia PRoperties,  bytes
// ObjectVersion                    2 bytes, Pos=0
// StreamNumber                     2 bytes, Pos=2
// MaxBitrate                       4 bytes, Pos=4
// AverageBitrate                   4 bytes, Pos=8
// MaxPacketSize                    4 bytes, Pos=12
// AveragePacketSize                4 bytes, Pos=16
// StartTime                        4 bytes, Pos=20
// Preroll                          4 bytes, Pos=24
// Duration                         4 bytes, Pos=28
// StreamName_Size                  1 bytes, Pos=32
// StreamName                       StreamNameSize bytes, Pos=33
// MimeType_Size                    1 bytes, Pos=33+StreamNameSize
// MimeType                         MimeTypeSize bytes, Pos=34+StreamNameSize
// TypeSpecific_Size                4 bytes, Pos=34+StreamNameSize+MimeTypeSize
// TypeSpecific                     TypeSpecificLength bytes, Pos=35+StreamNameSize+MimeTypeSize
//
void File_Rm::Mdpr()
{
    if (Size<38)
        return;

    size_t Mdpr_Offset=32; //StreamNameSize
    int8u StreamName_Size=Buffer[Buffer_Offset+Mdpr_Offset]; //StreamName_Size
    Mdpr_Offset+=1;
    if (Mdpr_Offset+StreamName_Size>Size)
        return;
    std::string Name((char*)Buffer+Buffer_Offset+Mdpr_Offset, StreamName_Size); //StreamName
    Mdpr_Offset+=StreamName_Size;
    if (Mdpr_Offset+1>Size)
        return;
    int8u MimeType_Size=Buffer[Buffer_Offset+Mdpr_Offset]; //MimeType_Size
    Mdpr_Offset+=1;
    if (Mdpr_Offset+MimeType_Size>Size)
        return;
    std::string MimeType((char*)Buffer+Buffer_Offset+Mdpr_Offset, MimeType_Size); //StreamName
    Mdpr_Offset+=MimeType_Size;
    if (Mdpr_Offset+4>Size)
        return;
    int32u TypeSpecific_Size=BigEndian2int32u(Buffer+Buffer_Offset+Mdpr_Offset); //TypeSpecific_Size
    Mdpr_Offset+=4;
    if (Mdpr_Offset+TypeSpecific_Size>Size)
        return;

    //Parse
    bool MdprMakesSense=true;
         if (MimeType=="video/x-pn-realvideo")
        Mdpr_Video(Buffer_Offset+Mdpr_Offset, TypeSpecific_Size);
         if (MimeType=="video/x-pn-realvideo-encrypted")
    {
        Mdpr_Video(Buffer_Offset+Mdpr_Offset, TypeSpecific_Size);
        Fill("Encrypted", "Y");
    }
    else if (MimeType=="audio/x-pn-realaudio")
        Mdpr_Audio(Buffer_Offset+Mdpr_Offset, TypeSpecific_Size);
    else if (MimeType=="audio/x-pn-realaudio-encrypted")
    {
        Mdpr_Audio(Buffer_Offset+Mdpr_Offset, TypeSpecific_Size);
        Fill("Encrypted", "Y");
    }
    else if (MimeType=="video/text")
        Stream_Prepare(Stream_Text);
    else if (MimeType=="logical-fileinfo")
        Mdpr_FileInfo(Buffer_Offset+Mdpr_Offset, TypeSpecific_Size);

    //Finalize
    if (MdprMakesSense)
    {
        Fill("BitRate", BigEndian2int32u(Buffer+Buffer_Offset+8)); //AverageBitrate
        Fill("Delay", BigEndian2int32u(Buffer+Buffer_Offset+20)); //StartTime
        Fill("PlayTime", BigEndian2int32u(Buffer+Buffer_Offset+28)); //Duration
    }
}

//---------------------------------------------------------------------------
// Atom "MDPR", with Name=audio5,  bytes
// FourCC1                          4 bytes, Pos=0
// Version1                         2 bytes, Pos=4
// Unknown                          2 bytes, Pos=6
// FourCC2                          4 bytes, Pos=8
// Unknown                          4 bytes, Pos=12
// Version2                         2 bytes, Pos=16
// HeaderSize                       4 bytes, Pos=18
// Flavor                           2 bytes, Pos=22
// CodedFrameSize                   4 bytes, Pos=24
// Unknown                          4 bytes, Pos=28
// Unknown                          4 bytes, Pos=32
// Unknown                          4 bytes, Pos=36
// SubPacketH                       2 bytes, Pos=40
// FrameSize                        2 bytes, Pos=42
// SubPacketSize                    2 bytes, Pos=44
// Unknown                          2 bytes, Pos=46
// SampleRate                       4 bytes, Pos=48
// Unknown                          4 bytes, Pos=52
// Unknown                          4 bytes, Pos=56
// Channels                         2 bytes, Pos=60
// "genr"?                          4 bytes, Pos=62
// AudCodec                         4 bytes, Pos=66
// ...
//
void File_Rm::Mdpr_Audio(size_t Buffer_Offset, size_t Size)
{
    if (Size<70)
        return;

    Stream_Prepare(Stream_Audio);

    int32u FourCC1=CC4(Buffer+Buffer_Offset);
    int16u Version=BigEndian2int16u(Buffer+Buffer_Offset+4); //Version

    if (FourCC1==CC4((int8u*)".ra\xFD"))
    {
             if (Version==3)
        {
            Fill("Codec", ".ra3");
            Fill("SamplingRate", 8000);
            Fill("Resolution", 16);
            Fill("Channel(s)", BigEndian2int16u(Buffer+Buffer_Offset+8)); //Channels
        }
        else if (Version==4)
        {
            Fill("Codec", Buffer+Buffer_Offset+62, 4); //AudCodec
            Fill("SamplingRate", BigEndian2int16u(Buffer+Buffer_Offset+48)); //SampleRate
            Fill("Channel(s)", BigEndian2int16u(Buffer+Buffer_Offset+54)); //Channels
        }
        else if (Version==5)
        {
            Fill("Codec", Buffer+Buffer_Offset+66, 4); //AudCodec
            Fill("SamplingRate", BigEndian2int32u(Buffer+Buffer_Offset+48)); //SampleRate
            Fill("Channel(s)", BigEndian2int16u(Buffer+Buffer_Offset+60)); //Channels
        }
    }
    else if (FourCC1==CC4("LSD:"))
    {
            Fill("Codec", "RealAudio Lossless");
            Fill("SamplingRate", BigEndian2int32u(Buffer+Buffer_Offset+12)); //SampleRate
    }
}

//---------------------------------------------------------------------------
// Atom "MDPR", with Name=FileInfo
// Size                             4 bytes, Pos=0
// ObjectVersion                    2 bytes, Pos=4
// NumPhysicalStreams               2 bytes, Pos=6
// NumRules                         2 bytes, Pos=8
// NumProperties                    2 bytes, Pos=10
// SizeOfProperty1                  4 bytes, Pos=12
// Property1                        SizeOfProperty1-4 bytes
// ...
//
void File_Rm::Mdpr_FileInfo(size_t Buffer_Offset, size_t Size)
{
    if (Size<16)
        return;

    size_t Info_Offset=12;
    while (Info_Offset+4<Size)
    {
        int32u Property_Size=BigEndian2int32u(Buffer+Buffer_Offset+Info_Offset); //SizeOfProperty
        if (Property_Size==0)
            return;
        Info_Offset+=4;
        if (Info_Offset+Property_Size>Size)
            return;
        Mdpr_FileInfo_Info(Buffer_Offset+Info_Offset, Property_Size-4);
        Info_Offset+=Property_Size-4;
    }
}

//---------------------------------------------------------------------------
// Atom "MDPR", with Name=FileInfo, Info
// ObjectVersion                    2 bytes, Pos=0
// NameLength                       1 bytes, Pos=2
// Name                             NameLength bytes, Pos=3
// Type                             4 bytes, Pos=3+NameLength
// ValueLength                      2 bytes, Pos=7+NameLength
// ValueData                        ValueLength bytes, Pos=9+NameLength
//
void File_Rm::Mdpr_FileInfo_Info(size_t Buffer_Offset, size_t Size)
{
    if (Size<9)
        return;

    int8u Name_Size=BigEndian2int8u(Buffer+Buffer_Offset+2);
    Ztring Name; Name.From_Local((char*)Buffer+Buffer_Offset+3, Name_Size);
    int32u Type=BigEndian2int32u(Buffer+Buffer_Offset+3+Name_Size);
    int16u Value_Size=BigEndian2int16u(Buffer+Buffer_Offset+7+Name_Size);

    Ztring Value;
         if (Type==0)
        Value.From_Number(BigEndian2int32u(Buffer+Buffer_Offset+9+Name_Size));
    else if (Type==2)
        Value.From_Local((char*)Buffer+Buffer_Offset+9+Name_Size, Value_Size);
}

//---------------------------------------------------------------------------
// Atom "MDPR", with Name=video, 34 bytes
// Size                             4 bytes, Pos=0
// FourCC1                          4 bytes, Pos=4
// FourCC2                          4 bytes, Pos=8
// Width                            2 bytes, Pos=12
// Height                           2 bytes, Pos=14
// BitsPerSample                    2 bytes, Pos=16
// Unknown                          4 bytes, Pos=18
// FrameRate                        2 bytes, Pos=22
// Unknown                          2 bytes, Pos=24
// Unknown                          2 bytes, Pos=26
// Unknown                          2 bytes, Pos=28
// Unknown                          2 bytes, Pos=30
// Unknown                          2 bytes, Pos=32
//
void File_Rm::Mdpr_Video(size_t Buffer_Offset, size_t Size)
{
    if (Size<34)
        return;

    Stream_Prepare(Stream_Video);
    Fill("Codec", Buffer+Buffer_Offset+8, 4); //FourCC2
    Fill("Width", BigEndian2int16u(Buffer+Buffer_Offset+12)); //Width
    Fill("Height", BigEndian2int16u(Buffer+Buffer_Offset+14)); //Height
    Fill("Resolution", BigEndian2int16u(Buffer+Buffer_Offset+16)); //Resolution
    Fill("FrameRate", BigEndian2int16u(Buffer+Buffer_Offset+22)); //FrameRate
}

//---------------------------------------------------------------------------
// Atom "PROP", global PROPerties, 42 bytes
// ObjectVersion                    2 bytes, Pos=0
// MaxBitrate                       4 bytes, Pos=2
// AverageBitrate                   4 bytes, Pos=6
// MaxPacketSize                    4 bytes, Pos=10
// AveragePacketSize                4 bytes, Pos=14
// NumberPackets                    4 bytes, Pos=18
// Duration                         4 bytes, Pos=22
// Preroll                          4 bytes, Pos=26
// IndexOffset                      4 bytes, Pos=30
// DataOffset                       4 bytes, Pos=34
// NumStreams                       2 bytes, Pos=38
// Flags                            2 bytes, Pos=40
//
// Flags:
// bit0=Save_Enabled
// bit1=Perfect_Play
// bit2=Live_Broadcast
// bit3=Allow download
//
void File_Rm::Prop()
{
    //Coherancy
    if (Size<42 || General.empty())
        return;

    Fill(Stream_General, 0, "BitRate", BigEndian2int32u(Buffer+Buffer_Offset+6)); //AverageBitrate
    Fill(Stream_General, 0, "PlayTime", BigEndian2int32u(Buffer+Buffer_Offset+22)); //Duration
}

//---------------------------------------------------------------------------
// Atom "RJMD", Property, warning no atom size!
// ObjectVersion                    4 bytes, Pos=0
// Property1_Size                   4 bytes, Pos=4
// Property1_Data                   Property1_Size-4 bytes
//
void File_Rm::Rjmd()
{
    /*
    size_t Local_Offset=4;
    size_t Local_Size=0;

    while (Local_Offset+4<Size)
    {
        Local_Size=BigEndian2int32u(End+Buffer_Offset+Local_Offset);
        Rjmd_Property(Buffer_Offset+Local_Offset+4, Local_Size-4, _T(""));
    }
    */
}

//---------------------------------------------------------------------------
// Atom "RJMD", MetadataProperty
// type                             4 bytes, Pos=0
// flags                            4 bytes, Pos=4
// value_offset                     4 bytes, Pos=8
// subproperties_offset             4 bytes, Pos=12
// num_subproperties                4 bytes, Pos=16
// name_length                      4 bytes, Pos=20
// name                             name_length bytes, Pos=24
// value_length                     4 bytes, Pos=24+name_length
// value                            value_length bytes, Pos=28+name_length
// PropListEntry                    num_subproperties*8 bytes, Pos
// MetadataProperty
//
//
void File_Rm::Rjmd_Property(size_t Buffer_Offset, size_t Size, Ztring CompleteName)
{
    if (Size<28)
        return;

    /*
    size_t Local_Offset=20;
    size_t Local_Size=0;

    int32u Type=BigEndian2int32u(End+Buffer_Offset); //Type
    int32u SubProperties_Count=BigEndian2int32u(End+Buffer_Offset+16); //num_subproperties
    Local_Size=BigEndian2int32u(End+Buffer_Offset+Local_Offset); //name_length
    Local_Offset+=4;
    Ztring Name; Name.From_Local((char*)End+Buffer_Offset+Local_Offset, Local_Size); //name
    Local_Offset+=Local_Size;
    Local_Size=BigEndian2int32u(End+Buffer_Offset+Local_Offset); //value_length
    Local_Offset+=4;
    Ztring Value;
         if (Type==0)
            Value.From_Local((char*)End+Buffer_Offset+Local_Offset, Local_Size); //value
    Local_Offset+=Local_Size;

    if (!CompleteName.empty())
        CompleteName+=_T("/");
    CompleteName=Name;

    if (!Value.empty())
        Fill(Stream_General, 0, CompleteName.To_Local().c_str(), Value);

    Local_Offset+=SubProperties_Count*8; //PropListEntry;
    while (Local_Offset+4<Size)
    {
        Local_Size=BigEndian2int32u(End+Buffer_Offset+Local_Offset);
        Rjmd_Property(Buffer_Offset+Local_Offset+4, Local_Size-4, CompleteName);
    }
    */
}

//---------------------------------------------------------------------------
// Atom ".RMF", Real Media Format, 10 bytes
// ObjectVersion                    2 bytes, Pos=0
// FileVersion                      4 bytes, Pos=2
// HeaderCount                      4 bytes, Pos=6
//
void File_Rm::Rmf()
{
    Stream_Prepare(Stream_General);
    Fill("Format", "RMF");
}

//---------------------------------------------------------------------------
void File_Rm::HowTo(stream_t StreamKind)
{
    switch (StreamKind)
    {
        case (Stream_General) :
            Fill_HowTo("Format", "R");
            Fill_HowTo("BitRate", "R");
            Fill_HowTo("Performer", "R");
            Fill_HowTo("Copyright", "R");
            Fill_HowTo("Comment", "R");
            Fill_HowTo("PlayTime", "R");
            break;
        case (Stream_Video) :
            Fill_HowTo("Codec", "R");
            Fill_HowTo("Width", "R");
            Fill_HowTo("Height", "R");
            Fill_HowTo("Resolution", "R");
            Fill_HowTo("FrameRate", "R");
            Fill_HowTo("BitRate", "R");
            Fill_HowTo("Delay", "R");
            Fill_HowTo("PlayTime", "R");
            break;
        case (Stream_Audio) :
            Fill_HowTo("Codec", "R");
            Fill_HowTo("SamplingRate", "R");
            Fill_HowTo("Channel(s)", "R");
            Fill_HowTo("BitRate", "R");
            Fill_HowTo("Delay", "R");
            Fill_HowTo("PlayTime", "R");
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

#endif //MEDIAINFO_RM_YES

