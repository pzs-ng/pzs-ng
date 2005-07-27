bin_PROGRAMS = \
	zipscript-c \
	postdel \
	racestats \
	cleanup \
	datacleaner \
	rescan

INCLUDES = \
	-I$(top_srcdir)/src/glufile

zipscript_c_SOURCES = \
	zipscript-c.c \
	helpfunctions.c \
	race-file.c \
	abs2rel.c \
	mp3info.c \
	ng-version.c \
	stats.c \
	zsfunctions.c \
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
	convert.c \
	multimedia.c

EXTRA_zipscript_c_SOURCES = \
	strlcpy.c \
	strsep.c

if DONT_HAVE_STRLCPY
zipscript_c_SOURCES += strlcpy.c
endif

if DONT_HAVE_STRSEP
zipscript_c_SOURCES += strsep.c
endif

zipscript_c_DEPENDENCIES = ../glufile/libglufile.la

zipscript_c_LDADD = ../glufile/libglufile.la

postdel_SOURCES = \
	postdel.c \
	ng-version.c \
	abs2rel.c \
	helpfunctions.c \
	zsfunctions.c \
	race-file.c \
	convert.c \
	dizreader.c \
	stats.c
        
if DONT_HAVE_STRLCPY
postdel_SOURCES += strlcpy.c
endif

postdel_DEPENDENCIES = ../glufile/libglufile.la

postdel_LDADD = ../glufile/libglufile.la

racestats_SOURCES = \
	racestats.c \
	zsfunctions.c \
	race-file.c \
	convert.c \
	dizreader.c \
	stats.c

rescan_SOURCES = \
	rescan.c \
	zsfunctions.c \
	race-file.c \
	multimedia.c \
	convert.c \
	dizreader.c \
	stats.c \
	complete.c \
	crc.c \
	ng-version.c

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
