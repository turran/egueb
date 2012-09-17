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

#include "esvg_animate_transform.h"
/* This file handles the 'animateTransform' tag. The idea
 * is that you can animate transformations by animating
 * the numbers that define a transformation and its type
 * For example you can animate a rotate from "0 60 60" to "360 60 60"
 * the syntax of the value is relative to the transform type
 * and uses the same format as the transform attribute
 *
 * From the svg spec:
 * from-to animation
 * Specifying a from value and a to value defines a simple
 * animation, equivalent to a values list with 2 values.
 * The animation function is defined to start with the from value,
 * and to finish with the to value.
 *
 * from-by animation
 * Specifying a from value and a by value defines a simple animation
 * in which the animation function is defined to start with the from value,
 * and to change this over the course of the simple duration by a delta
 * specified with the by attribute. This may only be used with attributes
 * that support addition (e.g. most numeric attributes).
 *
 * by animation
 * Specifying only a by value defines a simple animation in which the 
 * animation function is defined to offset the underlying value for the 
 * attribute, using a delta that varies over the course of the simple 
 * duration, starting from a delta of 0 and ending with the delta specified
 * with the by attribute. This may only be used with attributes that support addition.
 *
 * to animation
 * This describes an animation in which the animation function is defined
 * to start with the underlying value for the attribute, and finish with 
 * the value specified with the to attribute. Using this form, an author
 * can describe an animation that will start with any current value for the
 * attribute, and will end up at the desired to value.
 *
 *
 * So at the end, in case we dont have a 'from', the animation should
 * fetch the 'from' from the current attribute but only whenever the animation
 * starts, by default we put an "empty" from that on the animation start
 * it must be changed to the current value
 * If we have a 'values' attribute set, everything else must be ignored
 *
 * TODO handle:
 * accumulate=none|sum (sum based on the previous iteration)
 * additive=none|sum (add to the underlying value of the attribute and other
 * lower priority animations)
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT esvg_log_animate_transform

static Ender_Property *ESVG_ANIMATE_TRANSFORM_TYPE;

typedef struct _Esvg_Animate_Transform_Times_Data
{
	Eina_List *times;
	int64_t duration;
} Esvg_Animate_Transform_Times_Data;

typedef union _Esvg_Animate_Transform_Data
{
	struct {
		double angle;
		double cx;
		double cy;
		int count;
		int current;
	} rotate;

	struct {
		double sx;
		double sy;
		int count;
		int current;
	} scale;

	struct {
		double tx;
		double ty;
		int count;
		int current;
	} translate;

	struct {
		double angle;
	} skew;
} Esvg_Animate_Transform_Data;

typedef struct _Esvg_Animate_Transform
{
	/* properties */
	Esvg_Animate_Transform_Type type;
	/* interface */
	/* private */
	Esvg_Animate_Transform_Data data;
	Ender_Property *prop;
	Ender_Element *parent_e;
	Edom_Tag *parent_t;
	/* needed when generating the values */
	Eina_List *animations;
} Esvg_Animate_Transform;

typedef Eina_Bool (*Esvg_Animate_Transform_Etch_Setup)(
		Esvg_Animate_Transform *thiz,
		Etch *e,
		Eina_List *values,
		Eina_List *times);

static Esvg_Animate_Transform * _esvg_animate_transform_get(Edom_Tag *t)
{
	Esvg_Animate_Transform *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_ANIMATETRANSFORM)
		return NULL;
	thiz = esvg_animate_base_data_get(t);

	return thiz;
}

/* whenever the list values function is called, store the value
 * in our own list of values
 */
static Eina_Bool _esvg_animate_transform_values_cb(double v, void *data)
{
	Eina_List **l = data;
	double *d;

	d = malloc(sizeof(double));
	*d = v;

	*l = eina_list_append(*l, d);
	return EINA_TRUE;
}

static void * _esvg_animate_transform_value_get(const char *attr)
{	
	Eina_List *v = NULL;
	esvg_number_list_string_from(attr, _esvg_animate_transform_values_cb,
					&v);
	return v;
}

static void _esvg_animate_transform_value_free(void *v)
{
	double *d;

	EINA_LIST_FREE(v, d)
		free(d);
}
/*----------------------------------------------------------------------------*
 *                         The Etch animator callbacks                        *
 *----------------------------------------------------------------------------*/
