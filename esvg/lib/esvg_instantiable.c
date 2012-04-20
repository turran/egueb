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
#include "esvg_private_renderable.h"
#include "esvg_private_instantiable.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_INSTANTIABLE_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_INSTANTIABLE_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_INSTANTIABLE_MAGIC);\
	} while(0)

typedef struct _Esvg_Instantiable_Descriptor_Internal
{
	Edom_Tag_Free free;
} Esvg_Instantiable_Descriptor_Internal;

typedef struct _Esvg_Instantiable
{
	EINA_MAGIC
	/* properties */
	/* interface */
	Esvg_Instantiable_Descriptor_Internal descriptor;
	/* private */
	void *data;
} Esvg_Instantiable;

static Esvg_Instantiable * _esvg_instantiable_get(Edom_Tag *t)
{
	Esvg_Instantiable *thiz;

	thiz = esvg_renderable_data_get(t);
	ESVG_INSTANTIABLE_MAGIC_CHECK(thiz);

	return thiz;
}
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*
 *                         The Esvg Element interface                         *
 *----------------------------------------------------------------------------*/
static void _esvg_instantiable_free(Edom_Tag *t)
{
	Esvg_Instantiable *thiz;

	thiz = _esvg_instantiable_get(t);
	if (thiz->descriptor.free)
		thiz->descriptor.free(t);
	free(thiz);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* The ender wrapper */
#include "generated/esvg_generated_instantiable.c"

Eina_Bool esvg_is_instantiable_internal(Edom_Tag *t)
{
	Esvg_Instantiable *thiz;
	Eina_Bool ret;

	if (!esvg_is_renderable_internal(t))
		return EINA_FALSE;
	thiz = esvg_renderable_data_get(t);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_INSTANTIABLE_MAGIC);

	return ret;
}

void * esvg_instantiable_data_get(Edom_Tag *t)
{
	Esvg_Instantiable *thiz;

	thiz = _esvg_instantiable_get(t);
	return thiz->data;
}

Edom_Tag * esvg_instantiable_new(Esvg_Instantiable_Descriptor *descriptor,
		Esvg_Type type,
		void *data)
{
	Esvg_Instantiable *thiz;
	Esvg_Renderable_Descriptor pdescriptor;
	Edom_Tag *t;

	thiz = calloc(1, sizeof(Esvg_Instantiable));
	if (!thiz) return NULL;

	EINA_MAGIC_SET(thiz, ESVG_INSTANTIABLE_MAGIC);
	thiz->data = data;
	/* our own descriptor */
	thiz->descriptor.free = descriptor->free;
	/* default values */
	
	pdescriptor.child_add = descriptor->child_add;
	pdescriptor.child_remove = descriptor->child_remove;
	pdescriptor.attribute_set = descriptor->attribute_set;
	pdescriptor.attribute_get = descriptor->attribute_get;
	pdescriptor.cdata_set = descriptor->cdata_set;
	pdescriptor.text_set = descriptor->text_set;
	pdescriptor.free = _esvg_instantiable_free;
	pdescriptor.initialize = descriptor->initialize;
	pdescriptor.setup = descriptor->setup;
	pdescriptor.renderer_get = descriptor->renderer_get;

	t = esvg_renderable_new(&pdescriptor, type, thiz);

	return t;
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool esvg_is_instantiable(Ender_Element *e)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	return esvg_is_instantiable_internal(t);
}
