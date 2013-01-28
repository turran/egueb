MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS = esvg_loader

esvg_loader_CPPFLAGS = \
-I$(top_builddir) \
-I$(top_srcdir)/edom/include \
-I$(top_srcdir)/esvg/include \
@ESVG_CFLAGS@

esvg_loader_SOURCES = esvg_loader.c
esvg_loader_LDADD = $(top_builddir)/esvg/lib/libesvg.la @ESVG_LIBS@ -lm
esvg_loader_LDFLAGS =
