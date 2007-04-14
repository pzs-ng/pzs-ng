// File_Riff - Info for RIFF files
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
// Information about RIFF files
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef MediaInfo_File_RiffH
#define MediaInfo_File_RiffH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/File__Base.h"
#include <vector>
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Class File_Riff
//***************************************************************************

class File_Riff : public File__Base
{
protected :
    //Format
    void Read_Buffer_Init ();
    void Read_Buffer_Continue ();
    void Read_Buffer_Finalize ();

    //Information
    void HowTo (stream_t StreamKind);

private :
    //Buffer
    bool Buffer_Parse();
    bool Element_Parse();
    int8u  Element_Level;
    int32u Element_Name[10];
    size_t Element_Size;
    size_t Element_HeaderSize;
    int64u Element_Next[10];
    int8u  Element_ID;

    //Buffer, Temp
    bool List;

    //Global information
    int32u TotalFrame; //Count of frames in the whole AVI file (with odml too)
    int32u avih_TotalFrame; //Count of frames in one MOVI chunk
    float32 avih_FrameRate; //FrameRate of the first video stream in one MOVI chunk
    std::vector<File__Base*> Codec_External;
    std::vector<size_t> Codec_External_PacketCount;
    std::vector<int64u> Codec_External_StreamSize;
    std::vector<ZenLib::Ztring> Codec_External_Codec;

    //Chunks
    void AVI_ ();
    void AVI__cset ();
    void AVI__exif ();
    void AVI__exif_xxxx ();
    void AVI__goog ();
    void AVI__goog_GDAT ();
    void AVI__hdlr ();
    void AVI__hdlr_avih ();
    void AVI__hdlr_JUNK ();
    void AVI__hdlr_strl ();
    void AVI__hdlr_strl_indx ();
    void AVI__hdlr_strl_JUNK ();
    void AVI__hdlr_strl_strd ();
    void AVI__hdlr_strl_strf ();
    void AVI__hdlr_strl_strf_auds ();
    void AVI__hdlr_strl_strf_auds_Options();
    void AVI__hdlr_strl_strf_iavs ();
    void AVI__hdlr_strl_strf_mids ();
    void AVI__hdlr_strl_strf_txts ();
    void AVI__hdlr_strl_strf_vids ();
    void AVI__hdlr_strl_strh ();
    void AVI__hdlr_strl_strn ();
    void AVI__hdlr_strl_vprp ();
    void AVI__hdlr_odml ();
    void AVI__hdlr_odml_dmlh ();
    void AVI__hdlr_xxxx ();
    void AVI__idx1 ();
    void AVI__INFO ();
    void AVI__INFO_JUNK ();
    void AVI__INFO_xxxx ();
    void AVI__JUNK ();
    void AVI__movi ();
    void AVI__movi_xxxx ();
    void AVI__movi_xxxx___db ();
    void AVI__movi_xxxx___dc ();
    void AVI__movi_xxxx___tx ();
    void AVI__movi_xxxx___wb ();
    void AVI__movi_xxxx___xx ();
    void AVI__movi_rec_ ();
    void AVI__movi_rec__xxxx ();
    void AVI__GMET ();
    void AVIX ();
    void AVIX_idx1 ();
    void AVIX_movi ();
    void AVIX_movi_xxxx ();
    void AVIX_movi_rec_ ();
    void AVIX_movi_rec__xxxx ();
    void menu ();
    void PAL_ ();
    void RDIB ();
    void RMID ();
    void RMMP ();
    void WAVE ();
    void WAVE_data ();
    void WAVE_fact ();
    void WAVE_fmt_ ();

    //Temp
    int32u AVI__hdlr_strl_strh_StreamType;
    int32u AVI__hdlr_strl_strh_Scale;
    int32u AVI__hdlr_strl_strh_Rate;
    int32u AVI__hdlr_strl_strh_Length;
    Ztring AVI__hdlr_strl_strh_Codec;
    int32u AVI__hdlr_strl_strh_Width;
    int32u AVI__hdlr_strl_strh_Height;
    int64u AVI__movi_Size; //Size of the data part (AVI and AVIX)
    bool   Rec_Present;

    //Helpers
    bool Element_Name_Get();
    bool Element_Size_Get();
    void SetLastByID(int8u ID);
};

//***************************************************************************
// Const
//***************************************************************************

namespace Riff
{
    const int32u RIFF=0x52494646;
    const int32u LIST=0x4C495354;

