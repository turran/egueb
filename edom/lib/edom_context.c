/* Edom - DOM
 * Copyright (C) 2011 Jorge Luis Zapata
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

#include <Eina.h>

#include "Edom.h"
#include "edom_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Edom_Context
{
	Edom_Context_Descriptor descriptor;
	Edom_Parser *parser;
	/* the associated private data */
	void *data;
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Edom_Context * edom_context_new(Edom_Parser *parser,
		Edom_Context_Descriptor *d,
		void *data)
{
	Edom_Context *thiz;

	if (!d) return NULL;

	thiz = calloc(1, sizeof(Edom_Context));
	if (!thiz) return NULL;

	thiz->descriptor = *d;
	thiz->parser = parser;
	thiz->data = data;

	return thiz;
}

EAPI void edom_context_delete(Edom_Context *thiz)
{
	if (!thiz) return;

	if (thiz->descriptor.free)
		thiz->descriptor.free(thiz->data);
	free(thiz);
}

EAPI Eina_Bool edom_context_tag_open(Edom_Context *thiz, int tag, Eina_Array *contexts, const char *attrs, unsigned int length)
{
	if (!thiz) return EINA_FALSE;
	if (thiz->descriptor.tag_open)
		return thiz->descriptor.tag_open(thiz->data, tag, contexts, attrs, length);
	return EINA_FALSE;
}

EAPI void edom_context_tag_close(Edom_Context *thiz, int tag, Eina_Array *contexts)
{
	if (!thiz) return;
	if (thiz->descriptor.tag_close)
		return thiz->descriptor.tag_close(thiz->data, tag, contexts);
	return;
}

EAPI void edom_context_cdata(Edom_Context *thiz, const char *cdata, unsigned int length)
{
	if (!thiz) return;
	if (thiz->descriptor.cdata)
		return thiz->descriptor.cdata(thiz->data, cdata, length);
	return;
}

EAPI void edom_context_data(Edom_Context *thiz, const char *data, unsigned int length)
{
	if (!thiz) return;
	if (thiz->descriptor.data)
		return thiz->descriptor.data(thiz->data, data, length);
	return;
}

EAPI void * edom_context_data_get(Edom_Context *thiz)
{
	if (!thiz) return NULL;

	return thiz->data;
}

EAPI Edom_Parser * edom_context_parser_get(Edom_Context *thiz)
{
	if (!thiz) return NULL;
	return thiz->parser;
}
