// MediaInfo - All info about media files, different parser listing part
// Copyright (C) 2006-2007 Jerome Martinez, Zen@MediaArea.net
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
// How to:
// To add a new format,
// Fill includes, SelectFromExtension, ListFormats and LibraryIsModified
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#include <wx/wxprec.h>
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#include "MediaInfo/MediaInfo.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
    #if defined(MEDIAINFO_MK_YES)       || (!defined(MEDIAINFO_MULTI_NO)        && !defined(MEDIAINFO_MK_NO))
        #include "MediaInfo/Multiple/File_Mk.h"
    #endif
    #if defined(MEDIAINFO_OGG_YES)      || (!defined(MEDIAINFO_MULTI_NO)        && !defined(MEDIAINFO_OGG_NO))
        #include "MediaInfo/Multiple/File_Ogg.h"
    #endif
    #if defined(MEDIAINFO_RIFF_YES)     || (!defined(MEDIAINFO_MULTI_NO)        && !defined(MEDIAINFO_RIFF_NO))
        #include "MediaInfo/Multiple/File_Riff.h"
    #endif
    #if defined(MEDIAINFO_MPEG4_YES)    || (!defined(MEDIAINFO_MULTI_NO)        && !defined(MEDIAINFO_MPEG4_NO))
        #include "MediaInfo/Multiple/File_Mpeg4.h"
    #endif
    #if defined(MEDIAINFO_SKY_YES)      || (!defined(MEDIAINFO_MULTI_NO)        && !defined(MEDIAINFO_SKY_NO))
        #include "MediaInfo/Multiple/File_Skm.h"
    #endif
    #if defined(MEDIAINFO_MPEGPS_YES)   || (!defined(MEDIAINFO_MULTI_NO)        && !defined(MEDIAINFO_MPEGPS_NO))
        #include "MediaInfo/Multiple/File_MpegPs.h"
    #endif
    #if defined(MEDIAINFO_MPEGTS_YES)   || (!defined(MEDIAINFO_MULTI_NO)        && !defined(MEDIAINFO_MPEGTS_NO))
        #include "MediaInfo/Multiple/File_MpegTs.h"
    #endif
    #if defined(MEDIAINFO_FLV_YES)      || (!defined(MEDIAINFO_VIDEO_NO)        && !defined(MEDIAINFO_FLV_NO))
        #include "MediaInfo/Multiple/File_Flv.h"
    #endif
    #if defined(MEDIAINFO_SWF_YES)      || (!defined(MEDIAINFO_VIDEO_NO)        && !defined(MEDIAINFO_SWF_NO))
        #include "MediaInfo/Multiple/File_Swf.h"
    #endif
    #if defined(MEDIAINFO_MXF_YES)      || (!defined(MEDIAINFO_VIDEO_NO)        && !defined(MEDIAINFO_MXF_NO))
        #include "MediaInfo/Multiple/File_Mxf.h"
    #endif
    #if defined(MEDIAINFO_WM_YES)       || (!defined(MEDIAINFO_MULTI_NO)        && !defined(MEDIAINFO_WM_NO))
        #include "MediaInfo/Multiple/File_Wm.h"
    #endif
    #if defined(MEDIAINFO_RM_YES)       || (!defined(MEDIAINFO_MULTI_NO)        && !defined(MEDIAINFO_RM_NO))
        #include "MediaInfo/Multiple/File_Rm.h"
    #endif
    #if defined(MEDIAINFO_DVDV_YES)     || (!defined(MEDIAINFO_VIDEO_NO)        && !defined(MEDIAINFO_DVDV_NO))
        #include "MediaInfo/Multiple/File_Dvdv.h"
    #endif
    #if defined(MEDIAINFO_CDXA_YES)     || (!defined(MEDIAINFO_MULTI_NO)        && !defined(MEDIAINFO_CDXA_NO))
        #include "MediaInfo/Multiple/File_Cdxa.h"
    #endif
    #if defined(MEDIAINFO_AVC_YES)      || (!defined(MEDIAINFO_VIDEO_NO)        && !defined(MEDIAINFO_AVC_NO))
        #include "MediaInfo/Video/File_Avc.h"
    #endif
    #if defined(MEDIAINFO_MPEG4V_YES)   || (!defined(MEDIAINFO_VIDEO_NO)        && !defined(MEDIAINFO_MPEG4V_NO))
        #include "MediaInfo/Video/File_Mpeg4v.h"
    #endif
    #if defined(MEDIAINFO_MPEGV_YES)    || (!defined(MEDIAINFO_VIDEO_NO)        && !defined(MEDIAINFO_MPEGV_NO))
        #include "MediaInfo/Video/File_Mpegv.h"
    #endif
    #if defined(MEDIAINFO_FLIC_YES)     || (!defined(MEDIAINFO_VIDEO_NO)        && !defined(MEDIAINFO_FLIC_NO))
        #include "MediaInfo/Video/File_Flic.h"
    #endif
    #if defined(MEDIAINFO_AAC_YES)      || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_AAC_NO))
        #include "MediaInfo/Audio/File_Aac.h"
    #endif
    #if defined(MEDIAINFO_AC3_YES)      || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_AC3_NO))
        #include "MediaInfo/Audio/File_Ac3.h"
    #endif
    #if defined(MEDIAINFO_AMR_YES)      || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_AMR_NO))
        #include "MediaInfo/Audio/File_Amr.h"
    #endif
    #if defined(MEDIAINFO_DTS_YES)      || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_DTS_NO))
        #include "MediaInfo/Audio/File_Dts.h"
    #endif
    #if defined(MEDIAINFO_FLAC_YES)     || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_FLAC_NO))
        #include "MediaInfo/Audio/File_Flac.h"
    #endif
    #if defined(MEDIAINFO_APE_YES)      || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_APE_NO))
        #include "MediaInfo/Audio/File_Ape.h"
    #endif
    #if defined(MEDIAINFO_MPC_YES)      || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_MPC_NO))
        #include "MediaInfo/Audio/File_Mpc.h"
    #endif
    #if defined(MEDIAINFO_MPEGA_YES)    || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_MPEGA_NO))
        #include "MediaInfo/Audio/File_Mpega.h"
    #endif
    #if defined(MEDIAINFO_SNDFILE_YES)  || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_SNDFILE_NO))
        #include "MediaInfo/Audio/File_SndFile.h"
    #endif
    #if defined(MEDIAINFO_TWINVQ_YES)      || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_TWINVQ_NO))
        #include "MediaInfo/Audio/File_TwinVQ.h"
    #endif
    #if defined(MEDIAINFO_OTHERTEXT_YES)|| (!defined(MEDIAINFO_TEXT_NO)         && !defined(MEDIAINFO_OTHERTEXT_NO))
        #include "MediaInfo/Text/File_OtherText.h"
    #endif
    #if defined(MEDIAINFO_PNG_YES)      || (!defined(MEDIAINFO_IMAGE_NO)        && !defined(MEDIAINFO_PNG_NO))
        #include "MediaInfo/Image/File_Png.h"
    #endif
    #if defined(MEDIAINFO_JPEG_YES)     || (!defined(MEDIAINFO_IMAGE_NO)        && !defined(MEDIAINFO_JPEG_NO))
        #include "MediaInfo/Image/File_Jpeg.h"
    #endif
    #if defined(MEDIAINFO_BMP_YES)      || (!defined(MEDIAINFO_IMAGE_NO)        && !defined(MEDIAINFO_BMP_NO))
        #include "MediaInfo/Image/File_Bmp.h"
    #endif
    #if defined(MEDIAINFO_ICO_YES)      || (!defined(MEDIAINFO_IMAGE_NO)        && !defined(MEDIAINFO_ICO_NO))
        #include "MediaInfo/Image/File_Ico.h"
    #endif
    #if defined(MEDIAINFO_GIF_YES)      || (!defined(MEDIAINFO_IMAGE_NO)        && !defined(MEDIAINFO_GIF_NO))
        #include "MediaInfo/Image/File_Gif.h"
    #endif
    #if defined(MEDIAINFO_TIFF_YES)     || (!defined(MEDIAINFO_IMAGE_NO)        && !defined(MEDIAINFO_TIFF_NO))
        #include "MediaInfo/Image/File_Tiff.h"
    #endif
    #if defined(MEDIAINFO_7Z_YES)       || (!defined(MEDIAINFO_COMPRESSOR_NO)   && !defined(MEDIAINFO_7Z_NO))
        #include "MediaInfo/Archive/File_7z.h"
    #endif
    #if defined(MEDIAINFO_ZIP_YES)      || (!defined(MEDIAINFO_COMPRESSOR_NO)   && !defined(MEDIAINFO_ZIP_NO))
        #include "MediaInfo/Archive/File_Zip.h"
    #endif
    #if defined(MEDIAINFO_RAR_YES)      || (!defined(MEDIAINFO_COMPRESSOR_NO)   && !defined(MEDIAINFO_RAR_NO))
        #include "MediaInfo/Archive/File_Rar.h"
    #endif
    #if defined(MEDIAINFO_ACE_YES)      || (!defined(MEDIAINFO_COMPRESSOR_NO)   && !defined(MEDIAINFO_ACE_NO))
        #include "MediaInfo/Archive/File_Ace.h"
    #endif
    #if !defined(MEDIAINFO_OTHER_NO)
        #include "MediaInfo/File_Other.h"
    #endif
    #if !defined(MEDIAINFO_UNKNOWN_NO)
        #include "MediaInfo/File_Unknown.h"
    #endif
    #if !defined(MEDIAINFO_DUMMY_NO)
        #include "MediaInfo/File_Dummy.h"
    #endif
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//---------------------------------------------------------------------------
extern MediaInfo_Config Config;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void MediaInfo::SelectFromExtension (const Ztring &Parser)
{
    //Clear last value
    delete Info; Info=NULL;

    //Searching the right File_*
             if (0) {} //For #defines
    #if defined(MEDIAINFO_MK_YES)       || (!defined(MEDIAINFO_MULTI_NO)        && !defined(MEDIAINFO_MK_NO))
        else if (Parser==_T("Mk"))    Info=new File_Mk();
    #endif
    #if defined(MEDIAINFO_OGG_YES)      || (!defined(MEDIAINFO_MULTI_NO)        && !defined(MEDIAINFO_OGG_NO))
        else if (Parser==_T("Ogg"))         Info=new File_Ogg();
    #endif
    #if defined(MEDIAINFO_RIFF_YES)     || (!defined(MEDIAINFO_MULTI_NO)        && !defined(MEDIAINFO_RIFF_NO))
        else if (Parser==_T("Riff"))        Info=new File_Riff();
    #endif
    #if defined(MEDIAINFO_MPEG4_YES)    || (!defined(MEDIAINFO_MULTI_NO)        && !defined(MEDIAINFO_MPEG4_NO))
        else if (Parser==_T("Mpeg4"))       Info=new File_Mpeg4();
    #endif
    #if defined(MEDIAINFO_SKM_YES)      || (!defined(MEDIAINFO_MULTI_NO)        && !defined(MEDIAINFO_SKM_NO))
        else if (Parser==_T("Skm"))       Info=new File_Skm();
    #endif
    #if defined(MEDIAINFO_MPEGPS_YES)   || (!defined(MEDIAINFO_MULTI_NO)        && !defined(MEDIAINFO_MPEGPS_NO))
        else if (Parser==_T("Mpeg"))        Info=new File_MpegPs();
    #endif
    #if defined(MEDIAINFO_MPEGTS_YES)   || (!defined(MEDIAINFO_MULTI_NO)        && !defined(MEDIAINFO_MPEGTS_NO))
        else if (Parser==_T("MpegTs"))      Info=new File_MpegTs();
    #endif
    #if defined(MEDIAINFO_FLV_YES)      || (!defined(MEDIAINFO_VIDEO_NO)        && !defined(MEDIAINFO_FLV_NO))
        else if (Parser==_T("Flv"))         Info=new File_Swf();
    #endif
    #if defined(MEDIAINFO_SWF_YES)      || (!defined(MEDIAINFO_VIDEO_NO)        && !defined(MEDIAINFO_SWF_NO))
        else if (Parser==_T("Swf"))         Info=new File_Swf();
    #endif
    #if defined(MEDIAINFO_MXF_YES)      || (!defined(MEDIAINFO_VIDEO_NO)        && !defined(MEDIAINFO_MXF_NO))
        else if (Parser==_T("Mxf"))         Info=new File_Mxf();
    #endif
    #if defined(MEDIAINFO_WM_YES)       || (!defined(MEDIAINFO_MULTI_NO)        && !defined(MEDIAINFO_WM_NO))
        else if (Parser==_T("Wm"))          Info=new File_Wm();
    #endif
    #if defined(MEDIAINFO_RM_YES)       || (!defined(MEDIAINFO_MULTI_NO)        && !defined(MEDIAINFO_RM_NO))
        else if (Parser==_T("Rm"))          Info=new File_Rm();
    #endif
    #if defined(MEDIAINFO_DVDV_YES)     || (!defined(MEDIAINFO_VIDEO_NO)        && !defined(MEDIAINFO_DVDV_NO))
        else if (Parser==_T("Dvdv"))        Info=new File_Dvdv();
    #endif
    #if defined(MEDIAINFO_CDXA_YES)     || (!defined(MEDIAINFO_MULTI_NO)        && !defined(MEDIAINFO_CDXA_NO))
        else if (Parser==_T("Cdxa"))        Info=new File_Cdxa();
    #endif
    #if defined(MEDIAINFO_AVC_YES)      || (!defined(MEDIAINFO_VIDEO_NO)        && !defined(MEDIAINFO_AVC_NO))
        else if (Parser==_T("Avc"))         Info=new File_Avc();
    #endif
    #if defined(MEDIAINFO_MPEG4V_YES)   || (!defined(MEDIAINFO_VIDEO_NO)        && !defined(MEDIAINFO_MPEG4V_NO))
        else if (Parser==_T("Mpeg4v"))      Info=new File_Mpeg4v();
    #endif
    #if defined(MEDIAINFO_MPEGV_YES)    || (!defined(MEDIAINFO_VIDEO_NO)        && !defined(MEDIAINFO_MPEGV_NO))
        else if (Parser==_T("Mpegv"))       Info=new File_Mpegv();
    #endif
    #if defined(MEDIAINFO_FLIC_YES)     || (!defined(MEDIAINFO_VIDEO_NO)        && !defined(MEDIAINFO_FLIC_NO))
        else if (Parser==_T("Flic"))        Info=new File_Flic();
    #endif
    #if defined(MEDIAINFO_AAC_YES)      || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_AAC_NO))
        else if (Parser==_T("Aac"))         Info=new File_Aac();
    #endif
    #if defined(MEDIAINFO_AC3_YES)      || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_AC3_NO))
        else if (Parser==_T("Ac3"))         Info=new File_Ac3();
    #endif
    #if defined(MEDIAINFO_AMR_YES)      || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_AMR_NO))
        else if (Parser==_T("Amr"))         Info=new File_Amr();
    #endif
    #if defined(MEDIAINFO_DTS_YES)      || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_DTS_NO))
        else if (Parser==_T("Dts"))         Info=new File_Dts();
    #endif
    #if defined(MEDIAINFO_FLAC_YES)     || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_FLAC_NO))
        else if (Parser==_T("Flac"))        Info=new File_Flac();
    #endif
    #if defined(MEDIAINFO_APE_YES)      || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_APE_NO))
        else if (Parser==_T("Ape"))         Info=new File_Ape();
    #endif
    #if defined(MEDIAINFO_MPC_YES)      || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_MPC_NO))
        else if (Parser==_T("Mpc"))         Info=new File_Mpc();
    #endif
    #if defined(MEDIAINFO_MPEGA_YES)    || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_MPEGA_NO))
        else if (Parser==_T("Mpega"))       Info=new File_Mpega();
    #endif
    #if defined(MEDIAINFO_SNDFILE_YES)  || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_SNDFILE_NO))
        else if (Parser==_T("SndFile"))     Info=new File_SndFile();
    #endif
    #if defined(MEDIAINFO_TWINVQ_YES)   || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_TWINVQ_NO))
        else if (Parser==_T("TwinVQ"))     Info=new File_TwinVQ();
    #endif
    #if defined(MEDIAINFO_OTHERTEXT_YES)|| (!defined(MEDIAINFO_TEXT_NO)         && !defined(MEDIAINFO_OTHERTEXT_NO))
        else if (Parser==_T("OtherText"))   Info=new File_OtherText();
    #endif
    #if defined(MEDIAINFO_PNG_YES)      || (!defined(MEDIAINFO_IMAGE_NO)        && !defined(MEDIAINFO_PNG_NO))
        else if (Parser==_T("Png"))         Info=new File_Png();
    #endif
    #if defined(MEDIAINFO_JPEG_YES)     || (!defined(MEDIAINFO_IMAGE_NO)        && !defined(MEDIAINFO_JPEG_NO))
        else if (Parser==_T("Jpeg"))        Info=new File_Jpeg();
    #endif
    #if defined(MEDIAINFO_BMP_YES)      || (!defined(MEDIAINFO_IMAGE_NO)        && !defined(MEDIAINFO_BMP_NO))
        else if (Parser==_T("Bmp"))         Info=new File_Bmp();
    #endif
    #if defined(MEDIAINFO_ICO_YES)      || (!defined(MEDIAINFO_IMAGE_NO)        && !defined(MEDIAINFO_ICO_NO))
        else if (Parser==_T("Ico"))         Info=new File_Ico();
    #endif
    #if defined(MEDIAINFO_GIF_YES)      || (!defined(MEDIAINFO_IMAGE_NO)        && !defined(MEDIAINFO_GIF_NO))
        else if (Parser==_T("Gif"))         Info=new File_Gif();
    #endif
    #if defined(MEDIAINFO_TIFF_YES)     || (!defined(MEDIAINFO_IMAGE_NO)        && !defined(MEDIAINFO_TIFF_NO))
        else if (Parser==_T("Tiff"))        Info=new File_Tiff();
    #endif
    #if defined(MEDIAINFO_7Z_YES)       || (!defined(MEDIAINFO_COMPRESSOR_NO)   && !defined(MEDIAINFO_7Z_NO))
        else if (Parser==_T("7z"))          Info=new File_7z();
    #endif
    #if defined(MEDIAINFO_ZIP_YES)      || (!defined(MEDIAINFO_COMPRESSOR_NO)   && !defined(MEDIAINFO_ZIP_NO))
        else if (Parser==_T("zip"))         Info=new File_Zip();
    #endif
    #if defined(MEDIAINFO_RAR_YES)      || (!defined(MEDIAINFO_COMPRESSOR_NO)   && !defined(MEDIAINFO_RAR_NO))
        else if (Parser==_T("rar"))         Info=new File_Rar();
    #endif
    #if defined(MEDIAINFO_ACE_YES)      || (!defined(MEDIAINFO_COMPRESSOR_NO)   && !defined(MEDIAINFO_ACE_NO))
        else if (Parser==_T("ace"))         Info=new File_Ace();
    #endif
    #if !defined(MEDIAINFO_OTHER_NO)
        else if (Parser==_T("Other"))       Info=new File_Other();
    #endif
}

