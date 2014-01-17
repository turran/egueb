
if BUILD_ESVG

bin_PROGRAMS += src/bin/egueb_svg_loader

src_bin_egueb_svg_loader_CPPFLAGS = \
-I$(top_srcdir)/src/lib/dom \
-I$(top_srcdir)/src/lib/svg \
@ESVG_CFLAGS@

src_bin_egueb_svg_loader_SOURCES =  src/bin/egueb_svg_loader.c
src_bin_egueb_svg_loader_LDADD = \
$(top_builddir)/src/lib/svg/libegueb_svg.la \
$(top_builddir)/src/lib/dom/libegueb_dom.la \
@ESVG_LIBS@ -lm

endif
