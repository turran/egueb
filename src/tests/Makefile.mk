tests_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/lib \
-I$(top_srcdir)/src/lib/renderer \
-I$(top_srcdir)/src/lib/object \
@EDOM_CFLAGS@

tests_LDADD = \
$(top_builddir)/src/lib/libenesim.la \
@EDOM_LIBS@

check_PROGRAMS = \
src/tests/egueb_dom_node01

src_tests_egueb_dom_node01_SOURCES = src/tests/egueb_dom_node01.c
src_tests_egueb_dom_node01_LDADD = $(tests_LDADD)
src_tests_egueb_dom_node01_CPPFLAGS = $(tests_CPPFLAGS)

