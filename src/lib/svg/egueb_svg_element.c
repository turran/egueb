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
#include "egueb_svg_element.h"
#include "egueb_svg_reference.h"
#include "egueb_svg_referenceable.h"
#include "egueb_svg_main.h"
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
#if 0
typedef struct _Egueb_Svg_Element_Attributes
{
	Egueb_Svg_Attribute_Animated_Clip_Path clip_path;
	Egueb_Svg_Attribute_Animated_Color color;
	Egueb_Svg_Attribute_Animated_Number opacity;
	Egueb_Svg_Attribute_Animated_Paint fill;
	Egueb_Svg_Attribute_Animated_Paint stroke;
	Egueb_Svg_Attribute_Animated_Length stroke_width;
	Egueb_Svg_Attribute_Animated_Number stroke_opacity;
	Egueb_Svg_Attribute_Animated_Number fill_opacity;
	Egueb_Svg_Attribute_Animated_Number stop_opacity;
	Egueb_Svg_Attribute_Animated_Color stop_color;
	Egueb_Svg_Attribute_Animated_Enum stroke_line_cap;
	Egueb_Svg_Attribute_Animated_Enum stroke_line_join;
	Egueb_Svg_Attribute_Animated_Enum fill_rule;
	Egueb_Svg_Attribute_Animated_Enum visibility;
	Egueb_Svg_Attribute_Animated_Enum display;
	/* FIXME do we really need this? */
	int sets;
	/* has something changed ? */
	Eina_Bool changed;
} Egueb_Svg_Element_Attributes;
#endif
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

