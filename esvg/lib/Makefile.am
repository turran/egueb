MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/ecss/include \
-I$(top_srcdir)/edom/include \
-I$(top_srcdir)/esvg/include \
-I$(top_builddir)/ecss/include \
-I$(top_builddir)/edom/include \
-I$(top_builddir)/esvg/include \
-DENESIM_EXTENSION \
@ESVG_BUILD@ \
@ESVG_CFLAGS@ \
@ESVG_VIDEO_CFLAGS@

lib_LTLIBRARIES = libesvg.la

libesvg_la_SOURCES = \
esvg_a.c \
esvg_attribute.c \
esvg_attribute_animation.c \
esvg_attribute_display.c \
esvg_attribute_enum.c \
esvg_attribute_presentation.c \
esvg_attribute_visibility.c \
esvg_animate.c \
esvg_animate_base.c \
esvg_animate_transform.c \
esvg_animation.c \
esvg_circle.c \
esvg_clip_path.c \
esvg_clone.c \
esvg_context.c \
esvg_defs.c \
esvg_element.c \
esvg_ellipse.c \
esvg_g.c \
esvg_gradient.c \
esvg_image.c \
esvg_input.c \
esvg_line.c \
esvg_linear_gradient.c \
esvg_main.c \
esvg_paint_server.c \
esvg_parser.c \
esvg_path.c \
esvg_polygon.c \
esvg_polyline.c \
esvg_radial_gradient.c \
esvg_referenceable.c \
esvg_renderable.c \
esvg_renderable_container.c \
esvg_rect.c \
esvg_set.c \
esvg_stop.c \
esvg_style.c \
esvg_svg.c \
esvg_text.c \
esvg_types.c \
esvg_use.c \
esvg_video.c

#esvg_attribute_regular.c \
#esvg_gradient.c \
#esvg_pattern.c \
#esvg_shape.c \
#esvg_use.c

libesvg_la_LIBADD = \
$(top_builddir)/edom/lib/libedom.la \
$(top_builddir)/ecss/lib/libecss.la \
@ESVG_LIBS@ @ESVG_VIDEO_LIBS@ -lm

libesvg_la_LDFLAGS = -no-undefined -version-info @version_info@
