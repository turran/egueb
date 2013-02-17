GENERATED_FLAGS = -x

GENERATED_ELEMENTS = \
src/lib/svg/generated/esvg_generated_element.c \
src/lib/svg/generated/esvg_generated_element_a.c \
src/lib/svg/generated/esvg_generated_element_animate.c \
src/lib/svg/generated/esvg_generated_animate_base.c \
src/lib/svg/generated/esvg_generated_element_animate_transform.c \
src/lib/svg/generated/esvg_generated_element_animation.c \
src/lib/svg/generated/esvg_generated_element_circle.c \
src/lib/svg/generated/esvg_generated_element_clip_path.c \
src/lib/svg/generated/esvg_generated_element_defs.c \
src/lib/svg/generated/esvg_generated_element_element_rect.c \
src/lib/svg/generated/esvg_generated_element_ellipse.c \
src/lib/svg/generated/esvg_generated_element_g.c \
src/lib/svg/generated/esvg_generated_element_gradient.c \
src/lib/svg/generated/esvg_generated_element_image.c \
src/lib/svg/generated/esvg_generated_element_polygon.c \
src/lib/svg/generated/esvg_generated_element_polyline.c \
src/lib/svg/generated/esvg_generated_element_line.c \
src/lib/svg/generated/esvg_generated_element_linear_gradient.c \
src/lib/svg/generated/esvg_generated_element_path.c \
src/lib/svg/generated/esvg_generated_element_rect.c \
src/lib/svg/generated/esvg_generated_element_radial_gradient.c \
src/lib/svg/generated/esvg_generated_element_script.c \
src/lib/svg/generated/esvg_generated_element_set.c \
src/lib/svg/generated/esvg_generated_element_stop.c \
src/lib/svg/generated/esvg_generated_element_style.c \
src/lib/svg/generated/esvg_generated_element_svg.c \
src/lib/svg/generated/esvg_generated_element_text.c \
src/lib/svg/generated/esvg_generated_element_use.c \
src/lib/svg/generated/esvg_generated_element_video.c \
src/lib/svg/generated/esvg_generated_paint_server.c \
src/lib/svg/generated/esvg_generated_referenceable.c \
src/lib/svg/generated/esvg_generated_renderable.c

GENERATED_TYPES = \
src/lib/svg/generated/esvg_generated_boolean_animated.c \
src/lib/svg/generated/esvg_generated_color.c \
src/lib/svg/generated/esvg_generated_color_animated.c \
src/lib/svg/generated/esvg_generated_duration.c \
src/lib/svg/generated/esvg_generated_length.c \
src/lib/svg/generated/esvg_generated_length_animated.c \
src/lib/svg/generated/esvg_generated_matrix.c \
src/lib/svg/generated/esvg_generated_matrix_animated.c \
src/lib/svg/generated/esvg_generated_number.c \
src/lib/svg/generated/esvg_generated_number_animated.c \
src/lib/svg/generated/esvg_generated_paint.c \
src/lib/svg/generated/esvg_generated_paint_animated.c \
src/lib/svg/generated/esvg_generated_point.c \
src/lib/svg/generated/esvg_generated_rect.c \
src/lib/svg/generated/esvg_generated_rect_animated.c \
src/lib/svg/generated/esvg_generated_string_animated.c

GENERATED_FILES = $(GENERATED_ELEMENTS) $(GENERATED_TYPES)

src_lib_svg_generated_esvg_generated_matrix_GFLAGS = -o
src_lib_svg_generated_esvg_generated_rect_GFLAGS = -o

CLEANFILES += $(GENERATED_FILES)

BUILT_SOURCES = $(GENERATED_FILES)

%.c: $(top_srcdir)/data/svg/esvg_private.ender
	ender_generator $(GENERATED_FLAGS) $(value $(addsuffix _GFLAGS,$(basename $@))) $(top_srcdir)/data/svg/esvg_private.ender $(basename $(subst esvg_generated_,,$@)) $@
