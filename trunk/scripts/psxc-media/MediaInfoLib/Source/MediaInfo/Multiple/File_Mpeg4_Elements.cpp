// File_Mpeg4 - Info for MPEG-4 files
// Copyright (C) 2005-2007 Jerome Martinez, Zen@MediaArea.net
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
// Elements part
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
// Compilation condition
#if defined(MEDIAINFO_MPEG4_YES) || (!defined(MEDIAINFO_MULTI_NO) && !defined(MEDIAINFO_MPEG4_NO))
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <wx/wxprec.h>
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#include "MediaInfo/Multiple/File_Mpeg4.h"
#include <ZenLib/Utils.h>
#include <zlib/zlib.h>
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//---------------------------------------------------------------------------
extern void PlayTime_PlayTime123 (const Ztring &Value, ZtringListList &List); //TODO: remove it
//---------------------------------------------------------------------------

//***************************************************************************
// Format
//***************************************************************************

//---------------------------------------------------------------------------
// Element parse
//
bool File_Mpeg4::Element_Parse()
{
    #define FLUSH() \
    if (Config.Details_Get()) \
    { \
        Details_Add_Element_Flush(); \
    } \

    #define PURGE() \
    if (Config.Details_Get()) \
    { \
        Details_Add_Element_Purge(); \
    } \

    #define ATOM_BEGIN \
    if (Level!=Element_Level) \
    { \
        Level++; \
        switch (Element_Name[Level]) \
        {

        #define SKIP(_ATOM) \
                case Mpeg4::_ATOM : \
                            FLUSH() \
                            return true; \

        #define ATOM(_ATOM) \
                case Mpeg4::_ATOM : \
                        if (Level==Element_Level) \
                        { \
                            if (Element_Next[Element_Level]-File_Offset<=Buffer_Size || Mpeg4::_ATOM==Mpeg4::mdat || Mpeg4::_ATOM==Mpeg4::free) \
                            { \
                                _ATOM(); \
                                FLUSH() \
                                if (!List) \
                                    return true; \
                            } \
                            else \
                            { \
                                Buffer_Offset-=Element_HeaderSize; \
                                Element_Level--; \
                                PURGE() \
                                return false; \
                            } \
                        } \

        #define ATOM_DEFAULT(_ATOM) \
                default : \
                        if (Level==Element_Level) \
                        { \
                            if (Element_Next[Element_Level]-File_Offset<=Buffer_Size) \
                            { \
                                _ATOM(); \
                                FLUSH() \
                                if (!List) \
                                    return true; \
                            } \
                            else \
                            { \
                                Buffer_Offset-=Element_HeaderSize; \
                                Element_Level--; \
                                PURGE() \
                                return false; \
                            } \
                        } \

        #define LIST(_ATOM) \
                case Mpeg4::_ATOM : \
                        if (Level==Element_Level) \
                        { \
                            _ATOM(); \
                            FLUSH() \
                        } \

        #define LIST_DEFAULT(_ATOM) \
                default : \
                        if (Level==Element_Level) \
                        { \
                            _ATOM(); \
                            FLUSH() \
                        } \

    #define ATOM_END \
        } \
    } \
    else \
    { \
        List=true; \
    } \
    break; \

    //Details
    if (Config.Details_Get())
    {
        DETAILLEVEL_SET(0);
        std::string S1;
        S1.append(1, (char)((Element_Name[Element_Level]&0xFF000000)>>24));
        S1.append(1, (char)((Element_Name[Element_Level]&0x00FF0000)>>16));
        S1.append(1, (char)((Element_Name[Element_Level]&0x0000FF00)>> 8));
        S1.append(1, (char)((Element_Name[Element_Level]&0x000000FF)>> 0));
        Ztring Z1;
        Z1.From_Local(S1.c_str());
        if (Z1.empty())
            Details_Add_Element(Element_Level-1, _T("(empty)"), Element_Size);
        else
            Details_Add_Element(Element_Level-1, Z1, Element_Size);
    }

    //Init
    Buffer_Offset+=Element_HeaderSize;
    int8u Level=0;
    List=false;

    //Hierarchy
    ATOM_BEGIN
    ATOM(free)
    ATOM(ftyp)
    ATOM(idat)
    ATOM(idsc)
    ATOM(mdat)
    LIST(moov)
        ATOM_BEGIN
        LIST(moov_cmov)
            ATOM_BEGIN
            ATOM(moov_cmov_dcom)
            ATOM(moov_cmov_cmvd)
            ATOM_END
        ATOM(moov_iods)
        LIST(moov_meta)
            ATOM_BEGIN
            ATOM(moov_meta_hdlr)
            ATOM(moov_meta_keys)
                ATOM_BEGIN
                ATOM(moov_meta_keys_mdta)
                ATOM_END
            LIST(moov_meta_ilst)
                ATOM_BEGIN
                LIST_DEFAULT (moov_meta_ilst_xxxx);
                    ATOM_BEGIN
                    ATOM (moov_meta_ilst_xxxx_data);
                    ATOM (moov_meta_ilst_xxxx_mean);
                    ATOM (moov_meta_ilst_xxxx_name);
                    ATOM_END
                ATOM_END
            ATOM_END
        ATOM(moov_mvhd)
        LIST(moov_trak)
            ATOM_BEGIN
            LIST(moov_trak_edts)
                ATOM_BEGIN
                ATOM(moov_trak_edts_elst)
                ATOM_END
            LIST(moov_trak_mdia)
                ATOM_BEGIN
                ATOM(moov_trak_mdia_hdlr)
                ATOM(moov_trak_mdia_mdhd)
                LIST(moov_trak_mdia_minf)
                    ATOM_BEGIN
                    ATOM(moov_trak_mdia_minf_smhd)
                    LIST(moov_trak_mdia_minf_stbl)
                        ATOM_BEGIN
                        ATOM(moov_trak_mdia_minf_stbl_stco)
                        ATOM(moov_trak_mdia_minf_stbl_stsd)
                            ATOM_BEGIN
                            ATOM_DEFAULT(moov_trak_mdia_minf_stbl_stsd_xxxx)
                                ATOM_BEGIN
                                ATOM(moov_trak_mdia_minf_stbl_stsd_xxxx_alac)
                                ATOM(moov_trak_mdia_minf_stbl_stsd_xxxx_chan)
                                ATOM(moov_trak_mdia_minf_stbl_stsd_xxxx_esds)
                                LIST(moov_trak_mdia_minf_stbl_stsd_xxxx_wave)
                                    ATOM_BEGIN
                                    ATOM(moov_trak_mdia_minf_stbl_stsd_xxxx_esds)
                                    ATOM(moov_trak_mdia_minf_stbl_stsd_xxxx_wave_frma)
                                    ATOM_END
                                ATOM_END
                            ATOM_END
                        ATOM(moov_trak_mdia_minf_stbl_stsz)
                        ATOM(moov_trak_mdia_minf_stbl_stts)
                        ATOM_END
                    ATOM(moov_trak_mdia_minf_vmhd)
                    ATOM_END
                ATOM_END
            LIST(moov_trak_tapt)
                ATOM_BEGIN
                ATOM(moov_trak_tapt_clef)
                ATOM(moov_trak_tapt_prof)
                ATOM(moov_trak_tapt_enof)
                ATOM_END
            ATOM(moov_trak_tkhd)
            LIST(moov_trak_tref)
                ATOM_BEGIN
                ATOM(moov_trak_tref_tmcd)
                ATOM_END
            ATOM_END
        LIST(moov_udta)
            ATOM_BEGIN
            SKIP(moov_udta_AllF)
            ATOM(moov_udta_chpl)
            ATOM(moov_udta_cprt)
            SKIP(moov_udta_hinf)
            LIST(moov_udta_hnti)
                ATOM_BEGIN
                ATOM(moov_udta_hnti_rtp)
                ATOM_END
            SKIP(moov_udta_LOOP)
            ATOM(moov_udta_meta)
                ATOM_BEGIN
                ATOM(moov_udta_meta_hdlr)
                LIST(moov_udta_meta_ilst)
                    ATOM_BEGIN
                    LIST_DEFAULT (moov_udta_meta_ilst_xxxx);
                        ATOM_BEGIN
                        ATOM (moov_udta_meta_ilst_xxxx_data);
                        ATOM (moov_udta_meta_ilst_xxxx_mean);
                        ATOM (moov_udta_meta_ilst_xxxx_name);
                        ATOM_END
                    ATOM_END
                ATOM_END
            SKIP(moov_udta_ptv )
            SKIP(moov_udta_Sel0)
            SKIP(moov_udta_WLOC)
            ATOM_DEFAULT (moov_udta_xxxx); //User data
            ATOM_END
        ATOM_END
    ATOM(pckg)
    ATOM(pnot)
    ATOM(skip)
    ATOM(wide)
    default : ;
    }}
    FLUSH()

    //List
    if (List)
    {
        while (Buffer_Parse());
        if (Element_Next[Element_Level]-File_Offset>Buffer_Size)
            return false; //Not enough bytes
    }
    
    return true; //Atom unknown
}

//***************************************************************************
// Elements
//***************************************************************************

#define NAME(ELEMENT_NAME) \
    if (Config.Details_Get()>=Details_Level_Min) \
    { \
        Details_Add_Element(ELEMENT_NAME); \
    } \

