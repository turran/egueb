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
#include "esvg_private_main.h"
#include "esvg_private_attribute_presentation.h"
#include "esvg_private_context.h"
#include "esvg_private_element.h"
#include "esvg_private_svg.h"

#include "esvg_main.h"
#include "esvg_element.h"
#include "esvg_script_provider.h"
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
 *  the element "animation" state  (esvg_element_attribute_animate_set)
 *  so the anim value is the one that gets updated
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT _esvg_element_log

static int _esvg_element_log = -1;

#define ESVG_ELEMENT_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_ELEMENT_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_ELEMENT_MAGIC);\
	} while(0)

static Ender_Property *ESVG_ELEMENT_TYPE;
static Ender_Property *ESVG_ELEMENT_ID;
static Ender_Property *ESVG_ELEMENT_CLASS;
static Ender_Property *ESVG_ELEMENT_TRANSFORM;
static Ender_Property *ESVG_ELEMENT_STYLE;
static Ender_Property *ESVG_ELEMENT_CLIP_PATH;
static Ender_Property *ESVG_ELEMENT_OPACITY;
static Ender_Property *ESVG_ELEMENT_COLOR;
static Ender_Property *ESVG_ELEMENT_FILL;
static Ender_Property *ESVG_ELEMENT_FILL_OPACITY;
static Ender_Property *ESVG_ELEMENT_STROKE;
static Ender_Property *ESVG_ELEMENT_STROKE_OPACITY;
static Ender_Property *ESVG_ELEMENT_STROKE_WIDTH;
static Ender_Property *ESVG_ELEMENT_VISIBILITY;
static Ender_Property *ESVG_ELEMENT_STOP_COLOR;
static Ender_Property *ESVG_ELEMENT_STOP_OPACITY;
static Ender_Property *ESVG_ELEMENT_DISPLAY;
static Ender_Property *ESVG_ELEMENT_TOPMOST;

static Ender_Property *ESVG_ELEMENT_ONMOUSEOVER;
static Ender_Property *ESVG_ELEMENT_ONFOCUSIN;
static Ender_Property *ESVG_ELEMENT_ONFOCUSOUT;
static Ender_Property *ESVG_ELEMENT_ONACTIVATE;
static Ender_Property *ESVG_ELEMENT_ONCLICK;
static Ender_Property *ESVG_ELEMENT_ONMOUSEDOWN;
static Ender_Property *ESVG_ELEMENT_ONMOUSEUP;
static Ender_Property *ESVG_ELEMENT_ONMOUSEOVER;
static Ender_Property *ESVG_ELEMENT_ONMOUSEMOVE;
static Ender_Property *ESVG_ELEMENT_ONMOUSEOUT;

typedef struct _Esvg_Element_Descriptor_Internal
{
	Esvg_Element_Initialize initialize;
	Esvg_Element_Attribute_Set attribute_set;
	Esvg_Element_Attribute_Animated_Fetch attribute_animated_fetch;
	Edom_Tag_Attribute_Get attribute_get;
	Edom_Tag_Free free;
	Edom_Tag_Child_Add child_add;
	Edom_Tag_Child_Remove child_remove;
	Esvg_Element_Setup setup;
} Esvg_Element_Descriptor_Internal;

typedef struct _Esvg_Element_Setup_Data
{
	Esvg_Context *c;
	Enesim_Error **error;
	Esvg_Element_Setup_Interceptor pre;
	Esvg_Element_Setup_Interceptor post;
	Eina_Bool ret;
	void *data;
} Esvg_Element_Setup_Data;

typedef struct _Esvg_Element_Attributes
{
	Esvg_Attribute_Animated_Clip_Path clip_path;
	Esvg_Attribute_Animated_Color color;
	Esvg_Attribute_Animated_Number opacity;
	Esvg_Attribute_Animated_Paint fill;
	Esvg_Attribute_Animated_Paint stroke;
	Esvg_Attribute_Animated_Length stroke_width;
	Esvg_Attribute_Animated_Number stroke_opacity;
	Esvg_Attribute_Animated_Number fill_opacity;
	Esvg_Attribute_Animated_Number stop_opacity;
	Esvg_Attribute_Animated_Color stop_color;
	Esvg_Attribute_Animated_Enum stroke_line_cap;
	Esvg_Attribute_Animated_Enum stroke_line_join;
	Esvg_Attribute_Animated_Enum fill_rule;
	Esvg_Attribute_Animated_Enum visibility;
	Esvg_Attribute_Animated_Enum display;
	/* FIXME do we really need this? */
	int sets;
	/* has something changed ? */
	Eina_Bool changed;
} Esvg_Element_Attributes;

typedef struct _Esvg_Element
{
	EINA_MAGIC
	Esvg_Type type;
	/* properties */
	char *style;
	Eina_Bool style_changed;
	char *id;
	char *class;
	Ender_Element *topmost;
	/* FIXME this should not be part of the common element interface
	 * but only on the specific elements that support a transformation
	 * even so, there seems to be some initiatives to make the
	 * transform a presentation attribute
	 */
	Esvg_Attribute_Animated_Transform transform;
	Esvg_Element_Attributes attr_xml;
	Esvg_Element_Attributes attr_css;
	char *onfocusin;
	char *onfocusout;
	char *onactivate;
	char *onclick;
	char *onmousedown;
	char *onmouseup;
	char *onmouseover;
	char *onmousemove;
	char *onmouseout;
	/* the descriptor interface */
	Esvg_Element_Descriptor_Internal descriptor;
	/* private */
	Esvg_Attribute_Type current_attr_type;
	Eina_Bool current_attr_animate;
	Esvg_Element_Attributes *current_attr;
	Esvg_Attribute_Presentation attr_final;
	Esvg_Element_Context state_final;
	/* identifier of the last time an element has done the setup */
	int last_run;
	/* flag set whenever some property has changed */
	/* FIXME remove this */
	int changed;
	/* the ender element associated with this element */
	Ender_Element *e;
	/* private data used for the element implementations */
	void *data;
} Esvg_Element;

static Esvg_Element * _esvg_element_get(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = edom_tag_data_get(t);
	ESVG_ELEMENT_MAGIC_CHECK(thiz);

	return thiz;
}

/*----------------------------------------------------------------------------*
 *                              Event handlers                                *
 *----------------------------------------------------------------------------*/
static void _esvg_element_event_handler(Esvg_Element *thiz, const char *script,
		Ender_Value *v)
{
	Esvg_Script_Provider *provider;
	const char *content_script_type;

	if (!script) return;
	if (!thiz->topmost) return;

	esvg_svg_content_script_type_get(thiz->topmost, &content_script_type);
	provider = esvg_svg_script_provider_get(thiz->topmost, content_script_type);
	if (!provider) return;
	/* call the script with the passed in arg */
	printf("calling with %s\n", script);
	esvg_script_provider_run(provider, script, v);
}

static void _esvg_element_onfocusin_call(Ender_Element *e,
		const char *event_name, void *event_data, void *data)
{
	Esvg_Element *thiz = data;
	_esvg_element_event_handler(thiz, thiz->onfocusin, NULL);
}

static void _esvg_element_onfocusout_call(Ender_Element *e,
		const char *event_name, void *event_data, void *data)
{
	Esvg_Element *thiz = data;
	_esvg_element_event_handler(thiz, thiz->onfocusout, NULL);
}

static void _esvg_element_onactivate_call(Ender_Element *e,
		const char *event_name, void *event_data, void *data)
{
	Esvg_Element *thiz = data;
	_esvg_element_event_handler(thiz, thiz->onactivate, NULL);
}

static void _esvg_element_mouse_event_call(Esvg_Element *thiz,
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
	
	_esvg_element_event_handler(thiz, script, v);
	ender_value_unref(v);
}

static void _esvg_element_onclick_call(Ender_Element *e,
		const char *event_name, void *event_data, void *data)
{
	Esvg_Element *thiz = data;
	_esvg_element_mouse_event_call(thiz, event_data, thiz->onclick);
}

