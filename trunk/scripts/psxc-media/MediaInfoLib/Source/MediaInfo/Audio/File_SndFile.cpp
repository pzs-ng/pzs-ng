// File_Aac - Info for SndFile files
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
#if defined(MEDIAINFO_SNDFILE_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <SndFile/sndfile.h>
#include "MediaInfo/Audio/File_SndFile.h"
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Format
//***************************************************************************

//---------------------------------------------------------------------------
void File_SndFile::Read_File()
{
    SF_INFO Info;
    SNDFILE* File=sf_open(CompleteFileName.To_Local().c_str(), SFM_READ, &Info);
    if (!File)
        return;
    sf_close(File);

    //Integrity
    if (Info.format&0xFFFF0000==SF_FORMAT_RAW)
        return;

    Stream_Prepare(Stream_General);
    switch (Info.format&SF_FORMAT_TYPEMASK) //Major
    {
        case SF_FORMAT_WAV      : Fill("Format", "WAV");        break;
        case SF_FORMAT_AIFF     : Fill("Format", "AIFF");       break;
        case SF_FORMAT_AU       : Fill("Format", "AU");         break;
        case SF_FORMAT_RAW      : Fill("Format", "Audio-Raw");  break;
        case SF_FORMAT_PAF      : Fill("Format", "PAF");        break;
        case SF_FORMAT_SVX      : Fill("Format", "SVX");        break;
        case SF_FORMAT_NIST     : Fill("Format", "NIST");       break;
        case SF_FORMAT_VOC      : Fill("Format", "VOC");        break;
        case SF_FORMAT_IRCAM    : Fill("Format", "Ircam");      break;
        case SF_FORMAT_W64      : Fill("Format", "W64");        break;
        case SF_FORMAT_MAT4     : Fill("Format", "MAT4");       break;
        case SF_FORMAT_MAT5     : Fill("Format", "MAT5");       break;
        case SF_FORMAT_PVF      : Fill("Format", "PVF");        break;
        case SF_FORMAT_XI       : Fill("Format", "XI");         break;
        case SF_FORMAT_HTK      : Fill("Format", "HTK");        break;
        case SF_FORMAT_SDS      : Fill("Format", "SDS");        break;
        case SF_FORMAT_AVR      : Fill("Format", "AVR");        break;
        case SF_FORMAT_WAVEX    : Fill("Format", "WAV");        break;
        case SF_FORMAT_SD2      : Fill("Format", "SD2");        break;
    }

    //Codec
    Stream_Prepare(Stream_Audio);
    switch (Info.format&SF_FORMAT_SUBMASK) //Minor
    {
        case SF_FORMAT_PCM_S8   : Fill("Codec", "PCM");         Fill("Resolution", "8");    break;
        case SF_FORMAT_PCM_16   : Fill("Codec", "PCM");         Fill("Resolution", "16");   break;
        case SF_FORMAT_PCM_24   : Fill("Codec", "PCM");         Fill("Resolution", "24");   break;
        case SF_FORMAT_PCM_32   : Fill("Codec", "PCM");         Fill("Resolution", "8");    break;
        case SF_FORMAT_PCM_U8   : Fill("Codec", "PCM");         Fill("Resolution", "8");    break;
        case SF_FORMAT_FLOAT    : Fill("Codec", "Float");       Fill("Resolution", "32");   break;
        case SF_FORMAT_DOUBLE   : Fill("Codec", "Float");       Fill("Resolution", "64");   break;
        case SF_FORMAT_ULAW     : Fill("Codec", "U-Law");       Fill("Resolution", "");     break;
        case SF_FORMAT_ALAW     : Fill("Codec", "A-Law");       Fill("Resolution", "");     break;
        case SF_FORMAT_IMA_ADPCM: Fill("Codec", "ADPCM");       Fill("Resolution", "");     break;
        case SF_FORMAT_MS_ADPCM : Fill("Codec", "ADPCM");       Fill("Resolution", "");     break;
        case SF_FORMAT_GSM610   : Fill("Codec", "GSM 6.10");    Fill("Resolution", "");     break;
        case SF_FORMAT_G721_32  : Fill("Codec", "G721");        Fill("Resolution", "");     break;
        case SF_FORMAT_G723_24  : Fill("Codec", "G723");        Fill("Resolution", "");     break;
        case SF_FORMAT_DWVW_12  : Fill("Codec", "Delta");       Fill("Resolution", "12");   break;
        case SF_FORMAT_DWVW_16  : Fill("Codec", "Delta");       Fill("Resolution", "16");   break;
        case SF_FORMAT_DWVW_24  : Fill("Codec", "Delta");       Fill("Resolution", "24");   break;
        case SF_FORMAT_DWVW_N   : Fill("Codec", "Delta");       Fill("Resolution", "");     break;
        case SF_FORMAT_DPCM_8   : Fill("Codec", "8 bits diff"); Fill("Resolution", "");     break;
        case SF_FORMAT_DPCM_16  : Fill("Codec", "16 bits diff");Fill("Resolution", "");     break;
        }

    Fill("SamplingCount", Info.frames);
    Fill("SamplingRate", Info.samplerate);
    Fill("PlayTime", Info.frames*1000/Info.samplerate);
    Fill("Channel(s)", Info.channels);

    General_Fill(); //To have filesize
    Fill("BitRate", General[0](_T("FileSize")).To_int32s()*8*Info.samplerate/Info.frames/1000);
}

//***************************************************************************
// Information
//***************************************************************************

//---------------------------------------------------------------------------
void File_SndFile::HowTo(stream_t StreamKind)
{
    Stream_Prepare(Stream_General);
    Fill_HowTo("Format", "R");
    Fill_HowTo("PlayTime", "R");
    Fill_HowTo("OveralBitrate", "R");

    Stream_Prepare(Stream_Audio);
    Fill_HowTo("Codec", "R");
    Fill_HowTo("BitRate", "R");
    Fill_HowTo("Channel(s)", "R");
    Fill_HowTo("SamplingRate", "R");
    Fill_HowTo("SamplingCount", "R");
    Fill_HowTo("Resolution", "R");
    Fill_HowTo("PlayTime", "R");
}

} //NameSpace

#endif //MEDIAINFO_SNDFILE_YES

