// MediaInfo_Config - Configuration class
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
// Global configuration of MediaInfo
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#include <wx/file.h>
#include "MediaInfo/MediaInfo_Config.h"
#include "ZenLib/ZtringListListF.h"
using namespace ZenLib;
using namespace std;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//---------------------------------------------------------------------------
const Char*  MediaInfo_Version=_T("MediaInfoLib - v0.7.4.4");
const Char*  MediaInfo_Url=_T("http://mediainfo.sourceforge.net");
      Ztring EmptyZtring;       //Use it when we can't return a reference to a true Ztring
const Ztring EmptyZtring_Const; //Use it when we can't return a reference to a true Ztring, const version
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void File__Base_Codec              (ZtringListList &Info);
void File__Base_DefaultLanguage    (ZtringListList &Info);
void File__Base_Iso639             (ZtringListList &Info);
void File__Base_General            (ZtringListList &Info);
void File__Base_Video              (ZtringListList &Info);
void File__Base_Audio              (ZtringListList &Info);
void File__Base_Text               (ZtringListList &Info);
void File__Base_Chapters           (ZtringListList &Info);
void File__Base_Image              (ZtringListList &Info);
void File__Base_Menu               (ZtringListList &Info);
void File__Base_Summary            (ZtringListList &Info);
void File__Base_Format             (ZtringListList &Info);
void File__Base_Encoder            (ZtringListList &Info);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
MediaInfo_Config Config;
//---------------------------------------------------------------------------

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

MediaInfo_Config::MediaInfo_Config()
{
    Complete=0;
    BlockMethod=0;
    Internet=0;
    MultipleValues=0;
    ParseUnknownExtensions=1;
    ShowFiles_Nothing=1;
    ShowFiles_VideoAudio=1;
    ShowFiles_VideoOnly=1;
    ShowFiles_AudioOnly=1;
    ShowFiles_TextOnly=1;
    ParseSpeed=0.01;
    Details=0;
    LineSeparator=_T("\r\n");
    ColumnSeparator=_T(";");
    TagSeparator=_T(" / ");
    Quote=_T("\"");
    DecimalPoint=_T(".");
    ThousandsPoint=_T("");

    StreamsMax[Stream_General]=1;
    StreamsMax[Stream_Video]=1;
    StreamsMax[Stream_Audio]=2;
    StreamsMax[Stream_Text]=2;
    StreamsMax[Stream_Chapters]=1;
    StreamsMax[Stream_Image]=1;
    StreamsMax[Stream_Menu]=1;

    File__Base_Format(Format);
    File__Base_Codec(Codec);
    File__Base_Encoder(Encoder);
    File__Base_Iso639(Iso639);
    File__Base_General(Info[Stream_General]);
    File__Base_Video(Info[Stream_Video]);
    File__Base_Audio(Info[Stream_Audio]);
    File__Base_Text(Info[Stream_Text]);
    File__Base_Chapters(Info[Stream_Chapters]);
    File__Base_Image(Info[Stream_Image]);
    File__Base_Menu(Info[Stream_Menu]);
    ZtringListList ZLL1; Language_Set(ZLL1);
}

MediaInfo_Config::~MediaInfo_Config()
{
}

//***************************************************************************
// Info
//***************************************************************************

