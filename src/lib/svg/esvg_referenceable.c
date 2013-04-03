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
#include "esvg_private_main.h"
#include "esvg_private_attribute_presentation.h"
#include "esvg_private_context.h"
#include "esvg_element_private.h"
#include "esvg_private_referenceable.h"

#include "esvg_referenceable.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_REFERENCEABLE_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_REFERENCEABLE_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_REFERENCEABLE_MAGIC);\
	} while(0)

typedef struct _Esvg_Referenceable_Descriptor_Internal
{
	Edom_Tag_Free free;
	Esvg_Referenceable_Setup setup;
	Esvg_Referenceable_Cleanup cleanup;
	Esvg_Referenceable_Propagate propagate;
	Esvg_Referenceable_Reference_Add reference_add;
	Esvg_Referenceable_Reference_Remove reference_remove;
} Esvg_Referenceable_Descriptor_Internal;

typedef struct _Esvg_Referenceable
{
	EINA_MAGIC
	/* properties */
	/* interface */
	Esvg_Referenceable_Descriptor_Internal descriptor;
	/* private */
	Eina_List *references;
	void *data;
} Esvg_Referenceable;

static Esvg_Referenceable * _esvg_referenceable_get(Edom_Tag *t)
{
	Esvg_Referenceable *thiz;

	thiz = esvg_element_data_get(t);
	ESVG_REFERENCEABLE_MAGIC_CHECK(thiz);

	return thiz;
}

/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*
 *                         The Esvg Element interface                         *
 *----------------------------------------------------------------------------*/
static void _esvg_referenceable_free(Edom_Tag *t)
{
	Esvg_Referenceable *thiz;

	thiz = _esvg_referenceable_get(t);
	if (thiz->descriptor.free)
		thiz->descriptor.free(t);
	free(thiz);
}

static Esvg_Element_Setup_Return _esvg_referenceable_setup(Edom_Tag *t,
		Esvg_Context *c,
		const Esvg_Element_Context *parent_context,
		Esvg_Element_Context *context,
		Esvg_Attribute_Presentation *attr,
		Enesim_Log **error)
{
	Esvg_Element_Setup_Return ret = ESVG_SETUP_OK;
	Esvg_Referenceable *thiz;
	Esvg_Referenceable_Reference *rr;
	Eina_List *l;

	thiz = _esvg_referenceable_get(t);
	/* a referenceable should always get its attributes from its
	 * parent tree
	 */
	if (thiz->descriptor.setup)
		ret = thiz->descriptor.setup(t, c, context, attr, error);
	if (ret != ESVG_SETUP_OK)
	{
		printf(">>>>> setup enqueue!\n");
		return ret;
	}

	/* for each reference, propagate it */
	EINA_LIST_FOREACH(thiz->references, l, rr)
	{
		esvg_referenceable_reference_propagate(rr, c, error);
	}
	if (thiz->descriptor.cleanup)
		thiz->descriptor.cleanup(t);

	return ESVG_SETUP_OK;
}

/* The ender wrapper */
#define _esvg_referenceable_delete NULL
#include "esvg_generated_referenceable.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_referenceable_init(void)
{
	_esvg_referenceable_init();
}

void esvg_referenceable_shutdown(void)
{
	_esvg_referenceable_shutdown();
}

Eina_Bool esvg_is_referenceable_internal(Edom_Tag *t)
{
	Esvg_Referenceable *thiz;
	Eina_Bool ret;

	if (!esvg_is_element_internal(t))
		return EINA_FALSE;
	thiz = esvg_element_data_get(t);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_REFERENCEABLE_MAGIC);

	return ret;
}

void esvg_referenceable_reference_propagate(Esvg_Referenceable_Reference *rr,
		Esvg_Context *c,
		Enesim_Log **error)
{
	Esvg_Referenceable *thiz;
	const Esvg_Element_Context *ctx;
	const Esvg_Attribute_Presentation *attr;

	thiz = _esvg_referenceable_get(rr->t);
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
	ctx = esvg_element_context_get(rr->referencer);
	attr = esvg_element_attribute_presentation_get(rr->referencer);

	/* call the propagate interface */
	thiz->descriptor.propagate(rr->t, c, ctx, attr, rr->data, error);
}

