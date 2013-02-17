
check_PROGRAMS += src/tests/css/test01

src_tests_css_test01_SOURCES = src/tests/css/test01.c
src_tests_css_test01_CPPFLAGS = -I$(top_srcdir)/src/lib/css @ECSS_CFLAGS@
src_tests_css_test01_LDADD = src/lib/css/libecss.la @ECSS_LIBS@
