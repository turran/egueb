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
#include "egueb_css_private.h"

#include "egueb_css_main.h"
#include "egueb_css_font_weight.h"
#include "egueb_css_value_font_weight.h"

#include "egueb_dom_attr_private.h"
#include "egueb_dom_attr_basic_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                           Attribute interface                              *
 *----------------------------------------------------------------------------*/
EGUEB_DOM_ATTR_BASIC_BOILERPLATE(Egueb_Css_Font_Weight,
		Egueb_Css_Attr_Font_Weight, egueb_css_value_font_weight,
		egueb_css_attr_font_weight)
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_css_attr_font_weight_new(Egueb_Dom_String *ns,
		Eina_Bool animatable, Eina_Bool stylable,
		Eina_Bool inheritable)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_css_attr_font_weight);
	egueb_dom_attr_init(n, egueb_dom_string_ref(EGUEB_CSS_NAME_FONT_WEIGHT),
			ns, animatable, stylable, inheritable);
	return n;
}


