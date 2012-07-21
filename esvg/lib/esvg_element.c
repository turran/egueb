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

#include "esvg_main.h"
#include "esvg_element.h"
/*
 * TODO
 * use pointers to determine the state/final/whatever states, that
 * should be swapped on setup/cleanup
 * handle the following attributes and pass them to the parser context
 * onfocusin
 * onfocusout
 * onactivate
 * onclick
 * onmousedown
 * onmouseup
 * onmouseover
 * onmousemove
 * onmouseout
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
#define ESVG_LOG_DEFAULT esvg_log_element

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
static Ender_Property *ESVG_ELEMENT_TOPMOST;

typedef struct _Esvg_Element_Descriptor_Internal
{
	Esvg_Element_Initialize initialize;
	Esvg_Element_Attribute_Set attribute_set;
	Esvg_Element_Attribute_Animation_Add attribute_animation_add;
	Esvg_Element_Attribute_Animation_Remove attribute_animation_remove;
	Edom_Tag_Attribute_Get attribute_get;
	Edom_Tag_Free free;
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

typedef struct _Esvg_Element_State
{
	char *style;
} Esvg_Element_State;

typedef struct _Esvg_Element_Attributes
{
	Esvg_Attribute_Animated_String clip_path;
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
	/* FIXME do we really need this? */
	int sets;
	/* has something changed ? */
	Eina_Bool changed;
} Esvg_Element_Attributes;

typedef struct _Esvg_Element
{
	EINA_MAGIC
	Esvg_Type type;
	/* properties like the id, whatever */
	Esvg_Element_State current;
	Esvg_Element_State old;
	char *style;
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
/*----------------------------------------------------------------------------*
 *                              Context helpers                               *
 *----------------------------------------------------------------------------*/
static void _esvg_element_state_compose(Esvg_Element *thiz,
		const Esvg_Element_Context *parent, Esvg_Element_Context *d)
{
	Enesim_Matrix *m;

	if (thiz->transform.animated)
		m = &thiz->transform.anim.v;
	else
		m = &thiz->transform.base.v;

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
		enesim_matrix_compose(&parent->transform, m, &d->transform);
	}
	else
	{
		d->transform = *m;
	}
}	
/*----------------------------------------------------------------------------*
 *                            Attribute helpers                               *
 *----------------------------------------------------------------------------*/
/* initialize the presentation attributes with default values */
static void _esvg_element_attribute_presentation_setup(Esvg_Element_Attributes *a)
{
	Esvg_Color black = { 0, 0, 0 };
	Esvg_Length one = { ESVG_UNIT_LENGTH_PX, 1 };
	Esvg_Paint color = { ESVG_PAINT_COLOR, { black } };
	Esvg_Paint none = { ESVG_PAINT_NONE };

	/* now the default values */
	esvg_attribute_color_unset(&a->color.base, &black);
	esvg_attribute_length_unset(&a->stroke_width.base, &one);
	esvg_attribute_number_unset(&a->stroke_opacity.base, 1.0);
	esvg_attribute_number_unset(&a->fill_opacity.base, 1.0);
	esvg_attribute_number_unset(&a->opacity.base, 1.0);
	esvg_attribute_enum_unset(&a->fill_rule.base, ESVG_NON_ZERO);
	esvg_attribute_paint_unset(&a->fill.base, &color);
	esvg_attribute_paint_unset(&a->stroke.base, &none);
	esvg_attribute_enum_unset(&a->stroke_line_cap.base, ESVG_LINE_CAP_BUTT);
	esvg_attribute_enum_unset(&a->stroke_line_join.base, ESVG_LINE_JOIN_MITER);
	esvg_attribute_number_unset(&a->stop_opacity.base, 1.0);
}

static void _esvg_element_attribute_presentation_merge(
	const Esvg_Element_Attributes *s,
	Esvg_Attribute_Presentation *d)
{
	/* clip path */
	esvg_attribute_animated_string_merge(&s->clip_path, &d->clip_path);
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
	//esvg_attribute_animated_bool_merge(&s->visibility, &d->visibility);
	/* stop opacity */
	esvg_attribute_animated_number_merge(&s->stop_opacity, &d->stop_opacity);
	/* stop color */
	esvg_attribute_animated_color_merge(&s->stop_color, &d->stop_color);
	
}

