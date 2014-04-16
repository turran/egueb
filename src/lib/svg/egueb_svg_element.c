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
#include "egueb_svg_element.h"
#include "egueb_svg_reference.h"
#include "egueb_svg_referenceable.h"
#include "egueb_svg_referenceable_units.h"
#include "egueb_svg_element_clip_path.h"
#include "egueb_svg_document.h"

#include "egueb_svg_element_private.h"
#include "egueb_dom_string_private.h"
/*
 * TODO
 * use pointers to determine the state/final/whatever states, that
 * should be swapped on setup/cleanup
 * We need to keep track whenever an attribute is being animated or not,
 * to know what value (the base or the anim) we shold choose for merging
 * one possible way could be:
 *  Add callbacks whenever some element has been added/removed
 *  Add handlers whenever an animation/set has changed the attribute name
 *  if the attribute name is one of our own (color, stroke color, etc),
 *  then we should increment the animated value, so on the state merge
 *  take into account the animated or the base value.
 *  Add getters in their 'full' variant (base and anim value)
 *  When the above is done, then we can modify the animators to change
 *  the element "animation" state  (egueb_svg_element_attribute_animate_set)
 *  so the anim value is the one that gets updated
 *
 * We need on the API side several functions:
 * 1. The binding function which always gets the animated type (not styling or
 * simple).
 * 2. The C fast version which should allow a set/get of a basic type
 * Setting a value will set it on based on the is_animating, is_styling bools
 * Getting a value should also depend on that? Or have two versions?
 * One that gets the final var and another that gets the is_animating is styling
 * var? or another that gets the value as in the 1 case?
 *
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                          Css context interface                             *
 *----------------------------------------------------------------------------*/
static const char * _egueb_svg_element_css_property_get(void *e, const char *property)
{
#if 0
	Egueb_Dom_Node *n = e;
	Egueb_Dom_String *prop;
	Egueb_Dom_String *val;

	prop = egueb_dom_string_new_with_string(property);
	egueb_dom_element_attribute_get(n, prop, &val);
	return egueb_dom_string_string_get(val);
#endif
	return NULL;
}

/* FIXME we could call directly the function _attribute_set */
static void _egueb_svg_element_css_property_set(void *e, const char *property, const char *value)
{
	Egueb_Dom_Node *n = e;
	Egueb_Dom_String *prop;
	Egueb_Dom_String *val;

	prop = egueb_dom_string_new_with_string(property);
	val = egueb_dom_string_new_with_string(value);
	egueb_dom_element_attribute_type_set(n, prop, EGUEB_DOM_ATTR_TYPE_STYLED, val, NULL);
	egueb_dom_string_unref(val);
	egueb_dom_string_unref(prop);
}

static void * _egueb_svg_element_css_get_child(void *e)
{
#if 0
	Egueb_Dom_Node *n = e;
	
	return egueb_dom_tag_child_get(tag);
#endif
	return NULL;
}

static void * _egueb_svg_element_css_get_next_sibling(void *e)
{
#if 0
	Egueb_Dom_Node *n = e;
	Egueb_Dom_Node *next = NULL;

	egueb_dom_node_sibling_next_get(n, &next);
	return next;
#endif
	return NULL;
}

static const char * _egueb_svg_element_css_get_name(void *e)
{
#if 0
	Egueb_Dom_Node *n = e;
	Egueb_Dom_String *name;
	const char *ret;

	egueb_dom_node_name_get(n, &name);
	ret = egueb_dom_string_string_get(name);
	egueb_dom_string_unref(name);
	return ret;
#endif
	return NULL;
}

static Egueb_Css_Context _egueb_svg_element_css_context = {
	/* .property_set 	= */ _egueb_svg_element_css_property_set,
	/* .property_get 	= */ _egueb_svg_element_css_property_get,
	/* .get_name 		= */ _egueb_svg_element_css_get_name,
	/* .get_child 		= */ _egueb_svg_element_css_get_child,
	/* .get_next_sibling 	= */ _egueb_svg_element_css_get_next_sibling,
};
/*----------------------------------------------------------------------------*
 *                              Event handlers                                *
 *----------------------------------------------------------------------------*/
#if 0
static void _egueb_svg_element_event_handler(Egueb_Svg_Element *thiz, const char *script,
		Ender_Value *v)
{
	Egueb_Svg_Script_Provider *provider;
	const char *content_script_type;

	if (!script) return;
	if (!thiz->topmost) return;

	egueb_svg_element_svg_content_script_type_get(thiz->topmost, &content_script_type);
	provider = egueb_svg_element_svg_script_provider_get(thiz->topmost, content_script_type);
	if (!provider) return;
	/* call the script with the passed in arg */
	printf("calling with %s\n", script);
	egueb_svg_script_provider_run(provider, script, v);
}

static void _egueb_svg_element_onfocusin_call(Ender_Element *e,
		const char *event_name, void *event_data, void *data)
{
	Egueb_Svg_Element *thiz = data;
	_egueb_svg_element_event_handler(thiz, thiz->onfocusin, NULL);
}

static void _egueb_svg_element_onfocusout_call(Ender_Element *e,
		const char *event_name, void *event_data, void *data)
{
	Egueb_Svg_Element *thiz = data;
	_egueb_svg_element_event_handler(thiz, thiz->onfocusout, NULL);
}

static void _egueb_svg_element_onactivate_call(Ender_Element *e,
		const char *event_name, void *event_data, void *data)
{
	Egueb_Svg_Element *thiz = data;
	_egueb_svg_element_event_handler(thiz, thiz->onactivate, NULL);
}

static void _egueb_svg_element_mouse_event_call(Egueb_Svg_Element *thiz,
		void *event_data, const char *script)
{
	Ender_Value *v;
	Ender_Descriptor *d;
	Ender_Container *c;

	if (!script) return;

	d = ender_descriptor_find("MouseEvent");
	if (!d)
	{
		ERR("'MouseEvent' not defined");
		return;
	}

	c = ender_container_struct_new(d);
	v = ender_value_new_container_from(c);
	ender_value_struct_set(v, event_data);

	_egueb_svg_element_event_handler(thiz, script, v);
	ender_value_unref(v);
}

static void _egueb_svg_element_onclick_call(Ender_Element *e,
		const char *event_name, void *event_data, void *data)
{
	Egueb_Svg_Element *thiz = data;
	_egueb_svg_element_mouse_event_call(thiz, event_data, thiz->onclick);
}