#define NAME_VERSION_FLAG(ELEMENT_NAME) \
    if (Element_Size<4) \
        return; \
    int8u Version=BigEndian2int8u(Buffer+Buffer_Offset+0); \
    int32u Flags =BigEndian2int32u(Buffer+Buffer_Offset+0)&0x00FFFFFF; \
    \
    if (Config.Details_Get()>=Details_Level_Min) \
    { \
        Details_Add_Element(ELEMENT_NAME); \
        Details_Add_Info(0, "Version", Version); \
        Details_Add_Info(1, "Flags", Flags); \
    } \

#define INTEGRITY(ELEMENT_SIZE) \
     if (Element_Size<ELEMENT_SIZE) \
        return; \

#define INTEGRITY1(ELEMENT_SIZE0) \
     if (Version==0 && Element_Size<ELEMENT_SIZE0 \
      || Version>=1) \
        return; \

#define INTEGRITY2(ELEMENT_SIZE0, ELEMENT_SIZE1) \
     if (Version==0 && Element_Size<ELEMENT_SIZE0 \
      || Version==1 && Element_Size<ELEMENT_SIZE1 \
      || Version>=2) \
        return; \

#define LIST_SET(OFFSET) \
    Buffer_Offset+=OFFSET; \
    Element_Size-=OFFSET; \
    List=true; \

#define COHERANCY(TOVALIDATE, ERRORTEXT) \
    if (TOVALIDATE) \
    { \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(0, "ERROR: " ERRORTEXT, ""); \
        } \
        return; \
    } \

#define BEGIN \
    size_t Stream_Pos=0; \

#define BEGIN_VERSION_FLAG \
    if (Config.Details_Get()>=0.9) \
    { \
        Details_Add_Info(0, "Version", Version); \
        Details_Add_Info(1, "Flags", Flags); \
    } \
    size_t Stream_Pos=4; \

#define END \
    { \
    } \

#define GET_I1(_INFO, _NAME) \
    { \
        INTEGRITY(Stream_Pos+1) \
        _INFO=BigEndian2int8u(Buffer+Buffer_Offset+Stream_Pos); \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Stream_Pos, #_NAME, _INFO); \
        } \
        Stream_Pos+=1; \
    }

#define GET_I2(_INFO, _NAME) \
    { \
        INTEGRITY(Stream_Pos+2) \
        _INFO=BigEndian2int16u(Buffer+Buffer_Offset+Stream_Pos); \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Stream_Pos, #_NAME, _INFO); \
        } \
        Stream_Pos+=2; \
    }

#define GET_I4(_INFO, _NAME) \
    { \
        INTEGRITY(Stream_Pos+4) \
        _INFO=BigEndian2int32u(Buffer+Buffer_Offset+Stream_Pos); \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Stream_Pos, #_NAME, _INFO); \
        } \
        Stream_Pos+=4; \
    }

#define GET_CH(_BYTES, _INFO, _NAME) \
    { \
        INTEGRITY(Stream_Pos+_BYTES) \
        _INFO.assign((const char*)(Buffer+Buffer_Offset+Stream_Pos), _BYTES); \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Stream_Pos, #_NAME, _INFO); \
        } \
        Stream_Pos+=_BYTES; \
    }

#define GET_C4(_INFO, _NAME) \
    { \
        INTEGRITY(Stream_Pos+4) \
        _INFO=CC4(Buffer+Buffer_Offset+Stream_Pos); \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Stream_Pos, #_NAME, Buffer+Buffer_Offset+Stream_Pos, 4); \
        } \
        Stream_Pos+=4; \
    }

#define PEEK_I1(_INFO) \
    { \
        INTEGRITY(Stream_Pos+1) \
        _INFO=BigEndian2int8u(Buffer+Buffer_Offset+Stream_Pos); \
    }

#define SKIP_I1(_NAME) \
    { \
        INTEGRITY(Stream_Pos+1) \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Stream_Pos, #_NAME, BigEndian2int8u(Buffer+Buffer_Offset+Stream_Pos)); \
        } \
        Stream_Pos+=1; \
    } \

#define SKIP_I2(_NAME) \
    { \
        INTEGRITY(Stream_Pos+2) \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Stream_Pos, #_NAME, BigEndian2int16u(Buffer+Buffer_Offset+Stream_Pos)); \
        } \
        Stream_Pos+=2; \
    } \

#define SKIP_I4(_NAME) \
    { \
        INTEGRITY(Stream_Pos+4) \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Stream_Pos, #_NAME, BigEndian2int32u(Buffer+Buffer_Offset+Stream_Pos)); \
        } \
        Stream_Pos+=4; \
    } \

#define SKIP_CH(_BYTES, _NAME) \
    { \
        INTEGRITY(Stream_Pos+_BYTES) \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Stream_Pos, #_NAME, Buffer+Buffer_Offset+Stream_Pos, _BYTES); \
        } \
        Stream_Pos+=_BYTES; \
    } \

#define SKIP_C4(_NAME) \
    { \
        INTEGRITY(Stream_Pos+4) \
        if (Config.Details_Get()>=Details_Level_Min) \
        { \
            Details_Add_Info(Stream_Pos, #_NAME, Buffer+Buffer_Offset+Stream_Pos, 4); \
        } \
        Stream_Pos+=4; \
    } \

//---------------------------------------------------------------------------
// Atom "free"
//
void File_Mpeg4::free()
{
    NAME("Free space")
}

//---------------------------------------------------------------------------
// Atom "ftyp", Brand info, 8+ bytes
// MajorBrand                       4 bytes, Pos=0
// MajorBrandVersion                4 bytes, Pos=4
// CompatibleBrand_1                4 bytes, Pos=8
// CompatibleBrand_...              4 bytes, Pos=12
//
void File_Mpeg4::ftyp()
{
    NAME("File Type")
    INTEGRITY(8)

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(0, "MajorBrand", Buffer+Buffer_Offset+0, 4);
        Details_Add_Info(4, "MajorBrandVersion", BigEndian2int32u(Buffer+Buffer_Offset+4), 4);
        for (size_t Pos=8; Pos<Element_Size; Pos+=4)
            Details_Add_Info(Pos, "CompatibleBrand", Buffer+Buffer_Offset+Pos, 4);
    }

    //Filling
    Stream_Prepare(Stream_General);
    Fill("Format", Buffer+Buffer_Offset+0, 4);
}

//---------------------------------------------------------------------------
// Atom "idat", ?, ? bytes
//
void File_Mpeg4::idat()
{
    //Details
    if (Config.Details_Get())
    {
        Details_Add_Element("QTI");
    }

    //Filling
    Stream_Prepare(Stream_General);
    Fill("Format", "QTI");
}

//---------------------------------------------------------------------------
// Atom "idsc", ?, ? bytes
//
void File_Mpeg4::idsc()
{
    //Details
    if (Config.Details_Get())
    {
        Details_Add_Element("QTI");
    }

    //Filling
    Stream_Prepare(Stream_General);
    Fill("Format", "QTI");
}

//---------------------------------------------------------------------------
// Atom "mdat", , bytes
//
void File_Mpeg4::mdat()
{
    NAME("Data")

    //Filling
    if (Count_Get(Stream_General)==0)
    {
        Stream_Prepare(Stream_General);
        Fill("Format", "QT"); //if there is no ftyp atom, this is an old Quictime file
    }

    //Parse mdat if needed
    if (stco_ID.size()>0)
    {
        //Currently, we can only parse one chunk in one stream (for MPEG-PS in MOV), must be improved
        if (stco_ID.size()==1 && stco[stco_ID[0]]->size()==1)
        {
            mdat_MustParse=true;
            mdat_Parse();
            return;
        }
    }
}

//---------------------------------------------------------------------------
// Atom "moov", File header
// List...
//
void File_Mpeg4::moov()
{
    NAME("File header")

    //Filling
    if (Count_Get(Stream_General)==0)
    {
        Stream_Prepare(Stream_General);
        Fill("Format", "QT"); //if there is no ftyp atom, this is an old Quictime file
    }
}

//---------------------------------------------------------------------------
// Atom "cmov", compressed file header
// List...
//
void File_Mpeg4::moov_cmov()
{
    NAME("Compressed file header")
}

//---------------------------------------------------------------------------
// Atom "cmvd", Data, 0+ bytes
// Datas                            X bytes, Pos=0
//
void File_Mpeg4::moov_cmov_cmvd()
{
    NAME("Data")

    switch (moov_cmov_dcom_Compressor)
    {
        case Mpeg4::moov_cmov_dcom_zlib :
            {
                //Sizes
                unsigned long Source_Size=Element_Size-4;
                unsigned long Dest_Size=BigEndian2int32u(Buffer+Buffer_Offset);

                //Details
                if (Config.Details_Get())
                {
                    Details_Add_Info(0, "Dest Size", (size_t)Dest_Size);
                }

                //Uncompressing
                const int8u* Dest=new int8u[Dest_Size];
                if (uncompress((Bytef*)Dest, &Dest_Size, (Bytef*)Buffer+Buffer_Offset+4, Source_Size)<0)
                {
                    delete[] Dest;
                    return;
                }
                File_Mpeg4 MI;
                Open_Buffer_Init(&MI, File_Size, File_Offset+Buffer_Offset+4);
                Open_Buffer_Continue(&MI, Dest, Dest_Size);
                Open_Buffer_Finalize(&MI);
                Merge(MI);
                Merge(MI, Stream_General, 0, 0);
            }
            break;
        default: ;
    }


    /*
    //Coherancy test
    if (moov_cmov_dcom_Offset==Error)
        return;

    if (CC4(moov_cmov_dcom_Offset+8)!=CC4("zlib")) //Compressor
        return;

    //Compressed header
    const int8u* Begin2=Begin;
    size_t Begin2_Size=Begin_Size;
    unsigned long SourceLen=BigEndian2int32u((const char*)(Begin+Offset))-12;
    unsigned long DestLen=BigEndian2int32u((const char*)(Begin+Offset)+8);
    const int8u*  Begin2=new int8u[DestLen];
    if (uncompress((Bytef*)Begin2, &DestLen, (Bytef*)Begin+Offset+12, SourceLen)<0)
    {
        delete[] Begin2;
        return;
    }
    size_t Begin2_Size=DestLen;

    //Open a new file
    File_Qt Qt;
    if (Qt.Open(CompleteFileName)<0)
        Qt.Open(Begin2, Begin2_Size);
    Merge(Qt);
    delete[] Begin2; Begin2=NULL;
    */
}

