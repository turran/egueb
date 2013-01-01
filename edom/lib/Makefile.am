MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/edom/include \
-DEDOM_BUILD \
@EDOM_CFLAGS@

lib_LTLIBRARIES = libedom.la

libedom_la_SOURCES = \
edom_main.c \
edom_tag.c \
edom_parser.c

libedom_la_LIBADD = \
@EDOM_LIBS@

libedom_la_LDFLAGS = -no-undefined -version-info @version_info@