static void _egueb_svg_element_onmousedown_call(Ender_Element *e,
		const char *event_name, void *event_data, void *data)
{
	Egueb_Svg_Element *thiz = data;
	_egueb_svg_element_mouse_event_call(thiz, event_data, thiz->onmousedown);
}

static void _egueb_svg_element_onmouseup_call(Ender_Element *e,
		const char *event_name, void *event_data, void *data)
{
	Egueb_Svg_Element *thiz = data;
	_egueb_svg_element_mouse_event_call(thiz, event_data, thiz->onmouseup);
}

static void _egueb_svg_element_onmouseover_call(Ender_Element *e,
		const char *event_name, void *event_data, void *data)
{
	Egueb_Svg_Element *thiz = data;
	_egueb_svg_element_mouse_event_call(thiz, event_data, thiz->onmouseover);
}

static void _egueb_svg_element_onmousemove_call(Ender_Element *e,
		const char *event_name, void *event_data, void *data)
{
	Egueb_Svg_Element *thiz = data;
	_egueb_svg_element_mouse_event_call(thiz, event_data, thiz->onmousemove);
}

static void _egueb_svg_element_onmouseout_call(Ender_Element *e,
		const char *event_name, void *event_data, void *data)
{
	Egueb_Svg_Element *thiz = data;
	_egueb_svg_element_mouse_event_call(thiz, event_data, thiz->onmouseout);
}

/* called on every child of an element, whenever the element is being freed */
static Eina_Bool _egueb_svg_element_child_free_cb(Egueb_Dom_Tag *t, Egueb_Dom_Tag *child,
		void *data)
{
	Ender_Element *t_e;
	Ender_Element *child_e;

	t_e = egueb_svg_element_ender_get(t);
	child_e = egueb_svg_element_ender_get(child);
	ender_element_property_value_remove(t_e, EGUEB_DOM_CHILD, child, NULL);
	ender_element_unref(child_e);

	return EINA_TRUE;
}

static void _egueb_svg_element_mutation_cb(Ender_Element *e, const char *event_name,
		void *event_data, void *data)
{
	Egueb_Svg_Element *thiz = data;
	Ender_Event_Mutation *ev = event_data;

	/* FIXME we could check if the mutation is a remove, add, etc */
	thiz->changed++;
}

static int * _egueb_svg_element_attribute_animated_get(Egueb_Dom_Tag *t, const char *attr)
{
	Egueb_Svg_Element *thiz;
	int *animated;

	thiz = EGUEB_SVG_ELEMENT(t);
	/* get our own attributes */
	animated = _egueb_svg_element_attribute_animated_fetch(thiz, attr);
	if (!animated)
	{
		/* call the descriptor implementation */
		if (thiz->descriptor.attribute_animated_fetch)
			animated = thiz->descriptor.attribute_animated_fetch(t, attr);
	}
	return animated;
}

static void _egueb_svg_element_onfocusin_set(Egueb_Dom_Tag *t, const char *v)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(t);
	if (thiz->onfocusin)
	{
		free(thiz->onfocusin);
		thiz->onfocusin = NULL;
	}

	if (v)
	{
		thiz->onfocusin = strdup(v);
	}
}

static void _egueb_svg_element_onfocusin_get(Egueb_Dom_Tag *t, const char **v)
{
	Egueb_Svg_Element *thiz;

	if (!v) return;
	thiz = EGUEB_SVG_ELEMENT(t);
	*v = thiz->onfocusin;
}

static void _egueb_svg_element_onfocusout_set(Egueb_Dom_Tag *t, const char *v)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(t);
	if (thiz->onfocusout)
	{
		free(thiz->onfocusout);
		thiz->onfocusout = NULL;
	}

	if (v)
	{
		thiz->onfocusout = strdup(v);
	}
}

static void _egueb_svg_element_onfocusout_get(Egueb_Dom_Tag *t, const char **v)
{
	Egueb_Svg_Element *thiz;

	if (!v) return;
	thiz = EGUEB_SVG_ELEMENT(t);
	*v = thiz->onfocusout;
}

static void _egueb_svg_element_onactivate_set(Egueb_Dom_Tag *t, const char *v)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(t);
	if (thiz->onactivate)
	{
		free(thiz->onactivate);
		thiz->onactivate = NULL;
	}

	if (v)
	{
		thiz->onactivate = strdup(v);
	}
}

static void _egueb_svg_element_onactivate_get(Egueb_Dom_Tag *t, const char **v)
{
	Egueb_Svg_Element *thiz;

	if (!v) return;
	thiz = EGUEB_SVG_ELEMENT(t);
	*v = thiz->onactivate;
}

static void _egueb_svg_element_onclick_set(Egueb_Dom_Tag *t, const char *v)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(t);
	if (thiz->onclick)
	{
		free(thiz->onclick);
		thiz->onclick = NULL;
	}

	if (v)
	{
		thiz->onclick = strdup(v);
	}
}

static void _egueb_svg_element_onclick_get(Egueb_Dom_Tag *t, const char **v)
{
	Egueb_Svg_Element *thiz;

	if (!v) return;
	thiz = EGUEB_SVG_ELEMENT(t);
	*v = thiz->onclick;
}

static void _egueb_svg_element_onmousedown_set(Egueb_Dom_Tag *t, const char *v)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(t);
	if (thiz->onmousedown)
	{
		free(thiz->onmousedown);
		thiz->onmousedown = NULL;
	}

	if (v)
	{
		thiz->onmousedown = strdup(v);
	}
}

static void _egueb_svg_element_onmousedown_get(Egueb_Dom_Tag *t, const char **v)
{
	Egueb_Svg_Element *thiz;

	if (!v) return;
	thiz = EGUEB_SVG_ELEMENT(t);
	*v = thiz->onmousedown;
}

static void _egueb_svg_element_onmouseup_set(Egueb_Dom_Tag *t, const char *v)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(t);
	if (thiz->onmouseup)
	{
		free(thiz->onmouseup);
		thiz->onmouseup = NULL;
	}

	if (v)
	{
		thiz->onmouseup = strdup(v);
	}
}

static void _egueb_svg_element_onmouseup_get(Egueb_Dom_Tag *t, const char **v)
{
	Egueb_Svg_Element *thiz;

	if (!v) return;
	thiz = EGUEB_SVG_ELEMENT(t);
	*v = thiz->onmouseup;
}

static void _egueb_svg_element_onmouseover_set(Egueb_Dom_Tag *t, const char *v)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(t);
	if (thiz->onmouseover)
	{
		free(thiz->onmouseover);
		thiz->onmouseover = NULL;
	}

	if (v)
	{
		thiz->onmouseover = strdup(v);
	}
}

