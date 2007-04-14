// File_Ape - Info for Ape files
// Copyright (C) 2003-2007 Jasper van de Gronde, th.v.d.gronde@hccnet.nl
// Copyright (C) 2003-2007 Jerome Martinez, zen@mediaarea.net
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
#if defined(MEDIAINFO_APE_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Audio/File_Ape.h"
#include <MACLib/All.h>
#include <MACLib/MACLib.h>
#include <MACLib/APETag.h>
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Format
//***************************************************************************

//---------------------------------------------------------------------------
void File_Ape::Read_File()
{
    //We want to only parse .ape/.mac file, because mpcdec library crashes on some files (example : some BIOS.MAC files)
    if (CompleteFileName.size()<4 || (CompleteFileName.rfind(_T(".ape"))!=CompleteFileName.size()-4 && CompleteFileName.rfind(_T(".mac"))!=CompleteFileName.size()-4))
        return;

    //Open the file
    int nRetVal;
    IAPEDecompress *pAPEDecompress;
    try{
    pAPEDecompress=CreateIAPEDecompress(CompleteFileName.To_Unicode().c_str(), &nRetVal);
    }
    catch (...)
    {
        return;
    }
    if (nRetVal!=ERROR_SUCCESS)
        return;

    //Filling
    Stream_Prepare(Stream_General);
    Fill("Format", "APE");
    Fill("PlayTime", pAPEDecompress->GetInfo(APE_INFO_LENGTH_MS));
    Stream_Prepare(Stream_Audio);
    Fill("Codec", "APE");
    Fill("Codec_Settings", pAPEDecompress->GetInfo(APE_INFO_COMPRESSION_LEVEL));
    Fill("SamplingRate", pAPEDecompress->GetInfo(APE_INFO_SAMPLE_RATE));
    Fill("Channel(s)", pAPEDecompress->GetInfo(APE_INFO_CHANNELS));
    Fill("Resolution", pAPEDecompress->GetInfo(APE_INFO_BITS_PER_SAMPLE));
    Fill("FrameCount", pAPEDecompress->GetInfo(APE_INFO_TOTAL_FRAMES));
    //Fill("BitRate", pAPEDecompress->GetInfo(APE_INFO_AVERAGE_BITRATE); //Average bitrate indicated by Ape seems false
    Fill("BitRate_Mode", "VBR");

    //Tags
    CAPETag *pAPETag=(CAPETag *)pAPEDecompress->GetInfo(APE_INFO_TAG);
    bool HasID3Tag=pAPETag->GetHasID3Tag();
    bool HasAPETag=pAPETag->GetHasAPETag();

    if (HasID3Tag || HasAPETag)
    {
        //Iterate through all the tag fields
        CAPETagField *pTagField;
        for(int i=0; 0!=(pTagField=pAPETag->GetTagField(i)); i++)
        {
            Ztring Key; Key.From_Unicode(pTagField->GetFieldName()); Key.MakeUpperCase();
            Ztring Value; Value.From_UTF8(pTagField->GetFieldValue(), pTagField->GetFieldValueSize());

            //Parsing
                 if (Key==L"ALBUM")         Fill(Stream_General, 0, "Album", Value);
            else if (Key==L"ARTIST")        Fill(Stream_General, 0, "Performer", Value);
            else if (Key==L"AUTHOR")        Fill(Stream_General, 0, "Performer", Value);
            else if (Key==L"COMMENTS")      Fill(Stream_General, 0, "Comment", Value);
            else if (Key==L"COMPOSER")      Fill(Stream_General, 0, "WrittentBy", Value);
            else if (Key==L"YEAR")          Fill(Stream_General, 0, "Recorded_Date", Value);
            else if (Key==L"TRACK")         Fill(Stream_General, 0, "Track/Position", Value);
            else if (Key==L"TITLE")         Fill(Stream_General, 0, "Title", Value);
            else                            Fill(Stream_General, 0, Key.To_UTF8().c_str(), Value);
        }
    }
}