static void _esvg_element_attribute_presentation_merge_rel(
	const Esvg_Element_Attributes *rel,
	const Esvg_Element_Attributes *s,
	Esvg_Attribute_Presentation *d)
{
	/* clip path */
	esvg_attribute_animated_string_merge_rel(&rel->clip_path, &s->clip_path, &d->clip_path);
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
	//esvg_attribute_animated_bool_merge_rel(&rel->visibility, &s->visibility, &d->visibility);
	/* stop opacity */
	esvg_attribute_animated_number_merge_rel(&rel->stop_opacity, &s->stop_opacity, &d->stop_opacity);
	/* stop color */
	esvg_attribute_animated_color_merge_rel(&rel->stop_color, &s->stop_color, &d->stop_color);
}
/*----------------------------------------------------------------------------*
 *                               Setup helpers                                *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_element_child_setup_cb(Edom_Tag *t, Edom_Tag *child,
		void *data)
{
	Esvg_Element_Setup_Data *setup_data = data;
	Esvg_Element_Setup_Return ret;

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
static Eina_Bool _esvg_element_attribute_animation_add(Esvg_Element *thiz, const char *attr)
{
	int *animated = NULL;

	/* get our own attributes */
	if (strcmp(attr, "transform") == 0)
	{
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
	else
		return EINA_FALSE;
	if (animated)
		*animated++;
	return EINA_TRUE;
}

static Eina_Bool _esvg_element_attribute_animation_remove(Esvg_Element *thiz, const char *attr)
{
	int *animated = NULL;

	/* get our own attributes */
	if (strcmp(attr, "transform") == 0)
	{
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
	else
		return EINA_FALSE;
	if (animated)
		*animated--;
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

static void _esvg_element_transform_set(Edom_Tag *t, const Esvg_Animated_Transform *transform)
{
	Esvg_Element *thiz;
	Enesim_Matrix m;

	thiz = _esvg_element_get(t);
	enesim_matrix_identity(&m);
	esvg_attribute_animated_transform_set(&thiz->transform,
		transform, &m, thiz->current_attr_animate);
}

static void _esvg_element_transform_get(Edom_Tag *t, Esvg_Animated_Transform *transform)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_transform_get(&thiz->transform, transform);
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
	/* TODO handle the style
	 * the idea here is that we should parse the style attribute with ecss
	 * and apply each property/value
	 */
	if (thiz->style)
		free(thiz->style);
	if (style)
		thiz->style = strdup(style);
}

static void _esvg_element_style_get(Edom_Tag *t, const char **style)
{
	Esvg_Element *thiz;

	if (!style) return;
	thiz = _esvg_element_get(t);
	*style = thiz->style;
}

/* presentation attributes */
static void _esvg_element_clip_path_set(Edom_Tag *t, Esvg_Animated_String *clip_path)
{
	Esvg_Element *thiz;
	Esvg_Attribute_String *a;
	const char *v;

	thiz = _esvg_element_get(t);
	/* get the attribute to change */
	if (thiz->current_attr_animate)
		a = &thiz->current_attr->clip_path.anim;
	else
		a = &thiz->current_attr->clip_path.base;
	/* get the value to set */
	if (clip_path)
		v = clip_path->base;
	else
		v = NULL;

	esvg_attribute_string_set(a, v);
}

static void _esvg_element_clip_path_get(Edom_Tag *t, Esvg_Animated_String *clip_path)
{
	Esvg_Element *thiz;

	if (!clip_path) return;
	thiz = _esvg_element_get(t);
	clip_path->base = thiz->current_attr->clip_path.base.v;
	clip_path->anim = thiz->current_attr->clip_path.anim.v;
}

static void _esvg_element_clip_path_unset(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
}

static void _esvg_element_opacity_set(Edom_Tag *t, Esvg_Animated_Number *opacity)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_number_set(&thiz->current_attr->opacity,
		opacity, 1.0, thiz->current_attr_animate);
}

static void _esvg_element_opacity_get(Edom_Tag *t, Esvg_Animated_Number *opacity)
{
	Esvg_Element *thiz;
	Esvg_Attribute_Animated_Number *a;

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_number_get(&thiz->current_attr->opacity,
		opacity);
}

static void _esvg_element_opacity_unset(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
}

static void _esvg_element_color_set(Edom_Tag *t, const Esvg_Animated_Color *color)
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

static void _esvg_element_color_get(Edom_Tag *t, Esvg_Animated_Color *color)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_color_get(&thiz->current_attr->color,
		color);
}

