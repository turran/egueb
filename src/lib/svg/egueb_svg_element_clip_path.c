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
#include "egueb_svg_main.h"
#include "egueb_svg_reference.h"
#include "egueb_svg_referenceable.h"
#include "egueb_svg_referenceable_units.h"
#include "egueb_svg_string.h"
#include "egueb_svg_attr_referenceable_units.h"
#include "egueb_svg_attr_string.h"
#include "egueb_svg_element_clip_path.h"
#include "egueb_svg_element_g.h"
#include "egueb_svg_element_use.h"
#include "egueb_svg_document.h"

#include "egueb_svg_referenceable_private.h"
#include "egueb_svg_reference_private.h"
#include "egueb_svg_element_clip_path_private.h"

/* FIXME remove this */
#include "egueb_dom_document_private.h"
/*
 * Basically a clipPath has childs of any shape type and when the element
 * is referenced, the object that refers to the clipPath should use a new
 * reference. For now, a reference always stores a renderer, but that
 * needs to change, given that what we need to do is to create a new dom
 * tree (a clone) that will be used to setup the renderers on the correct
 * units space.
 *
 * if a clipping path has more than one child then the clip area is the union
 * of every child. that is, we need to use a compound and render every child
 * with the same fill renderer
 * To do an intersection of clips, the clip-path element (not its children)
 * should have the clip-path property set
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_ELEMENT_CLIP_PATH_DESCRIPTOR egueb_svg_element_clip_path_descriptor_get()
#define EGUEB_SVG_ELEMENT_CLIP_PATH_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Element_Clip_Path_Class, EGUEB_SVG_ELEMENT_CLIP_PATH_DESCRIPTOR)
#define EGUEB_SVG_ELEMENT_CLIP_PATH(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Element_Clip_Path, EGUEB_SVG_ELEMENT_CLIP_PATH_DESCRIPTOR)

typedef struct _Egueb_Svg_Element_Clip_Path
{
	Egueb_Svg_Referenceable base;
	/* properties */
	Egueb_Dom_Node *units;
	/* private */
} Egueb_Svg_Element_Clip_Path;

typedef struct _Egueb_Svg_Element_Clip_Path_Class
{
	Egueb_Svg_Referenceable_Class base;
} Egueb_Svg_Element_Clip_Path_Class;

/* function called whenever we want to process the clip path children */
static Eina_Bool _egueb_svg_element_clip_path_children_process_cb(
		Egueb_Dom_Node *child, void *data)
{
	/* process every child that is not a clonable element */
	if (!egueb_svg_element_clip_path_node_is_clonable(child))
	{
		DBG("Processing not clonable child");
		if (!egueb_dom_element_process(child))
			WARN("Child failed to process");
	}
	return EINA_TRUE;
}

/* we should never process an element that is live cloned or any of its
 * children
 */
static void _egueb_svg_element_clip_path_request_process_cb(
		Egueb_Dom_Event *e, void *data)
{
	Egueb_Svg_Element_Clip_Path *thiz = data;
	Egueb_Dom_Node *parent = NULL;
	Egueb_Dom_Node *target = NULL;

	/* if the element that requests is ourselves, just return */
	target = egueb_dom_event_target_get(e);
	if (target == EGUEB_DOM_NODE(thiz))
		goto done;
	
	/* check if the target's parent is ourselves, if so, check that
	 * it is not clonable, otherwise prevent the process */
	parent = egueb_dom_node_parent_get(target);
	if (parent == EGUEB_DOM_NODE(thiz) &&
		!egueb_svg_element_clip_path_node_is_clonable(target))
	/* TODO dont propagate the request process */
	ERR("FIXME dont propagate the request process");
	egueb_dom_node_unref(parent);
done:
	egueb_dom_node_unref(target);
}

/* return TRUE whenever the mutation event is prevented because
 * the parent node is ourselves
 * FIXME remove this
 */
