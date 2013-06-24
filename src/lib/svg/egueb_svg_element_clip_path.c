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
#include "egueb_svg_painter.h"
#include "egueb_svg_reference.h"
#include "egueb_svg_referenceable.h"
#include "egueb_svg_referenceable_units.h"
#include "egueb_svg_string.h"
#include "egueb_svg_attr_referenceable_units.h"
#include "egueb_svg_attr_string.h"
#include "egueb_svg_element_clip_path.h"
#include "egueb_svg_element_g.h"
#include "egueb_svg_document.h"

#include "egueb_svg_referenceable_private.h"
#include "egueb_svg_reference_private.h"
#include "egueb_svg_painter_private.h"

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
	/* TODO we can have our own clip path reference */
} Egueb_Svg_Element_Clip_Path;

typedef struct _Egueb_Svg_Element_Clip_Path_Class
{
	Egueb_Svg_Referenceable_Class base;
} Egueb_Svg_Element_Clip_Path_Class;

/* return TRUE if the node is of a type different than a smil node */
static Eina_Bool _egueb_svg_element_clip_path_node_is_clonable(Egueb_Dom_Node *n)
{
	return !egueb_smil_is_animation(n);
}

/* function called whenever we want to clone the clip path children */
static Eina_Bool _egueb_svg_element_clip_path_children_clone_cb(
		Egueb_Dom_Node *child, void *data)
{
	Egueb_Svg_Reference *ref = data;
	Egueb_Dom_Node *clone = NULL;
	Egueb_Dom_Node *g = NULL;

	/* process every child that is not a clonable element */
	if (!_egueb_svg_element_clip_path_node_is_clonable(child))
		return EINA_TRUE;
	/* for a children that is clonable, clone it and add it to the g */
	egueb_dom_node_clone(child, EINA_TRUE, EINA_TRUE, &clone);
	/* set the painter on the cloned element */
	/* TODO put this code on the clip path reference */
	if (egueb_svg_is_shape(clone))
	{
		Egueb_Svg_Painter *painter;

		painter = egueb_svg_painter_clip_path_new(ref);
		egueb_svg_shape_painter_set(clone, painter);
	}
	egueb_svg_reference_clip_path_g_get(ref, &g);
	egueb_dom_node_child_append(g, clone);
	egueb_dom_node_unref(g);

	return EINA_TRUE;
}

/* function called whenever we want to process the clip path children */
static Eina_Bool _egueb_svg_element_clip_path_children_process_cb(
		Egueb_Dom_Node *child, void *data)
{
	/* process every child that is not a clonable element */
	if (!_egueb_svg_element_clip_path_node_is_clonable(child))
	{
		DBG("Processing not clonable child");
		if (!egueb_dom_element_process(child))
			WARN("Child failed to process");
	}
	return EINA_TRUE;
}

/* return TRUE whenever the mutation event is prevented because
 * the parent node is ourselves
 */
static Eina_Bool _egueb_svg_element_clip_path_hierarchy_prevented(
		Egueb_Svg_Element_Clip_Path *thiz, Egueb_Dom_Event *e)
{
	Eina_Bool ret = EINA_FALSE;
	Egueb_Dom_Node *parent = NULL;

	egueb_dom_event_mutation_related_get(e, &parent);
	/* if the parent is not ourselves, dont propagate the change to the
	 * document
	 */
	if (EGUEB_DOM_NODE(thiz) != parent)
	{
		egueb_dom_event_mutation_process_prevent(e);
		ret = EINA_TRUE;
	}
	egueb_dom_node_unref(parent);
	return ret;
}

/* whenever a child is added, clone it also add it on every reference
 * in case the bubbling event comes from a node that is not our children
 * not make the document queue the clip path as if it has changed
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
	egueb_dom_event_target_get(e, &child);
	if (_egueb_svg_element_clip_path_node_is_clonable(child))
	{
		ERR("We still need to create live nodes");
	}
	egueb_dom_node_unref(child);
}

/* whenever a child is removed, remove the cloned element from every reference
 * in case the bubbling event comes from a node that is not our children
 * not make the document queue the clip path as if it has changed
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
	egueb_dom_event_target_get(e, &child);
	if (_egueb_svg_element_clip_path_node_is_clonable(child))
	{
		ERR("We still need to remove live nodes");
	}
	egueb_dom_node_unref(child);
}

/* whenever a child changed, dont propagate the change to the document
 * unless the parent is ourselfes and it is not of a clipable type
 */