static void _esvg_element_fill_set(Edom_Tag *t, const Esvg_Animated_Paint *fill)
{
	Esvg_Element *thiz;
	Esvg_Color black = { 0, 0, 0 };
	Esvg_Paint def = { ESVG_PAINT_COLOR, { black } };

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_paint_set(&thiz->current_attr->fill,
		fill, &def, thiz->current_attr_animate);
}

static void _esvg_element_fill_get(Edom_Tag *t, Esvg_Animated_Paint *fill)
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

static void _esvg_element_fill_opacity_set(Edom_Tag *t, Esvg_Animated_Number *fill_opacity)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_number_set(&thiz->current_attr->fill_opacity,
		fill_opacity, 1.0, thiz->current_attr_animate);
}

static void _esvg_element_fill_opacity_get(Edom_Tag *t, Esvg_Animated_Number *fill_opacity)
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

static void _esvg_element_stroke_set(Edom_Tag *t, const Esvg_Animated_Paint *stroke)
{
	Esvg_Element *thiz;
	Esvg_Paint def = { ESVG_PAINT_NONE };

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_paint_set(&thiz->current_attr->stroke,
		stroke, &def, thiz->current_attr_animate);
}

static void _esvg_element_stroke_get(Edom_Tag *t, Esvg_Animated_Paint *stroke)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_paint_get(&thiz->current_attr->stroke,
		stroke);
}

static void _esvg_element_stroke_width_set(Edom_Tag *t, const Esvg_Animated_Length *stroke_width)
{
	Esvg_Element *thiz;
	Esvg_Length def = { ESVG_UNIT_LENGTH_PX, 1 };

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_length_set(&thiz->current_attr->stroke_width,
		stroke_width, &def, thiz->current_attr_animate);
}

static void _esvg_element_stroke_width_get(Edom_Tag *t, Esvg_Animated_Length *stroke_width)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_length_get(&thiz->current_attr->stroke_width,
		stroke_width);
}

static void _esvg_element_stroke_opacity_set(Edom_Tag *t, Esvg_Animated_Number *stroke_opacity)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_number_set(&thiz->current_attr->stroke_opacity,
		stroke_opacity, 1.0, thiz->current_attr_animate);
}

static void _esvg_element_stroke_opacity_get(Edom_Tag *t, Esvg_Animated_Number *stroke_opacity)
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

static void _esvg_element_stop_color_set(Edom_Tag *t, Esvg_Animated_Color *stop_color)
{
	Esvg_Element *thiz;
	const Esvg_Color def = { 0, 0, 0};

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_color_set(&thiz->current_attr->stop_color,
		stop_color, &def, thiz->current_attr_animate);
}

static void _esvg_element_stop_color_get(Edom_Tag *t, Esvg_Animated_Color *stop_color)
{
	Esvg_Element *thiz;

	if (!stop_color) return;
	thiz = _esvg_element_get(t);
	esvg_attribute_animated_color_get(&thiz->current_attr->stop_color,
		stop_color);
}

static void _esvg_element_stop_opacity_set(Edom_Tag *t, Esvg_Animated_Number *stop_opacity)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_number_set(&thiz->current_attr->stop_opacity,
		stop_opacity, 1.0, thiz->current_attr_animate);
}

static void _esvg_element_stop_opacity_get(Edom_Tag *t, Esvg_Animated_Number *stop_opacity)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_animated_number_get(&thiz->current_attr->stop_opacity,
		stop_opacity);
}

static void _esvg_element_visibility_set(Edom_Tag *t, Esvg_Animated_Bool *visibility)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	//esvg_attribute_animated_bool_set(&thiz->current_attr->visibility,
	//	visibility, EINA_TRUE, thiz->current_attr_animate);
}

static void _esvg_element_visibility_get(Edom_Tag *t, Esvg_Animated_Bool *visibility)
{
	Esvg_Element *thiz;

	if (!visibility) return;

	thiz = _esvg_element_get(t);
}
/*----------------------------------------------------------------------------*
 *                           The Edom Tag interface                           *
 *----------------------------------------------------------------------------*/
const char * _esvg_element_name_get(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	return esvg_type_string_to(thiz->type);
}

Edom_Tag * _esvg_element_topmost_get(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	if (!thiz->topmost) return NULL;

	return ender_element_object_get(thiz->topmost);
}

