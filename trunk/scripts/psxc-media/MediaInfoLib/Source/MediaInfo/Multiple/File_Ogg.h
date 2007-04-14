// File_Ogg - Info for ogg files
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
// Information about Ogg files
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef MediaInfo_File_OggH
#define MediaInfo_File_OggH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/File__Base.h"
#include <vector>
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Class File_Ogg
//***************************************************************************

class File_Ogg : public File__Base
{
protected :
    //Formats
    void Read_Buffer_Continue ();

    //Information
    void HowTo (stream_t StreamKind);

private :
    //Buffer
    int16u Size;
    bool ShouldStop; //Offset is too far, should stop analyse

    //Positions
    std::vector<size_t> ChunkHeader_Offset;
    std::vector<int32u> ChunkHeader_ID;
    std::vector<size_t> ChunkData_Offset;
    std::vector<int16u> ChunkData_Size;
    std::vector<Ztring> Chapters_ID;
    std::vector<Ztring> Chapters_Text;

    //Chunks
    void ChunkHeader_Analyse();
    void ChunkData_Analyse();
    void Identification_Analyse();
    void Identification_Analyse(int32u ID);
    void Identification_Analyse(const int8u* Buffer, size_t Buffer_Size);
    void Identification_Vorbis_Analyse(const int8u* Buffer, size_t Buffer_Size);
    void Identification_Theora_Analyse(const int8u* Buffer, size_t Buffer_Size);
    void Identification_Video_Analyse(const int8u* Buffer, size_t Buffer_Size);
    void Identification_Audio_Analyse(const int8u* Buffer, size_t Buffer_Size);
    void Identification_Text_Analyse(const int8u* Buffer, size_t Buffer_Size);
    void Comment_Analyse();
    void Setup_Analyse();
};

} //NameSpace

#endif
