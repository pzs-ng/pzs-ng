INCLUDES = \
	-I$(top_srcdir)/src/zipscript
	
bin_PROGRAMS = sitewho

sitewho_SOURCES = sitewho.c

noinst_HEADERS = sitewho.h

extra_DIST = \
	README \
	CHANGELOG \
	who.foot \
	who.head \
	sitewho.conf

install-data-local:
	@([ -f "$(prefix)/ftp-data/misc/who.foot" ] || $(INSTALL) -m644 who.foot $(prefix)/ftp-data/misc)
	@([ -f "$(prefix)/ftp-data/misc/who.head" ] || $(INSTALL) -m644 who.head $(prefix)/ftp-data/misc)
	@([ -f "$(prefix)/bin/sitewho.conf" ]       || $(INSTALL) -m644 sitewho.conf $(prefix)/bin)
