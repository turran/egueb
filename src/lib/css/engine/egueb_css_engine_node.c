/* Egueb_Css_Engine - CSS
 * Copyright (C) 2011 Jorge Luis Zapata
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

#include "egueb_css_engine_node_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_css_engine_node_attribute_set(Egueb_Dom_Node *n,
		Egueb_Dom_String *attr, Egueb_Dom_String *val)
{
	DBG("Setting attr '%s' with value '%s'",
			egueb_dom_string_chars_get(attr),
			egueb_dom_string_chars_get(val));
	egueb_dom_element_attribute_type_set(n, attr, EGUEB_DOM_ATTR_TYPE_STYLED, val, NULL);
	egueb_dom_string_unref(val);
	egueb_dom_string_unref(attr);
}

void egueb_css_engine_node_attribute_set_simple(Egueb_Dom_Node *n,
		const char *attr, const char *val)
{
	Egueb_Dom_String *s_attr;
	Egueb_Dom_String *s_val;

	s_attr = egueb_dom_string_new_with_chars(attr);
	s_val = egueb_dom_string_new_with_chars(val);
	egueb_css_engine_node_attribute_set(n, s_attr, s_val);
}

void egueb_css_engine_node_attribute_clear(Egueb_Dom_Node *n,
		Egueb_Dom_String *s_attr)
{
	Egueb_Dom_Node *attr;

	attr = egueb_dom_element_attribute_node_get(n, s_attr);
	if (attr)
	{
		DBG("Clearing attr '%s'", egueb_dom_string_chars_get(s_attr));
		egueb_dom_attr_unset(attr, EGUEB_DOM_ATTR_TYPE_STYLED);
		egueb_dom_node_unref(attr);
	}
	egueb_dom_string_unref(s_attr);
}

void egueb_css_engine_node_attribute_clear_simple(Egueb_Dom_Node *n,
		const char *attr)
{
	Egueb_Dom_String *s_attr;

	s_attr = egueb_dom_string_new_with_chars(attr);
	egueb_css_engine_node_attribute_clear(n, s_attr);
}

Egueb_Dom_String * egueb_css_engine_node_attribute_get(Egueb_Dom_Node *n,
		Egueb_Dom_String *attr)
{
	return egueb_dom_element_attribute_type_get(n, attr, EGUEB_DOM_ATTR_TYPE_BASE);
}

Eina_Bool egueb_css_engine_node_attribute_is_simple(Egueb_Dom_Node *n,
		const char *attr, const char *is_val)
{
	Egueb_Dom_String *s_attr;
	Egueb_Dom_String *s_val;
	Eina_Bool ret = EINA_FALSE;

	s_attr = egueb_dom_string_new_with_chars(attr);
	s_val = egueb_css_engine_node_attribute_get(n, s_attr);
	if (egueb_dom_string_is_valid(s_val))
	{
		if (!strcmp (egueb_dom_string_chars_get(s_val), is_val))
			ret = EINA_TRUE;
	}
	egueb_dom_string_unref(s_attr);
	egueb_dom_string_unref(s_val);
	return ret;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

