AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4 -I common/m4

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

noinst_LTLIBRARIES =
lib_LTLIBRARIES =
bin_PROGRAMS =
check_PROGRAMS =
CLEANFILES =
EXTRA_DIST =
CLEAN_LOCAL =
GENERATED_FLAGS = -x
GENERATED_FILES =

### Libraries

if BUILD_EGUEB_DOM
include src/lib/dom/Makefile.mk
include src/lib/dom/parser/Makefile.mk
include src/lib/dom/attr/Makefile.mk
include src/lib/dom/element/Makefile.mk
include src/lib/dom/event/Makefile.mk
include src/lib/dom/feature/Makefile.mk
include src/lib/dom/value/Makefile.mk
endif

if BUILD_EGUEB_CSS
include src/lib/css/Makefile.mk
include src/lib/css/attr/Makefile.mk
include src/lib/css/value/Makefile.mk
endif

if BUILD_EGUEB_XLINK
include src/lib/xlink/Makefile.mk
include src/lib/xlink/attr/Makefile.mk
endif

if BUILD_EGUEB_SMIL
include src/lib/smil/Makefile.mk
include src/lib/smil/attr/Makefile.mk
include src/lib/smil/value/Makefile.mk
endif

if BUILD_EGUEB_SVG
include src/lib/svg/Makefile.mk
include src/lib/svg/attr/Makefile.mk
include src/lib/svg/element/Makefile.mk
include src/lib/svg/value/Makefile.mk
endif

### Modules

include src/modules/enesim_image/Makefile.mk
include src/modules/egueb_source/Makefile.mk

### Binaries
include src/bin/Makefile.mk

### Data
include data/Makefile.mk

### Tests

if BUILD_EGUEB_CSS
include src/tests/Makefile.mk
endif

sed_process = \
$(SED) \
-e 's,@VERSION\@,$(VERSION),g' \
-e 's,@VMAJ\@,$(VMAJ),g' \
-e 's,@prefix\@,$(prefix),g' \
-e 's,@exec_prefix\@,$(exec_prefix),g' \
-e 's,@libdir\@,$(libdir),g' \
-e 's,@includedir\@,$(includedir),g' \
-e 's,@pkgincludedir\@,$(pkgincludedir),g' \
-e 's,@pkglibdir\@,$(pkglibdir),g' \
-e 's,@egueb_dom_requirements\@,$(egueb_dom_requirements),g' \
-e 's,@egueb_dom_requirements_private\@,$(egueb_dom_requirements_private),g' \
-e 's,@egueb_css_requirements\@,$(egueb_css_requirements),g' \
-e 's,@egueb_css_requirements_private\@,$(egueb_css_requirements_private),g' \
-e 's,@egueb_xlink_requirements\@,$(egueb_xlink_requirements),g' \
-e 's,@egueb_xlink_requirements_private\@,$(egueb_xlink_requirements_private),g' \
-e 's,@egueb_svg_requirements\@,$(egueb_svg_requirements),g' \
-e 's,@build_opencl\@,$(build_opencl),g' \
-e 's,@build_opengl\@,$(build_opengl),g' \
< $< > $@ || rm $@

pc_verbose = $(pc_verbose_@AM_V@)
pc_verbose_ = $(pc_verbose_@AM_DEFAULT_V@)
pc_verbose_0 = @echo "  PC      " $@;

%.pc: %.pc.in Makefile
	@rm -f $@
	$(pc_verbose)$(sed_process)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA =

if BUILD_EGUEB_DOM
pkgconfig_DATA += egueb-dom.pc
endif

if BUILD_EGUEB_CSS
pkgconfig_DATA += egueb-css.pc
endif

if BUILD_EGUEB_XLINK
pkgconfig_DATA += egueb-xlink.pc
endif

if BUILD_EGUEB_SMIL
pkgconfig_DATA += egueb-smil.pc
endif

if BUILD_EGUEB_SVG
pkgconfig_DATA += egueb-svg.pc
endif

### Documentation

include common/Makefile_doxygen.mk

### Coverage

include common/Makefile_coverage.mk

EXTRA_DIST += \
AUTHORS \
COPYING \
README.md \
egueb-dom.pc.in \
egueb-css.pc.in \
egueb-xlink.pc.in \
egueb-smil.pc.in \
egueb-svg.pc.in