static void _egueb_svg_element_clip_path_attr_modified_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Svg_Element_Clip_Path *thiz = data;
	Egueb_Dom_Node *parent = NULL;
	Egueb_Dom_Node *target = NULL;

	/* check that the parent is oursleves */
	egueb_dom_event_target_get(e, &target);
	egueb_dom_node_parent_get(target, &parent);
	if (EGUEB_DOM_NODE(thiz) != parent)
	{
		egueb_dom_node_unref(parent);
		egueb_dom_node_unref(target);
		return;
	}
	egueb_dom_node_unref(parent);

	/* in case the attr modified is from a node that we should clone
	 * prevent the queue on the document
	 */
	if (_egueb_svg_element_clip_path_node_is_clonable(target))
	{
		egueb_dom_event_mutation_process_prevent(e);
	}
	egueb_dom_node_unref(target);
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
	Egueb_Dom_Node *g;

	DBG("Creating a new reference");
	/* TODO we need to check what kind of referencer is */
	/* create a new group */
	g = egueb_svg_element_g_new();
	/* FIXME add a better way to do this */
	{
		Egueb_Dom_Node *doc;

		egueb_dom_node_document_get(EGUEB_DOM_NODE(r), &doc);
		egueb_dom_document_node_adopt(doc, g, &g);
		egueb_dom_node_unref(doc);
	}
	/* make the group get the presentation attributes from ourelves and
	 * the geometry relative from the referrer
	 */
	egueb_svg_element_presentation_relative_set(g, EGUEB_DOM_NODE(r));
	egueb_svg_element_geometry_relative_set(g, referencer);
	/* create the reference clip path, set the g there */
	ref = egueb_svg_reference_clip_path_new();
	egueb_svg_reference_clip_path_g_set(ref, g);

	/* iterate over the list of children and clone the clonable elements */
	egueb_dom_node_children_foreach(EGUEB_DOM_NODE(r),
			_egueb_svg_element_clip_path_children_clone_cb,
			ref);

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
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_clip_path, Egueb_Svg_Element_Clip_Path, units);

ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_REFERENCEABLE_DESCRIPTOR,
		Egueb_Svg_Element_Clip_Path, Egueb_Svg_Element_Clip_Path_Class,
		egueb_svg_element_clip_path);

static void _egueb_svg_element_clip_path_class_init(void *k)
{
	Egueb_Svg_Referenceable_Class *klass;
	Egueb_Dom_Element_Class *e_klass;

	klass = EGUEB_SVG_REFERENCEABLE_CLASS(k);
	klass->process = _egueb_svg_element_clip_path_process;
	klass->reference_new = _egueb_svg_element_clip_path_reference_new;

	e_klass= EGUEB_DOM_ELEMENT_CLASS(k);
	e_klass->tag_name_get = _egueb_svg_element_clip_path_tag_name_get;
}

static void _egueb_svg_element_clip_path_class_deinit(void *k)
{
}

static void _egueb_svg_element_clip_path_instance_init(void *o)
{
	Egueb_Svg_Element_Clip_Path *thiz;

	thiz = EGUEB_SVG_ELEMENT_CLIP_PATH(o);
	/* create the properties */
	thiz->units = egueb_svg_attr_referenceable_units_new(
			egueb_dom_string_ref(EGUEB_SVG_CLIP_PATH_UNITS),
			EGUEB_SVG_REFERENCEABLE_UNITS_USER_SPACE_ON_USE, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_clip_path, units);
	/* add the required event handlers */
	egueb_dom_node_event_listener_add(EGUEB_DOM_NODE(o),
			EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED,
			_egueb_svg_element_clip_path_node_inserted_cb,
			EINA_TRUE, thiz);
	egueb_dom_node_event_listener_add(EGUEB_DOM_NODE(o),
			EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED,
			_egueb_svg_element_clip_path_node_removed_cb,
			EINA_TRUE, thiz);
	egueb_dom_node_event_listener_add(EGUEB_DOM_NODE(o),
			EGUEB_DOM_EVENT_MUTATION_ATTR_MODIFIED,
			_egueb_svg_element_clip_path_attr_modified_cb,
			EINA_TRUE, thiz);
}

static void _egueb_svg_element_clip_path_instance_deinit(void *o)
{
	Egueb_Svg_Element_Clip_Path *thiz;

	thiz = EGUEB_SVG_ELEMENT_CLIP_PATH(o);
	/* destroy the properties */
	egueb_dom_node_unref(thiz->units);
}

#if 0
static Eina_Bool _egueb_svg_element_clip_path_clone(Egueb_Dom_Tag *t, Egueb_Dom_Tag *child,
		void *user_data)
{
	Egueb_Svg_Clip_Path_Referenceable_Data *data = user_data;
	Ender_Element *cloned;
	Egueb_Dom_Tag *cloned_t;
	Ender_Element *child_e;

	/* TODO skip the text */
	/* TODO check that the renderable implementation renderer is a
	 * shape, if so, set the fill renderer of the cloned renderer
	 * as the referer renderer
	 */
	child_e = egueb_svg_element_ender_get(child);
	cloned = egueb_svg_clone_new(child_e);
	cloned_t = ender_element_object_get(cloned);
	ender_element_property_value_add(data->g, EGUEB_DOM_CHILD, cloned_t, NULL);

	return EINA_TRUE;
}