//---------------------------------------------------------------------------
// Atom "dcom", Data compressor name, 4 bytes
// Compressor                       4 bytes, Pos=0
//
void File_Mpeg4::moov_cmov_dcom()
{
    NAME("Data compressor name")
    INTEGRITY(4)

    //Reading
    moov_cmov_dcom_Compressor=CC4(Buffer+Buffer_Offset);

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(0, "Compressor", Buffer+Buffer_Offset, 4);
    }
}

//---------------------------------------------------------------------------
// Atom "iods", Initial object descriptor, 4+ bytes
// Version                          1 bytes, Pos=0
// Flags                            3 bytes, Pos=1
// Descriptors...
//
void File_Mpeg4::moov_iods()
{
    NAME_VERSION_FLAG("Initial object descriptor")
    INTEGRITY1(4)
    Descriptors(4, Element_Size-4);
}

//---------------------------------------------------------------------------
// Atom "meta", Metadatas
// Version                          1 bytes, Pos=0
// Flags                            3 bytes, Pos=1
// List...
//
void File_Mpeg4::moov_meta()
{
    NAME_VERSION_FLAG("Metadatas")
    INTEGRITY1(4)

    //Reading
    moov_meta_hdlr_Type=0;
}

//---------------------------------------------------------------------------
// Atom "bxml", Binary XML (ISO/IEC 14496-12), 4+ bytes
// Note: "hdlr" metadata type should be "mp7b"
// Version                          1 bytes, Pos=0
// Flags                            3 bytes, Pos=1
// Binary XML text                  X bytes, Pos=4
//
void File_Mpeg4::moov_meta_bxml()
{
    NAME_VERSION_FLAG("Binary XML")
    INTEGRITY1(4)
    COHERANCY(moov_meta_hdlr_Type!=Mpeg4::moov_meta_hdlr_mp7b, "Bad meta type")

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(0, "Value", "(Binary XML dump)");
    }
}

//---------------------------------------------------------------------------
// Atom "hdlr", Header (ISO/IEC 14496-12 handler), 24+ bytes
// Version                          1 bytes, Pos=0
// Flags                            3 bytes, Pos=1
// Type                             4 bytes, Pos=4 (filled if Quicktime)
// Metadata type                    4 bytes, Pos=8
// Manufacturer                     4 bytes, Pos=12
// Component reserved flags         4 bytes, Pos=16 (filled if Quicktime)
// Component reserved flags mask    4 bytes, Pos=20 (filled if Quicktime)
// Component type name              X bytes
//
void File_Mpeg4::moov_meta_hdlr()
{
    NAME_VERSION_FLAG("Header")
    INTEGRITY1(24)

    //Reading
    moov_meta_hdlr_Type=CC4(Buffer+Buffer_Offset+8);

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(4, "Type (Quicktime)", Buffer+Buffer_Offset+4, 16);
        Details_Add_Info(8, "Metadata type", Buffer+Buffer_Offset+8, 4);
        Details_Add_Info(12, "Manufacturer", Buffer+Buffer_Offset+12, 4);
        Details_Add_Info(16, "Component reserved flags", BigEndian2int32u(Buffer+Buffer_Offset+16));
        Details_Add_Info(20, "Component reserved flags mask", BigEndian2int32u(Buffer+Buffer_Offset+20));
        Details_Add_Info(24, "Component type name", Buffer+Buffer_Offset+24, Element_Size-24);
    }
}

//---------------------------------------------------------------------------
// Atom "keys", Keys, 8+ bytes
// Note: "hdlr" metadata type should be "mdta"
// Version                          1 bytes, Pos=0
// Flags                            3 bytes, Pos=1
// Count                            4 bytes, Pos=4
// List...
//
void File_Mpeg4::moov_meta_keys()
{
    NAME_VERSION_FLAG("Keys")
    INTEGRITY1(8)
    COHERANCY(moov_meta_hdlr_Type!=Mpeg4::moov_meta_hdlr_mdta, "Bad meta type")

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(4, "Count", BigEndian2int32u(Buffer+Buffer_Offset+4));
    }

    LIST_SET(8)
}

//---------------------------------------------------------------------------
// Atom "mdta", Name, 0+ bytes
// Note: "hdlr" metadata type should be "mdta"
// Value                            X bytes, Pos=0
//
void File_Mpeg4::moov_meta_keys_mdta()
{
    NAME("Name")
    COHERANCY(moov_meta_hdlr_Type!=Mpeg4::moov_meta_hdlr_mdta, "Bad meta type")

    //Reading
    std::string Value((const char*)(Buffer+Buffer_Offset), Element_Size);

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(0, "Value", Value.c_str());
    }

    //Filling
    moov_udta_meta_keys_List.push_back(Value);
}

//---------------------------------------------------------------------------
// Atom "ilst", List
// Note: "hdlr" metadata type should be "mdat" or "mdir"
// List...
//
void File_Mpeg4::moov_meta_ilst()
{
    NAME("List")
    INTEGRITY(8)
    COHERANCY(moov_meta_hdlr_Type!=Mpeg4::moov_meta_hdlr_mdta && moov_meta_hdlr_Type!=Mpeg4::moov_meta_hdlr_mdir, "Bad meta type")

    //Filling
    moov_udta_meta_keys_ilst_Pos=0;
}

//---------------------------------------------------------------------------
// Atom "",
// List...
//
void File_Mpeg4::moov_meta_ilst_xxxx()
{
    COHERANCY(moov_meta_hdlr_Type!=Mpeg4::moov_meta_hdlr_mdta && moov_meta_hdlr_Type!=Mpeg4::moov_meta_hdlr_mdir, "Bad meta type")

    moov_meta_ilst_xxxx_name_Name.clear(); //useful if metadata type = "mdir"
}

