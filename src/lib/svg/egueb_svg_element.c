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
 *                              Event handlers                                *
 *----------------------------------------------------------------------------*/
static void _egueb_svg_element_event_handler(Egueb_Dom_Event *ev,
		Egueb_Svg_Element *thiz, Egueb_Dom_Node *attr,
		Egueb_Dom_String **latest, void **obj)
{
	Egueb_Dom_String *curr = NULL;
	Eina_Bool changed = EINA_FALSE;

	/* if we are not at target, do nothing */
	if (egueb_dom_event_phase_get(ev) != EGUEB_DOM_EVENT_PHASE_AT_TARGET)
		return;

	egueb_dom_attr_final_get(attr, &curr);
	if (!egueb_dom_string_is_equal(curr, *latest))
	{
		changed = EINA_TRUE;
		/* update the latest */
		*latest = egueb_dom_string_ref(curr);
	}

	/* request the scripter in case we dont have one */
	if (!thiz->scripter && curr)
	{
		Egueb_Dom_Event *sev;
		Egueb_Dom_String *type;

		/* TODO check the attribute */
		type = egueb_dom_string_new_with_string("application/ecmascript");
		sev = egueb_dom_event_script_new(type);
		egueb_dom_node_event_dispatch(EGUEB_DOM_NODE(thiz),
				egueb_dom_event_ref(sev), NULL, NULL);
		/* instantiate the vm */
		thiz->scripter = egueb_dom_event_script_scripter_get(sev);
		egueb_dom_event_unref(sev);
	}

	if (thiz->scripter)
	{
		/* TODO check if we do have a new script and we had an object */
		if (changed)
		{
			/* TODO free it */
			if (*obj)
			{
				egueb_dom_scripter_script_free(thiz->scripter, *obj);
				*obj = NULL;
			}
			if (curr)
				egueb_dom_scripter_load(thiz->scripter, curr, obj);
		}
		/* TODO pass the context (thiz, evt, etc) */
		if (*obj)
		{
			egueb_dom_scripter_script_run_listener(thiz->scripter, *obj, ev);
		}
	}

	egueb_dom_string_unref(curr);
	/* call the script with the passed in arg */
}

#if 0
static void _egueb_svg_element_onfocusin_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Svg_Element *thiz = data;
}

static void _egueb_svg_element_onfocusout_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Svg_Element *thiz = data;
}

static void _egueb_svg_element_onactivate_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Svg_Element *thiz = data;
}
#endif

static void _egueb_svg_element_onclick_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Svg_Element *thiz = data;

	_egueb_svg_element_event_handler(e, thiz, thiz->onclick,
		&thiz->onclick_last, &thiz->onclick_obj);
}

static void _egueb_svg_element_onmousedown_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Svg_Element *thiz = data;

	_egueb_svg_element_event_handler(e, thiz, thiz->onmousedown,
		&thiz->onmousedown_last, &thiz->onmousedown_obj);
}

static void _egueb_svg_element_onmouseup_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Svg_Element *thiz = data;

	_egueb_svg_element_event_handler(e, thiz, thiz->onmouseup,
		&thiz->onmouseup_last, &thiz->onmouseup_obj);
}

static void _egueb_svg_element_onmousemove_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Svg_Element *thiz = data;

	_egueb_svg_element_event_handler(e, thiz, thiz->onmousemove,
		&thiz->onmousemove_last, &thiz->onmousemove_obj);
}

static void _egueb_svg_element_onmouseover_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Svg_Element *thiz = data;

	_egueb_svg_element_event_handler(e, thiz, thiz->onmouseover,
		&thiz->onmouseover_last, &thiz->onmouseover_obj);
}

static void _egueb_svg_element_onmouseout_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Svg_Element *thiz = data;

	_egueb_svg_element_event_handler(e, thiz, thiz->onmouseout,
		&thiz->onmouseout_last, &thiz->onmouseout_obj);
}

#if 0
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
		/* TODO revert the style */
		DBG_ELEMENT(EGUEB_DOM_NODE(e), "Using a style");
		/* apply the new style */
		egueb_css_attr_style_process(thiz->style);
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
	thiz->style = egueb_css_attr_style_new();
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
	/* event attributes */
	thiz->onclick = egueb_dom_attr_string_new(
			egueb_dom_string_ref(EGUEB_SVG_NAME_ONCLICK), NULL,
			EINA_FALSE, EINA_FALSE, EINA_FALSE);
	thiz->onmousedown = egueb_dom_attr_string_new(
			egueb_dom_string_ref(EGUEB_SVG_NAME_ONMOUSEDOWN), NULL,
			EINA_FALSE, EINA_FALSE, EINA_FALSE);
	thiz->onmouseup = egueb_dom_attr_string_new(
			egueb_dom_string_ref(EGUEB_SVG_NAME_ONMOUSEUP), NULL,
			EINA_FALSE, EINA_FALSE, EINA_FALSE);
	thiz->onmouseover = egueb_dom_attr_string_new(
			egueb_dom_string_ref(EGUEB_SVG_NAME_ONMOUSEOVER), NULL,
			EINA_FALSE, EINA_FALSE, EINA_FALSE);
	thiz->onmousemove = egueb_dom_attr_string_new(
			egueb_dom_string_ref(EGUEB_SVG_NAME_ONMOUSEMOVE), NULL,
			EINA_FALSE, EINA_FALSE, EINA_FALSE);
	thiz->onmouseout = egueb_dom_attr_string_new(
			egueb_dom_string_ref(EGUEB_SVG_NAME_ONMOUSEOUT), NULL,
			EINA_FALSE, EINA_FALSE, EINA_FALSE);

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
	/* event attributes */
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->onclick), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->onmousedown), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->onmouseup), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->onmouseover), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->onmousemove), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->onmouseout), NULL);

	/* register the dom events */
