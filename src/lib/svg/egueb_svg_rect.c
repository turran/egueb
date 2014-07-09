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
/* FIXME: fix parsing with ' ' and ',' (do like rgb(c,c,c)) */
static Eina_Bool egueb_svg_rect_string_from(Egueb_Svg_Rect *thiz, const char *attr_val)
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
		if (!egueb_dom_double_get(iter, &endptr, &val))
			return EINA_FALSE;

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
	return EINA_TRUE;
}

static char * egueb_svg_rect_string_to(Egueb_Svg_Rect *thiz)
{
	ERR("Not implemented");
	return NULL;
}

static void egueb_svg_rect_interpolate(Egueb_Svg_Rect *v,
		Egueb_Svg_Rect *a, Egueb_Svg_Rect *b, double m,
		Egueb_Svg_Rect *add, Egueb_Svg_Rect *acc, int mul)
{
	ERR("Not implemented");
}

EGUEB_DOM_VALUE_PRIMITIVE_SIMPLE_BOILERPLATE(egueb_svg_rect, Egueb_Svg_Rect);
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI const Egueb_Dom_Value_Descriptor * egueb_svg_rect_descriptor_get(void)
{
	return &_egueb_svg_rect_descriptor;
}