static void _egueb_svg_element_onmouseover_get(Egueb_Dom_Tag *t, const char **v)
{
	Egueb_Svg_Element *thiz;

	if (!v) return;
	thiz = EGUEB_SVG_ELEMENT(t);
	*v = thiz->onmouseover;
}

static void _egueb_svg_element_onmousemove_set(Egueb_Dom_Tag *t, const char *v)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(t);
	if (thiz->onmousemove)
	{
		free(thiz->onmousemove);
		thiz->onmousemove = NULL;
	}

	if (v)
	{
		thiz->onmousemove = strdup(v);
	}
}

static void _egueb_svg_element_onmousemove_get(Egueb_Dom_Tag *t, const char **v)
{
	Egueb_Svg_Element *thiz;

	if (!v) return;
	thiz = EGUEB_SVG_ELEMENT(t);
	*v = thiz->onmousemove;
}

static void _egueb_svg_element_onmouseout_set(Egueb_Dom_Tag *t, const char *v)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(t);
	if (thiz->onmouseout)
	{
		free(thiz->onmouseout);
		thiz->onmouseout = NULL;
	}

	if (v)
	{
		thiz->onmouseout = strdup(v);
	}
}

static void _egueb_svg_element_onmouseout_get(Egueb_Dom_Tag *t, const char **v)
{
	Egueb_Svg_Element *thiz;

	if (!v) return;
	thiz = EGUEB_SVG_ELEMENT(t);
	*v = thiz->onmouseout;
}
#endif

static void _egueb_svg_element_geometry_process(
		Egueb_Svg_Element *thiz, Egueb_Dom_Node *geometry)
{
	Egueb_Svg_Element *geom;
	if (!egueb_svg_is_element(geometry))
	{
		WARN("Relative geometry node '%p' does not inherit from the "
				"SVGElement interface", geometry);
		return;
	}
	geom = EGUEB_SVG_ELEMENT(geometry);
	thiz->viewbox = geom->viewbox;
}

static void _egueb_svg_element_presentation_attributes_process(
		Egueb_Svg_Element *thiz, Egueb_Dom_Node *relative,
		Egueb_Dom_Node *geometry)
{
	Egueb_Svg_Element *rel, *geom;
	Egueb_Dom_Node *inherited;

	if (!relative) return;
	if (!geometry) return;
	if (!egueb_svg_is_element(relative))
	{
		WARN("Relative presentation node '%p' does not inherit from the "
				"SVGElement interface", relative);
		return;
	}
	if (!egueb_svg_is_element(geometry))
	{
		WARN("Relative geometry node '%p' does not inherit from the "
				"SVGElement interface", geometry);
		return;
	}
	rel = EGUEB_SVG_ELEMENT(relative);
	geom = EGUEB_SVG_ELEMENT(geometry);
	/* TODO in case we have a parent and neither the parent
	 * or ourselves have changed an inheritable property
	 * just continue without generating again the final
	 * of state of such properties
	 */
	egueb_dom_attr_inheritable_process(thiz->color, rel->color);
	egueb_dom_attr_inheritable_process(thiz->fill, rel->fill);
	egueb_dom_attr_inheritable_process(thiz->fill_opacity, rel->fill_opacity);
	egueb_dom_attr_inheritable_process(thiz->font_family, rel->font_family);
	egueb_dom_attr_inheritable_process(thiz->opacity, rel->opacity);
	egueb_dom_attr_inheritable_process(thiz->overflow, rel->overflow);
	egueb_dom_attr_inheritable_process(thiz->stop_opacity, rel->stop_opacity);
	egueb_dom_attr_inheritable_process(thiz->stroke, rel->stroke);
	egueb_dom_attr_inheritable_process(thiz->stroke_dasharray, rel->stroke_dasharray);
	egueb_dom_attr_inheritable_process(thiz->stroke_line_cap, rel->stroke_line_cap);
	egueb_dom_attr_inheritable_process(thiz->stroke_line_join, rel->stroke_line_join);
	egueb_dom_attr_inheritable_process(thiz->stroke_width, rel->stroke_width);
	egueb_dom_attr_inheritable_process(thiz->stroke_opacity, rel->stroke_opacity);
	egueb_dom_attr_inheritable_process(thiz->text_anchor, rel->text_anchor);
	egueb_dom_attr_inheritable_process(thiz->visibility, rel->visibility);
	/* the font size is accumulative based on the parent's property */
	egueb_dom_attr_inheritable_process(thiz->font_size, rel->font_size);
	egueb_dom_attr_inherited_get(thiz->font_size, &inherited);
	if (inherited)
	{
		thiz->final_font_size = rel->final_font_size;
		egueb_dom_node_unref(inherited);
	}
	else
	{
		Egueb_Svg_Font_Size font_size;
		double w = 1;
		double h = 1;

		egueb_dom_attr_final_get(thiz->font_size, &font_size);
		/* for a relative length, we need the geometry */
		if (font_size.type == EGUEB_SVG_FONT_SIZE_TYPE_LENGTH &&
				font_size.value.length.unit ==
				EGUEB_SVG_UNIT_LENGTH_PERCENT)
		{
			w = geom->viewbox.w;
			h = geom->viewbox.h;
		}
		thiz->final_font_size = egueb_svg_font_size_final_get(
				&font_size, w, h, rel->final_font_size,
				rel->final_font_size);
	}
}
/*----------------------------------------------------------------------------*
 *                             Element interface                              *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_element_process(Egueb_Dom_Element *e)
{
	Egueb_Svg_Element *thiz;
	Egueb_Svg_Element_Class *klass;
	Egueb_Dom_String *style;
	Egueb_Dom_Node *relative = NULL;
	Egueb_Dom_Node *geometry = NULL;
	Eina_Bool ret = EINA_TRUE;

	thiz = EGUEB_SVG_ELEMENT(e);
	DBG_ELEMENT(EGUEB_DOM_NODE(e), "Processing element");
	//DBG("Doing the setup on the tag '%s'", egueb_dom_element_tag_name_get(e));

	/* in case a stylesheet has been applied, re-apply the style atribute */
	/* else in case the style attribute has changed, revert the style and apply
	 * the new style
	 */
	egueb_dom_attr_get(thiz->style, EGUEB_DOM_ATTR_TYPE_BASE,
			&style);
	if (thiz->last_style != style)
	{
		const char *str;

		str = egueb_dom_string_string_get(style);
		/* TODO revert the style */
		DBG_ELEMENT(EGUEB_DOM_NODE(e), "Using a style");
		/* apply the new style */
		egueb_css_context_inline_style_apply(&_egueb_svg_element_css_context, str, thiz);
		/* swap the styles */
		if (thiz->last_style)
		{
			egueb_dom_string_unref(thiz->last_style);
			thiz->last_style = NULL;
		}
		if (style)
		{
			thiz->last_style = egueb_dom_string_ref(style);
		}
		
	}
	if (style) egueb_dom_string_unref(style);

	relative = egueb_svg_element_presentation_relative_get(EGUEB_DOM_NODE(e));
	geometry = egueb_svg_element_geometry_relative_get(EGUEB_DOM_NODE(e));
	/* propagate the geometry information */
	_egueb_svg_element_geometry_process(thiz, geometry);
	/* propagate the inheritable attributes (the presentation attributes) */
	_egueb_svg_element_presentation_attributes_process(thiz, relative, geometry);

	/* now call the element interface for processing this element */
	klass = EGUEB_SVG_ELEMENT_CLASS_GET(thiz);
	if (klass->process) ret = klass->process(thiz);
	if (relative) egueb_dom_node_unref(relative);
	if (geometry) egueb_dom_node_unref(geometry);

	DBG_ELEMENT(EGUEB_DOM_NODE(e), "Done processing element with ret %d", ret);
	return ret;
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/

