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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "esvg_private_main.h"
#include "esvg_private_attribute_presentation.h"
#include "esvg_private_context.h"
#include "esvg_private_element.h"
#include "esvg_private_renderable.h"
#include "esvg_private_input.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Esvg_Input
{
	Edom_Tag *owner;
	Eina_List *over;
	Eina_List *clicked;
	double last_x;
	double last_y;
};

typedef struct _Esvg_Input_Coord
{
	double x;
	double y;
} Esvg_Input_Coord;

static Eina_Bool _esvg_input_find_intersection(Enesim_Renderer *r,
		Enesim_Renderer *layer, void *data)
{
	Esvg_Input_Coord *c = data;
	Enesim_Rectangle bounds;

	 /* intersect the bounds with x, y (maybe call is_inside?) */
	enesim_renderer_boundings(layer, &bounds);
	printf("bounds %" ENESIM_RECTANGLE_FORMAT " and pointer at %g %g\n",
			ENESIM_RECTANGLE_ARGS (&bounds), c->x, c->y);
	return EINA_TRUE;
}

static Eina_Bool _esvg_input_find(Edom_Tag *t, Edom_Tag *child,
		void *data)
{
	Esvg_Input *thiz = data;
	Esvg_Type type;

	type = esvg_element_internal_type_get(child);
	if (!esvg_type_is_renderable(type) && type != ESVG_A)
		return EINA_TRUE;

	if (esvg_type_is_renderable(type))
	{
		Enesim_Renderer *r;
		Eina_Rectangle in;
		Eina_Rectangle bounds;

		esvg_renderable_internal_renderer_get(child, &r);
		eina_rectangle_coords_from(&in, (int)thiz->last_x, (int)thiz->last_y, 1, 1);
		enesim_renderer_destination_boundings(r, &bounds, 0, 0);
		if (!eina_rectangles_intersect(&bounds, &in))
			return EINA_TRUE;

		if (type == ESVG_G || type == ESVG_SVG)
		{

		}
		else
		{

		}
		printf("renderable found! %p\n", r);
	}
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Esvg_Input * esvg_input_new(Edom_Tag *t)
{
	Esvg_Input *thiz;

	thiz = calloc(1, sizeof(Esvg_Input));
	thiz->owner = t;
	return thiz;
}

/* the tag can only be a <g> or a <svg> */
void esvg_input_feed_mouse_move(Esvg_Input *thiz, double x, double y)
{
	thiz->last_x = x;
	thiz->last_y = y;
	edom_tag_child_foreach(thiz->owner, _esvg_input_find, thiz);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

