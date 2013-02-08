MAINTAINERCLEANFILES = Makefile.in

check_PROGRAMS = test01

test01_SOURCES = test01.c
test01_CPPFLAGS = -I$(top_srcdir)/ecss/lib @ECSS_CFLAGS@
test01_LDADD = $(top_builddir)/ecss/lib/libecss.la @ECSS_LIBS@
