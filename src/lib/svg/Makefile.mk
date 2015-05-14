
lib_LTLIBRARIES += src/lib/svg/libegueb_svg.la

installed_svgheadersdir = $(pkgincludedir)-@VMAJ@/svg
dist_installed_svgheaders_DATA = \
src/lib/svg/Egueb_Svg.h \
src/lib/svg/egueb_svg_animate_transform_type.h \
src/lib/svg/egueb_svg_build.h \
src/lib/svg/egueb_svg_clip_path.h \
src/lib/svg/egueb_svg_color.h \
src/lib/svg/egueb_svg_display.h \
src/lib/svg/egueb_svg_document.h \
src/lib/svg/egueb_svg_element.h \
src/lib/svg/egueb_svg_enum.h \
src/lib/svg/egueb_svg_fill_rule.h \
src/lib/svg/egueb_svg_gradient.h \
src/lib/svg/egueb_svg_length.h \
src/lib/svg/egueb_svg_shape.h \
src/lib/svg/egueb_svg_string.h \
src/lib/svg/egueb_svg_main.h \
src/lib/svg/egueb_svg_matrix.h \
src/lib/svg/egueb_svg_number.h \
src/lib/svg/egueb_svg_overflow.h \
src/lib/svg/egueb_svg_paint.h \
src/lib/svg/egueb_svg_paint_server.h \
src/lib/svg/egueb_svg_painter.h \
src/lib/svg/egueb_svg_path_seg.h \
src/lib/svg/egueb_svg_point.h \
src/lib/svg/egueb_svg_pointer_events.h \
src/lib/svg/egueb_svg_rect.h \
src/lib/svg/egueb_svg_reference.h \
src/lib/svg/egueb_svg_referenceable.h \
src/lib/svg/egueb_svg_referenceable_units.h \
src/lib/svg/egueb_svg_renderable.h \
src/lib/svg/egueb_svg_renderable_container.h \
src/lib/svg/egueb_svg_spread_method.h \
src/lib/svg/egueb_svg_stroke_dasharray.h \
src/lib/svg/egueb_svg_stroke_line_cap.h \
src/lib/svg/egueb_svg_stroke_line_join.h \
src/lib/svg/egueb_svg_text_anchor.h \
src/lib/svg/egueb_svg_visibility.h \
src/lib/svg/egueb_svg_zoom_and_pan.h

