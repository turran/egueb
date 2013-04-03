
CLEANFILES += \
src/lib/css/ecss_lexer.c \
src/lib/css/ecss_grammar.c \
src/lib/css/ecss_grammar.h

lib_LTLIBRARIES += src/lib/css/libecss.la

installed_cssheadersdir = $(pkgincludedir)-@VMAJ@/css
dist_installed_cssheaders_DATA = src/lib/css/Ecss.h

src_lib_css_libecss_la_SOURCES = \
src/lib/css/ecss_context.c \
src/lib/css/ecss_main.c \
src/lib/css/ecss_rule.c \
src/lib/css/ecss_selector.c \
src/lib/css/ecss_style.c \
src/lib/css/ecss_grammar.y \
src/lib/css/ecss_filter.c \
src/lib/css/ecss_filter_class.c \
src/lib/css/ecss_filter_id.c \
src/lib/css/ecss_lexer.l \
src/lib/css/ecss_private.h

src_lib_css_libecss_la_YFLAGS = -d -p ecss_
src_lib_css_libecss_la_LFLAGS = -o$(LEX_OUTPUT_ROOT).c

src_lib_css_libecss_la_CPPFLAGS = -DECSS_BUILD @ECSS_CFLAGS@

src_lib_css_libecss_la_LIBADD = @ECSS_LIBS@ @LEXLIB@

src_lib_css_libecss_la_LDFLAGS = -no-undefined -version-info @version_info@