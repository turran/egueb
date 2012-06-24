MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/ecss/include \
@ECSS_CFLAGS@

check_PROGRAMS = \
test01

test01_SOURCES = test01.c
test01_DEPENDENCIES = $(top_builddir)/ecss/lib/libecss.la
test01_LDADD = $(top_builddir)/ecss/lib/libecss.la @ECSS_LIBS@
