MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/edom/include \
-I$(top_builddir)/edom/include \
@EDOM_BUILD@ \
@EDOM_CFLAGS@

lib_LTLIBRARIES = libedom.la

libedom_la_SOURCES = \
edom_tag.c \
edom_parser.c

libedom_la_LIBADD = \
@EDOM_LIBS@

libedom_la_DEPENDENCIES =

libedom_la_LDFLAGS = -no-undefined -version-info @version_info@

