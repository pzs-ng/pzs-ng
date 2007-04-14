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

//---------------------------------------------------------------------------
// Compilation condition
#include <MediaInfo/Setup.h>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/File__Base.h"
#ifdef __BORLAND__
    #include <mem.h>
#else
    #include <memory.h>
#endif
#include "wx/file.h"
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//---------------------------------------------------------------------------
extern MediaInfo_Config Config;
//---------------------------------------------------------------------------

//***************************************************************************
// Gestion de la classe
//***************************************************************************

//---------------------------------------------------------------------------
//Constructeurs
File__Base::File__Base ()
{
    //Init pointers
    Stream[Stream_General]=&General;
    Stream[Stream_Video]=&Video;
    Stream[Stream_Audio]=&Audio;
    Stream[Stream_Text]=&Text;
    Stream[Stream_Chapters]=&Chapters;
    Stream[Stream_Image]=&Image;
    Stream[Stream_Menu]=&Menu;

    //Buffer
    Buffer=NULL;
    Buffer_Size=0;
    Buffer_Size_Max=0;
    Buffer_Offset=0;
    Buffer_MinimumSize=0;
    Buffer_MaximumSize=1024*1024;
    Buffer_Init_Done=false;

    //Buffer
    File_Size=(int64u)-1;
    File_Offset=0;
    File_GoTo=0;

    //Optimization init
    StreamKind_Last=Stream_Max;
    StreamPos_Last=Error;

    //Details
    Details_Level_Base=0;
    Details_Level_Last=0;
    Details_Level_Min=0;

    //Debug
    Synched=false;
    Trusted=Error;
}

//---------------------------------------------------------------------------
//Constructeurs
File__Base::~File__Base ()
{
    //Buffer
    delete Buffer; Buffer=NULL;
}

//***************************************************************************
// Fonctions
//***************************************************************************

//---------------------------------------------------------------------------
int File__Base::Open_File (const Ztring &Fichier)
{
    Clear();
    CompleteFileName=Fichier;
    Read_File();
    if (Count_Get(Stream_General)>0)
    {
        Finalize();
        return 1;
    }
    else
        return 0;
}

void File__Base::Open_Buffer_Init (int64u File_Size_, int64u File_Offset_)
{
    //Demand to go elsewhere
    if (File_GoTo>File_Offset_ || File_Offset>=File_Size)
    {
        if (Config.Details_Get())
        {
            Details_Add_Info("(Not needed)");
        }
        Buffer_Clear();
        return; //Not needed
    }

    //File
    File_Size=File_Size_;
    File_Offset=File_Offset_;

    //Unsynched
    if (File_GoTo>0)
        Read_Buffer_Unsynched();
    File_GoTo=0;

    //Format
    if (!Buffer_Init_Done || File_Offset==0)
    {
        Buffer_Clear();
        Read_Buffer_Init();
        Buffer_Init_Done=true;
    }
}

void File__Base::Open_Buffer_Init (File__Base* Sub)
{
    if (Sub==NULL)
        Sub=this;

    Sub->Open_Buffer_Init(File_Size, File_Offset+Buffer_Offset);
}

void File__Base::Open_Buffer_Init (File__Base* Sub, int64u File_Size_, int64u File_Offset_)
{
    if (Sub==NULL)
        Sub=this;

    Sub->Open_Buffer_Init(File_Size_, File_Offset_);
}

