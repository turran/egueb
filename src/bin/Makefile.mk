
if BUILD_EGUEB_DOM

bin_PROGRAMS += src/bin/egueb_dom_loader

src_bin_egueb_dom_loader_CPPFLAGS = \
$(src_lib_dom_libegueb_dom_la_IFLAGS) \
@EGUEB_DOM_CFLAGS@

src_bin_egueb_dom_loader_SOURCES =  src/bin/egueb_dom_loader.c
src_bin_egueb_dom_loader_LDADD = \
$(top_builddir)/src/lib/dom/libegueb_dom.la \
@EGUEB_DOM_LIBS@ -lm

endif
