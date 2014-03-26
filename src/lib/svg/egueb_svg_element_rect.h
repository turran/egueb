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

#ifndef _EGUEB_SVG_ELEMENT_RECT_H_
#define _EGUEB_SVG_ELEMENT_RECT_H_

/**
 * @defgroup Egueb_Svg_Element_Rect_Group Rect
 * @brief Rect element
 * @ingroup Egueb_Svg_Shape_Group
 * @{
 */
EAPI Egueb_Dom_Node * egueb_svg_element_rect_new(void);

EAPI void egueb_svg_element_rect_x_set(Egueb_Dom_Node *n, const Egueb_Svg_Coord *x);
EAPI void egueb_svg_element_rect_x_get(Egueb_Dom_Node *n, Egueb_Svg_Coord_Animated *x);

EAPI void egueb_svg_element_rect_y_set(Egueb_Dom_Node *n, const Egueb_Svg_Coord *y);
EAPI void egueb_svg_element_rect_y_get(Egueb_Dom_Node *n, Egueb_Svg_Coord_Animated *y);

EAPI void egueb_svg_element_rect_width_set(Egueb_Dom_Node *n, const Egueb_Svg_Length *width);
EAPI void egueb_svg_element_rect_width_get(Egueb_Dom_Node *n, Egueb_Svg_Length_Animated *width);

EAPI void egueb_svg_element_rect_height_set(Egueb_Dom_Node *n, const Egueb_Svg_Length *height);
EAPI void egueb_svg_element_rect_height_get(Egueb_Dom_Node *n, Egueb_Svg_Length_Animated *height);

EAPI void egueb_svg_element_rect_rx_set(Egueb_Dom_Node *n, const Egueb_Svg_Coord *rx);
EAPI void egueb_svg_element_rect_rx_get(Egueb_Dom_Node *n, Egueb_Svg_Coord_Animated *rx);

EAPI void egueb_svg_element_rect_ry_set(Egueb_Dom_Node *n, const Egueb_Svg_Coord *ry);
EAPI void egueb_svg_element_rect_ry_get(Egueb_Dom_Node *n, Egueb_Svg_Coord_Animated *ry);

/**
 * @}
 */

#endif
