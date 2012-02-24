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

#include "Esvg.h"
#include "Esvg_Parser.h"
#include "esvg_parser_private.h"
#include "esvg_values.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static void _parser_radial_gradient_merge(Enesim_Renderer *r,
			Enesim_Renderer *rel)
{
}

static Eina_Bool _parser_radial_gradient_attribute_set(Edom_Tag *tag, const char *key, const char *value)
{
	Enesim_Renderer *r;

	r = esvg_parser_element_renderer_get(tag);
	if (strcmp(key, "cx") == 0)
	{
		Esvg_Coord cx;

		esvg_length_get(&cx, value, ESVG_COORD_0);
		esvg_radial_gradient_cx_set(r, &cx);
	}
	else if (strcmp(key, "cy") == 0)
	{
		Esvg_Coord cy;

		esvg_length_get(&cy, value, ESVG_COORD_0);
		esvg_radial_gradient_cy_set(r, &cy);
	}
	else if (strcmp(key, "r") == 0)
	{
		Esvg_Length rad;

		esvg_length_get(&rad, value, ESVG_LENGTH_0);
		esvg_radial_gradient_r_set(r, &rad);
	}
	else if (strcmp(key, "fx") == 0)
	{
		Esvg_Length fx;

		esvg_length_get(&fx, value, ESVG_LENGTH_0);
		esvg_radial_gradient_fx_set(r, &fx);
	}
	else if (strcmp(key, "fy") == 0)
	{
		Esvg_Length fy;

		esvg_length_get(&fy, value, ESVG_LENGTH_0);
		esvg_radial_gradient_fy_set(r, &fy);
	}

	return EINA_TRUE;
}

static const char * _parser_radial_gradient_attribute_get(Edom_Tag *tag, const char *attribute)
{
	return NULL;
}

static const char * _parser_radial_gradient_name_get(Edom_Tag *tag)
{
	return "radialGradient";
}

static Esvg_Parser_Gradient_Descriptor _descriptor = {
	/* .merge 		= */ _parser_radial_gradient_merge,
	/* .name_get 		= */ _parser_radial_gradient_name_get,
	/* .attribute_set 	= */ _parser_radial_gradient_attribute_set,
	/* .attribute_get 	= */ _parser_radial_gradient_attribute_get,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Edom_Tag * esvg_parser_radial_gradient_new(Edom_Parser *parser)
{
	Edom_Tag *tag;
	Enesim_Renderer *r;

	r = esvg_radial_gradient_new();
	tag = esvg_parser_gradient_new(parser, &_descriptor, ESVG_RADIALGRADIENT,
			r, NULL);

	return tag;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
