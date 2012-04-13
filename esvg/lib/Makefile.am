MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = parser

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
esvg_element.c \
esvg_parser.c \
esvg_renderable.c \
esvg_main.c \
esvg_svg.c \
esvg_types.c

#esvg_attribute_regular.c \
#esvg_clip_path.c \
#esvg_g.c \
#esvg_paint_server.c \
#esvg_circle.c \
#esvg_ellipse.c \
#esvg_gradient.c \
#esvg_image.c \
#esvg_line.c \
#esvg_linear_gradient.c \
#esvg_path.c \
#esvg_pattern.c \
#esvg_polygon.c \
#esvg_polyline.c \
#esvg_radial_gradient.c \
#esvg_rect.c \
#esvg_shape.c \
#esvg_text.c \
#esvg_use.c

libesvg_la_LIBADD = \
parser/libesvg_parser.la \
$(top_builddir)/edom/lib/libedom.la \
$(top_builddir)/ecss/lib/libecss.la \
@ESVG_LIBS@ -lm

libesvg_la_LDFLAGS = -no-undefined -version-info @version_info@