static void _esvg_animate_transform_rotate_angle_cb(Etch_Animation_Keyframe *k,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *data)
{
	Esvg_Animate_Transform *thiz = data;
	Esvg_Animated_Transform v;
	Enesim_Matrix m1;
	Enesim_Matrix m2;

	thiz->data.rotate.angle = curr->data.d;

	enesim_matrix_translate(&m2, thiz->data.rotate.cx, thiz->data.rotate.cy);
	enesim_matrix_rotate(&m1, thiz->data.rotate.angle * M_PI / 180.0);
	enesim_matrix_compose(&m2, &m1, &m1);

	enesim_matrix_translate(&m2, -thiz->data.rotate.cx, -thiz->data.rotate.cy);
	enesim_matrix_compose(&m1, &m2, &m1);
	v.base = m1;

	esvg_element_attribute_animate_set(thiz->parent_t, EINA_TRUE);
	ender_element_property_value_set(thiz->parent_e, thiz->prop, &v, NULL);
	esvg_element_attribute_animate_set(thiz->parent_t, EINA_FALSE);
}

static void _esvg_animate_transform_rotate_cx_cb(Etch_Animation_Keyframe *k,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *data)
{
	Esvg_Animate_Transform *thiz = data;

	thiz->data.rotate.cx = curr->data.d;
}

static void _esvg_animate_transform_rotate_cy_cb(Etch_Animation_Keyframe *k,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *data)
{
	Esvg_Animate_Transform *thiz = data;

	thiz->data.rotate.cy = curr->data.d;
}

static void _esvg_animate_transform_translate_tx_cb(Etch_Animation_Keyframe *k,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *data)
{
	Esvg_Animate_Transform *thiz = data;
	Esvg_Animated_Transform v;
	Enesim_Matrix m;

	thiz->data.translate.tx = curr->data.d;
	enesim_matrix_translate(&m, thiz->data.translate.tx, thiz->data.translate.ty);
	v.base = m;

	esvg_element_attribute_animate_set(thiz->parent_t, EINA_TRUE);
	ender_element_property_value_set(thiz->parent_e, thiz->prop, &v, NULL);
	esvg_element_attribute_animate_set(thiz->parent_t, EINA_FALSE);
}

static void _esvg_animate_transform_translate_ty_cb(Etch_Animation_Keyframe *k,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *data)
{
	Esvg_Animate_Transform *thiz = data;

	thiz->data.translate.ty = curr->data.d;
}

static void _esvg_animate_transform_scale_simple_cb(Etch_Animation_Keyframe *k,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *data)
{
	Esvg_Animate_Transform *thiz = data;
	Esvg_Animated_Transform v;
	Enesim_Matrix m;

	thiz->data.scale.sx = curr->data.d;
	enesim_matrix_scale(&m, thiz->data.scale.sx, thiz->data.scale.sx);
	v.base = m;

	esvg_element_attribute_animate_set(thiz->parent_t, EINA_TRUE);
	ender_element_property_value_set(thiz->parent_e, thiz->prop, &v, NULL);
	esvg_element_attribute_animate_set(thiz->parent_t, EINA_FALSE);
}

static void _esvg_animate_transform_scale_full_sx_cb(Etch_Animation_Keyframe *k,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *data)
{
	Esvg_Animate_Transform *thiz = data;
	Esvg_Animated_Transform v;
	Enesim_Matrix m;

	thiz->data.scale.sx = curr->data.d;
	enesim_matrix_scale(&m, thiz->data.scale.sx, thiz->data.scale.sy);
	v.base = m;

	esvg_element_attribute_animate_set(thiz->parent_t, EINA_TRUE);
	ender_element_property_value_set(thiz->parent_e, thiz->prop, &v, NULL);
	esvg_element_attribute_animate_set(thiz->parent_t, EINA_FALSE);
}

static void _esvg_animate_transform_scale_full_sy_cb(Etch_Animation_Keyframe *k,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *data)
{
	Esvg_Animate_Transform *thiz = data;

	thiz->data.scale.sy = curr->data.d;
}

static void _esvg_animate_transform_skewx_cb(Etch_Animation_Keyframe *k,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *data)
{
	Esvg_Animate_Transform *thiz = data;
	Esvg_Animated_Transform v;
	Enesim_Matrix m;
	double angle;

	angle = curr->data.d;
	enesim_matrix_values_set(&m, 1, tan(angle * M_PI / 180.0), 0, 0, 1, 0, 0, 0, 1);
	v.base = m;

	esvg_element_attribute_animate_set(thiz->parent_t, EINA_TRUE);
	ender_element_property_value_set(thiz->parent_e, thiz->prop, &v, NULL);
	esvg_element_attribute_animate_set(thiz->parent_t, EINA_FALSE);
}

