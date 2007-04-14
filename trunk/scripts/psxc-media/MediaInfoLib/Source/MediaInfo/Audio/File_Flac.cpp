// File_Flac - Info for Flac files
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
#if defined(MEDIAINFO_FLAC_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Audio/File_Flac.h"
#define FLAC__NO_DLL
#include <FLAC++/metadata.h>
#undef FLAC__NO_DLL
#include <ZenLib/Utils.h>
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Format
//***************************************************************************

//---------------------------------------------------------------------------
void File_Flac::Read_File()
{
    //Init
    FLAC::Metadata::SimpleIterator SI;
    if (!SI.is_valid())
         return;
    if (!SI.init(CompleteFileName.To_Local().c_str(), true, true))
        return;

    //Filling
    Stream_Prepare(Stream_General);
    Fill("Format", "FLAC");

    Stream_Prepare(Stream_Audio);
    Fill("Codec", "FLAC");

    do {
        switch(SI.get_block_type())
        {
            case FLAC__METADATA_TYPE_STREAMINFO:
                {
                    FLAC::Metadata::StreamInfo* StreamInfo=(FLAC::Metadata::StreamInfo*)SI.get_block();
                    if (StreamInfo)
                    {
                        if (StreamInfo->get_min_framesize()==StreamInfo->get_max_framesize()
                         && StreamInfo->get_min_framesize()!=0 ) // 0 means it is unknown
                            Fill("BitRate_Mode", "CBR");
                         else
                            Fill("BitRate_Mode", "VBR");
                        Fill("SamplingRate", StreamInfo->get_sample_rate());
                        Fill("Channel(s)", StreamInfo->get_channels());
                        Fill("Resolution", StreamInfo->get_bits_per_sample());
                        Fill(Stream_General, 0, "PlayTime", (int32u)(int64u_float64(StreamInfo->get_total_samples())*1000/StreamInfo->get_sample_rate()));
                    }
                }
                break;
            case FLAC__METADATA_TYPE_PADDING:
                break;
            case FLAC__METADATA_TYPE_APPLICATION:
                break;
            case FLAC__METADATA_TYPE_SEEKTABLE:
                break;
            case FLAC__METADATA_TYPE_VORBIS_COMMENT:
                {
                    FLAC::Metadata::VorbisComment* vorbiscomment =(FLAC::Metadata::VorbisComment*)SI.get_block();
                    if (vorbiscomment)
                    {
                        Fill("Encoded_Library", vorbiscomment->get_vendor_string());
                        for(size_t t=0; t<vorbiscomment->get_num_comments(); t++)
                        {
                            std::string Key(vorbiscomment->get_comment(t).get_field_name(), vorbiscomment->get_comment(t).get_field_name_length());
                            Ztring Value; Value.From_UTF8(vorbiscomment->get_comment(t).get_field_value(), vorbiscomment->get_comment(t).get_field_value_length());

                            //Parsing
                                 if (Key=="ALBUM")          Fill(Stream_General, 0, "Album", Value);
                            else if (Key=="ARTIST")         Fill(Stream_General, 0, "Performer", Value);
                            else if (Key=="COPYRIGHT")      Fill(Stream_General, 0, "Copyright", Value);
                            else if (Key=="DATE")           Fill(Stream_General, 0, "Recorded_Date", Value);
                            else if (Key=="DESCRIPTION")    Fill(Stream_General, 0, "Comment", Value);
                            else if (Key=="LOCATION")       Fill(Stream_General, 0, "RecordingLocation", Value);
                            else if (Key=="PERFORMER")      Fill(Stream_General, 0, "Performer", Value);
                            else if (Key=="TITLE")          Fill(Stream_General, 0, "Title", Value);
                            else if (Key=="TRACKNUMBER")    Fill(Stream_General, 0, "Track", Value);
                            else                            Fill(Stream_General, 0, Key.c_str(), Value);
                        }
                    }
                }
                break;
            case FLAC__METADATA_TYPE_CUESHEET:
                break;
        };
    }
    while(SI.next());
}

