MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/ecss/lib \
-I$(top_srcdir)/edom/lib \
-I$(top_srcdir)/esvg/lib \
-I$(top_builddir)/esvg/lib \
-DENESIM_EXTENSION \
-DESVG_BUILD \
@ESVG_CFLAGS@

noinst_LTLIBRARIES = libesvg_types.la

libesvg_types_la_SOURCES = \
esvg_boolean.c \
esvg_color.c \
esvg_duration.c \
esvg_length.c \
esvg_matrix.c \
esvg_number.c \
esvg_paint.c \
esvg_point.c \
esvg_rect.c \
esvg_string.c

libesvg_types_la_LIBADD =