static void _esvg_animate_transform_skewy_cb(Etch_Animation_Keyframe *k,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *data)
{
	Esvg_Animate_Transform *thiz = data;
	Esvg_Animated_Transform v;
	Enesim_Matrix m;
	double angle;

	angle = curr->data.d;
	enesim_matrix_values_set(&m, 1, 0, 0, tan(angle * M_PI / 180.0), 1, 0, 0, 0, 1);
	v.base = m;

	esvg_element_attribute_animate_set(thiz->parent_t, EINA_TRUE);
	ender_element_property_value_set(thiz->parent_e, thiz->prop, &v, NULL);
	esvg_element_attribute_animate_set(thiz->parent_t, EINA_FALSE);
}
/*----------------------------------------------------------------------------*
 *                             The setup variants                             *
 *----------------------------------------------------------------------------*/
static void _esvg_animate_transform_set_time_and_value(
		Esvg_Animate_Transform *thiz,
		Eina_List *times,
		Eina_List *values)
{
	Eina_List *tt;
	Eina_List *aa;
	Etch_Animation *a;
	Eina_List *l;
	Eina_List *v;

	tt = times;

	/* add the values to each animation */
	EINA_LIST_FOREACH (values, l, v)
	{
		Eina_List *ll;
		int64_t *time;
		double *vv;

		time = eina_list_data_get(tt);
		tt = eina_list_next(tt);

		aa = thiz->animations;
		EINA_LIST_FOREACH(v, ll, vv)
		{
			Etch_Animation_Keyframe *k;
			Etch_Data edata;

			a = eina_list_data_get(aa);
			aa = eina_list_next(aa);

			k = etch_animation_keyframe_add(a);
			edata.data.d = *vv;
			edata.type = ETCH_DOUBLE;
			etch_animation_keyframe_type_set(k, ETCH_ANIMATION_LINEAR);
			etch_animation_keyframe_value_set(k, &edata);
			etch_animation_keyframe_time_set(k, *time);
		}
	}
	/* enable each animation */
	EINA_LIST_FOREACH (thiz->animations, l, a)
	{
		etch_animation_enable(a);
	}
}

static void _esvg_animate_transform_simple(Esvg_Animate_Transform *thiz,
		Etch *e,
		Eina_List *times,
		Eina_List *values,
		Etch_Animation_Callback cb)
{
	Etch_Animation *a;

	a = etch_animation_add(e, ETCH_DOUBLE, cb,
				NULL, NULL, thiz);
	thiz->animations = eina_list_append(thiz->animations, a);
	_esvg_animate_transform_set_time_and_value(thiz, times, values);
}

/* on the rotate case we need to add the variant of one or three arguments
 * if only one, then there's no need to move the origin
 * if three, then we need to use the origin and count the times that the
 * animation callback is being called, on the third one, set the value
 */
static Eina_Bool _esvg_animate_transform_rotate(Esvg_Animate_Transform *thiz, Etch *e,
		Eina_List *values, Eina_List *times)
{
	Eina_Bool simple = EINA_TRUE;
	Eina_List *l;
	Eina_List *v;

	/* check if we should use the simple version */
	EINA_LIST_FOREACH (values, l, v)
	{
		if (eina_list_count(v) > 1)
		{
			simple = EINA_FALSE;
			break;
		}
	}
	/* create the needed animations */
	if (simple)
	{
		_esvg_animate_transform_simple(thiz, e, times, values,
				_esvg_animate_transform_rotate_angle_cb);
	}
	else
	{
		Etch_Animation *angle;
		Etch_Animation *cx;
		Etch_Animation *cy;

		angle = etch_animation_add(e, ETCH_DOUBLE, _esvg_animate_transform_rotate_angle_cb,
					NULL, NULL, thiz);
		thiz->animations = eina_list_append(thiz->animations, angle);
		/* generate three animations per each value
		 * one for the angle, and two for the origin
		 */
		cx = etch_animation_add(e, ETCH_DOUBLE, _esvg_animate_transform_rotate_cx_cb,
					NULL, NULL, thiz);
		thiz->animations = eina_list_append(thiz->animations, cx);
		cy = etch_animation_add(e, ETCH_DOUBLE, _esvg_animate_transform_rotate_cy_cb,
					NULL, NULL, thiz);
		thiz->animations = eina_list_append(thiz->animations, cy);

		_esvg_animate_transform_set_time_and_value(thiz, times, values);
	}
	return EINA_TRUE;
}

