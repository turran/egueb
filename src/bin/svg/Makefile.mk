
bin_PROGRAMS += src/bin/svg/esvg_loader

src_bin_svg_esvg_loader_CPPFLAGS = \
-I$(top_srcdir)/src/lib/dom \
-I$(top_srcdir)/src/lib/svg \
-I$(top_srcdir)/src/lib/svg/types \
-I$(top_srcdir)/src/lib/svg/elements \
@ESVG_CFLAGS@

src_bin_svg_esvg_loader_SOURCES =  src/bin/svg/esvg_loader.c
src_bin_svg_esvg_loader_LDADD = \
$(top_builddir)/src/lib/svg/libesvg.la \
$(top_builddir)/src/lib/dom/libedom.la \
@ESVG_LIBS@ -lm
