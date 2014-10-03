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

#ifndef _EGUEB_SVG_ATTR_PAINT_PRIVATE_H_
#define _EGUEB_SVG_ATTR_PAINT_PRIVATE_H_

Egueb_Dom_Node * egueb_svg_attr_paint_new(Egueb_Dom_String *name,
		const Egueb_Svg_Paint *def, Eina_Bool animatable,
		Eina_Bool stylable, Eina_Bool inheritable);

#endif

