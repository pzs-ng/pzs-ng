// File_TwinVQ - Info for TwinVQ files
// Copyright (C) 2007-2007 Jerome Martinez, Zen@MediaArea.net
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
// Information about Musepack files
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef MediaInfo_File_TwinVQH
#define MediaInfo_File_TwinVQH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/File__Base.h"
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Class File_TwinVQ
//***************************************************************************

class File_TwinVQ : public File__Base
{
protected :
    //Format
    void Read_Buffer_Continue ();

    //Information
    void HowTo (stream_t StreamKind);

private :
    //Buffer
    int32u Element_Size;
    int32u Element_Name;

    //Elements
    void Tags();
};

//***************************************************************************
// Const
//***************************************************************************

namespace TwinVQ
{
    const int32u _c__=0x28632920;
    const int32u AUTH=0x41555448;
    const int32u COMM=0x434F4D4D;
    const int32u COMT=0x434F4D54;
    const int32u DSIZ=0x4453495A;
    const int32u FILE=0x46494C45;
    const int32u NAME=0x4E414D45;
}

} //NameSpace

#endif