ENESIM_OBJECT_ABSTRACT_BOILERPLATE(EGUEB_DOM_ELEMENT_DESCRIPTOR,
		Egueb_Svg_Element, Egueb_Svg_Element_Class, egueb_svg_element);

static void _egueb_svg_element_class_init(void *k)
{
	Egueb_Dom_Element_Class *klass;

	klass = EGUEB_DOM_ELEMENT_CLASS(k);
	klass->process = _egueb_svg_element_process;
}

static void _egueb_svg_element_instance_init(void *o)
{
	Egueb_Svg_Element *thiz;
	Egueb_Dom_Node *n;

	thiz = EGUEB_SVG_ELEMENT(o);
	/* create the properties */
	thiz->id = egueb_dom_attr_string_new(
			egueb_dom_string_ref(EGUEB_SVG_ID), NULL, EINA_FALSE,
			EINA_FALSE, EINA_FALSE);
	thiz->klass = egueb_dom_attr_string_new(
			egueb_dom_string_ref(EGUEB_SVG_CLASS), NULL, EINA_FALSE,
			EINA_FALSE, EINA_FALSE);
	thiz->style = egueb_dom_attr_string_new(
			egueb_dom_string_ref(EGUEB_SVG_STYLE), NULL, EINA_FALSE,
			EINA_FALSE, EINA_FALSE);
	/* the presentation attributes */
	thiz->clip_path = egueb_svg_attr_clip_path_new(
			egueb_dom_string_ref(EGUEB_SVG_CLIP_PATH),
			&EGUEB_SVG_CLIP_PATH_NONE, EINA_TRUE, EINA_TRUE,
			EINA_TRUE);
	thiz->color = egueb_svg_attr_color_new(
			egueb_dom_string_ref(EGUEB_SVG_COLOR),
			&EGUEB_SVG_COLOR_BLACK, EINA_TRUE, EINA_TRUE,
			EINA_TRUE);
	thiz->display = egueb_svg_attr_display_new(
			egueb_dom_string_ref(EGUEB_SVG_DISPLAY),
			EGUEB_SVG_DISPLAY_INLINE, EINA_TRUE, EINA_TRUE,
			EINA_TRUE);
	thiz->fill = egueb_svg_attr_paint_new(
			egueb_dom_string_ref(EGUEB_SVG_FILL),
			&EGUEB_SVG_PAINT_BLACK,
			EINA_TRUE, EINA_TRUE, EINA_TRUE);
	thiz->fill_opacity = egueb_svg_attr_number_new(
			egueb_dom_string_ref(EGUEB_SVG_FILL_OPACITY),
			1, EINA_TRUE, EINA_TRUE,
			EINA_TRUE);
	thiz->font_family = egueb_svg_attr_string_new(
			egueb_dom_string_ref(EGUEB_SVG_FONT_FAMILY),
			NULL);
	thiz->font_size = egueb_svg_attr_font_size_new(
			egueb_dom_string_ref(EGUEB_SVG_FONT_SIZE),
			&EGUEB_SVG_FONT_SIZE_MEDIUM, EINA_TRUE, EINA_TRUE, EINA_TRUE);
	thiz->opacity = egueb_svg_attr_number_new(
			egueb_dom_string_ref(EGUEB_SVG_OPACITY),
			1, EINA_TRUE, EINA_TRUE,
			EINA_TRUE);
	/* by default the user agent style sheet sets it to HIDDEN
	 * http://www.w3.org/TR/SVG11/masking.html#OverflowAndClipProperties
	 * TODO whenever we do support agent style sheets we should go back
	 * to visible
	 */
	thiz->overflow = egueb_svg_attr_overflow_new(
			egueb_dom_string_ref(EGUEB_SVG_OVERFLOW),
			EGUEB_SVG_OVERFLOW_HIDDEN, EINA_TRUE, EINA_TRUE,
			EINA_TRUE);
	thiz->stop_color = egueb_svg_attr_color_new(
			egueb_dom_string_ref(EGUEB_SVG_STOP_COLOR),
			&EGUEB_SVG_COLOR_BLACK, EINA_TRUE, EINA_TRUE,
			EINA_TRUE);
	thiz->stop_opacity = egueb_svg_attr_number_new(
			egueb_dom_string_ref(EGUEB_SVG_STOP_OPACITY),
			1, EINA_TRUE, EINA_TRUE,
			EINA_TRUE);
	thiz->stroke = egueb_svg_attr_paint_new(
			egueb_dom_string_ref(EGUEB_SVG_STROKE),
			&EGUEB_SVG_PAINT_NONE,
			EINA_TRUE, EINA_TRUE, EINA_TRUE);
	thiz->stroke_dasharray = egueb_svg_attr_stroke_dasharray_new(
			egueb_dom_string_ref(EGUEB_SVG_STROKE_DASHARRAY),
			NULL, EINA_TRUE, EINA_TRUE, EINA_TRUE);
	thiz->stroke_line_cap = egueb_svg_attr_stroke_line_cap_new(
			egueb_dom_string_ref(EGUEB_SVG_STROKE_LINECAP),
			EGUEB_SVG_STROKE_LINE_CAP_BUTT,
			EINA_TRUE, EINA_TRUE, EINA_TRUE);
	/* FIXME shoud be miter */
	thiz->stroke_line_join = egueb_svg_attr_stroke_line_join_new(
			egueb_dom_string_ref(EGUEB_SVG_STROKE_LINEJOIN),
			EGUEB_SVG_STROKE_LINE_JOIN_ROUND,
			EINA_TRUE, EINA_TRUE, EINA_TRUE);
	thiz->stroke_width = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_STROKE_WIDTH),
			&EGUEB_SVG_LENGTH_1, EINA_TRUE, EINA_TRUE, EINA_TRUE);
	thiz->stroke_opacity = egueb_svg_attr_number_new(
			egueb_dom_string_ref(EGUEB_SVG_STROKE_OPACITY),
			1, EINA_TRUE, EINA_TRUE,
			EINA_TRUE);
	thiz->text_anchor = egueb_svg_attr_text_anchor_new(
			egueb_dom_string_ref(EGUEB_SVG_TEXT_ANCHOR),
			EGUEB_SVG_TEXT_ANCHOR_START, EINA_TRUE, EINA_TRUE,
			EINA_TRUE);
	thiz->visibility = egueb_svg_attr_visibility_new(
			egueb_dom_string_ref(EGUEB_SVG_VISIBILITY),
			EGUEB_SVG_VISIBILITY_VISIBLE, EINA_TRUE, EINA_TRUE,
			EINA_TRUE);

	n = EGUEB_DOM_NODE(o);

	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->id), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->klass), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->style), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->clip_path), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->color), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->display), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->fill), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->fill_opacity), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->font_family), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->font_size), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->opacity), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->overflow), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->stop_opacity), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->stop_color), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->stroke), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->stroke_dasharray), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->stroke_line_join), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->stroke_line_cap), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->stroke_opacity), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->stroke_width), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->text_anchor), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->visibility), NULL);
	/* our private stuff */
	enesim_matrix_identity(&thiz->transform);
}