//---------------------------------------------------------------------------
int MediaInfo::ListFormats()
{
    delete Info; Info=NULL;
    #if defined(MEDIAINFO_MK_YES)       || (!defined(MEDIAINFO_MULTI_NO) && !defined(MEDIAINFO_MK_NO))
        Info=new File_Mk();                 if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_OGG_YES)      || (!defined(MEDIAINFO_MULTI_NO) && !defined(MEDIAINFO_OGG_NO))
        Info=new File_Ogg();                if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_RIFF_YES)     || (!defined(MEDIAINFO_MULTI_NO) && !defined(MEDIAINFO_RIFF_NO))
        Info=new File_Riff();               if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_AVC_YES)      || (!defined(MEDIAINFO_AUDIO_NO) && !defined(MEDIAINFO_AVC_NO))
    if (InternalMethod==1)
    {
        Info=new File_Avc();                if (ApplyMethod()>0) return 1; //TODO: Curently, must do it here because Mp4v2 library freeze with H264 streams
    }
    #endif
    #if defined(MEDIAINFO_MPEGPS_YES)   || (!defined(MEDIAINFO_MULTI_NO) && !defined(MEDIAINFO_MPEGPS_NO))
        Info=new File_MpegPs();             if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_MPEGTS_YES)   || (!defined(MEDIAINFO_MULTI_NO) && !defined(MEDIAINFO_MPEGTS_NO))
        Info=new File_MpegTs();             if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_MPEG4_YES)    || (!defined(MEDIAINFO_MULTI_NO) && !defined(MEDIAINFO_MPEG4_NO))
        Info=new File_Mpeg4();              if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_SKM_YES)      || (!defined(MEDIAINFO_MULTI_NO) && !defined(MEDIAINFO_SKM_NO))
        Info=new File_Skm();              if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_FLV_YES)      || (!defined(MEDIAINFO_VIDEO_NO) && !defined(MEDIAINFO_FLV_NO))
        Info=new File_Flv();                if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_SWF_YES)      || (!defined(MEDIAINFO_VIDEO_NO) && !defined(MEDIAINFO_SWF_NO))
        Info=new File_Swf();                if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_MXF_YES)      || (!defined(MEDIAINFO_MULTI_NO) && !defined(MEDIAINFO_MXF_NO))
        Info=new File_Mxf();                if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_WM_YES)       || (!defined(MEDIAINFO_MULTI_NO) && !defined(MEDIAINFO_WM_NO))
        Info=new File_Wm();                 if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_RM_YES)       || (!defined(MEDIAINFO_MULTI_NO) && !defined(MEDIAINFO_RM_NO))
        Info=new File_Rm();                 if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_DVDV_YES)     || (!defined(MEDIAINFO_VIDEO_NO)        && !defined(MEDIAINFO_DVDV_NO))
        Info=new File_Dvdv();               if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_CDXA_YES)     || (!defined(MEDIAINFO_MULTI_NO)        && !defined(MEDIAINFO_CDXA_NO))
        Info=new File_Cdxa();               if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_AVC_YES)      || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_AVC_NO))
        Info=new File_Avc();                if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_MPEG4V_YES)   || (!defined(MEDIAINFO_VIDEO_NO)        && !defined(MEDIAINFO_MPEG4V_NO))
        Info=new File_Mpeg4v();             if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_MPEGV_YES)    || (!defined(MEDIAINFO_VIDEO_NO)        && !defined(MEDIAINFO_MPEGV_NO))
        Info=new File_Mpegv();              if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_FLIC_YES)     || (!defined(MEDIAINFO_VIDEO_NO)        && !defined(MEDIAINFO_FLIC_NO))
        Info=new File_Flic();               if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_AAC_YES)      || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_AAC_NO))
        Info=new File_Aac();                if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_AC3_YES)      || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_AC3_NO))
        Info=new File_Ac3();                if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_AMR_YES)      || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_AMR_NO))
        Info=new File_Amr();                if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_DTS_YES)      || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_DTS_NO))
        Info=new File_Dts();                if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_FLAC_YES)     || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_FLAC_NO))
        Info=new File_Flac();               if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_APE_YES)      || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_APE_NO))
        Info=new File_Ape();                if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_MPC_YES)      || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_MPC_NO))
        Info=new File_Mpc();                if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_MPEGA_YES)    || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_MPEGA_NO))
        Info=new File_Mpega();              if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_SNDFILE_YES)  || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_SNDFILE_NO))
        Info=new File_SndFile();            if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_TWINVQ_YES)      || (!defined(MEDIAINFO_AUDIO_NO)        && !defined(MEDIAINFO_TWINVQ_NO))
        Info=new File_TwinVQ();             if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_OTHERTEXT_YES)|| (!defined(MEDIAINFO_TEXT_NO)         && !defined(MEDIAINFO_OTHERTEXT_NO))
        Info=new File_OtherText();          if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_PNG_YES)      || (!defined(MEDIAINFO_IMAGE_NO)        && !defined(MEDIAINFO_PNG_NO))
        Info=new File_Png();                if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_JPEG_YES)     || (!defined(MEDIAINFO_IMAGE_NO)        && !defined(MEDIAINFO_JPEG_NO))
        Info=new File_Jpeg();               if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_BMP_YES)      || (!defined(MEDIAINFO_IMAGE_NO)        && !defined(MEDIAINFO_BMP_NO))
        Info=new File_Bmp();                if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_ICO_YES)      || (!defined(MEDIAINFO_IMAGE_NO)        && !defined(MEDIAINFO_ICO_NO))
        Info=new File_Ico();                if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_GIF_YES)      || (!defined(MEDIAINFO_IMAGE_NO)        && !defined(MEDIAINFO_GIF_NO))
        Info=new File_Gif();                if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_TIFF_YES)     || (!defined(MEDIAINFO_IMAGE_NO)        && !defined(MEDIAINFO_TIFF_NO))
        Info=new File_Tiff();               if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_7Z_YES)       || (!defined(MEDIAINFO_COMPRESSOR_NO)   && !defined(MEDIAINFO_7Z_NO))
        Info=new File_7z();                 if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_ZIP_YES)      || (!defined(MEDIAINFO_COMPRESSOR_NO)   && !defined(MEDIAINFO_ZIP_NO))
        Info=new File_Zip();                if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_RAR_YES)      || (!defined(MEDIAINFO_COMPRESSOR_NO)   && !defined(MEDIAINFO_RAR_NO))
        Info=new File_Rar();                if (ApplyMethod()>0) return 1;
    #endif
    #if defined(MEDIAINFO_ACE_YES)      || (!defined(MEDIAINFO_COMPRESSOR_NO)   && !defined(MEDIAINFO_ACE_NO))
        Info=new File_Ace();                if (ApplyMethod()>0) return 1;
    #endif
    #if !defined(MEDIAINFO_OTHER_NO)
    if (InternalMethod==1 || InternalMethod==2)
    {
        Info=new File_Other();              if (ApplyMethod()>0) return 1;
    }
    #endif
    #if !defined(MEDIAINFO_UNKNOWN_NO)
    if (InternalMethod==1)
    {
        Info=new File_Unknown();            if (ApplyMethod()>0) return 1;
    }
    #endif
    return 0;
}