static Eina_Bool _esvg_element_attribute_set(Edom_Tag *t, const char *key, const char *value)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
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
		esvg_element_clip_path_set(thiz->e, value);
	}
	else if (strcmp(key, "opacity") == 0)
	{
		double opacity = esvg_number_string_from(value, 1.0);
		esvg_element_opacity_set(thiz->e, opacity);
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
		double fill_opacity = esvg_number_string_from(value, 1.0);
		esvg_element_fill_opacity_set(thiz->e, fill_opacity);
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
		double stroke_opacity = esvg_number_string_from(value, 1.0);
		esvg_element_stroke_opacity_set(thiz->e, stroke_opacity);
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
		double stop_opacity = esvg_number_string_from(value, 1.0);
		esvg_element_stop_opacity_set(thiz->e, stop_opacity);
	}
	/* TODO in theory we should not allow css attributes to continue */
	else
	{
		if (thiz->descriptor.attribute_set)
			return thiz->descriptor.attribute_set(thiz->e, key, value);
	}

	return EINA_TRUE;
}

static Eina_Bool _esvg_element_attribute_get(Edom_Tag *t, const char *key, char **value)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
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
		if (thiz->descriptor.attribute_get)
			return thiz->descriptor.attribute_get(t, key, value);
	}
	return EINA_TRUE;
}

static void _esvg_element_free(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	/* remove every child object */
	edom_tag_child_foreach(t, _esvg_element_child_free_cb, NULL);
	//esvg_attribute_presentation_cleanup(&thiz->attr_xml);
	//esvg_attribute_presentation_cleanup(&thiz->attr_css);
	esvg_attribute_presentation_cleanup(&thiz->attr_final);
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
Eina_Bool esvg_element_attribute_animation_add(Edom_Tag *t, const char *attr)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	/* get our own attributes */
	DBG("adding animation on %s", attr);
	if (_esvg_element_attribute_animation_add(thiz, attr))
		return EINA_TRUE;
	/* call the descriptor implementation */
	if (thiz->descriptor.attribute_animation_add)
		return thiz->descriptor.attribute_animation_add(t, attr);
	return EINA_FALSE;
}

void esvg_element_attribute_animation_remove(Edom_Tag *t, const char *attr)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	/* get our own attributes */
	DBG("removing animation on %s", attr);
	if (_esvg_element_attribute_animation_remove(thiz, attr))
		return;
	/* call the descriptor implementation */
	if (thiz->descriptor.attribute_animation_remove)
		thiz->descriptor.attribute_animation_remove(t, attr);
}

