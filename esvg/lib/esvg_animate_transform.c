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

#if 0
static void _esvg_animate_transform_value_simple_etch_data_to(void *d,
		Etch_Data *data)
{
	Eina_List *l = d;
	double *v;

	v = eina_list_data_get(l);
	data->type = ETCH_DOUBLE;
	data->data.d = *v;
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

static Eina_Bool _esvg_animate_transform_value_get(const char *attr, void **v)
{	
	Eina_List *l = NULL;
	esvg_number_list_string_from(attr, _esvg_animate_transform_values_cb,
					&l);
	*v = l;
	return EINA_TRUE;
}

static void _esvg_animate_transform_value_free(void *v)
{
	double *d;

	EINA_LIST_FREE(v, d)
		free(d);
}

static void _esvg_animate_transform_generate(Edom_Tag *t,
		Etch_Animation **animations,
		Eina_List *values,
		Eina_List *times,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx)
{
	Eina_List *tt;
	Eina_List *l;
	Eina_List *v;

	tt = times;

	/* add the values to each animation */
	EINA_LIST_FOREACH (values, l, v)
	{
		Eina_List *ll;
		int64_t *time;
		double *vv;
		int acount = 0;

		time = eina_list_data_get(tt);
		tt = eina_list_next(tt);

		EINA_LIST_FOREACH(v, ll, vv)
		{
			Etch_Animation *a;
			Etch_Animation_Keyframe *k;
			Etch_Data edata;

			a = animations[acount];

			edata.data.d = *vv;
			edata.type = ETCH_DOUBLE;
			esvg_animate_base_animation_add_keyframe(a, abctx, &edata, *time, NULL);
			acount++;
		}
	}
}
/*----------------------------------------------------------------------------*
 *                        The translate type descriptor                       *
 *----------------------------------------------------------------------------*/
static void _esvg_animate_transform_translate_tx_cb(Edom_Tag *t,
		Ender_Element *e,
		Ender_Property *p,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *kdata,
		void *data)
{
	Esvg_Animate_Transform *thiz = data;
	Esvg_Animated_Transform v;
	Enesim_Matrix m;

	thiz->data.translate.tx = curr->data.d;
	enesim_matrix_translate(&m, thiz->data.translate.tx, thiz->data.translate.ty);
	v.base = m;

	ender_element_property_value_set(e, p, &v, NULL);
}

static void _esvg_animate_transform_translate_ty_cb(Edom_Tag *t,
		Ender_Element *e,
		Ender_Property *p,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *kdata,
		void *data)
{
	Esvg_Animate_Transform *thiz = data;

	thiz->data.translate.ty = curr->data.d;
}

static Eina_Bool _esvg_animate_transform_translate_generate(Edom_Tag *t,
		Eina_List *values,
		Eina_List *times,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx)
{
	Esvg_Animate_Transform *thiz;
	Eina_Bool simple = EINA_TRUE;
	Eina_List *l;
	Eina_List *v;

	thiz = _esvg_animate_transform_get(t);
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
		esvg_animate_base_animation_generate(t, values, times, actx, abctx, ETCH_DOUBLE,
				_esvg_animate_transform_value_simple_etch_data_to,
				_esvg_animate_transform_translate_tx_cb, thiz);
	}
	else
	{
		Etch_Animation *animations[2];
		Etch_Animation *tx;
		Etch_Animation *ty;

		ty = esvg_animate_base_animation_empty_add(t, ETCH_DOUBLE, actx, abctx,
				_esvg_animate_transform_translate_ty_cb, thiz);
		tx = esvg_animate_base_animation_simple_add(t, ETCH_DOUBLE, actx, abctx,
				_esvg_animate_transform_translate_tx_cb, thiz);
		animations[0] = tx;
		animations[1] = ty;
		_esvg_animate_transform_generate(t, animations, values, times, actx, abctx);
	}
	return EINA_TRUE;
}

static Esvg_Animate_Base_Type_Descriptor _translate_descriptor = {
	/* .value_get 		= */ _esvg_animate_transform_value_get,
	/* .value_free 		= */ _esvg_animate_transform_value_free,
	/* .animation_generate 	= */ _esvg_animate_transform_translate_generate,
};

/*----------------------------------------------------------------------------*
 *                          The rotate type descriptor                        *
 *----------------------------------------------------------------------------*/
/* on the rotate case we need to add the variant of one or three arguments
 * if only one, then there's no need to move the origin
 * if three, then we need to use the origin and count the times that the
 * animation callback is being called, on the third one, set the value
 */
static void _esvg_animate_transform_rotate_angle_cb(Edom_Tag *t,
		Ender_Element *e,
		Ender_Property *p,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *kdata,
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

	ender_element_property_value_set(e, p, &v, NULL);
}

static void _esvg_animate_transform_rotate_cx_cb(Edom_Tag *t,
		Ender_Element *e,
		Ender_Property *p,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *kdata,
		void *data)
{
	Esvg_Animate_Transform *thiz = data;

	thiz->data.rotate.cx = curr->data.d;
}

