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
#include "egueb_svg_renderable_container_private.h"

/* The renderable container abstracts the input handling of events
 * to propagate the input events to its children. We need to keep
 * track in a private list of every renderable child and whenever
 * a mousemove happens, also feed events into its own input object
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Eina_Bool _egueb_svg_renderable_container_children_process_cb(
		Egueb_Dom_Node *child, void *data)
{
	Egueb_Svg_Renderable_Container *thiz = data;
	Egueb_Dom_Node_Type type;

	/* check that it is an element */
	egueb_dom_node_type_get(child, &type);
	if (type != EGUEB_DOM_NODE_TYPE_ELEMENT_NODE)
		return EINA_TRUE;

	/* in case the tree has changed, add the new renderer */
	if (thiz->renderable_tree_changed)
	{
		if (egueb_svg_is_renderable(child))
		{
			Enesim_Renderer *r;
			Enesim_Renderer_Compound_Layer *l;

			r = egueb_svg_renderable_renderer_get(child);
			l = enesim_renderer_compound_layer_new();
			enesim_renderer_compound_layer_renderer_set(l, r);
			enesim_renderer_compound_layer_rop_set(l, ENESIM_BLEND);
			enesim_renderer_compound_layer_add(thiz->compound, l);
		}
	}
	if (!egueb_dom_element_process(child))
		WARN("Child failed to process");
	return EINA_TRUE;
}

static void _egueb_svg_renderable_container_tree_modified_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Svg_Renderable_Container *thiz = data;
	Egueb_Dom_Node *related = NULL;
	Egueb_Dom_Node *target = NULL;

	/* check that the parent is this container */
	egueb_dom_event_mutation_related_get(e, &related);
	if (!related) return;
	if (related != EGUEB_DOM_NODE(thiz))
	{
		egueb_dom_node_unref(related);
		return;
	}
	egueb_dom_node_unref(related);

	/* get the target and check if it is of type renderable */
	egueb_dom_event_target_get(e, &target);
	if (!target) return;

	if (enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(target),
			EGUEB_SVG_RENDERABLE_DESCRIPTOR))
		thiz->renderable_tree_changed = EINA_TRUE;
	egueb_dom_node_unref(target);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_ABSTRACT_BOILERPLATE(EGUEB_SVG_RENDERABLE_DESCRIPTOR,
		Egueb_Svg_Renderable_Container,
		Egueb_Svg_Renderable_Container_Class,
		egueb_svg_renderable_container);

static void _egueb_svg_renderable_container_class_init(void *k)
{
	Egueb_Svg_Renderable_Class *klass;

	klass = EGUEB_SVG_RENDERABLE_CLASS(k);
	klass->renderer_get = egueb_svg_renderable_container_renderer_get;
	klass->process = egueb_svg_renderable_container_process;
}

static void _egueb_svg_renderable_container_class_deinit(void *k)
{
}

static void _egueb_svg_renderable_container_instance_init(void *o)
{
	Egueb_Svg_Renderable_Container *thiz;
	Egueb_Dom_Node *n;
	Enesim_Renderer *r;

	thiz = EGUEB_SVG_RENDERABLE_CONTAINER(o);

	/* the rendering system */
	r = enesim_renderer_compound_new();
	enesim_renderer_compound_background_enable_set(r, EINA_TRUE);
	enesim_renderer_compound_background_color_set(r, 0x00000000);
	thiz->compound = r;

	n = EGUEB_DOM_NODE(o);
	/* our event listeners */
	egueb_dom_node_event_listener_add(n,
			EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED,
			_egueb_svg_renderable_container_tree_modified_cb,
			EINA_FALSE, thiz);
	egueb_dom_node_event_listener_add(n,
			EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED,
			_egueb_svg_renderable_container_tree_modified_cb,
			EINA_FALSE, thiz);
	/* mark it as changed */
	thiz->renderable_tree_changed = EINA_TRUE;
}

