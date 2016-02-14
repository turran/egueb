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
#include "egueb_css_private.h"

#include "egueb_css_main.h"
#include "egueb_css_attr_style.h"

#include "egueb_css_engine_selector_private.h"
#include "egueb_css_engine_rule_private.h"
#include "egueb_css_engine_style_private.h"

#include "egueb_dom_attr_private.h"
#include "egueb_dom_attr_object_private.h"
#include "egueb_css_attr_style_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_CSS_ATTR_STYLE_DESCRIPTOR 					\
		egueb_css_attr_style_descriptor_get()
#define EGUEB_CSS_ATTR_STYLE(o) ENESIM_OBJECT_INSTANCE_CHECK(o,		\
		Egueb_Css_Attr_String, EGUEB_CSS_ATTR_STYLE_DESCRIPTOR)

typedef struct _Egueb_Css_Attr_String
{
	Egueb_Dom_Attr_Object base;
	Egueb_Dom_String *value;
	Egueb_Dom_String *last_value;
	Eina_Bool force_process;
} Egueb_Css_Attr_String;

typedef struct _Egueb_Css_Attr_String_Class
{
	Egueb_Dom_Attr_Object_Class base;
} Egueb_Css_Attr_String_Class;

static Eina_Bool _egueb_css_attr_style_value_get(Egueb_Dom_Attr *p,
		Egueb_Dom_Attr_Type type, void ***o)
{
	Egueb_Css_Attr_String *thiz;

	thiz = EGUEB_CSS_ATTR_STYLE(p);
	switch (type)
	{
		case EGUEB_DOM_ATTR_TYPE_BASE:
		*o = (void **)&thiz->value;
		break;

		default:
		return EINA_FALSE;
		break;
	}
	return EINA_TRUE;
}

static const Egueb_Dom_Value_Descriptor *
_egueb_css_attr_style_value_descriptor_get(Egueb_Dom_Attr *p)
{
	return egueb_dom_value_dom_string_descriptor_get();
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_ATTR_OBJECT_DESCRIPTOR,
		Egueb_Css_Attr_String, Egueb_Css_Attr_String_Class,
		egueb_css_attr_style)

static void _egueb_css_attr_style_class_init(void *k)
{
	Egueb_Dom_Attr_Class *p_klass;
	Egueb_Dom_Attr_Object_Class *o_klass;

	p_klass = EGUEB_DOM_ATTR_CLASS(k);
	p_klass->value_descriptor_get = _egueb_css_attr_style_value_descriptor_get;

	o_klass = EGUEB_DOM_ATTR_OBJECT_CLASS(k);
	o_klass->value_get = _egueb_css_attr_style_value_get;
}

static void _egueb_css_attr_style_instance_init(void *o)
{
}

static void _egueb_css_attr_style_instance_deinit(void *o)
{
	Egueb_Css_Attr_String *thiz;

	thiz = EGUEB_CSS_ATTR_STYLE(o);
	if (thiz->value)
		egueb_dom_string_unref(thiz->value);
	if (thiz->last_value)
		egueb_dom_string_unref(thiz->last_value);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_css_attr_is_style(Egueb_Dom_Node *n)
{
	if (!n) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(n),
			EGUEB_CSS_ATTR_STYLE_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}

void egueb_css_attr_style_force_process(Egueb_Dom_Node *n)
{
	Egueb_Css_Attr_String *thiz;

	thiz = EGUEB_CSS_ATTR_STYLE(n);
	thiz->force_process = EINA_TRUE;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_css_attr_style_new(Egueb_Dom_String *ns)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_css_attr_style);
	egueb_dom_attr_init(n, egueb_dom_string_ref(EGUEB_CSS_NAME_STYLE),
			ns, EINA_FALSE, EINA_FALSE, EINA_FALSE);
	return n;
}

EAPI void egueb_css_attr_style_process(Egueb_Dom_Node *n)
{
	Egueb_Css_Attr_String *thiz;

	thiz = EGUEB_CSS_ATTR_STYLE(n);

	/* in case a stylesheet has been applied, re-apply the style atribute */
	/* else in case the style attribute has changed, revert the style and apply
	 * the new style
	 */
	if (thiz->last_value != thiz->value || thiz->force_process)
	{
		Egueb_Dom_Node *owner;

		owner = egueb_dom_attr_owner_get(n);
		const char *v;

		/* unapply previous style */
		if (thiz->last_value)
		{
			DBG_ELEMENT(EGUEB_DOM_NODE(owner), "Unapplying the style");
			egueb_dom_node_freeze(owner);
			v = egueb_dom_string_chars_get(thiz->value);
			egueb_css_engine_style_inline_unapply(owner, v);
			egueb_dom_node_thaw(owner);
			/* swap */
			egueb_dom_string_unref(thiz->last_value);
			thiz->last_value = NULL;
		}
		/* apply the new style */
		if (thiz->value)
		{
			DBG_ELEMENT(EGUEB_DOM_NODE(owner), "Applying the style");
			egueb_dom_node_freeze(owner);
			v = egueb_dom_string_chars_get(thiz->value);
			egueb_css_engine_style_inline_apply(owner, v);
			egueb_dom_node_thaw(owner);
			/* keep it */
			thiz->last_value = egueb_dom_string_ref(thiz->value);
		}

		thiz->force_process = EINA_FALSE;
		egueb_dom_node_unref(owner);
	}
}