//---------------------------------------------------------------------------
void File_Mpeg4::moov_meta_ilst_xxxx_data()
{
    NAME("Data")
    INTEGRITY(8)
    COHERANCY(moov_meta_hdlr_Type!=Mpeg4::moov_meta_hdlr_mdta && moov_meta_hdlr_Type!=Mpeg4::moov_meta_hdlr_mdir, "Bad meta type")

    //Reading
    enum kind {
        Kind_Binary     = 0x00,
        Kind_UTF8       = 0x01,
        Kind_UTF16      = 0x02,
        Kind_String_Mac = 0x03,
        Kind_Jpeg       = 0x0E,
        Kind_ints       = 0x15, //the size of the integer is derived from the container size
        Kind_float32    = 0x16,
        Kind_float64    = 0x17,
    };
    kind Kind=(kind)BigEndian2int32u(Buffer+Buffer_Offset);
    int32u Language=BigEndian2int32u(Buffer+Buffer_Offset+4);
    Ztring Value;
    switch (Kind)
    {
        case Kind_Binary :
                    switch (Element_Name[Element_Level-1])
                    {
                        case Mpeg4::moov_meta__disk :
                            //Reserved              2 bytes
                            //Part/Position         2 bytes
                            //Part/Position_Total   2 bytes
                            //Reserved              2 bytes (Optional?)
                            INTEGRITY(14)
                            Fill(Stream_General, 0, "Part/Position", _T("")); //Clear
                            Fill(Stream_General, 0, "Part/Position", Ztring::ToZtring(BigEndian2int16u(Buffer+Buffer_Offset+10)));
                            Fill(Stream_General, 0, "Part/Position_Total", _T("")); //Clear
                            Fill(Stream_General, 0, "Part/Position_Total", Ztring::ToZtring(BigEndian2int16u(Buffer+Buffer_Offset+12)));
                            return;
                        case Mpeg4::moov_meta__trkn :
                            //Reserved              2 bytes
                            //Track/Position        2 bytes
                            //Track/Position_Total  2 bytes
                            //Reserved              2 bytes (Optional?)
                            INTEGRITY(14)
                            Fill(Stream_General, 0, "Track/Position", _T("")); //Clear
                            Fill(Stream_General, 0, "Track/Position", Ztring::ToZtring(BigEndian2int16u(Buffer+Buffer_Offset+10)));
                            Fill(Stream_General, 0, "Track/Position_Total", _T("")); //Clear
                            Fill(Stream_General, 0, "Track/Position_Total", Ztring::ToZtring(BigEndian2int16u(Buffer+Buffer_Offset+12)));
                            return;
                        default:
                            Value=_T("(Binary)");
                    }
                    break;
        case Kind_UTF8 : Value.From_UTF8((const char*)(Buffer+Buffer_Offset+8), Element_Size-8); break;
        case Kind_UTF16 : Value=_T("UTF-16 encoding not yet supported"); break;
        case Kind_String_Mac : Value=_T("String(Mac) encoding not yet supported"); break;
        case Kind_Jpeg : Value=_T("(Image)"); break;
        case Kind_ints :
                    switch (Element_Size-8)
                    {
                        case 1 : Value.From_Number(BigEndian2int8s(Buffer+Buffer_Offset+8)); break;
                        case 2 : Value.From_Number(BigEndian2int16s(Buffer+Buffer_Offset+8)); break;
                        case 4 : Value.From_Number(BigEndian2int32s(Buffer+Buffer_Offset+8)); break;
                        case 8 : Value.From_Number(BigEndian2int64s(Buffer+Buffer_Offset+8)); break;
                    default: Value=_T("Unknown kind of integer value!");
                    }
                    break;
        case Kind_float32 : Value=_T("Float32 encoding not yet supported"); break;
        case Kind_float64 : Value=_T("Float64 encoding not yet supported"); break;
        default: Value=_T("Unknown kind of value!");
    }

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(0, "Kind", Kind);
        Details_Add_Info(4, "Language", Language_Get(Language));
        Details_Add_Info(8, "Value", Value);
    }

    //Filling
    switch (moov_meta_hdlr_Type)
    {
        case Mpeg4::moov_meta_hdlr_mdir :
            {
                std::string Parameter;
                if (Element_Name[Element_Level-1]==Mpeg4::moov_meta______)
                    Metadata_Get(Parameter, moov_meta_ilst_xxxx_name_Name);
                else
                    Metadata_Get(Parameter, Element_Name[Element_Level-1]);
                if (!Parameter.empty())
                {
                    Fill(Stream_General, 0, Parameter.c_str(), _T("")); //Clear
                    Fill(Stream_General, 0, Parameter.c_str(), Value);
                }
            }
            break;
        case Mpeg4::moov_meta_hdlr_mdta :
            COHERANCY(moov_udta_meta_keys_ilst_Pos>=moov_udta_meta_keys_List.size(), "Keys atom is not enough large!")
            {
                std::string Parameter;
                Metadata_Get(Parameter, moov_udta_meta_keys_List[moov_udta_meta_keys_ilst_Pos]);
                if (!Parameter.empty())
                {
                    Fill(Stream_General, 0, Parameter.c_str(), _T("")); //Clear
                    Fill(Stream_General, 0, Parameter.c_str(), Value);
                }
                moov_udta_meta_keys_ilst_Pos++;
            }

        default: ;
    }
}
//---------------------------------------------------------------------------
void File_Mpeg4::moov_meta_ilst_xxxx_mean()
{
    NAME("Mean")
    INTEGRITY(4)
    COHERANCY(moov_meta_hdlr_Type!=Mpeg4::moov_meta_hdlr_mdir, "Bad meta type")

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(4, "Mean", Buffer+Buffer_Offset+4, Element_Size-4, false);
    }
}
//---------------------------------------------------------------------------
void File_Mpeg4::moov_meta_ilst_xxxx_name()
{
    NAME("Name")
    INTEGRITY(4)
    COHERANCY(moov_meta_hdlr_Type!=Mpeg4::moov_meta_hdlr_mdir, "Bad meta type")

    //Reading
    moov_meta_ilst_xxxx_name_Name.assign ((char*)Buffer+Buffer_Offset+4, Element_Size-4);

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(4, "Name", moov_meta_ilst_xxxx_name_Name.c_str());
    }
}
//---------------------------------------------------------------------------
// Atom "xml ", XML (ISO/IEC 14496-12), 0+ bytes
// Note: "hdlr" metadata type should be "mp7t"
// Version                          1 bytes, Pos=0
// Flags                            3 bytes, Pos=1
// XML text                         X bytes, Pos=4
//
void File_Mpeg4::moov_meta_xml()
{
    NAME_VERSION_FLAG("XML")
    INTEGRITY1(4)
    COHERANCY(moov_meta_hdlr_Type!=Mpeg4::moov_meta_hdlr_mp7t, "Bad meta type")

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Element("XML");
        Details_Add_Info(0, "Version", BigEndian2int8u(Buffer+Buffer_Offset+0));
        Details_Add_Info(1, "Flags", BigEndian2int32u(Buffer+Buffer_Offset+0)&0x00FFFFFF);
        Details_Add_Info(4, "Value", "(XML dump)");
    }
}

//---------------------------------------------------------------------------
// Atom "mvhd", 108 bytes
// Version                          1 bytes, Pos=0
// Flags                            3 bytes, Pos=1
// Creation time                    4 bytes, Pos=4    (8 bytes if Version==1)
// Modification time                4 bytes, Pos=8/12 (8 bytes if Version==1)
// Time scale                       4 bytes, Pos=12/20
// Duration                         4 bytes, Pos=16/24 (8 bytes if Version==1)
// Preferred rate                   4 bytes, Pos=20/32
// Preferred volume                 2 bytes, Pos=24/36
// Reserved                         10 bytes, Pos=26/38
// Matrix structure                 36 bytes, Pos=36/48
// Preview time                     4 bytes, Pos=72/84
// Preview duration                 4 bytes, Pos=76/88
// Poster time                      4 bytes, Pos=80/92
// Selection time                   4 bytes, Pos=84/96
// Selection duration               4 bytes, Pos=88/100
// Current time                     4 bytes, Pos=92/104
// Next track ID                    4 bytes, Pos=96/108
//
void File_Mpeg4::moov_mvhd()
{
    NAME_VERSION_FLAG("Movie header")
    INTEGRITY2(100, 112)

    //Reading
    int32u Version_Offset=0;
    if (Version==1)
        Version_Offset=4;

    //Reading
    Ztring Date_Created;
    if (Version==0)
        Date_Created.Date_From_Seconds_1904(BigEndian2int32u(Buffer+Buffer_Offset+4)); //Creation time
    else
        Date_Created.Date_From_Seconds_1904(BigEndian2int64u(Buffer+Buffer_Offset+4)); //Creation time
    Ztring Date_Modified;
    if (Version==0)
        Date_Modified.Date_From_Seconds_1904(BigEndian2int32u(Buffer+Buffer_Offset+8+1*Version_Offset)); //Modification time
    else
        Date_Modified.Date_From_Seconds_1904(BigEndian2int64u(Buffer+Buffer_Offset+8+1*Version_Offset)); //Modification time
    int32u TimeScale=BigEndian2int32u(Buffer+Buffer_Offset+12+2*Version_Offset); //Time scale
    int64u Duration;
    if (Version==0)
        Duration=BigEndian2int32u(Buffer+Buffer_Offset+16+2*Version_Offset); //Duration
    else
        Duration=BigEndian2int64u(Buffer+Buffer_Offset+16+2*Version_Offset); //Duration
    size_t PlayTime=(size_t)(((float)Duration)/TimeScale*1000);

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(4, "Creation time", Date_Created);
        Details_Add_Info(8+1*Version_Offset, "Modification time", Date_Modified);
        Details_Add_Info(12+2*Version_Offset, "Time scale", TimeScale);
        Details_Add_Info(16+2*Version_Offset, "Duration", Duration);
        Details_Add_Info(20+3*Version_Offset, "Preferred rate", (float)BigEndian2int32u(Buffer+Buffer_Offset+20+3*Version_Offset)/65536);
        Details_Add_Info(24+3*Version_Offset, "Preferred volume", (float)BigEndian2int16u(Buffer+Buffer_Offset+24+3*Version_Offset)/256);
        Details_Add_Info(26+3*Version_Offset, "Reserved", "(10 bytes)");
        Details_Add_Info(36+3*Version_Offset, "Matrix structure", "(36 bytes)");
        Details_Add_Info(72+3*Version_Offset, "Preview time", BigEndian2int32u(Buffer+Buffer_Offset+72+3*Version_Offset));
        Details_Add_Info(76+3*Version_Offset, "Preview duration", BigEndian2int32u(Buffer+Buffer_Offset+76+3*Version_Offset));
        Details_Add_Info(80+3*Version_Offset, "Poster time", BigEndian2int32u(Buffer+Buffer_Offset+80+3*Version_Offset));
        Details_Add_Info(84+3*Version_Offset, "Selection time", BigEndian2int32u(Buffer+Buffer_Offset+84+3*Version_Offset));
        Details_Add_Info(88+3*Version_Offset, "Selection duration", BigEndian2int32u(Buffer+Buffer_Offset+88+3*Version_Offset));
        Details_Add_Info(92+3*Version_Offset, "Current time", BigEndian2int32u(Buffer+Buffer_Offset+92+3*Version_Offset));
        Details_Add_Info(96+3*Version_Offset, "Next track ID", BigEndian2int32u(Buffer+Buffer_Offset+96+3*Version_Offset));
    }

    //Filling
    Fill("PlayTime", PlayTime);
    Fill("Encoded_Date", Date_Created);
    Fill("Tagged_Date", Date_Modified);
}