void File__Base::Open_Buffer_Continue (const int8u* ToAdd, size_t ToAdd_Size)
{
    //Integrity
    if (ToAdd==NULL || ToAdd_Size==0)
        return;

    //The end
    if (File_Offset==File_Size)
    {
        if (Config.Details_Get())
        {
            Details_Add_Info("(Not needed)");
        }
        return;
    }

    //if (CompleteFileName.empty()) {wxFile F; F.Open((Ztring(_T("d:\\direct"))+Ztring::ToZtring((size_t)this, 16)/*+Ztring::ToZtring(File_Offset, 16)*/).c_str(), wxFile::write_append); F.Write(ToAdd, ToAdd_Size);}

    //Demand to go elsewhere
    size_t ToAdd_Offset=0;
    if (File_GoTo)
    {
        if (File_GoTo>File_Offset+ToAdd_Size)
        {
            if (Config.Details_Get())
            {
                Details_Add_Info("(Not needed)");
            }
            return;
        }

        //The needed offset is in the new buffer
        ToAdd_Offset=File_GoTo-File_Offset;
    }

    //Allocating Buffer
    if (Buffer_Size+ToAdd_Size-ToAdd_Offset>Buffer_Size_Max)
    {
        int8u* Old=Buffer;
        size_t Buffer_Size_Max_ToAdd=ToAdd_Size-ToAdd_Offset>32768?ToAdd_Size-ToAdd_Offset:32768;
        if (Buffer_Size_Max_ToAdd<Buffer_Size_Max) Buffer_Size_Max_ToAdd=Buffer_Size_Max;
        Buffer_Size_Max+=Buffer_Size_Max_ToAdd;
        Buffer=new int8u[Buffer_Size_Max];
        memcpy(Buffer, Old, Buffer_Size);
        delete[] Old; //No need to set to NULL
    }

    //Copying buffer
    memcpy(Buffer+Buffer_Size, ToAdd+ToAdd_Offset, ToAdd_Size-ToAdd_Offset);
    Buffer_Size+=ToAdd_Size-ToAdd_Offset;

    //Enough?
    if (Buffer_MinimumSize && File_Offset+Buffer_Size<File_Size && Buffer_Size<Buffer_MinimumSize)
        return; //We have not enough datas

    //Parsing
    Buffer_Offset=0;
    Trusted=Buffer_Size/1024;
    Read_Buffer_Continue();

    //Finnished?
    if(File_Offset+Buffer_Offset>=File_Size)
    {
        File_Offset=File_Size;
        Buffer_Clear();
        return;
    }

    //Demand to go elsewhere
    if (File_GoTo>0)
    {
        if (File_GoTo>=File_Size)
        {
            if (File_GoTo>File_Size)
                if (Config.Details_Get())
                {
                    Details_Add_Info(Error, "Want to go too far, we abandon", "");
                }
            File_Offset=File_Size;
        }
        Buffer_Clear();
        return;
    }
    if (Buffer_Offset>Buffer_Size)
    {
        File_GoTo=File_Offset+Buffer_Offset;
        Buffer_Clear();
        return;
    }

    //Reserving unused data
    memmove(Buffer, Buffer+Buffer_Offset, Buffer_Size-Buffer_Offset);
    Buffer_Size-=Buffer_Offset;
    File_Offset+=Buffer_Offset;
    Buffer_Offset=0;

    //Is it OK?
    if (Buffer_Size>Buffer_MaximumSize)
    {
        //Buffer too big, we abandon
        if (Config.Details_Get())
        {
            Details_Add_Info(Error, "Buffer too big, we abandon", "");
        }
        Buffer_Clear();
        File_Offset=File_Size;
        return;
    }
}

void File__Base::Open_Buffer_Continue (File__Base* Sub, const int8u* ToAdd, size_t ToAdd_Size)
{
    if (Sub==NULL)
        Sub=this;

    //Integrity
    if (ToAdd==NULL || ToAdd_Size==0)
        return;

    //Demand to go elsewhere
    if (Sub->File_GoTo || Sub->File_Offset==Sub->File_Size)
    {
        if (Config.Details_Get())
        {
            Details_Add_Info("(Not needed)");
        }
        return;
    }

    //Preparing Sub
    if (Sub!=this)
        Sub->Details_Level_Base=Details_Level_Base+Details_Level_Last+1;
    else
        Sub->Details_Level_Base=1;
    Sub->Details.clear();

    //Buffer?
    if (Sub->Buffer_Size<=Sub->File_Offset)
    {
        Sub->File_Offset-=Sub->Buffer_Size;
    }

    //Details
    size_t Buffer_Size_Before=Sub->Buffer_Size;

    //Parsing
    Sub->Open_Buffer_Continue(ToAdd, ToAdd_Size);

    //Details
    if (Config.Details_Get())
    {
        if (Sub!=this && !Sub->Details.empty())
        {
            Details_Add_Element_Flush();
            if (Buffer_Size_Before>0)
                Details_Add_Info(0, "Bytes from buffer", Buffer_Size_Before);
            if (!Details.empty())
                Details+=Config.LineSeparator_Get();
            Details+=Sub->Details;
            Sub->Details.clear();
            if (Sub->Buffer_Size>0)
                Details_Add_Info(ToAdd_Size-Sub->Buffer_Size, "Bytes buffered", Sub->Buffer_Size);
        }
        //else
        //    Details_Add_Element_Purge();
    }
}

