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
#include "esvg_private_attribute_presentation.h"
#include "esvg_private_attribute_animation.h"
#include "esvg_private_context.h"
#include "esvg_private_element.h"
#include "esvg_private_animation.h"
#include "esvg_private_animate_base.h"

#include "esvg_animate_base.h"
/*
 * This file handles the common attribute handling for the
 * 'animate_base value attributes'. The elements that inherit
 * from this are the 'animate' and 'animateTransform'
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_ANIMATE_BASE_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_ANIMATE_BASE_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_ANIMATE_BASE_MAGIC);\
	} while(0)

static Ender_Property *ESVG_ANIMATE_BASE_TO;
static Ender_Property *ESVG_ANIMATE_BASE_FROM;
static Ender_Property *ESVG_ANIMATE_BASE_VALUES;

typedef struct _Esvg_Animate_Base_Descriptor_Internal
{
	Edom_Tag_Free free;
	Esvg_Element_Attribute_Set attribute_set;
	Edom_Tag_Attribute_Get attribute_get;
	Esvg_Animate_Base_Setup setup;
} Esvg_Animate_Base_Descriptor_Internal;

typedef struct _Esvg_Animate_Base
{
	EINA_MAGIC
	/* properties */
	Esvg_Animate_Base_Context current;
	/* interface */
	Esvg_Animate_Base_Descriptor_Internal descriptor;
	/* private */
	void *data;
} Esvg_Animate_Base;

static Esvg_Animate_Base * _esvg_animate_base_get(Edom_Tag *t)
{
	Esvg_Animate_Base *thiz;

	thiz = esvg_animation_data_get(t);
	ESVG_ANIMATE_BASE_MAGIC_CHECK(thiz);

	return thiz;
}
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static void _esvg_animate_base_to_set(Edom_Tag *t, const char *to)
{
	Esvg_Animate_Base *thiz;

	thiz = _esvg_animate_base_get(t);
	if (thiz->current.value.to)
	{
		free(thiz->current.value.to);
		thiz->current.value.to = NULL;
	}
	if (to)
		thiz->current.value.to = strdup(to);
	thiz->current.changed = EINA_TRUE;
}

static void _esvg_animate_base_to_get(Edom_Tag *t, const char **to)
{
	Esvg_Animate_Base *thiz;

	if (!to) return;
	thiz = _esvg_animate_base_get(t);
	*to = thiz->current.value.to;
}

static void _esvg_animate_base_from_set(Edom_Tag *t, const char *from)
{
	Esvg_Animate_Base *thiz;

	thiz = _esvg_animate_base_get(t);
	if (thiz->current.value.from)
	{
		free(thiz->current.value.from);
		thiz->current.value.from = NULL;
	}
	if (from)
		thiz->current.value.from = strdup(from);
	thiz->current.changed = EINA_TRUE;
}

static void _esvg_animate_base_from_get(Edom_Tag *t, const char **from)
{
	Esvg_Animate_Base *thiz;

	if (!from) return;
	thiz = _esvg_animate_base_get(t);
	*from = thiz->current.value.from;
}

static void _esvg_animate_base_values_set(Edom_Tag *t, const char *values)
{
	Esvg_Animate_Base *thiz;

	thiz = _esvg_animate_base_get(t);
	if (thiz->current.value.values)
	{
		free(thiz->current.value.values);
		thiz->current.value.values = NULL;
	}
	if (values)
		thiz->current.value.values = strdup(values);
	thiz->current.changed = EINA_TRUE;
}

static void _esvg_animate_base_values_get(Edom_Tag *t, const char **values)
{
	Esvg_Animate_Base *thiz;

	if (!values) return;
	thiz = _esvg_animate_base_get(t);
	*values = thiz->current.value.values;
}
/*----------------------------------------------------------------------------*
 *                         The Esvg Element interface                         *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_animate_base_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	/* value attributes */
	/* for from and to, we should not fetch the property an generate
	 * the final data type, we better pass it still as strings
	 */
	if (strcmp(key, "calcMode") == 0)
	{
	}
	else if (strcmp(key, "values") == 0)
	{
		esvg_animate_base_values_set(e, value);
	}
	else if (strcmp(key, "keyTimes") == 0)
	{
	}
	else if (strcmp(key, "keySplines") == 0)
	{
	}
	else if (strcmp(key, "from") == 0)
	{
		esvg_animate_base_from_set(e, value);
	}
	else if (strcmp(key, "to") == 0)
	{
		esvg_animate_base_to_set(e, value);
	}
	else if (strcmp(key, "by") == 0)
	{
	}
	else
	{
		return EINA_FALSE;
	}
}

