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
#include "egueb_dom_attr.h"
#include "egueb_dom_event_mutation.h"

#include "egueb_dom_node_private.h"
#include "egueb_dom_attr_private.h"
#include "egueb_dom_value_private.h"
#include "egueb_dom_element_private.h"
#include "egueb_dom_event_mutation_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/* check if the property type is set and if the value is "inherited" use the
 * the relative property
 */
static Eina_Bool _egueb_dom_attr_inheritable_process(Egueb_Dom_Attr *thiz,
		Egueb_Dom_Attr_Type type, Egueb_Dom_Node *rel)
{
	if (thiz->set_mask & type)
	{
		if (thiz->inherited_mask & type)
		{
			Egueb_Dom_Attr *other;

			other = EGUEB_DOM_ATTR(rel);
			if (other->inherited)
				thiz->inherited = egueb_dom_node_ref(other->inherited);
			else
				thiz->inherited = egueb_dom_node_ref(rel);
		}
		return EINA_TRUE;
	}
	return EINA_FALSE;
}

static Eina_Bool _egueb_dom_attr_inheritable_process_full(Egueb_Dom_Attr *thiz,
		Egueb_Dom_Attr_Flag flag, Egueb_Dom_Attr_Type type,
		Egueb_Dom_Node *rel)
{
	if (thiz->flag_mask & flag)
	{
		return _egueb_dom_attr_inheritable_process(thiz, type, rel);
	}
	return EINA_FALSE;
}

static Eina_Bool _egueb_dom_attr_value_get(Egueb_Dom_Attr *thiz,
		Egueb_Dom_Attr_Type type, Egueb_Dom_Value *value)
{
	Egueb_Dom_Attr_Class *klass;

	klass = EGUEB_DOM_ATTR_CLASS_GET(thiz);
	return klass->value_get(thiz, type, value);
}
/*----------------------------------------------------------------------------*
 *                               Node interface                               *
 *----------------------------------------------------------------------------*/