void File__Base::Open_Buffer_Finalize ()
{
    //Format
    if (Config.Details_Get())
    {
        Details_Add_Element(0, _T("Finalizing"));
        FLUSH();
    }
    Read_Buffer_Finalize();

    //Buffer
    Buffer_Clear();
}

void File__Base::Open_Buffer_Finalize (File__Base* Sub)
{
    if (Sub==NULL)
        Sub=this;

    //Preparing Sub
    if (Sub!=this)
        Sub->Details_Level_Base=Details_Level_Base+Details_Level_Last+1;
    else
        Sub->Details_Level_Base=1;
    Sub->Details.clear();

    //Finalize
    Sub->Open_Buffer_Finalize();

    //Details
    if (!Sub->Details.empty())
    {
        Details+=Config.LineSeparator_Get();
        Details+=Sub->Details;
        Sub->Details.clear();
    }
}

//---------------------------------------------------------------------------
size_t File__Base::Count_Get (stream_t StreamKind, size_t Pos) const
{
    //Integrity
    if (StreamKind>=Stream_Max)
        return 0;

    //Count of streams
    if (Pos==Error)
        return (*Stream[StreamKind]).size();

    //Integrity
    if (Pos>=(*Stream[StreamKind]).size())
        return 0;

    //Count of piece of information in a stream
    return (*Stream[StreamKind])[Pos].size();
}

//---------------------------------------------------------------------------
const Ztring &File__Base::Get (stream_t StreamKind, size_t StreamNumber, size_t Parameter, info_t KindOfInfo)
{
    //Check integrity
    if (StreamKind>=Stream_Max || StreamNumber>=(*Stream[StreamKind]).size() || Parameter>=(*Stream[StreamKind])[StreamNumber].size() || KindOfInfo>=Info_Max)
        return Config.EmptyString_Get(); //Parameter is unknown

    //OK for Optimization?
    if (Optimized[StreamKind][StreamNumber] && (*Stream[StreamKind])[StreamNumber].size()-Config.Info_Get(StreamKind).size()!=0)
    {
        //Can't be optimized
        /*TEST
        Ztring A=Audio[0](_T("Video0_Delay"), Info_Options);
        A.clear();
        */
        for (size_t Pos=0; Pos<(*Stream[StreamKind])[StreamNumber].size(); Pos++)
        {
            size_t Pos_Info=Config.Info_Get(StreamKind).Find((*Stream[StreamKind])[StreamNumber][Pos][0]);
            if (Pos_Info!=Error)
                for (size_t Pos1=Config.Info_Get(StreamKind)[Pos_Info].size()-1; Pos1>=Info_Measure; Pos1--) //-- for optimization of ZtringList
                {
                    /*TEST if (Pos1!=Info_Options)  */
                         (*Stream[StreamKind])[StreamNumber][Pos](Pos1)=Config.Info_Get(StreamKind)[Pos_Info][Pos1];
                }
        }
        /*TEST
        A=Audio[0](_T("Video0_Delay"), Info_Options);
        A.clear();
        */
        Optimized[StreamKind][StreamNumber]=false;
    }

    /*TEST
    if (Count_Get(Stream_Audio)>0 && StreamKind==Stream_Audio && Audio[0](Parameter, 0)==_T("Video0_Delay"))
    {
        Ztring A=Audio[0](_T("Video0_Delay"), Info_Options);
        A.clear();
    }
    */
    //Verify validity of strings
    //-Info_Options
     if (!Optimized[StreamKind][StreamNumber] && KindOfInfo==Info_Options && (*Stream[StreamKind])[StreamNumber](Parameter, Info_Options).empty())
        (*Stream[StreamKind])[StreamNumber](Parameter, Info_Options)=_T("Y YT");

    //Optimization : KindOfInfo>Info_Text is in static lists
    if (KindOfInfo!=Info_HowTo && Optimized[StreamKind][StreamNumber] && KindOfInfo>=Info_Measure && (size_t)KindOfInfo<Config.Info_Get(StreamKind)[Parameter].size())
        return Config.Info_Get(StreamKind)[Parameter][KindOfInfo]; //look for static information only
    else
        return (*Stream[StreamKind])[StreamNumber][Parameter](KindOfInfo);
}

