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

#include "egueb_svg_main_private.h"
#include "egueb_svg_rect.h"

#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Value_Descriptor _descriptor;

static void _egueb_svg_rect_data_from(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	egueb_dom_value_primitive_data_from(v, data);
}

static void _egueb_svg_rect_data_to(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	egueb_dom_value_primitive_data_to(v, data);
}

static void _egueb_svg_rect_free(Egueb_Dom_Value *v)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	if (v->owned)
	{
		free(v->data.ptr);
		v->data.ptr = NULL;
	}
}

static void _egueb_svg_rect_copy(const Egueb_Dom_Value *v, Egueb_Dom_Value *copy)
{
	Egueb_Svg_Rect *vl;
	Egueb_Svg_Rect *cl;

	if (!v->data.ptr)
		return;

	if (!copy->data.ptr)
	{
		copy->data.ptr = calloc(1, sizeof(Egueb_Svg_Rect));
		copy->owned = EINA_TRUE;
	}
	cl = copy->data.ptr;
	vl = v->data.ptr;
	*cl = *vl;
}

static char * _egueb_svg_rect_string_to(const Egueb_Dom_Value *v)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_descriptor, NULL);
	return egueb_svg_rect_string_to(v->data.ptr);
}

static Eina_Bool _egueb_svg_rect_string_from(Egueb_Dom_Value *v, const char *str)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_descriptor, EINA_FALSE);
	if (!v->data.ptr)
	{
		v->data.ptr = calloc(1, sizeof(Egueb_Svg_Rect));
		v->owned = EINA_TRUE;
	}
	return egueb_svg_rect_string_from(v->data.ptr, str);
}

static void _egueb_svg_rect_interpolate(Egueb_Dom_Value *v,
		Egueb_Dom_Value *a, Egueb_Dom_Value *b, double m,
		Egueb_Dom_Value *add, Egueb_Dom_Value *acc, int mul)
{
}

static Egueb_Dom_Value_Descriptor _descriptor = {
	/* .data_from 		= */ _egueb_svg_rect_data_from,
	/* .data_from_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .data_to 		= */ _egueb_svg_rect_data_to,
	/* .data_to_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .init 		= */ NULL,
	/* .free 		= */ _egueb_svg_rect_free,
	/* .copy 		= */ _egueb_svg_rect_copy,
	/* .string_to 		= */ _egueb_svg_rect_string_to,
	/* .string_from 	= */ _egueb_svg_rect_string_from,
	/* .interpolate 	= */ _egueb_svg_rect_interpolate,
};
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI const Egueb_Dom_Value_Descriptor * egueb_svg_rect_descriptor_get(void)
{
	return &_descriptor;
}

/* FIXME: fix parsing with ' ' and ',' (do like rgb(c,c,c)) */
Eina_Bool egueb_svg_rect_string_from(Egueb_Svg_Rect *thiz, const char *attr_val)
{
	const char *iter;
	const char *tmp;
	char *endptr;
	double val;
	double *vbp;
	int nbr = 0;

        iter = tmp = attr_val;
	vbp = (double *)thiz;
	while (*tmp)
	{
		while (*tmp)
		{
			if ((*tmp == ' ') || (*tmp == ','))
				tmp++;
			else
			{
				iter = tmp;
				break;
			}
		}
		val = eina_strtod(iter, &endptr);
		if ((errno != ERANGE) &&
		    !((val == 0) && (attr_val == endptr)))
		{
			*vbp = val;
			vbp++;
			tmp = endptr;
			nbr++;
			/* we store only the 4 first numbers */
			if (nbr >= 4)
			{
				return EINA_TRUE;
			}
		}
	}
	return EINA_TRUE;
}

EAPI char * egueb_svg_rect_string_to(Egueb_Svg_Rect *thiz)
{
	ERR("Not implemented");
	return NULL;
}
