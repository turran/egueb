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
#include "esvg_private_element.h"
#include "esvg_private_animation.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_ANIMATION_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_ANIMATION_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_ANIMATION_MAGIC);\
	} while(0)

static Ender_Property *ESVG_ANIMATION_ATTRIBUTE_NAME;
static Ender_Property *ESVG_ANIMATION_ATTRIBUTE_TYPE;

typedef struct _Esvg_Animation_Descriptor_Internal
{
	Edom_Tag_Free free;
	Esvg_Element_Attribute_Set attribute_set;
	Edom_Tag_Attribute_Get attribute_get;
	Esvg_Animation_Setup setup;
} Esvg_Animation_Descriptor_Internal;

typedef struct _Esvg_Animation
{
	EINA_MAGIC
	/* properties */
	/* interface */
	Esvg_Animation_Descriptor_Internal descriptor;
	/* private */
	Esvg_Animation_Context context;
	void *data;
} Esvg_Animation;

static Esvg_Animation * _esvg_animation_get(Edom_Tag *t)
{
	Esvg_Animation *thiz;

	thiz = esvg_element_data_get(t);
	ESVG_ANIMATION_MAGIC_CHECK(thiz);

	return thiz;
}
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*
 *                         The Esvg Element interface                         *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_animation_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (strcmp(key, "attributeName") == 0)
	{
		esvg_animation_attribute_name_set(e, value);
	}
	else if (strcmp(key, "attributeType") == 0)
	{
		Esvg_Attribute_Type type;

		if (esvg_attribute_type_string_from(&type, value))
			esvg_animation_attribute_name_set(e, value);
	}
	else
	{
		return EINA_FALSE;
	}
}

static Eina_Bool _esvg_animation_attribute_get(Edom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static void _esvg_animation_free(Edom_Tag *t)
{
	Esvg_Animation *thiz;

	thiz = _esvg_animation_get(t);
	if (thiz->descriptor.free)
		thiz->descriptor.free(t);
	free(thiz);
}

/* TODO optimize so many 'ifs' */
static Eina_Bool _esvg_animation_setup(Edom_Tag *t,
		const Esvg_Element_Context *parent_context,
		Esvg_Element_Context *context,
		Esvg_Attribute_Presentation *attr,
		Enesim_Error **error)
{
	Esvg_Animation *thiz;

	thiz = _esvg_animation_get(t);
	/* do the setup */
	if (thiz->descriptor.setup)
		return thiz->descriptor.setup(t, context, &thiz->context, error);
	return EINA_TRUE;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* The ender wrapper */
#include "generated/esvg_generated_animation.c"

Eina_Bool esvg_is_animation_internal(Edom_Tag *t)
{
	Esvg_Animation *thiz;
	Eina_Bool ret;

	if (!esvg_is_element_internal(t))
		return EINA_FALSE;
	thiz = esvg_element_data_get(t);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_ANIMATION_MAGIC);

	return ret;
}

void * esvg_animation_data_get(Edom_Tag *t)
{
	Esvg_Animation *thiz;

	thiz = _esvg_animation_get(t);
	return thiz->data;
}

Edom_Tag * esvg_animation_new(Esvg_Animation_Descriptor *descriptor, Esvg_Type type,
		void *data)
{
	Esvg_Animation *thiz;
	Esvg_Element_Descriptor pdescriptor;
	Edom_Tag *t;

	thiz = calloc(1, sizeof(Esvg_Animation));
	if (!thiz) return NULL;

	EINA_MAGIC_SET(thiz, ESVG_ANIMATION_MAGIC);
	thiz->data = data;
	/* our own descriptor */
	thiz->descriptor.setup = descriptor->setup;
	thiz->descriptor.attribute_set = descriptor->attribute_set;
	thiz->descriptor.attribute_get = descriptor->attribute_get;
	/* default values */

	pdescriptor.child_add = descriptor->child_add;
	pdescriptor.child_remove = descriptor->child_remove;
	pdescriptor.attribute_set = _esvg_animation_attribute_set;
	pdescriptor.attribute_get = _esvg_animation_attribute_get;
	pdescriptor.cdata_set = descriptor->cdata_set;
	pdescriptor.text_set = descriptor->text_set;
	pdescriptor.free = _esvg_animation_free;
	pdescriptor.initialize = descriptor->initialize;
	pdescriptor.setup = _esvg_animation_setup;

	t = esvg_element_new(&pdescriptor, type, thiz);

	return t;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool esvg_is_animation(Ender_Element *e)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	return esvg_is_animation_internal(t);
}

EAPI void esvg_animation_attribute_name_set(Ender_Element *e, const char *name)
{
	ender_element_property_value_set(e, ESVG_ANIMATION_ATTRIBUTE_NAME, name, NULL);
}

EAPI void esvg_animation_attribute_name_get(Ender_Element *e, const char **name)
{
}

EAPI void esvg_animation_attribute_type_set(Ender_Element *e, Esvg_Attribute_Type type)
{
	ender_element_property_value_set(e, ESVG_ANIMATION_ATTRIBUTE_TYPE, type, NULL);
}

EAPI void esvg_animation_attribute_type_get(Ender_Element *e, Esvg_Attribute_Type *type)
{
}