static Eina_Bool _egueb_svg_element_clip_path_hierarchy_prevented(
		Egueb_Svg_Element_Clip_Path *thiz, Egueb_Dom_Event *e)
{
	Eina_Bool ret = EINA_FALSE;
	Egueb_Dom_Node *parent = NULL;

	parent = egueb_dom_event_mutation_related_get(e);
	/* if the parent is not ourselves, dont propagate the change to the
	 * document
	 */
	if (EGUEB_DOM_NODE(thiz) != parent)
	{
		egueb_dom_event_stop_propagation(e);
		ret = EINA_TRUE;
	}
	egueb_dom_node_unref(parent);
	return ret;
}

/* whenever a child is added, clone it also add it on every reference
 * in case the bubbling event comes from a node that is not our children
 * not make the document queue the clip path as if it has changed
 * FIXME remove this, just use a simple verion to recreate again the clones
 */
static void _egueb_svg_element_clip_path_node_inserted_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Svg_Element_Clip_Path *thiz = data;
	Egueb_Svg_Referenceable *r;
	Egueb_Dom_Node *child = NULL;

	/* check if we already have any reference, if not, there's no need
	 * to create live nodes again
	 */
	if (_egueb_svg_element_clip_path_hierarchy_prevented(thiz, e))
		return;

	r = EGUEB_SVG_REFERENCEABLE(thiz);
	if (!r->references)
	{
		DBG("No references available, nothing to do");
		return;
	}

	/* in case the node is a clipable element, clone it */
	child = egueb_dom_event_target_get(e);
	if (egueb_svg_element_clip_path_node_is_clonable(child))
	{
		ERR("We still need to create live nodes");
	}
	egueb_dom_node_unref(child);
}

/* whenever a child is removed, remove the cloned element from every reference
 * in case the bubbling event comes from a node that is not our children
 * not make the document queue the clip path as if it has changed
 * FIXME remove this, just use a simple verion to recreate again the clones
 */
static void _egueb_svg_element_clip_path_node_removed_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Svg_Element_Clip_Path *thiz = data;
	Egueb_Svg_Referenceable *r;
	Egueb_Dom_Node *child = NULL;

	if (_egueb_svg_element_clip_path_hierarchy_prevented(thiz, e))
		return;

	/* check if we already have any reference, if not, there's no need
	 * to create live nodes again
	 */
	r = EGUEB_SVG_REFERENCEABLE(thiz);
	if (!r->references)
	{
		DBG("No references available, nothing to do");
		return;
	}

	/* in case the node is a clipable element, clone it */
	child = egueb_dom_event_target_get(e);
	if (egueb_svg_element_clip_path_node_is_clonable(child))
	{
		ERR("We still need to remove live nodes");
	}
	egueb_dom_node_unref(child);
}
/*----------------------------------------------------------------------------*
 *                           Referenceable interface                          *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_element_clip_path_process(
		Egueb_Svg_Referenceable *r)
{
	/* iterate over the children and call the setup there */
	egueb_dom_node_children_foreach(EGUEB_DOM_NODE(r),
			_egueb_svg_element_clip_path_children_process_cb,
			r);
	return EINA_TRUE;
}

static Egueb_Svg_Reference * _egueb_svg_element_clip_path_reference_new(
		Egueb_Svg_Referenceable *r, Egueb_Dom_Node *referencer)
{
	Egueb_Svg_Reference *ref;

	DBG("Creating a new reference");
	ref = egueb_svg_reference_clip_path_new();
	return ref;
}

/*----------------------------------------------------------------------------*
 *                              Element interface                             *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_String * _egueb_svg_element_clip_path_tag_name_get(
		Egueb_Dom_Element *e)
{
	return egueb_dom_string_ref(EGUEB_SVG_NAME_CLIP_PATH);
}
/*----------------------------------------------------------------------------*
 *                                Node interface                              *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_element_clip_path_child_appendable(Egueb_Dom_Node *n,
		Egueb_Dom_Node *child)
{
	/* TODO 
	 * descriptive elements — ‘desc’, ‘metadata’, ‘title’
	 * animation elements — ‘animate’, ‘animateColor’, ‘animateMotion’, ‘animateTransform’, ‘set’
	 * shape elements — ‘circle’, ‘ellipse’, ‘line’, ‘path’, ‘polygon’, ‘polyline’, ‘rect’
	 * ‘text’
	 * 'use’
	 */
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_REFERENCEABLE_DESCRIPTOR,
		Egueb_Svg_Element_Clip_Path, Egueb_Svg_Element_Clip_Path_Class,
		egueb_svg_element_clip_path);

