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
/*
 * In order to support styling we need to keep track of every element
 * parsed, that is, generate the DOM tree. This way we can support Ecss
 * context to know whenver an element has to be applied with a specific
 * style
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Esvg_Style
{
	Ecss_Style *s;
} Esvg_Style;

static Esvg_Style * _esvg_style_get(Edom_Tag *t)
{
	Esvg_Style *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_STYLE)
		return NULL;
	thiz = esvg_element_data_get(t);

	return thiz;
}
/*----------------------------------------------------------------------------*
 *                          Css context interface                             *
 *----------------------------------------------------------------------------*/
static const char * _get_name(void *e)
{

}

static const char * _property_get(void *e, const char *property)
{
	Edom_Tag *tag = e;

	if (!strcmp(property, "class"))
	{
		return edom_tag_class_get(e);
	}
	else if (!strcmp(property, "id"))
	{
		return edom_tag_id_get(e);
	}
	else
	{
		return NULL;
	}
}

static void _property_set(void *e, const char *property, const char *value)
{
	Edom_Tag *tag = e;

	printf("setting %s %s\n", property, value);
#if 0
	edom_tag_attribute_set(tag, property, value);
#endif
}

static void * _get_child(void *e)
{
	Edom_Tag *tag = e;
	return edom_tag_child_get(tag);
}

static void * _get_next_sibling(void *e)
{
	Edom_Tag *tag = e;
	return edom_tag_next_get(tag);
}

static Ecss_Context _context = {
	/* .property_set 	= */ _property_set,
	/* .property_get 	= */ _property_get,
	/* .get_name 		= */ _get_name,
	/* .get_child 		= */ _get_child,
	/* .get_next_sibling 	= */ _get_next_sibling,
};

/*----------------------------------------------------------------------------*
 *                         The Esvg Element interface                         *
 *----------------------------------------------------------------------------*/
static void _esvg_style_cdata(Edom_Tag *t, const char *cdata, unsigned int length)
{
	Esvg_Style *thiz;
	Ecss_Style *s;

	thiz = _esvg_style_get(t);

	printf("SETTING CDATA!!!\n");
	s = ecss_style_load_from_content(cdata, length);
	if (!s) return;

	thiz->s = s;
}

static void _esvg_style_free(Edom_Tag *t)
{
	Esvg_Style *thiz;

	thiz = _esvg_style_get(t);
	free(thiz);
}

static Esvg_Element_Descriptor _descriptor = {
	/* .child_add		= */ NULL,
	/* .child_remove	= */ NULL,
	/* .attribute_get 	= */ NULL,
	/* .cdata_set 		= */ _esvg_style_cdata,
	/* .text_set 		= */ NULL,
	/* .free 		= */ _esvg_style_free,
	/* .attribute_set 	= */ NULL,
	/* .initialize 		= */ NULL,
	/* .clone		= */ NULL,
	/* .setup		= */ NULL,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_style_new(void)
{
	Esvg_Style *thiz;
	Edom_Tag *t;

	thiz = calloc(1, sizeof(Esvg_Style));
	if (!thiz) return NULL;

	/* default values */

	t = esvg_element_new(&_descriptor, ESVG_STYLE, thiz);
	return t;
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* The ender wrapper */
#include "generated/esvg_generated_style.c"

void esvg_parser_style_style_set(Esvg_Style *thiz, Ecss_Style *style)
{
	thiz->s = style;
}

void esvg_parser_style_apply(Esvg_Style *thiz, Edom_Tag *root)
{
	ecss_context_style_apply(&_context, thiz->s, root);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * esvg_style_new(void)
{
	return ender_element_new_with_namespace("style", "esvg");
}