//---------------------------------------------------------------------------
const Ztring &File__Base::Get (stream_t StreamKind, size_t StreamNumber, const Ztring &Parameter, info_t KindOfInfo, info_t KindOfSearch)
{
    size_t ParameterI=0;

    //Legacy
    if (Parameter.find(_T("_String"))!=Error)
    {
        Ztring S1=Parameter;
        S1.FindAndReplace(_T("_String"), _T("/String"));
        return Get(StreamKind, StreamNumber, S1, KindOfInfo, KindOfSearch);
    }
    if (Parameter==_T("Channels"))
        return Get(StreamKind, StreamNumber, _T("Channel(s)"), KindOfInfo, KindOfSearch);
    if (Parameter==_T("Artist"))
        return Get(StreamKind, StreamNumber, _T("Performer"), KindOfInfo, KindOfSearch);

    //Check integrity
    if (StreamKind>=Stream_Max || StreamNumber>=(*Stream[StreamKind]).size() || (ParameterI=(*Stream[StreamKind])[StreamNumber].Find(Parameter, KindOfSearch))==Error || KindOfInfo>=Info_Max)
        return Config.EmptyString_Get(); //Parameter is unknown

    //Special cases
    //-Inform for a stream
    if (Parameter==_T("Inform"))
        (*Stream[StreamKind])[StreamNumber](_T("Inform"))=Inform(StreamKind, StreamNumber);

    return Get(StreamKind, StreamNumber, ParameterI, KindOfInfo);
}

//---------------------------------------------------------------------------
int File__Base::Set (stream_t StreamKind, size_t StreamNumber, size_t Parameter, const Ztring &ToSet, const Ztring &OldValue)
{
    //Integrity
    while (Count_Get(StreamKind)<=StreamNumber)
        Stream_Prepare(StreamKind);

    return Set(StreamKind, StreamNumber, Get(StreamKind, StreamNumber, Parameter, Info_Name), ToSet, OldValue);
}

//---------------------------------------------------------------------------
int File__Base::Set (stream_t StreamKind, size_t StreamNumber, const Ztring &Parameter, const Ztring &ToSet, const Ztring &OldValue)
{
    //Integrity
    while (Count_Get(StreamKind)<=StreamNumber)
        Stream_Prepare(StreamKind);

    //Fill(StreamKind, StreamNumber, Parameter.To_Local().c_str(), ToSet);

    return Write(StreamKind, StreamNumber, Parameter, ToSet, OldValue);
}

//---------------------------------------------------------------------------
ZtringListList File__Base::Info_Capacities()
{
    //Ask to format parser to fill information
    Clear();

    //Fill
    ZtringListList ToReturn;
    size_t ToReturn_Pos;
    for (size_t StreamKind=0; StreamKind<Stream_Max; StreamKind++)
    {
        HowTo((stream_t)StreamKind);
        ToReturn_Pos=0;
        for (size_t Pos=0; Pos<Config.Info_Get((stream_t)StreamKind).size(); Pos++)
            if (Pos<Config.Info_Get((stream_t)StreamKind).size() && ((size_t)Info_Options)<Config.Info_Get((stream_t)StreamKind)[Pos].size() && Config.Info_Get((stream_t)StreamKind)[Pos][Info_Options][InfoOption_ShowInSupported]==_T('Y'))
            {
                ToReturn(StreamKind, ToReturn_Pos)=Get((stream_t)StreamKind, 0, Pos, Info_HowTo);
                ToReturn_Pos++;
            }
    }

    return ToReturn;
}

//---------------------------------------------------------------------------
ZtringListList File__Base::Info_Capacities_Parameters()
{
    //Fill
    ZtringListList ToReturn;
    size_t ToReturn_Pos;
    for (size_t StreamKind=0; StreamKind<Stream_Max; StreamKind++)
    {
        ToReturn_Pos=0;
        for (size_t Pos=0; Pos<Config.Info_Get((stream_t)StreamKind).size(); Pos++)
            if (Pos<Config.Info_Get((stream_t)StreamKind).size() && ((size_t)Info_Options)<Config.Info_Get((stream_t)StreamKind)[Pos].size() && Config.Info_Get((stream_t)StreamKind)[Pos][Info_Options][InfoOption_ShowInSupported]==_T('Y'))
            {
                ToReturn(StreamKind, ToReturn_Pos)=Config.Info_Get((stream_t)StreamKind)[Pos][Info_Name];
                ToReturn_Pos++;
            }
    }

    return ToReturn;
}

