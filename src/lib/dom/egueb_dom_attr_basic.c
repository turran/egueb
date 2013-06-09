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

#include "egueb_dom_main.h"
#include "egueb_dom_string.h"
#include "egueb_dom_attr.h"

#include "egueb_dom_attr_private.h"
#include "egueb_dom_attr_basic_private.h"
#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Eina_Bool _egueb_dom_attr_basic_value_get_local(Egueb_Dom_Attr *p,
		Egueb_Dom_Attr_Type type, void **o)
{
	Egueb_Dom_Attr_Basic_Class *klass;

	klass = EGUEB_DOM_ATTR_BASIC_CLASS_GET(p);
	return klass->value_get(p, type, o);
}
/*----------------------------------------------------------------------------*
 *                            Property interface                              *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_dom_attr_basic_value_get(Egueb_Dom_Attr *p,
		Egueb_Dom_Attr_Type type, Egueb_Dom_Value *value)
{
	Egueb_Dom_Attr_Basic_Class *klass;
	const Egueb_Dom_Value_Descriptor *desc;
	Egueb_Dom_Value_Data vdata;
	int32_t *i32;
	int64_t *i64;
	double *d;
	void *o;

	desc = egueb_dom_attr_value_descriptor_get(EGUEB_DOM_NODE(p));
	if (!_egueb_dom_attr_basic_value_get_local(p, type, &o))
		return EINA_FALSE;
	/* so far we have the pointer to either, int, long or double
	 * we must fill the data inside the value with such dereference
	 */
	switch (desc->data_from_type)
	{
		case EGUEB_DOM_VALUE_DATA_TYPE_INT32:
		i32 = o;
		value->data.i32 = *i32;
		break;

		case EGUEB_DOM_VALUE_DATA_TYPE_INT64:
		i64 = o;
		value->data.i64 = *i64;
		break;

		case EGUEB_DOM_VALUE_DATA_TYPE_DOUBLE:
		d = o;
		value->data.d = *d;
		break;
	}

	return EINA_TRUE;
}

static Eina_Bool _egueb_dom_attr_basic_value_set(Egueb_Dom_Attr *p,
		Egueb_Dom_Attr_Type type, Egueb_Dom_Value *value)
{
	Egueb_Dom_Attr_Basic_Class *klass;
	Egueb_Dom_Value_Data vdata;
	void *o;

	if (!_egueb_dom_attr_basic_value_get_local(p, type, &o))
		return EINA_FALSE;
	vdata.ptr = o;
	egueb_dom_value_data_to(value, &vdata);
	/* finally set the mask */
	p->set_mask |= type;
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                              Basic interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_ABSTRACT_BOILERPLATE(EGUEB_DOM_ATTR_DESCRIPTOR,
		Egueb_Dom_Attr_Basic, Egueb_Dom_Attr_Basic_Class,
		egueb_dom_attr_basic)

static void _egueb_dom_attr_basic_class_init(void *k)
{
	Egueb_Dom_Attr_Class *klass;

	klass = EGUEB_DOM_ATTR_CLASS(k);
	klass->value_get = _egueb_dom_attr_basic_value_get;
	klass->value_set = _egueb_dom_attr_basic_value_set;
}

static void _egueb_dom_attr_basic_instance_init(void *o)
{
}

static void _egueb_dom_attr_basic_instance_deinit(void *o)
{
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/


