ACLOCAL_AMFLAGS = -I m4

SUBDIRS =

if BUILD_ECSS
SUBDIRS += ecss
endif

if BUILD_EDOM
SUBDIRS += edom
endif

if BUILD_ESVG
SUBDIRS += esvg
endif

MAINTAINERCLEANFILES = \
Makefile.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA =

if BUILD_ECSS
pkgconfig_DATA += ecss.pc
endif

if BUILD_EDOM
pkgconfig_DATA += edom.pc
endif

if BUILD_ESVG
pkgconfig_DATA += esvg.pc
endif
