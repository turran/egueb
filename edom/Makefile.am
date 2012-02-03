MAINTAINERCLEANFILES = \
Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
@EFL_ESVG_BUILD@ \
@ESVG_CFLAGS@

noinst_LTLIBRARIES = libedom.la

libedom_la_SOURCES = \
edom_tag.c \
edom_context.c \
edom_parser.c

libedom_la_LIBADD =

libedom_la_DEPENDENCIES =
