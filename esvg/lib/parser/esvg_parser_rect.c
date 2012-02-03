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
static Eina_Bool _parser_rect_attribute_set(Edom_Tag *tag, const char *key, const char *value)
{
	Enesim_Renderer *r;

	r = esvg_parser_element_renderer_get(tag);
	if (strcmp(key, "x") == 0)
	{
		Esvg_Coord x;

		esvg_length_get(&x, value, ESVG_COORD_0);
		esvg_rect_x_set(r, &x);
	}
	else if (strcmp(key, "y") == 0)
	{
		Esvg_Coord y;

		esvg_length_get(&y, value, ESVG_COORD_0);
		esvg_rect_y_set(r, &y);
	}
	else if (strcmp(key, "rx") == 0)
	{
		Esvg_Coord rx;

		esvg_length_get(&rx, value, ESVG_COORD_0);
		esvg_rect_rx_set(r, &rx);
	}
	else if (strcmp(key, "ry") == 0)
	{
		Esvg_Coord ry;

		esvg_length_get(&ry, value, ESVG_COORD_0);
		esvg_rect_ry_set(r, &ry);
	}
	else if (strcmp(key, "width") == 0)
	{
		Esvg_Length width;

		esvg_length_get(&width, value, ESVG_LENGTH_0);
		esvg_rect_width_set(r, &width);
	}
	else if (strcmp(key, "height") == 0)
	{
		Esvg_Length height;

		esvg_length_get(&height, value, ESVG_LENGTH_0);
		esvg_rect_height_set(r, &height);
	}

	return EINA_TRUE;
}

static const char * _parser_rect_attribute_get(Edom_Tag *tag, const char *attribute)
{
	return NULL;
}

static const char * _parser_rect_name_get(Edom_Tag *tag)
{
	return "rect";
}

static Edom_Tag_Descriptor _descriptor = {
	/* .name_get 		= */ _parser_rect_name_get,
	/* .attribute_set 	= */ _parser_rect_attribute_set,
	/* .attribute_get 	= */ _parser_rect_attribute_get,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Edom_Tag * esvg_parser_rect_new(Edom_Context *c, Edom_Tag *topmost)
{
	Edom_Tag *tag;
	Enesim_Renderer *r;

	r = esvg_rect_new();
	tag = esvg_parser_shape_new(c, &_descriptor, ESVG_RECT, topmost, r, NULL);

	return tag;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