//---------------------------------------------------------------------------
bool MediaInfo::LibraryIsModified ()
{
    #if defined(MEDIAINFO_MULTI_NO) || defined(MEDIAINFO_VIDEO_NO) || defined(MEDIAINFO_AUDIO_NO) || defined(MEDIAINFO_TEXT_NO) || defined(MEDIAINFO_IMAGE_NO) || defined(MEDIAINFO_COMPRESSOR_NO) \
     || defined(MEDIAINFO_MK_NO) || defined(MEDIAINFO_OGG_NO) || defined(MEDIAINFO_RIFF_NO) || defined(MEDIAINFO_MPEG4_NO) || defined(MEDIAINFO_MPEGPS_NO) || defined(MEDIAINFO_MPEGTS_NO) || defined(MEDIAINFO_FLV_NO) || defined(MEDIAINFO_SWF_NO) || defined(MEDIAINFO_MXF_NO) || defined(MEDIAINFO_WM_NO) || defined(MEDIAINFO_QT_NO) || defined(MEDIAINFO_RM_NO) || defined(MEDIAINFO_DVDV_NO) || defined(MEDIAINFO_CDXA_NO) \
     || defined(MEDIAINFO_AVC_NO) || defined(MEDIAINFO_MPEG4V_NO) || defined(MEDIAINFO_MPEGV_NO) || defined(MEDIAINFO_FLIC_NO) \
     || defined(MEDIAINFO_AAC_NO) || defined(MEDIAINFO_AC3_NO) || defined(MEDIAINFO_AMR_NO) || defined(MEDIAINFO_DTS_NO) || defined(MEDIAINFO_FLAC_NO) || defined(MEDIAINFO_APE_NO) || defined(MEDIAINFO_MPC_NO) || defined(MEDIAINFO_MPEGA_NO) || defined(MEDIAINFO_SNDFILE_NO) || defined(MEDIAINFO_TWINVQ_NO) \
     || defined(MEDIAINFO_OTHERTEXT_NO) \
     || defined(MEDIAINFO_PNG_NO) || defined(MEDIAINFO_JPEG_NO) || defined(MEDIAINFO_BMP_NO) || defined(MEDIAINFO_ICO_NO) || defined(MEDIAINFO_GIF_NO) || defined(MEDIAINFO_TIFF_NO) \
     || defined(MEDIAINFO_7Z_NO) || defined(MEDIAINFO_ZIP_NO) || defined(MEDIAINFO_RAR_NO) || defined(MEDIAINFO_ACE_NO) \
     || defined(MEDIAINFO_OTHER_NO) || defined(MEDIAINFO_DUMMY_NO)
        return true;
    #else
        return false;
    #endif
}

//---------------------------------------------------------------------------
void MediaInfo::CreateDummy (const MediaInfo_String& Value)
{
    #if !defined(MEDIAINFO_DUMMY_NO)
    Info=new File_Dummy();
    ((File_Dummy*)Info)->KindOfDummy=Value;
    #endif
}

} //NameSpace


