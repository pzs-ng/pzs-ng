// File_Mpeg4 - Info for MPEG-4 files
// Copyright (C) 2004-2007 Jerome Martinez, Zen@MediaArea.net
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
// Information about MPEG-4 files
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef MediaInfo_File_Mpeg4H
#define MediaInfo_File_Mpeg4H
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/File__Base.h"
#include <map>
class File_MpegPs;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Class File_Mpeg4
//***************************************************************************

class File_Mpeg4 : public File__Base
{
protected :
    //Format
    void Read_Buffer_Init ();
    void Read_Buffer_Continue ();

    //Information
    void HowTo (stream_t StreamKind);

private :
    //Buffer
    bool Buffer_Parse();
    bool Element_Parse();
    int8u  Element_Level;
    int32u Element_Size;
    int32u Element_HeaderSize;
    int32u Element_Name[10]; //4CC of the list type
    int64u Element_Next[10];

    //mdat specific
    void mdat_Parse();
    bool mdat_MustParse;
    File__Base* mdat_Info;

    //Elements
    void free();
    void ftyp();
    void idat();
    void idsc();
    void mdat();
    void moov();
    void moov_cmov();
    void moov_cmov_cmvd();
    void moov_cmov_dcom();
    void moov_iods();
    void moov_meta();
    void moov_meta_hdlr();
    void moov_meta_bxml();
    void moov_meta_keys();
    void moov_meta_keys_mdta();
    void moov_meta_ilst();
    void moov_meta_ilst_xxxx();
    void moov_meta_ilst_xxxx_data();
    void moov_meta_ilst_xxxx_mean();
    void moov_meta_ilst_xxxx_name();
    void moov_meta_xml();
    void moov_mvhd();
    void moov_trak();
    void moov_trak_edts();
    void moov_trak_edts_elst();
    void moov_trak_edts_elst_list();
    void moov_trak_mdia();
    void moov_trak_mdia_hdlr();
    void moov_trak_mdia_mdhd();
    void moov_trak_mdia_minf();
    void moov_trak_mdia_minf_hint();
    void moov_trak_mdia_minf_smhd();
    void moov_trak_mdia_minf_vmhd();
    void moov_trak_mdia_minf_stbl();
    void moov_trak_mdia_minf_stbl_stco();
    void moov_trak_mdia_minf_stbl_stsd();
    void moov_trak_mdia_minf_stbl_stsd_xxxx();
    void moov_trak_mdia_minf_stbl_stsd_xxxxSound();
    void moov_trak_mdia_minf_stbl_stsd_xxxxVideo();
    void moov_trak_mdia_minf_stbl_stsd_xxxx_alac();
    void moov_trak_mdia_minf_stbl_stsd_xxxx_chan();
    void moov_trak_mdia_minf_stbl_stsd_xxxx_esds();
    void moov_trak_mdia_minf_stbl_stsd_xxxx_wave();
    void moov_trak_mdia_minf_stbl_stsd_xxxx_wave_frma();
    void moov_trak_mdia_minf_stbl_stsz();
    void moov_trak_mdia_minf_stbl_stts();
    void moov_trak_tapt();
    void moov_trak_tapt_clef();
    void moov_trak_tapt_prof();
    void moov_trak_tapt_enof();
    void moov_trak_tkhd();
    void moov_trak_tref();
    void moov_trak_tref_tmcd();
    void moov_udta();
    void moov_udta_chpl();
    void moov_udta_cprt();
    void moov_udta_hnti();
    void moov_udta_hnti_rtp();
    void moov_udta_meta();
    void moov_udta_meta_hdlr();
    void moov_udta_meta_ilst();
    void moov_udta_meta_ilst_xxxx();
    void moov_udta_meta_ilst_xxxx_data();
    void moov_udta_meta_ilst_xxxx_mean();
    void moov_udta_meta_ilst_xxxx_name();
    void moov_udta_xxxx();
    void pckg();
    void pnot();
    void skip();
    void wide();

    //Descriptors
    void Descriptors                (size_t Esds_Offset, size_t Buffer_Size);
    void Descriptors_ES             (size_t Esds_Offset, size_t Buffer_Size);
    void Descriptors_DecConfig      (size_t Esds_Offset, size_t Buffer_Size);
    void Descriptors_DecSpecific    (size_t Esds_Offset, size_t Buffer_Size);
    void Descriptors_SLConfig       (size_t Esds_Offset, size_t Buffer_Size);
    void Descriptors_Profiles       (size_t Esds_Offset, size_t Buffer_Size);

