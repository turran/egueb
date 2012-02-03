MAINTAINERCLEANFILES = \
Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
-I$(top_srcdir)/src/lib/edom \
-I$(top_builddir)/src/include \
@EFL_ESVG_BUILD@ \
@ESVG_CFLAGS@

noinst_LTLIBRARIES = libesvg_parser.la

libesvg_parser_la_SOURCES = \
esvg_parser.c \
esvg_parser_circle.c \
esvg_parser_clip_path.c \
esvg_parser_context.c \
esvg_parser_context_g.c \
esvg_parser_context_clip_path.c \
esvg_parser_context_defs.c \
esvg_parser_context_doc.c \
esvg_parser_context_gradient.c \
esvg_parser_context_style.c \
esvg_parser_context_svg.c \
esvg_parser_context_text.c \
esvg_parser_element.c \
esvg_parser_ellipse.c \
esvg_parser_g.c \
esvg_parser_gradient.c \
esvg_parser_image.c \
esvg_parser_line.c \
esvg_parser_linear_gradient.c \
esvg_parser_paint_server.c \
esvg_parser_path.c \
esvg_parser_pattern.c \
esvg_parser_polygon.c \
esvg_parser_polyline.c \
esvg_parser_radial_gradient.c \
esvg_parser_rect.c \
esvg_parser_shape.c \
esvg_parser_stop.c \
esvg_parser_style.c \
esvg_parser_svg.c \
esvg_parser_text.c \
esvg_parser_types.c \
esvg_parser_use.c

libesvg_parser_la_LIBADD =
