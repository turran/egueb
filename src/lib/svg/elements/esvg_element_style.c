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
#include "egueb_svg_element_style_private.h"

#include "egueb_svg_element_style.h"
/*
 * In order to support styling we need to keep track of every element
 * parsed, that is, generate the DOM tree. This way we can support Ecss
 * context to know whenver an element has to be applied with a specific
 * style
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Egueb_Svg_Element_Style
{
	Ecss_Style *s;
} Egueb_Svg_Element_Style;

static Egueb_Svg_Element_Style * _egueb_svg_element_style_get(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element_Style *thiz;

	if (egueb_svg_element_internal_type_get(t) != ESVG_TYPE_STYLE)
		return NULL;
	thiz = egueb_svg_element_data_get(t);

	return thiz;
}
/*----------------------------------------------------------------------------*
 *                         The Esvg Element interface                         *
 *----------------------------------------------------------------------------*/
static void _egueb_svg_element_style_cdata(Egueb_Dom_Tag *t, const char *cdata, unsigned int length)
{
	Egueb_Svg_Element_Style *thiz;
	Ecss_Style *s;

	thiz = _egueb_svg_element_style_get(t);

	s = ecss_style_load_from_content(cdata, length);
	if (!s) return;

	thiz->s = s;
}

static void _egueb_svg_element_style_free(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element_Style *thiz;

	thiz = _egueb_svg_element_style_get(t);
	free(thiz);
}

static Egueb_Svg_Element_Descriptor _descriptor = {
	/* .child_add		= */ NULL,
	/* .child_remove	= */ NULL,
	/* .attribute_get 	= */ NULL,
	/* .cdata_set 		= */ _egueb_svg_element_style_cdata,
	/* .text_set 		= */ NULL,
	/* .text_get 		= */ NULL,
	/* .free 		= */ _egueb_svg_element_style_free,
	/* .attribute_set 	= */ NULL,
	/* .initialize 		= */ NULL,
	/* .setup		= */ NULL,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Tag * _egueb_svg_element_style_new(void)
{
	Egueb_Svg_Element_Style *thiz;
	Egueb_Dom_Tag *t;

	thiz = calloc(1, sizeof(Egueb_Svg_Element_Style));
	if (!thiz) return NULL;

	/* default values */

	t = egueb_svg_element_new(&_descriptor, ESVG_TYPE_STYLE, thiz);
	return t;
}

/* The ender wrapper */
#define _egueb_svg_element_style_delete NULL
#include "egueb_svg_generated_element_style.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_svg_element_style_init(void)
{
	_egueb_svg_element_style_init();
}

void egueb_svg_element_style_shutdown(void)
{
	_egueb_svg_element_style_shutdown();
}

Eina_Bool egueb_svg_element_style_is_internal(Egueb_Dom_Tag *t)
{
	if (egueb_svg_element_internal_type_get(t) == ESVG_TYPE_STYLE)
		return EINA_TRUE;
	return EINA_FALSE;
}

void egueb_svg_element_style_apply(Egueb_Dom_Tag *t, Egueb_Dom_Tag *root)
{
	Egueb_Svg_Element_Style *thiz;

	thiz = _egueb_svg_element_style_get(t);
	egueb_svg_element_ecss_style_apply(root, thiz->s);
}

#if 0
void egueb_svg_parser_style_style_set(Egueb_Svg_Element_Style *thiz, Ecss_Style *style)
{
	thiz->s = style;
}

#endif
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * egueb_svg_element_style_new(void)
{
	return ESVG_ELEMENT_NEW("SVGStyleElement");
}
