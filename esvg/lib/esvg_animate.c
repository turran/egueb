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
#include "esvg_private_context.h"
#include "esvg_private_element.h"
#include "esvg_private_attribute_animation.h"
#include "esvg_private_animation.h"
#include "esvg_private_animate_base.h"
#include "esvg_private_svg.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Esvg_Animate
{
	/* properties */
	/* interface */
	/* private */
#if 0
	Eina_List *values;
	Ender_Property *prop;
	Ender_Element *parent_e;
	Edom_Tag *parent_t;
	Esvg_Attribute_Type attribute_type;
	/* new */
	Eina_List *animations;
#endif
} Esvg_Animate;

static Esvg_Animate * _esvg_animate_get(Edom_Tag *t)
{
	Esvg_Animate *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_ANIMATE)
		return NULL;
	thiz = esvg_animate_base_data_get(t);

	return thiz;
}

static void * _esvg_animate_value_get(const char *attr)
{
	printf("attr = %s\n", attr);
}
/*----------------------------------------------------------------------------*
 *                        The different type descriptors                      *
 *----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*
 *                       The Esvg_Length type descriptor                      *
 *----------------------------------------------------------------------------*/
static void _esvg_animate_length_cb(Edom_Tag *t,
		Ender_Element *e,
		Ender_Property *p,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *kdata,
		void *data)
{
	Esvg_Length *length = kdata;
	Esvg_Animated_Length v;

	v.base.value = curr->data.d;
	v.base.unit = length->unit;

	ender_element_property_value_set(e, p, &v, NULL);
}

static Eina_Bool _esvg_animate_length_get(const char *attr, void **value)
{
	Esvg_Length *v;

	v = calloc(1, sizeof(Esvg_Length));
	esvg_length_string_from(v, attr);
	*value = v;
	return EINA_TRUE;
}

static void _esvg_animate_length_etch_data_to(void *d,
		Etch_Data *data)
{
	Esvg_Length *v = d;

	data->type = ETCH_DOUBLE;
	data->data.d = v->value;
}

static Eina_Bool _esvg_animate_length_animation_generate(Edom_Tag *t,
		Eina_List *values,
		Eina_List *times,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx)
{
	esvg_animate_base_animation_generate(t, values, times, actx, abctx, ETCH_DOUBLE,
			_esvg_animate_length_etch_data_to, _esvg_animate_length_cb, NULL);
	return EINA_TRUE;
}

static Esvg_Animate_Base_Type_Descriptor _length_descriptor = {
	/* .value_get 		= */ _esvg_animate_length_get,
	/* .value_free 		= */ free,
	/* .animation_generate 	= */ _esvg_animate_length_animation_generate,
};
/*----------------------------------------------------------------------------*
 *                        The double type descriptor                          *
 *----------------------------------------------------------------------------*/
static void _esvg_animate_number_cb(Edom_Tag *t,
		Ender_Element *e,
		Ender_Property *p,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *kdata,
		void *data)
{
	Esvg_Animated_Number v;

	v.base = curr->data.d;
	ender_element_property_value_set(e, p, &v, NULL);
}

static void _esvg_animate_number_etch_data_to(void *d,
		Etch_Data *data)
{
	double *v = d;

	data->type = ETCH_DOUBLE;
	data->data.d = *v;
}

static Eina_Bool _esvg_animate_number_get(const char *attr, void **value)
{
	double *v;

	v = calloc(1, sizeof(double));
	*v = esvg_number_string_from(attr, 1.0);
	*value = v;
	return EINA_TRUE;
}

static Eina_Bool _esvg_animate_number_animation_generate(Edom_Tag *t,
		Eina_List *values,
		Eina_List *times,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx)
{
	esvg_animate_base_animation_generate(t, values, times, actx, abctx, ETCH_DOUBLE,
			_esvg_animate_number_etch_data_to, _esvg_animate_number_cb, NULL);
	return EINA_TRUE;
}

static Esvg_Animate_Base_Type_Descriptor _number_descriptor = {
	/* .value_get 		= */ _esvg_animate_number_get,
	/* .value_free 		= */ free,
	/* .animation_generate 	= */ _esvg_animate_number_animation_generate,
};
/*----------------------------------------------------------------------------*
 *                      The string type descriptor                            *
 *----------------------------------------------------------------------------*/