static void _esvg_element_onmousedown_call(Ender_Element *e,
		const char *event_name, void *event_data, void *data)
{
	Esvg_Element *thiz = data;
	_esvg_element_mouse_event_call(thiz, event_data, thiz->onmousedown);
}

static void _esvg_element_onmouseup_call(Ender_Element *e,
		const char *event_name, void *event_data, void *data)
{
	Esvg_Element *thiz = data;
	_esvg_element_mouse_event_call(thiz, event_data, thiz->onmouseup);
}

static void _esvg_element_onmouseover_call(Ender_Element *e,
		const char *event_name, void *event_data, void *data)
{
	Esvg_Element *thiz = data;
	_esvg_element_mouse_event_call(thiz, event_data, thiz->onmouseover);
}

static void _esvg_element_onmousemove_call(Ender_Element *e,
		const char *event_name, void *event_data, void *data)
{
	Esvg_Element *thiz = data;
	_esvg_element_mouse_event_call(thiz, event_data, thiz->onmousemove);
}

static void _esvg_element_onmouseout_call(Ender_Element *e,
		const char *event_name, void *event_data, void *data)
{
	Esvg_Element *thiz = data;
	_esvg_element_mouse_event_call(thiz, event_data, thiz->onmouseout);
}

/* called on every child of an element, whenever the element is being freed */
static Eina_Bool _esvg_element_child_free_cb(Edom_Tag *t, Edom_Tag *child,
		void *data)
{
	Ender_Element *t_e;
	Ender_Element *child_e;

	t_e = esvg_element_ender_get(t);
	child_e = esvg_element_ender_get(child);
	ender_element_property_value_remove(t_e, EDOM_CHILD, child, NULL);
	ender_element_unref(child_e);

	return EINA_TRUE;
}

static void _esvg_element_mutation_cb(Ender_Element *e, const char *event_name,
		void *event_data, void *data)
{
	Esvg_Element *thiz = data;
	Ender_Event_Mutation *ev = event_data;

	/* FIXME we could check if the mutation is a remove, add, etc */
	thiz->changed++;
}

static int * _esvg_element_attribute_animated_fetch(Esvg_Element *thiz, const char *attr)
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

static int * _esvg_element_attribute_animated_get(Edom_Tag *t, const char *attr)
{
	Esvg_Element *thiz;
	int *animated;

	thiz = _esvg_element_get(t);
	/* get our own attributes */
	animated = _esvg_element_attribute_animated_fetch(thiz, attr);
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
static void _esvg_element_state_compose(Esvg_Element *thiz,
		const Esvg_Element_Context *parent, Esvg_Element_Context *d)
{
	Enesim_Matrix m;

	esvg_attribute_animated_transform_final_get(&thiz->transform, &m);
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
static void _esvg_element_attribute_presentation_free(Esvg_Element_Attributes *a)
{
	Esvg_Attribute_Paint *p;
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
static void _esvg_element_attribute_presentation_setup(Esvg_Element_Attributes *a)
{
	Esvg_Color black = { 0, 0, 0 };
	Esvg_Length one = { ESVG_UNIT_LENGTH_PX, 1 };
	Esvg_Paint color = { ESVG_PAINT_TYPE_COLOR, black };
	Esvg_Paint none = { ESVG_PAINT_TYPE_NONE };
	Esvg_Number onen = { 1.0 };

	/* now the default values */
	esvg_attribute_color_unset(&a->color.base, &black);
	esvg_attribute_length_unset(&a->stroke_width.base, &one);
	esvg_attribute_number_unset(&a->stroke_opacity.base, &onen);
	esvg_attribute_number_unset(&a->fill_opacity.base, &onen);
	esvg_attribute_number_unset(&a->opacity.base, &onen);
	esvg_attribute_enum_unset(&a->fill_rule.base, ESVG_NON_ZERO);
	esvg_attribute_paint_unset(&a->fill.base, &color);
	esvg_attribute_paint_unset(&a->stroke.base, &none);
	esvg_attribute_enum_unset(&a->stroke_line_cap.base, ESVG_LINE_CAP_BUTT);
	esvg_attribute_enum_unset(&a->stroke_line_join.base, ESVG_LINE_JOIN_MITER);
	esvg_attribute_number_unset(&a->stop_opacity.base, &onen);
	esvg_attribute_enum_unset(&a->visibility.base, ESVG_VISIBILITY_VISIBLE);
	esvg_attribute_enum_unset(&a->display.base, ESVG_DISPLAY_BLOCK);
}

static void _esvg_element_attribute_presentation_merge(
	const Esvg_Element_Attributes *s,
	Esvg_Attribute_Presentation *d)
{
	/* clip path */
	esvg_attribute_animated_clip_path_merge(&s->clip_path, &d->clip_path);
	/* color */
	esvg_attribute_animated_color_merge(&s->color, &d->color);
	/* opacity */
	esvg_attribute_animated_number_merge(&s->opacity, &d->opacity);
	/* FIXME do the real merge (multiply, etc, etc) */
	/* fill */
	esvg_attribute_animated_paint_merge(&s->fill, &d->fill);
	/* fill opacity */
	esvg_attribute_animated_number_merge(&s->fill_opacity, &d->fill_opacity);
	/* fill rule */
	esvg_attribute_animated_enum_merge(&s->fill_rule, &d->fill_rule);
	/* stroke */
	esvg_attribute_animated_paint_merge(&s->stroke, &d->stroke);
	/* stroke width */
	esvg_attribute_animated_length_merge(&s->stroke_width, &d->stroke_width);
	/* stroke line cap */
	esvg_attribute_animated_enum_merge(&s->stroke_line_cap, &d->stroke_line_cap);
	/* stroke line join */
	esvg_attribute_animated_enum_merge(&s->stroke_line_join, &d->stroke_line_join);
	/* stroke opacity */
	esvg_attribute_animated_number_merge(&s->stroke_opacity, &d->stroke_opacity);
	/* visibility */
	esvg_attribute_animated_enum_merge(&s->visibility, &d->visibility);
	/* stop opacity */
	esvg_attribute_animated_number_merge(&s->stop_opacity, &d->stop_opacity);
	/* stop color */
	esvg_attribute_animated_color_merge(&s->stop_color, &d->stop_color);
	/* display */
	esvg_attribute_animated_enum_merge(&s->display, &d->display);
}

static void _esvg_element_attribute_presentation_merge_rel(
	const Esvg_Element_Attributes *rel,
	const Esvg_Element_Attributes *s,
	Esvg_Attribute_Presentation *d)
{
	/* clip path */
	esvg_attribute_animated_clip_path_merge_rel(&rel->clip_path, &s->clip_path, &d->clip_path);
	/* color */
	esvg_attribute_animated_color_merge_rel(&rel->color, &s->color, &d->color);
	/* opacity */
	esvg_attribute_animated_number_merge_rel(&rel->opacity, &s->opacity, &d->opacity);
	/* FIXME do the real merge_rel (multiply, etc, etc) */
	/* fill */
	esvg_attribute_animated_paint_merge_rel(&rel->fill, &s->fill, &d->fill);
	/* fill opacity */
	esvg_attribute_animated_number_merge_rel(&rel->fill_opacity, &s->fill_opacity, &d->fill_opacity);
	/* fill rule */
	esvg_attribute_animated_enum_merge_rel(&rel->fill_rule, &s->fill_rule, &d->fill_rule);
	/* stroke */
	esvg_attribute_animated_paint_merge_rel(&rel->stroke, &s->stroke, &d->stroke);
	/* stroke width */
	esvg_attribute_animated_length_merge_rel(&rel->stroke_width, &s->stroke_width, &d->stroke_width);
	/* stroke line cap */
	esvg_attribute_animated_enum_merge_rel(&rel->stroke_line_cap, &s->stroke_line_cap, &d->stroke_line_cap);
	/* stroke line join */
	esvg_attribute_animated_enum_merge_rel(&rel->stroke_line_join, &s->stroke_line_join, &d->stroke_line_join);
	/* stroke opacity */
	esvg_attribute_animated_number_merge_rel(&rel->stroke_opacity, &s->stroke_opacity, &d->stroke_opacity);
	/* visibility */
	esvg_attribute_animated_enum_merge_rel(&rel->visibility, &s->visibility, &d->visibility);
	/* stop opacity */
	esvg_attribute_animated_number_merge_rel(&rel->stop_opacity, &s->stop_opacity, &d->stop_opacity);
	/* stop color */
	esvg_attribute_animated_color_merge_rel(&rel->stop_color, &s->stop_color, &d->stop_color);
	/* display */
	esvg_attribute_animated_enum_merge_rel(&rel->display, &s->display, &d->display);
}
/*----------------------------------------------------------------------------*
 *                               Setup helpers                                *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_element_child_setup_cb(Edom_Tag *t, Edom_Tag *child,
		void *data)
{
	Esvg_Element_Setup_Data *setup_data = data;
	Esvg_Element_Setup_Return ret;
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
	ret = esvg_element_internal_setup(child, setup_data->c, setup_data->error);
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

/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Esvg_Type _esvg_element_type_get(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	return thiz->type;
}

static void _esvg_element_id_get(Edom_Tag *t, const char **id)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	if (!id) return;
	*id = thiz->id;
}

static void _esvg_element_id_set(Edom_Tag *t, const char *id)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	if (thiz->id)
	{
		free(thiz->id);
		thiz->id = NULL;
	}
	if (id)
	{
		thiz->id = strdup(id);
	}
}

static void _esvg_element_class_set(Edom_Tag *t, const char *class)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	if (thiz->class)
	{
		free(thiz->class);
		thiz->class = NULL;
	}
	if (class)
	{
		thiz->class = strdup(class);
	}
}

static void _esvg_element_class_get(Edom_Tag *t, const char **class)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	if (class)
		*class = thiz->class;
}

static void _esvg_element_transform_set(Edom_Tag *t, const Esvg_Matrix_Animated *transform)
{
	Esvg_Element *thiz;
	Enesim_Matrix m;

	thiz = _esvg_element_get(t);
	enesim_matrix_identity(&m);
	esvg_attribute_animated_transform_extended_set(&thiz->transform,
		transform, &m, thiz->current_attr_animate, &thiz->current_attr->sets);
}

static void _esvg_element_transform_get(Edom_Tag *t, Esvg_Matrix_Animated *transform)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_transform_get(&thiz->transform, transform);
}

static Eina_Bool _esvg_element_transform_is_set(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	return esvg_attribute_animated_transform_is_set(&thiz->transform);
}

static void _esvg_element_style_set(Edom_Tag *t, const char *style)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	/* FIXME we need to understand how SVG handles inheritance of properties
	 * and styling, does this properties go to our direct property matching?
	 * do they go into another set of properties that are set up on the _setup()
	 * function, what happens whenever the user changes the style string? remove old
	 * properties? and if the user has also set some property manually?
	 */
	if (thiz->style)
		free(thiz->style);
	if (style)
		thiz->style = strdup(style);
	thiz->style_changed = EINA_TRUE;
}

static void _esvg_element_style_get(Edom_Tag *t, const char **style)
{
	Esvg_Element *thiz;

	if (!style) return;
	thiz = _esvg_element_get(t);
	*style = thiz->style;
}

/* presentation attributes */
static void _esvg_element_clip_path_set(Edom_Tag *t, Esvg_Animated_Clip_Path *clip_path)
{
	Esvg_Element *thiz;
	Esvg_Clip_Path def = { ESVG_CLIP_PATH_NONE };

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_clip_path_extended_set(&thiz->current_attr->clip_path,
		clip_path, &def, thiz->current_attr_animate, &thiz->current_attr->sets);
}

static void _esvg_element_clip_path_get(Edom_Tag *t, Esvg_Animated_Clip_Path *clip_path)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_clip_path_get(&thiz->current_attr->clip_path,
		clip_path);
}

static void _esvg_element_clip_path_unset(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
}

static void _esvg_element_opacity_set(Edom_Tag *t, Esvg_Number_Animated *opacity)
{
	Esvg_Element *thiz;
	Esvg_Number def = { 1.0 };

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_number_extended_set(&thiz->current_attr->opacity,
		opacity, &def, thiz->current_attr_animate, &thiz->current_attr->sets);
}

static void _esvg_element_opacity_get(Edom_Tag *t, Esvg_Number_Animated *opacity)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_number_get(&thiz->current_attr->opacity,
		opacity);
}

