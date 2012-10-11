MAINTAINERCLEANFILES = Makefile.in config.h.in~ config.h.in

GENERATED_FILES = \
esvg_generated_a.c \
esvg_generated_animate.c \
esvg_generated_animate_base.c \
esvg_generated_animate_transform.c \
esvg_generated_animation.c \
esvg_generated_circle.c \
esvg_generated_clip_path.c \
esvg_generated_defs.c \
esvg_generated_element.c \
esvg_generated_ellipse.c \
esvg_generated_g.c \
esvg_generated_gradient.c \
esvg_generated_image.c \
esvg_generated_paint_server.c \
esvg_generated_polygon.c \
esvg_generated_polyline.c \
esvg_generated_line.c \
esvg_generated_linear_gradient.c \
esvg_generated_path.c \
esvg_generated_radial_gradient.c \
esvg_generated_referenceable.c \
esvg_generated_renderable.c \
esvg_generated_rect.c \
esvg_generated_set.c \
esvg_generated_stop.c \
esvg_generated_style.c \
esvg_generated_svg.c \
esvg_generated_text.c \
esvg_generated_types.c \
esvg_generated_use.c

CLEANFILES = $(GENERATED_FILES)

BUILT_SOURCES = $(GENERATED_FILES)

%.c:	$(top_srcdir)/esvg/data/esvg_private.ender
	ender_generator x $(top_srcdir)/esvg/data/esvg_private.ender $(basename $(subst esvg_generated_,,$@)) $@
