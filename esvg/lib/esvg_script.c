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
#include "esvg_private_script.h"

#include "esvg_script.h"

/* This is the place holder for the script tag, it should bypass
 * any logic to the real script handler, js, python, whatever
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Ender_Property *ESVG_SCRIPT_CONTENT_SCRIPT_TYPE;

typedef struct _Esvg_Script
{
	/* properties */
	char *content_script_type;
	char *cdata;
	/* interface */
	/* private */
} Esvg_Script;

static Esvg_Script * _esvg_script_get(Edom_Tag *t)
{
	Esvg_Script *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_SCRIPT)
		return NULL;
	thiz = esvg_element_data_get(t);

	return thiz;
}

/*----------------------------------------------------------------------------*
 *                         The Esvg Element interface                         *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_script_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (strcmp(key, "contentScriptType") == 0)
	{
		esvg_script_content_script_type_set(e, value);
	}
	else
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

static Eina_Bool _esvg_script_attribute_get(Edom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static void _esvg_script_cdata_set(Edom_Tag *t, const char *cdata, unsigned int length)
{
	Esvg_Script *thiz;

	thiz = _esvg_script_get(t);
	if (thiz->cdata)
	{
		free(thiz->cdata);
		thiz->cdata = NULL;
	}
	if (cdata && length)
	{
		thiz->cdata = strndup(cdata, length);
	}
}

static void _esvg_script_free(Edom_Tag *t)
{
	Esvg_Script *thiz;

	thiz = _esvg_script_get(t);
	free(thiz);
}

static Esvg_Element_Setup_Return _esvg_script_setup(Edom_Tag *t,
		Esvg_Context *c,
		const Esvg_Element_Context *parent_context,
		Esvg_Element_Context *context,
		Esvg_Attribute_Presentation *attr,
		Enesim_Error **error)
{
	/* call the implementation of the script */
	if (!esvg_element_changed(t))
		return ESVG_SETUP_OK;

	/* delete the previous instance of the script */
	/* create a new one */

	printf("doing the setup on the script\n");
}

static Esvg_Element_Descriptor _descriptor = {
	/* .child_add		= */ NULL,
	/* .child_remove	= */ NULL,
	/* .attribute_get 	= */ _esvg_script_attribute_get,
	/* .cdata_set 		= */ _esvg_script_cdata_set,
	/* .text_set 		= */ NULL,
	/* .text_get 		= */ NULL,
	/* .free 		= */ _esvg_script_free,
	/* .attribute_set 	= */ _esvg_script_attribute_set,
	/* .attribute_animated_fetch = */ NULL,
	/* .initialize 		= */ NULL,
	/* .setup		= */ _esvg_script_setup,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_script_new(void)
{
	Esvg_Script *thiz;
	Edom_Tag *t;

	thiz = calloc(1, sizeof(Esvg_Script));
	if (!thiz) return NULL;

	/* default values */
	thiz->content_script_type = strdup("application/ecmascript");

	t = esvg_element_new(&_descriptor, ESVG_SCRIPT, thiz);

	return t;
}

static void _esvg_script_content_script_type_set(Edom_Tag *t, const char *type)
{
	Esvg_Script *thiz;

	thiz = _esvg_script_get(t);
	if (thiz->content_script_type)
	{
		free(thiz->content_script_type);
		thiz->content_script_type = NULL;
	}
	if (type)
	{
		thiz->content_script_type = strdup(type);
	}
}

static void _esvg_script_content_script_type_get(Edom_Tag *t, const char **type)
{
	Esvg_Script *thiz;

	thiz = _esvg_script_get(t);
	*type = thiz->content_script_type;
}
/* The ender wrapper */
#define _esvg_script_content_script_type_is_set NULL
#define _esvg_script_delete NULL
#include "generated/esvg_generated_script.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_script_init(void)
{
	_esvg_script_init();
}

void esvg_script_shutdown(void)
{
	_esvg_script_shutdown();
}

Eina_Bool esvg_is_script_internal(Edom_Tag *t)
{
	if (esvg_element_internal_type_get(t) != ESVG_A)
		return EINA_FALSE;
	return EINA_TRUE;
}

void esvg_script_descriptor_register(Esvg_Script_Descriptor *d, const char *type)
{
	/* create our hash of scrip descriptor */
	/* add a new entry on the hash */
}

void esvg_script_descriptor_unregister(Esvg_Script_Descriptor *d, const char *type)
{
	/* remove it from our hash */
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * esvg_script_new(void)
{
	return ESVG_ELEMENT_NEW("script");
}

EAPI void esvg_script_content_script_type_set(Ender_Element *e, const char *type)
{
	ender_element_property_value_set(e, ESVG_SCRIPT_CONTENT_SCRIPT_TYPE, type, NULL);
}

