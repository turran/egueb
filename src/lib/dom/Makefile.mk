
lib_LTLIBRARIES += src/lib/dom/libedom.la

installed_domheadersdir = $(pkgincludedir)-@VMAJ@/dom
dist_installed_domheaders_DATA = \
src/lib/dom/Egueb_Dom.h \
src/lib/dom/egueb_dom_descriptor.h \
src/lib/dom/egueb_dom_node.h \
src/lib/dom/egueb_dom_element.h \
src/lib/dom/egueb_dom_main.h

src_lib_dom_libedom_la_SOURCES = \
src/lib/dom/egueb_dom_descriptor.c \
src/lib/dom/egueb_dom_element.c \
src/lib/dom/egueb_dom_element_private.h \
src/lib/dom/egueb_dom_node.c \
src/lib/dom/egueb_dom_node_private.h \
src/lib/dom/egueb_dom_main.c \
src/lib/dom/egueb_dom_tag.c \
src/lib/dom/egueb_dom_parser.c \
src/lib/dom/egueb_dom_private.h

src_lib_dom_libedom_la_CPPFLAGS = -DEDOM_BUILD @EDOM_CFLAGS@

src_lib_dom_libedom_la_LIBADD = @EDOM_LIBS@

src_lib_dom_libedom_la_LDFLAGS = -no-undefined -version-info @version_info@
