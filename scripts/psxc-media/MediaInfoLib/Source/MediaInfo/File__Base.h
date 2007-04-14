// File__Base - Base for other files
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
// Give common methods for all file types
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef MediaInfo__BaseH
#define MediaInfo__BaseH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/MediaInfo_Config.h"
#include <ZenLib/ZtringListList.h>
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Class File__Base
//***************************************************************************

class File__Base
{
public :
    //Constructor/Destructor
    File__Base();
    virtual ~File__Base();

    //Analyze
    int     Open_File (const Ztring &CompleteFileName); //1 if succeed, 0 if problem with format
    void    Open_Buffer_Init (int64u File_Size=0, int64u File_Offset=0);
    void    Open_Buffer_Init (File__Base* Sub);
    void    Open_Buffer_Init (File__Base* Sub, int64u File_Size, int64u File_Offset=0);
    void    Open_Buffer_Continue (const int8u* Buffer, size_t Buffer_Size);
    void    Open_Buffer_Continue (File__Base* Sub, const int8u* Buffer, size_t Buffer_Size);
    void    Open_Buffer_Finalize ();
    void    Open_Buffer_Finalize (File__Base* Sub);
    int     Save ();

    //Get information
    Ztring  Inform ();
    Ztring  Inform (stream_t StreamKind, size_t StreamNumber=0); //All about only a specific stream
    const Ztring &Get (stream_t StreamKind, size_t StreamNumber, size_t Parameter, info_t KindOfInfo=Info_Text);
    const Ztring &Get (stream_t StreamKind, size_t StreamNumber, const Ztring &Parameter, info_t KindOfInfo=Info_Text, info_t KindOfSearch=Info_Name);

    //Set information
    int           Set (stream_t StreamKind, size_t StreamNumber, size_t Parameter, const Ztring &ToSet, const Ztring &OldValue=_T(""));
    int           Set (stream_t StreamKind, size_t StreamNumber, const Ztring &Parameter, const Ztring &ToSet, const Ztring &OldValue=_T(""));

    //Options
    size_t Count_Get (stream_t StreamKind, size_t Pos=Error) const;
    void   Language_Set (); //Update language for an existing File__Base

    //Infos
           ZtringListList Info_Capacities ();
    static ZtringListList Info_Capacities_Parameters ();

    //Before filling the stream, the stream must be prepared
    size_t Stream_Prepare   (stream_t KindOfStream);
    void   General_Fill     (); //Special : pre-fill General with some important information

