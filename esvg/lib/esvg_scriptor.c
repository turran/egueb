/* Esvg - SVG
 * Copyright (C) 2012 Jorge Luis Zapata
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
#include "esvg_private_scriptor.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Esvg_Scriptor
{
	Esvg_Scriptor_Descriptor *descriptor;
	void *ctx;
};

static Eina_Hash * _scriptor_descriptors = NULL;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_scriptor_init(void)
{
	/* create our hash of scrip descriptor */
	_scriptor_descriptors = eina_hash_string_superfast_new(NULL);
#ifdef BUILD_ESVG_SCRIPTOR_V8
	esvg_scriptor_js_v8_init();
#endif
}

void esvg_scriptor_shutdown(void)
{
	/* TODO remove the script descriptors */
#ifdef BUILD_ESVG_SCRIPTOR_V8
	esvg_scriptor_js_v8_shutdown();
#endif
}

Esvg_Scriptor * esvg_scriptor_new(Esvg_Scriptor_Descriptor *descriptor,
		Ender_Element *e)
{
	Esvg_Scriptor *thiz;
	void *ctx;

	if (!descriptor) return NULL;
	if (!descriptor->context_new) return NULL;

	ctx = descriptor->context_new(e);
	if (!ctx) return NULL;

	thiz = calloc(1, sizeof(Esvg_Scriptor));
	thiz->descriptor = descriptor;
	thiz->ctx = ctx;

	return thiz;
}

void esvg_scriptor_free(Esvg_Scriptor *thiz)
{
	if (thiz->descriptor->context_free)
		thiz->descriptor->context_free(thiz->ctx);
	free(thiz);
}

/* TODO also pass a void * with a container that describes what's inside
 * such data, this way we can pass event data
 */
void esvg_scriptor_run(Esvg_Scriptor *thiz, const char *script)
{
	if (thiz->descriptor->run)
		thiz->descriptor->run(thiz->ctx, script);
}

Esvg_Scriptor_Descriptor * esvg_scriptor_descriptor_find(const char *type)
{
	Esvg_Scriptor_Descriptor *descriptor;

	descriptor = eina_hash_find(_scriptor_descriptors, type);
	return descriptor;
}

void esvg_scriptor_register(Esvg_Scriptor_Descriptor *d, const char *type)
{
	/* add a new entry on the hash */
	eina_hash_add(_scriptor_descriptors, type, d);
}

void esvg_scriptor_unregister(Esvg_Scriptor_Descriptor *d, const char *type)
{
	/* remove it from our hash */
	eina_hash_del(_scriptor_descriptors, type, d);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

