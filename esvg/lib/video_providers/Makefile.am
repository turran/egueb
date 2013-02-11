MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/ecss/lib \
-I$(top_srcdir)/edom/lib \
-I$(top_srcdir)/esvg/lib \
-I$(top_srcdir)/esvg/lib/types \
-I$(top_srcdir)/esvg/lib/elements \
-DENESIM_EXTENSION \
-DESVG_BUILD \
@ESVG_CFLAGS@

noinst_LTLIBRARIES = libesvg_video_providers.la

libesvg_video_providers_la_SOURCES =
libesvg_video_providers_la_LIBADD =

if BUILD_ESVG_VIDEO_GSTREAMER
AM_CPPFLAGS += @GSTREAMER_CFLAGS@
libesvg_video_providers_la_SOURCES += esvg_video_provider_gstreamer.c
libesvg_video_providers_la_LIBADD += @GSTREAMER_LIBS@
endif
