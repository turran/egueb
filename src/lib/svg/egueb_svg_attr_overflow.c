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

#include "egueb_dom_attr.h"
#include "egueb_svg_overflow.h"

#include "egueb_dom_attr_private.h"
#include "egueb_dom_attr_basic_private.h"
#include "egueb_svg_attr_overflow.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_ATTR_OVERFLOW_DESCRIPTOR 					\
		egueb_dom_attr_overflow_descriptor_get()
#define EGUEB_SVG_ATTR_OVERFLOW(o) ENESIM_OBJECT_INSTANCE_CHECK(o,		\
		Egueb_Svg_Attr_Overflow, EGUEB_SVG_ATTR_OVERFLOW_DESCRIPTOR)

typedef struct _Egueb_Svg_Attr_Overflow
{
	Egueb_Dom_Attr_Basic base;
	Egueb_Svg_Overflow styled;
	Egueb_Svg_Overflow anim;
	Egueb_Svg_Overflow value;
	Egueb_Svg_Overflow def;
} Egueb_Svg_Attr_Overflow;

typedef struct _Egueb_Svg_Attr_Overflow_Class
{
	Egueb_Dom_Attr_Basic_Class base;
} Egueb_Svg_Attr_Overflow_Class;

static Eina_Bool _egueb_svg_attr_overflow_value_get(Egueb_Dom_Attr *p,
		Egueb_Dom_Attr_Type type, void **o)
{
	Egueb_Svg_Attr_Overflow *thiz;

	thiz = EGUEB_SVG_ATTR_OVERFLOW(p);
	switch (type)
	{
		case EGUEB_DOM_ATTR_TYPE_ANIMATED:
		*o = &thiz->anim;
		break;

		case EGUEB_DOM_ATTR_TYPE_STYLED:
		*o = &thiz->styled;
		break;

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
_egueb_svg_attr_overflow_value_descriptor_get(Egueb_Dom_Attr *p)
{
	return egueb_svg_overflow_descriptor_get();
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_ATTR_BASIC_DESCRIPTOR,
		Egueb_Svg_Attr_Overflow, Egueb_Svg_Attr_Overflow_Class,
		egueb_svg_attr_overflow)

static void _egueb_svg_attr_overflow_class_init(void *k)
{
	Egueb_Dom_Attr_Class *p_klass;
	Egueb_Dom_Attr_Basic_Class *o_klass;

	p_klass = EGUEB_DOM_ATTR_CLASS(k);
	p_klass->value_descriptor_get = _egueb_svg_attr_overflow_value_descriptor_get;

	o_klass = EGUEB_DOM_ATTR_BASIC_CLASS(k);
	o_klass->value_get = _egueb_svg_attr_overflow_value_get;
}

static void _egueb_svg_attr_overflow_instance_init(void *o)
{
}

static void _egueb_svg_attr_overflow_instance_deinit(void *o)
{
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_svg_attr_overflow_new(Egueb_Dom_String *name,
		const Egueb_Svg_Overflow def, Eina_Bool animatable,
		Eina_Bool stylable, Eina_Bool inheritable)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_attr_overflow);
	egueb_dom_attr_init(n, name, animatable, stylable, inheritable);
	egueb_dom_attr_set(n, EGUEB_DOM_ATTR_TYPE_DEFAULT, def);
	return n;
}

