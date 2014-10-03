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

#ifndef _EGUEB_SVG_ELEMENT_STOP_H_
#define _EGUEB_SVG_ELEMENT_STOP_H_

/**
 * @defgroup Egueb_Svg_Element_Stop_Group Stop
 * @brief Gradient stop
 * @ingroup Egueb_Svg_Gradient_Group
 * @{
 */

EAPI Eina_Bool egueb_svg_element_is_stop(Egueb_Dom_Node *n);
EAPI Egueb_Dom_Node * egueb_svg_element_stop_new(void);
EAPI void egueb_svg_element_stop_offset_set(Egueb_Dom_Node *n, const Egueb_Svg_Length *v);
EAPI void egueb_svg_element_stop_offset_get(Egueb_Dom_Node *n, Egueb_Svg_Length_Animated *v);

/**
 * @}
 */

#endif
