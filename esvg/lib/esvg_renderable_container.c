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
#include "esvg_private_referenceable.h"
#include "esvg_private_types.h"

#include "esvg_renderable_container.h"
/* The renderable container abstracts the input handling of events
 * to propagate the input events to its children. We need to keep
 * track in a private list of every renderable child and whenever
 * a mousemove happens, also feed events into its own input object
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Esvg_Renderable_Descriptor_Internal
{
	/* tag interface */
	Edom_Tag_Free free;
	/* element interface */
	Esvg_Element_Initialize initialize;
	/* renderable interface */
	Esvg_Renderable_Setup setup;
	Esvg_Renderable_Renderer_Propagate renderer_propagate;
	/* renderable container interface */
	Esvg_Renderable_Container_Renderable_Add renderable_add;
	Esvg_Renderable_Container_Renderable_Remove renderable_remove;
} Esvg_Renderable_Descriptor_Internal;

typedef struct _Esvg_Renderable_Container
{
	/* properties */
	/* interface */
	Esvg_Renderable_Descriptor_Internal descriptor;
	/* private */
	Esvg_Input *input;
	Eina_List *renderables;
} Esvg_Renderable_Container;

static void _esvg_renderable_container_mouse_move(Ender_Element *e,
		const char *event_name, void *event_data, void *data)
{

}

static void _esvg_renderable_container_initialize(Ender_Element *e)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Edom_Tag * esvg_renderable_container_new(Esvg_Renderable_Container_Descriptor *descriptor, Esvg_Type type,
		void *data)
{
	Esvg_Renderable_Container *thiz;
	Esvg_Renderable_Descriptor pdescriptor;

	/* our own descriptor */
	/* parent descriptor */
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