static Eina_Bool _esvg_animate_transform_translate(Esvg_Animate_Transform *thiz, Etch *e,
		Eina_List *values, Eina_List *times)
{
	Eina_Bool simple = EINA_TRUE;
	Eina_List *l;
	Eina_List *v;

	/* check if we should use the simple version */
	EINA_LIST_FOREACH (values, l, v)
	{
		if (eina_list_count(v) > 1)
		{
			simple = EINA_FALSE;
			break;
		}
	}
	/* create the needed animations */
	if (simple)
	{
		_esvg_animate_transform_simple(thiz, e, times, values,
				_esvg_animate_transform_translate_tx_cb);
	}
	else
	{
		Etch_Animation *tx;
		Etch_Animation *ty;

		/* generate three animations per each value
		 * one for the tx, and two for the origin
		 */
		tx = etch_animation_add(e, ETCH_DOUBLE, _esvg_animate_transform_translate_tx_cb,
					NULL, NULL, thiz);
		thiz->animations = eina_list_append(thiz->animations, tx);
		ty = etch_animation_add(e, ETCH_DOUBLE, _esvg_animate_transform_translate_ty_cb,
					NULL, NULL, thiz);
		thiz->animations = eina_list_append(thiz->animations, ty);
		_esvg_animate_transform_set_time_and_value(thiz, times, values);
	}
	return EINA_TRUE;

}

static Eina_Bool _esvg_animate_transform_scale(Esvg_Animate_Transform *thiz, Etch *e,
		Eina_List *values, Eina_List *times)
{
	Eina_Bool simple = EINA_TRUE;
	Eina_List *l;
	Eina_List *v;

	/* check if we should use the simple version */
	EINA_LIST_FOREACH (values, l, v)
	{
		if (eina_list_count(v) > 1)
		{
			simple = EINA_FALSE;
			break;
		}
	}
	/* create the needed animations */
	if (simple)
	{
		_esvg_animate_transform_simple(thiz, e, times, values,
				_esvg_animate_transform_scale_simple_cb);
	}
	else
	{
		Etch_Animation *sx;
		Etch_Animation *sy;

		/* generate three animations per each value
		 * one for the sx, and two for the origin
		 */
		sx = etch_animation_add(e, ETCH_DOUBLE, _esvg_animate_transform_scale_full_sx_cb,
					NULL, NULL, thiz);
		thiz->animations = eina_list_append(thiz->animations, sx);
		sy = etch_animation_add(e, ETCH_DOUBLE, _esvg_animate_transform_scale_full_sy_cb,
					NULL, NULL, thiz);
		thiz->animations = eina_list_append(thiz->animations, sy);
		_esvg_animate_transform_set_time_and_value(thiz, times, values);
	}
	return EINA_TRUE;
}

static Eina_Bool _esvg_animate_transform_skewx(Esvg_Animate_Transform *thiz, Etch *e,
		Eina_List *values, Eina_List *times)
{
	Etch_Animation *skewx;

	skewx = etch_animation_add(e, ETCH_DOUBLE, _esvg_animate_transform_skewx_cb,
				NULL, NULL, thiz);
	thiz->animations = eina_list_append(thiz->animations, skewx);
	_esvg_animate_transform_set_time_and_value(thiz, times, values);

	return EINA_TRUE;
}

static Eina_Bool _esvg_animate_transform_skewy(Esvg_Animate_Transform *thiz, Etch *e,
		Eina_List *values, Eina_List *times)
{
	Etch_Animation *skewy;

	skewy = etch_animation_add(e, ETCH_DOUBLE, _esvg_animate_transform_skewy_cb,
				NULL, NULL, thiz);
	thiz->animations = eina_list_append(thiz->animations, skewy);
	_esvg_animate_transform_set_time_and_value(thiz, times, values);

	return EINA_FALSE;
}