/*----------------------------------------------------------------------------*
 *                       The Esvg Renderable interface                        *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_element_clip_path_child_add(Egueb_Dom_Tag *tag, Egueb_Dom_Tag *child)
{
	Egueb_Svg_Type type;

	if (!egueb_svg_is_element_internal(child))
		return EINA_FALSE;

	type = egueb_svg_element_internal_type_get(child);
	if (egueb_svg_type_is_renderable(type))
	{
		Egueb_Svg_Element_Clip_Path *thiz;

		thiz = _egueb_svg_element_clip_path_get(tag);
		thiz->tree_changed = EINA_TRUE;
		return EINA_TRUE;
	}
	return EINA_FALSE;
}

#if 0
static Eina_Bool _egueb_svg_element_clip_path_set_enesim_state_handle(Egueb_Dom_Tag *e,
		Enesim_Renderer *layer, void *data)
{
	Enesim_Renderer *shape = layer;
	Eina_Bool unref = EINA_FALSE;

	/* TODO in case of <use> check that the link is a shape */
	/* TODO handle the <use> tag. Given that it is not a shape ... */
	if (egueb_svg_is_use(shape))
	{
		egueb_svg_element_use_link_get(shape, &shape);
		if (!shape) return EINA_FALSE;
		if (!egueb_svg_is_shape(shape))
		{
			printf("use found but not a shape on the link\n");
			return EINA_FALSE;
		}
	}

	egueb_svg_shape_enesim_state_calculate_set(shape, _egueb_svg_element_clip_path_enesim_state_calculate, data);
	/* TODO for the sake of it, this should be done on the enesim state handler */
	enesim_renderer_rop_set(shape, ENESIM_FILL);
	/* TODO unref the shape in case comes from the use */
	if (unref)
		enesim_renderer_unref(shape);
	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_element_clip_path_unset_enesim_state_handle(Egueb_Dom_Tag *e,
		Enesim_Renderer *layer, void *data)
{
	Enesim_Renderer *shape = layer;
	Eina_Bool unref = EINA_FALSE;

	/* TODO in case of <use> check that the link is a shape */
	/* TODO handle the <use> tag. Given that it is not a shape ... */
	if (egueb_svg_is_use(shape))
	{
		egueb_svg_element_use_link_get(shape, &shape);
		if (!shape) return EINA_FALSE;
		if (!egueb_svg_is_shape(shape))
		{
			printf("use found but not a shape on the link\n");
			return EINA_FALSE;
		}
	}

	egueb_svg_shape_enesim_state_calculate_set(shape, NULL);
	/* TODO unref the shape in case comes from the use */
	if (unref)
		enesim_renderer_unref(shape);
	return EINA_TRUE;
}
#endif

static Egueb_Svg_Element_Setup_Return _egueb_svg_element_clip_path_setup(Egueb_Dom_Tag *e,
		Egueb_Svg_Context *c,
		Egueb_Svg_Element_Context *state,
		Egueb_Svg_Attribute_Presentation *attr,
		Enesim_Log **error)
{
	Egueb_Svg_Element_Clip_Path *thiz;

	thiz = _egueb_svg_element_clip_path_get(e);
	DBG("Clip path '%s' setup", egueb_svg_element_internal_id_get(e));
#if 0
	Egueb_Svg_Element_Clip_Path *thiz;

	thiz = _egueb_svg_element_clip_path_get(t);
	/* if the clip path is set use that as the renderer element */
	if (!attr->clip_path_set)
		return thiz->r;
	else
		return egueb_svg_element_renderer_get(attr->clip_path);
	/* TODO add a new transformation in case of the units */
	/* TODO we should use the same transformation as the relative renderer */
	/* in case this clip path has a clip-path set, call the setup on other */
	if (attr->clip_path_set)
	{
		Eina_Bool ret;

		egueb_svg_element_clip_path_relative_set(attr->clip_path, thiz->r,
				&state->transform);
		ret = enesim_renderer_setup(attr->clip_path, s, error);
		printf("clip path setup %d\n", ret);
	}
#endif
	return ESVG_SETUP_OK;
}

