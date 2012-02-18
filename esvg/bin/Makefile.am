MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS =

if BUILD_EMAGE
bin_PROGRAMS += esvg_loader
endif

esvg_loader_CPPFLAGS = \
-I$(top_builddir) \
-I$(top_srcdir)/esvg/include \
@ESVG_CFLAGS@ \
@ESVG_EMAGE_CFLAGS@

esvg_loader_SOURCES = esvg_loader.c
esvg_loader_LDADD = $(top_builddir)/esvg/lib/libesvg.la @ESVG_EMAGE_LIBS@ @ESVG_LIBS@ -lm
esvg_loader_LDFLAGS =