    const int32u AVI_=0x41564920;
    const int32u AVI__cset=0x63736574;
    const int32u AVI__exif=0x65786966;
    const int32u AVI__exif_ecor=0x65636F72;
    const int32u AVI__exif_emdl=0x656D646C;
    const int32u AVI__exif_emnt=0x656D6E74;
    const int32u AVI__exif_erel=0x6572656C;
    const int32u AVI__exif_etim=0x6574696D;
    const int32u AVI__exif_eucm=0x6575636D;
    const int32u AVI__exif_ever=0x65766572;
    const int32u AVI__goog=0x676F6F67;
    const int32u AVI__goog_GDAT=0x47444154;
    const int32u AVI__GMET=0x474D4554;
    const int32u AVI__hdlr=0x6864726C;
    const int32u AVI__hdlr_avih=0x61766968;
    const int32u AVI__hdlr_JUNK=0x4A554E4B;
    const int32u AVI__hdlr_strl=0x7374726C;
    const int32u AVI__hdlr_strl_indx=0x696E6478;
    const int32u AVI__hdlr_strl_JUNK=0x4A554E4B;
    const int32u AVI__hdlr_strl_strd=0x73747264;
    const int32u AVI__hdlr_strl_strf=0x73747266;
    const int32u AVI__hdlr_strl_strh=0x73747268;
    const int32u AVI__hdlr_strl_strh_auds=0x61756473;
    const int32u AVI__hdlr_strl_strh_iavs=0x69617673;
    const int32u AVI__hdlr_strl_strh_mids=0x6D696473;
    const int32u AVI__hdlr_strl_strh_vids=0x76696473;
    const int32u AVI__hdlr_strl_strh_txts=0x74787473;
    const int32u AVI__hdlr_strl_strn=0x7374726E;
    const int32u AVI__hdlr_strl_vprp=0x76707270;
    const int32u AVI__hdlr_odml=0x6F646D6C;
    const int32u AVI__hdlr_odml_dmlh=0x646D6C68;
    const int32u AVI__idx1=0x69647831;
    const int32u AVI__INFO=0x494E464F;
    const int32u AVI__INFO_IARL=0x4941524C;
    const int32u AVI__INFO_IART=0x49415254;
    const int32u AVI__INFO_ICMS=0x49434D53;
    const int32u AVI__INFO_ICMT=0x49434D54;
    const int32u AVI__INFO_ICOP=0x49434F50;
    const int32u AVI__INFO_ICRD=0x49435244;
    const int32u AVI__INFO_ICRP=0x49435250;
    const int32u AVI__INFO_IDIM=0x4944494D;
    const int32u AVI__INFO_IDIT=0x49444954;
    const int32u AVI__INFO_IDPI=0x49445049;
    const int32u AVI__INFO_IENG=0x49454E47;
    const int32u AVI__INFO_IGNR=0x49474E52;
    const int32u AVI__INFO_IKEY=0x494B4559;
    const int32u AVI__INFO_ILGT=0x494C4754;
    const int32u AVI__INFO_ILNG=0x494C4E47;
    const int32u AVI__INFO_IMED=0x494D4544;
    const int32u AVI__INFO_INAM=0x494E414D;
    const int32u AVI__INFO_IPLT=0x49504C54;
    const int32u AVI__INFO_IPRD=0x49505244;
    const int32u AVI__INFO_IRTD=0x49525444;
    const int32u AVI__INFO_ISBJ=0x4953424A;
    const int32u AVI__INFO_ISFT=0x49534654;
    const int32u AVI__INFO_ISHP=0x49534850;
    const int32u AVI__INFO_ISRC=0x49535243;
    const int32u AVI__INFO_ISRF=0x49535246;
    const int32u AVI__INFO_ITCH=0x49544348;
    const int32u AVI__INFO_JUNK=0x4A554E4B;
    const int32u AVI__JUNK=0x4A554E4B;
    const int32u AVI__movi=0x6D6F7669;
    const int32u AVI__movi_rec_=0x72656320;
    const int32u AVI__movi_xxxx___db=0x00006462;
    const int32u AVI__movi_xxxx___dc=0x00006463;
    const int32u AVI__movi_xxxx___tx=0x00007478;
    const int32u AVI__movi_xxxx___wb=0x00007762;
    const int32u AVIX=0x41564958;
    const int32u AVIX_idx1=0x69647831;
    const int32u AVIX_movi=0x6D6F7669;
    const int32u AVIX_movi_rec_=0x72656320;
    const int32u menu=0x6D656E75;
    const int32u PAL_=0x50414C20;
    const int32u RDIB=0x52444942;
    const int32u RMID=0x524D4944;
    const int32u RMMP=0x524D4D50;
    const int32u WAVE=0x57415645;
    const int32u WAVE_data=0x64617461;
    const int32u WAVE_fact=0x66616374;
    const int32u WAVE_fmt_=0x666D7420;
}

} //NameSpace

#endif
