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
#include "egueb_svg_main.h"

#include "egueb_svg_fill_rule.h"
#include "egueb_svg_value_fill_rule_private.h"

#include "egueb_dom_attr_private.h"
#include "egueb_dom_attr_basic_private.h"
#include "egueb_svg_attr_fill_rule_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
EGUEB_DOM_ATTR_BASIC_BOILERPLATE(Egueb_Svg_Fill_Rule,
		Egueb_Svg_Attr_Fill_Rule, egueb_svg_value_fill_rule,
		egueb_svg_attr_fill_rule)
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Dom_Node * egueb_svg_attr_fill_rule_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_attr_fill_rule);
	egueb_dom_attr_init(n, egueb_dom_string_ref(EGUEB_SVG_NAME_FILL_RULE),
			NULL, EINA_TRUE, EINA_TRUE, EINA_TRUE);
	egueb_dom_attr_set(n, EGUEB_DOM_ATTR_TYPE_DEFAULT, EGUEB_SVG_FILL_RULE_NON_ZERO);
	return n;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
