
lib_LTLIBRARIES += src/lib/svg/libesvg.la

installed_svgheadersdir = $(pkgincludedir)-@VMAJ@/svg
dist_installed_svgheaders_DATA = \
src/lib/svg/Esvg.h \
src/lib/svg/esvg_animate_base.h \
src/lib/svg/esvg_event.h \
src/lib/svg/esvg_main.h \
src/lib/svg/esvg_parser.h \
src/lib/svg/esvg_script_provider.h \
src/lib/svg/esvg_types.h \
src/lib/svg/esvg_video_provider.h \
src/lib/svg/esvg_paint_server.h \
src/lib/svg/esvg_referenceable.h \
src/lib/svg/esvg_renderable.h

GENERATED_FILES += \
src/lib/svg/elements/esvg_generated_animate_base.c \
src/lib/svg/elements/esvg_generated_paint_server.c \
src/lib/svg/elements/esvg_generated_referenceable.c \
src/lib/svg/elements/esvg_generated_renderable.c

src_lib_svg_libesvg_la_SOURCES = \
src/lib/svg/esvg_attribute.c \
src/lib/svg/esvg_attribute_private.h \
src/lib/svg/esvg_attribute_clip_path.c \
src/lib/svg/esvg_attribute_clip_path_private.h \
src/lib/svg/esvg_attribute_color.c \
src/lib/svg/esvg_attribute_color_private.h \
src/lib/svg/esvg_attribute_display.c \
src/lib/svg/esvg_attribute_display_private.h \
src/lib/svg/esvg_attribute_enum.c \
src/lib/svg/esvg_attribute_enum_private.h \
src/lib/svg/esvg_attribute_length.c \
src/lib/svg/esvg_attribute_length_private.h \
src/lib/svg/esvg_attribute_number.c \
src/lib/svg/esvg_attribute_number_private.h \
src/lib/svg/esvg_attribute_paint.c \
src/lib/svg/esvg_attribute_presentation.c \
src/lib/svg/esvg_attribute_string.c \
src/lib/svg/esvg_attribute_string_private.h \
src/lib/svg/esvg_attribute_visibility.c \
src/lib/svg/esvg_attribute_visibility_private.h \
src/lib/svg/esvg_animate_base.c \
src/lib/svg/esvg_animate_base.h \
src/lib/svg/esvg_animate_base_private.h \
src/lib/svg/esvg_clone.c \
src/lib/svg/esvg_clone_private.h \
src/lib/svg/esvg_context.c \
src/lib/svg/esvg_context_private.h \
src/lib/svg/esvg_eina.c \
src/lib/svg/esvg_input.c \
src/lib/svg/esvg_input_private.h \
src/lib/svg/esvg_main.c \
src/lib/svg/esvg_main_private.h \
src/lib/svg/esvg_paint_server.h \
src/lib/svg/esvg_paint_server.c \
src/lib/svg/esvg_parser.c \
src/lib/svg/esvg_parser_private.h \
src/lib/svg/esvg_paint_server_private.h \
src/lib/svg/esvg_referenceable.c \
src/lib/svg/esvg_referenceable.h \
src/lib/svg/esvg_referenceable_private.h \
src/lib/svg/esvg_renderable.c \
src/lib/svg/esvg_renderable.h \
src/lib/svg/esvg_renderable_private.h \
src/lib/svg/esvg_renderable_behaviour_private.h \
src/lib/svg/esvg_renderable_container.c \
src/lib/svg/esvg_renderable_container_private.h \
src/lib/svg/esvg_script_private.h \
src/lib/svg/esvg_script_provider.c \
src/lib/svg/esvg_script_provider_private.h \
src/lib/svg/esvg_types.c \
src/lib/svg/esvg_types_private.h \
src/lib/svg/esvg_video_provider.c \
src/lib/svg/esvg_video_provider_private.h \
src/lib/svg/esvg_private_attribute_animation.h \
src/lib/svg/esvg_private_attribute_presentation.h

#esvg_attribute_regular.c \
#esvg_element_gradient.c \
#esvg_pattern.c \
#esvg_shape.c \
#esvg_element_use.c

src_lib_svg_libesvg_la_CPPFLAGS = \
-I$(top_srcdir)/src/lib/css \
-I$(top_srcdir)/src/lib/dom \
-I$(top_srcdir)/src/lib/svg \
-I$(top_srcdir)/src/lib/svg/types \
-I$(top_srcdir)/src/lib/svg/elements \
-I$(top_builddir)/src/lib/svg/types \
-I$(top_builddir)/src/lib/svg/elements \
-DENESIM_EXTENSION \
-DESVG_BUILD \
@ESVG_CFLAGS@

src_lib_svg_libesvg_la_LIBADD = \
src/lib/dom/libedom.la \
src/lib/css/libecss.la \
@ESVG_LIBS@ @GSTREAMER_LIBS@ -lm

src_lib_svg_libesvg_la_LDFLAGS = -no-undefined -version-info @version_info@
