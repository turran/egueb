lib_LTLIBRARIES += src/lib/css/libegueb_css.la

installed_cssheadersdir = $(pkgincludedir)-@VMAJ@/css
dist_installed_cssheaders_DATA = \
src/lib/css/Egueb_Css.h \
src/lib/css/egueb_css_color.h \
src/lib/css/egueb_css_element_style.h \
src/lib/css/egueb_css_font.h \
src/lib/css/egueb_css_font_family.h \
src/lib/css/egueb_css_font_size.h \
src/lib/css/egueb_css_font_style.h \
src/lib/css/egueb_css_font_variant.h \
src/lib/css/egueb_css_font_weight.h \
src/lib/css/egueb_css_length.h \
src/lib/css/egueb_css_main.h \
src/lib/css/egueb_css_percentage.h

src_lib_css_libegueb_css_la_SOURCES = \
src/lib/css/egueb_css_main.c \
src/lib/css/egueb_css_length.c \
src/lib/css/egueb_css_color.c \
src/lib/css/egueb_css_element_style.c \
src/lib/css/egueb_css_font.c \
src/lib/css/egueb_css_font_private.h \
src/lib/css/egueb_css_font_family.c \
src/lib/css/egueb_css_font_size.c \
src/lib/css/egueb_css_font_size_private.h \
src/lib/css/egueb_css_font_style.c \
src/lib/css/egueb_css_font_style_private.h \
src/lib/css/egueb_css_font_variant.c \
src/lib/css/egueb_css_font_variant_private.h \
src/lib/css/egueb_css_font_weight.c \
src/lib/css/egueb_css_font_weight_private.h \
src/lib/css/egueb_css_percentage.c \
src/lib/css/egueb_css_private.h

src_lib_css_libegueb_css_la_IFLAGS = \
-I$(top_srcdir)/src/lib/css \
-I$(top_srcdir)/src/lib/css/attr \
-I$(top_srcdir)/src/lib/css/engine \
-I$(top_srcdir)/src/lib/css/value

src_lib_css_libegueb_css_la_CPPFLAGS = \
$(src_lib_css_libegueb_css_la_IFLAGS) \
$(src_lib_dom_libegueb_dom_la_IFLAGS) \
-DEGUEB_CSS_BUILD \
@EGUEB_CSS_CFLAGS@

src_lib_css_libegueb_css_la_LIBADD = \
$(top_builddir)/src/lib/dom/libegueb_dom.la \
@EGUEB_CSS_LIBS@

src_lib_css_libegueb_css_la_LDFLAGS = -no-undefined -version-info @version_info@

include src/lib/css/engine/Makefile.mk
