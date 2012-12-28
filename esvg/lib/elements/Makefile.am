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

noinst_LTLIBRARIES = libesvg_elements.la

libesvg_elements_la_SOURCES = \
esvg_a.c \
esvg_animate.c \
esvg_animate_base.c \
esvg_animate_transform.c \
esvg_animation.c \
esvg_circle.c \
esvg_clip_path.c \
esvg_defs.c \
esvg_element.c \
esvg_element_rect.c \
esvg_ellipse.c \
esvg_g.c \
esvg_gradient.c \
esvg_image.c \
esvg_line.c \
esvg_linear_gradient.c \
esvg_paint_server.c \
esvg_path.c \
esvg_polygon.c \
esvg_polyline.c \
esvg_radial_gradient.c \
esvg_referenceable.c \
esvg_renderable.c \
esvg_script.c \
esvg_set.c \
esvg_stop.c \
esvg_style.c \
esvg_svg.c \
esvg_text.c \
esvg_use.c \
esvg_video.c

libesvg_elements_la_LIBADD =