//---------------------------------------------------------------------------
void File__Base::Language_Set()
{
/*
    for (size_t StreamKind=(size_t)Stream_General; StreamKind<(size_t)Stream_Max; StreamKind++)//Note : Optimisation, only the first Stream is, so StreamNumber is only 0
        for (size_t Pos=0; Pos<Config.Info[StreamKind].size(); Pos++)
        {
             //Info_Name_Text
             const Ztring &Z1=Config.Language_Get(Config.Info_Get((stream_t)StreamKind, Pos, Info_Name));
             if (Z1.empty())
                Set((stream_t)StreamKind, 0, Pos, Info_Name_Text, Config.Info_Get((stream_t) StreamKind, Pos, Info_Name));
             else
                Set((stream_t)StreamKind, 0, Pos, Info_Name_Text, Z1);
             //Info_Measure_Text
             const Ztring Z2=Config.Language_Get(Config.Info_Get((stream_t)StreamKind, Pos, Info_Measure));
             if (Z2.empty())
                Set((stream_t)StreamKind, 0, Pos, Info_Measure_Text, Config.Info_Get((stream_t)StreamKind, Pos, Info_Measure);
             else
                Set((stream_t)StreamKind, 0, Pos, Info_Measure_Text, Z2);
        }
*/
}

//***************************************************************************
// Preparation des streams
//***************************************************************************

//---------------------------------------------------------------------------
size_t File__Base::Stream_Prepare (stream_t KindOfStream)
{
    //Integrity
    if (KindOfStream>Stream_Max)
        return Error;

    //Clear
    if (KindOfStream==Stream_Max)
    {
        StreamKind_Last=Stream_Max;
        StreamPos_Last=(size_t)-1;
        return 0;
    }

    //Add a stream
    Stream[KindOfStream]->resize(Stream[KindOfStream]->size()+1);
    Optimized[KindOfStream].push_back(true);

    //Fill the stream
    ZtringListList* ZLL=&(*Stream[KindOfStream])[Stream[KindOfStream]->size()-1];
    ZLL->reserve(Config.Info_Get(KindOfStream).size());
    for (size_t Pos=0; Pos<Config.Info_Get(KindOfStream).size(); Pos++)
    {
        (*ZLL)(Pos, Info_Name)      =Config.Info_Get(KindOfStream)[Pos][Info_Name];
        /*TEST (*ZLL)(Pos, Info_Options)   =Config.Info_Get(KindOfStream)(Pos, Info_Options);*/
        //Note: if you add a stream here, don't forget to put an exception in ::Get()
    }
    ZLL->Write(Config.Language_Get(Config.Info_Get(KindOfStream, 2, Info_Text)), 2, Info_Text);

    //Special cases
    if (KindOfStream==Stream_General)
        General_Fill();
    else
        ZLL->Write(Ztring::ToZtring(Stream[KindOfStream]->size()), 3, Info_Text);

    StreamKind_Last=KindOfStream;
    StreamPos_Last=Stream[KindOfStream]->size()-1;

    //Fill with already ready data
    for (size_t Pos=0; Pos<Fill_Temp.size(); Pos++)
        Fill(Fill_Temp(Pos, 0).To_UTF8().c_str(), Fill_Temp(Pos, 1));
    Fill_Temp.clear();

    return Stream[KindOfStream]->size()-1; //The position in the stream count
}

//***************************************************************************
// Filling
//***************************************************************************