static void _egueb_svg_element_instance_deinit(void *o)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(o);
	/* destroy the properties */
	egueb_dom_node_unref(thiz->id);
	egueb_dom_node_unref(thiz->klass);
	egueb_dom_node_unref(thiz->style);
	egueb_dom_node_unref(thiz->clip_path);
	egueb_dom_node_unref(thiz->color);
	egueb_dom_node_unref(thiz->display);
	egueb_dom_node_unref(thiz->fill);
	egueb_dom_node_unref(thiz->fill_opacity);
	egueb_dom_node_unref(thiz->font_family);
	egueb_dom_node_unref(thiz->font_size);
	egueb_dom_node_unref(thiz->opacity);
	egueb_dom_node_unref(thiz->overflow);
	egueb_dom_node_unref(thiz->stop_color);
	egueb_dom_node_unref(thiz->stop_opacity);
	egueb_dom_node_unref(thiz->stroke);
	egueb_dom_node_unref(thiz->stroke_dasharray);
	egueb_dom_node_unref(thiz->stroke_line_cap);
	egueb_dom_node_unref(thiz->stroke_line_join);
	egueb_dom_node_unref(thiz->stroke_opacity);
	egueb_dom_node_unref(thiz->stroke_width);
	egueb_dom_node_unref(thiz->text_anchor);
	egueb_dom_node_unref(thiz->visibility);
	/* remove the weak references */
	if (thiz->geometry_relative)
	{
		egueb_dom_node_weak_ref_remove(EGUEB_DOM_NODE(thiz),
				&thiz->geometry_relative);
	}
	if (thiz->presentation_relative)
	{
		egueb_dom_node_weak_ref_remove(EGUEB_DOM_NODE(thiz),
				&thiz->presentation_relative);
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_svg_element_clip_path_resolve(Egueb_Dom_Node *n,
		Egueb_Svg_Clip_Path *clip_path_current,
		Egueb_Svg_Clip_Path *clip_path_last, Egueb_Svg_Reference **r)
{
	if (!egueb_svg_clip_path_is_equal(clip_path_current, clip_path_last))
	{
		DBG("Clip path changed");
		/* remove the reference in case it has it */
		if (*r)
		{
			egueb_svg_reference_free(*r);
			*r = NULL;
		}

		/* add a reference for this clip path */
		if (clip_path_current->type == EGUEB_SVG_CLIP_PATH_TYPE_IRI)
		{
			Egueb_Dom_Node *ref = NULL;
			Egueb_Dom_Node *doc = NULL;
			Egueb_Dom_String iri = EGUEB_DOM_STRING_STATIC(clip_path_current->value.iri);

			doc = egueb_dom_node_document_get(n);
			if (!doc)
			{
				WARN("No document set");
				return;
			}
			ref = egueb_svg_document_element_get_by_iri(doc, &iri);
			egueb_dom_node_unref(doc);

			if (!ref || !egueb_svg_element_is_clip_path(ref))
			{
				ERR("Node with iri '%s' not found for the clip path",
						clip_path_current->value.iri);
				if (ref) egueb_dom_node_unref(ref);
			}
			else
			{
				*r = egueb_svg_referenceable_reference_add(ref,
						n, NULL);
				egueb_dom_node_unref(ref);
				egueb_svg_reference_process(*r);
			}
		}
		/* keep a copy */
		egueb_svg_clip_path_reset(clip_path_last);
		egueb_svg_clip_path_copy(clip_path_current, clip_path_last, EINA_FALSE);
	}
}

void egueb_svg_element_children_clone(Egueb_Dom_Node *n, Egueb_Dom_Node *from)
{
	Egueb_Dom_Node *from_child;

	/* clone every children */
	EINA_INLIST_FOREACH(from->children, from_child)
	{
		Egueb_Dom_Node *n_child;

		n_child = egueb_dom_node_clone(from_child, EINA_TRUE, EINA_TRUE, NULL);
		if (n_child)
			egueb_dom_node_child_append(n, n_child, NULL);
	}
}

#if 0
Eina_Bool egueb_svg_element_attribute_animation_add(Egueb_Dom_Tag *t, const char *attr,
		int *index)
{
	int *animated;

	DBG("Adding animation on '%s'", attr);
	animated = _egueb_svg_element_attribute_animated_get(t, attr);
	if (animated)
	{
		(*animated)++;
		*index = *animated;
		return EINA_TRUE;
	}
	else
	{
		return EINA_FALSE;
	}
}

void egueb_svg_element_attribute_animation_remove(Egueb_Dom_Tag *t, const char *attr)
{
	int *animated;

	DBG("Removing animation on '%s'", attr);
	animated = _egueb_svg_element_attribute_animated_get(t, attr);
	if (animated)
		(*animated)--;
}

Egueb_Svg_Type egueb_svg_element_internal_type_get(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(t);
	return thiz->type;
}

const char * egueb_svg_element_internal_id_get(Egueb_Dom_Tag *t)
{
	const char *id;

	_egueb_svg_element_id_get(t, &id);
	return id;
}

Eina_Bool egueb_svg_is_element_internal(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element *thiz;
	Eina_Bool ret;

	thiz = egueb_dom_tag_data_get(t);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_ELEMENT_MAGIC);

	return ret;
}

