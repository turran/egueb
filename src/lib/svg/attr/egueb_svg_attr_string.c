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

#include "egueb_dom_attr_private.h"
#include "egueb_dom_attr_object_private.h"
#include "egueb_svg_attr_string_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_ATTR_STRING_DESCRIPTOR 					\
		egueb_svg_attr_string_descriptor_get()
#define EGUEB_SVG_ATTR_STRING(o) ENESIM_OBJECT_INSTANCE_CHECK(o,		\
		Egueb_Svg_Attr_String, EGUEB_SVG_ATTR_STRING_DESCRIPTOR)

typedef struct _Egueb_Svg_Attr_String
{
	Egueb_Dom_Attr_Object base;
	Egueb_Dom_String *styled;
	Egueb_Dom_String *anim;
	Egueb_Dom_String *value;
	Egueb_Dom_String *def;
} Egueb_Svg_Attr_String;

typedef struct _Egueb_Svg_Attr_String_Class
{
	Egueb_Dom_Attr_Object_Class base;
} Egueb_Svg_Attr_String_Class;

static Eina_Bool _egueb_svg_attr_string_value_get(Egueb_Dom_Attr *p,
		Egueb_Dom_Attr_Type type, void ***o)
{
	Egueb_Svg_Attr_String *thiz;

	thiz = EGUEB_SVG_ATTR_STRING(p);
	switch (type)
	{
		case EGUEB_DOM_ATTR_TYPE_ANIMATED:
		*o = (void **)&thiz->anim;
		break;

		case EGUEB_DOM_ATTR_TYPE_STYLED:
		*o = (void **)&thiz->styled;
		break;

		case EGUEB_DOM_ATTR_TYPE_BASE:
		*o = (void **)&thiz->value;
		break;

		case EGUEB_DOM_ATTR_TYPE_DEFAULT:
		*o = (void **)&thiz->def;
		break;

		default:
		return EINA_FALSE;
		break;
	}
	return EINA_TRUE;
}

static const Egueb_Dom_Value_Descriptor *
_egueb_svg_attr_string_value_descriptor_get(Egueb_Dom_Attr *p)
{
	return egueb_dom_value_dom_string_descriptor_get();
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_ATTR_OBJECT_DESCRIPTOR,
		Egueb_Svg_Attr_String, Egueb_Svg_Attr_String_Class,
		egueb_svg_attr_string)

static void _egueb_svg_attr_string_class_init(void *k)
{
	Egueb_Dom_Attr_Class *p_klass;
	Egueb_Dom_Attr_Object_Class *o_klass;

	p_klass = EGUEB_DOM_ATTR_CLASS(k);
	p_klass->value_descriptor_get = _egueb_svg_attr_string_value_descriptor_get;

	o_klass = EGUEB_DOM_ATTR_OBJECT_CLASS(k);
	o_klass->value_get = _egueb_svg_attr_string_value_get;
}

static void _egueb_svg_attr_string_instance_init(void *o)
{
}

static void _egueb_svg_attr_string_instance_deinit(void *o)
{
	Egueb_Svg_Attr_String *thiz;

	thiz = EGUEB_SVG_ATTR_STRING(o);
	if (thiz->styled)
		egueb_dom_string_unref(thiz->styled);
	if (thiz->def)
		egueb_dom_string_unref(thiz->def);
	if (thiz->value)
		egueb_dom_string_unref(thiz->value);
	if (thiz->anim)
		egueb_dom_string_unref(thiz->anim);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Dom_Node * egueb_svg_attr_string_new(Egueb_Dom_String *name,
		Egueb_Dom_String *def)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_attr_string);
	egueb_dom_attr_init(n, name, NULL, EINA_TRUE, EINA_FALSE, EINA_FALSE);
	if (def) egueb_dom_attr_set(n, EGUEB_DOM_ATTR_TYPE_DEFAULT, def);
	return n;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