static int * _egueb_svg_element_attribute_animated_fetch(Egueb_Svg_Element *thiz, const char *attr)
{
	int *animated = NULL;

	/* get our own attributes */
	if (strcmp(attr, "transform") == 0)
	{
		animated = &thiz->transform.animated;
	}
	/* common presentation attributes */
	else if (strcmp(attr, "clip-path") == 0)
	{
		animated = &thiz->current_attr->clip_path.animated;
	}
	else if (strcmp(attr, "opacity") == 0)
	{
		animated = &thiz->current_attr->opacity.animated;
	}
	else if (strcmp(attr, "color") == 0)
	{
		animated = &thiz->current_attr->color.animated;
	}
	else if (strcmp(attr, "fill") == 0)
	{
		animated = &thiz->current_attr->fill.animated;
	}
	else if (strcmp(attr, "fill-rule") == 0)
	{
		animated = &thiz->current_attr->fill_rule.animated;
	}
	else if (strcmp(attr, "fill-opacity") == 0)
	{
		animated = &thiz->current_attr->fill_opacity.animated;
	}
	else if (strcmp(attr, "stroke") == 0)
	{
		animated = &thiz->current_attr->stroke.animated;
	}
	else if (strcmp(attr, "stroke-width") == 0)
	{
		animated = &thiz->current_attr->stroke_width.animated;
	}
	else if (strcmp(attr, "stroke-opacity") == 0)
	{
		animated = &thiz->current_attr->stroke_opacity.animated;
	}
	else if (strcmp(attr, "stroke-linecap") == 0)
	{
		animated = &thiz->current_attr->stroke_line_cap.animated;
	}
	else if (strcmp(attr, "stroke-linejoin") == 0)
	{
		animated = &thiz->current_attr->stroke_line_join.animated;
	}
	else if (strcmp(attr, "stop-color") == 0)
	{
		animated = &thiz->current_attr->stop_color.animated;
	}
	else if (strcmp(attr, "stop-opacity") == 0)
	{
		animated = &thiz->current_attr->stop_opacity.animated;
	}
	else if (strcmp(attr, "visibility") == 0)
	{
		animated = &thiz->current_attr->visibility.animated;
	}
	else if (strcmp(attr, "display") == 0)
	{
		animated = &thiz->current_attr->display.animated;
	}
	return animated;
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
/*----------------------------------------------------------------------------*
 *                              Context helpers                               *
 *----------------------------------------------------------------------------*/
static void _egueb_svg_element_state_compose(Egueb_Svg_Element *thiz,
		const Egueb_Svg_Element_Context *parent, Egueb_Svg_Element_Context *d)
{
	Enesim_Matrix m;

	egueb_svg_attribute_animated_transform_final_get(&thiz->transform, &m);
	if (parent)
	{
		/* only set */
		d->dpi_x = parent->dpi_x;
		d->dpi_y = parent->dpi_y;
		d->viewbox = parent->viewbox;
		d->bounds = parent->bounds;
		d->font_size = parent->font_size;
		d->renderable_behaviour = parent->renderable_behaviour;
		/* actually compose */
		enesim_matrix_compose(&parent->transform, &m, &d->transform);
	}
	else
	{
		d->transform = m;
	}
}
/*----------------------------------------------------------------------------*
 *                            Attribute helpers                               *
 *----------------------------------------------------------------------------*/
/* TODO free every attribute that can be allocated, i.e strings */
static void _egueb_svg_element_attribute_presentation_free(Egueb_Svg_Element_Attributes *a)
{
	Egueb_Svg_Attribute_Paint *p;
	/* we should only free the base values */

	p = &a->fill.base;
	/* TODO make this a function */
	if (p->v.type == ESVG_PAINT_TYPE_SERVER)
	{
		if (p->v.uri)
		{
			free(p->v.uri);
		}
	}
	p = &a->stroke.base;
	if (p->v.type == ESVG_PAINT_TYPE_SERVER)
	{
		if (p->v.uri)
		{
			free(p->v.uri);
		}
	}
}

/* initialize the presentation attributes with default values */
static void _egueb_svg_element_attribute_presentation_setup(Egueb_Svg_Element_Attributes *a)
{
	Egueb_Svg_Color black = { 0, 0, 0 };
	Egueb_Svg_Length one = { ESVG_UNIT_LENGTH_PX, 1 };
	Egueb_Svg_Paint color = { ESVG_PAINT_TYPE_COLOR, black };
	Egueb_Svg_Paint none = { ESVG_PAINT_TYPE_NONE };
	Egueb_Svg_Number onen = { 1.0 };

	/* now the default values */
	egueb_svg_attribute_color_unset(&a->color.base, &black);
	egueb_svg_attribute_length_unset(&a->stroke_width.base, &one);
	egueb_svg_attribute_number_unset(&a->stroke_opacity.base, &onen);
	egueb_svg_attribute_number_unset(&a->fill_opacity.base, &onen);
	egueb_svg_attribute_number_unset(&a->opacity.base, &onen);
	egueb_svg_attribute_enum_unset(&a->fill_rule.base, ESVG_NON_ZERO);
	egueb_svg_attribute_paint_unset(&a->fill.base, &color);
	egueb_svg_attribute_paint_unset(&a->stroke.base, &none);
	egueb_svg_attribute_enum_unset(&a->stroke_line_cap.base, ESVG_STROKE_LINE_CAP_BUTT);
	egueb_svg_attribute_enum_unset(&a->stroke_line_join.base, ESVG_STROKE_LINE_JOIN_MITER);
	egueb_svg_attribute_number_unset(&a->stop_opacity.base, &onen);
	egueb_svg_attribute_enum_unset(&a->visibility.base, ESVG_VISIBILITY_VISIBLE);
	egueb_svg_attribute_enum_unset(&a->display.base, ESVG_DISPLAY_BLOCK);
}

static void _egueb_svg_element_attribute_presentation_merge(
	const Egueb_Svg_Element_Attributes *s,
	Egueb_Svg_Attribute_Presentation *d)
{
	/* clip path */
	egueb_svg_attribute_animated_clip_path_merge(&s->clip_path, &d->clip_path);
	/* color */
	egueb_svg_attribute_animated_color_merge(&s->color, &d->color);
	/* opacity */
	egueb_svg_attribute_animated_number_merge(&s->opacity, &d->opacity);
	/* FIXME do the real merge (multiply, etc, etc) */
	/* fill */
	egueb_svg_attribute_animated_paint_merge(&s->fill, &d->fill);
	/* fill opacity */
	egueb_svg_attribute_animated_number_merge(&s->fill_opacity, &d->fill_opacity);
	/* fill rule */
	egueb_svg_attribute_animated_enum_merge(&s->fill_rule, &d->fill_rule);
	/* stroke */
	egueb_svg_attribute_animated_paint_merge(&s->stroke, &d->stroke);
	/* stroke width */
	egueb_svg_attribute_animated_length_merge(&s->stroke_width, &d->stroke_width);
	/* stroke line cap */
	egueb_svg_attribute_animated_enum_merge(&s->stroke_line_cap, &d->stroke_line_cap);
	/* stroke line join */
	egueb_svg_attribute_animated_enum_merge(&s->stroke_line_join, &d->stroke_line_join);
	/* stroke opacity */
	egueb_svg_attribute_animated_number_merge(&s->stroke_opacity, &d->stroke_opacity);
	/* visibility */
	egueb_svg_attribute_animated_enum_merge(&s->visibility, &d->visibility);
	/* stop opacity */
	egueb_svg_attribute_animated_number_merge(&s->stop_opacity, &d->stop_opacity);
	/* stop color */
	egueb_svg_attribute_animated_color_merge(&s->stop_color, &d->stop_color);
	/* display */
	egueb_svg_attribute_animated_enum_merge(&s->display, &d->display);
}

static void _egueb_svg_element_attribute_presentation_merge_rel(
	const Egueb_Svg_Element_Attributes *rel,
	const Egueb_Svg_Element_Attributes *s,
	Egueb_Svg_Attribute_Presentation *d)
{
	/* clip path */
	egueb_svg_attribute_animated_clip_path_merge_rel(&rel->clip_path, &s->clip_path, &d->clip_path);
	/* color */
	egueb_svg_attribute_animated_color_merge_rel(&rel->color, &s->color, &d->color);
	/* opacity */
	egueb_svg_attribute_animated_number_merge_rel(&rel->opacity, &s->opacity, &d->opacity);
	/* FIXME do the real merge_rel (multiply, etc, etc) */
	/* fill */
	egueb_svg_attribute_animated_paint_merge_rel(&rel->fill, &s->fill, &d->fill);
	/* fill opacity */
	egueb_svg_attribute_animated_number_merge_rel(&rel->fill_opacity, &s->fill_opacity, &d->fill_opacity);
	/* fill rule */
	egueb_svg_attribute_animated_enum_merge_rel(&rel->fill_rule, &s->fill_rule, &d->fill_rule);
	/* stroke */
	egueb_svg_attribute_animated_paint_merge_rel(&rel->stroke, &s->stroke, &d->stroke);
	/* stroke width */
	egueb_svg_attribute_animated_length_merge_rel(&rel->stroke_width, &s->stroke_width, &d->stroke_width);
	/* stroke line cap */
	egueb_svg_attribute_animated_enum_merge_rel(&rel->stroke_line_cap, &s->stroke_line_cap, &d->stroke_line_cap);
	/* stroke line join */
	egueb_svg_attribute_animated_enum_merge_rel(&rel->stroke_line_join, &s->stroke_line_join, &d->stroke_line_join);
	/* stroke opacity */
	egueb_svg_attribute_animated_number_merge_rel(&rel->stroke_opacity, &s->stroke_opacity, &d->stroke_opacity);
	/* visibility */
	egueb_svg_attribute_animated_enum_merge_rel(&rel->visibility, &s->visibility, &d->visibility);
	/* stop opacity */
	egueb_svg_attribute_animated_number_merge_rel(&rel->stop_opacity, &s->stop_opacity, &d->stop_opacity);
	/* stop color */
	egueb_svg_attribute_animated_color_merge_rel(&rel->stop_color, &s->stop_color, &d->stop_color);
	/* display */
	egueb_svg_attribute_animated_enum_merge_rel(&rel->display, &s->display, &d->display);
}
/*----------------------------------------------------------------------------*
 *                               Setup helpers                                *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_element_child_setup_cb(Egueb_Dom_Tag *t, Egueb_Dom_Tag *child,
		void *data)
{
	Egueb_Svg_Element_Setup_Data *setup_data = data;
	Egueb_Svg_Element_Setup_Return ret;
	Eina_Bool r = EINA_TRUE;

	/* first filter it */
	if (setup_data->pre)
		r = setup_data->pre(t, child, setup_data->c, setup_data->error,
				setup_data->data);

	if (!r)
	{
		return EINA_TRUE;
	}

	/* the real setup */
	ret = egueb_svg_element_internal_setup(child, setup_data->c, setup_data->error);
	switch (ret)
	{
		case ESVG_SETUP_FAILED:
		setup_data->ret = EINA_FALSE;
		return EINA_FALSE;

		case ESVG_SETUP_OK:
		if (setup_data->post)
		{
			if (!setup_data->post(t, child, setup_data->c, setup_data->error,
					setup_data->data))
			{
				setup_data->ret = EINA_FALSE;
				return EINA_FALSE;
			}
		}
		break;

		default:
		break;
	}

	return EINA_TRUE;
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

/*----------------------------------------------------------------------------*
 *                           The Egueb_Dom Tag interface                           *
 *----------------------------------------------------------------------------*/
static const char * _egueb_svg_element_name_get(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(t);
	return egueb_svg_type_string_to(thiz->type);
}

static Egueb_Dom_Tag * _egueb_svg_element_tag_topmost_get(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(t);
	if (!thiz->topmost) return NULL;

	return ender_element_object_get(thiz->topmost);
}

static Eina_Bool _egueb_svg_element_child_add(Egueb_Dom_Tag *t, Egueb_Dom_Tag *child)
{
	Egueb_Svg_Element *thiz;
	Eina_Bool ret = EINA_FALSE;

	thiz = EGUEB_SVG_ELEMENT(t);
	if (!egueb_svg_is_element_internal(child))
	{
		ERR("Element '%s' can not add a non-esvg child %p",
				egueb_svg_type_string_to(egueb_svg_element_internal_type_get(t)),
				child);
		return EINA_FALSE;
	}

	if (thiz->descriptor.child_add)
		ret = thiz->descriptor.child_add(t, child);

	if (!ret)
	{
		ERR("Element '%s' can not add child '%s'",
				egueb_svg_type_string_to(egueb_svg_element_internal_type_get(t)),
				egueb_svg_type_string_to(egueb_svg_element_internal_type_get(child)));
	}
	return EINA_TRUE;
	/* FIXME once every element supports correctly the possible childs to add */
	//return ret;
}

static Eina_Bool _egueb_svg_element_child_remove(Egueb_Dom_Tag *t, Egueb_Dom_Tag *child)
{
	Egueb_Svg_Element *thiz;
	Eina_Bool ret = EINA_FALSE;

	thiz = EGUEB_SVG_ELEMENT(t);
	/* FIXME avoid this case, put it on the common element functions */
	if (!egueb_svg_is_element_internal(child))
	{
		ERR("Element '%s' has a non-esvg child %p?",
				egueb_svg_type_string_to(egueb_svg_element_internal_type_get(t)),
				child);
		return EINA_FALSE;
	}

	if (thiz->descriptor.child_remove)
		ret = thiz->descriptor.child_remove(t, child);

	if (!ret)
	{
		ERR("Element '%s' can not remove child '%s'",
				egueb_svg_type_string_to(egueb_svg_element_internal_type_get(t)),
				egueb_svg_type_string_to(egueb_svg_element_internal_type_get(child)));
	}

	return ret;
}

static Eina_Bool _egueb_svg_element_attribute_set(Egueb_Dom_Tag *t, const char *key, const char *value)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(t);
	DBG("Setting attribute '%s' with value '%s' on element '%s'",
		key, value, egueb_svg_type_string_to(egueb_svg_element_internal_type_get(t)));
	if (thiz->descriptor.attribute_set)
	{
		if (thiz->descriptor.attribute_set(thiz->e, key, value))
			return EINA_TRUE;
	}
	if (strcmp(key, "id") == 0)
	{
		egueb_svg_element_id_set(thiz->e, value);
	}
	else if (strcmp(key, "class") == 0)
	{
		egueb_svg_element_class_set(thiz->e, value);
	}
	else if (strcmp(key, "transform") == 0)
	{
		Enesim_Matrix matrix;

		egueb_svg_transformation_string_from(&matrix, value);
		egueb_svg_element_transform_set(thiz->e, &matrix);
	}
	else if (strcmp(key, "style") == 0)
	{
		egueb_svg_element_style_set(thiz->e, value);
	}
	/* common presentation attributes */
	else if (strcmp(key, "clip-path") == 0)
	{
		Egueb_Svg_Clip_Path clip_path;

		egueb_svg_clip_path_string_from(&clip_path, value);
		egueb_svg_element_clip_path_set(thiz->e, &clip_path);
	}
	else if (strcmp(key, "opacity") == 0)
	{
		Egueb_Svg_Number opacity;
		egueb_svg_number_string_from(&opacity, value, 1.0);
		egueb_svg_element_opacity_set(thiz->e, &opacity);
	}
	else if (strcmp(key, "color") == 0)
	{
		Egueb_Svg_Color color;

		egueb_svg_color_string_from(&color, value);
		egueb_svg_element_color_set(thiz->e, &color);
	}
	else if (strcmp(key, "fill") == 0)
	{
		Egueb_Svg_Paint fill;

		if (egueb_svg_paint_string_from(&fill, value))
			egueb_svg_element_fill_set(thiz->e, &fill);
	}
	else if (strcmp(key, "fill-rule") == 0)
	{
		Egueb_Svg_Fill_Rule fill_rule;

		egueb_svg_parser_fill_rule_get(&fill_rule, value);
		egueb_svg_element_fill_rule_set(thiz->e, fill_rule);
	}
	else if (strcmp(key, "fill-opacity") == 0)
	{
		Egueb_Svg_Number fill_opacity;
		egueb_svg_number_string_from(&fill_opacity, value, 1.0);
		egueb_svg_element_fill_opacity_set(thiz->e, &fill_opacity);
	}
	else if (strcmp(key, "stroke") == 0)
	{
		Egueb_Svg_Paint stroke;

		if (egueb_svg_paint_string_from(&stroke, value))
			egueb_svg_element_stroke_set(thiz->e, &stroke);
	}
	else if (strcmp(key, "stroke-width") == 0)
	{
		Egueb_Svg_Length stroke_width;

		egueb_svg_length_string_from(&stroke_width, value);
		egueb_svg_element_stroke_width_set(thiz->e, &stroke_width);
	}
	else if (strcmp(key, "stroke-opacity") == 0)
	{
		Egueb_Svg_Number stroke_opacity;

		egueb_svg_number_string_from(&stroke_opacity, value, 1.0);
		egueb_svg_element_stroke_opacity_set(thiz->e, &stroke_opacity);
	}
	else if (strcmp(key, "stroke-linecap") == 0)
	{
		Egueb_Svg_Stroke_Line_Cap stroke_line_cap;

		stroke_line_cap = egueb_svg_stroke_line_cap_string_from(value);
		egueb_svg_element_stroke_line_cap_set(thiz->e, stroke_line_cap);
	}
	else if (strcmp(key, "stroke-linejoin") == 0)
	{
		Egueb_Svg_Stroke_Line_Join stroke_line_join;

		stroke_line_join = egueb_svg_stroke_line_join_string_from(value);
		egueb_svg_element_stroke_line_join_set(thiz->e, stroke_line_join);
	}
	else if (strcmp(key, "stop-color") == 0)
	{
		Egueb_Svg_Color color;

		egueb_svg_color_string_from(&color, value);
		egueb_svg_element_stop_color_set(thiz->e, &color);
	}
	else if (strcmp(key, "stop-opacity") == 0)
	{
		Egueb_Svg_Number stop_opacity;

		egueb_svg_number_string_from(&stop_opacity, value, 1.0);
		egueb_svg_element_stop_opacity_set(thiz->e, &stop_opacity);
	}
	else if (strcmp(key, "visibility") == 0)
	{
		Egueb_Svg_Visibility visibility;
		egueb_svg_visibility_string_from(&visibility, value);
		egueb_svg_element_visibility_set(thiz->e, visibility);
	}
	else if (strcmp(key, "display") == 0)
	{
		Egueb_Svg_Display display;
		egueb_svg_display_string_from(&display, value);
		egueb_svg_element_display_set(thiz->e, display);
	}
	else if (!strcmp(key, "onfocusin"))
	{
		egueb_svg_element_onfocusin_set(thiz->e, value);
	}
	else if (!strcmp(key, "onfocusout"))
	{
		egueb_svg_element_onfocusout_set(thiz->e, value);
	}
	else if (!strcmp(key, "onactivate"))
	{
		egueb_svg_element_onactivate_set(thiz->e, value);
	}
	else if (!strcmp(key, "onclick"))
	{
		egueb_svg_element_onclick_set(thiz->e, value);
	}
	else if (!strcmp(key, "onmousedown"))
	{
		egueb_svg_element_onmousedown_set(thiz->e, value);
	}
	else if (!strcmp(key, "onmouseup"))
	{
		egueb_svg_element_onmouseup_set(thiz->e, value);
	}
	else if (!strcmp(key, "onmouseover"))
	{
		egueb_svg_element_onmouseover_set(thiz->e, value);
	}
	else if (!strcmp(key, "onmousemove"))
	{
		egueb_svg_element_onmousemove_set(thiz->e, value);
	}
	else if (!strcmp(key, "onmouseout"))
	{
		egueb_svg_element_onmouseout_set(thiz->e, value);
	}
	/* TODO in theory we should not allow css attributes to continue */
	else
	{
		ERR("Unsupported attribute set '%s' with value '%s' on element '%s'",
				key, value,
				egueb_svg_type_string_to(egueb_svg_element_internal_type_get(t)));
		return EINA_FALSE;
	}

	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_element_attribute_get(Egueb_Dom_Tag *t, const char *key, char **value)
{
	Egueb_Svg_Element *thiz;
	Eina_Bool ret;

	thiz = EGUEB_SVG_ELEMENT(t);
	DBG("Getting attribute '%s' on element '%s'",
		key, egueb_svg_type_string_to(egueb_svg_element_internal_type_get(t)));
	/* we need to first call the implemenation as there might be name collisions */
	if (thiz->descriptor.attribute_get)
	{
			if (thiz->descriptor.attribute_get(t, key, value))
				return EINA_TRUE;
	}

	/* FIXME handle common properties */
	if (strcmp(key, "id") == 0)
	{
          _egueb_svg_element_id_get(t, (const char **)value);
	}
	else if (strcmp(key, "class") == 0)
	{
		_egueb_svg_element_class_get(t, (const char **)value);
	}
#if 0
	/* TODO all the below cases should get the attribute
	 * then, call a function to convert it to string
	 * and return such value
	 */
	else if (strcmp(key, "transform") == 0)
	{
		Enesim_Matrix matrix;

		_egueb_svg_element_transform_get(thiz->e, &matrix);
		egueb_svg_transformation_string_from(&matrix, value);
	}
	else if (strcmp(key, "style") == 0)
	{
		_egueb_svg_element_style_get(thiz->e, value);
	}
	/* common presentation attributes */
	else if (strcmp(key, "clip-path") == 0)
	{
		_egueb_svg_element_clip_path_get(thiz->e, value);
	}
	else if (strcmp(key, "opacity") == 0)
	{
		double opacity;

		_egueb_svg_element_opacity_get(thiz->e, opacity);
		 value = egueb_svg_number_string_from(value, 1.0);
	}
	else if (strcmp(key, "color") == 0)
	{
		Egueb_Svg_Color color;

		egueb_svg_color_string_from(&color, value);
		_egueb_svg_element_color_get(thiz->e, &color);
	}
	else if (strcmp(key, "fill") == 0)
	{
		Egueb_Svg_Paint fill;

		if (egueb_svg_paint_string_from(&fill, value))
			_egueb_svg_element_fill_get(thiz->e, &fill);
	}
	else if (strcmp(key, "fill-rule") == 0)
	{
		Egueb_Svg_Fill_Rule fill_rule;

		egueb_svg_parser_fill_rule_get(&fill_rule, value);
		_egueb_svg_element_fill_rule_get(thiz->e, fill_rule);
	}
	else if (strcmp(key, "fill-opacity") == 0)
	{
		double fill_opacity = egueb_svg_number_string_from(value, 1.0);
		_egueb_svg_element_fill_opacity_get(thiz->e, fill_opacity);
	}
	else if (strcmp(key, "stroke") == 0)
	{
		Egueb_Svg_Paint stroke;

		if (egueb_svg_paint_string_from(&stroke, value))
			_egueb_svg_element_stroke_get(thiz->e, &stroke);
	}
	else if (strcmp(key, "stroke-width") == 0)
	{
		Egueb_Svg_Length stroke_width;

		egueb_svg_length_string_from(&stroke_width, value, ESVG_LENGTH_1);
		_egueb_svg_element_stroke_width_get(thiz->e, &stroke_width);
	}
	else if (strcmp(key, "stroke-opacity") == 0)
	{
		double stroke_opacity = egueb_svg_number_string_from(value, 1.0);
		_egueb_svg_element_stroke_opacity_get(thiz->e, stroke_opacity);
	}
	else if (strcmp(key, "stroke-linecap") == 0)
	{
		Egueb_Svg_Stroke_Line_Cap stroke_line_cap;

		stroke_line_cap = egueb_svg_stroke_line_cap_string_from(value);
		_egueb_svg_element_stroke_line_cap_get(thiz->e, stroke_line_cap);
	}
	else if (strcmp(key, "stroke-linejoin") == 0)
	{
		Egueb_Svg_Stroke_Line_Join stroke_line_join;

		stroke_line_join = egueb_svg_stroke_line_join_string_from(value);
		_egueb_svg_element_stroke_line_join_get(thiz->e, stroke_line_join);
	}
	else if (strcmp(key, "stop-color") == 0)
	{
		Egueb_Svg_Color color;

		egueb_svg_color_string_from(&color, value);
		_egueb_svg_element_stop_color_get(thiz->e, &color);
	}
	else if (strcmp(key, "stop-opacity") == 0)
	{
		double stop_opacity = egueb_svg_number_string_from(value, 1.0);
		_egueb_svg_element_stop_opacity_get(thiz->e, stop_opacity);
	}
#endif
	else
	{
		ERR("Unsupported attribute get '%s'", key);
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

static void _egueb_svg_element_free(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(t);
	/* remove every child object */
	egueb_dom_tag_child_foreach(t, _egueb_svg_element_child_free_cb, NULL);
	_egueb_svg_element_attribute_presentation_free(&thiz->attr_xml);
	_egueb_svg_element_attribute_presentation_free(&thiz->attr_css);
	if (thiz->descriptor.free)
		thiz->descriptor.free(t);
	if (thiz->id)
		free(thiz->id);
	if (thiz->class)
		free(thiz->class);
	if (thiz->style)
		free(thiz->style);

	free(thiz);
}

/*----------------------------------------------------------------------------*
 *                          Css context interface                             *
 *----------------------------------------------------------------------------*/
static const char * _egueb_svg_element_css_property_get(void *e, const char *property)
{
	Egueb_Dom_Tag *tag = e;
	char *value;

	_egueb_svg_element_attribute_get(tag, property, &value);

	return value;
}

/* FIXME we could call directly the function _attribute_set */
static void _egueb_svg_element_css_property_set(void *e, const char *property, const char *value)
{
	Egueb_Dom_Tag *tag = e;
	_egueb_svg_element_attribute_set(tag, property, value);
}

static void * _egueb_svg_element_css_get_child(void *e)
{
	Egueb_Dom_Tag *tag = e;
	return egueb_dom_tag_child_get(tag);
}

static void * _egueb_svg_element_css_get_next_sibling(void *e)
{
	Egueb_Dom_Tag *tag = e;
	return egueb_dom_tag_next_get(tag);
}

static const char * _egueb_svg_element_css_get_name(void *e)
{
	Egueb_Dom_Tag *t = e;
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(t);
	return egueb_svg_type_string_to(thiz->type);
}

static Ecss_Context _egueb_svg_element_css_context = {
	/* .property_set 	= */ _egueb_svg_element_css_property_set,
	/* .property_get 	= */ _egueb_svg_element_css_property_get,
	/* .get_name 		= */ _egueb_svg_element_css_get_name,
	/* .get_child 		= */ _egueb_svg_element_css_get_child,
	/* .get_next_sibling 	= */ _egueb_svg_element_css_get_next_sibling,
};
#endif

static void _egueb_svg_element_presentation_attributes_process(
		Egueb_Svg_Element *thiz, Egueb_Dom_Node *relative)
{
	Egueb_Svg_Element *rel;

	if (!relative) return;
	if (!enesim_object_instance_inherits(
				ENESIM_OBJECT_INSTANCE(relative),
				EGUEB_SVG_ELEMENT_DESCRIPTOR))
	{
		WARN("Parent does not inherit from the SVGElement interface");
		return;
	}
	rel = EGUEB_SVG_ELEMENT(relative);
	/* TODO in case we have a parent and neither the parent
	 * or ourselves have changed an inheritable property
	 * just continue without generating again the final
	 * of state of such properties
	 */
	egueb_dom_attr_inheritable_process(thiz->color, rel->color);
	egueb_dom_attr_inheritable_process(thiz->fill, rel->fill);
	egueb_dom_attr_inheritable_process(thiz->fill_opacity, rel->fill_opacity);
	egueb_dom_attr_inheritable_process(thiz->opacity, rel->opacity);
	egueb_dom_attr_inheritable_process(thiz->stop_opacity, rel->stop_opacity);
	egueb_dom_attr_inheritable_process(thiz->stroke, rel->stroke);
	egueb_dom_attr_inheritable_process(thiz->stroke_line_cap, rel->stroke_line_cap);
	egueb_dom_attr_inheritable_process(thiz->stroke_line_join, rel->stroke_line_join);
	egueb_dom_attr_inheritable_process(thiz->stroke_width, rel->stroke_width);
	egueb_dom_attr_inheritable_process(thiz->stroke_opacity, rel->stroke_opacity);
	egueb_dom_attr_inheritable_process(thiz->visibility, rel->visibility);
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
	Eina_Bool ret = EINA_TRUE;

	thiz = EGUEB_SVG_ELEMENT(e);
	DBG("Processing element");
	//DBG("Doing the setup on the tag '%s'", egueb_dom_element_tag_name_get(e));

	/* in case a stylesheet has been applied, re-apply the style atribute */
	/* else in case the style attribute has changed, revert the style and apply
	 * the new style
	 */
	egueb_dom_attr_get(thiz->style, EGUEB_DOM_ATTR_TYPE_BASE,
			&style);
	if (thiz->last_style != style)
	{
		/* revert the style */
		/* apply the new style */
		/* swap the styles */
	}
	if (style) egueb_dom_string_unref(style);

	egueb_svg_element_presentation_relative_get(EGUEB_DOM_NODE(e), &relative);
	/* propagate the inheritable attributes (the presentation attributes) */
	_egueb_svg_element_presentation_attributes_process(thiz, relative);

	/* now call the element interface for processing this element */
	klass = EGUEB_SVG_ELEMENT_CLASS_GET(thiz);
	if (klass->process) ret = klass->process(thiz);
	if (relative) egueb_dom_node_unref(relative);

	return ret;
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element, Egueb_Svg_Element, id);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element, Egueb_Svg_Element, klass);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element, Egueb_Svg_Element, style);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element, Egueb_Svg_Element, clip_path);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element, Egueb_Svg_Element, color);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element, Egueb_Svg_Element, fill);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element, Egueb_Svg_Element, fill_opacity);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element, Egueb_Svg_Element, font_size);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element, Egueb_Svg_Element, opacity);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element, Egueb_Svg_Element, stop_color);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element, Egueb_Svg_Element, stop_opacity);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element, Egueb_Svg_Element, stroke);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element, Egueb_Svg_Element, stroke_line_join);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element, Egueb_Svg_Element, stroke_line_cap);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element, Egueb_Svg_Element, stroke_width);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element, Egueb_Svg_Element, stroke_opacity);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element, Egueb_Svg_Element, visibility);