#if 0
	egueb_dom_node_event_listener_add(n, "focusin", _egueb_svg_element_onfocusin_cb, thiz);
	egueb_dom_node_event_listener_add(n, "focusout", _egueb_svg_element_onfocusout_cb, thiz);
	egueb_dom_node_event_listener_add(n, "activate", _egueb_svg_element_onactivate_cb, thiz);
#endif
	egueb_dom_node_event_listener_add(n, EGUEB_DOM_EVENT_MOUSE_CLICK,
			_egueb_svg_element_onclick_cb, EINA_TRUE, thiz);
	egueb_dom_node_event_listener_add(n, EGUEB_DOM_EVENT_MOUSE_DOWN,
			_egueb_svg_element_onmousedown_cb, EINA_TRUE, thiz);
	egueb_dom_node_event_listener_add(n, EGUEB_DOM_EVENT_MOUSE_UP,
			_egueb_svg_element_onmouseup_cb, EINA_TRUE, thiz);
	egueb_dom_node_event_listener_add(n, EGUEB_DOM_EVENT_MOUSE_OVER,
			_egueb_svg_element_onmouseover_cb, EINA_TRUE, thiz);
	egueb_dom_node_event_listener_add(n, EGUEB_DOM_EVENT_MOUSE_MOVE,
			_egueb_svg_element_onmousemove_cb, EINA_TRUE, thiz);
	egueb_dom_node_event_listener_add(n, EGUEB_DOM_EVENT_MOUSE_OUT,
			_egueb_svg_element_onmouseout_cb, EINA_TRUE, thiz);

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
	/* the event attributes */
	egueb_dom_node_unref(thiz->onclick);
	egueb_dom_node_unref(thiz->onmousedown);
	egueb_dom_node_unref(thiz->onmouseup);
	egueb_dom_node_unref(thiz->onmouseover);
	egueb_dom_node_unref(thiz->onmousemove);
	egueb_dom_node_unref(thiz->onmouseout);
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

	/* TODO add the needed events whenever a children is added/removed to the from node */
	/* clone every children */
	EINA_INLIST_FOREACH(from->children, from_child)
	{
		Egueb_Dom_Node *n_child;
		Egueb_Dom_String *s;

		s = egueb_dom_node_name_get(from_child);
		DBG_ELEMENT(n, "Cloning child %s", egueb_dom_string_string_get(s));
		egueb_dom_string_unref(s);

		n_child = egueb_dom_node_clone(from_child, EINA_TRUE, EINA_TRUE, NULL);
		if (n_child)
			egueb_dom_node_child_append(n, n_child, NULL);
	}
}

void egueb_svg_element_children_process(Egueb_Dom_Node *n)
{
	Egueb_Dom_Node *child;

	EINA_INLIST_FOREACH(n->children, child)
	{
		Egueb_Dom_Node_Type type;
		type = egueb_dom_node_type_get(child);
		if (type == EGUEB_DOM_NODE_TYPE_ELEMENT)
			egueb_dom_element_process(child);
	}
}

Egueb_Dom_Node * egueb_svg_element_geometry_relative_get(Egueb_Dom_Node *n)
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
		if (egueb_dom_node_type_get(parent) == EGUEB_DOM_NODE_TYPE_DOCUMENT)
		{
			egueb_dom_node_unref(parent);
			parent = NULL;
		}
		return parent;
	}
}

Eina_Bool egueb_svg_element_geometry_relative_set(Egueb_Dom_Node *n,
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


Egueb_Dom_Node * egueb_svg_element_presentation_relative_get(
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
		if (egueb_dom_node_type_get(parent) == EGUEB_DOM_NODE_TYPE_DOCUMENT)
		{
			egueb_dom_node_unref(parent);
			parent = NULL;
		}
		return parent;
	}
}

Eina_Bool egueb_svg_element_presentation_relative_set(Egueb_Dom_Node *n,
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

void egueb_svg_element_clip_path_final_get(Egueb_Dom_Node *n,
		Egueb_Svg_Clip_Path *clip_path)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	egueb_dom_attr_final_get(thiz->clip_path, clip_path);
}

void egueb_svg_element_overflow_final_get(Egueb_Dom_Node *n, Egueb_Svg_Overflow *o)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	egueb_dom_attr_final_get(thiz->overflow, o);
}

