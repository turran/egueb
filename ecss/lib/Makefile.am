MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/ecss/include \
-DECSS_BUILD \
@ECSS_CFLAGS@

AM_YFLAGS = -d -p ecss_
AM_LFLAGS = -o$(LEX_OUTPUT_ROOT).c

CLEANFILES = \
ecss_lexer.c \
ecss_grammar.c \
ecss_grammar.h

lib_LTLIBRARIES = libecss.la

libecss_la_SOURCES = \
ecss_context.c \
ecss_main.c \
ecss_rule.c \
ecss_selector.c \
ecss_style.c \
ecss_grammar.y \
ecss_filter.c \
ecss_filter_class.c \
ecss_filter_id.c \
ecss_lexer.l

libecss_la_LIBADD = \
@ECSS_LIBS@ \
@LEXLIB@

libecss_la_LDFLAGS = -no-undefined -version-info @version_info@
