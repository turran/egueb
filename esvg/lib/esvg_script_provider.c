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

#include "esvg_script_provider.h"
#include "esvg_private_script_provider.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Esvg_Script_Provider
{
	Esvg_Script_Provider_Descriptor *descriptor;
	void *ctx;
};

static Eina_Hash * _script_provider_descriptors = NULL;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_script_provider_init(void)
{
	/* create our hash of scrip descriptor */
	_script_provider_descriptors = eina_hash_string_superfast_new(NULL);
#ifdef BUILD_ESVG_SCRIPTOR_V8
	esvg_script_provider_js_v8_init();
	esvg_script_provider_register(&esvg_script_provider_js_v8_descriptor,
			"application/ecmascript");
#endif
}

void esvg_script_provider_shutdown(void)
{
	/* TODO remove the script descriptors */
#ifdef BUILD_ESVG_SCRIPTOR_V8
	esvg_script_provider_unregister(&esvg_script_provider_js_v8_descriptor,
			"application/ecmascript");
	esvg_script_provider_js_v8_shutdown();
#endif
}

Esvg_Script_Provider * esvg_script_provider_new(Esvg_Script_Provider_Descriptor *descriptor,
		Ender_Element *e)
{
	Esvg_Script_Provider *thiz;
	void *ctx;

	if (!descriptor) return NULL;
	if (!descriptor->context_new) return NULL;

	ctx = descriptor->context_new(e);
	if (!ctx) return NULL;

	thiz = calloc(1, sizeof(Esvg_Script_Provider));
	thiz->descriptor = descriptor;
	thiz->ctx = ctx;

	return thiz;
}

void esvg_script_provider_free(Esvg_Script_Provider *thiz)
{
	if (thiz->descriptor->context_free)
		thiz->descriptor->context_free(thiz->ctx);
	free(thiz);
}

/* TODO also pass a void * with a container that describes what's inside
 * such data, this way we can pass event data
 */
void esvg_script_provider_run(Esvg_Script_Provider *thiz, const char *script,
		Ender_Value *v)
{
	if (thiz->descriptor->run)
		thiz->descriptor->run(thiz->ctx, script, v);
}

Esvg_Script_Provider_Descriptor * esvg_script_provider_descriptor_find(const char *type)
{
	Esvg_Script_Provider_Descriptor *descriptor;

	descriptor = eina_hash_find(_script_provider_descriptors, type);
	return descriptor;
}

void esvg_script_provider_register(Esvg_Script_Provider_Descriptor *d, const char *type)
{
	/* add a new entry on the hash */
	eina_hash_add(_script_provider_descriptors, type, d);
}

void esvg_script_provider_unregister(Esvg_Script_Provider_Descriptor *d, const char *type)
{
	/* remove it from our hash */
	eina_hash_del(_script_provider_descriptors, type, d);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

