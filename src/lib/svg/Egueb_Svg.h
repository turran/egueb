/* Egueb
 * Copyright (C) 2011 - 2013 Jorge Luis Zapata
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _EGUEB_SVG_H_
#define _EGUEB_SVG_H_

#include <Eina.h>
#include <Egueb_Dom.h>
#include <Enesim.h>

/**
 * @mainpage Esvg
 * @section intro Introduction
 * @section dependencies Dependencies
 * - Eina
 * - Enesim
 * - Etex
 *
 * @todo
 */

#include "egueb_svg_build.h"

/* basic data types */
#include "egueb_svg_main.h"
#include "egueb_svg_clip_path.h"
#include "egueb_svg_color.h"
#include "egueb_svg_display.h"
#include "egueb_svg_length.h"
#include "egueb_svg_fill_rule.h"
#include "egueb_svg_font_size.h"
#include "egueb_svg_matrix.h"
#include "egueb_svg_number.h"
#include "egueb_svg_overflow.h"
#include "egueb_svg_paint.h"
#include "egueb_svg_path_seg.h"
#include "egueb_svg_path_seg_list.h"
#include "egueb_svg_point.h"
#include "egueb_svg_point_list.h"
#include "egueb_svg_referenceable_units.h"
#include "egueb_svg_rect.h"
#include "egueb_svg_spread_method.h"
#include "egueb_svg_string.h"
#include "egueb_svg_stroke_line_cap.h"
#include "egueb_svg_stroke_line_join.h"
#include "egueb_svg_visibility.h"

/* attributes */
#include "egueb_svg_attr_clip_path.h"
#include "egueb_svg_attr_color.h"
#include "egueb_svg_attr_font_size.h"
#include "egueb_svg_attr_length.h"
#include "egueb_svg_attr_matrix.h"
#include "egueb_svg_attr_number.h"
#include "egueb_svg_attr_overflow.h"
#include "egueb_svg_attr_paint.h"
#include "egueb_svg_attr_rect.h"
#include "egueb_svg_attr_referenceable_units.h"
#include "egueb_svg_attr_spread_method.h"
#include "egueb_svg_attr_string.h"
#include "egueb_svg_attr_stroke_line_cap.h"
#include "egueb_svg_attr_stroke_line_join.h"
#include "egueb_svg_attr_visibility.h"
#include "egueb_svg_attr_xlink_href.h"

/* main abstractions */
#include "egueb_svg_gradient.h"
#include "egueb_svg_shape.h"
#include "egueb_svg_paint_server.h"
#include "egueb_svg_painter.h"
#include "egueb_svg_reference.h"
#include "egueb_svg_referenceable.h"
#include "egueb_svg_renderable.h"
#include "egueb_svg_renderable_container.h"

/* document */
#include "egueb_svg_document.h"

/* elements */
#include "egueb_svg_element.h"
#include "egueb_svg_element_a.h"
#include "egueb_svg_element_circle.h"
#include "egueb_svg_element_clip_path.h"
#include "egueb_svg_element_desc.h"
#include "egueb_svg_element_defs.h"
#include "egueb_svg_element_ellipse.h"
#include "egueb_svg_element_filter.h"
#include "egueb_svg_element_g.h"
#include "egueb_svg_element_image.h"
#include "egueb_svg_element_line.h"
#include "egueb_svg_element_linear_gradient.h"
#include "egueb_svg_element_path.h"
#include "egueb_svg_element_pattern.h"
#include "egueb_svg_element_polygon.h"
#include "egueb_svg_element_polyline.h"
#include "egueb_svg_element_radial_gradient.h"
#include "egueb_svg_element_rect.h"
#include "egueb_svg_element_use.h"
#include "egueb_svg_element_stop.h"
#include "egueb_svg_element_svg.h"
#include "egueb_svg_element_tspan.h"
#include "egueb_svg_element_text.h"

#if 0
/* providers */
#include "egueb_svg_video_provider.h"
#include "egueb_svg_script_provider.h"

/* shapes */
#include "egueb_svg_element_video.h"
/* containers */

/* other */
#include "egueb_svg_element_a.h"
#include "egueb_svg_element_style.h"
#include "egueb_svg_element_script.h"

/* paint servers */
#include "egueb_svg_element_gradient.h"
#include "egueb_svg_element_stop.h"
#include "egueb_svg_element_linear_gradient.h"
#include "egueb_svg_element_radial_gradient.h"

/* animation */
#include "egueb_svg_element_animation.h"
#include "egueb_svg_animate_base.h"
#include "egueb_svg_element_animate.h"
#include "egueb_svg_element_set.h"

/* parsing */
#include "egueb_svg_parser.h"

EAPI Eina_Bool egueb_svg_is_shape(Egueb_Dom_Tag *e);

EAPI Egueb_Dom_Tag * egueb_svg_pattern_new(void);
EAPI Eina_Bool egueb_svg_is_pattern(Egueb_Dom_Tag *e);
EAPI void egueb_svg_pattern_x_set(Egueb_Dom_Tag *e, const Egueb_Svg_Coord *x);
EAPI void egueb_svg_pattern_x_get(Egueb_Dom_Tag *e, Egueb_Svg_Coord *x);
EAPI void egueb_svg_pattern_y_set(Egueb_Dom_Tag *e, const Egueb_Svg_Coord *y);
EAPI void egueb_svg_pattern_y_get(Egueb_Dom_Tag *e, Egueb_Svg_Coord *y);
EAPI void egueb_svg_pattern_width_set(Egueb_Dom_Tag *e, const Egueb_Svg_Length *width);
EAPI void egueb_svg_pattern_width_get(Egueb_Dom_Tag *e, Egueb_Svg_Length *width);
EAPI void egueb_svg_pattern_height_set(Egueb_Dom_Tag *e, const Egueb_Svg_Length *height);
EAPI void egueb_svg_pattern_height_get(Egueb_Dom_Tag *e, Egueb_Svg_Length *height);
EAPI void egueb_svg_pattern_units_set(Egueb_Dom_Tag *e, Egueb_Svg_Pattern_Units units);
EAPI void egueb_svg_pattern_units_get(Egueb_Dom_Tag *e, Egueb_Svg_Pattern_Units *units);
EAPI Eina_Bool egueb_svg_pattern_units_is_set(Egueb_Dom_Tag *e);
EAPI void egueb_svg_pattern_transform_set(Egueb_Dom_Tag *e, const Enesim_Matrix *transform);
EAPI void egueb_svg_pattern_transform_get(Egueb_Dom_Tag *e, Enesim_Matrix *transform);
EAPI Eina_Bool egueb_svg_pattern_transform_is_set(Egueb_Dom_Tag *e);
EAPI void egueb_svg_pattern_content_set(Egueb_Dom_Tag *e, Egueb_Dom_Tag *content);
#endif

#endif /*_ESVG_H*/
