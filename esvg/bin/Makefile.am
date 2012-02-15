MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS = esvg_test

if BUILD_EMAGE
bin_PROGRAMS += esvg_loader
endif

esvg_test_CPPFLAGS = \
-I$(top_builddir) \
-I$(top_srcdir)/edom/include \
-I$(top_srcdir)/esvg/include \
@ESVG_CFLAGS@
esvg_test_SOURCES = esvg_main.c
esvg_test_LDADD = $(top_builddir)/esvg/lib/libesvg.la @ESVG_LIBS@
esvg_test_LDFLAGS =

esvg_loader_CPPFLAGS = \
-I$(top_builddir) \
-I$(top_srcdir)/esvg/include \
@ESVG_CFLAGS@ \
@ESVG_EMAGE_CFLAGS@

esvg_loader_SOURCES = esvg_loader.c
esvg_loader_LDADD = $(top_builddir)/esvg/lib/libesvg.la @ESVG_EMAGE_LIBS@ @ESVG_LIBS@ -lm
esvg_loader_LDFLAGS =
