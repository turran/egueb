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
#include "esvg_private_element.h"
#include "esvg_private_clone.h"

#include "esvg_main.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT esvg_log_clone

typedef struct _Esvg_Clone
{
	Ender_Element *ref;
	Ender_Element *our;
} Esvg_Clone;

static Ender_Element * _esvg_clone_duplicate(Ender_Element *e);

/* skip the edom properties */
static Eina_Bool _property_is_valid(Ender_Property *prop)
{
	if (prop == EDOM_PARENT)
		return EINA_FALSE;
	else if (prop == EDOM_ATTRIBUTE)
		return EINA_FALSE;
	/* we handle the child case using the foreach */
	else if (prop == EDOM_CHILD)
		return EINA_FALSE;
	return EINA_TRUE;
}

static void _descriptor_property(Ender_Property *prop, void *data)
{
	Esvg_Clone *ddata = data;
	Ender_Value *v = NULL;
	const char *name;

	if (!_property_is_valid(prop))
		return;

	/* FIXME we need to implement this functionality */
	if (!ender_element_property_value_is_set(ddata->our, prop))
		return;
	name = ender_property_name_get(prop);
	//printf("new property %s\n", name);
	ender_element_property_value_get_simple(ddata->ref, prop, &v);
	ender_element_property_value_set_simple(ddata->our, prop, v);
	ender_value_unref(v);
}

static Eina_Bool _esvg_clone_child_cb(Edom_Tag *t, Edom_Tag *child,
		void *data)
{
	Ender_Element *our = data;
	Ender_Element *child_e;
	Ender_Element *child_our;
	Edom_Tag *child_our_t;

	DBG("New child %s", edom_tag_name_get(child));
	if (!esvg_is_element_internal(child))
		return EINA_TRUE;

	child_e = esvg_element_ender_get(child);
	child_our = _esvg_clone_duplicate(child_e);
	child_our_t = ender_element_object_get(child_our);

	ender_element_property_value_add(our, EDOM_CHILD, child_our_t, NULL);

	return EINA_TRUE;
}

static Ender_Element * _esvg_clone_duplicate(Ender_Element *e)
{
	Esvg_Clone data;
	Ender_Descriptor *desc;
	Ender_Element *our;
	Ender_Namespace *ns;
	Edom_Tag *t;
	const char *name;
	const char *ns_name;

	/* create a new element of the same type */
	desc = ender_element_descriptor_get(e);
	if (!desc)
	{
		ERR("Referring to a non ender element?");
		return NULL;
	}

	name = ender_descriptor_name_get(desc);
	ns = ender_descriptor_namespace_get(desc);
	ns_name = ender_namespace_name_get(ns);
	our = ender_element_new_with_namespace(name, ns_name);

	data.ref = e;
	data.our = our;

	/* iterate over the properties and set them on the new element */
	ender_descriptor_property_list_recursive(desc, _descriptor_property, &data);
	/* iterate over the childs and clone them too */
	t = ender_element_object_get(e);
	edom_tag_child_foreach(t, _esvg_clone_child_cb, our);

	return our;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Element * esvg_clone_new(Ender_Element *e)
{
	Ender_Element *clone;

	if (!e) return NULL;

	clone = _esvg_clone_duplicate(e);
	if (clone)
	{
		Edom_Tag *t;

		t = ender_element_object_get(clone);
		DBG("Cloned element '%s'", edom_tag_name_get(t));
	}
	else
	{
		WARN("Impossible to clone '%s'",
				edom_tag_name_get(ender_element_object_get(e)));
	}

	return clone;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

