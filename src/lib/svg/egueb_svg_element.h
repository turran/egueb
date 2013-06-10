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

#ifndef _EGUEB_SVG_ELEMENT_H_
#define _EGUEB_SVG_ELEMENT_H_

#include "egueb_dom_node.h"
#include "egueb_svg_length.h"
#include "egueb_svg_number.h"
#include "egueb_svg_color.h"
#include "egueb_svg_paint.h"
#include "egueb_svg_display.h"
#include "egueb_svg_visibility.h"
#include "egueb_svg_fill_rule.h"
#include "egueb_svg_font_size.h"
#include "egueb_svg_stroke_line_join.h"
#include "egueb_svg_stroke_line_cap.h"
#include "egueb_svg_clip_path.h"

EAPI Eina_Bool egueb_svg_is_element(Egueb_Dom_Node *n);
EAPI Eina_Error egueb_svg_element_presentation_relative_get(Egueb_Dom_Node *n,
		Egueb_Dom_Node **presentation_relative);
EAPI Eina_Error egueb_svg_element_presentation_relative_set(Egueb_Dom_Node *n,
		Egueb_Dom_Node *presentation_relative);
EAPI Eina_Error egueb_svg_element_geometry_relative_get(Egueb_Dom_Node *n,
		Egueb_Dom_Node **geometry_relative);
EAPI Eina_Error egueb_svg_element_geometry_relative_set(Egueb_Dom_Node *n,
		Egueb_Dom_Node *geometry_relative);

EAPI void egueb_svg_element_clip_path_final_get(Egueb_Dom_Node *n,
		Egueb_Svg_Clip_Path *clip_path);

EAPI Eina_Error egueb_svg_element_font_size_set(Egueb_Dom_Node *n,
		const Egueb_Svg_Font_Size *font_size);
