MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/ecss/include \
-I$(top_srcdir)/edom/include \
-I$(top_srcdir)/esvg/include \
-I$(top_builddir)/esvg/include \
-DENESIM_EXTENSION \
-DESVG_BUILD \
@ESVG_CFLAGS@

noinst_LTLIBRARIES = libesvg_elements.la

libesvg_elements_la_SOURCES = \
esvg_element.c \
esvg_element_a.c \
esvg_element_animate.c \
esvg_animate_base.c \
esvg_element_animate_transform.c \
esvg_element_animation.c \
esvg_element_circle.c \
esvg_element_clip_path.c \
esvg_element_defs.c \
esvg_element_ellipse.c \
esvg_element_g.c \
esvg_element_gradient.c \
esvg_element_image.c \
esvg_element_line.c \
esvg_element_linear_gradient.c \
esvg_element_path.c \
esvg_element_polygon.c \
esvg_element_polyline.c \
esvg_element_radial_gradient.c \
esvg_element_rect.c \
esvg_element_script.c \
esvg_element_set.c \
esvg_element_stop.c \
esvg_element_style.c \
esvg_element_svg.c \
esvg_element_text.c \
esvg_element_use.c \
esvg_element_video.c \
esvg_paint_server.c \
esvg_referenceable.c \
esvg_renderable.c

libesvg_elements_la_LIBADD =
