MAINTAINERCLEANFILES = Makefile.in

lib_LTLIBRARIES = libedom.la

include_HEADERS = Edom.h

libedom_la_SOURCES = \
edom_main.c \
edom_tag.c \
edom_parser.c \
edom_private.h

libedom_la_CPPFLAGS = \
-DEDOM_BUILD \
@EDOM_CFLAGS@

libedom_la_LIBADD = \
@EDOM_LIBS@

libedom_la_LDFLAGS = -no-undefined -version-info @version_info@
