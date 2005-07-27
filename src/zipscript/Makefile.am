bin_PROGRAMS = \
	zipscript-c \
	postdel \
	racestats \
	cleanup \
	datacleaner \
	rescan

INCLUDES = \
	-I$(top_srcdir)/src/glufile \
	-I$(top_srcdir)/src/strlcpy

UNIVERSAL = \
	stats.c \
	convert.c \
	race-file.c \
	helpfunctions.c \
	mp3info.c \
	zsfunctions.c \
	ng-version.c \
	abs2rel.c

if DONT_HAVE_STRLCPY
UNIVERSAL += strlcpy.c
endif

if DONT_HAVE_STRSEP
UNIVERSAL += strsep.c
endif

zipscript_c_SOURCES = \
	zipscript-c.c \
	dizreader.c \
	handle_sfv.c \
	handle_nfo.c \
	handle_zip.c \
	handle_any.c \
	handling.c \
	crc.c \
	avinfo.c \
	racetypes.c \
	complete.c \
	multimedia.c \
	$(UNIVERSAL)

EXTRA_zipscript_c_SOURCES = \
	strlcpy.c \
	strsep.c

zipscript_c_DEPENDENCIES = ../glufile/libglufile.la

zipscript_c_LDADD = ../glufile/libglufile.la

postdel_SOURCES = \
	postdel.c \
	dizreader.c \
	$(UNIVERSAL)
        
if DONT_HAVE_STRLCPY
postdel_SOURCES += strlcpy.c
endif

postdel_DEPENDENCIES = ../glufile/libglufile.la

postdel_LDADD = ../glufile/libglufile.la

racestats_SOURCES = \
	racestats.c \
	dizreader.c \
	$(UNIVERSAL)

racestats_DEPENDENCIES = ../glufile/libglufile.la

racestats_LDADD = ../glufile/libglufile.la

rescan_SOURCES = \
	rescan.c \
	multimedia.c \
	dizreader.c \
	complete.c \
	crc.c \
	$(UNIVERSAL)

rescan_DEPENDENCIES = ../glufile/libglufile.la

rescan_LDADD = ../glufile/libglufile.la

cleanup_SOURCES = \
	cleanup.c

datacleaner_SOURCES = \
	datacleaner.c

noinst_HEADERS = \
	abs2rel.h \
	audio.h \
	avi.h \
	avinfo.h \
	cleanup.h \
	complete.h \
	constants.h \
	convert.h \
	crc.h \
	datacleaner.h \
	dizreader.h \
	errors.h \
	handle_any.h \
	handle_nfo.h \
	handlers.h \
	handle_sfv.h \
	handle_zip.h \
	handling.h \
	helpfunctions.h \
	macros.h \
	mp3info.h \
	multimedia.h \
	ng-version.h \
	objects.h \
	postdel.h \
	race-file.h \
	racetypes.h \
	scandir.h \
	stats.h \
	strlcpy.h \
	strsep.h \
	video.h \
	zsconfig.defaults.h \
	zsfunctions.h
