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
#include "egueb_svg_main.h"

#include "egueb_smil_animation_private.h"
#include "egueb_smil_animate_base_private.h"

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
#define EGUEB_SVG_ELEMENT_ANIMATE_TRANSFORM_DESCRIPTOR egueb_svg_element_animate_transform_descriptor_get()
#define EGUEB_SVG_ELEMENT_ANIMATE_TRANSFORM_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Element_Animate_Transform_Class, EGUEB_SVG_ELEMENT_ANIMATE_TRANSFORM_DESCRIPTOR)
#define EGUEB_SVG_ELEMENT_ANIMATE_TRANSFORM(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Element_Animate_Transform, EGUEB_SVG_ELEMENT_ANIMATE_TRANSFORM_DESCRIPTOR)

typedef struct _Egueb_Svg_Animate_Transform_Data
{
	double values[3];
	int count;
} Egueb_Svg_Animate_Transform_Data;

typedef struct _Egueb_Svg_Element_Animate_Transform
{
	Egueb_Smil_Animate_Base base;
	Egueb_Dom_Node *type;
	/* interface */
	/* private */
} Egueb_Svg_Element_Animate_Transform;

typedef struct _Egueb_Svg_Element_Animate_Transform_Class
{
	Egueb_Smil_Animate_Base_Class base;
} Egueb_Svg_Element_Animate_Transform_Class;

#if 0
static Egueb_Svg_Element_Animate_Transform * _egueb_svg_element_animate_transform_get(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element_Animate_Transform *thiz;

	if (egueb_svg_element_internal_type_get(t) != ESVG_TYPE_ANIMATETRANSFORM)
		return NULL;
	thiz = egueb_svg_animate_base_data_get(t);

	return thiz;
}

/* whenever the list values function is called, store the value
 * in our own list of values
 */
static Eina_Bool _egueb_svg_element_animate_transform_values_cb(double v, void *user_data)
{
	Egueb_Svg_Animate_Transform_Data *data = user_data;

	if (data->count > 3)
		return EINA_FALSE;

	data->values[data->count] = v;
	data->count++;

	return EINA_TRUE;
}

static void * _egueb_svg_element_animate_transform_value_new(void)
{
	Egueb_Svg_Animate_Transform_Data *data;

	data = calloc(1, sizeof(Egueb_Svg_Animate_Transform_Data));
	return data;
}

static Eina_Bool _egueb_svg_element_animate_transform_value_get(const char *attr, void **value)
{
	Egueb_Svg_Animate_Transform_Data *v = *value;

	egueb_svg_number_list_string_from(attr, _egueb_svg_element_animate_transform_values_cb,
				v);
	return EINA_TRUE;
}

static void * _egueb_svg_element_animate_transform_destination_new(void)
{
	Egueb_Svg_Matrix_Animated *v;

	v = calloc(1, sizeof(Egueb_Svg_Matrix_Animated));
	enesim_matrix_identity(&v->base);
	enesim_matrix_identity(&v->anim);
	return v;
}

static void _egueb_svg_element_animate_transform_destination_free(void *destination, Eina_Bool deep)
{
	Egueb_Svg_Matrix_Animated *d = destination;
	free(d);
}
/*----------------------------------------------------------------------------*
 *                        The translate type descriptor                       *
 *----------------------------------------------------------------------------*/
static void _egueb_svg_element_animate_transform_translate_interpolate(void *a, void *b,
		double m, void *add, void *acc, int mul, void *res)
{
	Egueb_Svg_Animate_Transform_Data *va = a;
	Egueb_Svg_Animate_Transform_Data *vb = b;
	Egueb_Svg_Animate_Transform_Data *vacc = acc;
	Egueb_Svg_Matrix_Animated *vadd = add;
	Egueb_Svg_Matrix_Animated *r = res;
	double tx;
	double ty = 0;

	egueb_dom_value_interpolate_double(va->values[0], vb->values[0], m, &tx);
	if (vacc)
	{
		tx += vacc->values[0] * mul;
	}
	if (va->count > 1)
	{
		egueb_dom_value_interpolate_double(va->values[1], vb->values[1], m, &ty);
		if (vacc)
			ty += vacc->values[1] * mul;
	}
	enesim_matrix_translate(&r->base, tx, ty);
	if (vadd)
		enesim_matrix_compose(&vadd->anim, &r->base, &r->base);
}

