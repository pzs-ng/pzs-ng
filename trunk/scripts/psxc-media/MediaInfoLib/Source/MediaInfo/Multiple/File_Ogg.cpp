// File_Ogg - Info for ogg files
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
#if defined(MEDIAINFO_FLV_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <ZenLib/ZtringListList.h>
#include <ZenLib/Utils.h>
#include "MediaInfo/Multiple/File_Ogg.h"
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Format
//***************************************************************************

void File_Ogg::Read_Buffer_Continue()
{
    ShouldStop=false;

    //Integrity test
    if (Buffer_Size<4 || CC4(Buffer)!=CC4("OggS"))
    {
        File_Offset=File_Size;
        return;
    }

    Stream_Prepare(Stream_General);
    Fill("Format", "Ogg");

    //Buffer
    while (!ShouldStop)
    {
        ChunkHeader_Analyse();
        if (!ShouldStop)
        {
            ChunkData_Analyse();
        }
    }

    //Some headers my have not comments, but Analysing start withs comments... We analyse this chunks now
    while (!ChunkHeader_ID.empty())
        Identification_Analyse(ChunkHeader_ID[0]);

    /*
    //Finding last frame time
    Buffer_Offset=End_Size-14;
    while (Buffer_Offset>0 && CC4(End+Buffer_Offset)!=CC4("OggS"))
        Buffer_Offset--;
    if (Buffer_Offset==0)
        return 1; //Time not found, but begin is OK...
    int64u Size=LittleEndian2int64u(End+Buffer_Offset+6);
    Ztring ID; ID.From_Number(LittleEndian2int32u(End+Buffer_Offset+14));
    size_t PlayTime=0;
    for (size_t Pos=0; Pos<Audio.size(); Pos++)
        if (Get(Stream_Audio, Pos, _T("ID"))==ID && Get(Stream_Audio, Pos, _T("SamplingRate")).To_int32u()!=0)
            PlayTime=Size*1000/Get(Stream_Audio, Pos, _T("SamplingRate")).To_int32u();
    for (size_t Pos=0; Pos<Video.size(); Pos++)
        if (Get(Stream_Video, Pos, _T("ID"))==ID && Get(Stream_Video, Pos, _T("FrameRate")). To_float32()!=0)
            PlayTime=Size*1000/Get(Stream_Video, Pos, _T("FrameRate")).To_int32u();
    if (PlayTime)
        Fill(Stream_General, 0, "PlayTime", PlayTime);
    */

    //No need of more
    File_Offset=File_Size;
}

//---------------------------------------------------------------------------
// ChunkHeader
// "OggS"                           4 bytes, Pos=0
// Version                          1 byte, Pos=4
// Flags                            1 byte, Pos=5
// absolute granule position        8 bytes, Pos=6
// SreamID                          4 byte, Pos=14
// Page counter                     4 bytes, Pos=18
// Checksum                         4 bytes, Pos=22
// DataSizeSize                     1 bytes, Pos=26
// DataSize                         DataSizeSize bytes, Pos=27, Xiph method
//
// Flags:
// Fresh/Continued packet
// Not first/first page of logical bitstream (BOS)
// Not last/last page of logical bitstream (EOS)
//
void File_Ogg::ChunkHeader_Analyse()
{
    //Look for header
    while (Buffer_Offset+28<Buffer_Size && CC4(Buffer)!=CC4("OggS"))
        Buffer_Offset++;
    if (Buffer_Offset+28>=Buffer_Size)
    {
        ShouldStop=true;
        return;
    }

    //DataSizeSize
    int8u DataSizeSize=Buffer[Buffer_Offset+26];
    if (Buffer_Offset+26+DataSizeSize>=Buffer_Size)
    {
        ShouldStop=true;
        return;
    }

    //DataSize
    Size=0;
    for (int8u Pos=0; Pos<DataSizeSize; Pos++)
        Size+=Buffer[Buffer_Offset+27+Pos];
    if (Buffer_Offset+27+DataSizeSize+Size>=Buffer_Size)
    {
        ShouldStop=true;
        return;
    }

    //Save position and ID
    size_t Pos=ChunkHeader_Offset.size();
    ChunkHeader_Offset.resize(Pos+1);
    ChunkHeader_ID.resize(Pos+1);
    ChunkData_Offset.resize(Pos+1);
    ChunkData_Size.resize(Pos+1);
    ChunkHeader_Offset[Pos]=Buffer_Offset;
    ChunkHeader_ID[Pos]=LittleEndian2int32u(Buffer+Buffer_Offset+14);
    ChunkData_Offset[Pos]=Buffer_Offset+27+DataSizeSize;
    ChunkData_Size[Pos]=Size;

    //Set Buffer_Offset
    Buffer_Offset+=27+DataSizeSize;
}