static void _esvg_animate_string_cb(Edom_Tag *t,
		Ender_Element *e,
		Ender_Property *p,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *kdata,
		void *data)
{
	Esvg_Animated_String v;

	v.base = curr->data.string;
	ender_element_property_value_set(e, p, &v, NULL);
}

static Eina_Bool _esvg_animate_string_get(const char *attr, void **value)
{
	char *v;

	v = strdup(attr);
	*value = v;
	return EINA_TRUE;
}

static void _esvg_animate_string_etch_data_to(void *d,
		Etch_Data *data)
{
	char *v = d;

	data->type = ETCH_STRING;
	data->data.string = v;
}

static Eina_Bool _esvg_animate_string_animation_generate(Edom_Tag *t,
		Eina_List *values,
		Eina_List *times,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx)
{
	esvg_animate_base_animation_generate(t, values, times, actx, abctx, ETCH_STRING,
			_esvg_animate_string_etch_data_to, _esvg_animate_string_cb, NULL);
	return EINA_TRUE;
}

static Esvg_Animate_Base_Type_Descriptor _string_descriptor = {
	/* .value_get 		= */ _esvg_animate_string_get,
	/* .value_free 		= */ free,
	/* .animation_generate 	= */ _esvg_animate_string_animation_generate,
};
/*----------------------------------------------------------------------------*
 *                   The path command type descriptor                         *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_animate_path_command_get(const char *attr, void **value)
{
	char *v;

	v = strdup(attr);
	//esvg_path_string_from();
	*value = v;
	return EINA_TRUE;
}

static void _esvg_animate_path_command_etch_data_to(void *d,
		Etch_Data *data)
{
	char *v = d;

	data->type = ETCH_STRING;
	data->data.string = v;
}

static void _esvg_animate_path_command_generate(void)
{

}

static Esvg_Animate_Base_Type_Descriptor _path_command_descriptor = {
	/* .value_get 		= */ _esvg_animate_path_command_get,
	/* .value_free 		= */ free,
	/* .animation_generate 	= */ NULL,
};
/*----------------------------------------------------------------------------*
 *                         The Esvg Element interface                         *
 *----------------------------------------------------------------------------*/
static void _esvg_animate_free(Edom_Tag *t)
{
	Esvg_Animate *thiz;

	thiz = _esvg_animate_get(t);
	free(thiz);
}

static Eina_Bool _esvg_animate_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	return EINA_TRUE;
}

static Eina_Bool _esvg_animate_attribute_get(Edom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static Eina_Bool _esvg_animate_type_descriptor_get(Edom_Tag *t,
		const char *name, Esvg_Animate_Base_Type_Descriptor **d)
{
	/* based on the property name get the correct descriptor */
	if (!strcmp(name, "esvg_animated_length") || !strcmp(name, "esvg_animated_coord"))
	{
		*d = &_length_descriptor;
	}
	else if (!strcmp(name, "esvg_animated_number"))
	{
		*d = &_number_descriptor;
	}
	else if (!strcmp(name, "esvg_animated_string"))
	{
		*d = &_string_descriptor;
	}
	else if (!strcmp(name, "esvg_animated_path_command"))
	{
		*d = &_path_command_descriptor;
	}
	else
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

static Esvg_Animate_Base_Descriptor _descriptor = {
	/* .attribute_get 	= */ _esvg_animate_attribute_get,
	/* .free 		= */ _esvg_animate_free,
	/* .initialize 		= */ NULL,
	/* .attribute_set 	= */ _esvg_animate_attribute_set,
	/* .type_descriptor_get	= */ _esvg_animate_type_descriptor_get,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_animate_new(void)
{
	Esvg_Animate *thiz;
	Edom_Tag *t;

	thiz = calloc(1, sizeof(Esvg_Animate));

	t = esvg_animate_base_new(&_descriptor, ESVG_ANIMATE, thiz);
	return t;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* The ender wrapper */
#define _esvg_animate_delete NULL
#include "generated/esvg_generated_animate.c"
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Element * esvg_animate_new(void)
{
	return ESVG_ELEMENT_NEW("animate");
}