//---------------------------------------------------------------------------
void File__Base::Fill (stream_t StreamKind, size_t StreamPos, const char* Parameter, const Ztring &Value, bool Replace)
{
    //Fill details
    if (!Details.empty() && !Value.empty())
        Details_Add_Info(Error, (std::string("--> ")+Parameter).c_str(), Value);

    //Handle Value before StreamKind
    if (StreamKind==Stream_Max || StreamPos>=(*Stream[StreamKind]).size())
    {
        ZtringList NewList;
        NewList.push_back(Ztring().From_UTF8(Parameter));
        NewList.push_back(Value);
        Fill_Temp.push_back(NewList);
        return; //No streams
    }

    Ztring &Target=(*Stream[StreamKind])[StreamPos](Ztring().From_Local(Parameter));
    if (Target.empty() || Replace)
        Target=Value; //First value
    else if (Value.empty())
        Target.clear(); //Empty value --> clear other values
    else
    {
        Target+=Config.TagSeparator_Get();
        Target+=Value;
    }
}

//---------------------------------------------------------------------------
void File__Base::Fill_HowTo (stream_t StreamKind, size_t StreamPos, const char* Parameter, const char* Value)
{
    Ztring Param; Param.From_UTF8(Parameter);

    //Test if stream is already created
    while (Count_Get(StreamKind)<=StreamPos)
        Stream_Prepare(StreamKind);

    //Test if parameter exists
    if ((*Stream[StreamKind])[StreamPos].Find(Param)==Error)
        return;

   (*Stream[StreamKind])[StreamPos](Param, Info_HowTo).From_UTF8(Value);
}

//---------------------------------------------------------------------------
void File__Base::Fill_Flush()
{
    Stream_Prepare(Stream_Max); //clear filling
    Fill_Temp.clear();
}

//***************************************************************************
// Details
//***************************************************************************

//---------------------------------------------------------------------------
Ztring Details_Offset (int64u OffsetToShow)
{
    int64u Offset=OffsetToShow%0x100000000ULL; //Only 32 bits
    Ztring Pos1; Pos1.From_Number(Offset, 16);
    Ztring Pos2;
    Pos2.resize(8-Pos1.size(), _T('0'));
    Pos2+=Pos1;
    Pos2.MakeUpperCase();
    Pos2+=_T(' ');
    return Pos2;
}

//---------------------------------------------------------------------------
void File__Base::Details_Add_Info_Generic (Ztring &String, size_t Pos, const char* Parameter, const Ztring& Value)
{
    const size_t Padding_Value=40;

    //Line separator
    if (!String.empty())
        String+=Config.LineSeparator_Get();

    //Show Offset
    if (Config.Details_Get()>0.7)
    {
        if (Pos!=Error)
            String+=Details_Offset(File_Offset+Buffer_Offset+Pos);
        else
            String+=_T("         ");
    }

    //Show Element
    Ztring Param; Param.From_Local(Parameter);
    if (Param.size()>Padding_Value) Param.resize(Padding_Value);
    if (Details_Level_Last<16)
        String.resize(String.size()+Details_Level_Base+Details_Level_Last+1, _T(' '));
    else
        String.resize(String.size()+Details_Level_Base+1, _T(' '));
    String+=Param;

    //Show Value
    if (!Value.empty())
    {
        String+=_T(": ");
        if (Details_Level_Last<16)
            String.resize(String.size()+Padding_Value-Param.size()-Details_Level_Base-Details_Level_Last, _T(' '));
        else
            String.resize(String.size()+Padding_Value-Param.size()-Details_Level_Base, _T(' '));
        String+=Value;
    }

    //{FILE* F=_wfopen((Ztring(_T("d:\\Details"))+Ztring::ToZtring((size_t)this, 16)).c_str(), _T("wb")); fwrite(Details.c_str(), Details.size()*2, 1, F); fclose(F);}
}

//---------------------------------------------------------------------------
void File__Base::Details_Add_Element (int8u Level, const Ztring &Text, size_t Size)
{
    //Coherancy
    if (Config.Details_Get()==0 || Details.size()>1024*1024)
        return;

    //Init
    Details_Level_Last=Level;

    //Show Offset
    if (Config.Details_Get()>0.7)
    {
        Details_Add_Element_Retain+=Details_Offset(File_Offset+Buffer_Offset);
    }

    //Filing
    Details_Add_Element_Retain.resize(Details_Add_Element_Retain.size()+Details_Level_Base+Details_Level_Last, _T(' '));
    Details_Add_Element_Retain+=Text;

    //Size
    if (Size!=Error && Config.Details_Get()>0.3)
    {
        Details_Add_Element_Retain+=_T(" (");
        Details_Add_Element_Retain+=Ztring::ToZtring(Size);
        Details_Add_Element_Retain+=_T(" bytes)");
    }
}

