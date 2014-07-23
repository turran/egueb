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
#include "egueb_smil_private.h"

#include "egueb_smil_main.h"
#include "egueb_smil_calc_mode.h"

#include "egueb_dom_attr_private.h"
#include "egueb_dom_attr_primitive_private.h"
#include "egueb_smil_calc_mode_private.h"
#include "egueb_smil_attr_calc_mode_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SMIL_PROPERTY_CALC_MODE_DESCRIPTOR 					\
		egueb_smil_attr_calc_mode_descriptor_get()
#define EGUEB_SMIL_PROPERTY_CALC_MODE(o) ENESIM_OBJECT_INSTANCE_CHECK(o,		\
		Egueb_Smil_Property_Calc_Mode, EGUEB_SMIL_PROPERTY_CALC_MODE_DESCRIPTOR)

typedef struct _Egueb_Smil_Property_Calc_Mode
{
	Egueb_Dom_Attr_Primitive base;
	Egueb_Smil_Calc_Mode value;
	Egueb_Smil_Calc_Mode def;
} Egueb_Smil_Property_Calc_Mode;

typedef struct _Egueb_Smil_Property_Calc_Mode_Class
{
	Egueb_Dom_Attr_Primitive_Class base;
} Egueb_Smil_Property_Calc_Mode_Class;

static Eina_Bool _egueb_smil_attr_calc_mode_value_get(Egueb_Dom_Attr *p,
		Egueb_Dom_Attr_Type type, void **o)
{
	Egueb_Smil_Property_Calc_Mode *thiz;

	thiz = EGUEB_SMIL_PROPERTY_CALC_MODE(p);
	switch (type)
	{
		case EGUEB_DOM_ATTR_TYPE_BASE:
		*o = &thiz->value;
		break;

		case EGUEB_DOM_ATTR_TYPE_DEFAULT:
		*o = &thiz->def;
		break;

		default:
		return EINA_FALSE;
		break;
	}
	return EINA_TRUE;
}

static const Egueb_Dom_Value_Descriptor *
_egueb_smil_attr_calc_mode_value_descriptor_get(Egueb_Dom_Attr *p)
{
	return egueb_smil_calc_mode_descriptor_get();
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_ATTR_PRIMITIVE_DESCRIPTOR,
		Egueb_Smil_Property_Calc_Mode, Egueb_Smil_Property_Calc_Mode_Class,
		egueb_smil_attr_calc_mode)

static void _egueb_smil_attr_calc_mode_class_init(void *k)
{
	Egueb_Dom_Attr_Class *p_klass;
	Egueb_Dom_Attr_Primitive_Class *o_klass;

	p_klass = EGUEB_DOM_ATTR_CLASS(k);
	p_klass->value_descriptor_get = _egueb_smil_attr_calc_mode_value_descriptor_get;

	o_klass = EGUEB_DOM_ATTR_PRIMITIVE_CLASS(k);
	o_klass->value_get = _egueb_smil_attr_calc_mode_value_get;
}

static void _egueb_smil_attr_calc_mode_instance_init(void *o)
{
}

static void _egueb_smil_attr_calc_mode_instance_deinit(void *o)
{
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Dom_Node * egueb_smil_attr_calc_mode_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_smil_attr_calc_mode);
	egueb_dom_attr_init(n, egueb_dom_string_ref(EGUEB_SMIL_NAME_CALC_MODE),
			EINA_FALSE, EINA_FALSE, EINA_FALSE);
	egueb_dom_attr_set(n, EGUEB_DOM_ATTR_TYPE_DEFAULT, EGUEB_SMIL_CALC_MODE_LINEAR);

	return n;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
