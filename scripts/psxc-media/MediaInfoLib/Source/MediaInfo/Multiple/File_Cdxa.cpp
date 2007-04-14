// File_Cdxa - Info for CDXA files
// Copyright (C) 2004-2007 Jerome Martinez, Zen@MediaArea.net
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
#if defined(MEDIAINFO_CDXA_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Multiple/File_Cdxa.h"
#include "ZenLib/Utils.h"
#include "MediaInfo/MediaInfo.h"
#include <wx/file.h>
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Format
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// CDXA = RIFF header + Raw sectors
// Riff header size = 44
// Raw sector size = 2352
//
// Raw sector :
// Sync             12 bytes (00 FF .. FF 00)
// Header           4 bytes
// SubHeader        8 bytes
// Datas            2324 bytes
// EDC (CRC)        4 bytes
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
File_Cdxa::File_Cdxa()
{
    MI=NULL;
}

//---------------------------------------------------------------------------
File_Cdxa::~File_Cdxa()
{
    delete MI; MI=NULL;
}

//***************************************************************************
// Format
//***************************************************************************

//---------------------------------------------------------------------------
void File_Cdxa::Read_Buffer_Init ()
{
    delete MI; MI=NULL;
    Element_Size=0;
    Trusted=100;
}

//---------------------------------------------------------------------------
void File_Cdxa::Read_Buffer_Continue ()
{
    //Can not be included in another container
    if (File_Offset!=0 && MI==NULL)
    {
        File_Offset=File_Size;
        return;
    }

    //Coherancy
    if (File_Offset==0)
    {
        //Integrity
        if (Buffer_Size<44)
            return;

        //Looking for header
        while (Buffer_Offset<Buffer_Size-44 && !IsHeader())
            Buffer_Offset++;
        if (Buffer_Offset>=Buffer_Size-44)
        {
            File_Offset=File_Size;
            return;
        }

        Header();
    }

    //Parsing
    while (Buffer_Parse());
}

//---------------------------------------------------------------------------
void File_Cdxa::Read_Buffer_Finalize ()
{
    //If nothing
    if (MI->Info==NULL)
    {
        General[0](_T("Format"))=Ztring(_T("CDXA"));
        General[0](_T("Format/String"))=Ztring(_T("CD Mode 2"));
        return;
    }

    //General
    MI->Open_Buffer_Finalize();
    Merge(*(MI->Info));
    Merge(*(MI->Info), Stream_General, 0, 0);
    General[0](_T("Format"))=Ztring(_T("CDXA/"))+General[0](_T("Format"));
    General[0](_T("Format/String"))=Ztring(_T("CD Mode 2 / "))+General[0](_T("Format/String"));
    Fill(Stream_General, 0, "PlayTime", "");
    Fill(Stream_Video, 0, "PlayTime", "");
}

//***************************************************************************
// Buffer
//***************************************************************************

//---------------------------------------------------------------------------
bool File_Cdxa::Buffer_Parse()
{
    //Synchronized?
    if (!Synchronized_Verify())
        return false;

    //Element next
    Element_Next=Buffer_Offset+2352;

    //Element size
    if (!Element_Size_Get())
        return false;

    //Parsing
    if (!Frame_Parse())
        return false;

    //If no need of more
    if (File_Offset+Buffer_Offset>1024*1024)
        return false;

    Buffer_Offset=Element_Next;
    return true;
}

//---------------------------------------------------------------------------
// Chunk, 2352 bytes
// Sync                                 12 bytes
// Header                               4 bytes
// SubHeader                            8 bytes
// Data                                 2324 bytes
// CRC                                  4 bytes
//
bool File_Cdxa::Frame_Parse()
{
    //Enough data?
    if (Buffer_Offset+24>Buffer_Size)
        return false;

    //Parsing
    Element_Parse();

    return true;
}

//---------------------------------------------------------------------------
bool File_Cdxa::Element_Parse()
{
    //Details
    if (Config.Details_Get())
    {
        Details_Add_Element(1, "Chunk");
    }

    //Positioning
    Buffer_Offset+=24;//Sync+Header+SubHeader

    //{wxFile F; F.Open((Ztring(_T("d:\\direct"))+Ztring::ToZtring((size_t)this, 16)).c_str(), wxFile::write_append); F.Write(Buffer+Buffer_Offset, Size);}
    MI->Open_Buffer_Init(File_Size, File_Offset+Buffer_Offset);
    MI->Open_Buffer_Continue(Buffer+Buffer_Offset, Element_Size);

    //Details
    if (Config.Details_Get())
    {
        if (!MI->Inform().empty())
        {
            Details_Add_Element(0, "CDXA", Element_Size);
            Details_Add_Element("");
            Details+=_T("\r\n");
            Details+=MI->Inform();
        }
    }

    return true;
}

//***************************************************************************
// Elements
//***************************************************************************

//---------------------------------------------------------------------------
void File_Cdxa::Header()
{
    //Details
    if (Config.Details_Get())
    {
        Details_Add_Element(1, "CDXA header"); \
    }

    //Skip RIFF Header
    Buffer_Offset+=44;

    if (MI==NULL)
    {
        MI=new MediaInfo;
        Stream_Prepare(Stream_General);
    }
}

//***************************************************************************
// Helpers
//***************************************************************************