//---------------------------------------------------------------------------
// Atom "trak", Track
// List...
//
void File_Mpeg4::moov_trak()
{
    NAME("Track")

    //Filling
    Fill_Flush();
    moov_trak_tkhd_TrackID=(int32u)-1;
    moov_trak_mdia_minf_stbl_stco_Parse=false;
    Stream_Prepare(Stream_Max); //clear filling
}

//---------------------------------------------------------------------------
// Atom "edts", Edit
// List...
//
void File_Mpeg4::moov_trak_edts()
{
    NAME("Edit")
}

//---------------------------------------------------------------------------
// Atom "elst", Edit List
// Version                          1 bytes, Pos=0
// Flags                            3 bytes, Pos=1
// Count                            4 bytes, Pos=4
//
void File_Mpeg4::moov_trak_edts_elst()
{
    NAME_VERSION_FLAG("Edit List")

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(4, "Count", BigEndian2int32u(Buffer+Buffer_Offset+4));
    }

    Buffer_Offset+=8;
    size_t Pos=0;
    while (File_Offset+Buffer_Offset+12<=Element_Next[Element_Level])
    {
        //Details
        if (Config.Details_Get())
        {
            Details_Add_Element(Element_Level+1, _T("Begin"));
        }
        moov_trak_edts_elst_list();
        //Details
        if (Config.Details_Get())
        {
            Details_Add_Element(Element_Level+1, _T("End"));
            Details_Add_Element_Flush();
        }
        Buffer_Offset+=12;
        Pos++;
    }
}

//---------------------------------------------------------------------------
// List
// TrackDuration                    4 bytes, Pos=0
// ???                              4 bytes, Pos=4
// MediaRate                        2 bytes, Pos=8
// ???                              2 bytes, Pos=10
//
void File_Mpeg4::moov_trak_edts_elst_list()
{
    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(0, "TrackDuration", BigEndian2int32u(Buffer+Buffer_Offset+0));
        Details_Add_Info(4, "???", BigEndian2int32u(Buffer+Buffer_Offset+4));
        Details_Add_Info(8, "MediaRate", BigEndian2int32u(Buffer+Buffer_Offset+8));
        Details_Add_Info(10, "???", BigEndian2int32u(Buffer+Buffer_Offset+10));
    }
}

//---------------------------------------------------------------------------
// Atom "mdia", Media
// List...
//
void File_Mpeg4::moov_trak_mdia()
{
    NAME("Media")

    //Reading
    moov_trak_mdia_mdhd_PlayTime=0;
}

//---------------------------------------------------------------------------
// Atom "hdlr", 24+ bytes
//
void File_Mpeg4::moov_trak_mdia_hdlr()
{
    NAME_VERSION_FLAG("Header")
    INTEGRITY1(24)

    int32u SubType; std::string Manufacturer, Title;
    BEGIN_VERSION_FLAG
    SKIP_CH(4,                                                  Component type)
    GET_C4 (SubType,                                            Component subtype)
    SKIP_CH(4,                                                  Component manufacturer)
    SKIP_I4(                                                    Component flags)
    SKIP_I4(                                                    Component flags mask)
    int8u Size;
    PEEK_I1(Size)
    if ((size_t)(24+1+Size)==Element_Size)
    {
        SKIP_I1(                                                Component name size)
        GET_CH (Size, Title,                                    Component name)
    }
    else
    {
        GET_CH (Element_Size-24, Title,                         Component name)
    }
    if (Title.find("Handler")!=std::string::npos || Title.find("vide")!=std::string::npos || Title.find("soun")!=std::string::npos)
        Title.clear(); //This is not a Title
    END

    //Filling
    if (!Title.empty())                Fill("Title",    Title);
    switch (SubType)
    {
        case Mpeg4::moov_trak_mdia_hdlr_soun :
            if (StreamKind_Last!=Stream_Audio)
            {
                Stream_Prepare(Stream_Audio);
            }
            break;
        case Mpeg4::moov_trak_mdia_hdlr_vide :
            if (StreamKind_Last!=Stream_Video)
            {
                Stream_Prepare(Stream_Video);
            }
            break;
        case Mpeg4::moov_trak_mdia_hdlr_text :
            if (StreamKind_Last!=Stream_Text)
            {
                Stream_Prepare(Stream_Text);
                Fill("Codec", "tx3g");
            }
            break;
        case Mpeg4::moov_trak_mdia_hdlr_subp :
            if (StreamKind_Last!=Stream_Text)
            {
                Stream_Prepare(Stream_Text);
                Fill("Codec", "subp");
            }
            break;
        case Mpeg4::moov_trak_mdia_hdlr_MPEG :
            moov_trak_mdia_minf_stbl_stco_Parse=true; //Data is in MDAT
            break;
        default: ;
    }
}

//---------------------------------------------------------------------------
// Atom "mdhd", 32 bytes
// Version                          1 bytes, Pos=0
// Flags                            3 bytes, Pos=1
// Creation time                    4 bytes, Pos=4     (8 bytes if Version==1)
// Modification time                4 bytes, Pos=8/12  (8 bytes if Version==1)
// Time scale                       4 bytes, Pos=12/20
// Duration                         4 bytes, Pos=16/24 (8 bytes if Version==1)
// Language                         2 bytes, Pos=20/32
// Quality                          2 bytes, Pos=22/34
//
void File_Mpeg4::moov_trak_mdia_mdhd()
{
    NAME_VERSION_FLAG("Header")
    INTEGRITY2(24, 36)

    //Reading
    int32u Version_Offset=0;
    if (Version==1)
        Version_Offset=4;
    Ztring Date_Created;
    if (Version==0)
        Date_Created.Date_From_Seconds_1904(BigEndian2int32u(Buffer+Buffer_Offset+4)); //Creation time
    else
        Date_Created.Date_From_Seconds_1904(BigEndian2int64u(Buffer+Buffer_Offset+4)); //Creation time
    Ztring Date_Modified;
    if (Version==0)
        Date_Modified.Date_From_Seconds_1904(BigEndian2int32u(Buffer+Buffer_Offset+8+1*Version_Offset)); //Modification time
    else
        Date_Modified.Date_From_Seconds_1904(BigEndian2int64u(Buffer+Buffer_Offset+8+1*Version_Offset)); //Modification time
    TimeScale=BigEndian2int32u(Buffer+Buffer_Offset+12+2*Version_Offset); //Time scale
    int64u Duration;
    if (Version==0)
        Duration=BigEndian2int32u(Buffer+Buffer_Offset+16+2*Version_Offset); //Duration
    else
        Duration=BigEndian2int64u(Buffer+Buffer_Offset+16+2*Version_Offset); //Duration
    //size_t PlayTime=(size_t)(((float)Duration)/TimeScale*1000);
    Fill("Language", Language_Get(BigEndian2int16u(Buffer+Buffer_Offset+20+3*Version_Offset)));

    if (TimeScale)
    {
        moov_trak_mdia_mdhd_PlayTime=(size_t)(((float)Duration)/TimeScale*1000);
        Fill("PlayTime", moov_trak_mdia_mdhd_PlayTime);
    }

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(4, "Creation time", Date_Created);
        Details_Add_Info(8+1*Version_Offset, "Modification time", Date_Modified);
        Details_Add_Info(12+2*Version_Offset, "Time scale", TimeScale);
        Details_Add_Info(16+2*Version_Offset, "Duration", Duration);
        Details_Add_Info(22+3*Version_Offset, "Quality", BigEndian2int32u(Buffer+Buffer_Offset+22+2*Version_Offset));
    }
}

//---------------------------------------------------------------------------
// Atom "minf", Media Information
// List...
//
void File_Mpeg4::moov_trak_mdia_minf()
{
    NAME("Media Information")
}

//---------------------------------------------------------------------------
// Atom "hint", Media Information
// Version                          1 bytes, Pos=0
// Flags                            3 bytes, Pos=1
// Maximum packet delivery unit     2 bytes, Pos=4
// Average packet delivery unit     2 bytes, Pos=6
// Maximum bit rate                 4 bytes, Pos=8
// Average bit rate                 4 bytes, Pos=12
// Reserved                         4 bytes, Pos=16
//
void File_Mpeg4::moov_trak_mdia_minf_hint()
{
    NAME_VERSION_FLAG("Hint")
    INTEGRITY1(20)

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(4, "Maximum packet delivery unit", BigEndian2int16u(Buffer+Buffer_Offset+4));
        Details_Add_Info(6, "Average packet delivery unit", BigEndian2int16u(Buffer+Buffer_Offset+6));
        Details_Add_Info(8, "Maximum bit rate", BigEndian2int32u(Buffer+Buffer_Offset+8));
        Details_Add_Info(12, "Average bit rate", BigEndian2int32u(Buffer+Buffer_Offset+12));
        Details_Add_Info(16, "Reserved", "4 bytes");
    }
}

//---------------------------------------------------------------------------
// Atom "vmhd", Audio Media Header, 8 bytes
// Version                          1 bytes, Pos=0
// Flags                            3 bytes, Pos=1
// Audio balance                    2 bytes, Pos=4
// Reserved                         2 bytes, Pos=6
//
void File_Mpeg4::moov_trak_mdia_minf_smhd()
{
    NAME("Sound Media Header")
    INTEGRITY(8)

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(0, "This is a Audio stream", "");
    }

    //Filling
    if (StreamKind_Last!=Stream_Audio)
    {
        Stream_Prepare(Stream_Audio);
    }
}

