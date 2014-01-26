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
#include "egueb_svg_color.h"
#include "egueb_svg_paint.h"
#include "egueb_svg_string.h"

#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/
#define _egueb_svg_paint_interpolate NULL
EGUEB_DOM_VALUE_PRIMITIVE_BOLIERPLATE(egueb_svg_paint, Egueb_Svg_Paint);
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
const Egueb_Svg_Paint EGUEB_SVG_PAINT_BLACK = {
	EGUEB_SVG_PAINT_TYPE_COLOR,
	{ 0, 0, 0 },
	NULL
};

const Egueb_Svg_Paint EGUEB_SVG_PAINT_NONE = {
	EGUEB_SVG_PAINT_TYPE_NONE,
	{ 0, 0, 0 },
	NULL
};

const Egueb_Svg_Paint EGUEB_SVG_PAINT_CURRENT_COLOR = {
	EGUEB_SVG_PAINT_TYPE_CURRENT_COLOR,
	{ 0, 0, 0 },
	NULL
};

EAPI const Egueb_Dom_Value_Descriptor * egueb_svg_paint_descriptor_get(void)
{
	return &_egueb_svg_paint_descriptor;
}

/*
 * none, currentColor, <color>, <uri>?
 */
EAPI Eina_Bool egueb_svg_paint_string_from(Egueb_Svg_Paint *paint, const char *attr)
{
	/* none */
	if (strncmp(attr, "none", 4) == 0)
	{
		paint->type = EGUEB_SVG_PAINT_TYPE_NONE;
	}
	/* currentColor */
	else if (strncmp(attr, "currentColor", 12) == 0)
	{
		paint->type = EGUEB_SVG_PAINT_TYPE_CURRENT_COLOR;
	}
	/* color name */
	else if (egueb_svg_color_string_from(&paint->color, attr))
	{
		paint->type = EGUEB_SVG_PAINT_TYPE_COLOR;
	}
	/* uri */
	else
	{
		paint->type = EGUEB_SVG_PAINT_TYPE_SERVER;
		paint->uri = strdup(attr);
	}

	return EINA_TRUE;
}

EAPI char * egueb_svg_paint_string_to(Egueb_Svg_Paint *thiz)
{
	switch (thiz->type)
	{
		case EGUEB_SVG_PAINT_TYPE_NONE:
		return strdup("none");
		break;

		case EGUEB_SVG_PAINT_TYPE_CURRENT_COLOR:
		return strdup("currentColor");
		break;

		case EGUEB_SVG_PAINT_TYPE_COLOR:
		return egueb_svg_color_string_to(&thiz->color);
		break;

		case EGUEB_SVG_PAINT_TYPE_SERVER:
		return strdup(thiz->uri);
		break;

		default:
		break;
	}
	return NULL;
}

EAPI Eina_Bool egueb_svg_paint_is_equal(const Egueb_Svg_Paint *p1,
		const Egueb_Svg_Paint *p2)
{
	/* sanity checks */
	if (p1 == p2) return EINA_TRUE;
	if (!p1) return EINA_FALSE;
	if (!p2) return EINA_FALSE;

	if (p1->type != p2->type) return EINA_FALSE;
	/* ok, we have values and same types, now compare each type */
	switch (p1->type)
	{
		case EGUEB_SVG_PAINT_TYPE_NONE:
		case EGUEB_SVG_PAINT_TYPE_CURRENT_COLOR:
		return EINA_TRUE;

		case EGUEB_SVG_PAINT_TYPE_COLOR:
		return egueb_svg_color_is_equal(&p1->color, &p2->color);

		case EGUEB_SVG_PAINT_TYPE_SERVER:
		return egueb_svg_string_is_equal(p1->uri, p2->uri);

		/* FIXME what to do in this cases? add an assert? */
		default:
		return EINA_FALSE;
	}
}

EAPI void egueb_svg_paint_copy(const Egueb_Svg_Paint *thiz, Egueb_Svg_Paint *copy, Eina_Bool full)
{
	egueb_svg_paint_reset(copy);

	/* common */
	copy->color = thiz->color;
	copy->type = thiz->type;
	if (thiz->type == EGUEB_SVG_PAINT_TYPE_SERVER)
	{
		if (thiz->uri)
		{
			copy->uri = strdup(thiz->uri);
		}
	}
}

EAPI void egueb_svg_paint_reset(Egueb_Svg_Paint *thiz)
{
	if (thiz->type == EGUEB_SVG_PAINT_TYPE_SERVER)
	{
		if (thiz->uri)
		{
			free(thiz->uri);
			thiz->uri = NULL;
		}
	}
}
