if BUILD_EGUEB_DOM_SCRIPTER_NEKO

lib_LTLIBRARIES += src/lib/dom/scripter/neko/libegueb_dom_scripter_neko.la
installed_domscripternekoheadersdir = $(pkgincludedir)-@VMAJ@/dom
dist_installed_domscripternekoheaders_DATA = \
src/lib/dom/scripter/neko/Egueb_Dom_Scripter_Neko.h

src_lib_dom_scripter_neko_libegueb_dom_scripter_neko_la_SOURCES = \
src/lib/dom/scripter/neko/egueb_dom_scripter_neko.c

src_lib_dom_scripter_neko_libegueb_dom_scripter_neko_la_CPPFLAGS = \
-I$(top_srcdir)/src/lib/base \
-I$(top_srcdir)/src/lib/dom \
@EGUEB_DOM_CFLAGS@

src_lib_dom_scripter_neko_libegueb_dom_scripter_neko_la_LIBS = \
$(top_builddir)/src/lib/base/libegueb_base.la \
$(top_builddir)/src/lib/dom/libegueb_dom.la \
@NEKO_LIBS@

src_lib_dom_scripter_neko_libegueb_dom_scripter_neko_la_LDFLAGS = -no-undefined -version-info @version_info@

endif