void egueb_svg_element_internal_topmost_get(Egueb_Dom_Tag *t, Ender_Element **e)
{
	Egueb_Svg_Element *thiz;

	if (!e) return;
	thiz = EGUEB_SVG_ELEMENT(t);
	*e = thiz->topmost;
}

/* FIXME what do we do here? return bool or the setup return? */
Eina_Bool egueb_svg_element_internal_child_setup(Egueb_Dom_Tag *t,
		Egueb_Svg_Context *c,
		Enesim_Log **error,
		Egueb_Svg_Element_Setup_Interceptor pre,
		Egueb_Svg_Element_Setup_Interceptor post,
		void *data)
{
	Egueb_Svg_Element_Setup_Data setup_data;

	setup_data.c = c;
	setup_data.pre = pre;
	setup_data.post = post;
	setup_data.error = error;
	setup_data.ret = EINA_TRUE;
	setup_data.data = data;

	egueb_dom_tag_child_foreach(t, _egueb_svg_element_child_setup_cb, &setup_data);
	return setup_data.ret;
}

Egueb_Svg_Element_Setup_Return egueb_svg_element_setup_rel(Egueb_Dom_Tag *t,
		Egueb_Svg_Context *c,
		const Egueb_Svg_Element_Context *rel_state,
		const Egueb_Svg_Attribute_Presentation *rel_attr,
		Enesim_Log **error)
{
	Egueb_Svg_Element *thiz;
	Egueb_Svg_Element_Setup_Return ret;

	thiz = EGUEB_SVG_ELEMENT(t);
	DBG("Doing the setup on the tag '%s'", egueb_dom_tag_name_get(t));
	/* FIXME given that we have to only setup a subtree, we should
	 * not get the parents attributes from the arguments */
	thiz->last_run = c->run;
#if 0
	if (thiz->last_run == c->run)
	{
		DBG("already run");
		return EINA_TRUE;
	}
#endif

	thiz->last_run = c->run;

	/* the idea here is to call the setup interface of the element */
	/* note that on SVG every element must be part of a topmost SVG
	 * that way we need to always pass the upper svg/g element of this
	 * so relative properties can be calcualted correctly */

	/* FIXME avoid so many copies */
	_egueb_svg_element_state_compose(thiz, rel_state, &thiz->state_final);

	/* FIXME should it have more priority than the properties? */
	/* FIXME dont check for the style only, check also if some style property has been
	 * set. If we animate a style based property but the style is not set, we never
	 * get in here
	 */

	/* apply the style first */
	/* FIXME check that the style has changed, if so revert it and start applying */
	if (thiz->style_changed)
	{
		/* reset the current style ? */
		/* apply it */
		if (thiz->style)
		{
			egueb_svg_element_attribute_type_set(t, ESVG_ATTR_CSS);
			egueb_css_context_inline_style_apply(&_egueb_svg_element_css_context, thiz->style, t);
			egueb_svg_element_attribute_type_set(t, ESVG_ATTR_XML);
		}
		thiz->style_changed = EINA_FALSE;
	}

	if (thiz->attr_css.sets)
	{
		/* merge the css and the xml into the final */
		_egueb_svg_element_attribute_presentation_merge_rel(&thiz->attr_xml, &thiz->attr_css, &thiz->attr_final);
	}
	else
	{
		_egueb_svg_element_attribute_presentation_merge(&thiz->attr_xml, &thiz->attr_final);
	}

	if (rel_attr)
	{
		egueb_svg_attribute_presentation_merge_rel(rel_attr, &thiz->attr_final, &thiz->attr_final);
	}


	if (!thiz->descriptor.setup)
		return ESVG_SETUP_OK;

	//egueb_svg_attribute_presentation_dump(new_attr);
	ret = thiz->descriptor.setup(t, c, rel_state, &thiz->state_final, &thiz->attr_final, error);
	if (ret == ESVG_SETUP_ENQUEUE)
	{
		egueb_svg_context_setup_enqueue(c, t);
	}
	else
	{
		thiz->changed = 0;
	}
	return ret;
}

Egueb_Svg_Element_Setup_Return egueb_svg_element_internal_setup(Egueb_Dom_Tag *t,
		Egueb_Svg_Context *c,
		Enesim_Log **error)
{
	Egueb_Dom_Tag *parent_t;
	Egueb_Svg_Element_Context *parent_state = NULL;
	Egueb_Svg_Attribute_Presentation *parent_attr = NULL;

	parent_t = egueb_dom_tag_parent_get(t);
	if (parent_t)
	{

		Egueb_Svg_Element *parent_thiz;

		parent_thiz = EGUEB_SVG_ELEMENT(parent_t);
		parent_state = &parent_thiz->state_final;
		parent_attr = &parent_thiz->attr_final;
	}
	return egueb_svg_element_setup_rel(t, c,
			parent_state, parent_attr, error);
}

void egueb_svg_element_initialize(Ender_Element *e)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(ender_element_object_get(e));
	thiz->e = e;

	/* register the mutation events */
	ender_event_listener_add(e, "Mutation", _egueb_svg_element_mutation_cb, thiz);
	/* register the dom events */
	ender_event_listener_add(e, "focusin", _egueb_svg_element_onfocusin_call, thiz);
	ender_event_listener_add(e, "focusout", _egueb_svg_element_onfocusout_call, thiz);
	ender_event_listener_add(e, "activate", _egueb_svg_element_onactivate_call, thiz);
	ender_event_listener_add(e, "click", _egueb_svg_element_onclick_call, thiz);
	ender_event_listener_add(e, "mousedown", _egueb_svg_element_onmousedown_call, thiz);
	ender_event_listener_add(e, "mouseup", _egueb_svg_element_onmouseup_call, thiz);
	ender_event_listener_add(e, "mouseover", _egueb_svg_element_onmouseover_call, thiz);
	ender_event_listener_add(e, "mousemove", _egueb_svg_element_onmousemove_call, thiz);
	ender_event_listener_add(e, "mouseout", _egueb_svg_element_onmouseout_call, thiz);

	if (thiz->descriptor.initialize)
		thiz->descriptor.initialize(e);
}