static void _esvg_element_opacity_unset(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
}

static void _esvg_element_color_set(Edom_Tag *t, const Esvg_Color_Animated *color)
{
	Esvg_Element *thiz;
	const Esvg_Color def = { 0, 0, 0};

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_color_set(&thiz->current_attr->color,
		color, &def, thiz->current_attr_animate);
}

static void _esvg_element_color_unset(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
}

static void _esvg_element_color_get(Edom_Tag *t, Esvg_Color_Animated *color)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_color_get(&thiz->current_attr->color,
		color);
}

static void _esvg_element_fill_set(Edom_Tag *t, const Esvg_Paint_Animated *fill)
{
	Esvg_Element *thiz;
	Esvg_Color black = { 0, 0, 0 };
	Esvg_Paint def = { ESVG_PAINT_TYPE_COLOR, black };

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_paint_extended_set(&thiz->current_attr->fill,
		fill, &def, thiz->current_attr_animate, &thiz->current_attr->sets);
}

static void _esvg_element_fill_get(Edom_Tag *t, Esvg_Paint_Animated *fill)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_paint_get(&thiz->current_attr->fill,
		fill);
}

static void _esvg_element_fill_unset(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
}

static void _esvg_element_fill_opacity_set(Edom_Tag *t, Esvg_Number_Animated *fill_opacity)
{
	Esvg_Element *thiz;
	Esvg_Number def = { 1.0 };

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_number_extended_set(&thiz->current_attr->fill_opacity,
		fill_opacity, &def, thiz->current_attr_animate, &thiz->current_attr->sets);
}

static void _esvg_element_fill_opacity_get(Edom_Tag *t, Esvg_Number_Animated *fill_opacity)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_number_get(&thiz->current_attr->fill_opacity,
		fill_opacity);
}

static void _esvg_element_fill_opacity_unset(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
}

static void _esvg_element_fill_rule_set(Edom_Tag *t, Esvg_Animated_Enum *fill_rule)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
}

static void _esvg_element_stroke_set(Edom_Tag *t, const Esvg_Paint_Animated *stroke)
{
	Esvg_Element *thiz;
	Esvg_Paint def = { ESVG_PAINT_TYPE_NONE };

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_paint_extended_set(&thiz->current_attr->stroke,
		stroke, &def, thiz->current_attr_animate, &thiz->current_attr->sets);
}

static void _esvg_element_stroke_get(Edom_Tag *t, Esvg_Paint_Animated *stroke)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_paint_get(&thiz->current_attr->stroke,
		stroke);
}

static void _esvg_element_stroke_width_set(Edom_Tag *t, const Esvg_Length_Animated *stroke_width)
{
	Esvg_Element *thiz;
	Esvg_Length def = { ESVG_UNIT_LENGTH_PX, 1 };
	Eina_Bool is_set;

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_length_extended_set(&thiz->current_attr->stroke_width,
		stroke_width, &def, thiz->current_attr_animate, &thiz->current_attr->sets);
}

static void _esvg_element_stroke_width_get(Edom_Tag *t, Esvg_Length_Animated *stroke_width)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_length_get(&thiz->current_attr->stroke_width,
		stroke_width);
}

static void _esvg_element_stroke_opacity_set(Edom_Tag *t, Esvg_Number_Animated *stroke_opacity)
{
	Esvg_Element *thiz;
	Esvg_Number def = { 1.0 };

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_number_extended_set(&thiz->current_attr->stroke_opacity,
		stroke_opacity, &def, thiz->current_attr_animate, &thiz->current_attr->sets);
}

static void _esvg_element_stroke_opacity_get(Edom_Tag *t, Esvg_Number_Animated *stroke_opacity)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_number_get(&thiz->current_attr->stroke_opacity,
		stroke_opacity);
}

