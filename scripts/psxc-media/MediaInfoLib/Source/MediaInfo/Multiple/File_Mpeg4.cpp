// File_Mpeg4 - Info for MPEG-4 files
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
//
// Information about MPEG-4 files
// Main part
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
// Compilation condition
#include <MediaInfo/Setup.h>
#if defined(MEDIAINFO_MPEG4_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Multiple/File_Mpeg4.h"
#if defined(MEDIAINFO_MPEGPS_YES)
    #include "MediaInfo/Multiple/File_MpegPs.h"
#endif
#include <ZenLib/Utils.h>
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Format
//***************************************************************************

//---------------------------------------------------------------------------
void File_Mpeg4::Read_Buffer_Init()
{
    //Buffer
    Element_Level=0;
    Element_Name[0]=0;
    Element_Next[0]=File_Size;

    //Temp
    TimeScale=0;
    mdat_MustParse=false;
    mdat_Info=NULL;
}

//---------------------------------------------------------------------------
void File_Mpeg4::Read_Buffer_Continue()
{
    //mdat parsing
    if (mdat_MustParse)
        mdat_Parse();

    //Normal parsing
    while (Buffer_Parse());
}

//***************************************************************************
// Buffer
//***************************************************************************

//---------------------------------------------------------------------------
//
bool File_Mpeg4::Buffer_Parse()
{
    //Element Level
    if (!Element_Level_Get())
        return false;

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

    //Exception, mdat atom must be parsed
    if (mdat_MustParse)
        return false;

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
// MDAT Parsing
//***************************************************************************

//---------------------------------------------------------------------------
// mdat parsing, for MPEG-PS, must be improved
//
void File_Mpeg4::mdat_Parse()
{
    #if defined(MEDIAINFO_MPEGPS_YES)
        if (mdat_Info==NULL)
            mdat_Info=new File_MpegPs();

        //Calculating buffer size to parse
        size_t ToParse_Size;
        if (Buffer_Offset+Buffer_Size<Element_Next[1])
            ToParse_Size=Buffer_Size-Buffer_Offset;
        else
        {
            ToParse_Size=Element_Next[1]-Buffer_Offset;
            mdat_MustParse=false; //We no more need
        }

        Open_Buffer_Init(mdat_Info, Element_Next[1], File_Offset+Buffer_Offset);
        Open_Buffer_Continue(mdat_Info, Buffer+Buffer_Offset, ToParse_Size);
        Buffer_Offset+=ToParse_Size;
        if (mdat_Info->File_GoTo>0 && mdat_Info->File_Size-mdat_Info->File_GoTo>0)
        {
            //Details
            if (Config.Details_Get())
            {
                Details_Add_Error("------------------------------------------");
                Details_Add_Error("---   MPEG-4, Jumping to end of file   ---");
                Details_Add_Error("------------------------------------------");
            }

            //Jumping
            File_GoTo=Element_Next[1]-(mdat_Info->File_Size-mdat_Info->File_GoTo);
            return;
        }

        if (!mdat_MustParse || File_Offset+Buffer_Offset==Element_Next[1])
        {
            Open_Buffer_Finalize(mdat_Info);
            Merge(*mdat_Info);
        }
    #endif
}

//***************************************************************************
// Helpers
//***************************************************************************

//---------------------------------------------------------------------------
bool File_Mpeg4::Element_Level_Get()
{
    //Integrity
    if (Element_Level>10)
    {
        File_Offset=File_Size;
        return false;
    }

    //End of this level?
    while (File_Offset+Buffer_Offset>=Element_Next[Element_Level])
    {
        if (Element_Level==0)
        {
            File_Offset=File_Size;
            return false;
        }
        Element_Level--; //To next element
    }

    return true;
}

//---------------------------------------------------------------------------
bool File_Mpeg4::Element_Name_Get()
{
    //Enough data?
    if (Buffer_Offset+8>Buffer_Size)
        return false;

    //Element name
    Element_Name[Element_Level]=CC4(Buffer+Buffer_Offset+4);

    return true;
}

//---------------------------------------------------------------------------
bool File_Mpeg4::Element_Size_Get()
{
    //Enough data?
    if (Buffer_Offset+8>Buffer_Size)
        return false;

    //Element size
    Element_HeaderSize=8;
    Element_Size=BigEndian2int32u(Buffer+Buffer_Offset);
    if (Element_Size<8)
    {
        //Special case: until the end of the file
             if (Element_Size==0 && File_Size!=(int64u)-1)
        {
            Element_Size=File_Size-(File_Offset+Buffer_Offset);
        }
        //Special case: Big files, size is 64-bit
        else if (Element_Size==1)
        {
            //Enough data?
            if (Buffer_Offset+16>Buffer_Size)
                return false;

            //Reading Extended size
            Element_Size=BigEndian2int64u(Buffer+Buffer_Offset+8);
            Element_HeaderSize=16;
        }
    }

    //Removing header size
    if (Element_Size>=Element_HeaderSize)
    {
        Element_Size-=Element_HeaderSize;
    }
    else
    {
        //Corrupted
        TRUSTED_ISNOT("Not enough place to have an atom");
        if (Element_Level>0)
            Element_Size=Element_Next[Element_Level-1]-(File_Offset+Buffer_Offset); //Maximum limit
        else
        {
            //Impossible to know where we are
            File_Offset=File_Size;
            return false;
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

    return true;
}

//---------------------------------------------------------------------------
//Get language string from 2CC
char* File_Mpeg4::Language_Get(int x)
{
    if (x<0x100 || x==0x7FFF)
    {
        //Old Quicktime method, I never saw it, skip it
        Language_Result[0] = '\0';
        Language_Result[1] = '\0';
        Language_Result[2] = '\0';
    }
    else
    {
        Language_Result[3] = '\0';
        Language_Result[2] = (x>> 0&0x1F)+0x60;
        Language_Result[1] = (x>> 5&0x1F)+0x60;
        Language_Result[0] = (x>>10&0x1F)+0x60;
    }
    return Language_Result;
}

//---------------------------------------------------------------------------
//Get Metadata definition from 4CC
File_Mpeg4::method File_Mpeg4::Metadata_Get(std::string &Parameter, int32u Meta)
{
    switch (Meta)
    {
        case Mpeg4::moov_meta___ART : Parameter="Performer"; return Method_String;
        case Mpeg4::moov_meta___alb : Parameter="Album"; return Method_String;
        case Mpeg4::moov_meta___aut : Parameter="Performer"; return Method_String;
        case Mpeg4::moov_meta___cmt : Parameter="Comment"; return Method_String;
        case Mpeg4::moov_meta___cpy : Parameter="Copyright"; return Method_String;
        case Mpeg4::moov_meta___day : Parameter="Encoded_Date"; return Method_String;
        case Mpeg4::moov_meta___des : Parameter="Title/More"; return Method_String;
        case Mpeg4::moov_meta___dir : Parameter="Director"; return Method_String;
        case Mpeg4::moov_meta___dis : Parameter="TermsOfUse"; return Method_String;
        case Mpeg4::moov_meta___edl : Parameter="Tagged_Date"; return Method_String;
        case Mpeg4::moov_meta___fmt : Parameter="Origin"; return Method_String;
        case Mpeg4::moov_meta___gen : Parameter="Genre"; return Method_String;
        case Mpeg4::moov_meta___hos : Parameter="HostComputer"; return Method_String;
        case Mpeg4::moov_meta___inf : Parameter="Title/More"; return Method_String;
        case Mpeg4::moov_meta___key : Parameter="Keywords"; return Method_String;
        case Mpeg4::moov_meta___mak : Parameter="Make"; return Method_String;
        case Mpeg4::moov_meta___mod : Parameter="Model"; return Method_String;
        case Mpeg4::moov_meta___nam : Parameter="Title"; return Method_String;
        case Mpeg4::moov_meta___prd : Parameter="Producer"; return Method_String;
        case Mpeg4::moov_meta___PRD : Parameter="Product"; return Method_String;
        case Mpeg4::moov_meta___prf : Parameter="Performer"; return Method_String;
        case Mpeg4::moov_meta___req : Parameter="SpecialPlaybackRequirements"; return Method_String;
        case Mpeg4::moov_meta___src : Parameter="DistribtedBy"; return Method_String;
        case Mpeg4::moov_meta___swr : Parameter="Encoded_application"; return Method_String;
        case Mpeg4::moov_meta___too : Parameter="Encoded_application"; return Method_String;
        case Mpeg4::moov_meta___wrn : Parameter="Warning"; return Method_String;
        case Mpeg4::moov_meta___wrt : Parameter="ScreenplayBy"; return Method_String;
        case Mpeg4::moov_meta__cpil : Parameter.clear(); return Method_None;
        case Mpeg4::moov_meta__disk : Parameter="Part"; return Method_Binary;
        case Mpeg4::moov_meta__name : Parameter="Title"; return Method_String;
        case Mpeg4::moov_meta__trkn : Parameter="Track"; return Method_Binary;
        case Mpeg4::moov_meta__tmpo : Parameter.clear(); return Method_None;
        default :
            {
                Parameter.clear();
                Parameter.append(1, (char)((Meta&0xFF000000)>>24));
                Parameter.append(1, (char)((Meta&0x00FF0000)>>16));
                Parameter.append(1, (char)((Meta&0x0000FF00)>> 8));
                Parameter.append(1, (char)((Meta&0x000000FF)>> 0));
                return Method_String;
            }
    }
}

//---------------------------------------------------------------------------
//Get Metadata definition from string
File_Mpeg4::method File_Mpeg4::Metadata_Get(std::string &Parameter, const std::string &Meta)
{
         if (Meta=="com.apple.quicktime.copyright") Parameter="Copyright";
    else if (Meta=="com.apple.quicktime.displayname") Parameter="Title";
    else if (Meta=="iTunNORM") Parameter="";
    else if (Meta=="iTunes_CDDB_IDs") Parameter="";
    else Parameter=Meta;
    return Method_String;
}

//***************************************************************************
// Information
//***************************************************************************

//---------------------------------------------------------------------------
void File_Mpeg4::HowTo(stream_t StreamKind)
{
    switch (StreamKind)
    {
        case (Stream_General) :
            Fill_HowTo("Format", "R");
            Fill_HowTo("PlayTime", "R");
            Fill_HowTo("Album", "R");
            Fill_HowTo("Part/Track_Total", "R");
            Fill_HowTo("Movie", "R");
            Fill_HowTo("Track", "R");
            Fill_HowTo("Track/Position", "R");
            Fill_HowTo("Performer", "R");
            Fill_HowTo("Encoded_Application", "R");
            Fill_HowTo("Encoded_Date", "R");
            Fill_HowTo("Encoded_Library", "R");
            Fill_HowTo("Comment", "R");
            Fill_HowTo("ContentType", "R");
            Fill_HowTo("Cover", "R");
            Fill_HowTo("Cover_Datas", "N");
            break;
        case (Stream_Video) :
            Fill_HowTo("Codec", "R");
            Fill_HowTo("BitRate", "R");
            Fill_HowTo("Width", "R");
            Fill_HowTo("Height", "R");
            Fill_HowTo("AspectRatio", "N");
            Fill_HowTo("FrameRate", "R");
            break;
        case (Stream_Audio) :
            Fill_HowTo("Codec", "R");
            Fill_HowTo("BitRate", "R");
            Fill_HowTo("Channel(s)", "N");
            Fill_HowTo("SamplingRate", "R");
            Fill_HowTo("Resolution", "N");
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

//***************************************************************************
//
//***************************************************************************

} //NameSpace

#endif //MEDIAINFO_MPEG4_YES