//---------------------------------------------------------------------------
// Atom "vmhd", Video Media Header, 12 bytes
// Version                          1 bytes, Pos=0
// Flags                            3 bytes, Pos=1
// Graphic mode                     2 bytes, Pos=4
// Graphic mode color R             2 bytes, Pos=6
// Graphic mode color G             2 bytes, Pos=8
// Graphic mode color B             2 bytes, Pos=10
//
void File_Mpeg4::moov_trak_mdia_minf_vmhd()
{
    NAME_VERSION_FLAG("Video Media Header")
    INTEGRITY2(12, 12)

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(0, "This is a Video stream", "");
    }

    //Filling
    if (StreamKind_Last!=Stream_Video)
    {
        Stream_Prepare(Stream_Video);
    }
}

//---------------------------------------------------------------------------
// Atom "",
// List...
//
void File_Mpeg4::moov_trak_mdia_minf_stbl()
{
    Buffer_MaximumSize=16*1024*1024; //If we are here, this is really a MPEG-4 file, and some atoms are very bigs...

    NAME("Sample Table")
}

//---------------------------------------------------------------------------
// Atom "stco"
//
void File_Mpeg4::moov_trak_mdia_minf_stbl_stco()
{
    NAME_VERSION_FLAG("Chunk offset")

    int32u Count;
    BEGIN_VERSION_FLAG
    GET_I4 (Count,                                              Number of entries)
    if (moov_trak_mdia_minf_stbl_stco_Parse)
    {
        stco_ID.push_back(moov_trak_tkhd_TrackID);
        stco[moov_trak_tkhd_TrackID]=new std::vector<int64u>;
        int32u Offset;
        for (int32u Pos=0; Pos<Count; Pos++)
        {
            DETAILLEVEL_SET(1);
            GET_I4 (Offset,                                     Offset)
            stco[moov_trak_tkhd_TrackID]->push_back(Offset);
        }
    }
    else
        Stream_Pos+=Count;
    END
}

//---------------------------------------------------------------------------
// Atom "stsd"
// ??                               4 bytes, Pos=0
// ??                               4 bytes, Pos=4
// Size                             4 bytes, Pos=8
// Data format                      4 bytes, Pos=12
// Reserved                         6 bytes, Pos=16
// Data reference index             2 bytes, Pos=20
// Hint track version               2 bytes, Pos=24
// Last compatible hint version     2 bytes, Pos=26
// Max packet size                  4 bytes, Pos=28
// Additional data table variable   X bytes, Pos=32
//
void File_Mpeg4::moov_trak_mdia_minf_stbl_stsd()
{
    NAME("Sample Description")
    LIST_SET(8)
}

//---------------------------------------------------------------------------
void File_Mpeg4::moov_trak_mdia_minf_stbl_stsd_xxxx()
{
    switch (StreamKind_Last)
    {
        case Stream_Audio : moov_trak_mdia_minf_stbl_stsd_xxxxSound(); break;
        case Stream_Video : moov_trak_mdia_minf_stbl_stsd_xxxxVideo(); break;
        default : ;
    }
}

//---------------------------------------------------------------------------
// Atom "", 26 bytes
// Reserved                         6 bytes, Pos=0
// Count                            2 bytes, Pos=6
// SoundVersion                     2 bytes, Pos=8
// Reserved                         6 bytes, Pos=10
// channels                         2 bytes, Pos=16
// sampleSize                       2 bytes, Pos=18
// packetSize                       2 bytes, Pos=20
// timeScale                        4 bytes, Pos=22
// Reserved                         2 bytes, Pos=26 (in specific cases only!)
// samplesPerPacket                 4 bytes, Pos=26+Reserved (if SoundVersion>1)
// bytesPerPacket                   4 bytes, Pos=30+Reserved (if SoundVersion>1)
// bytesPerFrame                    4 bytes, Pos=34+Reserved (if SoundVersion>1)
// bytesPerSample                   4 bytes, Pos=38+Reserved (if SoundVersion>1)
// Reserved                         20 bytes, Pos=42+Reserved (if SoundVersion>2)
// List...                                          (in specific cases only!)
//
void File_Mpeg4::moov_trak_mdia_minf_stbl_stsd_xxxxSound()
{
    NAME("Audio")
    INTEGRITY(26)

    int32u Version, Channels, SampleSize, ID, TimeScale;
    BEGIN
    SKIP_I4(                                                    Reserved)
    SKIP_I2(                                                    Reserved)
    SKIP_I2(                                                    Data reference index)
    GET_I2 (Version,                                            Version)
    SKIP_I2(                                                    Revision level)
    SKIP_I4(                                                    Vendor)
    GET_I2 (Channels,                                           Number of channels)
    GET_I2 (SampleSize,                                         Sample size)
    GET_I2 (ID,                                                 Compression ID)
    SKIP_I2(                                                    Packet size)
    if (ID==0)
    {
        GET_I4 (TimeScale,                                      Sample rate) INFO(Ztring::ToZtring(TimeScale/0x10000)+_T(" Hz"));
    }
    else
    {
        GET_I4 (TimeScale,                                      Frames per chunk) INFO(Ztring::ToZtring(TimeScale/0x10000)+_T(" frames"));
    }
    if (Version>=1)
    {
        SKIP_I4(                                                Samples per packet)
        SKIP_I4(                                                Bytes per packet)
        SKIP_I4(                                                Bytes per frame)
        SKIP_I4(                                                Bytes per sample)
        if (Version>=2)
        {
            SKIP_I4(                                            Unknown)
            SKIP_I4(                                            Unknown)
            SKIP_I4(                                            Unknown)
            SKIP_I4(                                            Unknown)
            SKIP_I4(                                            Unknown)
        }
    }

    //Filling
    std::string Codec;
    Codec.append(1, (char)((Element_Name[Element_Level]&0xFF000000)>>24));
    Codec.append(1, (char)((Element_Name[Element_Level]&0x00FF0000)>>16));
    if (Codec!="ms") //Normal
    {
        Codec.append(1, (char)((Element_Name[Element_Level]&0x0000FF00)>> 8));
        Codec.append(1, (char)((Element_Name[Element_Level]&0x000000FF)>> 0));
        Fill("Codec", ""); //Sometimes we have 2 atoms (1st with "jpeg")
        Fill("Codec", Codec.c_str());
        if (Codec=="drms")
            Fill("Encryption", "iTunes");
        if (Codec=="enca")
            Fill("Encryption", "Encrypted");
    }
    else //Microsoft 2CC
    {
        Fill("Codec", ((Element_Name[Element_Level]&0x0000FF00)>> 8)
                    + ((Element_Name[Element_Level]&0x000000FF)>> 0), 16); //FormatTag
    }
    Fill("Channel(s)", Channels);
    Fill("Resolution", SampleSize);
    if (ID==0)
        Fill("SamplingRate", TimeScale/0x10000);

    //Sometimes, more Atoms in this atoms
    if (Stream_Pos<Element_Size)
    {
        Buffer_Offset+=Stream_Pos;
        Element_Size-=Stream_Pos;
        List=true;
    }
}

//---------------------------------------------------------------------------
// Atom "", 26 bytes
// Reserved                         6 bytes, Pos=0
// List...                                          (in specific cases only!)
//
void File_Mpeg4::moov_trak_mdia_minf_stbl_stsd_xxxxVideo()
{
    NAME("Video")
    INTEGRITY(78)

    int32u Width, Height, CompressorName_Size, ColorTableID;
    BEGIN
    SKIP_I4(                                                    Reserved)
    SKIP_I2(                                                    Reserved)
    SKIP_I2(                                                    Data reference index)
    SKIP_I2(                                                    Version)
    SKIP_I2(                                                    Revision level)
    SKIP_I4(                                                    Vendor)
    SKIP_I4(                                                    Temporal quality)
    SKIP_I4(                                                    Spatial quality)
    GET_I2 (Width,                                              Width)
    GET_I2 (Height,                                             Height)
    SKIP_I4(                                                    Horizontal resolution)
    SKIP_I4(                                                    Vertical resolution)
    SKIP_I4(                                                    Data size)
    SKIP_I2(                                                    Frame count)
    GET_I1 (CompressorName_Size,                                Compressor name size)
    SKIP_CH(CompressorName_Size,                                Compressor name)
    Stream_Pos+=32-1-CompressorName_Size;
    SKIP_I2(                                                    Depth)
    GET_I2 (ColorTableID,                                       Color table ID)
    if (ColorTableID==0)
        Stream_Pos+=32; //Color Table

    //Filling
    std::string Codec;
    Codec.append(1, (char)((Element_Name[Element_Level]&0xFF000000)>>24));
    Codec.append(1, (char)((Element_Name[Element_Level]&0x00FF0000)>>16));
    Codec.append(1, (char)((Element_Name[Element_Level]&0x0000FF00)>> 8));
    Codec.append(1, (char)((Element_Name[Element_Level]&0x000000FF)>> 0));
    if (Codec!="mp4v") //mp4v can contain a lot of different codecs :(
    {
        Fill("Codec", ""); //Sometimes we have 2 atoms (1st with "jpeg")
        Fill("Codec", Codec.c_str());
        if (Codec=="drms")
            Fill("Encryption", "iTunes");
        if (Codec=="enca")
            Fill("Encryption", "Encrypted");
    }
    Fill("Width", "");
    Fill("Width", BigEndian2int16u(Buffer+Buffer_Offset+24));
    Fill("Height", "");
    Fill("Height", BigEndian2int16u(Buffer+Buffer_Offset+26));

    //Specific
    if (Codec=="dvc " || Codec=="DVC " || Codec=="dvcp" || Codec=="DVCP" || Codec=="dvpn" || Codec=="DVPN" || Codec=="dvpp" || Codec=="DVPP")
    {
        Fill("AspectRatio", ((float)4)/3);
    }

    LIST_SET(Stream_Pos)
}

