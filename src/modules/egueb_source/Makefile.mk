if BUILD_EGUEB_SVG

egueb_source_svg_LTLIBRARIES = src/modules/egueb_source/egueb_source_svg.la
egueb_source_svgdir = $(pkglibdir)/source

src_modules_egueb_source_egueb_source_svg_la_SOURCES = \
src/modules/egueb_source/egueb_source_svg.c

src_modules_egueb_source_egueb_source_svg_la_CPPFLAGS = \
-I$(top_srcdir)/src/lib/css \
$(src_lib_dom_libegueb_dom_la_IFLAGS) \
-I$(top_srcdir)/src/lib/svg \
-DEGUEB_SVG_BUILD \
@EGUEB_SVG_CFLAGS@

src_modules_egueb_source_egueb_source_svg_la_LIBADD = \
$(top_builddir)/src/lib/dom/libegueb_dom.la \
$(top_builddir)/src/lib/svg/libegueb_svg.la \
@EGUEB_SVG_LIBS@

src_modules_egueb_source_egueb_source_svg_la_LDFLAGS = -no-undefined -module -avoid-version

src_modules_egueb_source_egueb_source_svg_la_LIBTOOLFLAGS = --tag=disable-static

endif
