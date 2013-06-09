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
#include "egueb_svg_main_private.h"
#include "egueb_svg_element_private.h"
#include "egueb_svg_clone_private.h"

#include "egueb_svg_main.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT _egueb_svg_clone_log

static int _egueb_svg_clone_log = -1;

typedef struct _Egueb_Svg_Clone
{
	Ender_Element *ref;
	Ender_Element *our;
} Egueb_Svg_Clone;

static Ender_Element * _egueb_svg_clone_duplicate(Ender_Element *e);

/* skip the egueb_dom properties */
static Eina_Bool _property_is_valid(Ender_Property *prop)
{
	if (prop == EGUEB_DOM_PARENT)
		return EINA_FALSE;
	/* we handle the child case using the foreach */
	else if (prop == EGUEB_DOM_CHILD)
		return EINA_FALSE;
	return EINA_TRUE;
}

static void _descriptor_property(Ender_Property *prop, void *data)
{
	Egueb_Svg_Clone *ddata = data;
	Ender_Value *v = NULL;
	const char *name;

	if (!_property_is_valid(prop))
		return;

	name = ender_property_name_get(prop);
	/* FIXME we need to implement this functionality */
	if (!ender_element_property_value_is_set(ddata->our, prop)) {
		DBG("Property '%s' is not set", name);
		return;
	}
	DBG("Setting property '%s'", name);
	ender_element_property_value_get_simple(ddata->ref, prop, &v);
	ender_element_property_value_set_simple(ddata->our, prop, v);
	ender_value_unref(v);
}

static Eina_Bool _egueb_svg_clone_child_cb(Egueb_Dom_Tag *t, Egueb_Dom_Tag *child,
		void *data)
{
	Ender_Element *our = data;
	Ender_Element *child_e;
	Ender_Element *child_our;
	Egueb_Dom_Tag *child_our_t;

	DBG("New child %s", egueb_dom_tag_name_get(child));
	if (!egueb_svg_is_element_internal(child))
		return EINA_TRUE;

	child_e = egueb_svg_element_ender_get(child);
	child_our = _egueb_svg_clone_duplicate(child_e);
	child_our_t = ender_element_object_get(child_our);

	ender_element_property_value_add(our, EGUEB_DOM_CHILD, child_our_t, NULL);

	return EINA_TRUE;
}

static Ender_Element * _egueb_svg_clone_duplicate(Ender_Element *e)
{
	Egueb_Svg_Clone data;
	Ender_Descriptor *desc;
	Ender_Element *our;
	Ender_Namespace *ns;
	Egueb_Dom_Tag *t;
	const char *name;

	/* create a new element of the same type */
	desc = ender_element_descriptor_get(e);
	if (!desc)
	{
		ERR("Referring to a non ender element?");
		return NULL;
	}

	name = ender_descriptor_name_get(desc);
	ns = ender_descriptor_namespace_get(desc);
	our = ender_namespace_element_new(ns, name);

	data.ref = e;
	data.our = our;

	/* iterate over the properties and set them on the new element */
	ender_descriptor_property_list_recursive(desc, _descriptor_property, &data);
	/* iterate over the childs and clone them too */
	t = ender_element_object_get(e);
	egueb_dom_tag_child_foreach(t, _egueb_svg_clone_child_cb, our);

	return our;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_svg_clone_init(void)
{
	_egueb_svg_clone_log = eina_log_domain_register("egueb_svg_clone", ESVG_LOG_COLOR_DEFAULT);
	if (_egueb_svg_clone_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
}

void egueb_svg_clone_shutdown(void)
{
	if (_egueb_svg_clone_log < 0)
		return;
	eina_log_domain_unregister(_egueb_svg_clone_log);
	_egueb_svg_clone_log = -1;
}

Ender_Element * egueb_svg_clone_new(Ender_Element *e)
{
	Ender_Element *clone;

	if (!e) return NULL;

	clone = _egueb_svg_clone_duplicate(e);
	if (clone)
	{
		Egueb_Dom_Tag *t;

		t = ender_element_object_get(clone);
		DBG("Cloned element '%s'", egueb_dom_tag_name_get(t));
	}
	else
	{
		WARN("Impossible to clone '%s'",
				egueb_dom_tag_name_get(ender_element_object_get(e)));
	}

	return clone;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