//---------------------------------------------------------------------------
// ChunkData
// If First byte&0x01==1, this is a header packet:
// Type                             1 bytes, Pos=0
// Datas                            X bytes, Pos=1
//
// If First byte&0x01==0, this is a data packet:
// 0                                1 bit (Indicates data packet)
// Bit 2 of lenbytes                1 bit
// unused                           1 bit
// Keyframe                         1 bit
// unused                           2 bits
// Bit 0 of lenbytes                1 bit
// Bit 1 of lenbytes                1 bit
// SamplesCount                     lenbytes bytes (Frames for video, Samples for audio, 1ms units for text)
// Data                             X bytes, Pos=1+lenbytes
void File_Ogg::ChunkData_Analyse()
{
    //Integrity
    if (Size<1)
        return;

    //Parse
         if (0) ;
    else if (Buffer[Buffer_Offset]==0x01) Identification_Analyse();
    else if (Buffer[Buffer_Offset]==0x80) Identification_Analyse();
    else if (Buffer[Buffer_Offset]==0x03) Comment_Analyse();
    else if (Buffer[Buffer_Offset]==0x81) Comment_Analyse();
    else if (Buffer[Buffer_Offset]==0x05) Setup_Analyse();
    else if (Buffer[Buffer_Offset]&0x81==0)
    {
    }

    Buffer_Offset+=Size;
}

//---------------------------------------------------------------------------
// Stream chunk
// StreamKind                       6-8 bytes, Pos=1
// ... (Depend of StreamKind)
//
void File_Ogg::Identification_Analyse()
{
    //We do nothing here, only save offset
}

//---------------------------------------------------------------------------
// Stream chunk
// StreamKind                       6-8 bytes, Pos=1
// ... (Depend of StreamKind)
//
void File_Ogg::Identification_Analyse(int32u ID)
{
    //We should have comment chunk, we can start to analyse, starting to find the corresponding Identification chunk

    if (ChunkHeader_ID.empty() || ChunkHeader_ID.size()!=ChunkHeader_Offset.size() || ChunkHeader_ID.size()!=ChunkData_Offset.size() || ChunkHeader_ID.size()!=ChunkData_Size.size())
        return;

    size_t Pos=0;
    while (Pos<ChunkHeader_ID.size() && ChunkHeader_ID[Pos]!=ID)
        Pos++;
    if (Pos==ChunkHeader_ID.size())
        return;
    if (Buffer[ChunkData_Offset[Pos]]==0x01 || Buffer[ChunkData_Offset[Pos]]==0x80) //This is a identifier
    {
        Identification_Analyse(Buffer+ChunkData_Offset[Pos]+1, ChunkData_Size[Pos]-1);
        Fill("ID", ChunkHeader_ID[Pos]);
    }

    //Erase chunk
    ChunkHeader_ID.erase(ChunkHeader_ID.begin()+Pos);
    ChunkHeader_Offset.erase(ChunkHeader_Offset.begin()+Pos);
    ChunkData_Offset.erase(ChunkData_Offset.begin()+Pos);
    ChunkData_Size.erase(ChunkData_Size.begin()+Pos);
}

//---------------------------------------------------------------------------
// Identification chunk, video
// StreamKind                       8 bytes, Pos=0
// Datas                            X bytes, Pos=4
//
void File_Ogg::Identification_Analyse(const int8u* Buffer, size_t Buffer_Size)
{
    //Integrity
    if (Buffer_Size<8)
        return;

    if (0) ;
    else if (CC6(Buffer)==CC6("vorbis"))       Identification_Vorbis_Analyse(Buffer, Buffer_Size);
    else if (CC6(Buffer)==CC6("theora"))       Identification_Theora_Analyse(Buffer, Buffer_Size);
    else if (CC8(Buffer)==CC8("video\0\0\0"))  Identification_Video_Analyse (Buffer, Buffer_Size);
    else if (CC8(Buffer)==CC8("audio\0\0\0"))  Identification_Audio_Analyse (Buffer, Buffer_Size);
    else if (CC8(Buffer)==CC8("text\0\0\0\0")) Identification_Text_Analyse  (Buffer, Buffer_Size);
}