static void _egueb_svg_renderable_container_instance_deinit(void *o)
{
	Egueb_Svg_Renderable_Container *thiz;

	thiz = EGUEB_SVG_RENDERABLE_CONTAINER(o);
	enesim_renderer_unref(thiz->compound);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_svg_renderable_container_process(Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Renderable_Container *thiz;

	thiz = EGUEB_SVG_RENDERABLE_CONTAINER(r);
	/* in case the renderable tree has changed, be sure to clear the
	 * compound renderer and init it again
	 */
	if (thiz->renderable_tree_changed)
	{
		enesim_renderer_compound_layer_clear(thiz->compound);
	}
	/* iterate over the children and call the process there */
	egueb_dom_node_children_foreach(EGUEB_DOM_NODE(r),
			_egueb_svg_renderable_container_children_process_cb,
			thiz);
	thiz->renderable_tree_changed = EINA_FALSE;
	return EINA_TRUE;
}

Enesim_Renderer * egueb_svg_renderable_container_renderer_get(
		Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Renderable_Container *thiz;

	thiz = EGUEB_SVG_RENDERABLE_CONTAINER(r);
	return enesim_renderer_ref(thiz->compound);
}

#if 0
static void _egueb_svg_renderable_container_mouse_move(Ender_Element *e,
		const char *event_name, void *event_data, void *data)
{
	Egueb_Svg_Renderable_Container *thiz = data;
	Egueb_Svg_Event_Mouse *ev = event_data;

	egueb_svg_input_feed_mouse_move(thiz->input, ev->screen_x, ev->screen_y);
}

static void _egueb_svg_renderable_container_mouse_down(Ender_Element *e,
		const char *event_name, void *event_data, void *data)
{
	Egueb_Svg_Renderable_Container *thiz = data;
	Egueb_Svg_Event_Mouse *ev = event_data;

	egueb_svg_input_feed_mouse_down(thiz->input, 0);
}

static void _egueb_svg_renderable_container_mouse_up(Ender_Element *e,
		const char *event_name, void *event_data, void *data)
{
	Egueb_Svg_Renderable_Container *thiz = data;
	Egueb_Svg_Event_Mouse *ev = event_data;

	egueb_svg_input_feed_mouse_up(thiz->input, 0);
}

static Ender_Element * _egueb_svg_renderable_container_element_at(void *data, int x, int y)
{
	Egueb_Svg_Renderable_Container *thiz = data;
	Egueb_Dom_Tag *t;
	Eina_List *l;
	Eina_Rectangle in;

	eina_rectangle_coords_from(&in, x, y, 1, 1);
	EINA_LIST_REVERSE_FOREACH(thiz->renderables, l, t)
	{
		Ender_Element *e;
		Enesim_Renderer *r;
		Eina_Rectangle bounds;

		egueb_svg_renderable_internal_renderer_get(t, &r);
		enesim_renderer_destination_bounds_get(r, &bounds, 0, 0);
		//printf("%s: %d %d - %d %d %d %d\n", egueb_dom_tag_name_get(t), x, y, bounds.x, bounds.y, bounds.w, bounds.h);
		if (!eina_rectangles_intersect(&bounds, &in))
			continue;

		e = egueb_svg_element_ender_get(t);
		return e;
	}
	return NULL;
}

static Egueb_Svg_Input_Descriptor _renderable_container_input_descriptor = {
	/* .element_at 	= */ _egueb_svg_renderable_container_element_at,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Svg_Renderable_Container * egueb_svg_renderable_container_new(Ender_Element *e)
{
	Egueb_Svg_Renderable_Container *thiz;

	thiz = calloc(1, sizeof(Egueb_Svg_Renderable_Container));
	ender_event_listener_add(e, "mousemove", _egueb_svg_renderable_container_mouse_move, thiz);
	ender_event_listener_add(e, "mousedown", _egueb_svg_renderable_container_mouse_down, thiz);
	ender_event_listener_add(e, "mouseup", _egueb_svg_renderable_container_mouse_up, thiz);
	/* TODO handle mouse down and mouse up */
	thiz->e = e;
	thiz->input = egueb_svg_input_new(&_renderable_container_input_descriptor, thiz);

	return thiz;
}

void egueb_svg_renderable_container_free(Egueb_Svg_Renderable_Container *thiz)
{
	egueb_svg_renderable_container_clear(thiz);
	ender_event_listener_remove_full(thiz->e, "mousemove", _egueb_svg_renderable_container_mouse_move, thiz);
	ender_event_listener_remove_full(thiz->e, "mousedown", _egueb_svg_renderable_container_mouse_down, thiz);
	ender_event_listener_remove_full(thiz->e, "mouseup", _egueb_svg_renderable_container_mouse_up, thiz);
}

void egueb_svg_renderable_container_clear(Egueb_Svg_Renderable_Container *thiz)
{
	eina_list_free(thiz->renderables);
}


void egueb_svg_renderable_container_renderable_add(Egueb_Svg_Renderable_Container *thiz,
		Egueb_Dom_Tag *t)
{
	//printf("adding element %s\n", egueb_dom_tag_name_get(t));
	thiz->renderables = eina_list_append(thiz->renderables, t);
}

void egueb_svg_renderable_container_renderable_remove(Egueb_Svg_Renderable_Container *thiz,
		Egueb_Dom_Tag *t)
{
	thiz->renderables = eina_list_remove(thiz->renderables, t);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
#endif
EAPI Eina_Bool egueb_svg_is_renderable_container(Egueb_Dom_Node *n)
{
	if (!n) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(n),
			EGUEB_SVG_RENDERABLE_CONTAINER_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}
