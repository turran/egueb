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
#include "egueb_dom_private.h"

#include "egueb_dom_string.h"
#include "egueb_dom_main.h"
#include "egueb_dom_attr.h"

#include "egueb_dom_attr_private.h"
#include "egueb_dom_attr_object_private.h"
#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Eina_Bool _egueb_dom_attr_object_value_get_local(Egueb_Dom_Attr *p,
		Egueb_Dom_Attr_Type type, void ***o)
{
	Egueb_Dom_Attr_Object_Class *klass;

	klass = EGUEB_DOM_ATTR_OBJECT_CLASS_GET(p);
	return klass->value_get(p, type, o);
}

static void _egueb_dom_attr_object_free(Egueb_Dom_Attr *p,
		Egueb_Dom_Attr_Type type)
{
	void **o;

	if (_egueb_dom_attr_object_value_get_local(p, type, &o))
	{
		Egueb_Dom_Value v = EGUEB_DOM_VALUE_INIT;
		Egueb_Dom_Value_Data vd;
		const Egueb_Dom_Value_Descriptor *d;

		d = egueb_dom_attr_value_descriptor_get(EGUEB_DOM_NODE(p));
		egueb_dom_value_init(&v, d);
		vd.ptr = *o;
		egueb_dom_value_data_from(&v, &vd);
		egueb_dom_value_reset(&v);
	}
}
/*----------------------------------------------------------------------------*
 *                            Property interface                              *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_dom_attr_object_value_get(Egueb_Dom_Attr *p,
		Egueb_Dom_Attr_Type type, Egueb_Dom_Value *value)
{
	const Egueb_Dom_Value_Descriptor *d;
	Egueb_Dom_Value v = EGUEB_DOM_VALUE_INIT;
	Egueb_Dom_Value_Data vd;
	void **o;

	if (!_egueb_dom_attr_object_value_get_local(p, type, &o))
		return EINA_FALSE;

	d = egueb_dom_attr_value_descriptor_get(EGUEB_DOM_NODE(p));
	egueb_dom_value_init(&v, d);
	vd.ptr = *o;
	egueb_dom_value_data_from(&v, &vd);
	egueb_dom_value_copy(&v, value, EINA_FALSE);

	return EINA_TRUE;
}

static Eina_Bool _egueb_dom_attr_object_value_set(Egueb_Dom_Attr *p,
		Egueb_Dom_Attr_Type type, Egueb_Dom_Value *value)
{
	const Egueb_Dom_Value_Descriptor *d;
	Egueb_Dom_Value v = EGUEB_DOM_VALUE_INIT;
	Egueb_Dom_Value_Data vd;
	void **o;

	if (!_egueb_dom_attr_object_value_get_local(p, type, &o))
		return EINA_FALSE;
	d = egueb_dom_attr_value_descriptor_get(EGUEB_DOM_NODE(p));
	egueb_dom_value_init(&v, d);
	vd.ptr = *o;
	egueb_dom_value_data_from(&v, &vd);
	egueb_dom_value_copy(value, &v, EINA_FALSE);
	*o = v.data.ptr;

	/* in case the object is NULL just unset this property
	 * else, set it
	 */
	if (!*o)
		p->set_mask &= ~type;
	else
		p->set_mask |= type;
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_ABSTRACT_BOILERPLATE(EGUEB_DOM_ATTR_DESCRIPTOR,
		Egueb_Dom_Attr_Object, Egueb_Dom_Attr_Object_Class,
		egueb_dom_attr_object)

static void _egueb_dom_attr_object_class_init(void *k)
{
	Egueb_Dom_Attr_Class *klass;

	klass = EGUEB_DOM_ATTR_CLASS(k);
	klass->value_get = _egueb_dom_attr_object_value_get;
	klass->value_set = _egueb_dom_attr_object_value_set;
}

static void _egueb_dom_attr_object_instance_init(void *o)
{
}

static void _egueb_dom_attr_object_instance_deinit(void *o)
{
	_egueb_dom_attr_object_free(o, EGUEB_DOM_ATTR_TYPE_BASE);
	_egueb_dom_attr_object_free(o, EGUEB_DOM_ATTR_TYPE_DEFAULT);
	_egueb_dom_attr_object_free(o, EGUEB_DOM_ATTR_TYPE_ANIMATED);
	_egueb_dom_attr_object_free(o, EGUEB_DOM_ATTR_TYPE_STYLED);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