static void _esvg_animate_transform_rotate_cy_cb(Edom_Tag *t,
		Ender_Element *e,
		Ender_Property *p,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *kdata,
		void *data)
{
	Esvg_Animate_Transform *thiz = data;

	thiz->data.rotate.cy = curr->data.d;
}

static Eina_Bool _esvg_animate_transform_rotate_generate(Edom_Tag *t,
		Eina_List *values,
		Eina_List *times,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx)
{
	Esvg_Animate_Transform *thiz;
	Eina_Bool simple = EINA_TRUE;
	Eina_List *l;
	Eina_List *v;

	thiz = _esvg_animate_transform_get(t);
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
		esvg_animate_base_animation_generate(t, values, times, actx, abctx, ETCH_DOUBLE,
				_esvg_animate_transform_value_simple_etch_data_to,
				_esvg_animate_transform_rotate_angle_cb, thiz);
	}
	else
	{
		Etch_Animation *animations[3];
		Etch_Animation *angle;
		Etch_Animation *cx;
		Etch_Animation *cy;

		cx = esvg_animate_base_animation_empty_add(t, ETCH_DOUBLE, actx, abctx,
				_esvg_animate_transform_rotate_cx_cb, thiz);
		cy = esvg_animate_base_animation_empty_add(t, ETCH_DOUBLE, actx, abctx,
				_esvg_animate_transform_rotate_cy_cb, thiz);
		angle = esvg_animate_base_animation_simple_add(t, ETCH_DOUBLE, actx, abctx,
				_esvg_animate_transform_rotate_angle_cb, thiz);
		animations[0] = angle;
		animations[1] = cx;
		animations[2] = cy;
		_esvg_animate_transform_generate(t, animations, values, times, actx, abctx);
	}
	return EINA_TRUE;

}

static Esvg_Animate_Base_Type_Descriptor _rotate_descriptor = {
	/* .value_get 		= */ _esvg_animate_transform_value_get,
	/* .value_free 		= */ _esvg_animate_transform_value_free,
	/* .animation_generate 	= */ _esvg_animate_transform_rotate_generate,
};

/*----------------------------------------------------------------------------*
 *                          The scale type descriptor                         *
 *----------------------------------------------------------------------------*/
static void _esvg_animate_transform_scale_simple_cb(Edom_Tag *t,
		Ender_Element *e,
		Ender_Property *p,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *kdata,
		void *data)
{
	Esvg_Animate_Transform *thiz = data;
	Esvg_Animated_Transform v;
	Enesim_Matrix m;

	thiz->data.scale.sx = curr->data.d;
	enesim_matrix_scale(&m, thiz->data.scale.sx, thiz->data.scale.sx);
	v.base = m;

	ender_element_property_value_set(e, p, &v, NULL);
}

static void _esvg_animate_transform_scale_full_sx_cb(Edom_Tag *t,
		Ender_Element *e,
		Ender_Property *p,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *kdata,
		void *data)
{
	Esvg_Animate_Transform *thiz = data;
	Esvg_Animated_Transform v;
	Enesim_Matrix m;

	thiz->data.scale.sx = curr->data.d;
	enesim_matrix_scale(&m, thiz->data.scale.sx, thiz->data.scale.sy);
	v.base = m;

	ender_element_property_value_set(e, p, &v, NULL);
}

static void _esvg_animate_transform_scale_full_sy_cb(Edom_Tag *t,
		Ender_Element *e,
		Ender_Property *p,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *kdata,
		void *data)
{
	Esvg_Animate_Transform *thiz = data;

	thiz->data.scale.sy = curr->data.d;
}

static Eina_Bool _esvg_animate_transform_scale_generate(Edom_Tag *t,
		Eina_List *values,
		Eina_List *times,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx)
{
	Esvg_Animate_Transform *thiz;
	Eina_Bool simple = EINA_TRUE;
	Eina_List *l;
	Eina_List *v;

	thiz = _esvg_animate_transform_get(t);
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
		esvg_animate_base_animation_generate(t, values, times, actx, abctx, ETCH_DOUBLE,
				_esvg_animate_transform_value_simple_etch_data_to,
				_esvg_animate_transform_scale_simple_cb, thiz);
	}
	else
	{
		Etch_Animation *animations[2];
		Etch_Animation *sx;
		Etch_Animation *sy;

		sy = esvg_animate_base_animation_empty_add(t, ETCH_DOUBLE, actx, abctx,
				_esvg_animate_transform_scale_full_sy_cb, thiz);
		sx = esvg_animate_base_animation_simple_add(t, ETCH_DOUBLE, actx, abctx,
				_esvg_animate_transform_scale_full_sx_cb, thiz);
		animations[0] = sx;
		animations[1] = sy;
		_esvg_animate_transform_generate(t, animations, values, times, actx, abctx);
	}
	return EINA_TRUE;

}

static Esvg_Animate_Base_Type_Descriptor _scale_descriptor = {
	/* .value_get 		= */ _esvg_animate_transform_value_get,
	/* .value_free 		= */ _esvg_animate_transform_value_free,
	/* .animation_generate 	= */ _esvg_animate_transform_scale_generate,
};