    //Helpers
    bool Element_Level_Get();
    bool Element_Name_Get();
    bool Element_Size_Get();
    char* Language_Get(int Language);
    enum method
    {
        Method_None,
        Method_String,
        Method_Integer,
        Method_Binary,
    };
    method Metadata_Get(std::string &Parameter, int32u Meta);
    method Metadata_Get(std::string &Parameter, const std::string &Meta);

    //Temp
    bool List;
    int32u moov_cmov_dcom_Compressor;
    int32u moov_meta_hdlr_Type;
    std::string moov_meta_ilst_xxxx_name_Name;
    int32u moov_trak_mdia_mdhd_PlayTime;
    int32u moov_trak_tkhd_TrackID;
    int32u moov_trak_mdia_minf_stbl_stco_Parse;
    char   Language_Result[4];
    int32u TimeScale;

    std::vector<std::string> moov_udta_meta_keys_List;
    size_t moov_udta_meta_keys_ilst_Pos;
    bool Parse;
    std::vector<int32u>                    stco_ID;
    std::map<int32u, std::vector<int64u>*> stco;
    std::map<int32u, std::vector<int32u>*> stsz;
};

//***************************************************************************
// Const
//***************************************************************************

namespace Mpeg4
{
    const int32u free=0x66726565;
    const int32u ftyp=0x66747970;
    const int32u ftyp_qt=0x71742020;
    const int32u ftyp_isom=0x69736F6D;
    const int32u idat=0x69646174;
    const int32u idsc=0x69647363;
    const int32u mdat=0x6D646174;
    const int32u moov=0x6D6F6F76;
    const int32u moov_cmov=0x636D6F76;
    const int32u moov_cmov_cmvd=0x636D7664;
    const int32u moov_cmov_dcom=0x64636F6D;
    const int32u moov_cmov_dcom_zlib=0x7A6C6962;
    const int32u moov_iods=0x696F6473;
    const int32u moov_meta=0x6D657461;
    const int32u moov_meta______=0x2D2D2D2D;
    const int32u moov_meta___ART=0xA9415254;
    const int32u moov_meta___alb=0xA9616C62;
    const int32u moov_meta___aut=0xA9617574;
    const int32u moov_meta___cmt=0xA9636D74;
    const int32u moov_meta___cpy=0xA9637079;
    const int32u moov_meta___day=0xA9646179;
    const int32u moov_meta___des=0xA9646573;
    const int32u moov_meta___dir=0xA9646972;
    const int32u moov_meta___dis=0xA9646973;
    const int32u moov_meta___edl=0xA965646C;
    const int32u moov_meta___fmt=0xA9666D74;
    const int32u moov_meta___gen=0xA967656E;
    const int32u moov_meta___hos=0xA9686F73;
    const int32u moov_meta___inf=0xA9696E66;
    const int32u moov_meta___key=0xA96B6579;
    const int32u moov_meta___mak=0xA96D616B;
    const int32u moov_meta___mod=0xA96D6F64;
    const int32u moov_meta___nam=0xA96E616D;
    const int32u moov_meta___prd=0xA9707264;
    const int32u moov_meta___PRD=0xA9505244;
    const int32u moov_meta___prf=0xA9707266;
    const int32u moov_meta___req=0xA9726571;
    const int32u moov_meta___src=0xA9737263;
    const int32u moov_meta___swr=0xA9737772;
    const int32u moov_meta___too=0xA9746F6F;
    const int32u moov_meta___wrn=0xA977726E;
    const int32u moov_meta___wrt=0xA9777274;
    const int32u moov_meta__cpil=0x6370696C;
    const int32u moov_meta__disk=0x6469736B;
    const int32u moov_meta__name=0x6E616D65;
    const int32u moov_meta__trkn=0x74726B6E;
    const int32u moov_meta__tmpo=0x746D706F;
    const int32u moov_meta_hdlr=0x68646C72;
    const int32u moov_meta_hdlr_mdir=0x6D646972;
    const int32u moov_meta_hdlr_mdta=0x6D647461;
    const int32u moov_meta_hdlr_mp7b=0x6D703762;
    const int32u moov_meta_hdlr_mp7t=0x6D703774;
    const int32u moov_meta_bxml=0x62786D6C;
    const int32u moov_meta_keys=0x6B657973;
    const int32u moov_meta_keys_mdta=0x6D647461;
    const int32u moov_meta_ilst=0x696C7374;
    const int32u moov_meta_ilst_xxxx_data=0x64617461;
    const int32u moov_meta_ilst_xxxx_mean=0x6D65616E;
    const int32u moov_meta_ilst_xxxx_name=0x6E616D65;
    const int32u moov_meta_xml=0x786D6C20;
    const int32u moov_mvhd=0x6D766864;
    const int32u moov_trak=0x7472616B;
    const int32u moov_trak_edts=0x65647473;
    const int32u moov_trak_edts_elst=0x656C7374;
    const int32u moov_trak_mdia=0x6D646961;
    const int32u moov_trak_mdia_hdlr=0x68646C72;
    const int32u moov_trak_mdia_hdlr_MPEG=0x4D504547;
    const int32u moov_trak_mdia_hdlr_soun=0x736F756E;
    const int32u moov_trak_mdia_hdlr_subp=0x73756270;
    const int32u moov_trak_mdia_hdlr_text=0x74657874;
    const int32u moov_trak_mdia_hdlr_vide=0x76696465;
    const int32u moov_trak_mdia_mdhd=0x6D646864;
    const int32u moov_trak_mdia_minf=0x6D696E66;
    const int32u moov_trak_mdia_minf_hint=0x68696E74;
    const int32u moov_trak_mdia_minf_smhd=0x736D6864;
    const int32u moov_trak_mdia_minf_stbl=0x7374626C;
    const int32u moov_trak_mdia_minf_stbl_stco=0x7374636F;
    const int32u moov_trak_mdia_minf_stbl_stsd=0x73747364;
    const int32u moov_trak_mdia_minf_stbl_stsd_alac=0x616C6163;
    const int32u moov_trak_mdia_minf_stbl_stsd_mp4a=0x6D703461;
    const int32u moov_trak_mdia_minf_stbl_stsd_mp4s=0x6D703473;
    const int32u moov_trak_mdia_minf_stbl_stsd_mp4v=0x6D703476;
    const int32u moov_trak_mdia_minf_stbl_stsd_xxxx_alac=0x616C6163;
    const int32u moov_trak_mdia_minf_stbl_stsd_xxxx_chan=0x6368616E;
    const int32u moov_trak_mdia_minf_stbl_stsd_xxxx_esds=0x65736473;
    const int32u moov_trak_mdia_minf_stbl_stsd_xxxx_wave=0x77617665;
    const int32u moov_trak_mdia_minf_stbl_stsd_xxxx_wave_frma=0x66726D61;
    const int32u moov_trak_mdia_minf_stbl_stsz=0x7374737A;
    const int32u moov_trak_mdia_minf_stbl_stts=0x73747473;
    const int32u moov_trak_mdia_minf_vmhd=0x766D6864;
    const int32u moov_trak_tapt=0x74617074;
    const int32u moov_trak_tapt_clef=0x636C6566;
    const int32u moov_trak_tapt_prof=0x70726F66;
    const int32u moov_trak_tapt_enof=0x656E6F66;
    const int32u moov_trak_tkhd=0x746B6864;
    const int32u moov_trak_tref=0x74726566;
    const int32u moov_trak_tref_tmcd=0x746D6364;
    const int32u moov_udta=0x75647461;
    const int32u moov_udta_AllF=0x416C6C46;
    const int32u moov_udta_chpl=0x6368706C;
    const int32u moov_udta_cprt=0x63707274;
    const int32u moov_udta_hinf=0x68696E66;
    const int32u moov_udta_hnti=0x686E7469;
    const int32u moov_udta_hnti_rtp=0x72747020;
    const int32u moov_udta_LOOP=0x4C4F4F50;
    const int32u moov_udta_meta=0x6D657461;
    const int32u moov_udta_meta_hdlr=0x68646C72;
    const int32u moov_udta_meta_ilst=0x696C7374;
    const int32u moov_udta_meta_ilst_xxxx_data=0x64617461;
    const int32u moov_udta_meta_ilst_xxxx_mean=0x6D65616E;
    const int32u moov_udta_meta_ilst_xxxx_name=0x6E616D65;
    const int32u moov_udta_ptv =0x70747620;
    const int32u moov_udta_Sel0=0x53656C30;
    const int32u moov_udta_WLOC=0x574C4F43;
    const int32u pckg=0x70636B67;
    const int32u pnot=0x706E6F74;
    const int32u skip=0x736B6970;
    const int32u wide=0x77696465;
}

} //NameSpace

#endif
