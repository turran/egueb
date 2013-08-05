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
#include "egueb_svg_stroke_dasharray.h"
#include "egueb_dom_attr_private.h"
#include "egueb_dom_attr_object_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_ATTR_STROKE_DASHARRAY_DESCRIPTOR 				\
		egueb_svg_attr_stroke_dasharray_descriptor_get()
#define EGUEB_SVG_ATTR_STROKE_DASHARRAY(o) ENESIM_OBJECT_INSTANCE_CHECK(o,	\
		Egueb_Svg_Attr_Path_Seg_List, 				\
		EGUEB_SVG_ATTR_STROKE_DASHARRAY_DESCRIPTOR)

typedef struct _Egueb_Svg_Attr_Path_Seg_List
{
	Egueb_Dom_Attr_Object base;
	Egueb_Dom_List *value;
	Egueb_Dom_List *anim;
	Egueb_Dom_List *def;
} Egueb_Svg_Attr_Path_Seg_List;

typedef struct _Egueb_Svg_Attr_Path_Seg_List_Class
{
	Egueb_Dom_Attr_Object_Class base;
} Egueb_Svg_Attr_Path_Seg_List_Class;

static Eina_Bool _egueb_svg_attr_stroke_dasharray_value_get(Egueb_Dom_Attr *p,
		Egueb_Dom_Attr_Type type, void ***o)
{
	Egueb_Svg_Attr_Path_Seg_List *thiz;

	thiz = EGUEB_SVG_ATTR_STROKE_DASHARRAY(p);
	switch (type)
	{
		case EGUEB_DOM_ATTR_TYPE_BASE:
		*o = (void **)&thiz->value;
		break;

		case EGUEB_DOM_ATTR_TYPE_DEFAULT:
		*o = (void **)&thiz->def;
		break;

		case EGUEB_DOM_ATTR_TYPE_ANIMATED:
		*o = (void **)&thiz->anim;
		break;

		default:
		return EINA_FALSE;
		break;
	}
	return EINA_TRUE;
}

static const Egueb_Dom_Value_Descriptor *
_egueb_svg_attr_stroke_dasharray_value_descriptor_get(Egueb_Dom_Attr *p)
{
	return egueb_svg_stroke_dasharray_descriptor_get();
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_ATTR_OBJECT_DESCRIPTOR,
		Egueb_Svg_Attr_Path_Seg_List, Egueb_Svg_Attr_Path_Seg_List_Class,
		egueb_svg_attr_stroke_dasharray)

static void _egueb_svg_attr_stroke_dasharray_class_init(void *k)
{
	Egueb_Dom_Attr_Class *p_klass;
	Egueb_Dom_Attr_Object_Class *o_klass;

	p_klass = EGUEB_DOM_ATTR_CLASS(k);
	p_klass->value_descriptor_get = _egueb_svg_attr_stroke_dasharray_value_descriptor_get;

	o_klass = EGUEB_DOM_ATTR_OBJECT_CLASS(k);
	o_klass->value_get = _egueb_svg_attr_stroke_dasharray_value_get;
}

static void _egueb_svg_attr_stroke_dasharray_instance_init(void *o)
{
}

static void _egueb_svg_attr_stroke_dasharray_instance_deinit(void *o)
{
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_svg_attr_stroke_dasharray_new(Egueb_Dom_String *name,
		Egueb_Dom_List *def, Eina_Bool animatable, Eina_Bool stylable,
		Eina_Bool inheritable)
{
	Egueb_Dom_Node *n;
	
	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_attr_stroke_dasharray);
	egueb_dom_attr_init(n, name, animatable, stylable, inheritable);
	if (def)
		egueb_dom_attr_set(n, EGUEB_DOM_ATTR_TYPE_DEFAULT, def);
	return n;
}


