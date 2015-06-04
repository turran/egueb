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
#include "egueb_svg_event_request_painter_private.h"

/* The renderable container abstracts the input handling of events
 * to propagate the input events to its children. We need to keep
 * track in a private list of every renderable child and whenever
 * a mousemove happens, also feed events into its own input object
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static void _egueb_svg_renderable_container_request_painter_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Dom_Node *n;
	Egueb_Svg_Painter *painter;

	n = egueb_dom_event_target_get(e);
	DBG_ELEMENT(n, "Setting the generic painter on the renderable");
	painter = egueb_svg_renderable_class_painter_get(n);
	egueb_svg_event_request_painter_painter_set(e, painter);
	egueb_dom_node_unref(n);
}

static Eina_Bool _egueb_svg_renderable_container_children_process_cb(
		Egueb_Dom_Node *child, void *data)
{
	Egueb_Svg_Renderable_Container *thiz = data;
	Egueb_Dom_Node_Type type;

	/* check that it is an element */
	type = egueb_dom_node_type_get(child);
	if (type != EGUEB_DOM_NODE_TYPE_ELEMENT)
		return EINA_TRUE;

	/* in case the tree has changed, add the new renderer */
	if (thiz->renderable_tree_changed)
	{
		if (egueb_svg_is_renderable(child))
		{
			Enesim_Renderer *r;
			Enesim_Renderer_Compound_Layer *l;

			r = egueb_svg_renderable_renderer_get(child);
			DBG_ELEMENT(EGUEB_DOM_NODE(thiz), "Adding renderable %s",
					enesim_renderer_name_get(r));
			l = enesim_renderer_compound_layer_new();
			enesim_renderer_compound_layer_renderer_set(l, r);
			enesim_renderer_compound_layer_rop_set(l, ENESIM_ROP_BLEND);
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
	Egueb_Dom_Node *n;

	n = EGUEB_DOM_NODE(thiz);
	/* check that the parent is this container */
	related = egueb_dom_event_mutation_related_get(e);
	if (!related) return;
	if (related != n)
	{
		egueb_dom_node_unref(related);
		return;
	}
	egueb_dom_node_unref(related);

	/* get the target and check if it is of type renderable */
	target = egueb_dom_event_target_get(e);
	if (!target) return;

	if (enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(target),
			EGUEB_SVG_RENDERABLE_DESCRIPTOR))
	{
		thiz->renderable_tree_changed = EINA_TRUE;
		egueb_dom_element_enqueue(egueb_dom_node_ref(n));
	}
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
	egueb_dom_event_target_event_listener_add(n,
			EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED,
			_egueb_svg_renderable_container_tree_modified_cb,
			EINA_FALSE, thiz);
	egueb_dom_event_target_event_listener_add(n,
			EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED,
			_egueb_svg_renderable_container_tree_modified_cb,
			EINA_FALSE, thiz);
	egueb_dom_event_target_event_listener_add(n,
			EGUEB_SVG_EVENT_REQUEST_PAINTER,
			_egueb_svg_renderable_container_request_painter_cb,
			EINA_FALSE, NULL);
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

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool egueb_svg_is_renderable_container(Egueb_Dom_Node *n)
{
	if (!n) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(n),
			EGUEB_SVG_RENDERABLE_CONTAINER_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}