static Egueb_Svg_Attribute_Animated_Descriptor _translate_descriptor = {
	/* .value_new			= */ _egueb_svg_element_animate_transform_value_new,
	/* .value_get			= */ _egueb_svg_element_animate_transform_value_get,
	/* .value_free			= */ free,
	/* .destination_new		= */ _egueb_svg_element_animate_transform_destination_new,
	/* .destination_free		= */ _egueb_svg_element_animate_transform_destination_free,
	/* .destination_keep		= */ NULL,
	/* .destination_value_from	= */ NULL,
	/* .destination_value_to	= */ NULL,
	/* .interpolate		= */ _egueb_svg_element_animate_transform_translate_interpolate,
};
/*----------------------------------------------------------------------------*
 *                          The rotate type descriptor                        *
 *----------------------------------------------------------------------------*/
static void _egueb_svg_element_animate_transform_rotate_interpolate(void *a, void *b,
		double m, void *add, void *acc, int mul, void *res)
{
	Egueb_Svg_Animate_Transform_Data *va = a;
	Egueb_Svg_Animate_Transform_Data *vb = b;
	Egueb_Svg_Matrix_Animated *vadd = add;
	Egueb_Svg_Animate_Transform_Data *vacc = acc;
	Egueb_Svg_Matrix_Animated *r = res;
	Enesim_Matrix m1;
	double angle;

	egueb_dom_value_interpolate_double(va->values[0], vb->values[0], m, &angle);
	if (vacc)
	{
		angle += vacc->values[0] * mul;
	}
	enesim_matrix_rotate(&m1, angle * M_PI / 180.0);
	if (va->count > 1)
	{
		Enesim_Matrix m2;
		double cx;
		double cy;

		if (vacc)
		{
			cx += vacc->values[1] * mul;
			cy += vacc->values[1] * mul;
		}
		egueb_dom_value_interpolate_double(va->values[1], vb->values[1], m, &cx);
		egueb_dom_value_interpolate_double(va->values[2], vb->values[2], m, &cy);
		enesim_matrix_translate(&m2, cx, cy);
		enesim_matrix_compose(&m2, &m1, &m1);
		enesim_matrix_translate(&m2, -cx, -cy);
		enesim_matrix_compose(&m1, &m2, &m1);
	}
	r->base = m1;
	if (vadd)
		enesim_matrix_compose(&vadd->anim, &r->base, &r->base);
}

static Egueb_Svg_Attribute_Animated_Descriptor _rotate_descriptor = {
	/* .value_new			= */ _egueb_svg_element_animate_transform_value_new,
	/* .value_get			= */ _egueb_svg_element_animate_transform_value_get,
	/* .value_free			= */ free,
	/* .destination_new		= */ _egueb_svg_element_animate_transform_destination_new,
	/* .destination_free		= */ _egueb_svg_element_animate_transform_destination_free,
	/* .destination_keep		= */ NULL,
	/* .destination_value_from	= */ NULL,
	/* .destination_value_to	= */ NULL,
	/* .interpolate		= */ _egueb_svg_element_animate_transform_rotate_interpolate,
};
/*----------------------------------------------------------------------------*
 *                          The scale type descriptor                         *
 *----------------------------------------------------------------------------*/