    //Fill with datas
    void Fill (stream_t StreamKind, size_t StreamPos, const char* Parameter, const Ztring  &Value, bool Replace=false);
    void Fill_HowTo (stream_t StreamKind, size_t StreamPos, const char* Parameter, const char* Value);
    inline void Fill (stream_t StreamKind, size_t StreamPos, const char* Parameter, const std::string &Value, bool Utf8=true, bool Replace=false) {if (Utf8) Fill(StreamKind, StreamPos, Parameter, Ztring().From_UTF8(Value.c_str(), Value.size())); else Fill(StreamKind, StreamPos, Parameter, Ztring().From_Local(Value.c_str(), Value.size()), Replace);}
    inline void Fill (stream_t StreamKind, size_t StreamPos, const char* Parameter, const char*    Value, intu Value_Size=Error, bool Utf8=true, bool Replace=false) {if (Utf8) Fill(StreamKind, StreamPos, Parameter, Ztring().From_UTF8(Value, Value_Size)); else Fill(StreamKind, StreamPos, Parameter, Ztring().From_Local(Value, Value_Size), Replace);}
    inline void Fill (stream_t StreamKind, size_t StreamPos, const char* Parameter, const wchar_t* Value, intu Value_Size=Error, bool Replace=false) {Fill(StreamKind, StreamPos, Parameter, Ztring().From_Unicode(Value, Value_Size), Replace);}
    inline void Fill (stream_t StreamKind, size_t StreamPos, const char* Parameter, int8u          Value, intu Radix=10, bool Replace=false) {Fill(StreamKind, StreamPos, Parameter, Ztring::ToZtring(Value, Radix).MakeUpperCase(), Replace);}
    inline void Fill (stream_t StreamKind, size_t StreamPos, const char* Parameter, int8s          Value, intu Radix=10, bool Replace=false) {Fill(StreamKind, StreamPos, Parameter, Ztring::ToZtring(Value, Radix).MakeUpperCase(), Replace);}
    inline void Fill (stream_t StreamKind, size_t StreamPos, const char* Parameter, int16u         Value, intu Radix=10, bool Replace=false) {Fill(StreamKind, StreamPos, Parameter, Ztring::ToZtring(Value, Radix).MakeUpperCase(), Replace);}
    inline void Fill (stream_t StreamKind, size_t StreamPos, const char* Parameter, int16s         Value, intu Radix=10, bool Replace=false) {Fill(StreamKind, StreamPos, Parameter, Ztring::ToZtring(Value, Radix).MakeUpperCase(), Replace);}
    inline void Fill (stream_t StreamKind, size_t StreamPos, const char* Parameter, int32u         Value, intu Radix=10, bool Replace=false) {Fill(StreamKind, StreamPos, Parameter, Ztring::ToZtring(Value, Radix).MakeUpperCase(), Replace);}
    inline void Fill (stream_t StreamKind, size_t StreamPos, const char* Parameter, int32s         Value, intu Radix=10, bool Replace=false) {Fill(StreamKind, StreamPos, Parameter, Ztring::ToZtring(Value, Radix).MakeUpperCase(), Replace);}
    inline void Fill (stream_t StreamKind, size_t StreamPos, const char* Parameter, int64u         Value, intu Radix=10, bool Replace=false) {Fill(StreamKind, StreamPos, Parameter, Ztring::ToZtring(Value, Radix).MakeUpperCase(), Replace);}
    inline void Fill (stream_t StreamKind, size_t StreamPos, const char* Parameter, int64s         Value, intu Radix=10, bool Replace=false) {Fill(StreamKind, StreamPos, Parameter, Ztring::ToZtring(Value, Radix).MakeUpperCase(), Replace);}
    inline void Fill (stream_t StreamKind, size_t StreamPos, const char* Parameter, float32        Value, intu AfterComma=3, bool Replace=false) {Fill(StreamKind, StreamPos, Parameter, Ztring::ToZtring(Value, AfterComma), Replace);}
    #if defined (MACOSX)
    inline void Fill (stream_t StreamKind, size_t StreamPos, const char* Parameter, size_t         Value, intu Radix=10, bool Replace=false) {Fill(StreamKind, StreamPos, Parameter, Ztring::ToZtring(Value, Radix).MakeUpperCase(), Replace);}
    #endif
    inline void Fill (const char* Parameter, const Ztring  &Value, bool Replace=false) {Fill(StreamKind_Last, StreamPos_Last, Parameter, Value, Replace);} //With the last set
    inline void Fill (const char* Parameter, const std::string &Value, bool Utf8=true, bool Replace=false) {Fill(StreamKind_Last, StreamPos_Last, Parameter, Value, Utf8, Replace);} //With the last set
    inline void Fill (const char* Parameter, const char*    Value, intu ValueSize=Error, bool Utf8=true, bool Replace=false) {Fill(StreamKind_Last, StreamPos_Last, Parameter, Value, ValueSize, Utf8, Replace);} //With the last set
    inline void Fill (const char* Parameter, const int8u*   Value, intu ValueSize=Error, bool Utf8=true, bool Replace=false) {Fill(StreamKind_Last, StreamPos_Last, Parameter, (const char*)Value, ValueSize, Utf8, Replace);} //With the last set
    inline void Fill (const char* Parameter, const wchar_t* Value, intu ValueSize=Error, bool Replace=false) {Fill(StreamKind_Last, StreamPos_Last, Parameter, Value, ValueSize, Replace);} //With the last set
    inline void Fill (const char* Parameter, int8u          Value, intu Radix=10, bool Replace=false) {Fill(StreamKind_Last, StreamPos_Last, Parameter, Value, Radix, Replace);} //With the last set
    inline void Fill (const char* Parameter, int8s          Value, intu Radix=10, bool Replace=false) {Fill(StreamKind_Last, StreamPos_Last, Parameter, Value, Radix, Replace);} //With the last set
    inline void Fill (const char* Parameter, int16u         Value, intu Radix=10, bool Replace=false) {Fill(StreamKind_Last, StreamPos_Last, Parameter, Value, Radix, Replace);} //With the last set
    inline void Fill (const char* Parameter, int16s         Value, intu Radix=10, bool Replace=false) {Fill(StreamKind_Last, StreamPos_Last, Parameter, Value, Radix, Replace);} //With the last set
    inline void Fill (const char* Parameter, int32u         Value, intu Radix=10, bool Replace=false) {Fill(StreamKind_Last, StreamPos_Last, Parameter, Value, Radix, Replace);} //With the last set
    inline void Fill (const char* Parameter, int32s         Value, intu Radix=10, bool Replace=false) {Fill(StreamKind_Last, StreamPos_Last, Parameter, Value, Radix, Replace);} //With the last set
    inline void Fill (const char* Parameter, int64u         Value, intu Radix=10, bool Replace=false) {Fill(StreamKind_Last, StreamPos_Last, Parameter, Value, Radix, Replace);} //With the last set
    inline void Fill (const char* Parameter, int64s         Value, intu Radix=10, bool Replace=false) {Fill(StreamKind_Last, StreamPos_Last, Parameter, Value, Radix, Replace);} //With the last set
    inline void Fill (const char* Parameter, float32        Value, intu AfterComma=3, bool Replace=false) {Fill(StreamKind_Last, StreamPos_Last, Parameter, Value, AfterComma, Replace);} //With the last set
    inline void Fill (const char* Parameter, double         Value, intu AfterComma=3, bool Replace=false) {Fill(StreamKind_Last, StreamPos_Last, Parameter, (float32)Value, AfterComma, Replace);} //With the last set
    #if defined (MACOSX)
    inline void Fill (const char* Parameter, size_t         Value, intu Radix=10, bool Replace=false) {Fill(StreamKind_Last, StreamPos_Last, Parameter, Value, Radix, Replace);} //With the last set
    #endif
    inline void Fill_HowTo (const char* Parameter, const char* Value) {Fill_HowTo(StreamKind_Last, StreamPos_Last, Parameter, Value);} //With the last set
    ZtringListList Fill_Temp;
    void Fill_Flush ();

protected :
    //Read
    virtual void Read_File            (); //To overload
    virtual void Read_Buffer_Init     (); //To overload
    virtual void Read_Buffer_Unsynched(); //To overload
    virtual void Read_Buffer_Continue (); //To overload
    virtual void Read_Buffer_Finalize (); //To overload