//---------------------------------------------------------------------------
void File__Base::Details_Add_Element (int8u Level, int8u Text, size_t Size)
{
    //Element Name
    Ztring Pos1; Pos1.From_Number(Text, 16);
    Ztring Pos2;
    Pos2.resize(2-Pos1.size(), _T('0'));
    Pos2+=Pos1;
    Pos2.MakeUpperCase();

    Details_Add_Element(Level, Pos2, Size);
}

//---------------------------------------------------------------------------
void File__Base::Details_Add_Element (int8u Level, int16u Text, size_t Size)
{
    //Element Name
    Ztring Pos1; Pos1.From_Number(Text, 16);
    Ztring Pos2;
    Pos2.resize(4-Pos1.size(), _T('0'));
    Pos2+=Pos1;
    Pos2.MakeUpperCase();

    Details_Add_Element(Level, Pos2, Size);
}

//---------------------------------------------------------------------------
void File__Base::Details_Add_Element (int8u Level, int32u Text, size_t Size)
{
    //Element Name
    Ztring Pos1; Pos1.From_Number(Text, 16);
    Ztring Pos2;
    Pos2.resize(8-Pos1.size(), _T('0'));
    Pos2+=Pos1;
    Pos2.MakeUpperCase();

    Details_Add_Element(Level, Pos2, Size);
}

//---------------------------------------------------------------------------
void File__Base::Details_Add_Element (const Ztring &Text)
{
    //Coherancy
    if (Config.Details_Get()==0 || File_Offset==File_Size || Details.size()>1024*1024)
        return;

    //Needed?
    if (Config.Details_Get()<=0.7)
        return;

    Details_Add_Element_Retain+=Ztring(_T(" - "))+Text;
}

//---------------------------------------------------------------------------
void File__Base::Details_Add_Element_Info (size_t Pos, const char* Parameter, const Ztring& Value)
{
    //Coherancy
    if (Config.Details_Get()==0 || File_Offset==File_Size || Details.size()>1024*1024)
        return;

    //Needed?
    if (Config.Details_Get()<1)
        return;

    //Filling
    Details_Add_Info_Generic(Details_Add_Element_Info_Retain, Pos, Parameter, Value);
}

//---------------------------------------------------------------------------
void File__Base::Details_Add_Element_Flush ()
{
    if (!Details_Add_Element_Retain.empty())
    {
        if (!Details.empty())
            Details+=Config.LineSeparator_Get();
        Details+=Details_Add_Element_Retain;
        Details_Add_Element_Retain.clear();
        if (!Details_Add_Element_Info_Retain.empty())
        {
            if (!Details.empty())
                Details+=Config.LineSeparator_Get();
            Details+=Details_Add_Element_Info_Retain;
        }
    }
    Details_Add_Element_Info_Retain.clear();
}

//---------------------------------------------------------------------------
void File__Base::Details_Add_Element_Purge ()
{
    Details_Add_Element_Retain.clear();
    Details_Add_Element_Info_Retain.clear();
}

//---------------------------------------------------------------------------
void File__Base::Details_Add_Info (size_t Pos, const char* Parameter, const Ztring& Value)
{
    //Coherancy
    if (Config.Details_Get()==0 || File_Offset==File_Size || Details.size()>1024*1024)
        return;

    //Needed?
    if (Config.Details_Get()<=0.7)
        return;

    //Elements
    Details_Add_Element_Flush();

    //Filling
    Details_Add_Info_Generic(Details, Pos, Parameter, Value);

    //{wxFile F; F.Open((Ztring(_T("d:\\Details"))+Ztring::ToZtring((size_t)this, 16)).c_str(), wxFile::write); F.Write(Details.c_str(), Details.size()*sizeof(Char));}
}

//---------------------------------------------------------------------------
void File__Base::Details_Add_Info (const Ztring &Text)
{
    //Coherancy
    if (Config.Details_Get()==0 || File_Offset==File_Size || Details.size()>1024*1024)
        return;

    //Needed?
    if (Config.Details_Get()<=0.7)
        return;

    //Elements
    Details_Add_Element_Flush();

    //Filling
    Details+=_T(" - ")+Text;
}