//---------------------------------------------------------------------------
// Atom "alac"
// Reserved                         6 bytes, Pos=0
// Count                            2 bytes, Pos=6
//
void File_Mpeg4::moov_trak_mdia_minf_stbl_stsd_xxxx_alac()
{
    NAME("ALAC")
    INTEGRITY(8)
}

//---------------------------------------------------------------------------
// Atom "chan"
//
void File_Mpeg4::moov_trak_mdia_minf_stbl_stsd_xxxx_chan()
{
    NAME("Channels")
}

//---------------------------------------------------------------------------
// Atom "esds"
// Version                          3 bytes, Pos=0
// Flags                            1 bytes, Pos=1
// Descriptors...
//
void File_Mpeg4::moov_trak_mdia_minf_stbl_stsd_xxxx_esds()
{
    NAME_VERSION_FLAG("ES Descriptor")
    INTEGRITY1(4)

    //Reading
    Descriptors(4, Element_Size-4);
}

//---------------------------------------------------------------------------
// Atom "wave"
//
void File_Mpeg4::moov_trak_mdia_minf_stbl_stsd_xxxx_wave()
{
    NAME("WAVE")
}

//---------------------------------------------------------------------------
// Atom "frma"
//
void File_Mpeg4::moov_trak_mdia_minf_stbl_stsd_xxxx_wave_frma()
{
    NAME("Data format")
    INTEGRITY(4)

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(0, "Format", Buffer+Buffer_Offset, 4);
    }

    //Filling
    Ztring Codec; Codec.From_Local((char*)(Buffer+Buffer_Offset), 0, 4);
    if (Codec.find(_T("ms"))==0) //Microsoft 2CC
    {
        Fill("Codec", "");
        Fill("Codec", Buffer[Buffer_Offset+2]*0x100
                    + Buffer[Buffer_Offset+3]*0x001, 16); //FormatTag
    }
    else if (CC4(Buffer+Buffer_Offset)!=CC4("mp4a")) //mp4a can contain a lot of different codecs :(
    {
        Fill("Codec", "");
        Fill("Codec", Codec);
    }
}

/*
//---------------------------------------------------------------------------
// Atom "mp4s"
// Reserved                         6 bytes, Pos=0
// Count                            2 bytes, Pos=6
// List...
//
void File_Mpeg4::moov_trak_mdia_minf_stbl_stsd_mp4s()
{
    //Coherancy test
    if (Element_Size<8) //Size of atom
        return;

    //Filling
    Fill("Codec", "mp4s");

    //List
    Buffer_Offset+=8;
    Element_Size-=8;
    List=true;
}
*/

//---------------------------------------------------------------------------
// Atom "stsz"
//
void File_Mpeg4::moov_trak_mdia_minf_stbl_stsz()
{
    NAME_VERSION_FLAG("Sample Size")
    INTEGRITY1(12)

    int32u Size, Count;
    BEGIN_VERSION_FLAG
    GET_I4 (Size,                                               Sample Size)
    GET_I4 (Count,                                              Number of entries)

    int64u Stream_Size=0;

    if (Size>0)
    {
        Stream_Size=Size*Count;
        if (moov_trak_mdia_minf_stbl_stco_Parse)
        {
            stsz[moov_trak_tkhd_TrackID]=new std::vector<int32u>;
            stsz[moov_trak_tkhd_TrackID]->push_back(Stream_Size);
        }
    }
    else
    {
        if (moov_trak_mdia_minf_stbl_stco_Parse)
            stsz[moov_trak_tkhd_TrackID]=new std::vector<int32u>;
        int32u Offset;
        for (int32u Pos=12; Pos+4<=Element_Size; Pos+=4)
        {
            DETAILLEVEL_SET(1.1);
            GET_I4 (Offset,                                     Offset)
            Stream_Size+=Offset;
            if (moov_trak_mdia_minf_stbl_stco_Parse)
                stsz[moov_trak_tkhd_TrackID]->push_back(Offset);
        }
    }

    if (Stream_Size>0)
        Fill("StreamSize", Stream_Size);
}

//---------------------------------------------------------------------------
// Atom "stts"
//
void File_Mpeg4::moov_trak_mdia_minf_stbl_stts()
{
    NAME_VERSION_FLAG("Time to Sample")
    INTEGRITY1(8)

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(4, "Number of entries", BigEndian2int32u(Buffer+Buffer_Offset+4));
    }

    int32u Min=Error;
    int32u Max=0;
    int64u FrameCount=0;

    for (int32u Pos=8; Pos+8<=Element_Size; Pos+=8)
    {
        FrameCount+=BigEndian2int32u(Buffer+Buffer_Offset+Pos);
        int32u Value=BigEndian2int32u(Buffer+Buffer_Offset+Pos+4);
        if (Value<Min) Min=Value;
        if (Value>Max) Max=Value;
    }

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(8, "Min", Min);
        Details_Add_Info(8, "Max", Max);
    }

    //Filling
    if (StreamKind_Last==Stream_Video)
    {
        if (TimeScale && Min && Max)
        {
            if (Min!=Max)
            {
                Fill("FrameRate_Min", ((float)TimeScale)/Max);
                Fill("FrameRate_Max", ((float)TimeScale)/Min);
                Fill("FrameRate", ((float)FrameCount)/moov_trak_mdia_mdhd_PlayTime*1000);
            }
            else
                Fill("FrameRate", ((float)TimeScale)/Max);
        }
        Fill("FrameCount", FrameCount);
    }

    //Details
    for (int32u Pos=8; Pos+8<=Element_Size; Pos+=8)
    {
        //Details
        if (Config.Details_Get())
        {
            Details_Add_Info(Pos, "Sample Count", BigEndian2int32u(Buffer+Buffer_Offset+Pos));
            Details_Add_Info(Pos+4, "Sample Duration", BigEndian2int32u(Buffer+Buffer_Offset+Pos+4));
            if (Pos>=40)
            {
                Details_Add_Info(Pos+8, "(...)", "(...)");
                Pos=Element_Size;
            }
        }
    }

}

//---------------------------------------------------------------------------
// Atom "tapt", ????
// List...
//
void File_Mpeg4::moov_trak_tapt()
{
    NAME("????")
}

//---------------------------------------------------------------------------
// Atom "clef", ????
//
void File_Mpeg4::moov_trak_tapt_clef()
{
    NAME("????")
}

//---------------------------------------------------------------------------
// Atom "prof", ????
//
void File_Mpeg4::moov_trak_tapt_prof()
{
    NAME("????")
}

//---------------------------------------------------------------------------
// Atom "enof", ????
//
void File_Mpeg4::moov_trak_tapt_enof()
{
    NAME("????")
}