Esvg_Type esvg_element_internal_type_get(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	return thiz->type;
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
		Esvg_Element_Setup_Interceptor post,
		void *data)
{
	Esvg_Element_Setup_Data setup_data;

	setup_data.c = c;
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

	/* TODO apply the style first */
	/* FIXME check that the style has changed, if so revert it and start applying */
	/* FIXME should it have more priority than the properties? */
	if (thiz->style)
	{
		esvg_element_attribute_type_set(t, ESVG_ATTR_CSS);
		ecss_context_inline_style_apply(&_esvg_element_css_context, thiz->style, t);
		esvg_element_attribute_type_set(t, ESVG_ATTR_XML);
		/* merge the css and the xml into the final */
		_esvg_element_attribute_presentation_merge_rel(&thiz->attr_css, &thiz->attr_xml, &thiz->attr_final);
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
	DBG("viewbox %g %g %g %g", c->viewbox.min_x, c->viewbox.min_y, c->viewbox.width, c->viewbox.height);
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
	pdescriptor.topmost_get = _esvg_element_topmost_get;
	pdescriptor.child_add = descriptor->child_add;
	pdescriptor.child_remove = descriptor->child_remove;
	pdescriptor.attribute_set = _esvg_element_attribute_set;
	pdescriptor.attribute_get = _esvg_element_attribute_get;
	pdescriptor.cdata_set = descriptor->cdata_set;
	pdescriptor.text_set = descriptor->text_set;
	pdescriptor.free = _esvg_element_free;

	/* our own interface */
	thiz->descriptor.initialize = descriptor->initialize;
	thiz->descriptor.setup = descriptor->setup;
	thiz->descriptor.attribute_set = descriptor->attribute_set;
	thiz->descriptor.attribute_get = descriptor->attribute_get;
	thiz->descriptor.free = descriptor->free;

	t = edom_tag_new(&pdescriptor, thiz);

	_esvg_element_attribute_presentation_setup(&thiz->attr_xml);
	_esvg_element_attribute_presentation_setup(&thiz->attr_css);
	esvg_element_attribute_type_set(t, ESVG_ATTR_XML);

	return t;
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
#define _esvg_element_transform_is_set NULL
#include "generated/esvg_generated_element.c"

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
EAPI void esvg_element_id_get(Ender_Element *e, const char **id)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
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
	Esvg_Animated_Transform a;

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
	Esvg_Animated_Transform a;
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
EAPI void esvg_element_clip_path_set(Ender_Element *e, const char *id)
{
	Esvg_Animated_String a;

	if (!id)
	{
		ender_element_property_value_set(e, ESVG_ELEMENT_CLIP_PATH, NULL, NULL);
		return;
	}
	a.base = (char *)id;
	ender_element_property_value_set(e, ESVG_ELEMENT_CLIP_PATH, &a, NULL);
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
EAPI void esvg_element_opacity_set(Ender_Element *e, double opacity)
{
	Esvg_Animated_Number a;

	a.base = opacity;
	ender_element_property_value_set(e, ESVG_ELEMENT_OPACITY, &a, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_opacity_get(Ender_Element *e, double *opacity)
{
	Esvg_Animated_Number a;
	Edom_Tag *t;

	if (!opacity) return;

	t = ender_element_object_get(e);
	_esvg_element_opacity_get(t, &a);
	*opacity = a.base;
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
	Esvg_Animated_Color a;
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
	Esvg_Animated_Paint a;

	if (!fill)
	{
		ender_element_property_value_set(e, ESVG_ELEMENT_FILL, NULL, NULL);
		return;
	}
	a.base = *fill;
	ender_element_property_value_set(e, ESVG_ELEMENT_FILL, &a, NULL);
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
EAPI void esvg_element_fill_opacity_set(Ender_Element *e, double fill_opacity)
{
	Esvg_Animated_Number a;

	a.base = fill_opacity;
	ender_element_property_value_set(e, ESVG_ELEMENT_FILL_OPACITY, &a, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_fill_opacity_get(Ender_Element *e, double *fill_opacity)
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
	Esvg_Animated_Paint a;

	if (!stroke)
	{
		ender_element_property_value_set(e, ESVG_ELEMENT_STROKE, NULL, NULL);
		return;
	}
	a.base = *stroke;
	ender_element_property_value_set(e, ESVG_ELEMENT_STROKE, &a, NULL);
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
	Esvg_Animated_Length a;

	if (!stroke_width)
	{
		ender_element_property_value_set(e, ESVG_ELEMENT_STROKE_WIDTH, NULL, NULL);
	}
	a.base = *stroke_width;
	ender_element_property_value_set(e, ESVG_ELEMENT_STROKE_WIDTH, &a, NULL);
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
EAPI void esvg_element_stroke_opacity_set(Ender_Element *e, double stroke_opacity)
{
	Esvg_Animated_Number a;

	a.base = stroke_opacity;
	ender_element_property_value_set(e, ESVG_ELEMENT_STROKE_OPACITY, &a, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_stroke_opacity_get(Ender_Element *e, double *stroke_opacity)
{
	Esvg_Animated_Number a;
	Edom_Tag *t;

	if (!stroke_opacity) return;

	t = ender_element_object_get(e);
	_esvg_element_stroke_opacity_get(t, &a);
	*stroke_opacity = a.base;
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
EAPI void esvg_element_visibility_set(Ender_Element *e, Eina_Bool visibility)
{
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
EAPI void esvg_element_stop_opacity_set(Ender_Element *e, double stop_opacity)
{
	Esvg_Animated_Number a;

	a.base = stop_opacity;
	ender_element_property_value_set(e, ESVG_ELEMENT_STOP_OPACITY, &a, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_stop_opacity_get(Ender_Element *e, double *stop_opacity)
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
EAPI Eina_Bool esvg_element_setup(Ender_Element *e, Enesim_Error **error)
{
	Esvg_Context context;
	Edom_Tag *t;

	t = ender_element_object_get(e);
	esvg_context_init(&context);

	if (esvg_element_internal_setup(t, &context, error) == ESVG_SETUP_FAILED)
	{
		/* clean the context */
		esvg_context_shutdown(&context);
		return EINA_FALSE;
	}
	esvg_context_setup_dequeue(&context);
	return EINA_TRUE;
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
