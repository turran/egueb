MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/ecss/lib \
-I$(top_srcdir)/edom/lib \
-I$(top_srcdir)/esvg/lib \
-I$(top_srcdir)/esvg/lib/types \
-I$(top_srcdir)/esvg/lib/elements \
-DESVG_BUILD \
@ESVG_CFLAGS@

# PNG
emage_svg_LTLIBRARIES = emage_svg.la
emage_svgdir = $(libdir)/emage

emage_svg_la_SOURCES = \
emage_svg.c

emage_svg_la_LIBADD = $(top_builddir)/esvg/lib/libesvg.la @ESVG_LIBS@
emage_svg_la_LDFLAGS = -no-undefined -module -avoid-version
emage_svg_la_LIBTOOLFLAGS = --tag=disable-static