ENESIM_OBJECT_ABSTRACT_BOILERPLATE(EGUEB_DOM_ELEMENT_DESCRIPTOR,
		Egueb_Svg_Element, Egueb_Svg_Element_Class, egueb_svg_element);

static void _egueb_svg_element_class_init(void *k)
{
	Egueb_Dom_Element_Class *klass;

	klass = EGUEB_DOM_ELEMENT_CLASS(k);
	klass->process = _egueb_svg_element_process;
}

static void _egueb_svg_element_class_deinit(void *k)
{
}

static void _egueb_svg_element_instance_init(void *o)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(o);
	/* create the properties */
	thiz->id = egueb_dom_attr_string_new(
			egueb_dom_string_ref(EGUEB_SVG_ID), NULL);
	thiz->klass = egueb_dom_attr_string_new(
			egueb_dom_string_ref(EGUEB_SVG_CLASS), NULL);
	thiz->style = egueb_dom_attr_string_new(
			egueb_dom_string_ref(EGUEB_SVG_STYLE), NULL);
	/* the presentation attributes */
	thiz->clip_path = egueb_svg_attr_clip_path_new(
			egueb_dom_string_ref(EGUEB_SVG_CLIP_PATH),
			&EGUEB_SVG_CLIP_PATH_NONE, EINA_TRUE, EINA_TRUE,
			EINA_TRUE);
	thiz->color = egueb_svg_attr_color_new(
			egueb_dom_string_ref(EGUEB_SVG_COLOR),
			&EGUEB_SVG_COLOR_BLACK, EINA_TRUE, EINA_TRUE,
			EINA_TRUE);
	thiz->fill = egueb_svg_attr_paint_new(
			egueb_dom_string_ref(EGUEB_SVG_FILL),
			&EGUEB_SVG_PAINT_BLACK,
			EINA_TRUE, EINA_TRUE, EINA_TRUE);
	thiz->fill_opacity = egueb_svg_attr_number_new(
			egueb_dom_string_ref(EGUEB_SVG_FILL_OPACITY),
			1, EINA_TRUE, EINA_TRUE,
			EINA_TRUE);
	thiz->font_size = egueb_svg_attr_font_size_new(
			egueb_dom_string_ref(EGUEB_SVG_FONT_SIZE),
			&EGUEB_SVG_FONT_SIZE_MEDIUM, EINA_TRUE, EINA_TRUE, EINA_TRUE);
	thiz->opacity = egueb_svg_attr_number_new(
			egueb_dom_string_ref(EGUEB_SVG_OPACITY),
			1, EINA_TRUE, EINA_TRUE,
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
	thiz->visibility = egueb_svg_attr_visibility_new(
			egueb_dom_string_ref(EGUEB_SVG_VISIBILITY),
			EGUEB_SVG_VISIBILITY_VISIBLE, EINA_TRUE, EINA_TRUE,
			EINA_TRUE);

	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element, id);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element, klass);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element, style);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element, clip_path);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element, color);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element, fill);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element, fill_opacity);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element, font_size);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element, opacity);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element, stop_opacity);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element, stop_color);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element, stroke);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element, stroke_line_join);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element, stroke_line_cap);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element, stroke_width);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element, stroke_opacity);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element, visibility);
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
	egueb_dom_node_unref(thiz->fill);
	egueb_dom_node_unref(thiz->fill_opacity);
	egueb_dom_node_unref(thiz->font_size);
	egueb_dom_node_unref(thiz->opacity);
	egueb_dom_node_unref(thiz->stop_color);
	egueb_dom_node_unref(thiz->stop_opacity);
	egueb_dom_node_unref(thiz->stroke);
	egueb_dom_node_unref(thiz->stroke_width);
	egueb_dom_node_unref(thiz->stroke_opacity);
	egueb_dom_node_unref(thiz->stroke_line_cap);
	egueb_dom_node_unref(thiz->stroke_line_join);
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
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
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

			egueb_dom_node_document_get(n, &doc);
			if (!doc)
			{
				WARN("No document set");
				return;
			}
			egueb_svg_document_element_get_by_iri(doc, &iri, &ref);
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
		egueb_svg_clip_path_copy(clip_path_current, clip_path_last);
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
			ecss_context_inline_style_apply(&_egueb_svg_element_css_context, thiz->style, t);
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

