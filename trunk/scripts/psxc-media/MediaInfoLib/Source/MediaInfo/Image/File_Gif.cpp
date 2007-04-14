// File_Gif - Info for GIF files
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

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// GIF - Format
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// From http://www.onicos.com/staff/iz/formats/gif.html
//
// Header                               3 bytes - "GIF"
// Version                              3 bytes - "87a" or "89a"
// Logical Screen Width                 2 bytes
// Logical Screen Height                2 bytes
// Global Color Table Flag (GCTF)       1 bit
// Color Resolution                     3 bits
// Sort Flag to Global Color Table      1 bit
// Size of Global Color Table:          3 bits - 2^(1+n)
// Background Color Index               1 byte
// Pixel Aspect Ratio                   1 byte
// Global Color Table                   256*3 bytes (only if GCTF)
// Blocks                               ...
// Trailer                              1 byte - 0x3B
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
// Compilation condition
#include <MediaInfo/Setup.h>
#if defined(MEDIAINFO_GIF_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Image/File_Gif.h"
#include "ZenLib/Utils.h"
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Format
//***************************************************************************

//---------------------------------------------------------------------------
void File_Gif::Read_Buffer_Continue()
{
    //Integrity
    if (Buffer_Size<10)
        return;

    //Header
    if (CC3(Buffer)!=CC3("GIF"))
    {
        File_Offset=File_Size;
        return;
    }

    //Filling
    Stream_Prepare(Stream_General);
    Fill("Format", "GIF");

    Stream_Prepare(Stream_Image);
    Fill("Width", LittleEndian2int16u(Buffer+6));
    Fill("Height", LittleEndian2int16u(Buffer+8));
    Fill("Codec", Buffer, 6);

    //No need of more
    File_Offset=File_Size;
}

void File_Gif::HowTo(stream_t StreamKind)
{
         if (StreamKind==Stream_General)
    {
        General[0](_T("Format"), Info_HowTo)=_T("R");
    }
    else if (StreamKind==Stream_Image)
    {
        Video[0](_T("Width"), Info_HowTo)=_T("R");
        Video[0](_T("Height"), Info_HowTo)=_T("R");
    }

    return;
}

} //NameSpace

#endif
