MAINTAINERCLEANFILES = \
Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
-I$(top_srcdir)/src/lib/edom \
@ESVG_EMAGE_CFLAGS@

# PNG
if BUILD_EMAGE
emage_svg_LTLIBRARIES = emage_svg.la
emage_svgdir = $(libdir)/emage

emage_svg_la_SOURCES = \
emage_svg.c

emage_svg_la_LIBADD = $(top_builddir)/src/lib/libesvg.la @ESVG_EMAGE_LIBS@
emage_svg_la_LDFLAGS = -no-undefined -module -avoid-version
endif
