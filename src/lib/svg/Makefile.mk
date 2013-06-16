
lib_LTLIBRARIES += src/lib/svg/libegueb_svg.la

installed_svgheadersdir = $(pkgincludedir)-@VMAJ@/svg
dist_installed_svgheaders_DATA = \
src/lib/svg/Egueb_Svg.h \
src/lib/svg/egueb_svg_attr_clip_path.h \
src/lib/svg/egueb_svg_attr_color.h \
src/lib/svg/egueb_svg_attr_font_size.h \
src/lib/svg/egueb_svg_attr_length.h \
src/lib/svg/egueb_svg_attr_matrix.h \
src/lib/svg/egueb_svg_attr_number.h \
src/lib/svg/egueb_svg_attr_paint.h \
src/lib/svg/egueb_svg_attr_point_list.h \
src/lib/svg/egueb_svg_attr_rect.h \
src/lib/svg/egueb_svg_attr_referenceable_units.h \
src/lib/svg/egueb_svg_attr_string.h \
src/lib/svg/egueb_svg_attr_stroke_line_cap.h \
src/lib/svg/egueb_svg_attr_stroke_line_join.h \
src/lib/svg/egueb_svg_attr_visibility.h \
src/lib/svg/egueb_svg_clip_path.h \
src/lib/svg/egueb_svg_color.h \
src/lib/svg/egueb_svg_display.h \
src/lib/svg/egueb_svg_document.h \
src/lib/svg/egueb_svg_element.h \
src/lib/svg/egueb_svg_element_circle.h \
src/lib/svg/egueb_svg_element_clip_path.h \
src/lib/svg/egueb_svg_element_desc.h \
src/lib/svg/egueb_svg_element_defs.h \
src/lib/svg/egueb_svg_element_ellipse.h \
src/lib/svg/egueb_svg_element_g.h \
src/lib/svg/egueb_svg_element_image.h \
src/lib/svg/egueb_svg_element_line.h \
src/lib/svg/egueb_svg_element_linear_gradient.h \
src/lib/svg/egueb_svg_element_polygon.h \
src/lib/svg/egueb_svg_element_polyline.h \
src/lib/svg/egueb_svg_element_radial_gradient.h \
src/lib/svg/egueb_svg_element_rect.h \
src/lib/svg/egueb_svg_element_use.h \
src/lib/svg/egueb_svg_element_stop.h \
src/lib/svg/egueb_svg_element_svg.h \
src/lib/svg/egueb_svg_element_text.h \
src/lib/svg/egueb_svg_element_tspan.h \
src/lib/svg/egueb_svg_fill_rule.h \
src/lib/svg/egueb_svg_font_size.h \
src/lib/svg/egueb_svg_gradient.h \
src/lib/svg/egueb_svg_length.h \
src/lib/svg/egueb_svg_shape.h \
src/lib/svg/egueb_svg_string.h \
src/lib/svg/egueb_svg_main.h \
src/lib/svg/egueb_svg_matrix.h \
src/lib/svg/egueb_svg_number.h \
src/lib/svg/egueb_svg_paint.h \
src/lib/svg/egueb_svg_paint_server.h \
src/lib/svg/egueb_svg_painter.h \
src/lib/svg/egueb_svg_point.h \
src/lib/svg/egueb_svg_point_list.h \
src/lib/svg/egueb_svg_rect.h \
src/lib/svg/egueb_svg_reference.h \
src/lib/svg/egueb_svg_referenceable.h \
src/lib/svg/egueb_svg_referenceable_units.h \
src/lib/svg/egueb_svg_renderable.h \
src/lib/svg/egueb_svg_renderable_container.h \
src/lib/svg/egueb_svg_stroke_line_cap.h \
src/lib/svg/egueb_svg_stroke_line_join.h \
src/lib/svg/egueb_svg_visibility.h

