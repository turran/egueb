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
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Ender_Property *ESVG_ANIMATE_TRANSFORM_TYPE;

typedef struct _Esvg_Animate_Transform
{
	/* properties */
	/* interface */
	/* private */
	Etch_Animation *anim;
	Ender_Property *prop;
	Ender_Element *parent_e;
	Edom_Tag *parent_t;
	double from;
	double to;
	double by;
	Esvg_Attribute_Type attribute_type;
} Esvg_Animate_Transform;

static Esvg_Animate_Transform * _esvg_animate_transform_get(Edom_Tag *t)
{
	Esvg_Animate_Transform *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_ANIMATETRANSFORM)
		return NULL;
	thiz = esvg_animate_base_data_get(t);

	return thiz;
}

/*----------------------------------------------------------------------------*
 *                         The Etch animator callbacks                        *
 *----------------------------------------------------------------------------*/
static void _esvg_animate_transform_cb(const Etch_Data *curr,
		const Etch_Data *prev, void *data)
{
}

static Eina_Bool _esvg_animate_transform_container_etch_to(Esvg_Animate_Transform *thiz, Etch *etch,
		Ender_Property *p, Esvg_Animate_Base_Context *c)
{
	Ender_Container *ec;
	Etch_Animation *a;
	Etch_Data_Type dt;
	Etch_Data from;
	Etch_Data to;
	Etch_Animation_Callback cb;
	Etch_Animation_Keyframe *kf;
	const char *name;

	ec = ender_property_container_get(p);
	name = ender_container_registered_name_get(ec);
	if (!strcmp(name, "esvg_animated_transformed"))
	{
		dt = ETCH_DOUBLE;
		cb = _esvg_animate_transform_cb;

		thiz->from = esvg_number_string_from(c->value.from, 1.0);
		thiz->to = esvg_number_string_from(c->value.to, 1.0);
		from.data.d = thiz->from;
		to.data.d = thiz->to;
	}
	else
	{
		return EINA_FALSE;
	}

	a = etch_animation_add(etch, dt, cb, NULL, NULL, thiz);
	/* FIXME for now we add two keyframes */
	/* second keyframe */
	kf = etch_animation_keyframe_add(a);
	etch_animation_keyframe_type_set(kf, ETCH_ANIMATION_LINEAR);
	etch_animation_keyframe_value_set(kf, &from);
	etch_animation_keyframe_time_set(kf, 3, 1237);
	/* third keyframe */
	kf = etch_animation_keyframe_add(a);
	etch_animation_keyframe_type_set(kf, ETCH_ANIMATION_LINEAR);
	etch_animation_keyframe_value_set(kf, &to);
	etch_animation_keyframe_time_set(kf, 5, 2530);
	etch_animation_enable(a);

	printf("everything went ok!\n");

	thiz->anim = a;
	thiz->prop = p;

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
	return EINA_TRUE;
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

	thiz = _esvg_animate_transform_get(t);
	printf("animate_transform setup!\n");

	/* same */
	esvg_element_internal_topmost_get(t, &svg_e);
	if (!svg_e) goto done;

	etch = esvg_svg_etch_get(svg_e);
	if (!etch) goto done;

	if (thiz->anim)
		etch_animation_delete(thiz->anim);

	parent_t = edom_tag_parent_get(t);
	parent_e = esvg_element_ender_get(parent_t);
	/* get the property name */
	p = ender_element_property_get(parent_e, actx->target.attribute_name);
	if (!p) goto done;

	printf("property found!!!\n");
	thiz->parent_e = parent_e;
	thiz->parent_t = parent_t;
	thiz->attribute_type = actx->target.attribute_type;

	/* we should only process lengths, colors, integers, booleans, etc */
	if (!_esvg_animate_transform_container_etch_to(thiz, etch, p, abctx))
		goto done;

	/* check the type and create an animator of that container type */
	/* on every animation callback set the animation mode on the element */
	/* then call the property set */
	/* restore the mode */

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
	return ender_element_new_with_namespace("animate_transform", "esvg");
}