static void _esvg_element_stroke_line_cap_set(Edom_Tag *t, Esvg_Animated_Enum *cap)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
}

static void _esvg_element_stroke_line_join_set(Edom_Tag *t, Esvg_Animated_Enum *join)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
}

static void _esvg_element_stop_color_set(Edom_Tag *t, Esvg_Color_Animated *stop_color)
{
	Esvg_Element *thiz;
	const Esvg_Color def = { 0, 0, 0};

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_color_set(&thiz->current_attr->stop_color,
		stop_color, &def, thiz->current_attr_animate);
}

static void _esvg_element_stop_color_get(Edom_Tag *t, Esvg_Color_Animated *stop_color)
{
	Esvg_Element *thiz;

	if (!stop_color) return;
	thiz = _esvg_element_get(t);
	esvg_attribute_animated_color_get(&thiz->current_attr->stop_color,
		stop_color);
}

static void _esvg_element_stop_opacity_set(Edom_Tag *t, Esvg_Number_Animated *stop_opacity)
{
	Esvg_Element *thiz;
	Esvg_Number def = { 1.0 };

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_number_extended_set(&thiz->current_attr->stop_opacity,
		stop_opacity, &def, thiz->current_attr_animate, &thiz->current_attr->sets);
}

static void _esvg_element_stop_opacity_get(Edom_Tag *t, Esvg_Number_Animated *stop_opacity)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_number_get(&thiz->current_attr->stop_opacity,
		stop_opacity);
}

static void _esvg_element_visibility_set(Edom_Tag *t, Esvg_Animated_Enum *visibility)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_enum_extended_set(&thiz->current_attr->visibility,
		visibility, ESVG_VISIBILITY_VISIBLE, thiz->current_attr_animate, &thiz->current_attr->sets);
}

static void _esvg_element_visibility_get(Edom_Tag *t, Esvg_Animated_Enum *visibility)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_enum_get(&thiz->current_attr->visibility,
		visibility);
}

static void _esvg_element_display_set(Edom_Tag *t, Esvg_Animated_Enum *display)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_enum_extended_set(&thiz->current_attr->display,
		display, ESVG_DISPLAY_BLOCK, thiz->current_attr_animate, &thiz->current_attr->sets);
}

static void _esvg_element_display_get(Edom_Tag *t, Esvg_Animated_Enum *display)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_enum_get(&thiz->current_attr->display,
		display);
}

static void _esvg_element_onfocusin_set(Edom_Tag *t, const char *v)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
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

static void _esvg_element_onfocusin_get(Edom_Tag *t, const char **v)
{
	Esvg_Element *thiz;

	if (!v) return;
	thiz = _esvg_element_get(t);
	*v = thiz->onfocusin;
}

static void _esvg_element_onfocusout_set(Edom_Tag *t, const char *v)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
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

static void _esvg_element_onfocusout_get(Edom_Tag *t, const char **v)
{
	Esvg_Element *thiz;

	if (!v) return;
	thiz = _esvg_element_get(t);
	*v = thiz->onfocusout;
}

static void _esvg_element_onactivate_set(Edom_Tag *t, const char *v)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
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

static void _esvg_element_onactivate_get(Edom_Tag *t, const char **v)
{
	Esvg_Element *thiz;

	if (!v) return;
	thiz = _esvg_element_get(t);
	*v = thiz->onactivate;
}

static void _esvg_element_onclick_set(Edom_Tag *t, const char *v)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
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

static void _esvg_element_onclick_get(Edom_Tag *t, const char **v)
{
	Esvg_Element *thiz;

	if (!v) return;
	thiz = _esvg_element_get(t);
	*v = thiz->onclick;
}

static void _esvg_element_onmousedown_set(Edom_Tag *t, const char *v)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
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

static void _esvg_element_onmousedown_get(Edom_Tag *t, const char **v)
{
	Esvg_Element *thiz;

	if (!v) return;
	thiz = _esvg_element_get(t);
	*v = thiz->onmousedown;
}

static void _esvg_element_onmouseup_set(Edom_Tag *t, const char *v)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
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

static void _esvg_element_onmouseup_get(Edom_Tag *t, const char **v)
{
	Esvg_Element *thiz;

	if (!v) return;
	thiz = _esvg_element_get(t);
	*v = thiz->onmouseup;
}

static void _esvg_element_onmouseover_set(Edom_Tag *t, const char *v)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
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

static void _esvg_element_onmouseover_get(Edom_Tag *t, const char **v)
{
	Esvg_Element *thiz;

	if (!v) return;
	thiz = _esvg_element_get(t);
	*v = thiz->onmouseover;
}

static void _esvg_element_onmousemove_set(Edom_Tag *t, const char *v)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
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

static void _esvg_element_onmousemove_get(Edom_Tag *t, const char **v)
{
	Esvg_Element *thiz;

	if (!v) return;
	thiz = _esvg_element_get(t);
	*v = thiz->onmousemove;
}

static void _esvg_element_onmouseout_set(Edom_Tag *t, const char *v)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
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

static void _esvg_element_onmouseout_get(Edom_Tag *t, const char **v)
{
	Esvg_Element *thiz;

	if (!v) return;
	thiz = _esvg_element_get(t);
	*v = thiz->onmouseout;
}

/* The ender wrapper */
#define _esvg_element_delete NULL
#define _esvg_element_type_set NULL
#define _esvg_element_type_is_set NULL
#define _esvg_element_topmost_set NULL
#define _esvg_element_topmost_is_set NULL
#define _esvg_element_topmost_get esvg_element_internal_topmost_get
#define _esvg_element_id_is_set NULL
#define _esvg_element_class_is_set NULL
#define _esvg_element_style_is_set NULL
#define _esvg_element_clip_path_is_set NULL
#define _esvg_element_color_is_set NULL
#define _esvg_element_fill_is_set NULL
#define _esvg_element_stroke_is_set NULL
#define _esvg_element_stroke_width_is_set NULL
#define _esvg_element_stroke_opacity_is_set NULL
#define _esvg_element_fill_is_set NULL
#define _esvg_element_fill_opacity_is_set NULL
#define _esvg_element_opacity_is_set NULL
#define _esvg_element_visibility_is_set NULL
#define _esvg_element_stop_color_is_set NULL
#define _esvg_element_stop_opacity_is_set NULL
#define _esvg_element_display_is_set NULL

#define _esvg_element_onfocusin_is_set NULL
#define _esvg_element_onfocusout_is_set NULL
#define _esvg_element_onactivate_is_set NULL
#define _esvg_element_onclick_is_set NULL
#define _esvg_element_onmousedown_is_set NULL
#define _esvg_element_onmouseup_is_set NULL
#define _esvg_element_onmouseover_is_set NULL
#define _esvg_element_onmousemove_is_set NULL
#define _esvg_element_onmouseout_is_set NULL
#include "generated/esvg_generated_element.c"
/*----------------------------------------------------------------------------*
 *                           The Edom Tag interface                           *
 *----------------------------------------------------------------------------*/
static const char * _esvg_element_name_get(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	return esvg_type_string_to(thiz->type);
}

static Edom_Tag * _esvg_element_tag_topmost_get(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	if (!thiz->topmost) return NULL;

	return ender_element_object_get(thiz->topmost);
}

static Eina_Bool _esvg_element_child_add(Edom_Tag *t, Edom_Tag *child)
{
	Esvg_Element *thiz;
	Eina_Bool ret = EINA_FALSE;

	thiz = _esvg_element_get(t);
	if (!esvg_is_element_internal(child))
	{
		ERR("Element '%s' can not add a non-esvg child %p",
				esvg_type_string_to(esvg_element_internal_type_get(t)),
				child);
		return EINA_FALSE;
	}

	if (thiz->descriptor.child_add)
		ret = thiz->descriptor.child_add(t, child);

	if (!ret)
	{
		ERR("Element '%s' can not add child '%s'",
				esvg_type_string_to(esvg_element_internal_type_get(t)),
				esvg_type_string_to(esvg_element_internal_type_get(child)));
	}
	return EINA_TRUE;
	/* FIXME once every element supports correctly the possible childs to add */
	//return ret;
}

