// File_Cdxa - Info for CDXA files
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
// Information about CDXA files
// (like Video-CD...)
// CDXA are read by MS-Windows with CRC bytes
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef MediaInfo_File_CdxaH
#define MediaInfo_File_CdxaH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/File__Base.h"
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

class MediaInfo;

//***************************************************************************
// Class File_Cdxa
//***************************************************************************

class File_Cdxa : public File__Base
{
public :
    //Constructor/Destructor
    File_Cdxa();
    ~File_Cdxa();

    //Format
    void Read_Buffer_Init     ();
    void Read_Buffer_Continue ();
    void Read_Buffer_Finalize ();

    //Information
    void HowTo (stream_t StreamKind);

private :
    //Buffer
    bool Buffer_Parse ();
    bool Element_Parse();
    size_t Element_Size;
    size_t Element_Next;

    //Packet
    void Header ();
    void Chunk ();

    //Helpers
    bool IsHeader();
    bool Synchronize();
    bool Synchronized_Verify();
    bool Element_Size_Get();
    bool Frame_Parse();
    bool Detect_EOF_Needed();

    //Temp
    MediaInfo* MI;
};

} //NameSpace

#endif
