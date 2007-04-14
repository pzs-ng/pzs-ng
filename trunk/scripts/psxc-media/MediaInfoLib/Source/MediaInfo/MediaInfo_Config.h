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
#ifndef MediaInfo_ConfigH
#define MediaInfo_ConfigH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <ZenLib/ZtringListList.h>
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//---------------------------------------------------------------------------
/// \mainpage MediaInfoLib Index Page
///
/// \section Interfaces
///
/// There is 3 access methods
///
/// \subsection MediaInfo MediaInfo class (MediaInfoLib::MediaInfo)
/// Simplest interface : one file only \n
/// MediaInfoLib::MediaInfo::Open to analyse file \n
/// MediaInfoLib::MediaInfo::Inform to have a summary \n
/// MediaInfoLib::MediaInfo::Get to retreive one piece of information \n
///
/// \subsection MediaInfoList List class (MediaInfoLib::MediaInfoList)
/// To manage a list of files \n
/// MediaInfoLib::MediaInfoList::Open to analyse file \n
/// MediaInfoLib::MediaInfoList::Inform to have a summary \n
/// MediaInfoLib::MediaInfoList::Get to retreive one piece of information \n
/// MediaInfoLib::MediaInfoList::Close to close one file \n
///
/// \subsection C C Interface (MediaInfo_*)
/// For compatibility and DLL interface \n
/// This is a C interface for the List class \n
/// Note : Don't forget to include the MediaInfoDLL.Def file in your project! \n
/// MediaInfoDLL::MediaInfo_Open to analyse file \n
/// MediaInfoDLL::MediaInfo_Inform to have a summary \n
/// MediaInfoDLL::MediaInfo_Get to retreive one piece of information \n
/// MediaInfoDLL::MediaInfo_Close to free memory \n
///
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/// @brief Kinds of Stream
enum stream_t
{
    Stream_General,                 ///< StreamKind = General
    Stream_Video,                   ///< StreamKind = Video
    Stream_Audio,                   ///< StreamKind = Audio
    Stream_Text,                    ///< StreamKind = Text
    Stream_Chapters,                ///< StreamKind = Chapters
    Stream_Image,                   ///< StreamKind = Image
    Stream_Menu,                    ///< StreamKind = Menu
    Stream_Max
};

/// @brief Kind of information
enum info_t
{
    Info_Name,                      ///< InfoKind = Unique name of parameter
    Info_Text,                      ///< InfoKind = Value of parameter
    Info_Measure,                   ///< InfoKind = Unique name of measure unit of parameter
    Info_Options,                   ///< InfoKind = See infooptions_t
    Info_Name_Text,                 ///< InfoKind = Translated name of parameter
    Info_Measure_Text,              ///< InfoKind = Translated name of measure unit
    Info_Info,                      ///< InfoKind = More information about the parameter
    Info_HowTo,                     ///< InfoKind = How this parameter is supported, could be N (No), B (Beta), R (Read only), W (Read/Write)
    Info_Domain,                    ///< InfoKind = Domain of this piece of information
    Info_Max
};

/// Get(...)[infooptions_t] return a string like "YNYN..." \n
/// Use this enum to know at what correspond the Y (Yes) or N (No)
/// If Get(...)[0]==Y, then :
/// @brief Option if InfoKind = Info_Options
enum infooptions_t
{
    InfoOption_ShowInInform,        ///< Show this parameter in Inform()
    InfoOption_Reserved,            ///<
    InfoOption_ShowInSupported,     ///< Internal use only (info : Must be showed in Info_Capacities() )
    InfoOption_TypeOfValue,         ///< Value return by a standard Get() can be : T (Text), I (Integer, warning up to 64 bits), F (Float), D (Date), B (Binary datas coded Base64) (Numbers are in Base 10)
    InfoOption_Max
};