static Eina_Bool _esvg_element_child_remove(Edom_Tag *t, Edom_Tag *child)
{
	Esvg_Element *thiz;
	Eina_Bool ret = EINA_FALSE;

	thiz = _esvg_element_get(t);
	/* FIXME avoid this case, put it on the common element functions */
	if (!esvg_is_element_internal(child))
	{
		ERR("Element '%s' has a non-esvg child %p?",
				esvg_type_string_to(esvg_element_internal_type_get(t)),
				child);
		return EINA_FALSE;
	}

	if (thiz->descriptor.child_remove)
		ret = thiz->descriptor.child_remove(t, child);

	if (!ret)
	{
		ERR("Element '%s' can not remove child '%s'",
				esvg_type_string_to(esvg_element_internal_type_get(t)),
				esvg_type_string_to(esvg_element_internal_type_get(child)));
	}

	return ret;
}

static Eina_Bool _esvg_element_attribute_set(Edom_Tag *t, const char *key, const char *value)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	DBG("Setting attribute '%s' with value '%s' on element '%s'",
		key, value, esvg_type_string_to(esvg_element_internal_type_get(t)));
	if (thiz->descriptor.attribute_set)
	{
		if (thiz->descriptor.attribute_set(thiz->e, key, value))
			return EINA_TRUE;
	}
	if (strcmp(key, "id") == 0)
	{
		esvg_element_id_set(thiz->e, value);
	}
	else if (strcmp(key, "class") == 0)
	{
		esvg_element_class_set(thiz->e, value);
	}
	else if (strcmp(key, "transform") == 0)
	{
		Enesim_Matrix matrix;

		esvg_transformation_string_from(&matrix, value);
		esvg_element_transform_set(thiz->e, &matrix);
	}
	else if (strcmp(key, "style") == 0)
	{
		esvg_element_style_set(thiz->e, value);
	}
	/* common presentation attributes */
	else if (strcmp(key, "clip-path") == 0)
	{
		Esvg_Clip_Path clip_path;

		esvg_clip_path_string_from(&clip_path, value);
		esvg_element_clip_path_set(thiz->e, &clip_path);
	}
	else if (strcmp(key, "opacity") == 0)
	{
		Esvg_Number opacity;
		esvg_number_string_from(&opacity, value, 1.0);
		esvg_element_opacity_set(thiz->e, &opacity);
	}
	else if (strcmp(key, "color") == 0)
	{
		Esvg_Color color;

		esvg_color_string_from(&color, value);
		esvg_element_color_set(thiz->e, &color);
	}
	else if (strcmp(key, "fill") == 0)
	{
		Esvg_Paint fill;

		if (esvg_paint_string_from(&fill, value))
			esvg_element_fill_set(thiz->e, &fill);
	}
	else if (strcmp(key, "fill-rule") == 0)
	{
		Esvg_Fill_Rule fill_rule;

		esvg_parser_fill_rule_get(&fill_rule, value);
		esvg_element_fill_rule_set(thiz->e, fill_rule);
	}
	else if (strcmp(key, "fill-opacity") == 0)
	{
		Esvg_Number fill_opacity;
		esvg_number_string_from(&fill_opacity, value, 1.0);
		esvg_element_fill_opacity_set(thiz->e, &fill_opacity);
	}
	else if (strcmp(key, "stroke") == 0)
	{
		Esvg_Paint stroke;

		if (esvg_paint_string_from(&stroke, value))
			esvg_element_stroke_set(thiz->e, &stroke);
	}
	else if (strcmp(key, "stroke-width") == 0)
	{
		Esvg_Length stroke_width;

		esvg_length_string_from(&stroke_width, value);
		esvg_element_stroke_width_set(thiz->e, &stroke_width);
	}
	else if (strcmp(key, "stroke-opacity") == 0)
	{
		Esvg_Number stroke_opacity;

		esvg_number_string_from(&stroke_opacity, value, 1.0);
		esvg_element_stroke_opacity_set(thiz->e, &stroke_opacity);
	}
	else if (strcmp(key, "stroke-linecap") == 0)
	{
		Esvg_Stroke_Line_Cap stroke_line_cap;

		stroke_line_cap = esvg_stroke_line_cap_string_from(value);
		esvg_element_stroke_line_cap_set(thiz->e, stroke_line_cap);
	}
	else if (strcmp(key, "stroke-linejoin") == 0)
	{
		Esvg_Stroke_Line_Join stroke_line_join;

		stroke_line_join = esvg_stroke_line_join_string_from(value);
		esvg_element_stroke_line_join_set(thiz->e, stroke_line_join);
	}
	else if (strcmp(key, "stop-color") == 0)
	{
		Esvg_Color color;

		esvg_color_string_from(&color, value);
		esvg_element_stop_color_set(thiz->e, &color);
	}
	else if (strcmp(key, "stop-opacity") == 0)
	{
		Esvg_Number stop_opacity;

		esvg_number_string_from(&stop_opacity, value, 1.0);
		esvg_element_stop_opacity_set(thiz->e, &stop_opacity);
	}
	else if (strcmp(key, "visibility") == 0)
	{
		Esvg_Visibility visibility;
		esvg_visibility_string_from(&visibility, value);
		esvg_element_visibility_set(thiz->e, visibility);
	}
	else if (strcmp(key, "display") == 0)
	{
		Esvg_Display display;
		esvg_display_string_from(&display, value);
		esvg_element_display_set(thiz->e, display);
	}
	else if (!strcmp(key, "onfocusin"))
	{
		esvg_element_onfocusin_set(thiz->e, value);
	}
	else if (!strcmp(key, "onfocusout"))
	{
		esvg_element_onfocusout_set(thiz->e, value);
	}
	else if (!strcmp(key, "onactivate"))
	{
		esvg_element_onactivate_set(thiz->e, value);
	}
	else if (!strcmp(key, "onclick"))
	{
		esvg_element_onclick_set(thiz->e, value);
	}
	else if (!strcmp(key, "onmousedown"))
	{
		esvg_element_onmousedown_set(thiz->e, value);
	}
	else if (!strcmp(key, "onmouseup"))
	{
		esvg_element_onmouseup_set(thiz->e, value);
	}
	else if (!strcmp(key, "onmouseover"))
	{
		esvg_element_onmouseover_set(thiz->e, value);
	}
	else if (!strcmp(key, "onmousemove"))
	{
		esvg_element_onmousemove_set(thiz->e, value);
	}
	else if (!strcmp(key, "onmouseout"))
	{
		esvg_element_onmouseout_set(thiz->e, value);
	}
	/* TODO in theory we should not allow css attributes to continue */
	else
	{
		ERR("Unsupported attribute set '%s' with value '%s' on element '%s'",
				key, value,
				esvg_type_string_to(esvg_element_internal_type_get(t)));
		return EINA_FALSE;
	}

	return EINA_TRUE;
}

