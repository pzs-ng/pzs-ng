// File_Jpeg - Info for NewFormat files
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
// Links
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// http://www.fileformat.info/format/jpeg/
// http://park2.wakwak.com/~tsuruzoh/Computer/Digicams/exif-e.html
// http://www.w3.org/Graphics/JPEG/jfif3.pdf
// http://www.sentex.net/~mwandel/jhead/
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
// Compilation condition
#include <MediaInfo/Setup.h>
#if defined(MEDIAINFO_JPEG_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Image/File_Jpeg.h"
#include "ZenLib/Utils.h"
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//---------------------------------------------------------------------------
// JPEG Format
// SOI                              2 bytes, Pos=0 (0xFFD8)
// APP Marker                       2 bytes, Pos=2
//
//---------------------------------------------------------------------------

//***************************************************************************
// Format
//***************************************************************************

//---------------------------------------------------------------------------
void File_Jpeg::Read_Buffer_Continue()
{
    if (File_Offset==0)
    {
        //Integrity
        if (Buffer_Size<2)
            return;

        //Header
        if (CC2(Buffer)!=0xFFD8)
        {
            File_Offset=File_Size;
            return;
        }
        Buffer_Offset+=2;

        //Details
        if (Config.Details_Get())
        {
            Details_Add_Element(1, "JPEG header");
        }
    }

    //Parse
    while (Buffer_Parse());
}

//***************************************************************************
// Buffer
//***************************************************************************

//---------------------------------------------------------------------------
// An Element
// Size                             2 bytes, Pos=4
// Code                             5 bytes, Pos=6
//
bool File_Jpeg::Buffer_Parse()
{
    //Enough data?
    if (Buffer_Offset+4>Buffer_Size)
        return false;

    //Element size
    Element_Size=BigEndian2int16u(Buffer+Buffer_Offset+2);
    if (Element_Size<2) //At least the size of Size
    {
        File_Offset=File_Size;
        return false;
    }
    if (Element_Size+4>Buffer_Size) //Enough buffer?
        return false;

    //Element name
    Element_Name=BigEndian2int16u(Buffer+Buffer_Offset);

    //Parse
    Element_Parse();

    //Next element
    if (File_Offset==File_Size)
        return false; //No need of more
    Buffer_Offset+=Element_Size;
    return true;
}

//---------------------------------------------------------------------------
// Element parse
//
bool File_Jpeg::Element_Parse()
{
    //Details
    if (Config.Details_Get())
    {
        Ztring Pos1; Pos1.From_Number(Element_Name, 16);
        Ztring Pos2;
        Pos2.resize(4-Pos1.size(), _T('0'));
        Pos2+=Pos1;
        Pos2.MakeUpperCase();
        Details_Add_Element(1, Pos2);
    }

    //Offset
    Buffer_Offset+=4; //Marker + Size
    Element_Size-=2; //Size

    //Parsing
    switch (Element_Name)
    {
        case 0xFFD9 : End(); break;
        case 0xFFDB : JPEG(); break;
        case 0xFFE0 : APP0(); break;
        case 0xFFE1 : APP1(); break;
        default: ;
    }

	return true;
}

//***************************************************************************
// Elements
//***************************************************************************

#define INTEGRITY(ELEMENT_SIZE) \
    if (Element_Size<ELEMENT_SIZE) \
    { \
        if (Config.Details_Get()) \
        { \
            Details_Add_Info(Error, "Integrity error", ELEMENT_SIZE); \
        } \
       return; \
    } \

//---------------------------------------------------------------------------
void File_Jpeg::APP0()
{
    INTEGRITY(5);

    if (CC5(Buffer+Buffer_Offset)==CC5("JFIF\0"))
    {
        Buffer_Offset+=5;
        Element_Size-=5;
        APP0_JFIF();
    }
    if (CC5(Buffer+Buffer_Offset)==CC5("JFFF\0"))
    {
        Buffer_Offset+=5;
        Element_Size-=5;
        APP0_JFXX();
    }
}