src_lib_svg_libegueb_svg_la_SOURCES = \
src/lib/svg/egueb_svg_animate_transform_type.c \
src/lib/svg/egueb_svg_animate_transform_type_private.h \
src/lib/svg/egueb_svg_animate_transform_value.c \
src/lib/svg/egueb_svg_animate_transform_value_private.h \
src/lib/svg/egueb_svg_clip_path.c \
src/lib/svg/egueb_svg_color.c \
src/lib/svg/egueb_svg_color_private.h \
src/lib/svg/egueb_svg_display.c \
src/lib/svg/egueb_svg_display_private.h \
src/lib/svg/egueb_svg_document.c \
src/lib/svg/egueb_svg_document_private.h \
src/lib/svg/egueb_svg_element.c \
src/lib/svg/egueb_svg_element_private.h \
src/lib/svg/egueb_svg_event_request_painter.c \
src/lib/svg/egueb_svg_event_request_painter_private.h \
src/lib/svg/egueb_svg_fill_rule.c \
src/lib/svg/egueb_svg_gradient.c \
src/lib/svg/egueb_svg_gradient_private.h \
src/lib/svg/egueb_svg_length.c \
src/lib/svg/egueb_svg_length_private.h \
src/lib/svg/egueb_svg_main.c \
src/lib/svg/egueb_svg_main_private.h \
src/lib/svg/egueb_svg_matrix.c \
src/lib/svg/egueb_svg_matrix_private.h \
src/lib/svg/egueb_svg_number.c \
src/lib/svg/egueb_svg_number_private.h \
src/lib/svg/egueb_svg_overflow.c \
src/lib/svg/egueb_svg_overflow_private.h \
src/lib/svg/egueb_svg_paint.c \
src/lib/svg/egueb_svg_paint_private.h \
src/lib/svg/egueb_svg_paint_server.c \
src/lib/svg/egueb_svg_paint_server_private.h \
src/lib/svg/egueb_svg_painter.c \
src/lib/svg/egueb_svg_painter_private.h \
src/lib/svg/egueb_svg_painter_clip_path.c \
src/lib/svg/egueb_svg_painter_shape.c \
src/lib/svg/egueb_svg_painter_g.c \
src/lib/svg/egueb_svg_path_seg.c \
src/lib/svg/egueb_svg_path_seg_private.h \
src/lib/svg/egueb_svg_path_seg_list.c \
src/lib/svg/egueb_svg_path_seg_list_private.h \
src/lib/svg/egueb_svg_point.c \
src/lib/svg/egueb_svg_point_list.c \
src/lib/svg/egueb_svg_point_list_private.h \
src/lib/svg/egueb_svg_pointer_events.c \
src/lib/svg/egueb_svg_pointer_events_private.h \
src/lib/svg/egueb_svg_reference.c \
src/lib/svg/egueb_svg_reference_private.h \
src/lib/svg/egueb_svg_reference_gradient.c \
src/lib/svg/egueb_svg_reference_gradient_private.h \
src/lib/svg/egueb_svg_reference_gradient_linear.c \
src/lib/svg/egueb_svg_reference_gradient_radial.c \
src/lib/svg/egueb_svg_reference_clip_path.c \
src/lib/svg/egueb_svg_reference_paint_server.c \
src/lib/svg/egueb_svg_reference_paint_server_private.h \
src/lib/svg/egueb_svg_reference_pattern.c \
src/lib/svg/egueb_svg_referenceable.c \
src/lib/svg/egueb_svg_referenceable_private.h \
src/lib/svg/egueb_svg_referenceable_units.c \
src/lib/svg/egueb_svg_referenceable_units_private.h \
src/lib/svg/egueb_svg_renderable.c \
src/lib/svg/egueb_svg_renderable_private.h \
src/lib/svg/egueb_svg_renderable_container.c \
src/lib/svg/egueb_svg_renderable_container_private.h \
src/lib/svg/egueb_svg_shape.c \
src/lib/svg/egueb_svg_shape_private.h \
src/lib/svg/egueb_svg_spread_method.c \
src/lib/svg/egueb_svg_string.c \
src/lib/svg/egueb_svg_string_private.h \
src/lib/svg/egueb_svg_stroke_line_cap.c \
src/lib/svg/egueb_svg_stroke_line_cap_private.h \
src/lib/svg/egueb_svg_stroke_line_join.c \
src/lib/svg/egueb_svg_stroke_line_join_private.h \
src/lib/svg/egueb_svg_stroke_dasharray.c \
src/lib/svg/egueb_svg_text_anchor.c \
src/lib/svg/egueb_svg_text_content.c \
src/lib/svg/egueb_svg_text_content_private.h \
src/lib/svg/egueb_svg_visibility.c \
src/lib/svg/egueb_svg_visibility_private.h \
src/lib/svg/egueb_svg_zoom_and_pan.c \
src/lib/svg/egueb_svg_zoom_and_pan_private.h

src_lib_svg_libegueb_svg_la_IFLAGS = \
-I$(top_srcdir)/src/lib/svg \
-I$(top_srcdir)/src/lib/svg/attr \
-I$(top_srcdir)/src/lib/svg/element \
-I$(top_srcdir)/src/lib/svg/value

src_lib_svg_libegueb_svg_la_CPPFLAGS = \
$(src_lib_dom_libegueb_dom_la_IFLAGS) \
$(src_lib_css_libegueb_css_la_IFLAGS) \
$(src_lib_svg_libegueb_svg_la_IFLAGS) \
$(src_lib_xlink_libegueb_xlink_la_IFLAGS) \
$(src_lib_smil_libegueb_smil_la_IFLAGS) \
-DEGUEB_SVG_BUILD \
@EGUEB_SVG_CFLAGS@

src_lib_svg_libegueb_svg_la_LIBADD = \
$(top_builddir)/src/lib/dom/libegueb_dom.la \
$(top_builddir)/src/lib/css/libegueb_css.la \
$(top_builddir)/src/lib/smil/libegueb_smil.la \
$(top_builddir)/src/lib/xlink/libegueb_xlink.la \
@EGUEB_SVG_LIBS@ -lm

src_lib_svg_libegueb_svg_la_LDFLAGS = -no-undefined -version-info @version_info@