static Eina_Bool _esvg_animate_base_attribute_get(Edom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static void _esvg_animate_base_free(Edom_Tag *t)
{
	Esvg_Animate_Base *thiz;

	thiz = _esvg_animate_base_get(t);
	if (thiz->descriptor.free)
		thiz->descriptor.free(t);
	free(thiz);
}

static Eina_Bool _esvg_animate_base_setup(Edom_Tag *t,
		Esvg_Context *c,
		Esvg_Animation_Context *actx,
		Enesim_Error **error)
{
	Esvg_Animate_Base *thiz;

	thiz = _esvg_animate_base_get(t);

	/* do the setup */
	if (thiz->descriptor.setup)
		return thiz->descriptor.setup(t, c, actx, &thiz->current, error);
	return EINA_TRUE;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* The ender wrapper */
#define _esvg_animate_base_to_is_set NULL
#define _esvg_animate_base_from_is_set NULL
#define _esvg_animate_base_values_is_set NULL
#include "generated/esvg_generated_animate_base.c"

Eina_Bool esvg_is_animate_base_internal(Edom_Tag *t)
{
	Esvg_Animate_Base *thiz;
	Eina_Bool ret;

	if (!esvg_is_element_internal(t))
		return EINA_FALSE;
	thiz = esvg_element_data_get(t);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_ANIMATE_BASE_MAGIC);

	return ret;
}

void * esvg_animate_base_data_get(Edom_Tag *t)
{
	Esvg_Animate_Base *thiz;

	thiz = _esvg_animate_base_get(t);
	return thiz->data;
}

Edom_Tag * esvg_animate_base_new(Esvg_Animate_Base_Descriptor *descriptor, Esvg_Type type,
		void *data)
{
	Esvg_Animate_Base *thiz;
	Esvg_Animation_Descriptor pdescriptor;
	Edom_Tag *t;

	thiz = calloc(1, sizeof(Esvg_Animate_Base));
	if (!thiz) return NULL;

	EINA_MAGIC_SET(thiz, ESVG_ANIMATE_BASE_MAGIC);
	thiz->data = data;
	/* our own descriptor */
	thiz->descriptor.setup = descriptor->setup;
	thiz->descriptor.attribute_set = descriptor->attribute_set;
	thiz->descriptor.attribute_get = descriptor->attribute_get;
	/* default values */
	pdescriptor.attribute_set = _esvg_animate_base_attribute_set;
	pdescriptor.attribute_get = _esvg_animate_base_attribute_get;
	pdescriptor.free = _esvg_animate_base_free;
	pdescriptor.initialize = descriptor->initialize;
	pdescriptor.setup = _esvg_animate_base_setup;

	t = esvg_animation_new(&pdescriptor, type, thiz);

	return t;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool esvg_is_animate_base(Ender_Element *e)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	return esvg_is_animate_base_internal(t);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animate_base_to_set(Ender_Element *e, const char *v)
{
	ender_element_property_value_set(e, ESVG_ANIMATE_BASE_TO, v, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animate_base_to_get(Ender_Element *e, const char **v)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animate_base_from_set(Ender_Element *e, const char *v)
{
	ender_element_property_value_set(e, ESVG_ANIMATE_BASE_FROM, v, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animate_base_from_get(Ender_Element *e, const char **v)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animate_base_values_set(Ender_Element *e, const char *v)
{
	ender_element_property_value_set(e, ESVG_ANIMATE_BASE_VALUES, v, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animate_base_values_get(Ender_Element *e, const char **v)
{
}
