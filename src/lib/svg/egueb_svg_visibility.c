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
#include "egueb_svg_main_private.h"
#include "egueb_svg_visibility.h"

#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Value_Descriptor _descriptor;

static void _egueb_svg_visibility_data_from(Egueb_Dom_Value *v,
		Egueb_Dom_Value_Data *data)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	v->data.d = data->d;
}

static void _egueb_svg_visibility_data_to(Egueb_Dom_Value *v,
		Egueb_Dom_Value_Data *data)
{
	Egueb_Svg_Visibility *ptr;
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	ptr = data->ptr;
	if (!ptr) return;
	*ptr = v->data.i32;
}

static char * _egueb_svg_visibility_string_to(const Egueb_Dom_Value *v)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_descriptor, NULL);
	return egueb_svg_visibility_string_to(v->data.i32);
}

static Eina_Bool _egueb_svg_visibility_string_from(Egueb_Dom_Value *v, const char *str)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_descriptor, EINA_FALSE);
	return egueb_svg_visibility_string_from((Egueb_Svg_Visibility *)&v->data.i32, str);
}

static void _egueb_svg_visibility_interpolate(Egueb_Dom_Value *v,
		Egueb_Dom_Value *a, Egueb_Dom_Value *b, double m,
		Egueb_Dom_Value *add, Egueb_Dom_Value *acc, int mul)
{
}

static Egueb_Dom_Value_Descriptor _descriptor = {
	/* .data_from 		= */ _egueb_svg_visibility_data_from,
	/* .data_from_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_INT32,
	/* .data_to 		= */ _egueb_svg_visibility_data_to,
	/* .data_to_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .init 		= */ NULL,
	/* .free 		= */ NULL,
	/* .copy 		= */ NULL,
	/* .string_to 		= */ _egueb_svg_visibility_string_to,
	/* .string_from 	= */ _egueb_svg_visibility_string_from,
	/* .interpolate 	= */ _egueb_svg_visibility_interpolate,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI const Egueb_Dom_Value_Descriptor * egueb_svg_visibility_descriptor_get(void)
{
	return &_descriptor;
}

EAPI Eina_Bool egueb_svg_visibility_string_from(Egueb_Svg_Visibility *v,
		const char *attr)
{
	if (!strcmp(attr, "visible"))
		*v = EGUEB_SVG_VISIBILITY_VISIBLE;
	else if (!strcmp(attr, "hidden"))
		*v = EGUEB_SVG_VISIBILITY_HIDDEN;
	else if (!strcmp(attr, "collapse"))
		*v = EGUEB_SVG_VISIBILITY_COLLAPSE;
	else
		return EINA_FALSE;
	return EINA_TRUE;
}

EAPI char * egueb_svg_visibility_string_to(Egueb_Svg_Visibility v)
{
	switch (v)
	{
		case EGUEB_SVG_VISIBILITY_VISIBLE:
		return strdup("visible");

		case EGUEB_SVG_VISIBILITY_HIDDEN:
		return strdup("hidden");

		case EGUEB_SVG_VISIBILITY_COLLAPSE:
		return strdup("collapse");

		default:
		return NULL;
	}
}
