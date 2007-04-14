// File_Dvdv - Info for DVD objects (IFO) files
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
//
// Information about DVD objects
// (.ifo files on DVD-Video)
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
// Compilation condition
#if defined(MEDIAINFO_DVDV_YES) || (!defined(MEDIAINFO_VIDEO_NO) && !defined(MEDIAINFO_DVDV_NO))
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <wx/wxprec.h>
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#include <ZenLib/Utils.h>
#include "MediaInfo/Multiple/File_Dvdv.h"
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//---------------------------------------------------------------------------
const Char*  IFO_CodecV[]=
{
    _T("MPEG-1 Video"),
    _T("MPEG-2 Video"),
    _T(""),
    _T(""),
};
const Char*  IFO_AspectRatio[]=
{
    _T("1.333"),
    _T(""),
    _T(""),
    _T("1.778"),
};
const Char*  IFO_BitRate_Mode[]=
{
    _T("VBR"),
    _T("CBR"),
};
const size_t IFO_Width[]=
{720, 704, 352, 352};
const size_t IFO_Height[2][4]=
{{480, 480, 480, 240},  //NTSC
 {576, 576, 576, 288}}; //PAL
const size_t IFO_FrameRate[]=
{25, 30};
const Char*  IFO_CodecA[]=
{
    _T("AC3"),
    _T(""),
    _T("MPA1"),
    _T("MPA2"),
    _T("LPCM"),
    _T(""),
    _T("DTS"),
    _T("SDDS"),
};
const size_t IFO_Resolution[]=
{16, 20, 24, 0};
const size_t IFO_Channels[]=
{1, 2, 3, 4, 5, 6, 0, 0};
const size_t IFO_SamplingRate[]=
{48000, 0, 0, 0, 0, 0, 0, 0};
const Char*  IFO_Language_More[]=
{
    _T(""),
    _T("For visually impaired"),
    _T("Director's comments"),
    _T("Director's comments"),
};
const Char*  IFO_Language_MoreA[]=
{
    _T(""),
    _T(""),
    _T("For visually impaired"),
    _T("Director's comments"),
    _T("Director's comments"),
    _T(""),
    _T(""),
    _T(""),
};
const Char*  IFO_Language_MoreT[]=
{
    _T(""),
    _T(""),
    _T("Large"),
    _T("Children"),
    _T(""),
    _T(""),
    _T("Large"),
    _T("Children"),
    _T(""),
    _T("Forced"),
    _T(""),
    _T(""),
    _T(""),
    _T("Director comments"),
    _T("Director comments large"),
    _T("Director comments children"),
};


//---------------------------------------------------------------------------

//***************************************************************************
// Format
//***************************************************************************