//---------------------------------------------------------------------------
// Identification chunk, Vorbis, at least 29 bytes
// "vorbis"                         6 bytes, Pos=0
// Version                          4 bytes, Pos=6
// Channels                         1 bytes, Pos=10
// Samplingrate                     4 bytes, Pos=11
// BitRate_Maximum                  4 bytes, Pos=15
// BitRate_Nominal                  4 bytes, Pos=19
// BitRate_Minimum                  4 bytes, Pos=23
// BlockSize_0                      4 bits, Pos=27 (2^Value)
// BlockSize_1                      4 bits, Pos=27.4 (2^Value)
// Framing                          1 bits, Pos=28
//
 void File_Ogg::Identification_Vorbis_Analyse(const int8u* Buffer, size_t Buffer_Size)
{
    //Integrity
    if (Buffer_Size<29)
        return;

    Stream_Prepare(Stream_Audio);
    Fill("Codec", "Vorbis");
    if (LittleEndian2int32u(Buffer+15)<0x80000000) //This is a signed value, and negative values are not OK
        Fill("BitRate", LittleEndian2int32u(Buffer+19)); //BitRate_Nominal
    Fill("Channel(s)", LittleEndian2int8u(Buffer+10)); //Channess
    Fill("SamplingRate", LittleEndian2int32u(Buffer+11));
}

//---------------------------------------------------------------------------
// Identification chunk, Theora, at least 41 bytes
// "theora"                         6 bytes, Pos=0
// Version                          3 bytes, Pos=6
// Width_Blocks                     2 bytes, Pos=9
// Height_Blocks                    2 bytes, Pos=11
// Width                            3 bytes, Pos=13 (displayable width)
// Height                           3 bytes, Pos=16 (displayable heigth)
// Pos_X                            1 byte, Pos=19 (Pixels between 0 and Pox_X are not displayable)
// Pos_Y                            1 byte, Pos=20 (Pixels between 0 and Pox_Y are not displayable)
// FrameRate_N                      4 bytes, Pos=21
// FrameRate_D                      4 bytes, Pos=25 (FrameRate=FrameRate_N/FrameRate_D)
// PixelRatio_N                     3 bytes, Pos=29
// PixelRatio_D                     3 bytes, Pos=32 (PixelRatio=PixelRatio_N/PixelRatio_D, may be zero)
// ColorSpace                       1 byte, Pos=35
// NOMBR (VBR???)                   3 bytes, Pos=36
// Quality                          6 bits, Pos=39 (depend of encoder)
// KFGSHIFT???                      5 bits, Pos=
// Pixel_Format                     2 bits, Pos= (0=4:2:0, 2=4:2:2, 3=4:4:4)
// Reserved                         3 bits, Pos=
//
 void File_Ogg::Identification_Theora_Analyse(const int8u* Buffer, size_t Buffer_Size)
{
    //Integrity
    if (Buffer_Size<41)
        return;

    Stream_Prepare(Stream_Video);
    Fill("Codec", "Theora");
    if (BigEndian2int16u(Buffer+6)>0x0302) //Version 3.2.x
        return;
    Fill("FrameRate", ((float)BigEndian2int32u(Buffer+21))/(float)BigEndian2int32u(Buffer+25), 3);
    int32u Width=BigEndian2int32u(Buffer+12)&0x00FFFFFF; //Only 24 bits
    int32u Height=BigEndian2int32u(Buffer+15)&0x00FFFFFF; //Only 24 bits
    int32u PixelRatio_N=BigEndian2int32u(Buffer+29);
    int32u PixelRatio_D=BigEndian2int32u(Buffer+32);
    float PixelRatio=1;
    if (PixelRatio_N && PixelRatio_D)
        PixelRatio=((float)PixelRatio_N)/(float)PixelRatio_D;
    Fill("Width", Width);
    Fill("Height", Height);
    Fill("AspectRatio", ((float)Width)/((float)Height)*PixelRatio, 3);
}