static void _egueb_svg_element_animate_transform_scale_interpolate(void *a, void *b,
		double m, void *add, void *acc, int mul, void *res)
{
	Egueb_Svg_Animate_Transform_Data *va = a;
	Egueb_Svg_Animate_Transform_Data *vb = b;
	Egueb_Svg_Matrix_Animated *vadd = add;
	Egueb_Svg_Animate_Transform_Data *vacc = acc;
	Egueb_Svg_Matrix_Animated *r = res;
	double sx = 0;
	double sy = 1;

	egueb_dom_value_interpolate_double(va->values[0], vb->values[0], m, &sx);
	if (va->count > 1)
	{
		egueb_dom_value_interpolate_double(va->values[1], vb->values[1], m, &sy);
	}
	if (vacc)
	{
		sx += vacc->values[0] * mul;
		if (va->count > 1)
			sy += vacc->values[1] * mul;
	}

	enesim_matrix_scale(&r->base, sx, sy);
	if (vadd)
		enesim_matrix_compose(&vadd->anim, &r->base, &r->base);
}

static Egueb_Svg_Attribute_Animated_Descriptor _scale_descriptor = {
	/* .value_new			= */ _egueb_svg_element_animate_transform_value_new,
	/* .value_get			= */ _egueb_svg_element_animate_transform_value_get,
	/* .value_free			= */ free,
	/* .destination_new		= */ _egueb_svg_element_animate_transform_destination_new,
	/* .destination_free		= */ _egueb_svg_element_animate_transform_destination_free,
	/* .destination_keep		= */ NULL,
	/* .destination_value_from	= */ NULL,
	/* .destination_value_to	= */ NULL,
	/* .interpolate		= */ _egueb_svg_element_animate_transform_scale_interpolate,
};
/*----------------------------------------------------------------------------*
 *                          The skewx type descriptor                         *
 *----------------------------------------------------------------------------*/
static void _egueb_svg_element_animate_transform_skewx_interpolate(void *a, void *b,
		double m, void *add, void *acc, int mul, void *res)
{
	Egueb_Svg_Animate_Transform_Data *va = a;
	Egueb_Svg_Animate_Transform_Data *vb = b;
	Egueb_Svg_Matrix_Animated *vadd = add;
	Egueb_Svg_Animate_Transform_Data *vacc = acc;
	Egueb_Svg_Matrix_Animated *r = res;
	double angle;

	egueb_dom_value_interpolate_double(va->values[0], vb->values[0], m, &angle);
	if (vacc)
		angle += vacc->values[0] * mul;
	enesim_matrix_values_set(&r->base, 1, tan(angle * M_PI / 180.0), 0, 0, 1, 0, 0, 0, 1);
	if (vadd)
		enesim_matrix_compose(&vadd->anim, &r->base, &r->base);
}

static Egueb_Svg_Attribute_Animated_Descriptor _skewx_descriptor = {
	/* .value_new			= */ _egueb_svg_element_animate_transform_value_new,
	/* .value_get			= */ _egueb_svg_element_animate_transform_value_get,
	/* .value_free			= */ free,
	/* .destination_new		= */ _egueb_svg_element_animate_transform_destination_new,
	/* .destination_free		= */ _egueb_svg_element_animate_transform_destination_free,
	/* .destination_keep		= */ NULL,
	/* .destination_value_from	= */ NULL,
	/* .destination_value_to	= */ NULL,
	/* .interpolate		= */ _egueb_svg_element_animate_transform_skewx_interpolate,
};

/*----------------------------------------------------------------------------*
 *                          The skewy type descriptor                         *
 *----------------------------------------------------------------------------*/
static void _egueb_svg_element_animate_transform_skewy_interpolate(void *a, void *b,
		double m, void *add, void *acc, int mul, void *res)
{
	Egueb_Svg_Animate_Transform_Data *va = a;
	Egueb_Svg_Animate_Transform_Data *vb = b;
	Egueb_Svg_Animate_Transform_Data *vacc = acc;
	Egueb_Svg_Matrix_Animated *vadd = add;
	Egueb_Svg_Matrix_Animated *r = res;
	double angle;

	egueb_dom_value_interpolate_double(va->values[0], vb->values[0], m, &angle);
	if (vacc)
		angle += vacc->values[0] * mul;
	enesim_matrix_values_set(&r->base, 1, 0, 0, tan(angle * M_PI / 180.0), 1, 0, 0, 0, 1);
	if (vadd)
		enesim_matrix_compose(&vadd->anim, &r->base, &r->base);
}