Ztring MediaInfo_Config::Option (const Ztring &Option, const Ztring &Value)
{
         if (Option.empty())
    {
        return _T("");
    }
    else if (Option==_T("Charset_Config"))
    {
        return _T(""); //Only used in DLL, no Library action
    }
    else if (Option==_T("Charset_Output"))
    {
        return _T(""); //Only used in DLL, no Library action
    }
    else if (Option==_T("Complete"))
    {
        if (Value.empty())
            Complete_Set(0);
        else
            Complete_Set(1);
        return _T("");
    }
    else if (Option==_T("Complete_Get"))
    {
        if (Complete_Get())
            return _T("1");
        else
            return _T("");
    }
    else if (Option==_T("BlockMethod"))
    {
        if (Value.empty())
            BlockMethod_Set(0);
        else
            BlockMethod_Set(1);
        return _T("");
    }
    else if (Option==_T("BlockMethod_Get"))
    {
        if (BlockMethod_Get())
            return _T("1");
        else
            return _T("");
    }
    else if (Option==_T("Internet"))
    {
        if (Value.empty())
            Internet_Set(0);
        else
            Internet_Set(1);
        return _T("");
    }
    else if (Option==_T("Internet_Get"))
    {
        if (Internet_Get())
            return _T("1");
        else
            return _T("");
    }
    else if (Option==_T("MultipleValues"))
    {
        if (Value.empty())
            MultipleValues_Set(0);
        else
            MultipleValues_Set(1);
        return _T("");
    }
    else if (Option==_T("MultipleValues_Get"))
    {
        if (MultipleValues_Get())
            return _T("1");
        else
            return _T("");
    }
    else if (Option==_T("ParseUnknownExtensions"))
    {
        if (Value.empty())
            ParseUnknownExtensions_Set(0);
        else
            ParseUnknownExtensions_Set(1);
        return _T("");
    }
    else if (Option==_T("ParseUnknownExtensions_Get"))
    {
        if (ParseUnknownExtensions_Get())
            return _T("1");
        else
            return _T("");
    }
    else if (Option==_T("ShowFiles_Set"))
    {
        ShowFiles_Set(Value);
        return _T("");
    }
    else if (Option==_T("LineSeparator"))
    {
        LineSeparator_Set(Value);
        return _T("");
    }
    else if (Option==_T("LineSeparator_Get"))
    {
        return LineSeparator_Get();
    }
    else if (Option==_T("ColumnSeparator"))
    {
        ColumnSeparator_Set(Value);
        return _T("");
    }
    else if (Option==_T("ColumnSeparator_Get"))
    {
        return ColumnSeparator_Get();
    }
    else if (Option==_T("TagSeparator"))
    {
        TagSeparator_Set(Value);
        return _T("");
    }
    else if (Option==_T("TagSeparator_Get"))
    {
        return TagSeparator_Get();
    }
    else if (Option==_T("Quote"))
    {
        Quote_Set(Value);
        return _T("");
    }
    else if (Option==_T("Quote_Get"))
    {
        return Quote_Get();
    }
    else if (Option==_T("DecimalPoint"))
    {
        DecimalPoint_Set(Value);
        return _T("");
    }
    else if (Option==_T("DecimalPoint_Get"))
    {
        return DecimalPoint_Get();
    }
    else if (Option==_T("ThousandsPoint"))
    {
        ThousandsPoint_Set(Value);
        return _T("");
    }
    else if (Option==_T("ThousandsPoint_Get"))
    {
        return ThousandsPoint_Get();
    }
    else if (Option==_T("StreamMax"))
    {
        ZtringListList StreamMax=Value;
        StreamMax_Set(StreamMax);
        return _T("");
    }
    else if (Option==_T("StreamMax_Get"))
    {
        return StreamMax_Get();
    }
    else if (Option==_T("Language"))
    {
        ZtringListList Language=Value;
        Language_Set(Language);
        return _T("");
    }
    else if (Option==_T("Language_Get"))
    {
        return Language_Get();
    }
    else if (Option==_T("Inform"))
    {
        Inform_Set(Value);
        return _T("");
    }
    else if (Option==_T("Inform_Get"))
    {
        return Inform_Get();
    }
    else if (Option==_T("Info_Parameters"))
    {
        ZtringListList ToReturn=Info_Parameters_Get();

        //Adapt first column
        for (size_t Pos=0; Pos<ToReturn.size(); Pos++)
        {
            Ztring &C1=ToReturn(Pos, 0);
            if (!ToReturn(Pos, 1).empty())
            {
                C1.resize(25, ' ');
                ToReturn(Pos, 0)=C1 + _T(" :");
            }
        }

        ToReturn.Separator_Set(0, LineSeparator_Get());
        ToReturn.Separator_Set(1, _T(" "));
        ToReturn.Quote_Set(_T(""));
        return ToReturn.Read();
    }
    else if (Option==_T("Info_Parameters_CSV"))
    {
        return Info_Parameters_Get();
    }
    else if (Option==_T("Info_Codecs"))
    {
        return Info_Codecs_Get();
    }
    else if (Option==_T("Info_Version"))
    {
        return Info_Version_Get();
    }
    else if (Option==_T("Info_Url"))
    {
        return Info_Url_Get();
    }
    else
        return _T("Option not known");
}

