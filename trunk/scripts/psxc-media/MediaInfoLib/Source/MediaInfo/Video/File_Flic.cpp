// File_Flic - Info for Flic files
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
#if defined(MEDIAINFO_FLV_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Video/File_Flic.h"
#include <ZenLib/Utils.h>
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//From : http://www.compuphase.com/flic.htm
typedef struct {
  int32u Size;          //Size of FLIC including this header
  int16u Type;          //File type
  int16u Frames;        //Number of frames in first segment
  int16u Width;         //FLIC width in pixels
  int16u Height;        //FLIC height in pixels
  int16u Depth;         //Bits per pixel (usually 8)
  int16u Flags;         //Set to zero or to three
  int32u Speed;         //Delay between frames
  int16u Reserved1;     //Set to zero
  int32u Created;       //Date of FLIC creation (FLC only)
  int32u Creator;       //Serial number or compiler id (FLC only) (0x45474900 for EGI)
  int32u Updated;       //Date of FLIC update (FLC only)
  int32u Updater;       //Serial number (FLC only), see creator
  int16u AspectX;       //Width of square rectangle (FLC only)
  int16u AspectY;       //Height of square rectangle (FLC only)
  int16u ExtFlags;      //EGI: flags for specific EGI extensions
  int16u KeyFrames;     //EGI: key-image frequency
  int16u TotalFrames;   //EGI: total number of frames (segments)
  int32u ReqMemory;     //EGI: maximum chunk size (uncompressed)
  int16u MaxRegions;    //EGI: max. number of regions in a CHK_REGION chunk
  int16u TranspNum;     //EGI: number of transparent levels
  int8u  Reserved2[24]; //Set to zero
  int32u OFrame1;       //Offset to frame 1 (FLC only)
  int32u OFrame2;       //Offset to frame 2 (FLC only)
  int8u  Reserved3[40]; //Set to zero
} FLIC_HEADER;

//***************************************************************************
// Format
//***************************************************************************

void File_Flic::Read_Buffer_Continue()
{
    if (Buffer_Size<32)
        return;

    if (Buffer[4]!=0xAF || (Buffer[5]!=0x11 || Buffer[5]!=0x12 || Buffer[5]!=0x44 || Buffer[5]!=0x30 || Buffer[5]!=0x31))
    {
        File_Offset=File_Size;
        return;
    }

    Stream_Prepare(Stream_General);
    General[0](_T("Format"))=_T("FLIC");

    FLIC_HEADER* Flic=(FLIC_HEADER*)Buffer;

    Stream_Prepare(Stream_Video);
    bool FLC=false;
    if (Flic->Type==0xAF11)
        Video[0](_T("Codec"))=_T("FLI");
    else
    {
        Video[0](_T("Codec"))=_T("FLC");
        FLC=true;
    }
    Video[0](_T("FrameCount")).From_Number(Flic->Frames);
    if (FLC)
    {
        Video[0](_T("FrameRate")).From_Number(1/Flic->Speed); //ms per frame
        Video[0](_T("PlayTime")).From_Number(Flic->Frames*Flic->Speed);
    }
    else
    {
        Video[0](_T("FrameRate")).From_Number(Flic->Speed*1000/70); //multiple of 1/70 per frame
        Video[0](_T("PlayTime")).From_Number(Flic->Frames*Flic->Speed*1000/70);
    }
    Video[0](_T("Width")).From_Number(Flic->Width);
    Video[0](_T("Height")).From_Number(Flic->Height);
    Video[0](_T("Resolution")).From_Number(Flic->Depth);
    if (FLC)
        Video[0](_T("AspectRatio")).From_Number(Flic->AspectX/Flic->AspectY);

    //No need of more
    File_Offset=File_Size;
}

//***************************************************************************
// Information
//***************************************************************************

//---------------------------------------------------------------------------
void File_Flic::HowTo(stream_t StreamKind)
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

#endif //MEDIAINFO_FLIC_*

