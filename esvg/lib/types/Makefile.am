MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/ecss/include \
-I$(top_srcdir)/edom/include \
-I$(top_srcdir)/esvg/include \
-I$(top_builddir)/ecss/include \
-I$(top_builddir)/edom/include \
-I$(top_builddir)/esvg/include \
-DENESIM_EXTENSION \
@ESVG_BUILD@ \
@ESVG_CFLAGS@

noinst_LTLIBRARIES = libesvg_types.la

libesvg_types_la_SOURCES = \
esvg_boolean.c \
esvg_color.c \
esvg_length.c \
esvg_matrix.c \
esvg_number.c \
esvg_paint.c \
esvg_point.c \
esvg_viewbox.c

libesvg_types_la_LIBADD =