src_lib_svg_libegueb_svg_la_SOURCES = \
src/lib/svg/egueb_svg_attr_clip_path.c \
src/lib/svg/egueb_svg_attr_color.c \
src/lib/svg/egueb_svg_attr_font_size.c \
src/lib/svg/egueb_svg_attr_length.c \
src/lib/svg/egueb_svg_attr_matrix.c \
src/lib/svg/egueb_svg_attr_number.c \
src/lib/svg/egueb_svg_attr_paint.c \
src/lib/svg/egueb_svg_attr_point_list.c \
src/lib/svg/egueb_svg_attr_rect.c \
src/lib/svg/egueb_svg_attr_referenceable_units.c \
src/lib/svg/egueb_svg_attr_string.c \
src/lib/svg/egueb_svg_attr_stroke_line_cap.c \
src/lib/svg/egueb_svg_attr_stroke_line_join.c \
src/lib/svg/egueb_svg_attr_visibility.c \
src/lib/svg/egueb_svg_clip_path.c \
src/lib/svg/egueb_svg_color.c \
src/lib/svg/egueb_svg_display.c \
src/lib/svg/egueb_svg_document.c \
src/lib/svg/egueb_svg_element.c \
src/lib/svg/egueb_svg_element_circle.c \
src/lib/svg/egueb_svg_element_clip_path.c \
src/lib/svg/egueb_svg_element_desc.c \
src/lib/svg/egueb_svg_element_defs.c \
src/lib/svg/egueb_svg_element_ellipse.c \
src/lib/svg/egueb_svg_element_g.c \
src/lib/svg/egueb_svg_element_image.c \
src/lib/svg/egueb_svg_element_line.c \
src/lib/svg/egueb_svg_element_linear_gradient.c \
src/lib/svg/egueb_svg_element_polygon.c \
src/lib/svg/egueb_svg_element_polyline.c \
src/lib/svg/egueb_svg_element_radial_gradient.c \
src/lib/svg/egueb_svg_element_rect.c \
src/lib/svg/egueb_svg_element_use.c \
src/lib/svg/egueb_svg_element_stop.c \
src/lib/svg/egueb_svg_element_svg.c \
src/lib/svg/egueb_svg_element_svg_private.h \
src/lib/svg/egueb_svg_element_text.c \
src/lib/svg/egueb_svg_element_text_private.h \
src/lib/svg/egueb_svg_element_tspan.c \
src/lib/svg/egueb_svg_fill_rule.c \
src/lib/svg/egueb_svg_font_size.c \
src/lib/svg/egueb_svg_gradient.c \
src/lib/svg/egueb_svg_gradient_private.h \
src/lib/svg/egueb_svg_input.c \
src/lib/svg/egueb_svg_input_private.h \
src/lib/svg/egueb_svg_length.c \
src/lib/svg/egueb_svg_main.c \
src/lib/svg/egueb_svg_main_private.h \
src/lib/svg/egueb_svg_matrix.c \
src/lib/svg/egueb_svg_number.c \
src/lib/svg/egueb_svg_paint.c \
src/lib/svg/egueb_svg_paint_server.c \
src/lib/svg/egueb_svg_paint_server_private.h \
src/lib/svg/egueb_svg_painter.c \
src/lib/svg/egueb_svg_painter_private.h \
src/lib/svg/egueb_svg_painter_clip_path.c \
src/lib/svg/egueb_svg_painter_generic.c \
src/lib/svg/egueb_svg_point.c \
src/lib/svg/egueb_svg_point_list.c \
src/lib/svg/egueb_svg_rect.c \
src/lib/svg/egueb_svg_reference.c \
src/lib/svg/egueb_svg_reference_private.h \
src/lib/svg/egueb_svg_reference_gradient.c \
src/lib/svg/egueb_svg_reference_gradient_private.h \
src/lib/svg/egueb_svg_reference_gradient_linear.c \
src/lib/svg/egueb_svg_reference_gradient_radial.c \
src/lib/svg/egueb_svg_reference_clip_path.c \
src/lib/svg/egueb_svg_reference_paint_server.c \
src/lib/svg/egueb_svg_reference_paint_server_private.h \
src/lib/svg/egueb_svg_referenceable.c \
src/lib/svg/egueb_svg_referenceable_private.h \
src/lib/svg/egueb_svg_referenceable_units.c \
src/lib/svg/egueb_svg_renderable.c \
src/lib/svg/egueb_svg_renderable_private.h \
src/lib/svg/egueb_svg_renderable_container.c \
src/lib/svg/egueb_svg_renderable_container_private.h \
src/lib/svg/egueb_svg_shape.c \
src/lib/svg/egueb_svg_shape_private.h \
src/lib/svg/egueb_svg_string.c \
src/lib/svg/egueb_svg_string_private.h \
src/lib/svg/egueb_svg_stroke_line_cap.c \
src/lib/svg/egueb_svg_stroke_line_join.c \
src/lib/svg/egueb_svg_visibility.c

src_lib_svg_libegueb_svg_la_CPPFLAGS = \
-I$(top_srcdir)/src/lib/css \
-I$(top_srcdir)/src/lib/dom \
-I$(top_srcdir)/src/lib/smil \
-I$(top_srcdir)/src/lib/svg \
-I$(top_srcdir)/src/lib/svg/types \
-I$(top_srcdir)/src/lib/svg/elements \
-I$(top_builddir)/src/lib/svg/types \
-I$(top_builddir)/src/lib/svg/elements \
-DENESIM_EXTENSION \
-DESVG_BUILD \
@ESVG_CFLAGS@

src_lib_svg_libegueb_svg_la_LIBADD = \
$(top_builddir)/src/lib/dom/libegueb_dom.la \
$(top_builddir)/src/lib/css/libegueb_css.la \
$(top_builddir)/src/lib/smil/libegueb_smil.la \
@ESVG_LIBS@ @GSTREAMER_LIBS@ -lm

src_lib_svg_libegueb_svg_la_LDFLAGS = -no-undefined -version-info @version_info@