    //Write
    virtual int Write       (stream_t StreamKind, size_t StreamNumber, const Ztring &Parameter, const Ztring &ToSet, const Ztring &OldValue); //Write the value in memory
    virtual int WriteToDisk (); //Write modified tags on disk

    //How to
    virtual void HowTo (stream_t StreamType);

    //Arrays
    std::vector<ZtringListList> General;
    std::vector<ZtringListList> Video;
    std::vector<ZtringListList> Audio;
    std::vector<ZtringListList> Text;
    std::vector<ZtringListList> Chapters;
    std::vector<ZtringListList> Image;
    std::vector<ZtringListList> Menu;
    std::vector<ZtringListList>* Stream[Stream_Max]; //pointer to others listed streams

    //Debug
    Ztring Details;
    void Details_Add_Element      (int8u Level, const Ztring &Text, size_t Size=Error); //Element
    void Details_Add_Element      (int8u Level, const char*   Text, size_t Size=Error) {Details_Add_Element(Level, Ztring().From_UTF8(Text), Size);}
    void Details_Add_Element      (int8u Level, int8u         Text, size_t Size=Error);
    void Details_Add_Element      (int8u Level, int16u        Text, size_t Size=Error);
    void Details_Add_Element      (int8u Level, int32u        Text, size_t Size=Error);
    void Details_Add_Element      (             const Ztring &Text                   ); //More text about element
    void Details_Add_Element      (             const char*   Text                   ) {Details_Add_Element(Ztring().From_UTF8(Text));}
    void Details_Add_Element_Info (size_t Pos,  const char*   Parameter, const Ztring& Value);
    void Details_Add_Element_Info (size_t Pos,  const char*   Parameter, int32u Value, intu Radix=10) {Details_Add_Element_Info(Pos, Parameter, Ztring::ToZtring(Value, Radix).MakeUpperCase());}
    void Details_Add_Element_Flush();
    void Details_Add_Element_Purge();
    void Details_Add_Info (size_t Pos, const char* Parameter, const Ztring& Value);
    void Details_Add_Info (size_t Pos, const char* Parameter, const std::string& Value) {Details_Add_Info(Pos, Parameter, Ztring().From_Local(Value.c_str()));}
    void Details_Add_Info (size_t Pos, const char* Parameter, const char* Value, intu Value_Size=Error, bool Utf8=true) {if (Utf8) Details_Add_Info(Pos, Parameter, Ztring().From_UTF8(Value, Value_Size)); else Details_Add_Info(Pos, Parameter, Ztring().From_Local(Value, Value_Size));}
    void Details_Add_Info (size_t Pos, const char* Parameter, const int8u* Value, intu Value_Size=Error, bool Utf8=true) {Details_Add_Info(Pos, Parameter, (const char*)Value, Value_Size, Utf8);}
    void Details_Add_Info (size_t Pos, const char* Parameter, bool   Value) {if (Value) Details_Add_Info(Pos, Parameter, "Yes"); else Details_Add_Info(Pos, Parameter, "No");}
    void Details_Add_Info (size_t Pos, const char* Parameter, int8u  Value, intu Radix=16) {Details_Add_Info(Pos, Parameter, Ztring::ToZtring(Value, Radix).MakeUpperCase()+_T(" (")+Ztring::ToZtring(Value, 10).MakeUpperCase()+_T(")"));}
    void Details_Add_Info (size_t Pos, const char* Parameter, int8s  Value, intu Radix=16) {Details_Add_Info(Pos, Parameter, Ztring::ToZtring(Value, Radix).MakeUpperCase()+_T(" (")+Ztring::ToZtring(Value, 10).MakeUpperCase()+_T(")"));}
    void Details_Add_Info (size_t Pos, const char* Parameter, int16u Value, intu Radix=16) {Details_Add_Info(Pos, Parameter, Ztring::ToZtring(Value, Radix).MakeUpperCase()+_T(" (")+Ztring::ToZtring(Value, 10).MakeUpperCase()+_T(")"));}
    void Details_Add_Info (size_t Pos, const char* Parameter, int16s Value, intu Radix=16) {Details_Add_Info(Pos, Parameter, Ztring::ToZtring(Value, Radix).MakeUpperCase()+_T(" (")+Ztring::ToZtring(Value, 10).MakeUpperCase()+_T(")"));}
    void Details_Add_Info (size_t Pos, const char* Parameter, int32u Value, intu Radix=16) {Details_Add_Info(Pos, Parameter, Ztring::ToZtring(Value, Radix).MakeUpperCase()+_T(" (")+Ztring::ToZtring(Value, 10).MakeUpperCase()+_T(")"));}
    void Details_Add_Info (size_t Pos, const char* Parameter, int32s Value, intu Radix=16) {Details_Add_Info(Pos, Parameter, Ztring::ToZtring(Value, Radix).MakeUpperCase()+_T(" (")+Ztring::ToZtring(Value, 10).MakeUpperCase()+_T(")"));}
    void Details_Add_Info (size_t Pos, const char* Parameter, int64u Value, intu Radix=16) {Details_Add_Info(Pos, Parameter, Ztring::ToZtring(Value, Radix).MakeUpperCase()+_T(" (")+Ztring::ToZtring(Value, 10).MakeUpperCase()+_T(")"));}
    void Details_Add_Info (size_t Pos, const char* Parameter, int64s Value, intu Radix=16) {Details_Add_Info(Pos, Parameter, Ztring::ToZtring(Value, Radix).MakeUpperCase()+_T(" (")+Ztring::ToZtring(Value, 10).MakeUpperCase()+_T(")"));}
    void Details_Add_Info (size_t Pos, const char* Parameter, float32 Value, intu AfterComma=3) {Details_Add_Info(Pos, Parameter, Ztring::ToZtring(Value, AfterComma));}
    #if defined (MACOSX)
    void Details_Add_Info (size_t Pos, const char* Parameter, size_t Value, intu Radix=16) {Details_Add_Info(Pos, Parameter, Ztring::ToZtring(Value, Radix).MakeUpperCase());}
    #endif
    void Details_Add_Info (size_t Pos, const char* Parameter, double  Value, intu AfterComma=3) {Details_Add_Info(Pos, Parameter, Ztring::ToZtring(Value, AfterComma));}
    void Details_Add_Info (const Ztring &Parameter);
    void Details_Add_Info (const char* Parameter) {Details_Add_Info(Ztring().From_UTF8(Parameter));}
    void Details_Add_Error(const char* Parameter);
    void Details_Remove_Info ();
    void Details_Add_Info_Generic (Ztring &String, size_t Pos, const char* Parameter, const Ztring& Value);
    int8u Details_Level_Base;
    int8u Details_Level_Last;
    Ztring Details_Add_Element_Retain;
    Ztring Details_Add_Element_Info_Retain;
    float32 Details_Level_Min;