void egueb_svg_element_ecss_style_apply(Egueb_Dom_Tag *t, Ecss_Style *s)
{
	egueb_svg_element_attribute_type_set(t, ESVG_ATTR_CSS);
	ecss_context_style_apply(&_egueb_svg_element_css_context, s, t);
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
#if 0
void egueb_svg_element_context_dump(const Egueb_Svg_Element_Context *c)
{
	DBG("dpi %g %g", c->dpi_x, c->dpi_y);
	DBG("viewbox %g %g %g %g", c->viewbox.x, c->viewbox.y, c->viewbox.w, c->viewbox.h);
	DBG("transformation %" ENESIM_MATRIX_FORMAT, ENESIM_MATRIX_ARGS (&c->transform));
}
#endif
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

EAPI Eina_Error egueb_svg_element_geometry_relative_get(Egueb_Dom_Node *n,
		Egueb_Dom_Node **geometry_relative)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	if (thiz->geometry_relative)
	{
		*geometry_relative = egueb_dom_node_ref(
				thiz->geometry_relative);
		return EINA_ERROR_NONE;
	}
	else
	{
		return egueb_dom_node_parent_get(n, geometry_relative);
	}
}

EAPI Eina_Error egueb_svg_element_geometry_relative_set(Egueb_Dom_Node *n,
		Egueb_Dom_Node *geometry_relative)
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
			return EGUEB_DOM_ERROR_NOT_SUPPORTED;
		egueb_dom_node_weak_ref_add(geometry_relative,
			&thiz->geometry_relative);
	}
	return EINA_ERROR_NONE;
}


