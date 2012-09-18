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
typedef void (*Esvg_Animate_Keyframe_Value_Cb)(const char *v,
		Etch_Animation_Keyframe *k, Etch_Data *data);
typedef void (*Esvg_Animate_Value_Etch_Data_To)(void *d,
		Etch_Data *data);

typedef struct _Esvg_Animate
{
	/* properties */
	/* interface */
	/* private */
	Etch_Animation *anim;
	Eina_List *values;
	Ender_Property *prop;
	Ender_Element *parent_e;
	Edom_Tag *parent_t;
	Esvg_Attribute_Type attribute_type;
	/* new */
	Eina_List *animations;
} Esvg_Animate;

typedef struct _Esvg_Animate_Keyframe_Value_Cb_Data
{
	Esvg_Animate *thiz;
	Etch_Animation_Type type;
	Esvg_Animate_Keyframe_Value_Cb cb;
	const char *attr;
} Esvg_Animate_Keyframe_Value_Cb_Data;

typedef struct _Esvg_Animate_Keyframe_Time_Cb_Data
{
	Esvg_Animate *thiz;
	int idx;
	Etch_Time time;
	Etch_Time inc;
} Esvg_Animate_Keyframe_Time_Cb_Data;

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
 *                          The Esvg_Length callbacks                         *
 *----------------------------------------------------------------------------*/
static void _esvg_animate_length_cb(Etch_Animation_Keyframe *k,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *data)
{
	Esvg_Animate *thiz = data;
	Esvg_Animated_Length v;
	Esvg_Attribute_Type old_type;
	Esvg_Length *length;

	length = etch_animation_keyframe_data_get(k);

	v.base.value = curr->data.d;
	v.base.unit = length->unit;

	old_type = esvg_element_attribute_type_get(thiz->parent_t);
	esvg_element_attribute_type_set(thiz->parent_t, thiz->attribute_type);
	ender_element_property_value_set(thiz->parent_e, thiz->prop, &v, NULL);
	esvg_element_attribute_type_set(thiz->parent_t, old_type);
}

static void * _esvg_animate_length_get(const char *attr)
{
	Esvg_Length *v;

	v = calloc(1, sizeof(Esvg_Length));
	esvg_length_string_from(v, attr);
	return v;
}

static void _esvg_animate_length_etch_data_to(void *d,
		Etch_Data *data)
{
	Esvg_Length *v = d;

	data->type = ETCH_DOUBLE;
	data->data.d = v->value;
}
/*----------------------------------------------------------------------------*
 *                            The double callbacks                            *
 *----------------------------------------------------------------------------*/
static void _esvg_animate_double_cb(Etch_Animation_Keyframe *k,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *data)
{
	Esvg_Animate *thiz = data;
	Esvg_Animated_Number v;
	Esvg_Attribute_Type old_type;


	v.base = curr->data.d;

	old_type = esvg_element_attribute_type_get(thiz->parent_t);
	esvg_element_attribute_type_set(thiz->parent_t, thiz->attribute_type);
	esvg_element_attribute_animate_set(thiz->parent_t, EINA_TRUE);
	ender_element_property_value_set(thiz->parent_e, thiz->prop, &v, NULL);
	esvg_element_attribute_animate_set(thiz->parent_t, EINA_FALSE);
	esvg_element_attribute_type_set(thiz->parent_t, old_type);
}

static void * _esvg_animate_number_get(const char *attr)
{
	double *v;

	v = calloc(1, sizeof(double));
	*v = esvg_number_string_from(attr, 1.0);
	return v;
}

static void _esvg_animate_number_etch_data_to(void *d,
		Etch_Data *data)
{
	double *v = d;

	data->type = ETCH_DOUBLE;
	data->data.d = *v;
}

/*----------------------------------------------------------------------------*
 *                            The string callbacks                            *
 *----------------------------------------------------------------------------*/
static void _esvg_animate_string_cb(Etch_Animation_Keyframe *k,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *data)
{
	Esvg_Animate *thiz = data;
	Esvg_Animated_String v;
	Esvg_Attribute_Type old_type;


	v.base = curr->data.string;
	printf("setting string! %s\n", v.base);
	old_type = esvg_element_attribute_type_get(thiz->parent_t);
	esvg_element_attribute_type_set(thiz->parent_t, thiz->attribute_type);
	esvg_element_attribute_animate_set(thiz->parent_t, EINA_TRUE);
	ender_element_property_value_set(thiz->parent_e, thiz->prop, &v, NULL);
	esvg_element_attribute_animate_set(thiz->parent_t, EINA_FALSE);
	esvg_element_attribute_type_set(thiz->parent_t, old_type);
}

static void * _esvg_animate_string_get(const char *attr)
{
	char *v;

	v = strdup(attr);
	return v;
}

static void _esvg_animate_string_etch_data_to(void *d,
		Etch_Data *data)
{
	char *v = d;

	data->type = ETCH_STRING;
	data->data.string = v;
}
/*----------------------------------------------------------------------------*
 *                              The Etch helpers                              *
 *----------------------------------------------------------------------------*/
static void _esvg_animate_values_cb(const char *v, void *user_data)
{
	Esvg_Animate_Keyframe_Value_Cb_Data *data = user_data;
	Etch_Animation_Keyframe *kf;
	Etch_Data edata;

	kf = etch_animation_keyframe_add(data->thiz->anim);
	etch_animation_keyframe_type_set(kf, data->type);
	data->cb(v, kf, &edata);
	etch_animation_keyframe_value_set(kf, &edata);
}

