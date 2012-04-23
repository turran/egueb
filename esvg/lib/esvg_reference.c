/* Esvg - SVG
 * Copyright (C) 2011 Jorge Luis Zapata, Vincent Torri
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "esvg_private_main.h"
#include "esvg_private_reference.h"

#include "esvg_main.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/* skip the edom properties */
static Eina_Bool _property_is_valid(Ender_Property *prop)
{
	if (prop == EDOM_PARENT)
		return EINA_FALSE;
	else if (prop == EDOM_ATTRIBUTE)
		return EINA_FALSE;
	return EINA_TRUE;
}
static void _descriptor_property(Ender_Property *prop, void *data)
{
	Esvg_Reference *thiz = data;
	Ender_Value *v = NULL;
	const char *name;

	if (!_property_is_valid(prop))
		return;

	/* FIXME we need to implement this functionality */
	if (!ender_element_property_value_is_set(thiz->our, prop))
		return;
	name = ender_property_name_get(prop);
	printf("new property %s\n", name);
	ender_element_property_value_get_simple(thiz->ref, prop, &v);
	ender_element_property_value_set_simple(thiz->our, prop, v);
	ender_value_unref(v);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Esvg_Reference * esvg_reference_new(Ender_Element *e)
{
	Esvg_Reference *thiz;
	Ender_Descriptor *desc;
	Ender_Element *our;
	Ender_Namespace *ns;
	const char *name;
	const char *ns_name;

	desc = ender_element_descriptor_get(e);

	/* create a new element of the same type */
	name = ender_descriptor_name_get(desc);
	ns = ender_descriptor_namespace_get(desc);
	ns_name = ender_namespace_name_get(ns);
	our = ender_element_new_with_namespace(name, ns_name);

	thiz = calloc(1, sizeof(Esvg_Reference));
	thiz->our = our;
	thiz->ref = e;

	/* iterate over the properties and set them on the new element */
	ender_descriptor_property_list_recursive(desc, _descriptor_property, thiz);

	return thiz;
}

void esvg_reference_free(Esvg_Reference *thiz)
{
	/* TODO remove the refs */
	free(thiz);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

