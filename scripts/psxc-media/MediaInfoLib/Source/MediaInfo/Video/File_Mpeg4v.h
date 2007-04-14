// File_Avc - Info for MPEG-4 Visual Visual files
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
// Information about MPEG-4 Visual files
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef MediaInfo_Mpeg4vH
#define MediaInfo_Mpeg4vH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/File__Base.h"
#include "MediaInfo/Multiple/File_Mpeg4.h"
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Class File_Mpeg4v
//***************************************************************************

class File_Mpeg4v : public File__Base
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
    int8u  Element_Name;
    int64u Element_Size;
    int64u Element_Next;

    //Count of a Packet
    size_t Frame_Count;
    size_t IVOP_Count;
    size_t PVOP_Count;
    size_t BVOP_Count;
    size_t SVOP_Count;
    size_t NVOP_Count;

    //From VOL, needed in VOP
    size_t interlaced;
    size_t newpred_enable;
    size_t time_size;
    size_t reduced_resolution_vop_enable;
    size_t shape;
    size_t sprite_enable;
    size_t scalability;
    size_t enhancement_type;
    size_t complexity_estimation_disable;

    //Temp
    Ztring Library;
    size_t Chroma;
    size_t Standard;
    size_t PictureStructure;

    //Packets
    void video_object_start();
    void video_object_layer_start();
    void fgs_bp_start();
    void visual_object_sequence_start();
    void visual_object_sequence_end();
    void user_data_start();
    void group_of_vop_start();
    void video_session_error();
    void visual_object_start();
    void vop_start();
    void slice_start();
    void extension_start();
    void fgs_vop_start();
    void fba_object_start();
    void fba_object_plane_start();
    void mesh_object_start();
    void mesh_object_plane_start();
    void still_texture_object_start();
    void texture_spatial_layer_start();
    void texture_snr_layer_start();
    void texture_tile_start();
    void texture_shape_layer_start();
    void system_start();

    //Error controls
    std::vector<int8u> Frame_ShouldBe;

    //Helpers
    bool Detect_NonMPEG4V();
    bool Element_Size_Get();
    bool Element_Name_IsOK();

public :
    //Information for container
    size_t RIFF_VOP_Count; //RIFF limited : count the number of VOP in this chunk
    size_t RIFF_VOP_Count_Max; //RIFF limited : count the number of VOP in this chunk
};

} //NameSpace

#endif
