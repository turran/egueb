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

#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Value_Descriptor _descriptor;

static void _egueb_svg_paint_data_from(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	egueb_dom_value_primitive_data_from(v, data);
}

static void _egueb_svg_paint_data_to(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	egueb_dom_value_primitive_data_to(v, data);
}

static void _egueb_svg_paint_free(Egueb_Dom_Value *v)
{
	if (v->owned)
	{
		egueb_svg_paint_reset(v->data.ptr);
		free(v->data.ptr);
		v->data.ptr = NULL;
	}
}

static void _egueb_svg_paint_copy(const Egueb_Dom_Value *v, Egueb_Dom_Value *copy)
{
	Egueb_Svg_Paint *vd = v->data.ptr;
	Egueb_Svg_Paint *cd;

	if (!vd) return;
	if (!copy->data.ptr)
	{
		copy->data.ptr = calloc(1, sizeof(Egueb_Svg_Paint));
		copy->owned = EINA_TRUE;
	}
	cd = copy->data.ptr;
	egueb_svg_paint_copy(vd, cd);
}

static char * _egueb_svg_paint_string_to(const Egueb_Dom_Value *v)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_descriptor, NULL);
	return egueb_svg_paint_string_to(v->data.ptr);
}

static Eina_Bool _egueb_svg_paint_string_from(Egueb_Dom_Value *v, const char *str)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_descriptor, EINA_FALSE);
	if (!v->data.ptr)
	{
		v->data.ptr = calloc(1, sizeof(Egueb_Svg_Paint));
		v->owned = EINA_TRUE;
	}
	return egueb_svg_paint_string_from(v->data.ptr, str);
}

static Egueb_Dom_Value_Descriptor _descriptor = {
	/* .data_from 		= */ _egueb_svg_paint_data_from,
	/* .data_from_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .data_to 		= */ _egueb_svg_paint_data_to,
	/* .data_to_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .init 		= */ NULL,
	/* .free 		= */ _egueb_svg_paint_free,
	/* .copy 		= */ _egueb_svg_paint_copy,
	/* .string_to 		= */ _egueb_svg_paint_string_to,
	/* .string_from 	= */ _egueb_svg_paint_string_from,
	/* .interpolate 	= */ NULL,
};
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

EAPI const Egueb_Dom_Value_Descriptor * egueb_svg_paint_descriptor_get(void)
{
	return &_descriptor;
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
	ERR("Not implemented");
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

EAPI void egueb_svg_paint_copy(const Egueb_Svg_Paint *thiz, Egueb_Svg_Paint *copy)
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
