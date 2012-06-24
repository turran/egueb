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
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
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
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* the tag can only be a <g> or a <svg> */
void esvg_input_feed_mouse_move(Ender_Element *e, double x, double y)
{
#if 0
	Esvg_Input_Coord c;
	Edom_Tag *t;
	Enesim_Renderer *r;

	t = ender_element_object_get(e);
	esvg_renderable_internal_renderer_get(t, &r);

	/* FIXME we need to fix this, we are not sure what kind of renderer scheme
	 * each renderable might do
	 */
	enesim_renderer_clipper_content_get(r, &r);
	/* this must be a compound renderer */
	c.x = x;
	c.y = y;
	printf("renderer %p\n", r);
	/* iterate through every renderable */
	enesim_renderer_compound_layer_reverse_foreach(r,
			_esvg_input_find_intersection,
			&c);
#endif
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