/*
int File_Flac::Write (const Ztring &ToSet, stream_t StreamKind, size_t StreamNumber, const Ztring &Parameter, const Ztring &OldValue)
{
    if ( StreamNumber != 0 )
        return 1;

    FLAC::Metadata::SimpleIterator metadata_it;
    if ( metadata_it.is_valid() == false ) return -1;
    if ( metadata_it.init(wxString(General[0](_T("CompleteName")).c_str()).mb_str(*wxConvCurrent), true, true) == false ) return -1;
    if ( metadata_it.is_writable() == false ) return -1;

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
    //case Stream_General:
    case Stream_Audio:
        {
            bool have_found_vorbiscomment=false;
            do {
                switch(metadata_it.get_block_type()) {
                case FLAC__METADATA_TYPE_VORBIS_COMMENT:
                    {
                        have_found_vorbiscomment=true;
                        FLAC::Metadata::VorbisComment* vorbiscomment =(FLAC::Metadata::VorbisComment*)metadata_it.get_block();
                        if ( vorbiscomment )
                        {
                            if ( key == _T("Encoded_Library") )
                            {
                                std::string temp_vendor_string(wxString(ToSet.c_str()).mb_str(wxConvUTF8));
                                vorbiscomment->set_vendor_string(FLAC::Metadata::VorbisComment::Entry(temp_vendor_string.c_str(), temp_vendor_string.size()));
                                //TODO metadata_it.set_block(vorbiscomment.get());
                            }
                            else
                            {
                                for(unsigned int t=0; t<vorbiscomment->get_num_comments(); ++t)
                                {
                                    const Ztring curkey_org(wxString(std::string(vorbiscomment->get_comment(t).get_field_name(),vorbiscomment->get_comment(t).get_field_name_length()).c_str(),wxConvUTF8).c_str());
                                    const Ztring curvalue(wxString(std::string(vorbiscomment->get_comment(t).get_field_value(),vorbiscomment->get_comment(t).get_field_value_length()).c_str(),wxConvUTF8).c_str());
                                    Ztring fieldname;
                                    Ztring curkey(curkey_org);

                                    curkey.MakeUpperCase();
                                    {
                                        to_mediainfo_t::const_iterator it(to_mediainfo.find(curkey));
                                        if ( it != to_mediainfo.end() ) {
                                            fieldname=it->second;
                                        } else {
                                            fieldname=curkey_org;
                                        }
                                    }
                                    if ( key == curkey || key == fieldname )
                                    {
                                        vorbiscomment->delete_comment(t);
                                        --t;
                                    }
                                }
                                if ( !ToSet.empty() )
                                {
                                    typedef boost::tokenizer<boost::char_separator<Char>,tstring::iterator,tstring > tokenizer_t;
                                    boost::char_separator<Char> sep(_T("/"));
                                    tstring tempstr(ToSet); // This shouldn't be necessary, but somehow I couldn't get it to work without this.
                                    tokenizer_t tokens(tempstr.begin(), tempstr.end(), sep);
                                    for(tokenizer_t::iterator tok_it=tokens.begin(); tok_it!=tokens.end();++tok_it) {
                                        const std::string t_tok_it(wxString(tok_it->c_str()).mb_str(wxConvUTF8));
                                        vorbiscomment->insert_comment(vorbiscomment->get_num_comments(),FLAC::Metadata::VorbisComment::Entry(wxString(key_org.c_str()).mb_str(wxConvUTF8),t_tok_it.c_str(),t_tok_it.size()));
                                    }
                                }
                                //TODO metadata_it.set_block(vorbiscomment.get());
                            }
                            return 1;
                        }
                    }
                    break;
                };
            } while(metadata_it.next());

            if ( !have_found_vorbiscomment && !ToSet.empty() )
            {
                FLAC::Metadata::VorbisComment* vorbiscomment =(FLAC::Metadata::VorbisComment*)metadata_it.get_block();

                std::string temp_vendor_string((wxString(_T("MediaInfo library "))+wxString(__DATE__,wxConvLibc)).mb_str(wxConvUTF8));
                vorbiscomment->set_vendor_string(FLAC::Metadata::VorbisComment::Entry(temp_vendor_string.c_str(), temp_vendor_string.size()));
                {
                    typedef boost::tokenizer<boost::char_separator<Char>,Ztring::iterator,Ztring > tokenizer_t;
                    boost::char_separator<Char> sep(_T("/"));
                    tstring tempstr(ToSet); // This shouldn't be necessary, but somehow I couldn't get it to work without this.
                    tokenizer_t tokens(tempstr.begin(), tempstr.end(), sep);
                    for(tokenizer_t::iterator tok_it=tokens.begin(); tok_it!=tokens.end();++tok_it) {
                        const std::string t_tok_it(wxString(tok_it->c_str()).mb_str(wxConvUTF8));
                        vorbiscomment->insert_comment(vorbiscomment->get_num_comments(),FLAC::Metadata::VorbisComment::Entry(wxString(key_org.c_str()).mb_str(wxConvUTF8),t_tok_it.c_str(),t_tok_it.size()));
                    }
                }

                //TODO metadata_it.insert_block_after(vorbiscomment.get());
            }
        }
        break;
    }

    return -1;
}
*/

//***************************************************************************
// Information
//***************************************************************************

//---------------------------------------------------------------------------
void File_Flac::HowTo(stream_t StreamKind)
{
         if (StreamKind==Stream_General)
    {
        General[0](_T("Format"), Info_HowTo)=_T("R");
        General[0](_T("BitRate"), Info_HowTo)=_T("R");
        General[0](_T("Title"), Info_HowTo)=_T("R");
        General[0](_T("Title/More"), Info_HowTo)=_T("R");
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

#endif //MEDIAINFO_FLAC_YES