/* increment each keyframe */
static void _esvg_animate_time_cb(const char *v, void *user_data)
{
	Esvg_Animate_Keyframe_Time_Cb_Data *data = user_data;
	Etch_Animation_Keyframe *kf;

	kf = etch_animation_keyframe_get(data->thiz->anim, data->idx);
	etch_animation_keyframe_time_set(kf, data->time);
	data->time += data->inc;
	data->idx++;
}

static void _esvg_animate_key_times_cb(const char *v, void *user_data)
{
	Esvg_Animate_Keyframe_Time_Cb_Data *data = user_data;
	Etch_Animation_Keyframe *kf;
	double percent;

	percent = esvg_number_string_from(v, 1.0);
	kf = etch_animation_keyframe_get(data->thiz->anim, data->idx);
	etch_animation_keyframe_time_set(kf, data->time * percent);
	data->idx++;
}

static void _esvg_animate_key_splines_cb(const char *v, void *user_data)
{
	Etch_Data ndata;
	Etch_Data cdata;
	Etch_Data cp0;
	Etch_Data cp1;

	/* iterate over the attribute by either space or commas */
	/* get the current keyframe and the next */
	/* the range is from 0 to 1, so we need to multiply the value with that
	 * factor and set the value on the cubic argument */
}

static Eina_Bool _esvg_animate_container_etch_to(Esvg_Animate *thiz, Etch *etch,
		Ender_Property *p,
		Esvg_Animation_Context *ac,
		Esvg_Animate_Base_Context *c)
{
	Esvg_Animate_Base_Value_Get value_get = NULL;
	Esvg_Animate_Value_Etch_Data_To data_to = NULL;
	Ender_Container *ec;
	Etch_Data_Type dt;
	Etch_Animation_Callback cb;
	Eina_List *times = NULL;
	Eina_List *values = NULL;
	Eina_Bool has_from;
	const char *name;

	ec = ender_property_container_get(p);
	name = ender_container_registered_name_get(ec);
	if (!strcmp(name, "esvg_animated_length"))
	{
		dt = ETCH_DOUBLE;
		cb = _esvg_animate_length_cb;
		value_get = _esvg_animate_length_get;
		data_to = _esvg_animate_length_etch_data_to;
	}
	else if (!strcmp(name, "esvg_animated_number"))
	{
		dt = ETCH_DOUBLE;
		cb = _esvg_animate_double_cb;
		value_get = _esvg_animate_number_get;
		data_to = _esvg_animate_number_etch_data_to;
	}
	else if (!strcmp(name, "esvg_animated_string"))
	{
		dt = ETCH_STRING;
		cb = _esvg_animate_string_cb;
		value_get = _esvg_animate_string_get;
		data_to = _esvg_animate_string_etch_data_to;
	}
	else
	{
		return EINA_FALSE;
	}
	thiz->prop = p;
	esvg_animate_base_values_generate(c, value_get,
			&values, &has_from);
	esvg_animate_base_times_generate(ac, c, values, &times);

	if (values && times)
	{
		Etch_Animation *a;
		Eina_List *tt;
		Eina_List *l;
		void *v;
		int64_t *time;

		a = etch_animation_add(etch, dt, cb,
					NULL, NULL, thiz);
		tt = times;
		EINA_LIST_FOREACH(values, l, v)
		{
			Etch_Animation_Keyframe *k;
			Etch_Data edata;

			time = eina_list_data_get(tt);

			/* add a keyframe */
			k = etch_animation_keyframe_add(a);
			//etch_animation_keyframe_type_set(k, c->value.calc_mode);
			//etch_animation_keyframe_type_set(k, ETCH_ANIMATION_LINEAR);
			etch_animation_keyframe_type_set(k, ETCH_ANIMATION_DISCRETE);
			/* convert it to the destination etch type */
			data_to(v, &edata);
			/* set the value */
			etch_animation_keyframe_data_set(k, v, NULL);
			etch_animation_keyframe_value_set(k, &edata);
			// set the time
			etch_animation_keyframe_time_set(k, *time);

			tt = eina_list_next(tt);
		}
		etch_animation_enable(a);
	}
	thiz->values = values;
	//esvg_animate_base_values_free(values, free);
	//esvg_animate_base_times_free(times);

	return EINA_TRUE;
}
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

static Eina_Bool _esvg_animate_setup(Edom_Tag *t,
		Esvg_Context *c,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx,
		Enesim_Error **error)
{
	Esvg_Animate *thiz;
	Ender_Element *svg_e;
	Ender_Property *p;
	Etch *etch;

	thiz = _esvg_animate_get(t);

	/* same */
	esvg_element_internal_topmost_get(t, &svg_e);
	if (!svg_e) goto done;

	etch = esvg_svg_etch_get(svg_e);
	if (!etch) goto done;

	if (thiz->anim)
		etch_animation_delete(thiz->anim);

	/* FIXME should we get a reference here ? */
	p = actx->p;
	thiz->parent_e = actx->parent_e;
	thiz->parent_t = actx->parent_t;
	thiz->attribute_type = actx->target.attribute_type;

	/* we should only process lengths, colors, integers, booleans, etc */
	if (!_esvg_animate_container_etch_to(thiz, etch, p, actx, abctx))
		goto done;
	/* check the type and create an animator of that container type */
	/* on every animation callback set the animation mode on the element */
	/* then call the property set */
	/* restore the mode */

done:

	return EINA_TRUE;
}

static Esvg_Animate_Base_Descriptor _descriptor = {
	/* .attribute_get 	= */ _esvg_animate_attribute_get,
	/* .free 		= */ _esvg_animate_free,
	/* .initialize 		= */ NULL,
	/* .attribute_set 	= */ _esvg_animate_attribute_set,
	/* .setup		= */ _esvg_animate_setup,
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

