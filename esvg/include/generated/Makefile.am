MAINTAINERCLEANFILES = Makefile.in config.h.in~ config.h.in

BUILT_SOURCES = \
esvg_generated_element.c \
esvg_generated_renderable.c \
esvg_generated_svg.c

%.c:	$(top_srcdir)/esvg/data/esvg_private.ender
	ender_generator $(top_srcdir)/esvg/data/esvg_private.ender $(basename $(subst esvg_generated_,,$@)) $@