    //Some quick helpers
    inline void DETAILLEVEL_SET (float32 Value)     {Details_Level_Min=Value;}
    inline void NAME (const char* Parameter)        {if (Config.Details_Get()>=Details_Level_Min) Details_Add_Element(Parameter);}
    inline void INFO (const char* Parameter)        {if (Config.Details_Get()>=Details_Level_Min) Details_Add_Info(Parameter);}
    inline void INFO (const Ztring &Parameter)      {if (Config.Details_Get()>=Details_Level_Min) Details_Add_Info(Parameter);}
    inline void NOT_NEEDED ()                       {if (Config.Details_Get()>=Details_Level_Min) Details_Add_Info("(Not needed)");}
    inline void FLUSH ()                            {if (Config.Details_Get()) Details_Add_Element_Flush();}
    inline void PURGE ()                            {if (Config.Details_Get()) Details_Add_Element_Purge();}
    inline void TRUSTED_ISNOT (const char* Reason)  {if (Config.Details_Get()>=Details_Level_Min) Details_Add_Error(Reason);
                                                     Trusted--; if (Trusted==0) {Clear(); File_Offset=File_Size; }};

    //Debug
    bool   Synched;
    size_t Trusted;

    //Optimization
    Ztring            CompleteFileName; //Cached filename for Read()
    std::vector<bool> Optimized[Stream_Max]; //If true, Arrays are not fully filled
    stream_t StreamKind_Last;
    size_t   StreamPos_Last;

