MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = generated elements types script_providers video_providers

lib_LTLIBRARIES = libesvg.la

installed_apiheadersdir = $(pkgincludedir)-@VMAJ@
dist_installed_apiheaders_DATA = \
Esvg.h \
elements/esvg_animate_base.h \
elements/esvg_element.h \
elements/esvg_element_a.h \
elements/esvg_element_animate.h \
elements/esvg_element_animate_transform.h \
elements/esvg_element_animation.h \
elements/esvg_element_circle.h \
elements/esvg_element_clip_path.h \
elements/esvg_element_defs.h \
elements/esvg_element_ellipse.h \
elements/esvg_element_g.h \
elements/esvg_element_gradient.h \
elements/esvg_element_image.h \
elements/esvg_element_line.h \
elements/esvg_element_linear_gradient.h \
elements/esvg_element_path.h \
elements/esvg_element_polygon.h \
elements/esvg_element_polyline.h \
elements/esvg_element_radial_gradient.h \
elements/esvg_element_rect.h \
elements/esvg_element_script.h \
elements/esvg_element_set.h \
elements/esvg_element_stop.h \
elements/esvg_element_style.h \
elements/esvg_element_svg.h \
elements/esvg_element_text.h \
elements/esvg_element_use.h \
elements/esvg_element_video.h \
elements/esvg_paint_server.h \
elements/esvg_referenceable.h \
elements/esvg_renderable.h \
esvg_event.h \
esvg_main.h \
esvg_parser.h \
esvg_script_provider.h \
esvg_types.h \
esvg_video_provider.h \
types/esvg_boolean.h \
types/esvg_color.h \
types/esvg_length.h \
types/esvg_matrix.h \
types/esvg_number.h \
types/esvg_paint.h \
types/esvg_rect.h \
types/esvg_string.h

libesvg_la_SOURCES = \
esvg_attribute.c \
esvg_attribute_animation.c \
esvg_attribute_clip_path.c \
esvg_attribute_display.c \
esvg_attribute_enum.c \
esvg_attribute_length.c \
esvg_attribute_paint.c \
esvg_attribute_presentation.c \
esvg_attribute_visibility.c \
esvg_clone.c \
esvg_context.c \
esvg_eina.c \
esvg_input.c \
esvg_main.c \
esvg_parser.c \
esvg_renderable_container.c \
esvg_script_provider.c \
esvg_types.c \
esvg_video_provider.c \
esvg_private_attribute.h \
esvg_private_attribute_animation.h \
esvg_private_attribute_clip_path.h \
esvg_private_attribute_display.h \
esvg_private_attribute_enum.h \
esvg_private_attribute_presentation.h \
esvg_private_attribute_visibility.h \
esvg_private_clone.h \
esvg_private_context.h \
esvg_private_input.h \
esvg_private_main.h \
esvg_private_parser.h \
esvg_private_referenceable.h \
esvg_private_renderable_container.h \
esvg_private_script_provider.h \
esvg_private_script.h \
esvg_private_types.h \
esvg_private_video_provider.h

#esvg_attribute_regular.c \
#esvg_element_gradient.c \
#esvg_pattern.c \
#esvg_shape.c \
#esvg_element_use.c

libesvg_la_CPPFLAGS = \
-I$(top_srcdir)/ecss/lib \
-I$(top_srcdir)/edom/lib \
-I$(top_srcdir)/esvg/lib/types \
-I$(top_srcdir)/esvg/lib/elements \
-DENESIM_EXTENSION \
-DESVG_BUILD \
@ESVG_CFLAGS@

libesvg_la_LIBADD = \
$(top_builddir)/edom/lib/libedom.la \
$(top_builddir)/ecss/lib/libecss.la \
$(top_builddir)/esvg/lib/elements/libesvg_elements.la \
$(top_builddir)/esvg/lib/types/libesvg_types.la \
$(top_builddir)/esvg/lib/video_providers/libesvg_video_providers.la \
$(top_builddir)/esvg/lib/script_providers/libesvg_script_providers.la \
@ESVG_LIBS@ @GSTREAMER_LIBS@ -lm

libesvg_la_LDFLAGS = -no-undefined -version-info @version_info@