//***************************************************************************
// Info
//***************************************************************************

//---------------------------------------------------------------------------
void MediaInfo_Config::Complete_Set (size_t NewValue)
{
    Enter(true);
    Complete=NewValue;
    Leave();
}

size_t MediaInfo_Config::Complete_Get ()
{
    Enter();
    Leave();
    return Complete;
}

//---------------------------------------------------------------------------
void MediaInfo_Config::BlockMethod_Set (size_t NewValue)
{
    Enter(true);
    BlockMethod=NewValue;
    Leave();
}

size_t MediaInfo_Config::BlockMethod_Get ()
{
    Enter();
    Leave();
    return BlockMethod;
}

//---------------------------------------------------------------------------
void MediaInfo_Config::Internet_Set (size_t NewValue)
{
    Enter(true);
    Internet=NewValue;
    Leave();
}

size_t MediaInfo_Config::Internet_Get ()
{
    Enter();
    Leave();
    return Internet;
}

//---------------------------------------------------------------------------
void MediaInfo_Config::MultipleValues_Set (size_t NewValue)
{
    Enter(true);
    MultipleValues=NewValue;
    Leave();
}

size_t MediaInfo_Config::MultipleValues_Get ()
{
    Enter();
    Leave();
    return MultipleValues;
}

//---------------------------------------------------------------------------
void MediaInfo_Config::ParseUnknownExtensions_Set (size_t NewValue)
{
    Enter(true);
    ParseUnknownExtensions=NewValue;
    Leave();
}

size_t MediaInfo_Config::ParseUnknownExtensions_Get ()
{
    Enter();
    Leave();
    return ParseUnknownExtensions;
}

//---------------------------------------------------------------------------
void MediaInfo_Config::ShowFiles_Set (const ZtringListList &NewShowFiles)
{
    Enter(true);
    for  (size_t Pos=0; Pos<NewShowFiles.size(); Pos++)
    {
        const Ztring& Object=NewShowFiles.Read(Pos, 0);
             if (Object==_T("Nothing"))
            ShowFiles_Nothing=NewShowFiles.Read(Pos, 1).empty()?1:0;
        else if (Object==_T("VideoAudio"))
            ShowFiles_VideoAudio=NewShowFiles.Read(Pos, 1).empty()?1:0;
        else if (Object==_T("VideoOnly"))
            ShowFiles_VideoOnly=NewShowFiles.Read(Pos, 1).empty()?1:0;
        else if (Object==_T("AudioOnly"))
            ShowFiles_AudioOnly=NewShowFiles.Read(Pos, 1).empty()?1:0;
        else if (Object==_T("TextOnly"))
            ShowFiles_TextOnly=NewShowFiles.Read(Pos, 1).empty()?1:0;
    }
    Leave();
}

size_t MediaInfo_Config::ShowFiles_Nothing_Get ()
{
    Enter();
    Leave();
    return ShowFiles_Nothing;
}

size_t MediaInfo_Config::ShowFiles_VideoAudio_Get ()
{
    Enter();
    Leave();
    return ShowFiles_VideoAudio;
}

size_t MediaInfo_Config::ShowFiles_VideoOnly_Get ()
{
    Enter();
    Leave();
    return ShowFiles_VideoOnly;
}

size_t MediaInfo_Config::ShowFiles_AudioOnly_Get ()
{
    Enter();
    Leave();
    return ShowFiles_AudioOnly;
}

size_t MediaInfo_Config::ShowFiles_TextOnly_Get ()
{
    Enter();
    Leave();
    return ShowFiles_TextOnly;
}

//---------------------------------------------------------------------------
void MediaInfo_Config::ParseSpeed_Set (float32 NewValue)
{
    Enter(true);
    ParseSpeed=NewValue;
    Leave();
}

float32 MediaInfo_Config::ParseSpeed_Get ()
{
    Enter();
    Leave();
    return ParseSpeed;
}

//---------------------------------------------------------------------------
void MediaInfo_Config::Details_Set (float NewValue)
{
    Enter(true);
    Details=NewValue;
    Leave();
}

float MediaInfo_Config::Details_Get ()
{
    Enter();
    Leave();
    return Details;
}

//---------------------------------------------------------------------------
void MediaInfo_Config::LineSeparator_Set (const Ztring &NewValue)
{
    Enter(true);
    LineSeparator=NewValue;
    Leave();
}

