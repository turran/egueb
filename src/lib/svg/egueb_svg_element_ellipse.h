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
#ifndef _EGUEB_SVG_ELEMENT_ELLIPSE_H_
#define _EGUEB_SVG_ELEMENT_ELLIPSE_H_

/**
 * @defgroup Egueb_Svg_Element_Ellipse_Group Ellipse
 * @brief Ellipse element
 * @ingroup Egueb_Svg_Shape_Group
 * @{
 */

EAPI Egueb_Dom_Node * egueb_svg_element_ellipse_new(void);

#if 0
EAPI Eina_Bool egueb_svg_is_ellipse(Ender_Element *e);
EAPI void egueb_svg_element_ellipse_cx_set(Ender_Element *e, const Egueb_Svg_Coord *cx);
EAPI void egueb_svg_element_ellipse_cx_get(Ender_Element *e, Egueb_Svg_Coord *cx);
EAPI void egueb_svg_element_ellipse_cy_set(Ender_Element *e, const Egueb_Svg_Coord *cy);
EAPI void egueb_svg_element_ellipse_cy_get(Ender_Element *e, Egueb_Svg_Coord *cy);
EAPI void egueb_svg_element_ellipse_rx_set(Ender_Element *e, const Egueb_Svg_Length *rx);
EAPI void egueb_svg_element_ellipse_rx_get(Ender_Element *e, Egueb_Svg_Length *rx);
EAPI void egueb_svg_element_ellipse_ry_set(Ender_Element *e, const Egueb_Svg_Length *ry);
EAPI void egueb_svg_element_ellipse_ry_get(Ender_Element *e, Egueb_Svg_Length *ry);
#endif

/**
 * @}
 */

#endif