//---------------------------------------------------------------------------
// Identification chunk, video, at least 52 bytes
// "video\0\0\0"                    8 bytes, Pos=0
// fccHandler                       4 bytes, Pos=8
// SizeOfStructure                  4 bytes, Pos=12
// TimeUnit                         8 bytes, Pos=16 (10000000/TimeUnit is stream tick rate in ticks/sec)
// SamplesPerUnit                   8 bytes, Pos=24
// DefaultLengh                     4 bytes, Pos=32 (in media time)
// BufferSize                       4 bytes, Pos=36
// BitsPerSample                    2 bytes, Pos=40
// Reserved                         2 bytes, Pos=42
// Width                            4 bytes, Pos=44
// Height                           4 bytes, Pos=48
//
void File_Ogg::Identification_Video_Analyse(const int8u* Buffer, size_t Buffer_Size)
{
    //Integrity
    if (Buffer_Size<52)
        return;

    Stream_Prepare(Stream_Video);
    Fill("Codec", Buffer+8, 4);
    Fill("FrameRate", (float)10000000/(float)LittleEndian2int64u(Buffer+16), 3);
    Fill("Width", LittleEndian2int32u(Buffer+44));
    Fill("Height", LittleEndian2int32u(Buffer+48));
}

//---------------------------------------------------------------------------
// Identification chunk, audio, at least 52 bytes
// "audio\0\0\0"                    8 bytes, Pos=0
// fccHandler                       4 bytes, Pos=8
// SizeOfStructure                  4 bytes, Pos=12
// TimeUnit                         8 bytes, Pos=16
// SamplesPerUnit                   8 bytes, Pos=24
// DefaultLengh                     4 bytes, Pos=32 (in media time)
// BufferSize                       4 bytes, Pos=36
// BitsPerSample                    2 bytes, Pos=40
// Reserved                         2 bytes, Pos=42
// Channels                         2 bytes, Pos=44
// BlockAlign                       2 bytes, Pos=46
// AvgBytesPerSec                   4 bytes, Pos=48
//
void File_Ogg::Identification_Audio_Analyse(const int8u* Buffer, size_t Buffer_Size)
{
    //Integrity
    if (Buffer_Size<52)
        return;

    Stream_Prepare(Stream_Audio);
    Fill("Codec", Buffer+8, 4);
    if (LittleEndian2int32u(Buffer+48)<0x80000000) //This is a signed value, and negative values are not OK
        Fill("BitRate", LittleEndian2int32u(Buffer+48)*8);
    Fill("Channel(s)", LittleEndian2int8u(Buffer+44)==5?6:LittleEndian2int8u(Buffer+44)); //5 channels are 5.1
    Fill("SamplingRate", LittleEndian2int64u(Buffer+24));
}

//---------------------------------------------------------------------------
// Identification chunk, text
// "text\0\0\0\0"                   8 bytes, Pos=0
//
void File_Ogg::Identification_Text_Analyse(const int8u* Buffer, size_t Buffer_Size)
{
    Stream_Prepare(Stream_Text);
    Fill("Codec", "Subrip");
}