const Ztring &MediaInfo_Config::LineSeparator_Get ()
{
    Enter();
    Leave();
    return LineSeparator;
}

//---------------------------------------------------------------------------
void MediaInfo_Config::ColumnSeparator_Set (const Ztring &NewValue)
{
    Enter(true);
    ColumnSeparator=NewValue;
    Leave();
}

const Ztring &MediaInfo_Config::ColumnSeparator_Get ()
{
    Enter();
    Leave();
    return ColumnSeparator;
}

//---------------------------------------------------------------------------
void MediaInfo_Config::TagSeparator_Set (const Ztring &NewValue)
{
    Enter(true);
    TagSeparator=NewValue;
    Leave();
}

const Ztring &MediaInfo_Config::TagSeparator_Get ()
{
    Enter();
    Leave();
    return TagSeparator;
}

//---------------------------------------------------------------------------
void MediaInfo_Config::Quote_Set (const Ztring &NewValue)
{
    Enter(true);
    Quote=NewValue;
    Leave();
}

const Ztring &MediaInfo_Config::Quote_Get ()
{
    Enter();
    Leave();
    return Quote;
}

//---------------------------------------------------------------------------
void MediaInfo_Config::DecimalPoint_Set (const Ztring &NewValue)
{
    Enter(true);
    DecimalPoint=NewValue;
    Leave();
}

const Ztring &MediaInfo_Config::DecimalPoint_Get ()
{
    Enter();
    Leave();
    return DecimalPoint;
}

//---------------------------------------------------------------------------
void MediaInfo_Config::ThousandsPoint_Set (const Ztring &NewValue)
{
    Enter(true);
    ThousandsPoint=NewValue;
    Leave();
}

const Ztring &MediaInfo_Config::ThousandsPoint_Get ()
{
    Enter();
    Leave();
    return ThousandsPoint;
}

//---------------------------------------------------------------------------
void MediaInfo_Config::StreamMax_Set (const ZtringListList &NewValue)
{
    Enter(true);
    //TODO : implementation
    Leave();
}

Ztring MediaInfo_Config::StreamMax_Get ()
{
    Enter();
    ZtringListList StreamMax;
    //TODO : implementation
    Ztring ToReturn=StreamMax.Read();
    Leave();
    return ToReturn;
}

//---------------------------------------------------------------------------
void MediaInfo_Config::Language_Set (const ZtringListList &NewValue)
{
    Enter(true);

    //Which language to choose?
    bool Raw=false;
    //-Raw
         if (NewValue.size()==1 && NewValue[0].size()==1 && NewValue[0][0]==_T("raw"))
    {
        Raw=true;
        //Fill base words (with English translation even if we don't want of it)
        File__Base_DefaultLanguage(Language);
        //Write internal name instead of translation
        for (size_t Pos=0; Pos<Language.size(); Pos++)
            Language.Write(Language[Pos][0], Pos, 1);
    }
    //-Add custom language to English language
    else
    {
        //Fill base words (with English translation)
        File__Base_DefaultLanguage(Language);
        //Add custom language to English language
        for (size_t Pos=0; Pos<Language.size(); Pos++)
        {
            size_t PosNew=NewValue.Find(Language[Pos][0], 0, 0, _T("=="), Ztring_CaseSensitive);
            if (PosNew!=Error && 1<NewValue[PosNew].size())
                Language.Write(NewValue[PosNew][1], Pos, 1);
        }
    }

    //Fill Info
    for (size_t StreamKind=0; StreamKind<Stream_Max; StreamKind++)
        for (size_t Pos=0; Pos<Info[StreamKind].size(); Pos++)
        {
            //Strings - Info_Name_Text
            Ztring ToReplace=Info[StreamKind](Pos, Info_Name);
            if (!Raw && ToReplace.find(_T("/String"))!=Error)
            {
                ToReplace.FindAndReplace(_T("/String1"), _T(""));
                ToReplace.FindAndReplace(_T("/String2"), _T(""));
                ToReplace.FindAndReplace(_T("/String3"), _T(""));
                ToReplace.FindAndReplace(_T("/String4"), _T(""));
                ToReplace.FindAndReplace(_T("/String5"), _T(""));
                ToReplace.FindAndReplace(_T("/String6"), _T(""));
                ToReplace.FindAndReplace(_T("/String7"), _T(""));
                ToReplace.FindAndReplace(_T("/String8"), _T(""));
                ToReplace.FindAndReplace(_T("/String9"), _T(""));
                ToReplace.FindAndReplace(_T("/String"),  _T(""));
            }
            if (!Raw && ToReplace.find(_T("/"))!=Error) //Complex values, like XXX/YYY --> We translate both XXX and YYY
            {
                Ztring ToReplace1=ToReplace.SubString(_T(""), _T("/"));
                Ztring ToReplace2=ToReplace.SubString(_T("/"), _T(""));
                Info[StreamKind](Pos, Info_Name_Text)=Language.Read(ToReplace1, ToReplace1);
                Info[StreamKind](Pos, Info_Name_Text)+=_T("/");
                Info[StreamKind](Pos, Info_Name_Text)+=Language.Read(ToReplace2, ToReplace2);
            }
            else
                Info[StreamKind](Pos, Info_Name_Text)=Language.Read(ToReplace, ToReplace);
            //Strings - Info_Measure_Text
            Info[StreamKind](Pos, Info_Measure_Text)=Language.Read(Info[StreamKind](Pos, Info_Measure), Info[StreamKind](Pos, Info_Measure));
            //Slashes

        }

    Leave();
}

