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

#ifndef _EGUEB_SVG_ELEMENT_USE_H_
#define _EGUEB_SVG_ELEMENT_USE_H_

EAPI Egueb_Dom_Node * egueb_svg_element_use_new(void);
EAPI Eina_Bool egueb_svg_element_is_use(Egueb_Dom_Node *n);

#if 0
EAPI void egueb_svg_element_use_x_set(Ender_Element *e, const Egueb_Svg_Coord *x);
EAPI void egueb_svg_element_use_x_get(Ender_Element *e, Egueb_Svg_Coord *x);
EAPI void egueb_svg_element_use_y_set(Ender_Element *e, const Egueb_Svg_Coord *y);
EAPI void egueb_svg_element_use_y_get(Ender_Element *e, Egueb_Svg_Coord *y);
EAPI void egueb_svg_element_use_width_set(Ender_Element *e, const Egueb_Svg_Length *width);
EAPI void egueb_svg_element_use_width_get(Ender_Element *e, Egueb_Svg_Length *width);
EAPI void egueb_svg_element_use_height_set(Ender_Element *e, const Egueb_Svg_Length *height);
EAPI void egueb_svg_element_use_height_get(Ender_Element *e, Egueb_Svg_Length *height);
EAPI void egueb_svg_element_use_link_set(Ender_Element *e, const char *link);
EAPI void egueb_svg_element_use_link_get(Ender_Element *e, const char **link);
#endif

#endif
