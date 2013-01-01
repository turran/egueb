MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = elements types script_providers video_providers

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/ecss/include \
-I$(top_srcdir)/edom/include \
-I$(top_srcdir)/esvg/include \
-DENESIM_EXTENSION \
-DESVG_BUILD \
@ESVG_CFLAGS@

lib_LTLIBRARIES = libesvg.la

libesvg_la_SOURCES = \
esvg_attribute.c \
esvg_attribute_animation.c \
esvg_attribute_display.c \
esvg_attribute_clip_path.c \
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
esvg_video_provider.c

#esvg_attribute_regular.c \
#esvg_element_gradient.c \
#esvg_pattern.c \
#esvg_shape.c \
#esvg_element_use.c

libesvg_la_LIBADD = \
$(top_builddir)/edom/lib/libedom.la \
$(top_builddir)/ecss/lib/libecss.la \
$(top_builddir)/esvg/lib/elements/libesvg_elements.la \
$(top_builddir)/esvg/lib/types/libesvg_types.la \
$(top_builddir)/esvg/lib/video_providers/libesvg_video_providers.la \
$(top_builddir)/esvg/lib/script_providers/libesvg_script_providers.la \
@ESVG_LIBS@ @GSTREAMER_LIBS@ -lm

libesvg_la_LDFLAGS = -no-undefined -version-info @version_info@