static void _egueb_dom_attr_clone(Egueb_Dom_Node *n, Eina_Bool live,
		Eina_Bool deep, Egueb_Dom_Node *clone)
{
	egueb_dom_attr_copy(n, clone);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_ABSTRACT_BOILERPLATE(EGUEB_DOM_NODE_DESCRIPTOR, Egueb_Dom_Attr,
		Egueb_Dom_Attr_Class, egueb_dom_attr);

static void _egueb_dom_attr_class_init(void *k)
{
	Egueb_Dom_Node_Class *n_klass = EGUEB_DOM_NODE_CLASS(k);

	n_klass->type = EGUEB_DOM_NODE_TYPE_ATTRIBUTE_NODE;
	n_klass->clone = _egueb_dom_attr_clone;
}

static void _egueb_dom_attr_instance_init(void *o)
{
	Egueb_Dom_Attr *thiz;

	thiz = EGUEB_DOM_ATTR(o);
	thiz->ref = 1;
}

static void _egueb_dom_attr_instance_deinit(void *o)
{
	Egueb_Dom_Attr *thiz;

	thiz = EGUEB_DOM_ATTR(o);
	if (thiz->inherited)
	{
		egueb_dom_node_unref(thiz->inherited);
		thiz->inherited = NULL;
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Error egueb_dom_attr_set_va(Egueb_Dom_Node *n,
		int prop_mask, va_list args)
{
	Egueb_Dom_Attr *thiz;
	const Egueb_Dom_Value_Descriptor *d;
	Eina_Error ret = EINA_ERROR_NONE;
	int i = 0;

	thiz = EGUEB_DOM_ATTR(n);
	d = egueb_dom_attr_value_descriptor_get(n);
	while (prop_mask)
	{
		Egueb_Dom_Attr_Type type = (prop_mask & 1) << i;

		if (type)
		{
			Egueb_Dom_Value v = EGUEB_DOM_VALUE_INIT;
			Eina_Bool set;

			/* collect the data */
			EGUEB_DOM_VALUE_DATA_FROM(&v, d, args);
			set = egueb_dom_attr_value_set(n, type, &v);
			egueb_dom_value_reset(&v);
			if (!set)
			{
				ret = EGUEB_DOM_ERROR_NOT_FOUND;
				break;
			}
			thiz->set_mask |= type;
		}
		prop_mask = prop_mask >> 1;
		i++;
	}
	return ret;
}

Eina_Error egueb_dom_attr_get_va(Egueb_Dom_Node *n,
		int prop_mask, va_list args)
{
	Egueb_Dom_Attr *thiz;
	const Egueb_Dom_Value_Descriptor *d;
	Eina_Error ret = EINA_ERROR_NONE;
	int i = 0;

	thiz = EGUEB_DOM_ATTR(n);
	d = egueb_dom_attr_value_descriptor_get(n);
	while (prop_mask)
	{
		Egueb_Dom_Attr_Type type = (prop_mask & 1) << i;

		if (type)
		{
			Egueb_Dom_Value v = EGUEB_DOM_VALUE_INIT;

			egueb_dom_value_init(&v, d);
			/* collect the data */
			if (!_egueb_dom_attr_value_get(thiz, type, &v))
			{
				ret = EGUEB_DOM_ERROR_NOT_FOUND;
				break;
			}
			EGUEB_DOM_VALUE_DATA_TO(&v, d, args);
			egueb_dom_value_reset(&v);
		}
		prop_mask = prop_mask >> 1;
		i++;
	}
	return ret;
}

const Egueb_Dom_Value_Descriptor *
egueb_dom_attr_value_descriptor_get(Egueb_Dom_Node *n)
{
	Egueb_Dom_Attr_Class *klass;
	Egueb_Dom_Attr *thiz;

	thiz = EGUEB_DOM_ATTR(n);
	klass = EGUEB_DOM_ATTR_CLASS_GET(n);
	return klass->value_descriptor_get(thiz);
}

void egueb_dom_attr_copy(Egueb_Dom_Node *n,
		Egueb_Dom_Node *to)
{
	Egueb_Dom_Attr *thiz;
	Egueb_Dom_Attr *other;
	Egueb_Dom_Value v = EGUEB_DOM_VALUE_INIT;
	const Egueb_Dom_Value_Descriptor *d;

	d = egueb_dom_attr_value_descriptor_get(n);
	egueb_dom_value_init(&v, d);
	/* get the value from one and set it on the other */
	egueb_dom_attr_value_get(n, EGUEB_DOM_ATTR_TYPE_ANIMATED, &v);
	egueb_dom_attr_value_set(to, EGUEB_DOM_ATTR_TYPE_ANIMATED, &v);

	egueb_dom_attr_value_get(n, EGUEB_DOM_ATTR_TYPE_STYLED, &v);
	egueb_dom_attr_value_set(to, EGUEB_DOM_ATTR_TYPE_STYLED, &v);

	egueb_dom_attr_value_get(n, EGUEB_DOM_ATTR_TYPE_BASE, &v);
	egueb_dom_attr_value_set(to, EGUEB_DOM_ATTR_TYPE_BASE, &v);

	/* set the inherited mask */
	thiz = EGUEB_DOM_ATTR(n);
	other = EGUEB_DOM_ATTR(to);
	other->inherited_mask = thiz->inherited_mask;
	other->set_mask = thiz->set_mask;
}

void egueb_dom_attr_init(Egueb_Dom_Node *n, Egueb_Dom_String *name,
		Eina_Bool animatable, Eina_Bool stylable, Eina_Bool inheritable)
{
	Egueb_Dom_Attr *thiz;

	thiz = EGUEB_DOM_ATTR(n);
	thiz->name = name;
	thiz->flag_mask = egueb_dom_attr_flag_mask_generate(animatable,
			stylable, inheritable);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Error egueb_dom_attr_name_get(Egueb_Dom_Node *n,
		Egueb_Dom_String **name)
{
	Egueb_Dom_Attr *thiz;

	if (!name) return EGUEB_DOM_ERROR_INVALID_ACCESS;

	thiz = EGUEB_DOM_ATTR(n);
	if (!thiz->name) *name = NULL;
	else *name = egueb_dom_string_ref(thiz->name);
	return EINA_ERROR_NONE;
}

EAPI Eina_Bool egueb_dom_attr_is_stylable(Egueb_Dom_Node *n)
{
	Egueb_Dom_Attr *thiz;
	thiz = EGUEB_DOM_ATTR(n);
	return thiz->flag_mask & EGUEB_DOM_ATTR_FLAG_STYLABLE;
}

EAPI Eina_Bool egueb_dom_attr_is_animatable(Egueb_Dom_Node *n)
{
	Egueb_Dom_Attr *thiz;
	thiz = EGUEB_DOM_ATTR(n);
	return thiz->flag_mask & EGUEB_DOM_ATTR_FLAG_ANIMATABLE;
}

EAPI Eina_Bool egueb_dom_attr_is_inheritable(Egueb_Dom_Node *n)
{
	Egueb_Dom_Attr *thiz;
	thiz = EGUEB_DOM_ATTR(n);
	return thiz->flag_mask & EGUEB_DOM_ATTR_FLAG_INHERITABLE;
}

EAPI Eina_Bool egueb_dom_attr_is_set(Egueb_Dom_Node *n)
{
	Egueb_Dom_Attr *thiz;
	thiz = EGUEB_DOM_ATTR(n);
	if (thiz->set_mask & (EGUEB_DOM_ATTR_TYPE_ANIMATED | EGUEB_DOM_ATTR_TYPE_STYLED | EGUEB_DOM_ATTR_TYPE_BASE))
		return EINA_TRUE;
	else
		return EINA_FALSE;
}

EAPI Eina_Bool egueb_dom_attr_type_is_set(Egueb_Dom_Node *n,
		Egueb_Dom_Attr_Type type)
{
	Egueb_Dom_Attr *thiz;
	thiz = EGUEB_DOM_ATTR(n);
	if (thiz->set_mask & type)
		return EINA_TRUE;
	else
		return EINA_FALSE;
}

EAPI Eina_Bool egueb_dom_attr_has_value(Egueb_Dom_Node *n)
{
	Egueb_Dom_Attr *thiz;
	thiz = EGUEB_DOM_ATTR(n);
	if (thiz->set_mask & (EGUEB_DOM_ATTR_TYPE_ANIMATED |
			EGUEB_DOM_ATTR_TYPE_BASE |
			EGUEB_DOM_ATTR_TYPE_STYLED))
		return EINA_TRUE;
	else
		return EINA_FALSE;
}

EAPI void egueb_dom_attr_inherit(Egueb_Dom_Node *n,
		Egueb_Dom_Attr_Type type)
{
	Egueb_Dom_Attr *thiz;
	thiz = EGUEB_DOM_ATTR(n);
	thiz->inherited_mask |= type;
}

EAPI Eina_Bool egueb_dom_attr_unset(Egueb_Dom_Node *n, int prop_mask)
{
	Egueb_Dom_Attr *thiz;
	thiz = EGUEB_DOM_ATTR(n);
	/* we just unset the mask, the real property value will be removed
	 * at the next set or whenever the property is freed
	 */
	thiz->set_mask &= ~prop_mask;
}

EAPI Eina_Error egueb_dom_attr_set(Egueb_Dom_Node *n, int prop_mask,
	...)
{
	Eina_Error ret;
	va_list args;

	va_start(args, prop_mask);
	ret = egueb_dom_attr_set_va(n, prop_mask, args);
	va_end(args);

	return ret;
}

EAPI Eina_Error egueb_dom_attr_get(Egueb_Dom_Node *thiz, int prop_mask,
	...)
{
	Eina_Error ret;
	va_list args;

	va_start(args, prop_mask);
	ret = egueb_dom_attr_get_va(thiz, prop_mask, args);
	va_end(args);

	return ret;
}

EAPI Eina_Bool egueb_dom_attr_value_get(Egueb_Dom_Node *n,
		Egueb_Dom_Attr_Type type, Egueb_Dom_Value *value)
{
	Egueb_Dom_Attr *thiz;
	Egueb_Dom_Attr_Class *klass;
	Egueb_Dom_Value v = EGUEB_DOM_VALUE_INIT;
	const Egueb_Dom_Value_Descriptor *d;

	if (!value) return EINA_FALSE;

	d = egueb_dom_attr_value_descriptor_get(n);
	egueb_dom_value_init(&v, d);

	thiz = EGUEB_DOM_ATTR(n);
	klass = EGUEB_DOM_ATTR_CLASS_GET(thiz);
	if (!klass->value_get(thiz, type, &v))
		return EINA_FALSE;
	/* finally copy the value to the passed in value */
	egueb_dom_value_copy(&v, value);
	egueb_dom_value_reset(&v);
	return EINA_TRUE;
}

EAPI Eina_Bool egueb_dom_attr_value_set(Egueb_Dom_Node *n,
		Egueb_Dom_Attr_Type type, Egueb_Dom_Value *value)
{
	Egueb_Dom_Attr *thiz;
	Egueb_Dom_Attr_Class *klass;

	thiz = EGUEB_DOM_ATTR(n);
	klass = EGUEB_DOM_ATTR_CLASS_GET(thiz);
	if (klass->value_set(thiz, type, value) && thiz->owner)
	{
		Egueb_Dom_Event *event;

		/* trigger the mutation event */
		event = egueb_dom_event_mutation_new();
		egueb_dom_event_mutation_init_internal(event,
				EGUEB_DOM_EVENT_MUTATION_ATTR_MODIFIED,
				EINA_TRUE, EINA_FALSE,
				n,
				NULL,
				value,
				thiz->name,
				EGUEB_DOM_EVENT_MUTATION_ATTR_TYPE_MODIFICATION,
				type);
		egueb_dom_node_event_dispatch(thiz->owner, event, NULL);
		/* in case the property is inheritable, mark the element
		 * with a flag informing that. That is helpful on the processing
		 * functions of the sublcasses of an element to know if they
		 * need to process again the inheritance feature to calculate
		 * the final values
		 */
		if (egueb_dom_attr_is_inheritable(n))
		{
			Egueb_Dom_Element *e;
			e = EGUEB_DOM_ELEMENT(thiz->owner);
			e->inheritable_changed = EINA_TRUE;
		}
		thiz->set_mask |= type;
	}
	return EINA_TRUE;
}

EAPI void egueb_dom_attr_inheritable_process(Egueb_Dom_Node *n,
			Egueb_Dom_Node *rel)
{
	Egueb_Dom_Attr *thiz;
	Egueb_Dom_Attr *other;

	if (!egueb_dom_attr_is_inheritable(n)) return;

	thiz = EGUEB_DOM_ATTR(n);
	if (thiz->inherited)
	{
		egueb_dom_node_unref(thiz->inherited);
		thiz->inherited = NULL;
	}
	if (!rel) return;

	if (_egueb_dom_attr_inheritable_process_full(thiz,
			EGUEB_DOM_ATTR_FLAG_ANIMATABLE,
			EGUEB_DOM_ATTR_TYPE_ANIMATED, rel))
	{
		DBG("Animated set, using it");
		return;
	}
	if (_egueb_dom_attr_inheritable_process_full(thiz,
			EGUEB_DOM_ATTR_FLAG_STYLABLE,
			EGUEB_DOM_ATTR_TYPE_STYLED, rel))
	{
		DBG("Styled set, using it");
		return;
	}
	if (_egueb_dom_attr_inheritable_process(thiz,
			EGUEB_DOM_ATTR_TYPE_BASE, rel))
	{
		DBG("Base set, using it");
		return;
	}
	/* in case nnne of the property types is set, inherit from the relative
	 * one
	 */
	other = EGUEB_DOM_ATTR(rel);
	if (other->inherited)
		thiz->inherited = egueb_dom_node_ref(other->inherited);
	else
		thiz->inherited = egueb_dom_node_ref(rel);
}

EAPI Eina_Bool egueb_dom_attr_final_value_get(Egueb_Dom_Node *n,
		Egueb_Dom_Value *v)
{
	Egueb_Dom_Attr *thiz;

	thiz = EGUEB_DOM_ATTR(n);
	/* in case we have an inherited property use that one */
	if (thiz->inherited)
		return egueb_dom_attr_final_value_get(thiz->inherited, v);

	/* do the sandwich */
	if (egueb_dom_attr_value_get(n, EGUEB_DOM_ATTR_TYPE_ANIMATED, v))
		return EINA_TRUE;
	else if (egueb_dom_attr_value_get(n, EGUEB_DOM_ATTR_TYPE_STYLED, v))
		return EINA_TRUE;
	else if (egueb_dom_attr_value_get(n, EGUEB_DOM_ATTR_TYPE_BASE, v))
		return EINA_TRUE;
	else if (egueb_dom_attr_value_get(n, EGUEB_DOM_ATTR_TYPE_DEFAULT, v))
		return EINA_TRUE;
	else
		return EINA_FALSE;
}

EAPI Eina_Bool egueb_dom_attr_final_get_va(Egueb_Dom_Node *n, va_list args)
{
	Egueb_Dom_Attr *thiz;
	Eina_Bool ret = EINA_FALSE;

	thiz = EGUEB_DOM_ATTR(n);
	/* in case we have an inherited property use that one */
	if (thiz->inherited)
		return egueb_dom_attr_final_get_va(thiz->inherited, args);

	if (egueb_dom_attr_type_is_set(n, EGUEB_DOM_ATTR_TYPE_ANIMATED))
	{
		ret = egueb_dom_attr_get_va(n,
				EGUEB_DOM_ATTR_TYPE_ANIMATED, args);
	}
	else if (egueb_dom_attr_type_is_set(n, EGUEB_DOM_ATTR_TYPE_STYLED))
	{
		ret = egueb_dom_attr_get_va(n,
				EGUEB_DOM_ATTR_TYPE_STYLED, args);
	}
	else if (egueb_dom_attr_type_is_set(n, EGUEB_DOM_ATTR_TYPE_BASE))
	{
		ret = egueb_dom_attr_get_va(n,
				EGUEB_DOM_ATTR_TYPE_BASE, args);
	}
	else if (egueb_dom_attr_type_is_set(n, EGUEB_DOM_ATTR_TYPE_DEFAULT))
	{
		ret = egueb_dom_attr_get_va(n,
				EGUEB_DOM_ATTR_TYPE_DEFAULT, args);
	}
	return ret;
}

EAPI Eina_Bool egueb_dom_attr_final_get(Egueb_Dom_Node *n, ...)
{
	Eina_Bool ret;
	va_list args;

	va_start(args, n);
	ret = egueb_dom_attr_final_get_va(n, args);
	va_end(args);
	return ret;
}

EAPI Eina_Bool egueb_dom_attr_string_set(Egueb_Dom_Node *attr,
		Egueb_Dom_Attr_Type type, Egueb_Dom_String *str)
{
	Egueb_Dom_Value v = EGUEB_DOM_VALUE_INIT;
	const Egueb_Dom_Value_Descriptor *d;
	Eina_Bool ret;

	d = egueb_dom_attr_value_descriptor_get(attr);
	egueb_dom_value_init(&v, d);
	egueb_dom_value_string_from(&v, str);
	ret = egueb_dom_attr_value_set(attr, type, &v);
	egueb_dom_value_reset(&v);
	return ret;
}

EAPI Eina_Bool egueb_dom_attr_string_get(Egueb_Dom_Node *attr,
		Egueb_Dom_Attr_Type type, Egueb_Dom_String **str)
{
	Egueb_Dom_Value v = EGUEB_DOM_VALUE_INIT;
	const Egueb_Dom_Value_Descriptor *d;
	Eina_Bool ret;

	d = egueb_dom_attr_value_descriptor_get(attr);
	egueb_dom_value_init(&v, d);
	ret = egueb_dom_attr_value_get(attr, type, &v);
	egueb_dom_value_string_to(&v, str);
	egueb_dom_value_reset(&v);
	return ret;
}

EAPI void egueb_dom_attr_inherited_get(Egueb_Dom_Node *attr,
		Egueb_Dom_Node **inherited)
{
	Egueb_Dom_Attr *thiz;

	thiz = EGUEB_DOM_ATTR(attr);
	if (thiz->inherited)
		*inherited = egueb_dom_node_ref(thiz->inherited);
	else
		*inherited = NULL;
}
