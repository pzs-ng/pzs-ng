// Config - Config file for MediaInfo
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
// All compilation definitions
// Helpers for compilers (precompilation)
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//***************************************************************************
// Precompilation
//***************************************************************************

#include <MediaInfo/PreComp.h>

//***************************************************************************
// Compilation conditions
//***************************************************************************

//---------------------------------------------------------------------------
// Multiple
#if !defined(MEDIAINFO_MULTI_NO) && !defined(MEDIAINFO_CDXA_NO) && !defined(MEDIAINFO_CDXA_YES)
    #define MEDIAINFO_CDXA_YES
#endif
#if !defined(MEDIAINFO_MULTI_NO) && !defined(MEDIAINFO_FLV_NO) && !defined(MEDIAINFO_FLV_YES)
    #define MEDIAINFO_FLV_YES
#endif
#if !defined(MEDIAINFO_MULTI_NO) && !defined(MEDIAINFO_MK_NO) && !defined(MEDIAINFO_MK_YES)
    #define MEDIAINFO_MK_YES
#endif
#if !defined(MEDIAINFO_MULTI_NO) && !defined(MEDIAINFO_MPEG4_NO) && !defined(MEDIAINFO_MPEG4_YES)
    #define MEDIAINFO_MPEG4_YES
#endif
#if !defined(MEDIAINFO_MULTI_NO) && !defined(MEDIAINFO_MPEGPS_NO) && !defined(MEDIAINFO_MPEGPS_YES)
    #define MEDIAINFO_MPEGPS_YES
#endif
#if !defined(MEDIAINFO_MULTI_NO) && !defined(MEDIAINFO_MPEGTS_NO) && !defined(MEDIAINFO_MPEGTS_YES)
    #define MEDIAINFO_MPEGTS_YES
#endif
#if !defined(MEDIAINFO_MULTI_NO) && !defined(MEDIAINFO_MXF_NO) && !defined(MEDIAINFO_MXF_YES)
    #define MEDIAINFO_MXF_YES
#endif
#if !defined(MEDIAINFO_MULTI_NO) && !defined(MEDIAINFO_OGG_NO) && !defined(MEDIAINFO_OGG_YES)
    #define MEDIAINFO_OGG_YES
#endif
#if !defined(MEDIAINFO_MULTI_NO) && !defined(MEDIAINFO_RIFF_NO) && !defined(MEDIAINFO_RIFF_YES)
    #define MEDIAINFO_RIFF_YES
#endif
#if !defined(MEDIAINFO_MULTI_NO) && !defined(MEDIAINFO_RM_NO) && !defined(MEDIAINFO_RM_YES)
    #define MEDIAINFO_RM_YES
#endif
#if !defined(MEDIAINFO_MULTI_NO) && !defined(MEDIAINFO_SKM_NO) && !defined(MEDIAINFO_SKM_YES)
    #define MEDIAINFO_SKM_YES
#endif
#if !defined(MEDIAINFO_MULTI_NO) && !defined(MEDIAINFO_SWF_NO) && !defined(MEDIAINFO_SWF_YES)
    #define MEDIAINFO_SWF_YES
#endif
#if !defined(MEDIAINFO_MULTI_NO) && !defined(MEDIAINFO_WM_NO) && !defined(MEDIAINFO_WM_YES)
    #define MEDIAINFO_WM_YES
#endif

//---------------------------------------------------------------------------
// Video
#if !defined(MEDIAINFO_VIDEO_NO) && !defined(MEDIAINFO_AVC_NO) && !defined(MEDIAINFO_AVC_YES)
    #define MEDIAINFO_AVC_YES
#endif
#if !defined(MEDIAINFO_VIDEO_NO) && !defined(MEDIAINFO_FLIC_NO) && !defined(MEDIAINFO_FLIC_YES)
    #define MEDIAINFO_MPEGV_YES
#endif
#if !defined(MEDIAINFO_VIDEO_NO) && !defined(MEDIAINFO_MPEG4V_NO) && !defined(MEDIAINFO_MPEG4V_YES)
    #define MEDIAINFO_MPEG4V_YES
#endif
#if !defined(MEDIAINFO_VIDEO_NO) && !defined(MEDIAINFO_MPEGV_NO) && !defined(MEDIAINFO_MPEGV_YES)
    #define MEDIAINFO_MPEGV_YES
#endif

//---------------------------------------------------------------------------
// Audio
#if !defined(MEDIAINFO_AUDIO_NO) && !defined(MEDIAINFO_AAC_NO) && !defined(MEDIAINFO_AAC_YES)
    #define MEDIAINFO_AAC_YES
#endif
#if !defined(MEDIAINFO_AUDIO_NO) && !defined(MEDIAINFO_AC3_NO) && !defined(MEDIAINFO_AC3_YES)
    #define MEDIAINFO_AC3_YES
#endif
#if !defined(MEDIAINFO_AUDIO_NO) && !defined(MEDIAINFO_AMR_NO) && !defined(MEDIAINFO_AMR_YES)
    #define MEDIAINFO_AMR_YES
