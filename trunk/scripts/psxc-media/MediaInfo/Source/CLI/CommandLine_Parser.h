// MediaInfo_CLI - A Command Line Interface for MediaInfoLib
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

//---------------------------------------------------------------------------
#ifndef CommandLine_ParserH
#define CommandLine_ParserH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "Common/Core.h"
#include "Config.h"
//---------------------------------------------------------------------------

//***************************************************************************
//
//***************************************************************************

int Parse(Core &I, MediaInfoLib::String &Argument);


//---------------------------------------------------------------------------
#define CL_METHOD(_NAME) \
    int _NAME(Core &MI, MediaInfoLib::String &Argument)

#define CL_OPTION(_NAME) \
    int _NAME(Core &MI, MediaInfoLib::String &Argument, MediaInfoLib::String &Option, MediaInfoLib::String &Value)

CL_OPTION(f);
CL_OPTION(Help);
CL_OPTION(Help_xxx);
CL_OPTION(Info_Parameters);
CL_OPTION(Inform);
CL_OPTION(Language);
CL_OPTION(Output_HTML);
CL_OPTION(Version);

//***************************************************************************
//
//***************************************************************************

#endif