#if 0
/* core */
EAPI Egueb_Svg_Type egueb_svg_element_type_get(Ender_Element *e);
EAPI const char * egueb_svg_element_name_get(Ender_Element *e);
EAPI Ender_Element * egueb_svg_element_topmost_get(Ender_Element *e);
EAPI Eina_Bool egueb_svg_element_style_is_set(Egueb_Dom_Node *n);
#endif
/* attributes */
EAPI void egueb_svg_element_id_set(Egueb_Dom_Node *n, const char *id);
EAPI void egueb_svg_element_id_get(Egueb_Dom_Node *n, const char **id);
EAPI void egueb_svg_element_class_set(Egueb_Dom_Node *n, const char *id);
EAPI void egueb_svg_element_transform_set(Egueb_Dom_Node *n, const Enesim_Matrix *transform);
EAPI void egueb_svg_element_transform_get(Egueb_Dom_Node *n, Enesim_Matrix *transform);
EAPI void egueb_svg_element_style_set(Egueb_Dom_Node *n, const char *style);
EAPI void egueb_svg_element_style_get(Egueb_Dom_Node *n, const char **style);
EAPI void egueb_svg_element_clip_path_set(Egueb_Dom_Node *n, Egueb_Svg_Clip_Path *clip_path);
EAPI void egueb_svg_element_clip_path_unset(Egueb_Dom_Node *n);
EAPI void egueb_svg_element_opacity_set(Egueb_Dom_Node *n, const Egueb_Svg_Number *v);
EAPI void egueb_svg_element_opacity_get(Egueb_Dom_Node *n, Egueb_Svg_Number *v);
EAPI void egueb_svg_element_opacity_unset(Egueb_Dom_Node *n);
EAPI void egueb_svg_element_color_set(Egueb_Dom_Node *n, const Egueb_Svg_Color *color);
EAPI void egueb_svg_element_color_get(Egueb_Dom_Node *n, Egueb_Svg_Color *color);
EAPI void egueb_svg_element_color_unset(Egueb_Dom_Node *n);
EAPI void egueb_svg_element_fill_set(Egueb_Dom_Node *n, const Egueb_Svg_Paint *fill);
EAPI void egueb_svg_element_fill_unset(Egueb_Dom_Node *n);
EAPI void egueb_svg_element_fill_opacity_set(Egueb_Dom_Node *n, const Egueb_Svg_Number *v);
EAPI void egueb_svg_element_fill_opacity_get(Egueb_Dom_Node *n, Egueb_Svg_Number *v);
EAPI void egueb_svg_element_fill_opacity_unset(Egueb_Dom_Node *n);
EAPI void egueb_svg_element_fill_rule_set(Egueb_Dom_Node *n, Egueb_Svg_Fill_Rule fill_rule);
EAPI void egueb_svg_element_fill_rule_unset(Egueb_Dom_Node *n);
EAPI void egueb_svg_element_stroke_set(Egueb_Dom_Node *n, const Egueb_Svg_Paint *stroke);
EAPI void egueb_svg_element_stroke_unset(Egueb_Dom_Node *n);
EAPI void egueb_svg_element_stroke_width_set(Egueb_Dom_Node *n, const Egueb_Svg_Length *stroke_width);
EAPI void egueb_svg_element_stroke_width_unset(Egueb_Dom_Node *n);
EAPI void egueb_svg_element_stroke_opacity_set(Egueb_Dom_Node *n, const Egueb_Svg_Number *v);
EAPI void egueb_svg_element_stroke_opacity_get(Egueb_Dom_Node *n, Egueb_Svg_Number *v);
EAPI void egueb_svg_element_stroke_opacity_unset(Egueb_Dom_Node *n);
EAPI void egueb_svg_element_stroke_line_cap_set(Egueb_Dom_Node *n, Egueb_Svg_Stroke_Line_Cap cap);
EAPI void egueb_svg_element_stroke_line_cap_unset(Egueb_Dom_Node *n);
EAPI void egueb_svg_element_stroke_line_join_set(Egueb_Dom_Node *n, Egueb_Svg_Stroke_Line_Join join);
EAPI void egueb_svg_element_stroke_line_join_unset(Egueb_Dom_Node *n);
EAPI void egueb_svg_element_visibility_set(Egueb_Dom_Node *n, Egueb_Svg_Visibility visibility);
EAPI void egueb_svg_element_visibility_unset(Egueb_Dom_Node *n);
EAPI void egueb_svg_element_display_set(Egueb_Dom_Node *n, Egueb_Svg_Display display);
EAPI void egueb_svg_element_stop_opacity_set(Egueb_Dom_Node *n, const Egueb_Svg_Number *v);
EAPI void egueb_svg_element_stop_opacity_get(Egueb_Dom_Node *n, Egueb_Svg_Number *v);
EAPI void egueb_svg_element_stop_color_set(Egueb_Dom_Node *n, Egueb_Svg_Color *stop_color);
EAPI void egueb_svg_element_stop_color_get(Egueb_Dom_Node *n, Egueb_Svg_Color *stop_color);
/* events */
EAPI void egueb_svg_element_onfocusin_set(Egueb_Dom_Node *n, const char *v);
EAPI void egueb_svg_element_onfocusout_set(Egueb_Dom_Node *n, const char *v);
EAPI void egueb_svg_element_onactivate_set(Egueb_Dom_Node *n, const char *v);
EAPI void egueb_svg_element_onclick_set(Egueb_Dom_Node *n, const char *v);
EAPI void egueb_svg_element_onmousedown_set(Egueb_Dom_Node *n, const char *v);
EAPI void egueb_svg_element_onmouseup_set(Egueb_Dom_Node *n, const char *v);
EAPI void egueb_svg_element_onmouseover_set(Egueb_Dom_Node *n, const char *v);
EAPI void egueb_svg_element_onmousemove_set(Egueb_Dom_Node *n, const char *v);
EAPI void egueb_svg_element_onmouseout_set(Egueb_Dom_Node *n, const char *v);

#endif
