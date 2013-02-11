MAINTAINERCLEANFILES = Makefile.in

GENERATED_FLAGS = -x

GENERATED_ELEMENTS = \
esvg_generated_element.c \
esvg_generated_element_a.c \
esvg_generated_element_animate.c \
esvg_generated_animate_base.c \
esvg_generated_element_animate_transform.c \
esvg_generated_element_animation.c \
esvg_generated_element_circle.c \
esvg_generated_element_clip_path.c \
esvg_generated_element_defs.c \
esvg_generated_element_element_rect.c \
esvg_generated_element_ellipse.c \
esvg_generated_element_g.c \
esvg_generated_element_gradient.c \
esvg_generated_element_image.c \
esvg_generated_element_polygon.c \
esvg_generated_element_polyline.c \
esvg_generated_element_line.c \
esvg_generated_element_linear_gradient.c \
esvg_generated_element_path.c \
esvg_generated_element_rect.c \
esvg_generated_element_radial_gradient.c \
esvg_generated_element_script.c \
esvg_generated_element_set.c \
esvg_generated_element_stop.c \
esvg_generated_element_style.c \
esvg_generated_element_svg.c \
esvg_generated_element_text.c \
esvg_generated_element_use.c \
esvg_generated_element_video.c \
esvg_generated_paint_server.c \
esvg_generated_referenceable.c \
esvg_generated_renderable.c

GENERATED_TYPES = \
esvg_generated_boolean_animated.c \
esvg_generated_color.c \
esvg_generated_color_animated.c \
esvg_generated_duration.c \
esvg_generated_length.c \
esvg_generated_length_animated.c \
esvg_generated_matrix.c \
esvg_generated_matrix_animated.c \
esvg_generated_number.c \
esvg_generated_number_animated.c \
esvg_generated_paint.c \
esvg_generated_paint_animated.c \
esvg_generated_point.c \
esvg_generated_rect.c \
esvg_generated_rect_animated.c \
esvg_generated_string_animated.c

GENERATED_FILES = $(GENERATED_ELEMENTS) $(GENERATED_TYPES)

esvg_generated_matrix_GFLAGS = -o
esvg_generated_rect_GFLAGS = -o

CLEANFILES = $(GENERATED_FILES)

BUILT_SOURCES = $(GENERATED_FILES)

%.c:	$(top_srcdir)/esvg/data/esvg_private.ender
	ender_generator $(GENERATED_FLAGS) $(value $(addsuffix _GFLAGS,$(basename $@))) $(top_srcdir)/esvg/data/esvg_private.ender $(basename $(subst esvg_generated_,,$@)) $@
