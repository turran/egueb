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
#include "egueb_svg_referenceable_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                             Element interface                              *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_referenceable_process(Egueb_Svg_Element *e)
{
	Egueb_Svg_Referenceable *thiz;
	Egueb_Svg_Referenceable_Class *klass;
	Egueb_Svg_Reference *r;
	Eina_List *l;

	thiz = EGUEB_SVG_REFERENCEABLE(e);
	klass = EGUEB_SVG_REFERENCEABLE_CLASS_GET(e);
	/* we first need to process the element itself */
	if (klass->process)
	{
		if (!klass->process(thiz))
			return EINA_FALSE;
	}
	/* for each reference, propagate it */
	EINA_LIST_FOREACH(thiz->references, l, r)
	{
		egueb_svg_reference_process(r);
	}
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_ABSTRACT_BOILERPLATE(EGUEB_SVG_ELEMENT_DESCRIPTOR,
		Egueb_Svg_Referenceable, Egueb_Svg_Referenceable_Class,
		egueb_svg_referenceable);

static void _egueb_svg_referenceable_class_init(void *k)
{
	Egueb_Svg_Element_Class *klass;

	klass = EGUEB_SVG_ELEMENT_CLASS(k);
	klass->process = _egueb_svg_referenceable_process;
}

static void _egueb_svg_referenceable_class_deinit(void *k)
{
}

static void _egueb_svg_referenceable_instance_init(void *o)
{
}

static void _egueb_svg_referenceable_instance_deinit(void *o)
{
	/* when we reach this moment, we shuld not have any reference */
}
#if 0
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_svg_referenceable_reference_propagate(Egueb_Svg_Referenceable_Reference *rr,
		Egueb_Svg_Context *c,
		Enesim_Log **error)
{
	Egueb_Svg_Referenceable *thiz;
	const Egueb_Svg_Element_Context *ctx;
	const Egueb_Svg_Attribute_Presentation *attr;

	thiz = _egueb_svg_referenceable_get(rr->t);
	/* TODO this should be the entry point of elements that have
	 * references, here we should check if the element needs setup
	 * if so, call it instead of this
	 */
	/* TODO check that the element hasnt changed
	 * if it has changed, check that the context is the same
	 * if not, dont call anything
	 */

	if (!thiz->descriptor.propagate) return;

	/* get the context and attributes from the referencer */
	ctx = egueb_svg_element_context_get(rr->referencer);
	attr = egueb_svg_element_attribute_presentation_get(rr->referencer);

	/* call the propagate interface */
	thiz->descriptor.propagate(rr->t, c, ctx, attr, rr->data, error);
}

void egueb_svg_referenceable_reference_remove(Egueb_Dom_Tag *t, Egueb_Svg_Referenceable_Reference *rr)
{
	Egueb_Svg_Referenceable *thiz;

	thiz = _egueb_svg_referenceable_get(t);
	/* trigger the remove interface */
	if (thiz->descriptor.reference_remove)
		thiz->descriptor.reference_remove(t, rr);

	thiz->references = eina_list_remove(thiz->references, rr);
	/* TODO unref the renderer, pass the renderer to the remove? */
	free(rr);
}

void egueb_svg_referenceable_reference_foreach(Egueb_Dom_Tag *t, Egueb_Svg_Referenceable_Cb cb, void *data)
{
	Egueb_Svg_Referenceable *thiz;
	Egueb_Svg_Referenceable_Reference *rr;
	Eina_List *l;

	thiz = _egueb_svg_referenceable_get(t);
	/* iterate over the list of references */
	EINA_LIST_FOREACH(thiz->references, l, rr)
	{
		/* and call the function */
		if (!cb(t, rr, data))
			break;
	}
}
#endif
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* TODO remove this, use an event or something */
void egueb_svg_referenceable_reference_remove(Egueb_Dom_Node *n,
		Egueb_Svg_Reference *r)
{
	Egueb_Svg_Referenceable *thiz;
	thiz = EGUEB_SVG_REFERENCEABLE(n);
	thiz->references = eina_list_remove(thiz->references, r);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool egueb_svg_is_referenceable(Egueb_Dom_Node *n)
{
	if (!n) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(n),
			EGUEB_SVG_REFERENCEABLE_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Egueb_Svg_Reference * egueb_svg_referenceable_reference_add(
		Egueb_Dom_Node *n, Egueb_Dom_Node *referencer,
		Egueb_Svg_Reference_Destroy_Cb notify)
{
	Egueb_Svg_Referenceable *thiz;
	Egueb_Svg_Referenceable_Class *klass;
	Egueb_Svg_Reference *r;

	if (!referencer) return;

	klass = EGUEB_SVG_REFERENCEABLE_CLASS_GET(n);
	if (!klass->reference_new) return NULL;

	thiz = EGUEB_SVG_REFERENCEABLE(n);
	/* create a new reference */
	r = klass->reference_new(thiz, referencer);
	if (!r) return NULL;

	/* set the common members */
	egueb_svg_reference_init(r, n, referencer);

	/* add to the list of references */
	thiz->references = eina_list_append(thiz->references, r);
	return r;
}
