// MediaInfoList - A list of MediaInfo
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
#include <wx/wxprec.h>
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#if defined(__BORLANDC__) && defined (_DEBUG)
    //Why? in Debug mode with release Wx Libs, wxAssert is not defined?
    void wxAssert (int, const wchar_t*, int, const wchar_t*, const wchar_t*){return;}
    void wxAssert (int, const char*, int, const char*, const char*){return;}
#endif
#include <wx/dir.h>
#include <wx/filename.h>
#include <ZenLib/ZtringListList.h>
#include "MediaInfoList.h"
using namespace ZenLib;
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
MediaInfoList::MediaInfoList(unsigned int Count_Init)
{
    //Initialisation
    Info.reserve(Count_Init);
    for (size_t Pos=0; Pos<Info.size(); Pos++)
        Info[Pos]=NULL;
}

//---------------------------------------------------------------------------
//Destructeur
MediaInfoList::~MediaInfoList()
{
    Close(Error);
}

//***************************************************************************
// Fichiers
//***************************************************************************

//---------------------------------------------------------------------------
int MediaInfoList::Open(const MediaInfo_String &File, const fileoptions_t Options)
{
    int CountValid=0; //Count of valid files

    //Option FileOption_Close
    if (Options & FileOption_CloseAll)
        Close(All);

    //Option Recursive
    int Flags;
    //if (Options & FileOption_Recursive)
        Flags=wxDIR_FILES | wxDIR_DIRS;
    //else
    //    Flags=wxDIR_FILES;

    //Search for files
    wxArrayString Liste;
    wxFileName FullPath; FullPath=File.c_str();
    //-File
    if (FullPath.FileExists())
    {
        FullPath.Normalize();
        Liste.Add(FullPath.GetFullPath());
    }
    //-Directory
    else if (FullPath.DirExists())
    {
        FullPath.Normalize();
        wxDir::GetAllFiles(FullPath.GetFullPath(), &Liste, _T(""), Flags);
    }
    //-WildCards
    else
    {
        wxString FileName=FullPath.GetFullName();
        FullPath.SetFullName(_T("")); //Supress filename
        FullPath.Normalize();
        if (FullPath.DirExists())
            wxDir::GetAllFiles(FullPath.GetPath(), &Liste, FileName, Flags);
    }

    //Initialiser un nouveau info avec l'extension
    size_t Size_Before=Info.size();
    Info.resize(Size_Before+Liste.GetCount());
    for (size_t Pos=0; Pos<Liste.GetCount(); Pos++)
    {
        Info[Size_Before+Pos]=new MediaInfo();
        Info[Size_Before+Pos]->Open(Liste[Pos].c_str());
        CountValid++;
    }
    return CountValid;
}

//---------------------------------------------------------------------------
int MediaInfoList::Open (const int8u* Begin, size_t Begin_Size, const int8u* End, size_t End_Size, int64u FileSize)
{
	Info.resize(Info.size()+1);
    Info[Info.size()-1]=new MediaInfo();
    Info[Info.size()-1]->Open(Begin, Begin_Size, End, End_Size, FileSize);
    return 1;
}

//---------------------------------------------------------------------------
int MediaInfoList::Save(unsigned int)
{
    return 0; //Not yet implemented
}

//---------------------------------------------------------------------------
//int MediaInfoList::Save_All()
//{
//    return 0; //Not yet implemented
//}

//---------------------------------------------------------------------------
void MediaInfoList::Close(unsigned int FilePos)
{
    if (FilePos==Error)
    {
        for (size_t Pos=0; Pos<Info.size(); Pos++)
        {
            delete Info[Pos]; Info[Pos]=NULL;
        }
        Info.clear();
    }
    else if (FilePos<Info.size())
    {
        delete Info[FilePos]; Info[FilePos]=NULL;
        Info.erase(Info.begin()+FilePos);
    }
}

//---------------------------------------------------------------------------
//void MediaInfoList::Close_All()
//{
//}

//***************************************************************************
// Get File info
//***************************************************************************

//---------------------------------------------------------------------------
MediaInfo_String MediaInfoList::Inform(unsigned int FilePos, size_t)
{
    if (FilePos==Error)
    {
        Ztring Retour;
        unsigned int FilePos=0;
        ZtringListList MediaInfo_Custom_View; MediaInfo_Custom_View.Write(Option(_T("Inform_Get")));
        Retour+=MediaInfo_Custom_View(Stream_Max+2, 1);//Page_Begin
        while (FilePos<Info.size())
        {
            Retour+=Inform(FilePos);
            if (FilePos<Info.size()-1)
                Retour+=MediaInfo_Custom_View(Stream_Max+3, 1);//Page_Middle
            FilePos++;
        }
        Retour+=MediaInfo_Custom_View(Stream_Max+4, 1);//Page_End
        //Retour.FindAndReplace(_T("\\n"),_T( "\n"), 0, Ztring_Recursive);
        return Retour.c_str();
    }

    if (FilePos>=Info.size() || Info[FilePos]==NULL || Info[FilePos]->Count_Get(Stream_General)==0)
        return Config.EmptyString_Get();

    return Info[FilePos]->Inform();
}

