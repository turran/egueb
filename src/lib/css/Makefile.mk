
CLEANFILES += \
src/lib/css/egueb_css_lexer.c \
src/lib/css/egueb_css_grammar.c \
src/lib/css/egueb_css_grammar.h

lib_LTLIBRARIES += src/lib/css/libegueb_css.la

installed_cssheadersdir = $(pkgincludedir)-@VMAJ@/css
dist_installed_cssheaders_DATA = \
src/lib/css/Egueb_Css.h \
src/lib/css/egueb_css_color.h \
src/lib/css/egueb_css_font_size.h \
src/lib/css/egueb_css_font_style.h \
src/lib/css/egueb_css_font_variant.h \
src/lib/css/egueb_css_font_weight.h \
src/lib/css/egueb_css_font.h \
src/lib/css/egueb_css_length.h \
src/lib/css/egueb_css_main.h \
src/lib/css/egueb_css_selector.h \
src/lib/css/egueb_css_rule.h \
src/lib/css/egueb_css_style.h \
src/lib/css/egueb_css_context.h \
src/lib/css/egueb_css_percentage.h

src_lib_css_libegueb_css_la_SOURCES = \
src/lib/css/egueb_css_context.c \
src/lib/css/egueb_css_main.c \
src/lib/css/egueb_css_rule.c \
src/lib/css/egueb_css_selector.c \
src/lib/css/egueb_css_style.c \
src/lib/css/egueb_css_grammar.y \
src/lib/css/egueb_css_filter.c \
src/lib/css/egueb_css_filter_class.c \
src/lib/css/egueb_css_filter_id.c \
src/lib/css/egueb_css_lexer.l \
src/lib/css/egueb_css_length.c \
src/lib/css/egueb_css_color.c \
src/lib/css/egueb_css_font.c \
src/lib/css/egueb_css_font_size.c \
src/lib/css/egueb_css_font_style.c \
src/lib/css/egueb_css_font_variant.c \
src/lib/css/egueb_css_font_weight.c \
src/lib/css/egueb_css_percentage.c \
src/lib/css/egueb_css_private.h

src_lib_css_libegueb_css_la_YFLAGS = -d -p egueb_css_
src_lib_css_libegueb_css_la_LFLAGS = -o$(LEX_OUTPUT_ROOT).c

src_lib_css_libegueb_css_la_CPPFLAGS = \
-I$(top_srcdir)/src/lib/base \
-DEGUEB_CSS_BUILD \
@EGUEB_CSS_CFLAGS@

src_lib_css_libegueb_css_la_LIBADD = \
$(top_builddir)/src/lib/base/libegueb_base.la \
@EGUEB_CSS_LIBS@ \
@LEXLIB@

src_lib_css_libegueb_css_la_LDFLAGS = -no-undefined -version-info @version_info@
