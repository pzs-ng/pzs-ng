// File_Mpc - Info for Musepack files
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
#if defined(MEDIAINFO_MPC_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Audio/File_Mpc.h"
#include <cstdio>
#include <mpcdec/mpcdec.h>
#include <mpcdec/reader.h>
#include <mpcdec/streaminfo.h>
#include <ZenLib/Utils.h>
using namespace std;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Format
//***************************************************************************

//---------------------------------------------------------------------------
void File_Mpc::Read_Buffer_Continue()
{
    //Integrity
    if (Buffer_Size<=3)
        return;

    //Header
    if (CC3(Buffer)!=CC3("MP+"))
    {
        File_Offset=File_Size;
        return;
    }

    //Filling
    Read_Buffer_Continue_Read_File();

    //No need of more
    File_Offset=File_Size;
}

//---------------------------------------------------------------------------
//Note: we can't parse directly the file because the Musepack parser does too much false positives.
//
void File_Mpc::Read_Buffer_Continue_Read_File()
{
    #ifdef UNICODE
        #ifdef _WIN32
            FILE *File=_wfopen(CompleteFileName.c_str(), L"rb");
        #else
            FILE *File=fopen(CompleteFileName.To_Local().c_str(), "rb");
        #endif
    #else //UNICODE
        FILE *File=fopen(CompleteFileName.c_str(), "rb");
    #endif //UNICODE
    if (!File)
        return;

    mpc_reader_file Reader;
    mpc_reader_setup_file_reader(&Reader, File);

    mpc_streaminfo Info;
    mpc_streaminfo_init(&Info);
    if (mpc_streaminfo_read(&Info, &Reader.reader)!=ERROR_CODE_OK)
    {
        fclose(File);
        return; //Not a Musepack file
    }

    //Integrity
    if (Info.stream_version>10)
    {
        fclose(File);
        return;
    }

    //Filling
    Stream_Prepare(Stream_General);
    Fill("Format", "MPC");
    Stream_Prepare(Stream_Audio);
    if (Info.sample_freq)       Fill("SamplingRate", Info.sample_freq);
    if (Info.pcm_samples)       Fill("SamplingCount", Info.pcm_samples);
    if (Info.average_bitrate)   Fill("BitRate", Info.average_bitrate, 0);
    if (Info.stream_version)    Fill("Codec", Ztring::Ztring(_T("SV"))+Ztring::ToZtring(Info.stream_version));
    if (Info.profile_name)      Fill("Codec/Info", Info.profile_name);
    if (Info.channels)          Fill("Channel(s)", Info.channels);
    if (Info.encoder)           Fill("Encoded_Library", Info.encoder);
                                Fill("Resolution", 16); //MPC support only 16 bits

    fclose(File);
}

//***************************************************************************
// Information
//***************************************************************************

//---------------------------------------------------------------------------
void File_Mpc::HowTo(stream_t StreamKind)
{
        if (StreamKind==Stream_General)
    {
        General[0](_T("Format"), Info_HowTo)=_T("R");
        General[0](_T("OverallBitRate"), Info_HowTo)=_T("R");
        General[0](_T("PlayTime"), Info_HowTo)=_T("R");
        General[0](_T("Encoded_Library"), Info_HowTo)=_T("R");
    }
    else if (StreamKind==Stream_Audio)
    {
        Audio[0](_T("Codec"), Info_HowTo)=_T("R");
        Audio[0](_T("BitRate"), Info_HowTo)=_T("R");
        Audio[0](_T("Channel(s)"), Info_HowTo)=_T("R");
        Audio[0](_T("SamplingRate"), Info_HowTo)=_T("R");
        Audio[0](_T("SamplingCount"), Info_HowTo)=_T("R");
        Audio[0](_T("Resolution"), Info_HowTo)=_T("R");
    }
}

} //NameSpace

#endif //MEDIAINFO_MPC_YES

