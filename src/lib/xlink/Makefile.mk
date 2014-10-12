lib_LTLIBRARIES += src/lib/xlink/libegueb_xlink.la

installed_xlinkheadersdir = $(pkgincludedir)-@VMAJ@/xlink
dist_installed_xlinkheaders_DATA = \
src/lib/xlink/Egueb_Xlink.h \
src/lib/xlink/egueb_xlink_main.h

src_lib_xlink_libegueb_xlink_la_SOURCES = \
src/lib/xlink/egueb_xlink_main.c \
src/lib/xlink/egueb_xlink_private.h

src_lib_xlink_libegueb_xlink_la_IFLAGS = \
-I$(top_srcdir)/src/lib/xlink \
-I$(top_srcdir)/src/lib/xlink/attr

src_lib_xlink_libegueb_xlink_la_CPPFLAGS = \
$(src_lib_dom_libegueb_dom_la_IFLAGS) \
$(src_lib_xlink_libegueb_xlink_la_IFLAGS) \
-DEGUEB_XLINK_BUILD \
@EGUEB_XLINK_CFLAGS@

src_lib_xlink_libegueb_xlink_la_LIBADD = \
$(top_builddir)/src/lib/dom/libegueb_dom.la \
@EGUEB_XLINK_LIBS@ -lm

src_lib_xlink_libegueb_xlink_la_LDFLAGS = -no-undefined -version-info @version_info@

