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
#include "esvg_private_referenceable.h"
#include "esvg_private_paint_server.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_PAINT_SERVER_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_PAINT_SERVER_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_PAINT_SERVER_MAGIC);\
	} while(0)

static Ender_Property *ESVG_PAINT_SERVER_RENDERER;

typedef struct _Esvg_Paint_Server_Descriptor_Internal
{
	Edom_Tag_Free free;
} Esvg_Paint_Server_Descriptor_Internal;

typedef struct _Esvg_Paint_Server
{
	EINA_MAGIC
	/* properties */
	/* interface */
	Esvg_Paint_Server_Descriptor_Internal descriptor;
	/* private */
	void *data;
} Esvg_Paint_Server;

static Esvg_Paint_Server * _esvg_paint_server_get(Edom_Tag *t)
{
	Esvg_Paint_Server *thiz;

	thiz = esvg_referenceable_data_get(t);
	ESVG_PAINT_SERVER_MAGIC_CHECK(thiz);

	return thiz;
}
/*----------------------------------------------------------------------------*
 *                         Esvg Element interface                             *
 *----------------------------------------------------------------------------*/
static void _esvg_paint_server_free(Edom_Tag *t)
{
	Esvg_Paint_Server *thiz;

	thiz = _esvg_paint_server_get(t);
	if (thiz->descriptor.free)
		thiz->descriptor.free(t);
	free(thiz);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
#include "generated/esvg_generated_paint_server.c"

Eina_Bool esvg_is_paint_server_internal(Edom_Tag *t)
{
	Esvg_Paint_Server *thiz;
	Eina_Bool ret;

	if (!esvg_is_element_internal(t))
		return EINA_FALSE;
	thiz = esvg_element_data_get(t);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_PAINT_SERVER_MAGIC);

	return ret;
}

Edom_Tag * esvg_paint_server_new(Esvg_Paint_Server_Descriptor *descriptor,
		Esvg_Type type,
		void *data)
{
	Esvg_Paint_Server *thiz;
	Esvg_Referenceable_Descriptor pdescriptor;
	Edom_Tag *t;

	thiz = calloc(1, sizeof(Esvg_Paint_Server));
	if (!thiz) return NULL;

	EINA_MAGIC_SET(thiz, ESVG_PAINT_SERVER_MAGIC);
	thiz->data = data;

	pdescriptor.child_add = descriptor->child_add;
	pdescriptor.child_remove = descriptor->child_remove;
	pdescriptor.attribute_set = descriptor->attribute_set;
	pdescriptor.attribute_get = descriptor->attribute_get;
	pdescriptor.cdata_set = descriptor->cdata_set;
	pdescriptor.text_set = descriptor->text_set;
	pdescriptor.free = _esvg_paint_server_free;
	pdescriptor.initialize = descriptor->initialize;
	pdescriptor.setup = descriptor->setup;
	pdescriptor.cleanup = descriptor->cleanup;
	pdescriptor.propagate = descriptor->propagate;
	pdescriptor.renderer_new = descriptor->renderer_new;
	pdescriptor.reference_add = descriptor->reference_add;

	t = esvg_referenceable_new(&pdescriptor, type, thiz);

	return t;
}

void * esvg_paint_server_data_get(Edom_Tag *t)
{
	Esvg_Paint_Server *thiz;

	thiz = _esvg_paint_server_get(t);
	return thiz->data;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool esvg_is_paint_server(Ender_Element *e)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	return esvg_is_paint_server_internal(t);
}
