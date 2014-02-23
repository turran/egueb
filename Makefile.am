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

include src/lib/base/Makefile.mk

if BUILD_EGUEB_CSS
include src/lib/css/Makefile.mk
endif

if BUILD_EGUEB_DOM
include src/lib/dom/Makefile.mk
include src/lib/dom/parser/Makefile.mk
endif

if BUILD_EGUEB_SMIL
include src/lib/smil/Makefile.mk
endif

if BUILD_EGUEB_SVG
include src/lib/svg/Makefile.mk
# include src/lib/svg/elements/Makefile.mk
# include src/lib/svg/types/Makefile.mk
# include src/lib/svg/image_providers/Makefile.mk
# include src/lib/svg/script_providers/Makefile.mk
# include src/lib/svg/video_providers/Makefile.mk
endif

### Modules

if BUILD_EGUEB_SVG
include src/modules/svg/enesim_image/Makefile.mk
endif

### Binaries
include src/bin/Makefile.mk

### Tests

if BUILD_EGUEB_CSS
include src/tests/Makefile.mk
endif

### The rule for the generated files
CLEANFILES += $(GENERATED_FILES)
BUILT_SOURCES = $(GENERATED_FILES)

$(GENERATED_FILES): $(top_srcdir)/data/svg/esvg_private.ender
	ender_generator $(GENERATED_FLAGS) $(value $(addsuffix _GFLAGS,$(basename $(notdir $@)))) $(top_srcdir)/data/svg/esvg_private.ender $(basename $(notdir $(subst esvg_generated_,,$@))) $@

sed_process = \
$(SED) \
-e 's,@VERSION\@,$(VERSION),g' \
-e 's,@VMAJ\@,$(VMAJ),g' \
-e 's,@prefix\@,$(prefix),g' \
-e 's,@exec_prefix\@,$(exec_prefix),g' \
-e 's,@libdir\@,$(libdir),g' \
-e 's,@includedir\@,$(includedir),g' \
-e 's,@pkgincludedir\@,$(pkgincludedir),g' \
-e 's,@edom_requirements\@,$(edom_requirements),g' \
-e 's,@edom_requirements_private\@,$(edom_requirements_private),g' \
-e 's,@ecss_requirements\@,$(ecss_requirements),g' \
-e 's,@esvg_requirements\@,$(esvg_requirements),g' \
-e 's,@egueb_base_requirements\@,$(egueb_base_requirements),g' \
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

pkgconfig_DATA += egueb-base.pc

if BUILD_EGUEB_CSS
pkgconfig_DATA += egueb-css.pc
endif

if BUILD_EGUEB_DOM
pkgconfig_DATA += egueb-dom.pc
pkgconfig_DATA += egueb-smil.pc
endif

if BUILD_EGUEB_SVG
pkgconfig_DATA += egueb-svg.pc
endif

### Documentation

include doc/Makefile.mk

### Coverage

include common/Makefile_coverage.mk
