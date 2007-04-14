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
### It will try to make all .cpp file in directory below
MEDIAINFO_SRC = ../../../Source/MediaInfo

#############################################################################
### Flags
MI_CFLAGS   = $(MI_PROJECT_CFLAGS)  -O3 -Wall                           $(MI_GLOBAL_CFLAGS)
MI_INCS     = $(MI_PROJECT_INCS)    -I../../../Source                   $(MI_GLOBAL_INCS)

CXX         = $(MI_CPP)
CXXFLAGS    = $(MI_CFLAGS) -I. $(MI_INCS)

SOURCES     = $(wildcard $(MEDIAINFO_SRC)/*.cpp) \
              $(wildcard $(MEDIAINFO_SRC)/Archive/*.cpp) \
              $(wildcard $(MEDIAINFO_SRC)/Audio/*.cpp) \
              $(wildcard $(MEDIAINFO_SRC)/Image/*.cpp) \
              $(wildcard $(MEDIAINFO_SRC)/Multiple/*.cpp) \
              $(wildcard $(MEDIAINFO_SRC)/Video/*.cpp) \
              $(wildcard $(MEDIAINFO_SRC)/Text/*.cpp) \

OBJECTS     = $(patsubst %.cpp,%.o,$(SOURCES))

BIN         = $(MI_BIN)

#############################################################################
### Compilation
$(BIN): $(OBJECTS)
	ar r $(BIN) $(OBJECTS)
	ranlib $(BIN)

#############################################################################
### Make dependencies list for all cpp and .h files
DEPENDS=$(patsubst %.cpp,%.d,$(SOURCES))
DEPENDS_NOEXT=$(patsubst %.cpp,%,$(SOURCES))

depend::
	for i in $(DEPENDS_NOEXT) ; \
        do \
        echo $(MI_CPP) -MM $(MI_CFLAGS) -MF $$i.d -MT $$i.o $(MI_INCS) $$i.cpp ;\
	$(MI_CPP) -MM $(MI_CFLAGS) -MF $$i.d -MT $$i.o $(MI_INCS) $$i.cpp ;\
	done
