MAINTAINERCLEANFILES = Makefile.in config.h.in~ config.h.in

GENERATED_FILES = \
esvg_generated_animation.c \
esvg_generated_circle.c \
esvg_generated_element.c \
esvg_generated_ellipse.c \
esvg_generated_g.c \
esvg_generated_polygon.c \
esvg_generated_polyline.c \
esvg_generated_line.c \
esvg_generated_path.c \
esvg_generated_renderable.c \
esvg_generated_rect.c \
esvg_generated_svg.c

CLEANFILES = $(GENERATED_FILES)

BUILT_SOURCES = $(GENERATED_FILES)

%.c:	$(top_srcdir)/esvg/data/esvg_private.ender
	ender_generator $(top_srcdir)/esvg/data/esvg_private.ender $(basename $(subst esvg_generated_,,$@)) $@