static Eina_Bool _esvg_animate_transform_container_etch_to(Esvg_Animate_Transform *thiz,
		Etch *etch,
		Ender_Property *p,
		Esvg_Animation_Context *ac,
		Esvg_Animate_Base_Context *c)
{
	Esvg_Animate_Transform_Etch_Setup setups[ESVG_ANIMATE_TRANSFORM_TYPES]  = {
		_esvg_animate_transform_translate,
		_esvg_animate_transform_scale,
		_esvg_animate_transform_rotate,
		_esvg_animate_transform_skewx,
		_esvg_animate_transform_skewy
	};
	Esvg_Animate_Transform_Etch_Setup setup;
	Ender_Container *ec;
	Etch_Animation *anim;
	Eina_List *values = NULL;
	Eina_List *times = NULL;
	Eina_List *l;
	Eina_Bool has_from;
	const char *name;

	ec = ender_property_container_get(p);
	name = ender_container_registered_name_get(ec);

	if (strcmp(name, "esvg_animated_transform"))
		return EINA_FALSE;
	/* get the type that we will animate: rotate, scale, etc */
	setup = setups[thiz->type];

	esvg_animate_base_values_generate(c, _esvg_animate_transform_value_get,
			&values, &has_from);
	esvg_animate_base_times_generate(ac, c, values, &times);

	if (values && times)
	{
		setup(thiz, etch, values, times);
	}
	/* for each animation, set the repeat times */
	EINA_LIST_FOREACH (thiz->animations, l, anim)
	{
		etch_animation_repeat_set(anim, ac->timing.repeat_count);
	}

	esvg_animate_base_values_free(values, _esvg_animate_transform_value_free);
	esvg_animate_base_times_free(times);

	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                         The Esvg Element interface                         *
 *----------------------------------------------------------------------------*/
static void _esvg_animate_transform_free(Edom_Tag *t)
{
	Esvg_Animate_Transform *thiz;

	thiz = _esvg_animate_transform_get(t);
	free(thiz);
}

static Eina_Bool _esvg_animate_transform_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (!strcmp(key, "type"))
	{
		Esvg_Animate_Transform_Type type;

		esvg_animate_transform_type_string_from(&type, value);
		esvg_animate_transform_type_set(e, type);
		return EINA_TRUE;
	}
	return EINA_FALSE;
}

static Eina_Bool _esvg_animate_transform_attribute_get(Edom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static Eina_Bool _esvg_animate_transform_setup(Edom_Tag *t,
		Esvg_Context *c,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx,
		Enesim_Error **error)
{
	Esvg_Animate_Transform *thiz;
	Ender_Element *svg_e;
	Ender_Element *parent_e;
	Edom_Tag *parent_t;
	Ender_Property *p;
	Etch *etch;
	Etch_Animation *anim;

	thiz = _esvg_animate_transform_get(t);

	/* same */
	esvg_element_internal_topmost_get(t, &svg_e);
	if (!svg_e) goto done;

	etch = esvg_svg_etch_get(svg_e);
	if (!etch) goto done;

	/* FIXME should we get a reference here ? */
	p = actx->p;
	thiz->parent_t = actx->parent_t;
	thiz->parent_e = actx->parent_e;

	EINA_LIST_FREE (thiz->animations, anim)
	{
		etch_animation_remove(etch, anim);
	}

	/* we should only process lengths, colors, integers, booleans, etc */
	if (!_esvg_animate_transform_container_etch_to(thiz, etch, p, actx, abctx))
		goto done;
	thiz->prop = p;
done:

	return EINA_TRUE;
}

static Esvg_Animate_Base_Descriptor _descriptor = {
	/* .attribute_get 	= */ _esvg_animate_transform_attribute_get,
	/* .free 		= */ _esvg_animate_transform_free,
	/* .initialize 		= */ NULL,
	/* .attribute_set 	= */ _esvg_animate_transform_attribute_set,
	/* .setup		= */ _esvg_animate_transform_setup,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static void _esvg_animate_transform_type_set(Edom_Tag *t, Esvg_Animate_Transform_Type type)
{
	Esvg_Animate_Transform *thiz;

	thiz = _esvg_animate_transform_get(t);
	thiz->type = type;
}

static void _esvg_animate_transform_type_get(Edom_Tag *t, Esvg_Animate_Transform_Type *type)
{
	Esvg_Animate_Transform *thiz;

	if (!type) return;
	thiz = _esvg_animate_transform_get(t);
	*type = thiz->type;
}

static Edom_Tag * _esvg_animate_transform_new(void)
{
	Esvg_Animate_Transform *thiz;
	Edom_Tag *t;

	thiz = calloc(1, sizeof(Esvg_Animate_Transform));

	t = esvg_animate_base_new(&_descriptor, ESVG_ANIMATETRANSFORM, thiz);
	return t;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* The ender wrapper */
#define _esvg_animate_transform_delete NULL
#define _esvg_animate_transform_type_is_set NULL
#include "generated/esvg_generated_animate_transform.c"
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Element * esvg_animate_transform_new(void)
{
	return ESVG_ELEMENT_NEW("animate_transform");
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animate_transform_type_set(Ender_Element *e, Esvg_Animate_Transform_Type type)
{
	ender_element_property_value_set(e, ESVG_ANIMATE_TRANSFORM_TYPE, type, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animate_transform_type_get(Ender_Element *e, Esvg_Animate_Transform_Type *type)
{
}