static Egueb_Svg_Attribute_Animated_Descriptor _skewy_descriptor = {
	/* .value_new			= */ _egueb_svg_element_animate_transform_value_new,
	/* .value_get			= */ _egueb_svg_element_animate_transform_value_get,
	/* .value_free			= */ free,
	/* .destination_new		= */ _egueb_svg_element_animate_transform_destination_new,
	/* .destination_free		= */ _egueb_svg_element_animate_transform_destination_free,
	/* .destination_keep		= */ NULL,
	/* .destination_value_from	= */ NULL,
	/* .destination_value_to	= */ NULL,
	/* .interpolate		= */ _egueb_svg_element_animate_transform_skewy_interpolate,
};
/*----------------------------------------------------------------------------*
 *                         The Esvg Element interface                         *
 *----------------------------------------------------------------------------*/
static void _egueb_svg_element_animate_transform_free(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element_Animate_Transform *thiz;

	thiz = _egueb_svg_element_animate_transform_get(t);
	free(thiz);
}

static Eina_Bool _egueb_svg_element_animate_transform_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (!strcmp(key, "type"))
	{
		Egueb_Svg_Animate_Transform_Type type;

		egueb_svg_animate_transform_type_string_from(&type, value);
		egueb_svg_element_animate_transform_type_set(e, type);
	}
	else
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_element_animate_transform_attribute_get(Egueb_Dom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static Eina_Bool _egueb_svg_element_animate_transform_type_descriptor_get(Egueb_Dom_Tag *t,
		const char *name, Egueb_Svg_Attribute_Animated_Descriptor **d)
{
	Egueb_Svg_Element_Animate_Transform *thiz;

	/* based on the property name get the correct descriptor */
	if (strcmp(name, "SVGAnimatedMatrix"))
		return EINA_FALSE;
	/* check the type and use the correct generator */
	thiz = _egueb_svg_element_animate_transform_get(t);
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
	return EINA_TRUE;
}

static Egueb_Svg_Animate_Base_Descriptor _descriptor = {
	/* .attribute_get	= */ _egueb_svg_element_animate_transform_attribute_get,
	/* .free		= */ _egueb_svg_element_animate_transform_free,
	/* .initialize		= */ NULL,
	/* .attribute_set	= */ _egueb_svg_element_animate_transform_attribute_set,
	/* .type_descriptor_get	= */ _egueb_svg_element_animate_transform_type_descriptor_get,
};
#endif
/*----------------------------------------------------------------------------*
 *                              Element interface                             *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_String * _egueb_svg_element_animate_transform_tag_name_get(
		Egueb_Dom_Element *e)
{
	return egueb_dom_string_ref(EGUEB_SVG_NAME_ANIMATE_TRANSFORM);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SMIL_ANIMATE_BASE_DESCRIPTOR,
		Egueb_Svg_Element_Animate_Transform,
		Egueb_Svg_Element_Animate_Transform_Class,
		egueb_svg_element_animate_transform);

static void _egueb_svg_element_animate_transform_class_init(void *k)
{
	Egueb_Dom_Element_Class *e_klass;

	e_klass= EGUEB_DOM_ELEMENT_CLASS(k);
	e_klass->tag_name_get = _egueb_svg_element_animate_transform_tag_name_get;
}

static void _egueb_svg_element_animate_transform_instance_init(void *o)
{
}

static void _egueb_svg_element_animate_transform_instance_deinit(void *o)
{
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Creates a new animate_transform element
 * @return The newly created animate_transform element @ender_transfer{full}
 */
EAPI Egueb_Dom_Node * egueb_svg_element_animate_transform_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_element_animate_transform);
	return n;
}