Ztring MediaInfo_Config::Language_Get ()
{
    Enter();
    Ztring ToReturn=Language.Read();
    Leave();
    return ToReturn;
}

const Ztring &MediaInfo_Config::Language_Get (const Ztring &Value)
{
    Enter();
    size_t Pos=Language.Find(Value, 0, 0, _T("=="), Ztring_CaseSensitive);
    if (Pos==Error || 1>=Language[Pos].size())
    {
        Leave();
        return EmptyString_Get();
    }
    Leave();
    return Language[Pos][1];
}

//---------------------------------------------------------------------------
Ztring MediaInfo_Config::Language_Get (const Ztring &Count, const Ztring &Value)
{
    //Integrity
    if (Count.empty())
        return EmptyString_Get();

    //Different Plurals are available or not?
    if (Language_Get(Value+_T("1")).empty())
    {
        //if (Count==_T("0") || Count==_T("1"))
            return Count+Language_Get(Value);
        //else
            //return Count+Language_Get(Value+_T("s"));
    }

    //Detecting plural form for multiple plurals
    size_t CountI=Count.To_int32u();
    size_t Pos3=CountI/100;
    int8u  Pos2=(CountI-Pos3)/10;
    int8u  Pos1=CountI-Pos3-Pos2;
    size_t Form=0;

    //Polish has 2 plurial, Algorithm of Polish
    if (Pos3==0)
    {
        if (Pos2==0)
        {
                 if (Pos1<=1)
                Form=1; //000 to 001 kanal
            else if (Pos1>=2 && Pos1<=4)
                Form=2; //002 to 004 kanaly
            else //if (Pos1>=5)
                Form=3; //005 to 009 kanalow
        }
        else if (Pos2==1)
                Form=3; //010 to 019 kanalow
        else //if (Pos2>=2)
        {
                 if (Pos1<=1)
                Form=3; //020 to 021, 090 to 091 kanalow
            else if (Pos1>=2 && Pos1<=4)
                Form=2; //022 to 024, 092 to 094 kanali
            else //if (Pos1>=5)
                Form=3; //025 to 029, 095 to 099 kanalow
        }
    }
    else //if (Pos3>=1)
    {
        if (Pos2==0)
        {
                 if (Pos1<=1)
                Form=3; //100 to 101 kanalow
            else if (Pos1>=2 && Pos1<=4)
                Form=2; //102 to 104 kanaly
            else //if (Pos1>=5)
                Form=3; //105 to 109 kanalow
        }
        else if (Pos2==1)
                Form=3; //110 to 119 kanalow
        else //if (Pos2>=2)
        {
                 if (Pos1<=1)
                Form=3; //120 to 121, 990 to 991 kanalow
            else if (Pos1>=2 && Pos1<=4)
                Form=2; //122 to 124, 992 to 994 kanali
            else //if (Pos1>=5)
                Form=3; //125 to 129, 995 to 999 kanalow
        }
    }

    Ztring ToReturn=Count;
         if (Form==1)
        ToReturn+=Language_Get(Value+_T("1"));
    else if (Form==2)
        ToReturn+=Language_Get(Value+_T("2"));
    else if (Form==3)
        ToReturn+=Language_Get(Value+_T("3"));
    return ToReturn;
}

