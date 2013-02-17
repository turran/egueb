AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4

MAINTAINERCLEANFILES = \
Makefile.in \
aclocal.m4 \
compile \
config.guess \
config.h.in \
config.h.in~ \
config.sub \
configure \
depcomp \
install-sh \
ltmain.sh \
missing \
ylwrap \
$(PACKAGE)-$(VERSION).tar.bz2 \
$(PACKAGE)-$(VERSION).tar.gz \
m4/libtool.m4 \
m4/lt~obsolete.m4 \
m4/ltoptions.m4 \
m4/ltsugar.m4 \
m4/ltversion.m4

lib_LTLIBRARIES =
bin_PROGRAMS =
check_PROGRAMS =
CLEANFILES =
EXTRA_DIST =

### Libraries

if BUILD_ECSS
include src/lib/css/Makefile.mk
endif

if BUILD_EDOM
include src/lib/dom/Makefile.mk
endif

if BUILD_ESVG
include src/lib/svg/generated/Makefile.mk
include src/lib/svg/elements/Makefile.mk
include src/lib/svg/types/Makefile.mk
include src/lib/svg/image_providers/Makefile.mk
include src/lib/svg/script_providers/Makefile.mk
include src/lib/svg/video_providers/Makefile.mk
include src/lib/svg/Makefile.mk
endif

### Modules

if BUILD_ESVG
include src/modules/svg/enesim_image/Makefile.mk
endif

### Binaries

if BUILD_ESVG
include src/bin/svg/Makefile.mk
endif

### Data

if BUILD_EDOM
include data/dom/Makefile.mk
endif

if BUILD_ESVG
include data/svg/Makefile.mk
endif

### Tests

if BUILD_ECSS
include src/tests/css/Makefile.mk
endif


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