EAPI Eina_Error egueb_svg_element_presentation_relative_get(Egueb_Dom_Node *n,
		Egueb_Dom_Node **presentation_relative)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	if (thiz->presentation_relative)
	{
		*presentation_relative = egueb_dom_node_ref(
				thiz->presentation_relative);
		return EINA_ERROR_NONE;
	}
	else
	{
		return egueb_dom_node_parent_get(n, presentation_relative);
	}
}

EAPI Eina_Error egueb_svg_element_presentation_relative_set(Egueb_Dom_Node *n,
		Egueb_Dom_Node *presentation_relative)
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
			return EGUEB_DOM_ERROR_NOT_SUPPORTED;
		egueb_dom_node_weak_ref_add(presentation_relative,
				&thiz->presentation_relative);
	}
	return EINA_ERROR_NONE;
}

EAPI void egueb_svg_element_clip_path_final_get(Egueb_Dom_Node *n,
		Egueb_Svg_Clip_Path *clip_path)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	egueb_dom_attr_final_get(thiz->clip_path, clip_path);
}

#if 0

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI const char * egueb_svg_element_name_get(Ender_Element *e)
{
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	return egueb_dom_tag_name_get(t);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Element * egueb_svg_element_topmost_get(Ender_Element *e)
{
	Egueb_Dom_Tag *t;
	Ender_Element *topmost;

	t = ender_element_object_get(e);
	egueb_svg_element_internal_topmost_get(t, &topmost);
	return topmost;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool egueb_svg_element_style_is_set(Egueb_Dom_Node *n)
{
	return EINA_FALSE;
}

#endif

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
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_clip_path_unset(Egueb_Dom_Node *n)
{
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
EAPI void egueb_svg_element_color_set(Egueb_Dom_Node *n, const Egueb_Svg_Color *color)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_color_get(Egueb_Dom_Node *n, Egueb_Svg_Color *color)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_color_unset(Egueb_Dom_Node *n)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_fill_set(Egueb_Dom_Node *n, const Egueb_Svg_Paint *fill)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_fill_unset(Egueb_Dom_Node *n)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_fill_opacity_set(Egueb_Dom_Node *n, const Egueb_Svg_Number *v)
{
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

EAPI Eina_Error egueb_svg_element_font_size_set(Egueb_Dom_Node *n,
		const Egueb_Svg_Font_Size *font_size)
{
	Egueb_Svg_Element *thiz;

	thiz = EGUEB_SVG_ELEMENT(n);
	return egueb_dom_attr_set(thiz->font_size, EGUEB_DOM_ATTR_TYPE_BASE,
			font_size);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_stroke_set(Egueb_Dom_Node *n, const Egueb_Svg_Paint *stroke)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_stroke_unset(Egueb_Dom_Node *n)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_stroke_width_set(Egueb_Dom_Node *n, const Egueb_Svg_Length *stroke_width)
{
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
