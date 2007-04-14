## Makefile for MediaInfo
## Copyright (C) 2002-2006 Jerome Martinez, Zen@MediaArea.net
## Copyright (C) 2006-2006 Richard Buteau
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
##
#############################################################################
#############################################################################

#############################################################################
### Local configuration
MI_CPP             = g++
MI_LIB             = ar
MI_GLOBAL_CFLAGS   = -march=i386 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D_LARGEFILE_SOURCE=1
MI_GLOBAL_INCS     = -I../../../../Shared/Include

#############################################################################
### Project configuration
MI_BIN             = ../../../Release/GCC_Linux_i386/Library/libMediaInfo.a
MI_PROJECT_CFLAGS  = -DUNICODE -D_UNICODE -DMEDIAINFO_APE_NO -DMEDIAINFO_FLAC_NO
MI_PROJECT_INCS    = -I../../../Source

#############################################################################
### Do no edit after this line
include Main.mak

# Some cleanup
clean:
	rm -rf $(OBJECTS)
	rm -rf $(MI_BIN)
	rm -rf $(DEPENDS)
