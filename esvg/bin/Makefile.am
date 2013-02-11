MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS = esvg_loader

esvg_loader_CPPFLAGS = \
-I$(top_srcdir)/edom/lib \
-I$(top_srcdir)/esvg/lib \
-I$(top_srcdir)/esvg/lib/types \
-I$(top_srcdir)/esvg/lib/elements \
@ESVG_CFLAGS@

esvg_loader_SOURCES = esvg_loader.c
esvg_loader_LDADD = $(top_builddir)/esvg/lib/libesvg.la @ESVG_LIBS@ -lm
esvg_loader_LDFLAGS =