//---------------------------------------------------------------------------
// Atom "tkhd", Track header, 84/96 bytes
// Version                          1 bytes, Pos=0
// Flags                            3 bytes, Pos=1
// Creation time                    4 bytes, Pos=4     (8 bytes if Version==1)
// Modification time                4 bytes, Pos=8/12  (8 bytes if Version==1)
// Track ID                         4 bytes, Pos=12/20
// Reserved                         4 bytes, Pos=16/24
// Duration                         4 bytes, Pos=20/28 (8 bytes if Version==1)
// Reserved                         8 bytes, Pos=24/36
// Layer                            2 bytes, Pos=32/44
// Alternate group                  2 bytes, Pos=34/46
// Volume                           2 bytes, Pos=36/48
// Reserved                         2 bytes, Pos=38/50
// Matrix structure                 36 bytes, Pos=40/52
// Track width                      4 bytes, Pos=76/88
// Track height                     4 bytes, Pos=80/92
//
void File_Mpeg4::moov_trak_tkhd()
{
    NAME_VERSION_FLAG("Header")
    INTEGRITY2(84, 96)

    //Reading
    int32u Version_Offset=0;
    if (Version==1)
        Version_Offset=4;
    if (Version==0)
        Fill("Encoded_Date", Ztring().Date_From_Seconds_1904(BigEndian2int32u(Buffer+Buffer_Offset+4))); //Creation time
    else
        Fill("Encoded_Date", Ztring().Date_From_Seconds_1904(BigEndian2int64u(Buffer+Buffer_Offset+4))); //Creation time
    if (Version==0)
        Fill("Tagged_Date", Ztring().Date_From_Seconds_1904(BigEndian2int32u(Buffer+Buffer_Offset+8+1*Version_Offset))); //Modification time
    else
        Fill("Tagged_Date", Ztring().Date_From_Seconds_1904(BigEndian2int64u(Buffer+Buffer_Offset+8+1*Version_Offset))); //Modification time
    int64u Duration;
    if (Version==0)
        Duration=BigEndian2int32u(Buffer+Buffer_Offset+20+2*Version_Offset); //Duration
    else
        Duration=BigEndian2int64u(Buffer+Buffer_Offset+20+2*Version_Offset); //Duration
    moov_trak_tkhd_TrackID=BigEndian2int32u(Buffer+Buffer_Offset+12+2*Version_Offset);

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(1, "Track in Poster", (bool)((Flags&0x00000008)>3));
        Details_Add_Info(1, "Track in Preview", (bool)((Flags&0x00000004)>2));
        Details_Add_Info(1, "Track in Movie", (bool)((Flags&0x00000002)>1));
        Details_Add_Info(1, "Track Enabled", (bool)((Flags&0x000000001)>0));
//        Details_Add_Info(4, "Creation time", moov_trak_tkhd_Date_Created);
//        Details_Add_Info(8+1*Version_Offset, "Modification time", moov_trak_tkhd_Date_Modified);
        Details_Add_Info(12+2*Version_Offset, "Track ID", BigEndian2int32u(Buffer+Buffer_Offset+12+2*Version_Offset));
        Details_Add_Info(16+2*Version_Offset, "Reserved", "(4 bytes)");
        Details_Add_Info(20+2*Version_Offset, "Duration", Duration);
        Details_Add_Info(24+3*Version_Offset, "Reserved", "(8 bytes)");
        Details_Add_Info(32+3*Version_Offset, "Layer", BigEndian2int16u(Buffer+Buffer_Offset+32+3*Version_Offset));
        Details_Add_Info(34+3*Version_Offset, "Alternate group", BigEndian2int16u(Buffer+Buffer_Offset+34+3*Version_Offset));
        Details_Add_Info(36+3*Version_Offset, "Volume", (float)BigEndian2int16u(Buffer+Buffer_Offset+36+3*Version_Offset)/256);
        Details_Add_Info(38+3*Version_Offset, "Reserved", "(2 bytes)");
        Details_Add_Info(38+3*Version_Offset, "Matrix structure", "(36 bytes)");
        //Details_Add_Info(76+3*Version_Offset, "Width", moov_trak_tkhd_Width/0x10000);
        //Details_Add_Info(80+3*Version_Offset, "Height", moov_trak_tkhd_Height/0x10000);
    }

    //Filling
    Fill("ID", "");
    Fill("ID", BigEndian2int32u(Buffer+Buffer_Offset+12+2*Version_Offset));
}

//---------------------------------------------------------------------------
// Atom "tref", Edit
// List...
//
void File_Mpeg4::moov_trak_tref()
{
    NAME("Track Reference")
}

//---------------------------------------------------------------------------
// Atom "tmcd", ????
// List...
//
void File_Mpeg4::moov_trak_tref_tmcd()
{
    NAME("TimeCode")

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(0, "Value", BigEndian2int32u(Buffer+Buffer_Offset+0));
    }
}

//---------------------------------------------------------------------------
// Atom "",
// List...
//
void File_Mpeg4::moov_udta()
{
    NAME("User Data")
}

//---------------------------------------------------------------------------
void File_Mpeg4::moov_udta_chpl()
{
    NAME("Chapters")

    Stream_Prepare(Stream_Chapters);

    size_t Element_Offset=9;
    size_t Pos=0;

    while (Element_Offset<Element_Size)
    {
        int8u Size=BigEndian2int8u(Buffer+Buffer_Offset+Element_Offset+8);
        int64u Time=BigEndian2int64u(Buffer+Buffer_Offset+Element_Offset)/100;
        Ztring Value; Value.From_UTF8((char*)Buffer+Buffer_Offset+Element_Offset+9, Size);
        if (Value.empty())
            Value.From_Local((char*)Buffer+Buffer_Offset+Element_Offset+9, Size); //Trying Local...

        //Details
        if (Config.Details_Get())
        {
            Details_Add_Info(Element_Offset+0, "Time", Time);
            Details_Add_Info(Element_Offset+8, "Size", Size);
            Details_Add_Info(Element_Offset+9, "Value", Value);
        }

        //TODO: Some hack for quickly parse Time.
        ZtringListList List=_T("Time/String");
        List.push_back(_T("Time;")+Ztring::ToZtring(Time/100));
        PlayTime_PlayTime123(_T("Time"), List);
        Ztring TimeS=List(_T("Time/String3"));
        if (TimeS.empty())
            TimeS=_T("00:00:00.000");

        //Filling
        Ztring ToAdd;
        ToAdd+=TimeS; //ToAdd.From_Number(Time);
        ToAdd+=_T(" - ");
        ToAdd+=Value;
        Fill(Ztring::ToZtring(Pos+1).To_Local().c_str(), ToAdd);

        //Next
        Pos++;
        Element_Offset+=8+1+Size;
    }
}

//---------------------------------------------------------------------------
void File_Mpeg4::moov_udta_cprt()
{
    NAME_VERSION_FLAG("Copyright")
    INTEGRITY1(6)

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(4, "Language", Language_Get(BigEndian2int16u(Buffer+Buffer_Offset+4)));
        Details_Add_Info(6, "Value", Buffer+Buffer_Offset+6, Element_Size-6);
    }

    //Filling
    Fill(Stream_General, 0, "Copyright", (char*)Buffer+Buffer_Offset+6, Element_Size-6);
}

//---------------------------------------------------------------------------
// Atom "",
// List...
//
void File_Mpeg4::moov_udta_hnti()
{
    NAME("Hint Info")
}

//---------------------------------------------------------------------------
void File_Mpeg4::moov_udta_hnti_rtp()
{
    NAME("Real Time")

    //Details
    if (Config.Details_Get())
    {
        Details_Add_Info(0, "Value", Buffer+Buffer_Offset, Element_Size<64?Element_Size:64, false);
    }
}

//---------------------------------------------------------------------------
void File_Mpeg4::moov_udta_meta()
{
    NAME("Metadata")
    LIST_SET(4)
}

//---------------------------------------------------------------------------
void File_Mpeg4::moov_udta_meta_hdlr()
{
    moov_meta_hdlr();
}

//---------------------------------------------------------------------------
void File_Mpeg4::moov_udta_meta_ilst()
{
    moov_meta_ilst();
}

//---------------------------------------------------------------------------
void File_Mpeg4::moov_udta_meta_ilst_xxxx()
{
    moov_meta_ilst_xxxx();
}

//---------------------------------------------------------------------------
void File_Mpeg4::moov_udta_meta_ilst_xxxx_data()
{
    moov_meta_ilst_xxxx_data();
}
//---------------------------------------------------------------------------
void File_Mpeg4::moov_udta_meta_ilst_xxxx_mean()
{
    moov_meta_ilst_xxxx_mean();
}
//---------------------------------------------------------------------------
void File_Mpeg4::moov_udta_meta_ilst_xxxx_name()
{
    moov_meta_ilst_xxxx_name();
}

//---------------------------------------------------------------------------
void File_Mpeg4::moov_udta_xxxx()
{
    //Coherancy test
    if (Count_Get(Stream_General)==0)
        return;
    if (Buffer_Offset<6)
        return;

    //Reading
    std::string Parameter;
    method Method=Metadata_Get(Parameter, Element_Name[Element_Level]);

    switch (Method)
    {
        case Method_None :
            break;
        case Method_Binary :
            break;
        case Method_String :
            {
                //Details
                if (Config.Details_Get())
                {
                    Details_Add_Element("Text");
                }

                //Reading
                size_t String_Offset=0;
                int16u String_Size=0;
                int16u Language=0;
                Ztring Value;
                while (String_Offset+4<Element_Size)
                {
                    //Reading
                    String_Size=BigEndian2int16u(Buffer+Buffer_Offset+String_Offset);
                /*
                    Language=BigEndian2int16u(Buffer+Buffer_Offset+String_Offset+2);
                    if (String_Offset+4+String_Size>Element_Size)
                        return; //Corrupted
                    Value.From_Local((const char*)(Buffer+Buffer_Offset+String_Offset+4), String_Size);

                    //Details
                    if (Config.Details_Get())
                    {
                        Details_Add_Info(String_Offset+0, "Size", String_Size);
                        Details_Add_Info(String_Offset+2, "Language", Language_Get(Language));
                        Details_Add_Info(String_Offset+4, "Value", Value);
                    }

                    //Filling
                    //Fill(Stream_General, 0, Parameter.c_str(), Value);

                    //Finnishing
                */
                    String_Offset+=4;
                    String_Offset+=String_Size;
                }
            }
            break;
        case Method_Integer :
            //Details
            if (Config.Details_Get())
            {
                Details_Add_Element("Integer");
            }

            //Reading
            break;
    }
}

//---------------------------------------------------------------------------
// Atom "pckg", , bytes
//
void File_Mpeg4::pckg()
{
    NAME("QTCA")

    //Filling
    Stream_Prepare(Stream_General);
    Fill("Format", "QTCA");
}

//---------------------------------------------------------------------------
// Atom "pnot", , bytes
//
void File_Mpeg4::pnot()
{
    NAME("Preview")
}

//---------------------------------------------------------------------------
// Atom "skip", , bytes
//
void File_Mpeg4::skip()
{
    NAME("Skip")
}

//---------------------------------------------------------------------------
// Atom "wide", , 0 byte
//
void File_Mpeg4::wide()
{
    NAME("Wide")
}

//***************************************************************************
//
//***************************************************************************

} //NameSpace

#endif //MEDIAINFO_MPEG4_*