static Eina_Bool _esvg_element_attribute_get(Edom_Tag *t, const char *key, char **value)
{
	Esvg_Element *thiz;
	Eina_Bool ret;

	thiz = _esvg_element_get(t);
	DBG("Getting attribute '%s' on element '%s'",
		key, esvg_type_string_to(esvg_element_internal_type_get(t)));
	/* we need to first call the implemenation as there might be name collisions */
	if (thiz->descriptor.attribute_get)
	{
			if (thiz->descriptor.attribute_get(t, key, value))
				return EINA_TRUE;
	}

	/* FIXME handle common properties */
	if (strcmp(key, "id") == 0)
	{
          _esvg_element_id_get(t, (const char **)value);
	}
	else if (strcmp(key, "class") == 0)
	{
		_esvg_element_class_get(t, (const char **)value);
	}
#if 0
	/* TODO all the below cases should get the attribute
	 * then, call a function to convert it to string
	 * and return such value
	 */
	else if (strcmp(key, "transform") == 0)
	{
		Enesim_Matrix matrix;

		_esvg_element_transform_get(thiz->e, &matrix);
		esvg_transformation_string_from(&matrix, value);
	}
	else if (strcmp(key, "style") == 0)
	{
		_esvg_element_style_get(thiz->e, value);
	}
	/* common presentation attributes */
	else if (strcmp(key, "clip-path") == 0)
	{
		_esvg_element_clip_path_get(thiz->e, value);
	}
	else if (strcmp(key, "opacity") == 0)
	{
		double opacity;

		_esvg_element_opacity_get(thiz->e, opacity);
		 value = esvg_number_string_from(value, 1.0);
	}
	else if (strcmp(key, "color") == 0)
	{
		Esvg_Color color;

		esvg_color_string_from(&color, value);
		_esvg_element_color_get(thiz->e, &color);
	}
	else if (strcmp(key, "fill") == 0)
	{
		Esvg_Paint fill;

		if (esvg_paint_string_from(&fill, value))
			_esvg_element_fill_get(thiz->e, &fill);
	}
	else if (strcmp(key, "fill-rule") == 0)
	{
		Esvg_Fill_Rule fill_rule;

		esvg_parser_fill_rule_get(&fill_rule, value);
		_esvg_element_fill_rule_get(thiz->e, fill_rule);
	}
	else if (strcmp(key, "fill-opacity") == 0)
	{
		double fill_opacity = esvg_number_string_from(value, 1.0);
		_esvg_element_fill_opacity_get(thiz->e, fill_opacity);
	}
	else if (strcmp(key, "stroke") == 0)
	{
		Esvg_Paint stroke;

		if (esvg_paint_string_from(&stroke, value))
			_esvg_element_stroke_get(thiz->e, &stroke);
	}
	else if (strcmp(key, "stroke-width") == 0)
	{
		Esvg_Length stroke_width;

		esvg_length_string_from(&stroke_width, value, ESVG_LENGTH_1);
		_esvg_element_stroke_width_get(thiz->e, &stroke_width);
	}
	else if (strcmp(key, "stroke-opacity") == 0)
	{
		double stroke_opacity = esvg_number_string_from(value, 1.0);
		_esvg_element_stroke_opacity_get(thiz->e, stroke_opacity);
	}
	else if (strcmp(key, "stroke-linecap") == 0)
	{
		Esvg_Stroke_Line_Cap stroke_line_cap;

		stroke_line_cap = esvg_stroke_line_cap_string_from(value);
		_esvg_element_stroke_line_cap_get(thiz->e, stroke_line_cap);
	}
	else if (strcmp(key, "stroke-linejoin") == 0)
	{
		Esvg_Stroke_Line_Join stroke_line_join;

		stroke_line_join = esvg_stroke_line_join_string_from(value);
		_esvg_element_stroke_line_join_get(thiz->e, stroke_line_join);
	}
	else if (strcmp(key, "stop-color") == 0)
	{
		Esvg_Color color;

		esvg_color_string_from(&color, value);
		_esvg_element_stop_color_get(thiz->e, &color);
	}
	else if (strcmp(key, "stop-opacity") == 0)
	{
		double stop_opacity = esvg_number_string_from(value, 1.0);
		_esvg_element_stop_opacity_get(thiz->e, stop_opacity);
	}
#endif
	else
	{
		ERR("Unsupported attribute get '%s'", key);
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

static void _esvg_element_free(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	/* remove every child object */
	edom_tag_child_foreach(t, _esvg_element_child_free_cb, NULL);
	_esvg_element_attribute_presentation_free(&thiz->attr_xml);
	_esvg_element_attribute_presentation_free(&thiz->attr_css);
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
static const char * _esvg_element_css_property_get(void *e, const char *property)
{
	Edom_Tag *tag = e;
	char *value;

	_esvg_element_attribute_get(tag, property, &value);

	return value;
}

/* FIXME we could call directly the function _attribute_set */
static void _esvg_element_css_property_set(void *e, const char *property, const char *value)
{
	Edom_Tag *tag = e;
	_esvg_element_attribute_set(tag, property, value);
}

static void * _esvg_element_css_get_child(void *e)
{
	Edom_Tag *tag = e;
	return edom_tag_child_get(tag);
}

static void * _esvg_element_css_get_next_sibling(void *e)
{
	Edom_Tag *tag = e;
	return edom_tag_next_get(tag);
}

static const char * _esvg_element_css_get_name(void *e)
{
	Edom_Tag *t = e;
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	return esvg_type_string_to(thiz->type);
}

static Ecss_Context _esvg_element_css_context = {
	/* .property_set 	= */ _esvg_element_css_property_set,
	/* .property_get 	= */ _esvg_element_css_property_get,
	/* .get_name 		= */ _esvg_element_css_get_name,
	/* .get_child 		= */ _esvg_element_css_get_child,
	/* .get_next_sibling 	= */ _esvg_element_css_get_next_sibling,
};

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool esvg_element_attribute_animation_add(Edom_Tag *t, const char *attr,
		int *index)
{
	int *animated;

	DBG("Adding animation on '%s'", attr);
	animated = _esvg_element_attribute_animated_get(t, attr);
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

void esvg_element_attribute_animation_remove(Edom_Tag *t, const char *attr)
{
	int *animated;

	DBG("Removing animation on '%s'", attr);
	animated = _esvg_element_attribute_animated_get(t, attr);
	if (animated)
		(*animated)--;
}

Esvg_Type esvg_element_internal_type_get(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	return thiz->type;
}

const char * esvg_element_internal_id_get(Edom_Tag *t)
{
	const char *id;

	_esvg_element_id_get(t, &id);
	return id;
}

Eina_Bool esvg_is_element_internal(Edom_Tag *t)
{
	Esvg_Element *thiz;
	Eina_Bool ret;

	thiz = edom_tag_data_get(t);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_ELEMENT_MAGIC);

	return ret;
}

void esvg_element_internal_topmost_get(Edom_Tag *t, Ender_Element **e)
{
	Esvg_Element *thiz;

	if (!e) return;
	thiz = _esvg_element_get(t);
	*e = thiz->topmost;
}

/* FIXME what do we do here? return bool or the setup return? */
Eina_Bool esvg_element_internal_child_setup(Edom_Tag *t,
		Esvg_Context *c,
		Enesim_Error **error,
		Esvg_Element_Setup_Interceptor pre,
		Esvg_Element_Setup_Interceptor post,
		void *data)
{
	Esvg_Element_Setup_Data setup_data;

	setup_data.c = c;
	setup_data.pre = pre;
	setup_data.post = post;
	setup_data.error = error;
	setup_data.ret = EINA_TRUE;
	setup_data.data = data;

	edom_tag_child_foreach(t, _esvg_element_child_setup_cb, &setup_data);
	return setup_data.ret;
}

Esvg_Element_Setup_Return esvg_element_setup_rel(Edom_Tag *t,
		Esvg_Context *c,
		const Esvg_Element_Context *rel_state,
		const Esvg_Attribute_Presentation *rel_attr,
		Enesim_Error **error)
{
	Esvg_Element *thiz;
	Esvg_Element_Setup_Return ret;

	thiz = _esvg_element_get(t);
	DBG("Doing the setup on the tag '%s'", edom_tag_name_get(t));
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
	_esvg_element_state_compose(thiz, rel_state, &thiz->state_final);

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
			esvg_element_attribute_type_set(t, ESVG_ATTR_CSS);
			ecss_context_inline_style_apply(&_esvg_element_css_context, thiz->style, t);
			esvg_element_attribute_type_set(t, ESVG_ATTR_XML);
		}
		thiz->style_changed = EINA_FALSE;
	}

	if (thiz->attr_css.sets)
	{
		/* merge the css and the xml into the final */
		_esvg_element_attribute_presentation_merge_rel(&thiz->attr_xml, &thiz->attr_css, &thiz->attr_final);
	}
	else
	{
		_esvg_element_attribute_presentation_merge(&thiz->attr_xml, &thiz->attr_final);
	}

	if (rel_attr)
	{
		esvg_attribute_presentation_merge_rel(rel_attr, &thiz->attr_final, &thiz->attr_final);
	}


	if (!thiz->descriptor.setup)
		return ESVG_SETUP_OK;

	//esvg_attribute_presentation_dump(new_attr);
	ret = thiz->descriptor.setup(t, c, rel_state, &thiz->state_final, &thiz->attr_final, error);
	if (ret == ESVG_SETUP_ENQUEUE)
	{
		esvg_context_setup_enqueue(c, t);
	}
	else
	{
		thiz->changed = 0;
	}
	return ret;
}

Esvg_Element_Setup_Return esvg_element_internal_setup(Edom_Tag *t,
		Esvg_Context *c,
		Enesim_Error **error)
{
	Edom_Tag *parent_t;
	Esvg_Element_Context *parent_state = NULL;
	Esvg_Attribute_Presentation *parent_attr = NULL;

	parent_t = edom_tag_parent_get(t);
	if (parent_t)
	{

		Esvg_Element *parent_thiz;

		parent_thiz = _esvg_element_get(parent_t);
		parent_state = &parent_thiz->state_final;
		parent_attr = &parent_thiz->attr_final;
	}
	return esvg_element_setup_rel(t, c,
			parent_state, parent_attr, error);
}

void esvg_element_initialize(Ender_Element *e)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(ender_element_object_get(e));
	thiz->e = e;

	/* register the mutation events */
	ender_event_listener_add(e, "Mutation", _esvg_element_mutation_cb, thiz);
	/* register the dom events */
	ender_event_listener_add(e, "focusin", _esvg_element_onfocusin_call, thiz);
	ender_event_listener_add(e, "focusout", _esvg_element_onfocusout_call, thiz);
	ender_event_listener_add(e, "activate", _esvg_element_onactivate_call, thiz);
	ender_event_listener_add(e, "click", _esvg_element_onclick_call, thiz);
	ender_event_listener_add(e, "mousedown", _esvg_element_onmousedown_call, thiz);
	ender_event_listener_add(e, "mouseup", _esvg_element_onmouseup_call, thiz);
	ender_event_listener_add(e, "mouseover", _esvg_element_onmouseover_call, thiz);
	ender_event_listener_add(e, "mousemove", _esvg_element_onmousemove_call, thiz);
	ender_event_listener_add(e, "mouseout", _esvg_element_onmouseout_call, thiz);

	if (thiz->descriptor.initialize)
		thiz->descriptor.initialize(e);
}

