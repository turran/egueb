
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
src/lib/svg/esvg_attribute_clip_path.c \
src/lib/svg/esvg_attribute_display.c \
src/lib/svg/esvg_attribute_enum.c \
src/lib/svg/esvg_attribute_length.c \
src/lib/svg/esvg_attribute_paint.c \
src/lib/svg/esvg_attribute_presentation.c \
src/lib/svg/esvg_attribute_visibility.c \
src/lib/svg/esvg_animate_base.c \
src/lib/svg/esvg_animate_base.h \
src/lib/svg/esvg_paint_server.h \
src/lib/svg/esvg_paint_server.c \
src/lib/svg/esvg_referenceable.c \
src/lib/svg/esvg_referenceable.h \
src/lib/svg/esvg_renderable.c \
src/lib/svg/esvg_renderable.h \
src/lib/svg/esvg_clone.c \
src/lib/svg/esvg_context.c \
src/lib/svg/esvg_eina.c \
src/lib/svg/esvg_input.c \
src/lib/svg/esvg_main.c \
src/lib/svg/esvg_parser.c \
src/lib/svg/esvg_renderable_container.c \
src/lib/svg/esvg_script_provider.c \
src/lib/svg/esvg_types.c \
src/lib/svg/esvg_video_provider.c \
src/lib/svg/esvg_private_attribute.h \
src/lib/svg/esvg_private_attribute_animation.h \
src/lib/svg/esvg_private_attribute_clip_path.h \
src/lib/svg/esvg_private_attribute_display.h \
src/lib/svg/esvg_private_attribute_enum.h \
src/lib/svg/esvg_private_attribute_presentation.h \
src/lib/svg/esvg_private_attribute_visibility.h \
src/lib/svg/esvg_private_clone.h \
src/lib/svg/esvg_private_context.h \
src/lib/svg/esvg_private_input.h \
src/lib/svg/esvg_private_main.h \
src/lib/svg/esvg_private_parser.h \
src/lib/svg/esvg_private_referenceable.h \
src/lib/svg/esvg_private_renderable_container.h \
src/lib/svg/esvg_private_script_provider.h \
src/lib/svg/esvg_private_script.h \
src/lib/svg/esvg_private_types.h \
src/lib/svg/esvg_private_renderable_behaviour.h \
src/lib/svg/esvg_private_renderable.h \
src/lib/svg/esvg_private_video_provider.h

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