Esvg_Referenceable_Reference * esvg_referenceable_reference_add(Edom_Tag *t,
		Edom_Tag *referencer)
{
	Esvg_Referenceable_Reference *rr;
	Esvg_Referenceable *thiz;

	thiz = _esvg_referenceable_get(t);

	/* create the reference struct */
	rr = calloc(1, sizeof(Esvg_Referenceable_Reference));
	rr->referencer = referencer;
	rr->t = t;

	/* trigger the add interface in case it has it */
	if (thiz->descriptor.reference_add)
	{
		Eina_Bool ret;
		ret = thiz->descriptor.reference_add(t, rr);
		if (!ret)
		{
			free(rr);
			return NULL;
		}
	}

	/* add to the list of references */
	thiz->references = eina_list_append(thiz->references, rr);

	return rr;
}

void esvg_referenceable_reference_remove(Edom_Tag *t, Esvg_Referenceable_Reference *rr)
{
	Esvg_Referenceable *thiz;

	thiz = _esvg_referenceable_get(t);
	/* trigger the remove interface */
	if (thiz->descriptor.reference_remove)
		thiz->descriptor.reference_remove(t, rr);

	thiz->references = eina_list_remove(thiz->references, rr);
	/* TODO unref the renderer, pass the renderer to the remove? */
	free(rr);
}

void esvg_referenceable_reference_foreach(Edom_Tag *t, Esvg_Referenceable_Cb cb, void *data)
{
	Esvg_Referenceable *thiz;
	Esvg_Referenceable_Reference *rr;
	Eina_List *l;

	thiz = _esvg_referenceable_get(t);
	/* iterate over the list of references */
	EINA_LIST_FOREACH(thiz->references, l, rr)
	{
		/* and call the function */
		if (!cb(t, rr, data))
			break;
	}
}

void * esvg_referenceable_data_get(Edom_Tag *t)
{
	Esvg_Referenceable *thiz;

	thiz = _esvg_referenceable_get(t);
	return thiz->data;
}

Edom_Tag * esvg_referenceable_new(Esvg_Referenceable_Descriptor *descriptor, Esvg_Type type,
		void *data)
{
	Esvg_Referenceable *thiz;
	Esvg_Element_Descriptor pdescriptor;
	Edom_Tag *t;

	thiz = calloc(1, sizeof(Esvg_Referenceable));
	if (!thiz) return NULL;

	EINA_MAGIC_SET(thiz, ESVG_REFERENCEABLE_MAGIC);
	thiz->data = data;
	/* our own descriptor */
	thiz->descriptor.setup = descriptor->setup;
	thiz->descriptor.cleanup = descriptor->cleanup;
	thiz->descriptor.propagate = descriptor->propagate;
	thiz->descriptor.reference_add = descriptor->reference_add;
	/* default values */

	pdescriptor.child_add = descriptor->child_add;
	pdescriptor.child_remove = descriptor->child_remove;
	pdescriptor.attribute_set = descriptor->attribute_set;
	pdescriptor.attribute_get = descriptor->attribute_get;
	pdescriptor.attribute_animated_fetch = descriptor->attribute_animated_fetch;
	pdescriptor.cdata_set = descriptor->cdata_set;
	pdescriptor.text_set = descriptor->text_set;
	pdescriptor.text_get = descriptor->text_get;
	pdescriptor.free = _esvg_referenceable_free;
	pdescriptor.initialize = descriptor->initialize;
	pdescriptor.setup = _esvg_referenceable_setup;

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
EAPI Eina_Bool esvg_is_referenceable(Ender_Element *e)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	return esvg_is_referenceable_internal(t);
}
