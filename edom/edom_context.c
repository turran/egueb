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

#include <Eina.h>
#include "Edom.h"
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
	Edom_Context *c;

	if (!d) return NULL;

	c = calloc(1, sizeof(Edom_Context));
	if (!c) return NULL;

	c->descriptor = *d;
	c->parser = parser;
	c->data = data;

	return c;
}

EAPI void edom_context_delete(Edom_Context *c)
{
	if (!c) return;

	if (c->descriptor.free)
		c->descriptor.free(c->data);
	free(c);
}

EAPI Eina_Bool edom_context_tag_open(Edom_Context *c, int tag, Eina_Array *contexts, const char *attrs, unsigned int length)
{
	if (!c) return EINA_FALSE;
	if (c->descriptor.tag_open)
		return c->descriptor.tag_open(c->data, tag, contexts, attrs, length);
	return EINA_FALSE;
}

EAPI void edom_context_tag_close(Edom_Context *c, int tag, Eina_Array *contexts)
{
	if (!c) return;
	if (c->descriptor.tag_close)
		return c->descriptor.tag_close(c->data, tag, contexts);
	return;
}

EAPI void edom_context_cdata(Edom_Context *c, const char *cdata, unsigned int length)
{
	if (!c) return;
	if (c->descriptor.cdata)
		return c->descriptor.cdata(c->data, cdata, length);
	return;
}

EAPI void edom_context_data(Edom_Context *c, const char *data, unsigned int length)
{
	if (!c) return;
	if (c->descriptor.data)
		return c->descriptor.data(c->data, data, length);
	return;
}

EAPI void * edom_context_data_get(Edom_Context *c)
{
	if (!c) return NULL;

	return c->data;
}

EAPI Edom_Parser * edom_context_parser_get(Edom_Context *thiz)
{
	if (!thiz) return NULL;
	return thiz->parser;
}
