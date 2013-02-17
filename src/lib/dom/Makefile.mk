
lib_LTLIBRARIES += src/lib/dom/libedom.la

installed_domheadersdir = $(pkgincludedir)-@VMAJ@/dom
dist_installed_domheaders_DATA = src/lib/dom/Edom.h

src_lib_dom_libedom_la_SOURCES = \
src/lib/dom/edom_main.c \
src/lib/dom/edom_tag.c \
src/lib/dom/edom_parser.c \
src/lib/dom/edom_private.h

src_lib_dom_libedom_la_CPPFLAGS = -DEDOM_BUILD @EDOM_CFLAGS@

src_lib_dom_libedom_la_LIBADD = @EDOM_LIBS@

src_lib_dom_libedom_la_LDFLAGS = -no-undefined -version-info @version_info@
