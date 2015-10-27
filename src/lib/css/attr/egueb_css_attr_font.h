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

#ifndef _EGUEB_CSS_ATTR_FONT_H_
#define _EGUEB_CSS_ATTR_FONT_H_

EAPI Egueb_Dom_Node * egueb_css_attr_font_new(Egueb_Dom_String *ns,
		Eina_Bool animatable, Eina_Bool stylable,
		Eina_Bool inheritable);

EAPI Enesim_Text_Font * egueb_css_attr_font_resolve(Egueb_Dom_Node *attr,
		double rel_font_size, double rel_x_height);

#endif
