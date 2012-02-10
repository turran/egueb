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
#include "Esvg.h"
#include "esvg_private.h"
#include "esvg_values.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_PAINT_SERVER_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_PAINT_SERVER_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_PAINT_SERVER_MAGIC);\
	} while(0)

typedef struct _Esvg_Paint_Server
{
	EINA_MAGIC
	Esvg_Paint_Server_Setup setup;
	void *data;
} Esvg_Paint_Server;

static Esvg_Paint_Server * _esvg_paint_server_get(Enesim_Renderer *r)
{
	Esvg_Paint_Server *thiz;

	thiz = esvg_element_data_get(r);
	ESVG_PAINT_SERVER_MAGIC_CHECK(thiz);

	return thiz;
}
/*----------------------------------------------------------------------------*
 *                         Esvg Element interface                             *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_paint_server_setup(Enesim_Renderer *r,
		Esvg_Element_State *estate,
		Esvg_Attribute_Presentation *attr,
		Enesim_Surface *s,
		Enesim_Error **error)
{
	Esvg_Paint_Server *thiz;

	thiz = _esvg_paint_server_get(r);
	/* FIXME we should not go through here */
	return EINA_TRUE;
}

static void _esvg_paint_server_cleanup(Enesim_Renderer *r)
{
	Esvg_Paint_Server *thiz;

	thiz = _esvg_paint_server_get(r);
	/* FIXME for later */
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_paint_server_renderer_setup(Enesim_Renderer *r,
		const Esvg_Element_State *state,
		Enesim_Renderer *rel)
{
	Esvg_Paint_Server *thiz;

	thiz = _esvg_paint_server_get(r);
	if (!thiz->setup) return;
	thiz->setup(r, state, rel);
}

Enesim_Renderer * esvg_paint_server_new(Esvg_Paint_Server_Descriptor *descriptor,
		void *data)
{
	Esvg_Paint_Server *thiz;
	Esvg_Element_Descriptor pdescriptor = {NULL, NULL, NULL, NULL};
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Paint_Server));
	if (!thiz) return NULL;

	EINA_MAGIC_SET(thiz, ESVG_PAINT_SERVER_MAGIC);
	thiz->setup = descriptor->setup;
	thiz->data = data;

	pdescriptor.name_get = descriptor->name_get;
	pdescriptor.renderer_get = descriptor->renderer_get;
	pdescriptor.setup = _esvg_paint_server_setup;
	pdescriptor.cleanup = _esvg_paint_server_cleanup;
	pdescriptor.clone = descriptor->clone;

	/* Default values */
	r = esvg_element_new(&pdescriptor, thiz);
	return r;
}

void * esvg_paint_server_data_get(Enesim_Renderer *r)
{
	Esvg_Paint_Server *thiz;

	thiz = _esvg_paint_server_get(r);
	return thiz->data;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool esvg_is_paint_server(Enesim_Renderer *r)
{
	Esvg_Paint_Server *thiz;
	Eina_Bool ret;

	if (!esvg_is_element(r))
		return EINA_FALSE;
	thiz = esvg_element_data_get(r);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_PAINT_SERVER_MAGIC);

	return ret;
}
