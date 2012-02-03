MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = edom parser

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
-I$(top_srcdir)/src/lib/edom \
-I$(top_builddir)/src/include \
@EFL_ESVG_BUILD@ \
@ESVG_CFLAGS@

lib_LTLIBRARIES = libesvg.la

libesvg_la_SOURCES = \
esvg_attribute_presentation.c \
esvg_attribute_regular.c \
esvg_clip_path.c \
esvg_container.c \
esvg_element.c \
esvg_g.c \
esvg_main.c \
esvg_paint_server.c \
esvg_circle.c \
esvg_ellipse.c \
esvg_gradient.c \
esvg_image.c \
esvg_line.c \
esvg_linear_gradient.c \
esvg_path.c \
esvg_polygon.c \
esvg_polyline.c \
esvg_radial_gradient.c \
esvg_rect.c \
esvg_shape.c \
esvg_svg.c \
esvg_text.c \
esvg_types.c \
esvg_use.c \
esvg_values.c

libesvg_la_LIBADD = \
edom/libedom.la \
parser/libesvg_parser.la \
@ESVG_LIBS@ -lm

libesvg_la_LDFLAGS = -no-undefined -version-info @version_info@