//---------------------------------------------------------------------------
// Comment chunk
// Type                             1 byte
// "vorbis"                         6 bytes
// vendor_length                    4 bytes
// vendor_string                    vendor_length bytes
// user_comment_list_HowMany        4 bytes
// length                           4 bytes
// comment                          length bytes
// length                           4 bytes
// comment                          length bytes
// (...)
// Framing bit                      1 bit
//
void File_Ogg::Comment_Analyse()
{
    if (ChunkHeader_ID.empty() || Size<12)
        return;

    if (!(CC1(Buffer+Buffer_Offset)==0x03 && CC6(Buffer+Buffer_Offset+1)==CC6("vorbis")) && !(CC1(Buffer+Buffer_Offset)==0x81 && CC6(Buffer+Buffer_Offset+1)==CC6("theora")))
        return; //Not realy a comment

    //We have comment chunk, we can start to analyse, starting to find the corresponding Identification chunk
    Identification_Analyse(ChunkHeader_ID[ChunkHeader_ID.size()-1]);

    //We can start to analyse comments
    size_t Comment_Buffer_Offset=Buffer_Offset+11;
    size_t Comment_Size=LittleEndian2int32u(Buffer+Buffer_Offset+7);
    Fill("Encoded_Library", Buffer+Comment_Buffer_Offset, Comment_Size, true);

    Comment_Buffer_Offset+=Comment_Size;
    size_t CommentCount=LittleEndian2int32u(Buffer+Comment_Buffer_Offset); //Count of comments
    Comment_Buffer_Offset+=4;
    while (CommentCount>0 && Comment_Buffer_Offset+5<Buffer_Offset+Size)
    {
        Comment_Size=LittleEndian2int32u(Buffer+Comment_Buffer_Offset); //Read size
        Comment_Buffer_Offset+=4; //Size
        if (Comment_Buffer_Offset+Comment_Size<Buffer_Offset+Size)
        {
            Ztring String; String.From_UTF8((char*)(Buffer+Comment_Buffer_Offset), Comment_Size);
            Ztring ID=String.SubString(_T(""), _T("="));
            Ztring Text;
            if (!ID.empty())
                Text=String.SubString(_T("="), _T(""));
            else //Out of specifications!!! Grrr Doom9 :)
            {
                ID=_T("ENCODER");
                Text=String;
            }

            //Fill depends of ID
            if (0) ;
            else if (ID==_T("TITLE"))           Fill(Stream_General, 0, "Track", Text);
            else if (ID==_T("VERSION"))         Fill(Stream_General, 0, "Track/More", Text);
            else if (ID==_T("ALBUM"))           Fill(Stream_General, 0, "Album", Text);
            else if (ID==_T("TRACKNUMBER"))     Fill(Stream_General, 0, "Track/Position", Text);
            else if (ID==_T("ARTIST"))          Fill(Stream_General, 0, "Performer", Text);
            else if (ID==_T("PERFORMER"))       Fill(Stream_General, 0, "Performer", Text);
            else if (ID==_T("COPYRIGHT"))       Fill(Stream_General, 0, "Copyright", Text);
            else if (ID==_T("LICENCE"))         Fill(Stream_General, 0, "TermsOfUse", Text);
            else if (ID==_T("ORGANIZATION"))    Fill(Stream_General, 0, "Producer", Text);
            else if (ID==_T("DESCRIPTION"))     Fill(Stream_General, 0, "Comment", Text);
            else if (ID==_T("GENRE"))           Fill(Stream_General, 0, "Genre", Text);
            else if (ID==_T("DATE"))            Fill(Stream_General, 0, "Recorded_Date", Text);
            else if (ID==_T("LOCATION"))        Fill(Stream_General, 0, "Recorded_Location", Text);
            else if (ID==_T("CONTACT"))         Fill(Stream_General, 0, "Publisher", Text);
            else if (ID==_T("ISRC"))            Fill(Stream_General, 0, "ISRC", Text);
            else if (ID==_T("COMMENT"))         Fill(Stream_General, 0, "Comment", Text);
            else if (ID==_T("COMMENTS"))        Fill(Stream_General, 0, "Comment", Text);
            else if (ID==_T("AUTHOR"))          Fill(Stream_General, 0, "Performer", Text);
            else if (ID==_T("ENCODER"))         Fill(Stream_General, 0, "Encoded_Application", Text);
            else if (ID==_T("ENCODED_USING"))   Fill(Stream_General, 0, "Encoded_Application", Text);
            else if (ID==_T("ENCODER_URL"))     Fill(Stream_General, 0, "Encoded_Application/Url", Text);
            else if (ID==_T("LWING_GAIN"))      Fill(                   "ReplayGain_Gain", Text);
            else if (ID.find(_T("CHAPTER"))==0)
            {
                if (ID.find(_T("NAME"))==Error)
                {
                    size_t Pos=Chapters_ID.size();
                    Chapters_ID.resize(Pos+1);
                    Chapters_ID[Pos]=ID.SubString(_T("CHAPTER"), _T(""));
                    Chapters_Text.resize(Pos+1);
                    Chapters_Text[Pos]=Text;
                }
                else
                {
                    Ztring ID_ToFind=ID.SubString(_T("CHAPTER"), _T("NAME"));
                    size_t Pos=0;
                    while (Pos<Chapters_ID.size() && Chapters_ID[Pos]!=ID_ToFind)
                        Pos++;
                    if (Pos<Chapters_ID.size() && Chapters_Text.size()==Chapters_ID.size())
                    {
                        Text.FindAndReplace(_T("\n"), _T(""), Text.size()-1); //Some chapters names have extra characters, not needed
                        Text.FindAndReplace(_T("\r"), _T(""), Text.size()-1); //Some chapters names have extra characters, not needed
                        Text.FindAndReplace(_T(" "), _T(""), Text.size()-1); //Some chapters names have extra characters, not needed
                        Chapters_Text[Pos]+=_T(" ");
                        Chapters_Text[Pos]+=Text;
                    }
                }
            }
            else                                Fill(                   ID.To_Local().c_str(), Text);
            /*
            else if (ID==_T("LANGUAGE") && Commentaire[Commentaire_EnCours]=='A') {Audio[Commentaire_I[Commentaire_EnCours]](_T("Language/String"))=Texte; Audio[Commentaire_I[Commentaire_EnCours]](_T("Language"))=Ztring(Texte.c_str(),2);}
            else if (ID==_T("LANGUAGE") && Commentaire[Commentaire_EnCours]=='T') {Text[Commentaire_I[Commentaire_EnCours]](_T("Language/String"))=Texte; Text[Commentaire_I[Commentaire_EnCours]](_T("Language"))=Ztring(Texte.c_str(),2);}
            else if (Commentaire[Commentaire_EnCours]=='A')
            {
                //TODO : Chaine_Tableau2 ou 1 gere mal quand Chaine[0] est vide et pas Chaine[1]
                //TODO : RechercherEntre : si pas _T("="), retorune Buffer, pas cool
                if (ID==_T(""))
                {
                    ZtringList C3=Ztring(Ztring(_T("(Note by MediaInfo, Out of specifications);"))+Texte);
                    C3(Info_Name_Text)=_T("(Note by MediaInfo, Out of specifications)");
                    Audio[Commentaire_I[Commentaire_EnCours]].push_back(C3);
                }
                else
                {
                    ZtringList C3=Ztring(C2.SubString(_T(""), _T("="))+_T(";")+C2.SubString(_T("="), _T("")));
                    Ztring C4=C3(0);
                    C3(Info_Name_Text)=C4;
                    Audio[Commentaire_I[Commentaire_EnCours]].push_back(C3);
                }
            }
            else if (Commentaire[Commentaire_EnCours]=='T') Text[Commentaire_I[Commentaire_EnCours]].push_back((C2.SubString(_T(""), _T("="))+_T(";")+C2.SubString(_T("="), _T(""))).c_str()); //Video C++ patch, doesn't like push_back overload
            else
            {
                 General[0].push_back(C2.SubString(_T(""), _T("=")));
                 General[0](C2.SubString(_T(""), _T("=")), Info_Name_Text)=C2.SubString(_T(""), _T("="));
                 General[0](C2.SubString(_T(""), _T("=")))=C2.SubString(_T("="), _T(""));
            }
            */

            CommentCount--;
            Comment_Buffer_Offset+=Comment_Size;
        }
        else
            CommentCount=0;
    }

    if (!Chapters_ID.empty() && Chapters_Text.size()==Chapters_ID.size())
    {
        Stream_Prepare(Stream_Chapters);
        for (size_t Pos=0; Pos<Chapters_ID.size(); Pos++)
            Fill(Chapters_ID[Pos].To_Local().c_str(), Chapters_Text[Pos]);
        Fill("Total", Chapters_ID[Chapters_ID.size()-1].To_int32u());
        Chapters_ID.clear();
        Chapters_Text.clear();
    }

}