//---------------------------------------------------------------------------
bool File_Cdxa::Synchronized_Verify()
{
    //Synched?
    if (Buffer_Offset+2352*2+12>Buffer_Size && !(
        CC8(Buffer+Buffer_Offset+2352*0)==0x00FFFFFFFFFFFFFFFFLL && CC4(Buffer+Buffer_Offset+2352*0+8)==0xFFFFFF00
     && CC8(Buffer+Buffer_Offset+2352*1)==0x00FFFFFFFFFFFFFFFFLL && CC4(Buffer+Buffer_Offset+2352*1+8)==0xFFFFFF00
     && CC8(Buffer+Buffer_Offset+2352*2)==0x00FFFFFFFFFFFFFFFFLL && CC4(Buffer+Buffer_Offset+2352*2+8)==0xFFFFFF00
     ))
    {
        //Filling
        TRUSTED_ISNOT("Synchro error");
        if (File_Offset==File_Size)
            return false;

        //Resynch
        Synchronize();
        if (!Synched)
            return false;
    }

    return true;
}

//---------------------------------------------------------------------------
bool File_Cdxa::Element_Size_Get()
{
    //End of buffer management
    if (Buffer_Offset+2352>Buffer_Size)
    {
        if (File_Offset+Buffer_Size>=File_Size)
        {
            //End of file
            if (Buffer_Offset+24>Buffer_Size)
                Element_Size=Buffer_Size-(Buffer_Offset+24);
            else
                return false; //Not enouth place
        }
        else
            return false; //Wait for data
    }
    else
        Element_Size=2352-24-4;

    return true;
}

//---------------------------------------------------------------------------
bool File_Cdxa::Synchronize()
{
    //Look for first Sync word
    while (Buffer_Offset+12<=Buffer_Size && !(CC8(Buffer+Buffer_Offset)==0x00FFFFFFFFFFFFFFFFLL && CC4(Buffer+Buffer_Offset+8)==0xFFFFFF00))
        Buffer_Offset++;

    //Must wait more data?
    if (Buffer_Offset+12>Buffer_Size)
    {
        Synched=false;
        return false;
    }

    //OK, we continue
    Synched=true;
    return true;
}

//---------------------------------------------------------------------------
bool File_Cdxa::Detect_EOF_Needed()
{
    //Jump to the end of the file
    if (File_Size!=(int64u)-1 && File_Size>File_Offset+Buffer_Size+1*1024*1024 && (
       (File_Offset>1*1024*1024)
    || (MI!=NULL && MI->Open_Buffer_Continue_GoTo_Get()>0)
    ))
    {
        Details_Add_Error("----------------------------------------");
        Details_Add_Error("---   CDXA, Jumping to end of file   ---");
        Details_Add_Error("----------------------------------------");
        File_GoTo=File_Size-1*1024*1024;
        return true;
    }

    return false;
}

//---------------------------------------------------------------------------
// RIFF Header, 44 bytes
// RIFF header                      4 bytes, Pos=0
// RIFF data size                   4 bytes, Pos=4
// Format (CDXA)                    4 bytes, Pos=8
// Format Header                    4 bytes, Pos=12
// Format Size                      4 bytes, Pos=16
// Format user_id                   2 bytes, Pos=20
// Format group_id                  2 bytes, Pos=22
// Format attributes                2 bytes, Pos=24
// Format xa_signature              2 bytes, Pos=26 ("XA")
// Format xa_track_number           4 bytes, Pos=28
// Format Reserved                  4 bytes, Pos=32
// Data Header                      4 bytes, Pos=36
// Data Size                        4 bytes, Pos=40
//
// Attributes (big endian):
// 15 Directory
// 14 CDDA
// 13 Interleaved
// 12 Mode2Form2 --> 2324 bytes/block
// 11 Mode2Form1 --> 2048 bytes/block
// 10 Exec_Other
// 09 Reserved
// 08 Read_Other
// 07 Reserved
// 06 Exec_Group
// 05 Reserved
// 04 Read_Group
// 03 Reserved
// 02 Exec_User
// 01 Reserved
// 00 Read_User
//
bool File_Cdxa::IsHeader()
{
    if (                           Buffer_Offset<Buffer_Size-0x2C &&
                        CC4(Buffer+Buffer_Offset+0x00)==CC4("RIFF")
     && LittleEndian2int32u(Buffer+Buffer_Offset+0x04)==LittleEndian2int32u(Buffer+Buffer_Offset+0x28)+0x24 //Sizes of chunks
     &&                 CC4(Buffer+Buffer_Offset+0x08)==CC4("CDXA")
     &&                 CC4(Buffer+Buffer_Offset+0x0C)==CC4("fmt ")
     && LittleEndian2int32u(Buffer+Buffer_Offset+0x10)==0x10
     &&                 CC4(Buffer+Buffer_Offset+0x24)==CC4("data")
       )
        return true;
    else
        return false;
}

//***************************************************************************
// Information
//***************************************************************************

//---------------------------------------------------------------------------
void File_Cdxa::HowTo(stream_t StreamKind)
{
    switch (StreamKind)
    {
        case (Stream_General) :
            Fill_HowTo("Format", "R");
            break;
        case (Stream_Video) :
            Fill_HowTo("Codec", "R");
            Fill_HowTo("BitRate", "R|Overall BitRate - Audio BitRates");
            Fill_HowTo("Width", "R");
            Fill_HowTo("Height", "R");
            Fill_HowTo("AspectRatio", "R");
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

#endif //MEDIAINFO_CDXA_YES

