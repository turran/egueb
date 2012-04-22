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
@ESVG_CFLAGS@

lib_LTLIBRARIES = libesvg.la

libesvg_la_SOURCES = \
esvg_attribute_presentation.c \
esvg_attribute_animation.c \
esvg_animation.c \
esvg_circle.c \
esvg_element.c \
esvg_ellipse.c \
esvg_g.c \
esvg_gradient.c \
esvg_instantiable.c \
esvg_line.c \
esvg_linear_gradient.c \
esvg_main.c \
esvg_paint_server.c \
esvg_parser.c \
esvg_path.c \
esvg_polygon.c \
esvg_polyline.c \
esvg_radial_gradient.c \
esvg_renderable.c \
esvg_rect.c \
esvg_stop.c \
esvg_svg.c \
esvg_types.c

#esvg_attribute_regular.c \
#esvg_clip_path.c \
#esvg_gradient.c \
#esvg_image.c \
#esvg_linear_gradient.c \
#esvg_pattern.c \
#esvg_radial_gradient.c \
#esvg_shape.c \
#esvg_text.c \
#esvg_use.c

libesvg_la_LIBADD = \
$(top_builddir)/edom/lib/libedom.la \
$(top_builddir)/ecss/lib/libecss.la \
@ESVG_LIBS@ -lm

libesvg_la_LDFLAGS = -no-undefined -version-info @version_info@
