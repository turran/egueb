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
#include "egueb_css_engine_context_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                          Css context interface                             *
 *----------------------------------------------------------------------------*/
static const char * _egueb_css_context_attribute_get(void *e, const char *attribute)
{
#if 0
	Egueb_Dom_Node *n = e;
	Egueb_Dom_String *prop;
	Egueb_Dom_String *val;

	prop = egueb_dom_string_new_with_string(attribute);
	egueb_dom_element_attribute_get(n, prop, &val);
	return egueb_dom_string_string_get(val);
#endif
	return NULL;
}

/* FIXME we could call directly the function _attribute_set */
static void _egueb_css_context_attribute_set(void *e, const char *attribute, const char *value)
{
	Egueb_Dom_Node *n = e;
	Egueb_Dom_String *prop;
	Egueb_Dom_String *val;

	prop = egueb_dom_string_new_with_string(attribute);
	val = egueb_dom_string_new_with_string(value);
	egueb_dom_element_attribute_type_set(n, prop, EGUEB_DOM_ATTR_TYPE_STYLED, val, NULL);
	egueb_dom_string_unref(val);
	egueb_dom_string_unref(prop);
}

static void * _egueb_css_context_get_child(void *e)
{
#if 0
	Egueb_Dom_Node *n = e;
	
	return egueb_dom_tag_child_get(tag);
#endif
	return NULL;
}

static void * _egueb_css_context_get_next_sibling(void *e)
{
#if 0
	Egueb_Dom_Node *n = e;
	Egueb_Dom_Node *next = NULL;

	egueb_dom_node_sibling_next_get(n, &next);
	return next;
#endif
	return NULL;
}

static const char * _egueb_css_context_get_name(void *e)
{
#if 0
	Egueb_Dom_Node *n = e;
	Egueb_Dom_String *name;
	const char *ret;

	egueb_dom_node_name_get(n, &name);
	ret = egueb_dom_string_string_get(name);
	egueb_dom_string_unref(name);
	return ret;
#endif
	return NULL;
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Css_Engine_Context egueb_css_context = {
	/* .attribute_set 	= */ _egueb_css_context_attribute_set,
	/* .attribute_get 	= */ _egueb_css_context_attribute_get,
	/* .get_name 		= */ _egueb_css_context_get_name,
	/* .get_child 		= */ _egueb_css_context_get_child,
	/* .get_next_sibling 	= */ _egueb_css_context_get_next_sibling,
};
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