/*----------------------------------------------------------------------------*
 *                          The skewx type descriptor                         *
 *----------------------------------------------------------------------------*/
static void _esvg_animate_transform_skewx_cb(Edom_Tag *t,
		Ender_Element *e,
		Ender_Property *p,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *kdata,
		void *data)
{
	Esvg_Animate_Transform *thiz = data;
	Esvg_Animated_Transform v;
	Enesim_Matrix m;
	double angle;

	angle = curr->data.d;
	enesim_matrix_values_set(&m, 1, tan(angle * M_PI / 180.0), 0, 0, 1, 0, 0, 0, 1);
	v.base = m;

	ender_element_property_value_set(e, p, &v, NULL);
}

static Eina_Bool _esvg_animate_transform_skewx_generate(Edom_Tag *t,
		Eina_List *values,
		Eina_List *times,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx)
{
	Esvg_Animate_Transform *thiz;

	thiz = _esvg_animate_transform_get(t);
	esvg_animate_base_animation_generate(t, values, times, actx, abctx, ETCH_DOUBLE,
			_esvg_animate_transform_value_simple_etch_data_to,
			_esvg_animate_transform_skewx_cb, thiz);
	return EINA_TRUE;
}

static Esvg_Animate_Base_Type_Descriptor _skewx_descriptor = {
	/* .value_get 		= */ _esvg_animate_transform_value_get,
	/* .value_free 		= */ _esvg_animate_transform_value_free,
	/* .animation_generate 	= */ _esvg_animate_transform_skewx_generate,
};

/*----------------------------------------------------------------------------*
 *                          The skewy type descriptor                         *
 *----------------------------------------------------------------------------*/
static void _esvg_animate_transform_skewy_cb(Edom_Tag *t,
		Ender_Element *e,
		Ender_Property *p,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *kdata,
		void *data)
{
	Esvg_Animate_Transform *thiz = data;
	Esvg_Animated_Transform v;
	Enesim_Matrix m;
	double angle;

	angle = curr->data.d;
	enesim_matrix_values_set(&m, 1, 0, 0, tan(angle * M_PI / 180.0), 1, 0, 0, 0, 1);
	v.base = m;

	ender_element_property_value_set(e, p, &v, NULL);
}

static Eina_Bool _esvg_animate_transform_skewy_generate(Edom_Tag *t,
		Eina_List *values,
		Eina_List *times,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx)
{
	Esvg_Animate_Transform *thiz;

	thiz = _esvg_animate_transform_get(t);
	esvg_animate_base_animation_generate(t, values, times, actx, abctx, ETCH_DOUBLE,
			_esvg_animate_transform_value_simple_etch_data_to,
			_esvg_animate_transform_skewy_cb, thiz);
	return EINA_TRUE;
}

static Esvg_Animate_Base_Type_Descriptor _skewy_descriptor = {
	/* .value_get 		= */ _esvg_animate_transform_value_get,
	/* .value_free 		= */ _esvg_animate_transform_value_free,
	/* .animation_generate 	= */ _esvg_animate_transform_skewy_generate,
};
#endif
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

static Eina_Bool _esvg_animate_transform_type_descriptor_get(Edom_Tag *t,
		const char *name, Esvg_Animate_Base_Type_Descriptor **d)
{
	Esvg_Animate_Transform *thiz;
	Esvg_Animated_Transform *v;

	/* based on the property name get the correct descriptor */
	if (strcmp(name, "esvg_animated_transform"))
		return EINA_FALSE;
	return EINA_FALSE;
#if 0
	/* check the type and use the correct generator */
	thiz = _esvg_animate_transform_get(t);
	switch (thiz->type)
	{
		case ESVG_ANIMATE_TRANSFORM_TYPE_TRANSLATE:
		*d = &_translate_descriptor;
		break;

		case ESVG_ANIMATE_TRANSFORM_TYPE_SCALE:
		*d = &_scale_descriptor;
		break;

		case ESVG_ANIMATE_TRANSFORM_TYPE_ROTATE:
		*d = &_rotate_descriptor;
		break;

		case ESVG_ANIMATE_TRANSFORM_TYPE_SKEWX:
		*d = &_skewx_descriptor;
		break;

		case ESVG_ANIMATE_TRANSFORM_TYPE_SKEWY:
		*d = &_skewy_descriptor;
		break;

		default:
		return EINA_FALSE;
	}
	v = calloc(1, sizeof(Esvg_Animated_Transform));
	*dst = v;

	return EINA_TRUE;
#endif
}

static Esvg_Animate_Base_Descriptor _descriptor = {
	/* .attribute_get 	= */ _esvg_animate_transform_attribute_get,
	/* .free 		= */ _esvg_animate_transform_free,
	/* .initialize 		= */ NULL,
	/* .attribute_set 	= */ _esvg_animate_transform_attribute_set,
	/* .type_descriptor_get	= */ _esvg_animate_transform_type_descriptor_get,
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


