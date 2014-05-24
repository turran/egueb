
if BUILD_EGUEB_DOM

bin_PROGRAMS += src/bin/egueb_dom_loader

src_bin_egueb_dom_loader_CPPFLAGS = \
-I$(top_srcdir)/src/lib/dom \
@EGUEB_DOM_CFLAGS@

src_bin_egueb_dom_loader_SOURCES =  src/bin/egueb_dom_loader.c
src_bin_egueb_dom_loader_LDADD = \
$(top_builddir)/src/lib/dom/libegueb_dom.la \
@EGUEB_DOM_LIBS@ -lm

endif
