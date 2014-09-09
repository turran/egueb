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
#include "egueb_css_private.h"
#include "egueb_css_font_family.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_css_font_family_value_string_from(
		Egueb_Css_Font_Family_Value *thiz, const char *attr_val)
{
	return EINA_FALSE;
}

char * egueb_css_font_family_value_string_to(Egueb_Css_Font_Family_Value *thiz)
{
	return NULL;
}

void egueb_css_font_family_value_reset(Egueb_Css_Font_Family_Value *thiz)
{

}

void egueb_css_font_family_value_copy(Egueb_Css_Font_Family_Value *src,
		Egueb_Css_Font_Family_Value *dst, Eina_Bool content)
{

}

void egueb_css_font_family_value_interpolate(Egueb_Css_Font_Family_Value *v,
		Egueb_Css_Font_Family_Value *a, Egueb_Css_Font_Family_Value *b,
		double m, Egueb_Dom_Value *add, Egueb_Dom_Value *acc, int mul)
{

}

Eina_Bool egueb_css_font_family_string_from(Egueb_Dom_List *thiz, const char *attr_val)
{
	return EINA_FALSE;
}

char * egueb_css_font_family_string_to(Egueb_Dom_List *thiz)
{
	return NULL;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