void egueb_svg_element_color_final_get(Egueb_Dom_Node *n,
		Egueb_Svg_Color *color)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	egueb_dom_attr_final_get(thiz->color, color);
}

void egueb_svg_element_fill_final_get(Egueb_Dom_Node *n,
		Egueb_Svg_Paint *fill)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	egueb_dom_attr_final_get(thiz->fill, fill);
}

void egueb_svg_element_stroke_final_get(Egueb_Dom_Node *n,
		Egueb_Svg_Paint *stroke)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	egueb_dom_attr_final_get(thiz->stroke, stroke);
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
EAPI Eina_Bool egueb_svg_is_element(Egueb_Dom_Node *n)
{
	if (!n) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(n),
			EGUEB_SVG_ELEMENT_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}

EAPI void egueb_svg_element_clip_path_set(Egueb_Dom_Node *n, Egueb_Svg_Clip_Path *v)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	egueb_dom_attr_set(thiz->clip_path, EGUEB_DOM_ATTR_TYPE_BASE, v);
}

EAPI void egueb_svg_element_clip_path_get(Egueb_Dom_Node *n, Egueb_Svg_Clip_Path_Animated *v)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->clip_path, v);
}

EAPI void egueb_svg_element_opacity_set(Egueb_Dom_Node *n, const Egueb_Svg_Number *v)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	egueb_dom_attr_set(thiz->opacity, EGUEB_DOM_ATTR_TYPE_BASE, v);
}

EAPI void egueb_svg_element_opacity_get(Egueb_Dom_Node *n, Egueb_Svg_Number_Animated *v)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->opacity, v);
}

EAPI void egueb_svg_element_overflow_set(Egueb_Dom_Node *n, const Egueb_Svg_Overflow v)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	egueb_dom_attr_set(thiz->overflow, EGUEB_DOM_ATTR_TYPE_BASE, v);
}

EAPI void egueb_svg_element_overflow_get(Egueb_Dom_Node *n, Egueb_Svg_Overflow_Animated *v)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->overflow, v);
}

EAPI void egueb_svg_element_color_set(Egueb_Dom_Node *n, const Egueb_Svg_Color *v)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	egueb_dom_attr_set(thiz->color, EGUEB_DOM_ATTR_TYPE_BASE, v);
}

EAPI void egueb_svg_element_color_get(Egueb_Dom_Node *n, Egueb_Svg_Color_Animated *v)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	EGUEB_SVG_ELEMENT_ATTR_SIMPLE_GET(thiz->color, v);
}

EAPI void egueb_svg_element_fill_set(Egueb_Dom_Node *n, const Egueb_Svg_Paint *v)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	egueb_dom_attr_set(thiz->fill, EGUEB_DOM_ATTR_TYPE_BASE, v);
}

EAPI void egueb_svg_element_fill_get(Egueb_Dom_Node *n, Egueb_Svg_Paint_Animated *v)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->fill, v);
}

EAPI void egueb_svg_element_fill_opacity_set(Egueb_Dom_Node *n, const Egueb_Svg_Number *v)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	egueb_dom_attr_set(thiz->fill_opacity, EGUEB_DOM_ATTR_TYPE_BASE, v);
}

EAPI void egueb_svg_element_fill_opacity_get(Egueb_Dom_Node *n, Egueb_Svg_Number_Animated *v)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->fill_opacity, v);
}

EAPI void egueb_svg_element_font_size_set(Egueb_Dom_Node *n,
		const Egueb_Svg_Font_Size *v)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	egueb_dom_attr_set(thiz->font_size, EGUEB_DOM_ATTR_TYPE_BASE, v);
}

EAPI void egueb_svg_element_stroke_set(Egueb_Dom_Node *n, const Egueb_Svg_Paint *v)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	egueb_dom_attr_set(thiz->stroke, EGUEB_DOM_ATTR_TYPE_BASE, v);
}

EAPI void egueb_svg_element_stroke_get(Egueb_Dom_Node *n, Egueb_Svg_Paint_Animated *v)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->stroke, v);
}

EAPI void egueb_svg_element_stroke_width_set(Egueb_Dom_Node *n, const Egueb_Svg_Length *stroke_width)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	egueb_dom_attr_set(thiz->stroke_width, EGUEB_DOM_ATTR_TYPE_BASE, stroke_width);
}

EAPI void egueb_svg_element_stroke_width_get(Egueb_Dom_Node *n, Egueb_Svg_Length_Animated *v)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->stroke_width, v);
}

EAPI void egueb_svg_element_stroke_opacity_set(Egueb_Dom_Node *n, const Egueb_Svg_Number *v)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	egueb_dom_attr_set(thiz->stroke_opacity, EGUEB_DOM_ATTR_TYPE_BASE, v);
}

EAPI void egueb_svg_element_stroke_opacity_get(Egueb_Dom_Node *n, Egueb_Svg_Number_Animated *v)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->stroke_opacity, v);
}