//---------------------------------------------------------------------------
MediaInfo_String MediaInfoList::Get(unsigned int FilePos, stream_t KindOfStream, unsigned int StreamNumber, unsigned int Parameter, info_t KindOfInfo)
{
    if (FilePos==Error || FilePos>=Info.size() || Info[FilePos]==NULL || Info[FilePos]->Count_Get(Stream_General)==0)
        return Config.EmptyString_Get();

    return Info[FilePos]->Get(KindOfStream, StreamNumber, Parameter, KindOfInfo);
}

//---------------------------------------------------------------------------
MediaInfo_String MediaInfoList::Get(unsigned int FilePos, stream_t KindOfStream, unsigned int StreamNumber, const MediaInfo_String &Parameter, info_t KindOfInfo, info_t KindOfSearch)
{
    //TRACE(Trace+=_T("Get(L), CompleteName=");Trace+=Info[FilePos].Get(Stream_General, 0, _T("CompleteName")).c_str();)
    //TRACE(Trace+=_T("Get(L), StreamKind=");Trace+=ZenLib::Ztring::ToZtring((int8u)KindOfStream);Trace+=_T(", StreamNumber=");Trace+=ZenLib::Ztring::ToZtring((int8u)StreamNumber);Trace+=_T(", Parameter=");Trace+=ZenLib::Ztring(Parameter);Trace+=_T(", KindOfInfo=");Trace+=ZenLib::Ztring::ToZtring((int8u)KindOfInfo);Trace+=_T(", KindOfSearch=");Trace+=ZenLib::Ztring::ToZtring((int8u)KindOfSearch);)
    //TRACE(Trace+=_T("Get(L), will return ");Trace+=Info[FilePos].Get(KindOfStream, StreamNumber, Parameter, KindOfInfo, KindOfSearch).c_str();)

    if (FilePos==Error || FilePos>=Info.size() || Info[FilePos]==NULL || Info[FilePos]->Count_Get(Stream_General)==0)
        return Config.EmptyString_Get();

    return Info[FilePos]->Get(KindOfStream, StreamNumber, Parameter, KindOfInfo, KindOfSearch);
}

//***************************************************************************
// Set File info
//***************************************************************************

//---------------------------------------------------------------------------
int MediaInfoList::Set(const MediaInfo_String &ToSet, unsigned int FilePos, stream_t StreamKind, unsigned int StreamNumber, unsigned int Parameter, const MediaInfo_String &OldValue)
{
    if (FilePos==(unsigned int)-1)
        FilePos=0; //TODO : average

    if (FilePos>=Info.size() || Info[FilePos]==NULL || Info[FilePos]->Count_Get(Stream_General)==0)
        return -1;

    return Info[FilePos]->Set(ToSet, StreamKind, StreamNumber, Parameter, OldValue);
}

//---------------------------------------------------------------------------
int MediaInfoList::Set(const MediaInfo_String &ToSet, unsigned int FilePos, stream_t StreamKind, unsigned int StreamNumber, const MediaInfo_String &Parameter, const MediaInfo_String &OldValue)
{
    if (FilePos==(unsigned int)-1)
        FilePos=0; //TODO : average

    if (FilePos>=Info.size() || Info[FilePos]==NULL || Info[FilePos]->Count_Get(Stream_General)==0)
        return -1;

    return Info[FilePos]->Set(ToSet, StreamKind, StreamNumber, Parameter, OldValue);
}

//***************************************************************************
// Information
//***************************************************************************

//---------------------------------------------------------------------------
MediaInfo_String MediaInfoList::Option (const MediaInfo_String &Option, const MediaInfo_String &Value)
{
         if (Option==_T(""))
        return _T("");
    else if (Option==_T("Language_Update"))
    {
        //Special case : Language_Update must update all MediaInfo classes
        for (unsigned int Pos=0; Pos<Info.size(); Pos++)
            if (Info[Pos])
                Info[Pos]->Option(_T("Language_Update"), Value);

        return _T("");
    }
    else if (Option==_T("Create_Dummy"))
    {
        Info.resize(Info.size()+1);
        Info[Info.size()-1]=new MediaInfo();
        Info[Info.size()-1]->Option(Option, Value);
        return _T("");
    }
    else
        return MediaInfo::Option_Static(Option, Value);

}

//---------------------------------------------------------------------------
MediaInfo_String MediaInfoList::Option_Static (const MediaInfo_String &Option, const MediaInfo_String &Value)
{
    return MediaInfo::Option_Static(Option, Value);
}

//---------------------------------------------------------------------------
int MediaInfoList::State_Get()
{
    return 0; //Not yet implemented
}

//---------------------------------------------------------------------------
int  MediaInfoList::Count_Get (unsigned int FilePos, stream_t StreamKind, unsigned int StreamNumber)
{
    if (FilePos>=Info.size() || Info[FilePos]==NULL)
        return 0;

    return Info[FilePos]->Count_Get(StreamKind, StreamNumber);
}

//---------------------------------------------------------------------------
int MediaInfoList::Count_Get()
{
    return Info.size();
}

} //NameSpace

