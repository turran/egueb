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
#include "egueb_svg_main_private.h"
#include "egueb_svg_stroke_dasharray.h"

#include "egueb_dom_attr_private.h"
#include "egueb_dom_attr_primitive_private.h"
#include "egueb_svg_attr_stroke_dasharray_private.h"
#include "egueb_svg_value_stroke_dasharray_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
EGUEB_DOM_ATTR_PRIMITIVE_BOILERPLATE(Egueb_Svg_Stroke_Dasharray,
		Egueb_Svg_Attr_Stroke_Dasharray, egueb_svg_value_stroke_dasharray,
		egueb_svg_attr_stroke_dasharray)
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Dom_Node * egueb_svg_attr_stroke_dasharray_new(Egueb_Dom_String *name,
		Eina_Bool animatable, Eina_Bool stylable, Eina_Bool inheritable)
{
	Egueb_Dom_Node *n;
	
	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_attr_stroke_dasharray);
	egueb_dom_attr_init(n, name, NULL, animatable, stylable, inheritable);
	return n;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
