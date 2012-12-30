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
#include "esvg_private_main.h"
#include "esvg_private_attribute_presentation.h"
#include "esvg_private_context.h"
#include "esvg_private_element.h"
#include "esvg_private_style.h"

#include "esvg_element_style.h"
/*
 * In order to support styling we need to keep track of every element
 * parsed, that is, generate the DOM tree. This way we can support Ecss
 * context to know whenver an element has to be applied with a specific
 * style
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Esvg_Element_Style
{
	Ecss_Style *s;
} Esvg_Element_Style;

static Esvg_Element_Style * _esvg_element_style_get(Edom_Tag *t)
{
	Esvg_Element_Style *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_TYPE_STYLE)
		return NULL;
	thiz = esvg_element_data_get(t);

	return thiz;
}
/*----------------------------------------------------------------------------*
 *                         The Esvg Element interface                         *
 *----------------------------------------------------------------------------*/
static void _esvg_element_style_cdata(Edom_Tag *t, const char *cdata, unsigned int length)
{
	Esvg_Element_Style *thiz;
	Ecss_Style *s;

	thiz = _esvg_element_style_get(t);

	s = ecss_style_load_from_content(cdata, length);
	if (!s) return;

	thiz->s = s;
}

static void _esvg_element_style_free(Edom_Tag *t)
{
	Esvg_Element_Style *thiz;

	thiz = _esvg_element_style_get(t);
	free(thiz);
}

static Esvg_Element_Descriptor _descriptor = {
	/* .child_add		= */ NULL,
	/* .child_remove	= */ NULL,
	/* .attribute_get 	= */ NULL,
	/* .cdata_set 		= */ _esvg_element_style_cdata,
	/* .text_set 		= */ NULL,
	/* .text_get 		= */ NULL,
	/* .free 		= */ _esvg_element_style_free,
	/* .attribute_set 	= */ NULL,
	/* .initialize 		= */ NULL,
	/* .setup		= */ NULL,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_element_style_new(void)
{
	Esvg_Element_Style *thiz;
	Edom_Tag *t;

	thiz = calloc(1, sizeof(Esvg_Element_Style));
	if (!thiz) return NULL;

	/* default values */

	t = esvg_element_new(&_descriptor, ESVG_TYPE_STYLE, thiz);
	return t;
}

/* The ender wrapper */
#define _esvg_element_style_delete NULL
#include "generated/esvg_generated_element_style.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_element_style_init(void)
{
	_esvg_element_style_init();
}

void esvg_element_style_shutdown(void)
{
	_esvg_element_style_shutdown();
}

Eina_Bool esvg_element_style_is_internal(Edom_Tag *t)
{
	if (esvg_element_internal_type_get(t) == ESVG_TYPE_STYLE)
		return EINA_TRUE;
	return EINA_FALSE;
}

void esvg_element_style_apply(Edom_Tag *t, Edom_Tag *root)
{
	Esvg_Element_Style *thiz;

	thiz = _esvg_element_style_get(t);
	esvg_element_ecss_style_apply(root, thiz->s);
}

#if 0
void esvg_parser_style_style_set(Esvg_Element_Style *thiz, Ecss_Style *style)
{
	thiz->s = style;
}

#endif
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * esvg_element_style_new(void)
{
	return ESVG_ELEMENT_NEW("SVGStyleElement");
}