#endif
#if !defined(MEDIAINFO_AUDIO_NO) && !defined(MEDIAINFO_APE_NO) && !defined(MEDIAINFO_APE_YES)
    #define MEDIAINFO_APE_YES
#endif
#if !defined(MEDIAINFO_AUDIO_NO) && !defined(MEDIAINFO_DTS_NO) && !defined(MEDIAINFO_DTS_YES)
    #define MEDIAINFO_DTS_YES
#endif
#if !defined(MEDIAINFO_AUDIO_NO) && !defined(MEDIAINFO_FLAC_NO) && !defined(MEDIAINFO_FLAC_YES)
    #define MEDIAINFO_FLAC_YES
#endif
#if !defined(MEDIAINFO_AUDIO_NO) && !defined(MEDIAINFO_ID3_NO) && !defined(MEDIAINFO_ID3_YES)
    #define MEDIAINFO_ID3_YES
#endif
#if !defined(MEDIAINFO_AUDIO_NO) && !defined(MEDIAINFO_MPC_NO) && !defined(MEDIAINFO_MPC_YES)
    #define MEDIAINFO_MPC_YES
#endif
#if !defined(MEDIAINFO_AUDIO_NO) && !defined(MEDIAINFO_MPEGA_NO) && !defined(MEDIAINFO_MPEGA_YES)
    #define MEDIAINFO_MPEGA_YES
#endif
#if !defined(MEDIAINFO_AUDIO_NO) && !defined(MEDIAINFO_SNDFILE_NO) && !defined(MEDIAINFO_SNDFILE_YES)
    #define MEDIAINFO_SNDFILE_YES
#endif
#if !defined(MEDIAINFO_AUDIO_NO) && !defined(MEDIAINFO_TWINVQ_NO) && !defined(MEDIAINFO_TWINVQ_YES)
    #define MEDIAINFO_TWINVQ_YES
#endif

//---------------------------------------------------------------------------
// Text
#if !defined(MEDIAINFO_TEXT_NO) && !defined(MEDIAINFO_OTHERTEXT_NO) && !defined(MEDIAINFO_OTHERTEXT_YES)
    #define MEDIAINFO_OTHERTEXT_YES
#endif

//---------------------------------------------------------------------------
// Image
#if !defined(MEDIAINFO_IMAGE_NO) && !defined(MEDIAINFO_BMP_NO) && !defined(MEDIAINFO_BMP_YES)
    #define MEDIAINFO_BMP_YES
#endif
#if !defined(MEDIAINFO_IMAGE_NO) && !defined(MEDIAINFO_GIF_NO) && !defined(MEDIAINFO_GIF_YES)
    #define MEDIAINFO_GIF_YES
#endif
#if !defined(MEDIAINFO_IMAGE_NO) && !defined(MEDIAINFO_ICO_NO) && !defined(MEDIAINFO_ICO_YES)
    #define MEDIAINFO_ICO_YES
#endif
#if !defined(MEDIAINFO_IMAGE_NO) && !defined(MEDIAINFO_JPEG_NO) && !defined(MEDIAINFO_JPEG_YES)
    #define MEDIAINFO_JPEG_YES
#endif
#if !defined(MEDIAINFO_IMAGE_NO) && !defined(MEDIAINFO_PNG_NO) && !defined(MEDIAINFO_PNG_YES)
    #define MEDIAINFO_PNG_YES
#endif
#if !defined(MEDIAINFO_IMAGE_NO) && !defined(MEDIAINFO_TIFF_NO) && !defined(MEDIAINFO_TIFF_YES)
    #define MEDIAINFO_TIFF_YES
#endif

//---------------------------------------------------------------------------
// Archive
#if !defined(MEDIAINFO_ARCHIVE_NO) && !defined(MEDIAINFO_7Z_NO) && !defined(MEDIAINFO_7Z_YES)
    #define MEDIAINFO_7Z_YES
#endif
#if !defined(MEDIAINFO_ARCHIVE_NO) && !defined(MEDIAINFO_ACE_NO) && !defined(MEDIAINFO_ACE_YES)
    #define MEDIAINFO_ACE_YES
#endif
#if !defined(MEDIAINFO_ARCHIVE_NO) && !defined(MEDIAINFO_RAR_NO) && !defined(MEDIAINFO_RAR_YES)
    #define MEDIAINFO_RAR_YES
#endif
#if !defined(MEDIAINFO_ARCHIVE_NO) && !defined(MEDIAINFO_ZIP_NO) && !defined(MEDIAINFO_ZIP_YES)
    #define MEDIAINFO_ZIP_YES
#endif

//---------------------------------------------------------------------------
// Other
#if !defined(MEDIAINFO_DUMMY_NO)
    #define MEDIAINFO_DUMMY_YES
#endif
#if !defined(MEDIAINFO_OTHER_NO)
    #define MEDIAINFO_OTHER_YES
#endif
#if !defined(MEDIAINFO_UNKNOWN_NO)
    #define MEDIAINFO_UNKNOWN_YES
#endif
//---------------------------------------------------------------------------

