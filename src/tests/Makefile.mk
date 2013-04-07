tests_dom_CPPFLAGS = \
-I$(top_srcdir)/src/lib/dom \
@EDOM_CFLAGS@

tests_dom_LDADD = \
$(top_builddir)/src/lib/libenesim.la \
@EDOM_LIBS@

tests_css_CPPFLAGS = \
-I$(top_srcdir)/src/lib/css \
@ECSS_CFLAGS@

tests_css_LDADD = \
$(top_builddir)/src/lib/css/libecss.la \
@ECSS_LIBS@

check_PROGRAMS += \
src/tests/egueb_css_test01 \
src/tests/egueb_dom_node01

src_tests_egueb_dom_node01_SOURCES = src/tests/egueb_dom_node01.c
src_tests_egueb_dom_node01_CPPFLAGS = $(tests_dom_CPPFLAGS)
src_tests_egueb_dom_node01_LDADD = $(tests_dom_LDADD)

src_tests_egueb_css_test01_SOURCES = src/tests/egueb_css_test01.c
src_tests_egueb_css_test01_CPPFLAGS = $(tests_css_CPPFLAGS)
src_tests_egueb_css_test01_LDADD = $(tests_css_LDADD)

