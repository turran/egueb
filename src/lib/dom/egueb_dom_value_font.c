/* Egueb
 * Copyright (C) 2011 - 2013 Jorge Luis Zapata
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

#include "egueb_dom_private.h"
#include "egueb_dom_value.h"
#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/
static Eina_Bool egueb_dom_value_font_string_from(Egueb_Css_Font *thiz,
		const char *str)
{
	return egueb_css_font_string_from(thiz, str);
}

static char * egueb_dom_value_font_string_to(Egueb_Css_Font *thiz)
{
	return egueb_css_font_string_to(thiz);
}

static void egueb_dom_value_font_interpolate(Egueb_Css_Font *v,
		Egueb_Css_Font *a, Egueb_Css_Font *b, double m,
		Egueb_Css_Font *add, Egueb_Css_Font *acc, int mul)
{
	ERR("Not implemented");
}

EGUEB_DOM_VALUE_PRIMITIVE_SIMPLE_BOILERPLATE(egueb_dom_value_font, Egueb_Css_Font);
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI const Egueb_Dom_Value_Descriptor * egueb_dom_value_font_descriptor_get(void)
{
	return &_egueb_dom_value_font_descriptor;
}