    //Utils
    size_t Merge(const File__Base &ToAdd); //Merge 2 File_Base
    size_t Merge(File__Base &ToAdd, stream_t StreamKind, size_t StreamPos_From, size_t StreamPos_To); //Merge 2 streams

    //Divers
    void Clear();

public :
    //Buffer
    int8u* Buffer;
    size_t Buffer_Size;
    size_t Buffer_Size_Max;
    size_t Buffer_Offset; //Temporary usage in this parser
    size_t Buffer_MinimumSize;
    size_t Buffer_MaximumSize;
    bool   Buffer_Init_Done;
    void   Buffer_Clear();
    void   Details_Clear();
    void   Details_Add(const char* Parameter);

    //File
    int64u File_Size;
    int64u File_Offset;
    int64u File_GoTo; //How many byte to skip?

    //End
    void Finalize();
    void Finalize__All      (stream_t StreamKind);
    void Finalize__All      (stream_t StreamKind, size_t Pos, Ztring &Codec_List, Ztring &Language_List);
    void Finalize_General   (size_t Pos);
    void Finalize_Video     (size_t Pos);
    void Finalize_Audio     (size_t Pos);
    void Finalize_Text      (size_t Pos);
    void Finalize_Chapters  (size_t Pos);
    void Finalize_Image     (size_t Pos);
    void Finalize_Menu      (size_t Pos);
    void Finalize_Tags      ();

public :  //A virer
    void Traiter(Ztring &C); //enleve les $if...
};

} //NameSpace

#endif