//---------------------------------------------------------------------------
void MediaInfo_Config::Inform_Set (const ZtringListList &NewValue)
{
    Enter(true);
    if (NewValue.Read(0, 0)==_T("Details"))
        Details_Set(NewValue.Read(0, 1).To_float32());
    else
    {
        Details_Set(0);
        //Inform
        if (NewValue==_T("Summary"))
            File__Base_Summary(Custom_View);
        else
            Custom_View=NewValue;
    }

    //Parsing pointer to a file
    if (Custom_View(0, 0).find(_T("file://"))==0)
    {
        ZtringListListF FromFile;
        Ztring FileName(Custom_View(0, 0), 7, Ztring::npos);
        FromFile.Load(FileName.c_str());
        Custom_View=FromFile;
    }

    //Parsing pointers to files in streams
    for (size_t Pos=0; Pos<Custom_View.size(); Pos++)
    {
        if (Custom_View(Pos, 1).find(_T("file://"))==0)
        {
            //Open
            Ztring FileName(Custom_View(Pos, 1), 7, Ztring::npos);
            wxFile F;
            F.Open(FileName.c_str());

            //Read
            char* Buffer=new char[F.Length()+1];
            Buffer[F.Length()+1]='\0';
            F.Read(Buffer, F.Length());
            F.Close();
            Ztring FromFile; FromFile.From_Local(Buffer);

            //Merge
            FromFile.FindAndReplace(_T("\r\n"), _T("\\r\\n"), 0, Ztring_Recursive);
            FromFile.FindAndReplace(_T("\n"), _T("\\r\\n"), 0, Ztring_Recursive);
            Custom_View(Pos, 1)=FromFile;
        }
    }

    Leave();
}

Ztring MediaInfo_Config::Inform_Get ()
{
    Enter();
    Ztring ToReturn=Custom_View.Read();
    Leave();
    return ToReturn;
}

const Ztring &MediaInfo_Config::Inform_Get (const Ztring &Value)
{
    Enter();
    size_t Pos=Custom_View.Find(Value);
    if (Pos==Error || 1>=Custom_View[Pos].size())
    {
        Leave();
        return EmptyString_Get();
    }
    Leave();
    return Custom_View[Pos][1];
}

//---------------------------------------------------------------------------
const Ztring &MediaInfo_Config::Format_Get (const Ztring &Value, infoformat_t KindOfFormatInfo) const
{
    size_t Pos=Format.Find(Value, 0, 0, _T("=="), Ztring_CaseSensitive);
    if (Pos==Error || (size_t)KindOfFormatInfo>=Format[Pos].size())
        return EmptyString_Get();
    return Format[Pos][KindOfFormatInfo];
}

//---------------------------------------------------------------------------
const Ztring &MediaInfo_Config::Codec_Get (const Ztring &Value, infocodec_t KindOfCodecInfo) const
{
    size_t Pos=Codec.Find(Value, 0, 0, _T("=="), Ztring_CaseSensitive);
    if (Pos==Error || (size_t)KindOfCodecInfo>=Codec[Pos].size())
        return EmptyString_Get();
    return Codec[Pos][KindOfCodecInfo];
}

//---------------------------------------------------------------------------
const Ztring &MediaInfo_Config::Codec_Get (const Ztring &Value, infocodec_t KindOfCodecInfo, stream_t KindOfStream) const
{
    //Transform to text
    Ztring KindOfStreamS;
    switch (KindOfStream)
    {
        case Stream_General  : KindOfStreamS=_T("G"); break;
        case Stream_Video    : KindOfStreamS=_T("V"); break;
        case Stream_Audio    : KindOfStreamS=_T("A"); break;
        case Stream_Text     : KindOfStreamS=_T("T"); break;
        case Stream_Image    : KindOfStreamS=_T("I"); break;
        case Stream_Chapters : KindOfStreamS=_T("C"); break;
        case Stream_Menu     : KindOfStreamS=_T("M"); break;
        case Stream_Max      : KindOfStreamS=_T(" "); break;
    }

    //Searching
    size_t Pos=0;
    do
        Pos=Codec.Find(Value, 0, Pos+1, _T("=="), Ztring_CaseSensitive);
    while (Pos!=Error && Codec.Read(Pos, InfoCodec_KindOfStream)!=KindOfStreamS);

    //Is it ok?
    if (Pos==Error || (size_t)KindOfCodecInfo>=Codec[Pos].size())
        return EmptyString_Get();
    return Codec[Pos][KindOfCodecInfo];
}