void egueb_svg_element_egueb_css_style_apply(Egueb_Dom_Tag *t, Egueb_Css_Style *s)
{
	egueb_svg_element_attribute_type_set(t, ESVG_ATTR_CSS);
	egueb_css_context_style_apply(&_egueb_svg_element_css_context, s, t);
	egueb_svg_element_attribute_type_set(t, ESVG_ATTR_XML);
}

void egueb_svg_element_topmost_set(Egueb_Dom_Tag *t, Ender_Element *topmost)
{
	Egueb_Svg_Element *thiz;
	Egueb_Svg_Element_Event_Topmost_Changed event_data;

	thiz = EGUEB_SVG_ELEMENT(t);
	/* if the topmost to set is the same, do nothing */
	if (thiz->topmost == topmost)
		return;

	event_data.previous = thiz->topmost;
	event_data.current = topmost;
	event_data.child = t;
	if (topmost)
		ender_event_dispatch(topmost, "TopmostChanged", &event_data);
	if (thiz->topmost)
		ender_event_dispatch(thiz->topmost, "TopmostChanged", &event_data);
	thiz->topmost = topmost;
}

void * egueb_svg_element_data_get(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(t);
	return thiz->data;
}

void egueb_svg_element_attribute_type_set(Egueb_Dom_Tag *t, Egueb_Svg_Attribute_Type type)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(t);
	if (type == thiz->current_attr_type)
		return;

	switch (type)
	{
		case ESVG_ATTR_CSS:
		thiz->current_attr = &thiz->attr_css;
		thiz->current_attr_type = ESVG_ATTR_CSS;
		break;

		case ESVG_ATTR_XML:
		thiz->current_attr_type = ESVG_ATTR_XML;
		thiz->current_attr = &thiz->attr_xml;
		break;

		default:
		return;
	}
}

Egueb_Svg_Attribute_Type egueb_svg_element_attribute_type_get(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(t);
	return thiz->current_attr_type;
}

void egueb_svg_element_attribute_animate_set(Egueb_Dom_Tag *t, Eina_Bool animate)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(t);
	thiz->current_attr_animate = animate;
}

Eina_Bool egueb_svg_element_attribute_animate_get(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(t);
	return thiz->current_attr_animate;
}

Ender_Element * egueb_svg_element_ender_get(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(t);
	return thiz->e;
}

Eina_Bool egueb_svg_element_changed(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(t);
	return thiz->changed;
}

Eina_Bool egueb_svg_element_has_setup(Egueb_Dom_Tag *t, Egueb_Svg_Context *c)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(t);
	if (thiz->last_run == c->run)
		return EINA_TRUE;
	return EINA_FALSE;
}

const Egueb_Svg_Element_Context * egueb_svg_element_context_get(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(t);
	return &thiz->state_final;
}

const Egueb_Svg_Attribute_Presentation * egueb_svg_element_attribute_presentation_get(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(t);
	return &thiz->attr_final;
}
#endif
/*----------------------------------------------------------------------------*
 *                           Property API helpers                             *
 *----------------------------------------------------------------------------*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool egueb_svg_is_element(Egueb_Dom_Node *n)
{
	if (!n) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(n),
			EGUEB_SVG_ELEMENT_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}

EAPI Egueb_Dom_Node * egueb_svg_element_geometry_relative_get(Egueb_Dom_Node *n)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	if (thiz->geometry_relative)
	{
		return egueb_dom_node_ref(thiz->geometry_relative);
	}
	else
	{
		Egueb_Dom_Node *parent;

		parent = egueb_dom_node_parent_get(n);
		/* check if it is the topmost */
		if (egueb_dom_node_type_get(parent) == EGUEB_DOM_NODE_TYPE_DOCUMENT_NODE)
		{
			egueb_dom_node_unref(parent);
			parent = NULL;
		}
		return parent;
	}
}

EAPI Eina_Bool egueb_svg_element_geometry_relative_set(Egueb_Dom_Node *n,
		Egueb_Dom_Node *geometry_relative, Eina_Error *err)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	if (thiz->geometry_relative)
	{
		egueb_dom_node_weak_ref_remove(thiz->geometry_relative,
				&thiz->geometry_relative);
		thiz->geometry_relative = NULL;
	}
	if (geometry_relative)
	{
		if (!egueb_svg_is_element(geometry_relative))
		{
			if (err) *err = EGUEB_DOM_ERROR_NOT_SUPPORTED;
			return EINA_FALSE;
		}
		egueb_dom_node_weak_ref_add(geometry_relative,
			&thiz->geometry_relative);
	}
	return EINA_TRUE;
}


EAPI Egueb_Dom_Node * egueb_svg_element_presentation_relative_get(
		Egueb_Dom_Node *n)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	if (thiz->presentation_relative)
	{
		return egueb_dom_node_ref(thiz->presentation_relative);
	}
	else
	{
		Egueb_Dom_Node *parent;

		parent = egueb_dom_node_parent_get(n);
		if (!parent)
		{
			CRIT_ELEMENT(n, "No parent available");
			return NULL;
		}
		/* check if it is the topmost */
		if (egueb_dom_node_type_get(parent) == EGUEB_DOM_NODE_TYPE_DOCUMENT_NODE)
		{
			egueb_dom_node_unref(parent);
			parent = NULL;
		}
		return parent;
	}
}

EAPI Eina_Bool egueb_svg_element_presentation_relative_set(Egueb_Dom_Node *n,
		Egueb_Dom_Node *presentation_relative, Eina_Error *err)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	if (thiz->presentation_relative)
	{
		egueb_dom_node_weak_ref_remove(thiz->presentation_relative,
				&thiz->presentation_relative);
		thiz->presentation_relative = NULL;
	}
	if (presentation_relative)
	{
		if (!egueb_svg_is_element(presentation_relative))
		{
			if (err) *err = EGUEB_DOM_ERROR_NOT_SUPPORTED;
			return EINA_FALSE;
		}

		egueb_dom_node_weak_ref_add(presentation_relative,
				&thiz->presentation_relative);
	}
	return EINA_TRUE;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_id_get(Egueb_Dom_Node *n, const char **id)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_id_set(Egueb_Dom_Node *n, const char *id)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_class_set(Egueb_Dom_Node *n, const char *class)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_transform_set(Egueb_Dom_Node *n, const Enesim_Matrix *transform)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_transform_get(Egueb_Dom_Node *n, Enesim_Matrix *transform)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_style_set(Egueb_Dom_Node *n, const char *style)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_style_get(Egueb_Dom_Node *n, const char **style)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_clip_path_set(Egueb_Dom_Node *n, Egueb_Svg_Clip_Path *clip_path)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	egueb_dom_attr_set(thiz->clip_path, EGUEB_DOM_ATTR_TYPE_BASE, clip_path);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_clip_path_get(Egueb_Dom_Node *n, Egueb_Svg_Clip_Path_Animated *clip_path)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	EGUEB_SVG_ELEMENT_ATTR_PRIMITIVE_GET(thiz->clip_path, clip_path, egueb_svg_clip_path_copy);

}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_clip_path_final_get(Egueb_Dom_Node *n,
		Egueb_Svg_Clip_Path *clip_path)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	egueb_dom_attr_final_get(thiz->clip_path, clip_path);
}