//---------------------------------------------------------------------------
void File_Dvdv::Read_Buffer_Continue()
{
    if (Buffer_Size<600)
        return;

    if (CC8(Buffer)!=CC8("DVDVIDEO"))
    {
        File_Offset=File_Size;
        return;
    }

    //If menu only
    if (CC4(Buffer+8)==CC4("-VMG"))
    {
        Stream_Prepare(Stream_General);
        General[0](_T("Format"))=_T("DVD Video");
        General[0](_T("Format/String"))=_T("DVD Video (Menu)");
        General[0](_T("Format/Extensions"))=_T("IFO");
        File_Offset=File_Size;
        return;
    }

    if (CC4(Buffer+8)!=CC4("-VTS"))
    {
        File_Offset=File_Size;
        return;
    }

    //Video
    int16u U0=BigEndian2int16u((char*)Buffer+0x200);
    size_t Standard=(U0>>12)&0x03;
    if (Standard>=2)
    {
        File_Offset=File_Size;
        return;
    }
    size_t CodecV=(U0>>14)&0x03;
    size_t AspectRatio=(U0>>10)&0x03;
    size_t BitRate_Mode=(U0>>5)&0x01;
    size_t Width_Height_Fps=(U0>>3)&0x03;

    Stream_Prepare(Stream_General);
    General[0](_T("Format"))=_T("DVD Video");
    General[0](_T("Format/String"))=_T("DVD Video");
    General[0](_T("Format/Extensions"))=_T("IFO");

    Stream_Prepare(Stream_Video);
    Video[0](_T("Codec"))=IFO_CodecV[CodecV];
    Video[0](_T("BitRate_Mode"))=IFO_BitRate_Mode[BitRate_Mode];
    Video[0](_T("AspectRatio"))=IFO_AspectRatio[AspectRatio];
    Video[0](_T("Width")).From_Number(IFO_Width[Width_Height_Fps]);
    Video[0](_T("Height")).From_Number(IFO_Height[Standard][Width_Height_Fps]);
    Video[0](_T("FrameRate")).From_Number(IFO_FrameRate[Width_Height_Fps]);

    //Audio
    size_t Audio_Count=BigEndian2int16u((char*)Buffer+0x202);
    for (size_t PosA=0; PosA<Audio_Count; PosA++)
    {
        int64u U2=BigEndian2int64u((char*)Buffer+0x204+8*PosA);
        size_t CodecA=(U2>>61)&0x07;
        size_t Resolution=(U2>>54)&0x03;
        size_t Channels=(U2>>48)&0x07;
        size_t SamplingRate=(U2>>51)&0x07;
        char Language[4];
        Language[0]=*(Buffer+0x204+8*PosA+2);
        Language[1]=*(Buffer+0x204+8*PosA+3);
        Language[2]=*(Buffer+0x204+8*PosA+4);
        Language[3]=0;
        size_t Language_More=(U2>>16)&0x07;

        size_t Pos=Stream_Prepare(Stream_Audio);
        Audio[Pos](_T("Codec"))=IFO_CodecA[CodecA];
        Audio[Pos](_T("SamplingRate")).From_Number(IFO_SamplingRate[SamplingRate]);
        if (Channels<4)
            Audio[Pos](_T("Channel(s)")).From_Number(IFO_Channels[Channels]);
        else if (Channels==4)
            Audio[Pos](_T("Channel(s)"))=_T("6");
        else if (Channels==5)
            Audio[Pos](_T("Channel(s)"))=_T("7");
        if (Resolution!=3)
            Audio[Pos](_T("Resolution")).From_Number(IFO_Resolution[Resolution]);
        else
            Audio[Pos](_T("Resolution"))=_T("DRC");
        Audio[Pos](_T("Language")).From_Local(Language);
        Audio[Pos](_T("Language_More"))=IFO_Language_More[Language_More];
    }

    //Text
    size_t Text_Count=BigEndian2int16u((char*)Buffer+0x254);
    if (0x256+6*Text_Count>Buffer_Size)
        return;
    for (size_t PosT=0; PosT<Text_Count; PosT++)
    {
        int64u U2=BigEndian2int64u((char*)Buffer+0x256+6*PosT);
        if (!U2)
            continue;
        char Language[4];
        Language[0]=*(Buffer+0x256+6*PosT+2);
        Language[1]=*(Buffer+0x256+6*PosT+3);
        Language[2]=*(Buffer+0x256+6*PosT+4);
        Language[3]=0;
        size_t Language_More=(U2>>16)&0x07;

        size_t Pos=Stream_Prepare(Stream_Text);
        Text[Pos](_T("Codec"))=_T("2-bit RLE");
        Text[Pos](_T("Language")).From_Local(Language);
        Text[Pos](_T("Language_More"))=IFO_Language_More[Language_More];
    }

    //No need of more
    File_Offset=File_Size;
}

//***************************************************************************
// Information
//***************************************************************************

//---------------------------------------------------------------------------
void File_Dvdv::HowTo(stream_t StreamKind)
{
    switch (StreamKind)
    {
        case (Stream_General) :
            Fill_HowTo("Format", "R");
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
            Fill_HowTo("Resolution", "R");
            Fill_HowTo("Language", "R");
            Fill_HowTo("Language_More", "R");
            break;
            Fill_HowTo("Codec", "R");
            Fill_HowTo("Language", "R");
            Fill_HowTo("Language_More", "R");
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

#endif //MEDIAINFO_DVDV_YES