/// @brief File opening options
enum fileoptions_t
{
    FileOption_Nothing      =0x00,
    FileOption_Recursive    =0x01,  ///< Browse folders recursively
    FileOption_CloseAll     =0x02,  ///< Close all files before open
    FileOption_Max          =0x04
};

/// Used with Format info
/// @brief Used with Format info
enum infoformat_t
{
    InfoFormat_Name,                ///<
    InfoFormat_LongName,            ///<
    InfoFormat_Family,              ///<
    InfoFormat_KindofFormat,        ///<
    InfoFormat_Parser,              ///<
    InfoFormat_Info,                ///<
    InfoFormat_Extensions,          ///<
    InfoFormat_Url,                 ///<
    InfoFormat_Max
};

/// Used with Codec info
/// @brief Used with Codec info
enum infocodec_t
{
    InfoCodec_Codec,                ///<
    InfoCodec_Name,                 ///<
    InfoCodec_KindOfCode,           ///<
    InfoCodec_KindOfStream,         ///<
    InfoCodec_KindofCodec,          ///<
    InfoCodec_BitRate_Mode,         ///<
    InfoCodec_Description,          ///<
    InfoCodec_Url,                  ///<
    InfoCodec_Max
};

/// Used with Encoder info
/// @brief Used with Encoder info
enum infoencoder_t
{
    InfoEncoder_Name,               ///<
    InfoEncoder_LongName,           ///<
    InfoEncoder_Date,               ///<
    InfoEncoder_Max
};

/// Used by BlockMethod
/// @brief Used by BlockMethod
enum blockmethod_t
{
    BlockMethod_Now,                ///< Return now, without parsing (init only)
    BlockMethod_Often,              ///< Return as often as possible
    BlockMethod_Local,              ///< Return after local parsing (no Internet connection)
    BlockMethod_Needed,             ///< Return when a user interaction is needed
    BlockMethod_Max
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//Char types
#undef  _T
#define _T(__x)     __T(__x)
#if defined(UNICODE) || defined (_UNICODE)
    typedef wchar_t Char;
    #undef  __T
    #define __T(__x) L ## __x
#else
    typedef char Char;
    #undef  __T
    #define __T(__x) __x
#endif
typedef std::basic_string<Char, std::char_traits<Char>, std::allocator<Char> > MediaInfo_String;
typedef std::basic_stringstream<Char> MediaInfo_StringStream;
typedef std::basic_string<Char, std::char_traits<Char>, std::allocator<Char> > String;
typedef std::basic_stringstream<Char> StringStream;
//---------------------------------------------------------------------------

//***************************************************************************
// Class MediaInfo_Config
//***************************************************************************

class MediaInfo_Config
{
public :
    //Constructor/Destructor
    MediaInfo_Config();
    ~MediaInfo_Config();

    //General
    Ztring Option (const Ztring &Option, const Ztring &Value=_T(""));

    //Info
          void      Complete_Set (size_t NewValue);
          size_t    Complete_Get ();

          void      BlockMethod_Set (size_t NewValue);
          size_t    BlockMethod_Get ();

          void      Internet_Set (size_t NewValue);
          size_t    Internet_Get ();

          void      MultipleValues_Set (size_t NewValue);
          size_t    MultipleValues_Get ();

          void      ParseUnknownExtensions_Set (size_t NewValue);
          size_t    ParseUnknownExtensions_Get ();

          void      ShowFiles_Set (const ZtringListList &NewShowFiles);
          size_t    ShowFiles_Nothing_Get ();
          size_t    ShowFiles_VideoAudio_Get ();
          size_t    ShowFiles_VideoOnly_Get ();
          size_t    ShowFiles_AudioOnly_Get ();
          size_t    ShowFiles_TextOnly_Get ();

          void      ParseSpeed_Set (float32 NewValue);
          float32   ParseSpeed_Get ();

          void      Details_Set (float NewValue);
          float     Details_Get ();

          void      LineSeparator_Set (const Ztring &NewValue);
    const Ztring   &LineSeparator_Get ();