void esvg_element_style_apply(Edom_Tag *t, Ecss_Style *s)
{
	esvg_element_attribute_type_set(t, ESVG_ATTR_CSS);
	ecss_context_style_apply(&_esvg_element_css_context, s, t);
	esvg_element_attribute_type_set(t, ESVG_ATTR_XML);
}

void esvg_element_topmost_set(Edom_Tag *t, Ender_Element *topmost)
{
	Esvg_Element *thiz;
	Esvg_Element_Event_Topmost_Changed event_data;

	thiz = _esvg_element_get(t);
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

void * esvg_element_data_get(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	return thiz->data;
}

void esvg_element_attribute_type_set(Edom_Tag *t, Esvg_Attribute_Type type)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
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

Esvg_Attribute_Type esvg_element_attribute_type_get(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	return thiz->current_attr_type;
}

void esvg_element_attribute_animate_set(Edom_Tag *t, Eina_Bool animate)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	thiz->current_attr_animate = animate;
}

Eina_Bool esvg_element_attribute_animate_get(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	return thiz->current_attr_animate;
}

Ender_Element * esvg_element_ender_get(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	return thiz->e;
}

Eina_Bool esvg_element_changed(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	return thiz->changed;
}

Eina_Bool esvg_element_has_setup(Edom_Tag *t, Esvg_Context *c)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	if (thiz->last_run == c->run)
		return EINA_TRUE;
	return EINA_FALSE;
}

const Esvg_Element_Context * esvg_element_context_get(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	return &thiz->state_final;
}

const Esvg_Attribute_Presentation * esvg_element_attribute_presentation_get(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	return &thiz->attr_final;
}

/*----------------------------------------------------------------------------*
 *                           Property API helpers                             *
 *----------------------------------------------------------------------------*/
void esvg_element_property_string_set(Ender_Element *e, Ender_Property *p, const char *v)
{
	if (!v)
	{
		ender_element_property_value_set(e, p, NULL, NULL);
		return;
	}
	else
	{
		Esvg_String_Animated a;

		a.base = (char *)v;
		ender_element_property_value_set(e, p, &a, NULL);
	}
}

void esvg_element_property_length_set(Ender_Element *e, Ender_Property *p, const Esvg_Length *v)
{
	if (!v)
	{
		ender_element_property_value_set(e, p, NULL, NULL);
	}
	else
	{
		Esvg_Length_Animated a;

		a.base = *v;
		ender_element_property_value_set(e, p, &a, NULL);
	}
}

void esvg_element_property_paint_set(Ender_Element *e, Ender_Property *p, const Esvg_Paint *v)
{
	if (!v)
	{
		ender_element_property_value_set(e, p, NULL, NULL);
	}
	else
	{
		Esvg_Paint_Animated a;

		a.base = *v;
		ender_element_property_value_set(e, p, &a, NULL);
	}
}

void esvg_element_property_clip_path_set(Ender_Element *e, Ender_Property *p, const Esvg_Clip_Path *v)
{
	if (!v)
	{
		ender_element_property_value_set(e, p, NULL, NULL);
	}
	else
	{
		Esvg_Animated_Clip_Path a;

		a.base = *v;
		ender_element_property_value_set(e, p, &a, NULL);
	}
}

void esvg_element_property_number_set(Ender_Element *e, Ender_Property *p, const Esvg_Number *v)
{
	if (!v)
	{
		ender_element_property_value_set(e, p, NULL, NULL);
	}
	else
	{
		Esvg_Number_Animated a;

		a.base = *v;
		ender_element_property_value_set(e, p, &a, NULL);
	}
}

void esvg_element_property_enum_set(Ender_Element *e, Ender_Property *p, int v)
{
	Esvg_Animated_Enum a;

	a.base = v;
	ender_element_property_value_set(e, p, &a, NULL);
}

#if 0
/* FIXME this functions should be implemented */
void esvg_element_request_setup(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	if (thiz->setup_requested) return;
	thiz->setup_requested = EINA_TRUE;
	/* trigger the event so the svg element can capture it */
	ender_event_dispatch(thiz->e, "SetupRequested", NULL);
}

double esvg_element_context_x_length_calculate(Esvg_Element_Context *t, Esvg_Length *l)
{

}

double esvg_element_context_y_length_calculate(Esvg_Element_Context *t, Esvg_Length *l)
{

}

double esvg_element_context_other_length_calculate(Esvg_Element_Context *t, Esvg_Length *l)
{

}
#endif

void esvg_element_context_dump(const Esvg_Element_Context *c)
{
	DBG("dpi %g %g", c->dpi_x, c->dpi_y);
	DBG("viewbox %g %g %g %g", c->viewbox.x, c->viewbox.y, c->viewbox.w, c->viewbox.h);
	DBG("transformation %" ENESIM_MATRIX_FORMAT, ENESIM_MATRIX_ARGS (&c->transform));
}

