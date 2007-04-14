// File_Mpegts - Info for MPEG Transport Stream files
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
// Information about MPEG Transport Stream files
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef MediaInfo_MpegTsH
#define MediaInfo_MpegTsH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/File__Base.h"
#include "MediaInfo/Multiple/File_MpegPs.h"
#include <vector>
#include <map>
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Class File_MpegTs
//***************************************************************************

class File_MpegTs : public File__Base
{
protected :
    //Formats
    void Read_Buffer_Init ();
    void Read_Buffer_Unsynched ();
    void Read_Buffer_Continue ();
    void Read_Buffer_Finalize ();

    //Information
    void HowTo (stream_t StreamKind);

public :
    File_MpegTs();
    ~File_MpegTs();

private :
    bool Buffer_Parse();
    bool Frame_Parse();
    bool Element_Parse();
    int16u Element_Name;
    size_t Element_Size;
    size_t Element_HeaderSize;
    size_t Element_Next;

    //TS
    size_t TS_Packet_Count;
    size_t Streams_Count;
    int32u Network_PID;
    std::vector<int32u> ProgramMap_PID;
    bool   PayLoad_Start;

    //Elements
    void ProgramAssociationTable();
    void ProgramAssociationTable_program_association_section();
    void ConditionalAccessTable();
    void Reserved();
    void Null();
    void TS_Program_Map_Section();
    void TS_Network_Section();
    void PES();

    std::vector<File_MpegPs*> TS_Handle;
    std::vector<bool> PES_Needed;
    std::vector<bool> TS_Needed;
    std::vector<int32u> TS_UsefulPID;
    std::map<int32u, int32u> TS_UsefulPID_StreamID;
    std::vector<int64u> TimeStamp_Start;
    std::vector<int64u> TimeStamp_End;

    //Helpers
    bool   Synchronize();
    bool   Detect_EOF_Needed();
    void   AdaptationField();
    void   Frame_Parse_Scrambled();
};

} //NameSpace

#endif