//---------------------------------------------------------------------------
// JFIF, Header, 9+ bytes
// Version                          2 bytes, Pos=0
// Unit                             1 byte,  Pos=2  (0=Pixels, 1=dpi, 2=dpcm)
// Xdensity                         2 bytes, Pos=3
// Ydensity                         2 bytes, Pos=5
// Xthumbail                        1 byte,  Pos=7
// Ythumbail                        1 byte,  Pos=8
// RGB thumbail                     3*Xthumbail*Ythumbail
//
void File_Jpeg::APP0_JFIF()
{
    INTEGRITY(9);
    Fill_General();

    //Parsing
    int8u  Unit=BigEndian2int8u(Buffer+Buffer_Offset+2);
    int16u Width=BigEndian2int16u(Buffer+Buffer_Offset+3);
    int16u Height=BigEndian2int16u(Buffer+Buffer_Offset+5);

    //Filling
    switch (Unit)
    {
        case 0 : //Pixels
            Fill("Width", Width);
            Fill("Height", Height);
            break;
        default: ;
    }
}

//---------------------------------------------------------------------------
// JFXX, Extension
// extension_code                   1 byte,  Pos=0
//                                           0x10 Thumbnail coded using JPEG
//                                           0x11 Thumbnail stored using 1 byte/pixel
//                                           0x13 Thumbnail stored using 3 bytes/pixel
// extension_data                   X bytes, Pos=1
//
void File_Jpeg::APP0_JFXX()
{
    INTEGRITY(1);
}

//---------------------------------------------------------------------------
// Thumbail JPEG
// Normal JPEG without JFIF
//
void File_Jpeg::APP0_JFXX_JPEG()
{
}

//---------------------------------------------------------------------------
// Thumbail 1 byte per pixel
// Xthumbnail                       1 byte, Pos=0 (Thumbnail horizontal pixel count)
// Ythumbnail                       1 byte, Pos=1 (Thumbnail vertical pixel count)
// palette                          768 bytes, Pos=2 (24-bit RGB pixel values for the color palette.)
// Thumbail                         Xthumbnail*Ythumbnail bytes, Pos=770 (8-bit values for the thumbnail pixels)
//
void File_Jpeg::APP0_JFXX_1B()
{
}

//---------------------------------------------------------------------------
// Thumbail 3 bytes per pixel
// Xthumbnail                       1 byte, Pos=0 (Thumbnail horizontal pixel count)
// Ythumbnail                       1 byte, Pos=1 (Thumbnail vertical pixel count)
// Thumbail                         3*Xthumbnail*Ythumbnail bytes, Pos=2 (24-bit values for the thumbnail pixels)
//
void File_Jpeg::APP0_JFXX_3B()
{
}

//---------------------------------------------------------------------------
void File_Jpeg::APP1()
{
    INTEGRITY(6)

    if (CC6(Buffer+Buffer_Offset)==CC6("Exif\0\0"))
    {
        Buffer_Offset+=6;
        Element_Size-=6;
        APP1_EXIF();
    }
}

//---------------------------------------------------------------------------
// Exif format
// Alignment                        4 bytes, Pos=0 (0x49492A00 --> LE, 0x4D4D2A00 --> BE)
// First IFD                        4 bytes
//
void File_Jpeg::APP1_EXIF()
{
    INTEGRITY(8)
    Fill_General();

    //Alignement
    int32u Aligment=BigEndian2int32u(Buffer+Buffer_Offset);
    if (Aligment!=0x49492A00 && Aligment!=0x4D4D2A00)
        return;

    //Parsing
    int32u First_IFD;
    if (Aligment==0x49492A00)
        First_IFD=LittleEndian2int32u(Buffer+Buffer_Offset+4);
    else
        First_IFD=BigEndian2int32u(Buffer+Buffer_Offset+4);
}

//---------------------------------------------------------------------------
void File_Jpeg::End()
{

}

//---------------------------------------------------------------------------
void File_Jpeg::JPEG()
{
    //No need of more
    File_Offset=File_Size;
}

//***************************************************************************
// Helpers
//***************************************************************************

//---------------------------------------------------------------------------
void File_Jpeg::Fill_General()
{
    if (Count_Get(Stream_General)>0)
        return; //Already done

    //Filling
    Stream_Prepare(Stream_General);
    Fill("Format", "JPEG");

    Stream_Prepare(Stream_Image);
    Fill("Codec", "JPEG");
    Fill("Codec/String", "JPEG"); //To Avoid automatic filling
}

//***************************************************************************
// Information
//***************************************************************************

//---------------------------------------------------------------------------
void File_Jpeg::HowTo(stream_t StreamKind)
{
         if (StreamKind==Stream_General)
    {
        General[0](_T("Format"), Info_HowTo)=_T("R");
    }
    else if (StreamKind==Stream_Image)
    {
        Video[0](_T("Codec"), Info_HowTo)=_T("Raw");
    }

    return;
}

} //NameSpace

#endif