//---------------------------------------------------------------------------
void File__Base::Details_Add_Error (const char* Parameter)
{
    //Coherancy
    if (Config.Details_Get()==0 || File_Offset==File_Size || Details.size()>1024*1024)
        return;

    if (Config.Details_Get()<=0.7)
        return;

    //Elements
    Details_Add_Element_Flush();

    //Filling
    Details_Add_Element(Details_Level_Last, Parameter, Error);
    FLUSH();
}

//---------------------------------------------------------------------------
void File__Base::Details_Clear ()
{
    Details.clear();
}

//---------------------------------------------------------------------------
void File__Base::Details_Add (const char* Parameter)
{
    Details+=Ztring().From_UTF8(Parameter);
}

//---------------------------------------------------------------------------
void File__Base::Details_Remove_Info ()
{

    size_t Pos=Details.rfind(Config.LineSeparator_Get());
    if (Pos!=Error)
        Details.resize(Pos);
}

//***************************************************************************
// Divers
//***************************************************************************

void File__Base::Clear()
{
    for (size_t StreamKind=0; StreamKind<Stream_Max; StreamKind++)
    {
        (*Stream[StreamKind]).clear();
        Optimized[StreamKind].clear();
    }
}

//---------------------------------------------------------------------------
void File__Base::Read_File()
{
}

//---------------------------------------------------------------------------
void File__Base::Read_Buffer_Init()
{
}

//---------------------------------------------------------------------------
void File__Base::Read_Buffer_Unsynched()
{
    Synched=false;
}

//---------------------------------------------------------------------------
void File__Base::Read_Buffer_Continue()
{
    File_Offset=File_Size;
}

//---------------------------------------------------------------------------
void File__Base::Read_Buffer_Finalize()
{
}

//---------------------------------------------------------------------------
int File__Base::Write(stream_t, size_t, const Ztring &, const Ztring &, const Ztring &)
{
    return -1;
}

int File__Base::WriteToDisk()
{
    return -1;
}

//---------------------------------------------------------------------------
void File__Base::HowTo(stream_t)
{
    return;
}

//---------------------------------------------------------------------------
size_t File__Base::Merge(const File__Base &ToAdd)
{
    size_t Count=0;
    for (size_t StreamKind=(size_t)Stream_General+1; StreamKind<(size_t)Stream_Max; StreamKind++)
        for (size_t StreamPos=0; StreamPos<ToAdd.Stream[StreamKind]->size(); StreamPos++)
        {
            //Prepare a new stream
            Stream_Prepare((stream_t)StreamKind);

            //Merge
            Stream[StreamKind]->at(StreamPos_Last)=ToAdd.Stream[StreamKind]->at(StreamPos);
            Count++;
        }
    return Count;
}

//---------------------------------------------------------------------------
size_t File__Base::Merge(File__Base &ToAdd, stream_t StreamKind, size_t StreamPos_From, size_t StreamPos_To)
{
    //Integrity
    if (&ToAdd==NULL || StreamPos_From>=ToAdd.Stream[StreamKind]->size())
        return 0;

    //Destination
    while (StreamPos_To>=Stream[StreamKind]->size())
        Stream_Prepare(StreamKind);

    //Merging
    size_t Count=0;
    for (size_t Pos=0; Pos<ToAdd.Stream[StreamKind]->at(StreamPos_From).size(); Pos++)
    {
        Ztring ToFill_Name=ToAdd.Get(StreamKind, StreamPos_From, Pos, Info_Name);
        Ztring ToFill_Value=ToAdd.Get(StreamKind, StreamPos_From, Pos);
        if (!ToFill_Value.empty())
        {
            //Ztring &Target=Get(StreamKind, StreamPos_To, Pos);
            //Target=ToFill_Value;
            (Stream[StreamKind]->at(StreamPos_To))(ToFill_Name)=ToFill_Value;
            //Fill(StreamKind, StreamPos_To, ToFill_Name.To_Local().c_str(), ToFill_Value);
            Count++;
        }
    }

    return 1;
}

//---------------------------------------------------------------------------
void File__Base::Buffer_Clear()
{
    //Buffer
    delete Buffer; Buffer=NULL;
    Buffer_Size=0;
    Buffer_Size_Max=0;
    Buffer_Offset=0;
    Buffer_MinimumSize=0;
}

} //NameSpace


