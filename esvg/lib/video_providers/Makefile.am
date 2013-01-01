MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/ecss/include \
-I$(top_srcdir)/edom/include \
-I$(top_srcdir)/esvg/include \
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