Edom_Tag * esvg_element_new(Esvg_Element_Descriptor *descriptor, Esvg_Type type,
		void *data)
{
	Esvg_Element *thiz;
	Edom_Tag_Descriptor pdescriptor;
	Edom_Tag *t;

	thiz = calloc(1, sizeof(Esvg_Element));
	if (!thiz) return NULL;

	EINA_MAGIC_SET(thiz, ESVG_ELEMENT_MAGIC);
	thiz->data = data;
	thiz->type = type;
	enesim_matrix_identity(&thiz->transform.base.v);
	enesim_matrix_identity(&thiz->transform.anim.v);

	/* the tag interface */
	pdescriptor.name_get = _esvg_element_name_get;
	pdescriptor.topmost_get = _esvg_element_tag_topmost_get;
	pdescriptor.child_add = _esvg_element_child_add;
	pdescriptor.child_remove = _esvg_element_child_remove;
	pdescriptor.attribute_set = _esvg_element_attribute_set;
	pdescriptor.attribute_get = _esvg_element_attribute_get;
	pdescriptor.cdata_set = descriptor->cdata_set;
	pdescriptor.text_set = descriptor->text_set;
	pdescriptor.text_get = descriptor->text_get;
	pdescriptor.free = _esvg_element_free;

	/* our own interface */
	thiz->descriptor.initialize = descriptor->initialize;
	thiz->descriptor.setup = descriptor->setup;
	thiz->descriptor.attribute_set = descriptor->attribute_set;
	thiz->descriptor.attribute_get = descriptor->attribute_get;
	thiz->descriptor.free = descriptor->free;
	thiz->descriptor.attribute_animated_fetch = descriptor->attribute_animated_fetch;
	thiz->descriptor.child_add = descriptor->child_add;
	thiz->descriptor.child_remove = descriptor->child_remove;

	t = edom_tag_new(&pdescriptor, thiz);

	_esvg_element_attribute_presentation_setup(&thiz->attr_xml);
	_esvg_element_attribute_presentation_setup(&thiz->attr_css);
	esvg_element_attribute_type_set(t, ESVG_ATTR_XML);

	return t;
}

void esvg_element_init(void)
{
	_esvg_element_log = eina_log_domain_register("esvg_element", ESVG_LOG_COLOR_DEFAULT);
	if (_esvg_element_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_esvg_element_init();
}

void esvg_element_shutdown(void)
{
	if (_esvg_element_log < 0)
		return;
	_esvg_element_shutdown();
	eina_log_domain_unregister(_esvg_element_log);
	_esvg_element_log = -1;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool esvg_is_element(Ender_Element *e)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	return esvg_is_element_internal(t);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Esvg_Type esvg_element_type_get(Ender_Element *e)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	return esvg_element_internal_type_get(t);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI const char * esvg_element_name_get(Ender_Element *e)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	return edom_tag_name_get(t);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_id_get(Ender_Element *e, const char **id)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	/* FIXME use the internal variant and return the char not use an in/out param */
	_esvg_element_id_get(t, id);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_id_set(Ender_Element *e, const char *id)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_ID, id, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_class_set(Ender_Element *e, const char *class)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_CLASS, class, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_transform_set(Ender_Element *e, const Enesim_Matrix *transform)
{
	Esvg_Matrix_Animated a;

	if (!transform)
	{
		ender_element_property_value_set(e, ESVG_ELEMENT_TRANSFORM, NULL, NULL);
		return;
	}
	a.base = *transform;
	ender_element_property_value_set(e, ESVG_ELEMENT_TRANSFORM, &a, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_transform_get(Ender_Element *e, Enesim_Matrix *transform)
{
	Esvg_Matrix_Animated a;
	Edom_Tag *t;

	if (!transform) return;
	t = ender_element_object_get(e);
	_esvg_element_transform_get(t, &a);
	*transform = a.base;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_style_set(Ender_Element *e, const char *style)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_STYLE, style, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_style_get(Ender_Element *e, const char **style)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool esvg_element_style_is_set(Ender_Element *e)
{
	return EINA_FALSE;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_clip_path_set(Ender_Element *e, Esvg_Clip_Path *clip_path)
{
	esvg_element_property_clip_path_set(e, ESVG_ELEMENT_CLIP_PATH, clip_path);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_clip_path_unset(Ender_Element *e)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_opacity_set(Ender_Element *e, const Esvg_Number *v)
{
	esvg_element_property_number_set(e, ESVG_ELEMENT_OPACITY, v);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_opacity_get(Ender_Element *e, Esvg_Number *v)
{
	Esvg_Number_Animated a;
	Edom_Tag *t;

	if (!v) return;

	t = ender_element_object_get(e);
	_esvg_element_opacity_get(t, &a);
	*v = a.base;
}


/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_opacity_unset(Ender_Element *e)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_color_set(Ender_Element *e, const Esvg_Color *color)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_COLOR, color, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_color_get(Ender_Element *e, Esvg_Color *color)
{
	Esvg_Color_Animated a;
	Edom_Tag *t;

	if (!color) return;
	t = ender_element_object_get(e);
	_esvg_element_color_get(t, &a);
	*color = a.base;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_color_unset(Ender_Element *e)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_fill_set(Ender_Element *e, const Esvg_Paint *fill)
{
	esvg_element_property_paint_set(e, ESVG_ELEMENT_FILL, fill);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_fill_unset(Ender_Element *e)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_fill_opacity_set(Ender_Element *e, const Esvg_Number *v)
{
	esvg_element_property_number_set(e, ESVG_ELEMENT_FILL_OPACITY, v);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_fill_opacity_get(Ender_Element *e, Esvg_Number *v)
{
}


/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_fill_opacity_unset(Ender_Element *e)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_fill_rule_set(Ender_Element *e, Esvg_Fill_Rule fill_rule)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_fill_rule_unset(Ender_Element *e)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_stroke_set(Ender_Element *e, const Esvg_Paint *stroke)
{
	esvg_element_property_paint_set(e, ESVG_ELEMENT_STROKE, stroke);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_stroke_unset(Ender_Element *e)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_stroke_width_set(Ender_Element *e, const Esvg_Length *stroke_width)
{
	esvg_element_property_length_set(e, ESVG_ELEMENT_STROKE_WIDTH, stroke_width);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_stroke_width_unset(Ender_Element *e)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_stroke_opacity_set(Ender_Element *e, const Esvg_Number *v)
{
	esvg_element_property_number_set(e, ESVG_ELEMENT_STROKE_OPACITY, v);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_stroke_opacity_get(Ender_Element *e, Esvg_Number *v)
{
	Esvg_Number_Animated a;
	Edom_Tag *t;

	if (!v) return;

	t = ender_element_object_get(e);
	_esvg_element_stroke_opacity_get(t, &a);
	*v = a.base;
}


/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_stroke_opacity_unset(Ender_Element *e)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_stroke_line_cap_set(Ender_Element *e, Esvg_Stroke_Line_Cap cap)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_stroke_line_cap_unset(Ender_Element *e)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_stroke_line_join_set(Ender_Element *e, Esvg_Stroke_Line_Join join)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_stroke_line_join_unset(Ender_Element *e)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_visibility_set(Ender_Element *e, Esvg_Visibility visibility)
{
	esvg_element_property_enum_set(e, ESVG_ELEMENT_VISIBILITY, visibility);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_visibility_unset(Ender_Element *e)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_display_set(Ender_Element *e, Esvg_Display display)
{
	esvg_element_property_enum_set(e, ESVG_ELEMENT_DISPLAY, display);
}


/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_stop_opacity_set(Ender_Element *e, const Esvg_Number *v)
{
	esvg_element_property_number_set(e, ESVG_ELEMENT_STOP_OPACITY, v);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_stop_opacity_get(Ender_Element *e, Esvg_Number *v)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_stop_color_set(Ender_Element *e, Esvg_Color *stop_color)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_STOP_COLOR, stop_color, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_stop_color_get(Ender_Element *e, Esvg_Color *stop_color)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_onfocusin_set(Ender_Element *e, const char *v)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_ONFOCUSIN, v, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_onfocusout_set(Ender_Element *e, const char *v)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_ONFOCUSOUT, v, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_onactivate_set(Ender_Element *e, const char *v)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_ONACTIVATE, v, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_onclick_set(Ender_Element *e, const char *v)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_ONCLICK, v, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_onmousedown_set(Ender_Element *e, const char *v)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_ONMOUSEDOWN, v, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_onmouseup_set(Ender_Element *e, const char *v)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_ONMOUSEUP, v, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_onmouseover_set(Ender_Element *e, const char *v)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_ONMOUSEOVER, v, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_onmousemove_set(Ender_Element *e, const char *v)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_ONMOUSEMOVE, v, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_onmouseout_set(Ender_Element *e, const char *v)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_ONMOUSEOUT, v, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Element * esvg_element_topmost_get(Ender_Element *e)
{
	Edom_Tag *t;
	Ender_Element *topmost;

	t = ender_element_object_get(e);
	esvg_element_internal_topmost_get(t, &topmost);
	return topmost;
}