//---------------------------------------------------------------------------
const Ztring &MediaInfo_Config::Encoder_Get (const Ztring &Value, infoencoder_t KindOfEncoderInfo) const
{
    size_t Pos=Encoder.Find(Value, 0, 0, _T("=="), Ztring_CaseSensitive);
    if (Pos==Error || (size_t)KindOfEncoderInfo>=Encoder[Pos].size())
        return EmptyString_Get();
    return Encoder[Pos][KindOfEncoderInfo];
}

//---------------------------------------------------------------------------
const Ztring &MediaInfo_Config::Iso639_Get (const Ztring &Value) const
{
    size_t Pos=Iso639.Find(Value);
    if (Pos==Error || 1>=Iso639.size())
        return EmptyString_Get();
    return Iso639[Pos][1];
}

//---------------------------------------------------------------------------
const Ztring &MediaInfo_Config::Info_Get (stream_t KindOfStream, const Ztring &Value, info_t KindOfInfo) const
{
    if (KindOfStream>=Stream_Max)
        return EmptyString_Get();
    size_t Pos=Info[KindOfStream].Find(Value);
    if (Pos==Error || (size_t)KindOfInfo>=Info[KindOfStream][Pos].size())
        return EmptyString_Get();
    return Info[KindOfStream][Pos][KindOfInfo];
}

const Ztring &MediaInfo_Config::Info_Get (stream_t KindOfStream, size_t Pos, info_t KindOfInfo) const
{
    if (KindOfStream>=Stream_Max)
        return EmptyString_Get();
    if (Pos>=Info[KindOfStream].size() || (size_t)KindOfInfo>=Info[KindOfStream][Pos].size())
        return EmptyString_Get();
    return Info[KindOfStream][Pos][KindOfInfo];
}

//---------------------------------------------------------------------------
Ztring MediaInfo_Config::Info_Parameters_Get () const
{
    ZtringListList ToReturn;
    size_t ToReturn_Pos=0;

    for (size_t StreamKind=0; StreamKind<Stream_Max; StreamKind++)
    {
        ToReturn(ToReturn_Pos, 0)=Info[StreamKind].Read(_T("StreamKind"), Info_Measure);
        ToReturn_Pos++;
        for (size_t Pos=0; Pos<Info[StreamKind].size(); Pos++)
            if (!Info[StreamKind].Read(Pos, Info_Name).empty())
            {
                ToReturn(ToReturn_Pos, 0)=Info[StreamKind].Read(Pos, Info_Name);
                ToReturn(ToReturn_Pos, 1)=Info[StreamKind].Read(Pos, Info_Info);
                ToReturn_Pos++;
            }
        ToReturn_Pos++;
    }
    return ToReturn.Read();
}

//---------------------------------------------------------------------------
Ztring MediaInfo_Config::Info_Tags_Get () const
{
    return _T("");
}

Ztring MediaInfo_Config::Info_Codecs_Get () const
{
    return Codec.Read();
}

Ztring MediaInfo_Config::Info_Version_Get () const
{
    return MediaInfo_Version;
}

Ztring MediaInfo_Config::Info_Url_Get () const
{
    return MediaInfo_Url;
}

//---------------------------------------------------------------------------
Ztring &MediaInfo_Config::EmptyString_Get ()
{
    Enter();
    EmptyZtring.clear();
    Leave();
    return EmptyZtring;
}

const Ztring &MediaInfo_Config::EmptyString_Get () const
{
    return EmptyZtring_Const;
}

//***************************************************************************
// Enter/Leave
//***************************************************************************

void MediaInfo_Config::Enter (bool)
{
}

void MediaInfo_Config::Leave ()
{
}

} //NameSpace