static Eina_Bool _egueb_svg_element_clip_path_reference_add(Egueb_Dom_Tag *t, Egueb_Svg_Referenceable_Reference *rr)
{
	Egueb_Svg_Clip_Path_Referenceable_Data *data;
	Ender_Element *g;
	Egueb_Dom_Tag *g_t;
	Egueb_Dom_Tag *referer_t;
	Enesim_Renderer *r;
	Enesim_Renderer *proxy;

	g = egueb_svg_element_g_new();
	g_t = ender_element_object_get(g);
	egueb_svg_renderable_internal_renderer_get(g_t, &proxy);
	/* TODO handle the tree changed */
	/* TODO two different objects can reference us, or either a renderable (shapes)
	 * or a referenceable (another clip path)
	 */
	/* now get the renderer from the element that references us */
	referer_t = rr->referencer;
	egueb_svg_renderable_implementation_renderer_get(referer_t, &r);

	data = calloc(1, sizeof(Egueb_Svg_Clip_Path_Referenceable_Data));
	data->referrer = r;
	data->g = g;
	data->proxy = proxy;

	/* clone each child and reparent it */
	egueb_dom_tag_child_foreach(t, _egueb_svg_element_clip_path_clone, data);
	rr->data = data;

	/* if it is a renderable then store the renderer it has */
	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_element_clip_path_reference_propagate(Egueb_Dom_Tag *t,
		Egueb_Svg_Context *c,
		const Egueb_Svg_Element_Context *ctx,
		const Egueb_Svg_Attribute_Presentation *attr,
		void *user_data,
		Enesim_Log **error)
{
	Egueb_Svg_Element_Context clone_ctx;
	Egueb_Svg_Clip_Path_Referenceable_Data *data = user_data;
	Egueb_Dom_Tag *g_t;

	/* if the tree has changed then re-create the clone */

	/* copy the context but use our own behaviour data */
	/* FIXME not the that ctx here is from the referrer, that is,
	 * the one with the clip-path set, we need to handle
	 * that by removing the clip path check from the common renderable
	 * setup
	 */
	clone_ctx = *ctx;
	clone_ctx.renderable_behaviour.context_set = _egueb_svg_element_clip_path_context_set;
	clone_ctx.renderable_behaviour.data = data;

	/* set the correct proxied renderer */
	/* FIXME for now we dont support clip path referencing another
	 * on that case our own proxy should be the referenced one
	 */
	/* do the setup on the new tree but using this new context */
	g_t = ender_element_object_get(data->g);
	egueb_svg_element_setup_rel(g_t, c, &clone_ctx, NULL, error);

	return ESVG_SETUP_OK;
}

static Egueb_Svg_Referenceable_Descriptor _descriptor = {
	/* .child_add		= */ NULL,
	/* .child_remove	= */ NULL,
	/* .attribute_get	= */ _egueb_svg_element_clip_path_attribute_get,
	/* .cdata_set		= */ NULL,
	/* .text_set		= */ NULL,
	/* .text_get		= */ NULL,
	/* .free		= */ _egueb_svg_element_clip_path_free,
	/* .initialize		= */ NULL,
	/* .attribute_set	= */ _egueb_svg_element_clip_path_attribute_set,
	/* .attribute_animated_fetch = */ NULL,
	/* .setup		= */ _egueb_svg_element_clip_path_setup,
	/* .cleanup		= */ NULL,
	/* .reference_propagate	= */ _egueb_svg_element_clip_path_reference_propagate,
	/* .reference_add	= */ _egueb_svg_element_clip_path_reference_add,
	/* .reference_remove	= */ NULL,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
#endif
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

EAPI Eina_Error egueb_svg_element_clip_path_units_get(Egueb_Dom_Node *n,
		Egueb_Svg_Referenceable_Units_Animated *units)
{
	Egueb_Svg_Element_Clip_Path *thiz;

	thiz = EGUEB_SVG_ELEMENT_CLIP_PATH(n);
	if (egueb_dom_attr_type_is_set(thiz->units, EGUEB_DOM_ATTR_TYPE_BASE))
		egueb_dom_attr_get(thiz->units, EGUEB_DOM_ATTR_TYPE_BASE, &units->base);
	else
		egueb_dom_attr_get(thiz->units, EGUEB_DOM_ATTR_TYPE_DEFAULT, &units->base);
	if (egueb_dom_attr_type_is_set(thiz->units, EGUEB_DOM_ATTR_TYPE_ANIMATED))
		egueb_dom_attr_get(thiz->units, EGUEB_DOM_ATTR_TYPE_ANIMATED, &units->anim);
	else
		units->anim = units->base;
	return EINA_ERROR_NONE;
}

#if 0
EAPI void egueb_svg_element_clip_path_units_set(Ender_Element *e, Egueb_Svg_Clip_Path_Units units)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_CLIP_PATH_CLIP_PATH_UNITS, units, NULL);
}

#endif
