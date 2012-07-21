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
#include "esvg_a.h"

/* TODO
 * whenever a child is added, if it is of type renderable, then
 * register the click event, whenever the click event occurs, trigger
 * a function on the svg that will inform that the document should
 * go to another location
 * the 'a' can have any child that its parent may container except an 'a'
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Ender_Property *ESVG_A_HREF;

typedef struct _Esvg_A
{
	/* properties */
	char *href;
	/* interface */
	/* private */
} Esvg_A;

static Esvg_A * _esvg_a_get(Edom_Tag *t)
{
	Esvg_A *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_A)
		return NULL;
	thiz = esvg_element_data_get(t);

	return thiz;
}
/*----------------------------------------------------------------------------*
 *                         The Esvg Element interface                         *
 *----------------------------------------------------------------------------*/
static void _esvg_a_free(Edom_Tag *t)
{
	Esvg_A *thiz;

	thiz = _esvg_a_get(t);
	free(thiz);
}

static Eina_Bool _esvg_a_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (strcmp(key, "xlink:href") == 0)
	{
		esvg_a_href_set(e, value);
#if 0
		Esvg_A *thiz;

		thiz = edom_tag_data_get(tag);
		/* absolute */
		if (*value == '/')
		{
			thiz->href = strdup(value);
		}
		/* relative */
		else
		{
			Edom_Parser *parser;
			char real[PATH_MAX];
			const char *root;

			parser = edom_tag_parser_get(tag);
			root = edom_parser_root_get(parser);
			strcpy(real, root);
			strcat(real, value);
			thiz->href = strdup(real);
		}
#endif
	}
	/*
	 * a color and a opacity are part of the presentation attributes
	 * and already parsed on the element
	 */
	return EINA_TRUE;
}

static Eina_Bool _esvg_a_attribute_get(Edom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

/* TODO optimize so many 'ifs' */
static Esvg_Element_Setup_Return _esvg_a_setup(Edom_Tag *t,
		Esvg_Context *c,
		const Esvg_Element_Context *parent_context,
		Esvg_Element_Context *context,
		Esvg_Attribute_Presentation *attr,
		Enesim_Error **error)
{
	Esvg_A *thiz;

	thiz = _esvg_a_get(t);
	return EINA_TRUE;
}

static Esvg_Element_Descriptor _descriptor = {
	/* .child_add		= */ NULL,
	/* .child_remove	= */ NULL,
	/* .attribute_get 	= */ _esvg_a_attribute_get,
	/* .cdata_set 		= */ NULL,
	/* .text_set 		= */ NULL,
	/* .free 		= */ _esvg_a_free,
	/* .attribute_set 	= */ _esvg_a_attribute_set,
	/* .initialize 		= */ NULL,
	/* .setup		= */ _esvg_a_setup,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_a_new(void)
{
	Esvg_A *thiz;
	Edom_Tag *t;

	thiz = calloc(1, sizeof(Esvg_A));
	if (!thiz) return NULL;

	/* default values */

	t = esvg_element_new(&_descriptor, ESVG_A, thiz);
	return t;
}

static void _esvg_a_href_set(Edom_Tag *t, const char *href)
{
	Esvg_A *thiz;

	thiz = _esvg_a_get(t);
	if (thiz->href)
	{
		free(thiz->href);
		thiz->href = NULL;
	}
	if (href)
		thiz->href = strdup(href);
}

static void _esvg_a_href_get(Edom_Tag *t, const char **href)
{
	Esvg_A *thiz;

	thiz = _esvg_a_get(t);
	if (!href)
		return;
	*href = thiz->href;
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool esvg_is_a_internal(Edom_Tag *t)
{
	if (esvg_element_internal_type_get(t) != ESVG_A)
		return EINA_FALSE;
	return EINA_TRUE;
}

/* The ender wrapper */
#define _esvg_a_href_is_set NULL
#define _esvg_a_delete NULL
#include "generated/esvg_generated_a.c"
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * esvg_a_new(void)
{
	return ender_element_new_with_namespace("a", "esvg");
}

EAPI Eina_Bool esvg_is_a(Ender_Element *e)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	return esvg_is_a_internal(t);
}

EAPI void esvg_a_href_set(Ender_Element *e, const char *href)
{
	ender_element_property_value_set(e, ESVG_A_HREF, href, NULL);
}
