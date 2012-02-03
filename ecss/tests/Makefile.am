MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
@ECSS_CFLAGS@

check_PROGRAMS = \
test01

test01_SOURCES = test01.c
test01_DEPENDENCIES = $(top_builddir)/src/lib/libecss.la
test01_LDADD = $(top_builddir)/src/lib/libecss.la @ECSS_LIBS@
