MAINTAINERCLEANFILES = Makefile.in config.h.in~ config.h.in

GENERATED_FILES = \
esvg_generated_element.c \
esvg_generated_renderable.c \
esvg_generated_svg.c \
esvg_generated_rect.c

CLEANFILES = $(GENERATED_FILES)

BUILT_SOURCES = $(GENERATED_FILES)

%.c:	$(top_srcdir)/esvg/data/esvg_private.ender
	ender_generator $(top_srcdir)/esvg/data/esvg_private.ender $(basename $(subst esvg_generated_,,$@)) $@