/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_opacity_set(Egueb_Dom_Node *n, const Egueb_Svg_Number *v)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_opacity_get(Egueb_Dom_Node *n, Egueb_Svg_Number *v)
{
}


/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_opacity_unset(Egueb_Dom_Node *n)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_overflow_set(Egueb_Dom_Node *n, const Egueb_Svg_Overflow o)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	egueb_dom_attr_set(thiz->overflow, EGUEB_DOM_ATTR_TYPE_BASE, o);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_overflow_final_get(Egueb_Dom_Node *n, Egueb_Svg_Overflow *o)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	egueb_dom_attr_final_get(thiz->overflow, o);
}


/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_color_set(Egueb_Dom_Node *n, const Egueb_Svg_Color *color)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	egueb_dom_attr_set(thiz->color, EGUEB_DOM_ATTR_TYPE_BASE, color);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_color_get(Egueb_Dom_Node *n, Egueb_Svg_Color_Animated *color)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	EGUEB_SVG_ELEMENT_ATTR_SIMPLE_GET(thiz->color, color);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_color_final_get(Egueb_Dom_Node *n,
		Egueb_Svg_Color *color)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	egueb_dom_attr_final_get(thiz->color, color);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_fill_set(Egueb_Dom_Node *n, const Egueb_Svg_Paint *fill)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	egueb_dom_attr_set(thiz->fill, EGUEB_DOM_ATTR_TYPE_BASE, fill);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_fill_get(Egueb_Dom_Node *n, Egueb_Svg_Paint_Animated *fill)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	EGUEB_SVG_ELEMENT_ATTR_PRIMITIVE_GET(thiz->fill, fill, egueb_svg_paint_copy);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_fill_final_get(Egueb_Dom_Node *n,
		Egueb_Svg_Paint *fill)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	egueb_dom_attr_final_get(thiz->fill, fill);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_fill_opacity_set(Egueb_Dom_Node *n, const Egueb_Svg_Number *v)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	egueb_dom_attr_set(thiz->fill_opacity, EGUEB_DOM_ATTR_TYPE_BASE, v);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_fill_opacity_get(Egueb_Dom_Node *n, Egueb_Svg_Number *v)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_fill_opacity_unset(Egueb_Dom_Node *n)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_fill_rule_set(Egueb_Dom_Node *n, Egueb_Svg_Fill_Rule fill_rule)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_fill_rule_unset(Egueb_Dom_Node *n)
{
}

EAPI void egueb_svg_element_font_size_set(Egueb_Dom_Node *n,
		const Egueb_Svg_Font_Size *font_size)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	egueb_dom_attr_set(thiz->font_size, EGUEB_DOM_ATTR_TYPE_BASE,
			font_size);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_stroke_set(Egueb_Dom_Node *n, const Egueb_Svg_Paint *stroke)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	egueb_dom_attr_set(thiz->stroke, EGUEB_DOM_ATTR_TYPE_BASE, stroke);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_stroke_get(Egueb_Dom_Node *n, Egueb_Svg_Paint_Animated *stroke)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	EGUEB_SVG_ELEMENT_ATTR_PRIMITIVE_GET(thiz->stroke, stroke, egueb_svg_paint_copy);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_stroke_final_get(Egueb_Dom_Node *n,
		Egueb_Svg_Paint *stroke)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	egueb_dom_attr_final_get(thiz->stroke, stroke);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_stroke_width_set(Egueb_Dom_Node *n, const Egueb_Svg_Length *stroke_width)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	egueb_dom_attr_set(thiz->stroke_width, EGUEB_DOM_ATTR_TYPE_BASE, stroke_width);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_stroke_width_unset(Egueb_Dom_Node *n)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_stroke_opacity_set(Egueb_Dom_Node *n, const Egueb_Svg_Number *v)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	egueb_dom_attr_set(thiz->stroke_opacity, EGUEB_DOM_ATTR_TYPE_BASE, v);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_stroke_opacity_get(Egueb_Dom_Node *n, Egueb_Svg_Number *v)
{
}


/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_stroke_opacity_unset(Egueb_Dom_Node *n)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_stroke_line_cap_set(Egueb_Dom_Node *n, Egueb_Svg_Stroke_Line_Cap cap)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_stroke_line_cap_unset(Egueb_Dom_Node *n)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_stroke_line_join_set(Egueb_Dom_Node *n, Egueb_Svg_Stroke_Line_Join join)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_stroke_line_join_unset(Egueb_Dom_Node *n)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_visibility_set(Egueb_Dom_Node *n, Egueb_Svg_Visibility visibility)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_visibility_unset(Egueb_Dom_Node *n)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_display_set(Egueb_Dom_Node *n, Egueb_Svg_Display display)
{
}


/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_stop_opacity_set(Egueb_Dom_Node *n, const Egueb_Svg_Number *v)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_stop_opacity_get(Egueb_Dom_Node *n, Egueb_Svg_Number *v)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_stop_color_set(Egueb_Dom_Node *n, Egueb_Svg_Color *stop_color)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_stop_color_get(Egueb_Dom_Node *n, Egueb_Svg_Color *stop_color)
{
}

#if 0
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_onfocusin_set(Egueb_Dom_Node *n, const char *v)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_onfocusout_set(Egueb_Dom_Node *n, const char *v)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_onactivate_set(Egueb_Dom_Node *n, const char *v)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_onclick_set(Egueb_Dom_Node *n, const char *v)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_onmousedown_set(Egueb_Dom_Node *n, const char *v)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_onmouseup_set(Egueb_Dom_Node *n, const char *v)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_onmouseover_set(Egueb_Dom_Node *n, const char *v)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_onmousemove_set(Egueb_Dom_Node *n, const char *v)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_onmouseout_set(Egueb_Dom_Node *n, const char *v)
{
}
#endif
