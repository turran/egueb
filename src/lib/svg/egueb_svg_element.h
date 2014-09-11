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

/**
 * @file
 * @ender_group{Egueb_Svg_Element}
 */

#include "egueb_dom_node.h"
#include "egueb_svg_length.h"
#include "egueb_svg_number.h"
#include "egueb_svg_color.h"
#include "egueb_svg_paint.h"
#include "egueb_svg_display.h"
#include "egueb_svg_visibility.h"
#include "egueb_svg_fill_rule.h"
#include "egueb_svg_stroke_line_join.h"
#include "egueb_svg_stroke_line_cap.h"
#include "egueb_svg_clip_path.h"
#include "egueb_svg_overflow.h"

/**
 * @defgroup Egueb_Svg_Element Element 
 * @brief Element @ender_inherits{Egueb_Dom_Element}
 * @ingroup Egueb_Svg_Group
 * @{
 */

EAPI Eina_Bool egueb_svg_is_element(Egueb_Dom_Node *n);

EAPI void egueb_svg_element_clip_path_set(Egueb_Dom_Node *n, Egueb_Svg_Clip_Path *v);
EAPI void egueb_svg_element_clip_path_get(Egueb_Dom_Node *n, Egueb_Svg_Clip_Path_Animated *v);
EAPI void egueb_svg_element_opacity_set(Egueb_Dom_Node *n, const Egueb_Svg_Number *v);
EAPI void egueb_svg_element_opacity_get(Egueb_Dom_Node *n, Egueb_Svg_Number_Animated *v);
EAPI void egueb_svg_element_overflow_set(Egueb_Dom_Node *n, const Egueb_Svg_Overflow v);
EAPI void egueb_svg_element_overflow_get(Egueb_Dom_Node *n, Egueb_Svg_Overflow_Animated *v);
EAPI void egueb_svg_element_color_set(Egueb_Dom_Node *n, const Egueb_Svg_Color *v);
EAPI void egueb_svg_element_color_get(Egueb_Dom_Node *n, Egueb_Svg_Color_Animated *v);
EAPI void egueb_svg_element_fill_set(Egueb_Dom_Node *n, const Egueb_Svg_Paint *v);
EAPI void egueb_svg_element_fill_get(Egueb_Dom_Node *n, Egueb_Svg_Paint_Animated *v);
EAPI void egueb_svg_element_fill_opacity_set(Egueb_Dom_Node *n, const Egueb_Svg_Number *v);
EAPI void egueb_svg_element_fill_opacity_get(Egueb_Dom_Node *n, Egueb_Svg_Number_Animated *v);
EAPI void egueb_svg_element_font_size_set(Egueb_Dom_Node *n, const Egueb_Css_Font_Size *v);
EAPI void egueb_svg_element_stroke_set(Egueb_Dom_Node *n, const Egueb_Svg_Paint *v);
EAPI void egueb_svg_element_stroke_get(Egueb_Dom_Node *n, Egueb_Svg_Paint_Animated *v);
EAPI void egueb_svg_element_stroke_width_set(Egueb_Dom_Node *n, const Egueb_Svg_Length *stroke_width);
EAPI void egueb_svg_element_stroke_width_get(Egueb_Dom_Node *n, Egueb_Svg_Length_Animated *v);
EAPI void egueb_svg_element_stroke_opacity_set(Egueb_Dom_Node *n, const Egueb_Svg_Number *v);
EAPI void egueb_svg_element_stroke_opacity_get(Egueb_Dom_Node *n, Egueb_Svg_Number_Animated *v);

/**
 * @}
 */

#endif
