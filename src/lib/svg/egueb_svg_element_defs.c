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
#include "egueb_svg_element_private.h"
#include "egueb_svg_element_defs.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_ELEMENT_DEFS_DESCRIPTOR egueb_svg_element_defs_descriptor_get()
#define EGUEB_SVG_ELEMENT_DEFS_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Element_Defs_Class, EGUEB_SVG_ELEMENT_DEFS_DESCRIPTOR)
#define EGUEB_SVG_ELEMENT_DEFS(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Element_Defs, EGUEB_SVG_ELEMENT_DEFS_DESCRIPTOR)

typedef struct _Egueb_Svg_Element_Defs
{
	Egueb_Svg_Element base;
} Egueb_Svg_Element_Defs;

typedef struct _Egueb_Svg_Element_Defs_Class
{
	Egueb_Svg_Element_Class base;
} Egueb_Svg_Element_Defs_Class;

static void _egueb_svg_element_defs_attr_modified_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Dom_Node *target = NULL;

	/* a mutation is being triggered from one of our children
	 * or a children of our children, dont propagate it
	 */
	egueb_dom_event_target_get(e, &target);
	if (egueb_svg_is_renderable(target))
	{
		DBG("Preventing a process on a renderable element");
		egueb_dom_event_mutation_process_prevent(e);
	}
	egueb_dom_node_unref(target);
}

static Eina_Bool _egueb_svg_element_defs_children_process_cb(
		Egueb_Dom_Node *child, void *data)
{
	Egueb_Dom_Node_Type type;

	type = egueb_dom_node_type_get(child);
	if (type != EGUEB_DOM_NODE_TYPE_ELEMENT_NODE)
		return EINA_TRUE;
	if (egueb_svg_is_renderable(child))
		return EINA_TRUE;
	egueb_dom_element_process(child);
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                             Element interface                              *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_element_defs_process(Egueb_Svg_Element *e)
{
	/* iterate over the children and call the process there */
	egueb_dom_node_children_foreach(EGUEB_DOM_NODE(e),
			_egueb_svg_element_defs_children_process_cb,
			NULL);
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                             Element interface                              *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_String * _egueb_svg_element_defs_tag_name_get(
		Egueb_Dom_Element *e)
{
	return egueb_dom_string_ref(EGUEB_SVG_NAME_DEFS);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_ELEMENT_DESCRIPTOR,
		Egueb_Svg_Element_Defs, Egueb_Svg_Element_Defs_Class,
		egueb_svg_element_defs);

static void _egueb_svg_element_defs_class_init(void *k)
{
	Egueb_Svg_Element_Class *klass;
	Egueb_Dom_Element_Class *e_klass;

	klass= EGUEB_SVG_ELEMENT_CLASS(k);
	klass->process = _egueb_svg_element_defs_process;

	e_klass= EGUEB_DOM_ELEMENT_CLASS(k);
	e_klass->tag_name_get = _egueb_svg_element_defs_tag_name_get;
}

static void _egueb_svg_element_defs_instance_init(void *o)
{
	Egueb_Dom_Node *n;

	n = EGUEB_DOM_NODE(o);
	/* whenever an instance is created we need to add a listener
	 * on every mutation bubbled event so we can mark the event as
	 * 'not needed for a process' so the document wont process it
	 */
	egueb_dom_node_event_listener_add(n,
			EGUEB_DOM_EVENT_MUTATION_ATTR_MODIFIED,
			_egueb_svg_element_defs_attr_modified_cb,
			EINA_FALSE, NULL);
}

static void _egueb_svg_element_defs_instance_deinit(void *o)
{
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_svg_element_defs_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_element_defs);
	return n;
}
