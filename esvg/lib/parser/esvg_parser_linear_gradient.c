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
#include "esvg_private.h"
#include "esvg_values.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static void _parser_linear_gradient_merge(Enesim_Renderer *r,
			Enesim_Renderer *rel)
{
	if (esvg_linear_gradient_x1_is_set(r))
	{
		Esvg_Coord c;

		esvg_linear_gradient_x1_get(rel, &c);
		esvg_linear_gradient_x1_set(r, &c);
	}
	if (esvg_linear_gradient_y1_is_set(r))
	{
		Esvg_Coord c;

		esvg_linear_gradient_y1_get(rel, &c);
		esvg_linear_gradient_y1_set(r, &c);
	}
	if (esvg_linear_gradient_x2_is_set(r))
	{
		Esvg_Coord c;

		esvg_linear_gradient_x2_get(rel, &c);
		esvg_linear_gradient_x2_set(r, &c);
	}
	if (esvg_linear_gradient_y2_is_set(r))
	{
		Esvg_Coord c;

		esvg_linear_gradient_y2_get(rel, &c);
		esvg_linear_gradient_y2_set(r, &c);
	}
}

static Eina_Bool _parser_linear_gradient_attribute_set(Edom_Tag *tag, const char *key, const char *value)
{
	Enesim_Renderer *r;

	r = esvg_parser_element_renderer_get(tag);

	if (strcmp(key, "x1") == 0)
	{
		Esvg_Coord x;

		esvg_length_get(&x, value, ESVG_COORD_0);
		esvg_linear_gradient_x1_set(r, &x);
	}
	else if (strcmp(key, "y1") == 0)
	{
		Esvg_Coord y;

		esvg_length_get(&y, value, ESVG_COORD_0);
		esvg_linear_gradient_y1_set(r, &y);
	}
	else if (strcmp(key, "x2") == 0)
	{
		Esvg_Coord x;

		esvg_length_get(&x, value, ESVG_COORD_0);
		esvg_linear_gradient_x2_set(r, &x);
	}
	else if (strcmp(key, "y2") == 0)
	{
		Esvg_Coord y;

		esvg_length_get(&y, value, ESVG_COORD_0);
		esvg_linear_gradient_y2_set(r, &y);
	}

	return EINA_TRUE;
}

static const char * _parser_linear_gradient_attribute_get(Edom_Tag *tag, const char *attribute)
{
	return NULL;
}

static const char * _parser_linear_gradient_name_get(Edom_Tag *tag)
{
	return "linearGradient";
}

static Esvg_Parser_Gradient_Descriptor _descriptor = {
	/* .merge 		= */ _parser_linear_gradient_merge,
	/* .name_get 		= */ _parser_linear_gradient_name_get,
	/* .attribute_set 	= */ _parser_linear_gradient_attribute_set,
	/* .attribute_get 	= */ _parser_linear_gradient_attribute_get,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Edom_Tag * esvg_parser_linear_gradient_new(Edom_Context *c, Edom_Tag *topmost)
{
	Edom_Tag *tag;
	Enesim_Renderer *r;

	r = esvg_linear_gradient_new();
	tag = esvg_parser_gradient_new(c, &_descriptor, ESVG_LINEARGRADIENT,
			topmost, r, NULL);
	return tag;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

