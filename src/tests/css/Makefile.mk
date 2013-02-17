
check_PROGRAMS += src/tests/css/test01

src_tests_css_test01_SOURCES = test01.c
src_tests_css_test01_CPPFLAGS = -I$(top_srcdir)/ecss/lib @ECSS_CFLAGS@
src_tests_css_test01_LDADD = src/lib/css/lib/libecss.la @ECSS_LIBS@
