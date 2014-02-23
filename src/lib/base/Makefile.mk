lib_LTLIBRARIES += src/lib/base/libegueb_base.la

installed_baseheadersdir = $(pkgincludedir)-@VMAJ@/base
dist_installed_baseheaders_DATA = \
src/lib/base/egueb_base_utils.h

src_lib_base_libegueb_base_la_SOURCES = \
src/lib/base/egueb_base_utils.c

src_lib_base_libegueb_base_la_CPPFLAGS = \
-DEGUEB_BASE_BUILD \
@EGUEB_BASE_CFLAGS@

src_lib_base_libegueb_base_la_LIBADD = @EGUEB_BASE_LIBS@
src_lib_base_libegueb_base_la_LDFLAGS = -no-undefined -version-info @version_info@

