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
#include "egueb_svg_main_private.h"
#include "egueb_svg_private_attribute_presentation.h"
#include "egueb_svg_context_private.h"
#include "egueb_svg_element_private.h"
#include "egueb_svg_element_svg_private.h"
#include "egueb_svg_script_provider_private.h"

#include "egueb_svg_element_script.h"

/* This is the place holder for the script tag, it should bypass
 * any logic to the real script handler, js, python, whatever
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Ender_Property *ESVG_ELEMENT_SCRIPT_CONTENT_SCRIPT_TYPE;

typedef struct _Egueb_Svg_Element_Script
{
	/* properties */
	char *content_script_type;
	char *cdata;
	/* interface */
	/* private */
	Egueb_Svg_Script_Provider *scriptor;
} Egueb_Svg_Element_Script;

static Egueb_Svg_Element_Script * _egueb_svg_element_script_get(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element_Script *thiz;

	if (egueb_svg_element_internal_type_get(t) != ESVG_TYPE_SCRIPT)
		return NULL;
	thiz = egueb_svg_element_data_get(t);

	return thiz;
}

/*----------------------------------------------------------------------------*
 *                         The Esvg Element interface                         *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_element_script_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (strcmp(key, "contentScriptType") == 0)
	{
		egueb_svg_element_script_content_script_type_set(e, value);
	}
	else
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_element_script_attribute_get(Egueb_Dom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static void _egueb_svg_element_script_cdata_set(Egueb_Dom_Tag *t, const char *cdata, unsigned int length)
{
	Egueb_Svg_Element_Script *thiz;

	thiz = _egueb_svg_element_script_get(t);
	if (thiz->cdata)
	{
		free(thiz->cdata);
		thiz->cdata = NULL;
	}
	if (cdata && length)
	{
		thiz->cdata = eina_strndup(cdata, length);
	}
}

static void _egueb_svg_element_script_free(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element_Script *thiz;

	thiz = _egueb_svg_element_script_get(t);
	free(thiz);
}

static Egueb_Svg_Element_Setup_Return _egueb_svg_element_script_setup(Egueb_Dom_Tag *t,
		Egueb_Svg_Context *c,
		const Egueb_Svg_Element_Context *parent_context,
		Egueb_Svg_Element_Context *context,
		Egueb_Svg_Attribute_Presentation *attr,
		Enesim_Log **error)
{
	Egueb_Svg_Element_Script *thiz;
	Ender_Element *topmost;

	/* call the implementation of the script */
	if (!egueb_svg_element_changed(t))
		return ESVG_SETUP_OK;

	thiz = _egueb_svg_element_script_get(t);
	if (thiz->scriptor)
	{
		/* TODO delete the previous instance of the script */
		thiz->scriptor = NULL;
	}
	/* create a new context */
	egueb_svg_element_internal_topmost_get(t, &topmost);
	thiz->scriptor = egueb_svg_element_svg_script_provider_get(topmost, thiz->content_script_type);
	if (!thiz->scriptor) return ESVG_SETUP_OK;

	/* run the script */
	egueb_svg_script_provider_run(thiz->scriptor, thiz->cdata, NULL);
	return ESVG_SETUP_OK;
}

static Egueb_Svg_Element_Descriptor _descriptor = {
	/* .child_add		= */ NULL,
	/* .child_remove	= */ NULL,
	/* .attribute_get 	= */ _egueb_svg_element_script_attribute_get,
	/* .cdata_set 		= */ _egueb_svg_element_script_cdata_set,
	/* .text_set 		= */ NULL,
	/* .text_get 		= */ NULL,
	/* .free 		= */ _egueb_svg_element_script_free,
	/* .attribute_set 	= */ _egueb_svg_element_script_attribute_set,
	/* .attribute_animated_fetch = */ NULL,
	/* .initialize 		= */ NULL,
	/* .setup		= */ _egueb_svg_element_script_setup,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Tag * _egueb_svg_element_script_new(void)
{
	Egueb_Svg_Element_Script *thiz;
	Egueb_Dom_Tag *t;

	thiz = calloc(1, sizeof(Egueb_Svg_Element_Script));
	if (!thiz) return NULL;

	/* default values */
	thiz->content_script_type = strdup("application/ecmascript");

	t = egueb_svg_element_new(&_descriptor, ESVG_TYPE_SCRIPT, thiz);

	return t;
}

static void _egueb_svg_element_script_content_script_type_set(Egueb_Dom_Tag *t, const char *type)
{
	Egueb_Svg_Element_Script *thiz;

	thiz = _egueb_svg_element_script_get(t);
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

static void _egueb_svg_element_script_content_script_type_get(Egueb_Dom_Tag *t, const char **type)
{
	Egueb_Svg_Element_Script *thiz;

	thiz = _egueb_svg_element_script_get(t);
	*type = thiz->content_script_type;
}
/* The ender wrapper */
#define _egueb_svg_element_script_content_script_type_is_set NULL
#define _egueb_svg_element_script_delete NULL
#include "egueb_svg_generated_element_script.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_svg_element_script_init(void)
{
	_egueb_svg_element_script_init();
}

void egueb_svg_element_script_shutdown(void)
{
	_egueb_svg_element_script_shutdown();
}

Eina_Bool egueb_svg_is_script_internal(Egueb_Dom_Tag *t)
{
	if (egueb_svg_element_internal_type_get(t) != ESVG_TYPE_SCRIPT)
		return EINA_FALSE;
	return EINA_TRUE;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * egueb_svg_element_script_new(void)
{
	return ESVG_ELEMENT_NEW("SVGScriptElement");
}

EAPI void egueb_svg_element_script_content_script_type_set(Ender_Element *e, const char *type)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_SCRIPT_CONTENT_SCRIPT_TYPE, type, NULL);
}