/*
//---------------------------------------------------------------------------
int File_Ape::Write (const Ztring &ToSet, stream_t StreamKind, size_t StreamNumber, const Ztring &Parameter, const Ztring &OldValue)
{
    #ifndef _UNICODE
        return -1;
    #endif

    if ( StreamNumber != 0 )
        return 1;

    Ztring key_org;
    {
        from_mediainfo_t::const_iterator it(from_mediainfo.find(Parameter));
        if ( it != from_mediainfo.end() ) {
            key_org=it->second;
        } else {
            key_org=Parameter;
        }
    }
    Ztring key(key_org);
    key.MakeUpperCase();
    switch(StreamKind)
    {
    case Stream_General:
    case Stream_Audio:
        {
            // variable declares
            int                    nRetVal = 0;                                        // generic holder for return values

            // open the file and error check
            IAPEDecompress *    pAPEDecompress = CreateIAPEDecompress(wxString(General[0](_T("CompleteName")).c_str()).wc_str(*wxConvCurrent), &nRetVal);
            if (nRetVal != ERROR_SUCCESS)
            {
                return -1;
            }

            // set tag
            CAPETag * pAPETag = (CAPETag *) pAPEDecompress->GetInfo(APE_INFO_TAG);
            Ztring key_to_set;
            Ztring value_to_set;
            if ( pAPETag->GetHasID3Tag() || pAPETag->GetHasAPETag() ) {
                CAPETagField * pTagField;
                for(int i = 0; 0 != (pTagField=pAPETag->GetTagField(i)); ++i)
                {
                    const Ztring curkey_org(wxString(pTagField->GetFieldName(),wxConvUTF8).c_str());
                    Ztring curkey(curkey_org);
                    curkey.MakeUpperCase();
                    if ( key == curkey )
                    {
                        key_to_set=curkey_org;
                        if ( pTagField->GetIsReadOnly() ) {
                            return -1;
                        }
                        else {
                            break;
                        }
                    }
                }
            }
            if ( key_to_set.empty() ) key_to_set=key_org;
            if ( !value_to_set.empty() ) value_to_set+=_T("/");
            value_to_set+=ToSet;

            if ( ToSet.empty() )
            {
                std::string temp_value_to_set(wxString(key_to_set.c_str()).mb_str(wxConvUTF8));
                for(std::string::iterator it=temp_value_to_set.begin(); it!=temp_value_to_set.end(); ++it) {
                    if ( *it == '/' ) *it=0;
                }

                pAPETag->SetFieldString(wxString(key_to_set.c_str()).wc_str(*wxConvCurrent), temp_value_to_set.c_str(), true);
            }
            else
            {
                pAPETag->RemoveField(wxString(key_to_set.c_str()).wc_str(*wxConvCurrent));
            }

            pAPETag->Save();
        }
        break;
    }

    return 1;
}
*/

//***************************************************************************
// Information
//***************************************************************************

//---------------------------------------------------------------------------
void File_Ape::HowTo(stream_t StreamKind)
{
         if (StreamKind==Stream_General)
    {
        General[0](_T("Format"), Info_HowTo)=_T("R");
        General[0](_T("BitRate"), Info_HowTo)=_T("R");
        General[0](_T("PlayTime"), Info_HowTo)=_T("R");
        General[0](_T("Author"), Info_HowTo)=_T("R");
        General[0](_T("Album"), Info_HowTo)=_T("R");
        General[0](_T("Track"), Info_HowTo)=_T("R");
        General[0](_T("Comment"), Info_HowTo)=_T("R");
    }
    else if (StreamKind==Stream_Audio)
    {
        Audio[0](_T("BitRate"), Info_HowTo)=_T("R");
        Audio[0](_T("Channel(s)"), Info_HowTo)=_T("R");
        Audio[0](_T("SamplingRate"), Info_HowTo)=_T("R");
        Audio[0](_T("Codec"), Info_HowTo)=_T("R");
    }
}

} //NameSpace

#endif //MEDIAINFO_APE_YES