static void _egueb_svg_element_clip_path_class_init(void *k)
{
	Egueb_Svg_Referenceable_Class *klass;
	Egueb_Dom_Element_Class *e_klass;
	Egueb_Dom_Node_Class *n_klass;

	klass = EGUEB_SVG_REFERENCEABLE_CLASS(k);
	klass->process = _egueb_svg_element_clip_path_process;
	klass->reference_new = _egueb_svg_element_clip_path_reference_new;

	e_klass= EGUEB_DOM_ELEMENT_CLASS(k);
	e_klass->tag_name_get = _egueb_svg_element_clip_path_tag_name_get;

	n_klass= EGUEB_DOM_NODE_CLASS(k);
	n_klass->child_appendable = _egueb_svg_element_clip_path_child_appendable;
}

static void _egueb_svg_element_clip_path_instance_init(void *o)
{
	Egueb_Svg_Element_Clip_Path *thiz;
	Egueb_Dom_Node *n;

	thiz = EGUEB_SVG_ELEMENT_CLIP_PATH(o);
	/* create the properties */
	thiz->units = egueb_svg_attr_referenceable_units_new(
			egueb_dom_string_ref(EGUEB_SVG_CLIP_PATH_UNITS),
			EGUEB_SVG_REFERENCEABLE_UNITS_USER_SPACE_ON_USE, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);

	n = EGUEB_DOM_NODE(o);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->units), NULL);
	/* add the required event handlers */
	egueb_dom_node_event_listener_add(EGUEB_DOM_NODE(o),
			EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED,
			_egueb_svg_element_clip_path_node_inserted_cb,
			EINA_FALSE, thiz);
	egueb_dom_node_event_listener_add(EGUEB_DOM_NODE(o),
			EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED,
			_egueb_svg_element_clip_path_node_removed_cb,
			EINA_FALSE, thiz);
	egueb_dom_node_event_listener_add(EGUEB_DOM_NODE(o),
			EGUEB_DOM_EVENT_PROCESS,
			_egueb_svg_element_clip_path_request_process_cb,
			EINA_FALSE, thiz);
}

static void _egueb_svg_element_clip_path_instance_deinit(void *o)
{
	Egueb_Svg_Element_Clip_Path *thiz;

	thiz = EGUEB_SVG_ELEMENT_CLIP_PATH(o);
	/* destroy the properties */
	egueb_dom_node_unref(thiz->units);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* return TRUE if the node is of a type different than a smil node */
Eina_Bool egueb_svg_element_clip_path_node_is_clonable(Egueb_Dom_Node *n)
{
	return !egueb_smil_is_animation(n);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_svg_element_clip_path_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_element_clip_path);
	return n;
}

EAPI Eina_Bool egueb_svg_element_is_clip_path(Egueb_Dom_Node *n)
{
	if (!n) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(n),
			EGUEB_SVG_ELEMENT_CLIP_PATH_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}

EAPI void egueb_svg_element_clip_path_units_get(Egueb_Dom_Node *n,
		Egueb_Svg_Referenceable_Units_Animated *units)
{
	Egueb_Svg_Element_Clip_Path *thiz;

	thiz = EGUEB_SVG_ELEMENT_CLIP_PATH(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->units, units);
}

#if 0
EAPI void egueb_svg_element_clip_path_units_set(Ender_Element *e, Egueb_Svg_Clip_Path_Units units)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_CLIP_PATH_CLIP_PATH_UNITS, units, NULL);
}

#endif