          void      ColumnSeparator_Set (const Ztring &NewValue);
    const Ztring   &ColumnSeparator_Get ();

          void      TagSeparator_Set (const Ztring &NewValue);
    const Ztring   &TagSeparator_Get ();

          void      Quote_Set (const Ztring &NewValue);
    const Ztring   &Quote_Get ();

          void      DecimalPoint_Set (const Ztring &NewValue);
    const Ztring   &DecimalPoint_Get ();

          void      ThousandsPoint_Set (const Ztring &NewValue);
    const Ztring   &ThousandsPoint_Get ();

          void      StreamMax_Set (const ZtringListList &NewValue);
          Ztring    StreamMax_Get ();

          void      Language_Set (const ZtringListList &NewLanguage);
          Ztring    Language_Get ();
    const Ztring   &Language_Get (const Ztring &Value);
          Ztring    Language_Get (const Ztring &Count, const Ztring &Value);

          void      Inform_Set (const ZtringListList &NewInform);
          Ztring    Inform_Get ();
    const Ztring   &Inform_Get (const Ztring &Value);

    const Ztring   &Format_Get (const Ztring &Value, infoformat_t KindOfFormatInfo=InfoFormat_Name) const;
    const ZtringListList &Format_Get() const {return Format;}; //Should not be, but too difficult to hide it

    const Ztring   &Codec_Get (const Ztring &Value, infocodec_t KindOfCodecInfo=InfoCodec_Name) const;
    const Ztring   &Codec_Get (const Ztring &Value, infocodec_t KindOfCodecInfo, stream_t KindOfStream) const;

    const Ztring   &Encoder_Get (const Ztring &Value, infoencoder_t KindOfEncoderInfo=InfoEncoder_LongName) const;

    const Ztring   &Iso639_Get (const Ztring &Value) const;

    const Ztring   &Info_Get (stream_t KindOfStream, const Ztring &Value, info_t KindOfInfo=Info_Text) const;
    const Ztring   &Info_Get (stream_t KindOfStream, size_t Pos, info_t KindOfInfo=Info_Text) const;
    const ZtringListList &Info_Get(stream_t KindOfStream) const {return Info[KindOfStream];}; //Should not be, but too difficult to hide it

          Ztring    Info_Parameters_Get () const;
          Ztring    Info_Tags_Get       () const;
          Ztring    Info_Codecs_Get     () const;
          Ztring    Info_Version_Get    () const;
          Ztring    Info_Url_Get        () const;

          Ztring   &EmptyString_Get(); //Use it when we can't return a reference to a true string
    const Ztring   &EmptyString_Get() const; //Use it when we can't return a reference to a true string

private :
    void Enter (bool Set=false);
    void Leave ();

    size_t          Complete;
    size_t          BlockMethod;
    size_t          Internet;
    size_t          MultipleValues;
    size_t          ParseUnknownExtensions;
    size_t          ShowFiles_Nothing;
    size_t          ShowFiles_VideoAudio;
    size_t          ShowFiles_VideoOnly;
    size_t          ShowFiles_AudioOnly;
    size_t          ShowFiles_TextOnly;
    float32         ParseSpeed;
    float           Details;
    Ztring          ColumnSeparator;
    Ztring          LineSeparator;
    Ztring          TagSeparator;
    Ztring          Quote;
    Ztring          DecimalPoint;
    Ztring          ThousandsPoint;
    size_t          StreamsMax[Stream_Max];
    ZtringListList  Language; //ex. : "KB;Ko"
    ZtringListList  Custom_View; //Definition of "General", "Video", "Audio", "Text", "Chapters", "Image"

    ZtringListList  Format;
    ZtringListList  Codec;
    ZtringListList  Encoder;
    ZtringListList  Iso639;
    ZtringListList  Info[Stream_Max]; //General info
};

extern MediaInfo_Config Config;

} //NameSpace

#endif