//---------------------------------------------------------------------------
// Setup chunk
//
void File_Ogg::Setup_Analyse()
{
    ShouldStop=true; //At this step, header information is over
}

//***************************************************************************
// Information
//***************************************************************************

//---------------------------------------------------------------------------
void File_Ogg::HowTo(stream_t StreamKind)
{
    switch (StreamKind)
    {
        case (Stream_General) :
            Fill_HowTo("Format", "R");
            Fill_HowTo("BitRate", "R");
            Fill_HowTo("PlayTime", "R");
            Fill_HowTo("Album", "R ALBUM");
            Fill_HowTo("Movie", "R TITLE");
            Fill_HowTo("Movie/More", "R VERSION");
            Fill_HowTo("Track", "R TITLE");
            Fill_HowTo("Track/More", "R VERSION");
            Fill_HowTo("Artist", "R ARTIST");
            Fill_HowTo("Encoded_Date", "R DATE");
            Fill_HowTo("Recorded_Location", "R LOCATION");
            Fill_HowTo("Copyright", "R COPYRIGHT");
            Fill_HowTo("Comment", "R DESCRIPTION or COMMENT");
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
            Fill_HowTo("Codec", "R");
            Fill_HowTo("BitRate", "R");
            Fill_HowTo("Channel(s)", "R");
            Fill_HowTo("SamplingRate", "R");
            Fill_HowTo("Language", "R LANGUAGE");
            Fill_HowTo("Language/String", "R");
            Fill_HowTo("Language/Info", "R");
            break;
        case (Stream_Text) :
            Fill_HowTo("Codec", "R");
            Fill_HowTo("Language", "R");
            Fill_HowTo("Language/String", "R LANGUAGE");
            Fill_HowTo("Language/Info", "R");
            break;
        case (Stream_Chapters) :
            Fill_HowTo("Total", "R");
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

#endif //MEDIAINFO_OGG_YES
