MAINTAINERCLEANFILES = \
Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_builddir)/ \
-I$(top_srcdir)/esvg/include \
-I$(top_srcdir)/ecss/include \
-I$(top_srcdir)/edom/include \
@ESVG_EMAGE_CFLAGS@

# PNG
if BUILD_EMAGE
emage_svg_LTLIBRARIES = emage_svg.la
emage_svgdir = $(libdir)/emage

emage_svg_la_SOURCES = \
emage_svg.c

emage_svg_la_LIBADD = $(top_builddir)/esvg/lib/libesvg.la @ESVG_EMAGE_LIBS@
emage_svg_la_LDFLAGS = -no-undefined -module -avoid-version
endif
