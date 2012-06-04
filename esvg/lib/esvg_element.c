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
	Esvg_Element_Context state;
	Esvg_Attribute_Presentation attr_xml;
	Esvg_Attribute_Presentation attr_css;
	/* the descriptor interface */
	Esvg_Element_Descriptor_Internal descriptor;
	/* private */
	Esvg_Attribute_Type current_attr_type;
	Eina_Bool current_attr_animate;
	Esvg_Attribute_Presentation *current_attr;
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
static void _esvg_element_state_compose(const Esvg_Element_Context *s,
		const Esvg_Element_Context *parent, Esvg_Element_Context *d)
{
	/* only set */
	d->dpi_x = parent->dpi_x;
	d->dpi_y = parent->dpi_y;
	d->viewbox = parent->viewbox;
	d->bounds = parent->bounds;
	d->font_size = parent->font_size;
	d->renderable_behaviour = parent->renderable_behaviour;
	/* actually compose */
	enesim_matrix_compose(&parent->transform.base, &s->transform.base, &d->transform.base);
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

	thiz = _esvg_element_get(t);
	if (!transform) return;

	thiz->state.transform = *transform;
}

static void _esvg_element_transform_get(Edom_Tag *t, Esvg_Animated_Transform *transform)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	if (transform) *transform = thiz->state.transform;
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

static void _esvg_element_clip_path_set(Edom_Tag *t, const char *clip_path)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_presentation_clip_path_set(thiz->current_attr, clip_path);
}

static void _esvg_element_clip_path_get(Edom_Tag *t, const char **clip_path)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
}

static void _esvg_element_clip_path_unset(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_presentation_clip_path_unset(thiz->current_attr);
}

static void _esvg_element_opacity_set(Edom_Tag *t, Esvg_Animated_Number *opacity)
{
	Esvg_Element *thiz;
	thiz = _esvg_element_get(t);

	/* FIXME only set the correct attr, or anim or base */
	esvg_attribute_presentation_opacity_set(thiz->current_attr, opacity);
}

static void _esvg_element_opacity_get(Edom_Tag *t, Esvg_Animated_Number *opacity)
{
	Esvg_Element *thiz;
	thiz = _esvg_element_get(t);
	*opacity = thiz->current_attr->opacity;
}

static void _esvg_element_opacity_unset(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_presentation_opacity_unset(thiz->current_attr);
}

static void _esvg_element_color_set(Edom_Tag *t, const Esvg_Color *color)
{
	Esvg_Element *thiz;
	thiz = _esvg_element_get(t);

	esvg_attribute_presentation_color_set(thiz->current_attr, color);
}

static void _esvg_element_color_unset(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_presentation_color_unset(thiz->current_attr);
}

static void _esvg_element_color_get(Edom_Tag *t, Esvg_Color *color)
{
	Esvg_Element *thiz;
	thiz = _esvg_element_get(t);

	*color = thiz->current_attr->color;
	printf("getting color! %d %d %d\n", color->r, color->g, color->b);
}

static void _esvg_element_fill_set(Edom_Tag *t, const Esvg_Paint *fill)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_presentation_fill_set(thiz->current_attr, fill);
}

static void _esvg_element_fill_get(Edom_Tag *t, Esvg_Paint *fill)
{
	Esvg_Element *thiz;

	if (!fill) return;
	thiz = _esvg_element_get(t);
	*fill = thiz->current_attr->fill;
}

static void _esvg_element_fill_unset(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_presentation_fill_unset(thiz->current_attr);
}

static void _esvg_element_fill_opacity_set(Edom_Tag *t, double fill_opacity)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_presentation_fill_opacity_set(thiz->current_attr, fill_opacity);
}

static void _esvg_element_fill_opacity_get(Edom_Tag *t, double *fill_opacity)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	*fill_opacity = thiz->current_attr->fill_opacity;
}

static void _esvg_element_fill_opacity_unset(Edom_Tag *t)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_presentation_fill_opacity_unset(thiz->current_attr);
}

static void _esvg_element_fill_rule_set(Edom_Tag *t, Esvg_Fill_Rule fill_rule)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_presentation_fill_rule_set(thiz->current_attr, fill_rule);
}

static void _esvg_element_stroke_set(Edom_Tag *t, const Esvg_Paint *stroke)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_presentation_stroke_set(thiz->current_attr, stroke);
}

static void _esvg_element_stroke_get(Edom_Tag *t, Esvg_Paint *stroke)
{
	Esvg_Element *thiz;

	if (!stroke) return;
	thiz = _esvg_element_get(t);
	*stroke = thiz->current_attr->stroke;
}

static void _esvg_element_stroke_width_set(Edom_Tag *t, const Esvg_Length *stroke_width)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_presentation_stroke_width_set(thiz->current_attr, stroke_width);
}

static void _esvg_element_stroke_width_get(Edom_Tag *t, Esvg_Length *stroke_width)
{
	Esvg_Element *thiz;

	if (!stroke_width) return;

	thiz = _esvg_element_get(t);
	*stroke_width = thiz->current_attr->stroke_width;
}

static void _esvg_element_stroke_opacity_set(Edom_Tag *t, double stroke_opacity)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_presentation_stroke_opacity_set(thiz->current_attr, stroke_opacity);
}

static void _esvg_element_stroke_opacity_get(Edom_Tag *t, double *stroke_opacity)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	*stroke_opacity = thiz->current_attr->stroke_opacity;
}

static void _esvg_element_stroke_line_cap_set(Edom_Tag *t, Esvg_Stroke_Line_Cap cap)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_presentation_stroke_line_cap_set(thiz->current_attr, cap);
}

static void _esvg_element_stroke_line_join_set(Edom_Tag *t, Esvg_Stroke_Line_Join join)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_presentation_stroke_line_join_set(thiz->current_attr, join);
}

static void _esvg_element_stop_color_set(Edom_Tag *t, Esvg_Color *stop_color)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_presentation_stop_color_set(thiz->current_attr, stop_color);
}

static void _esvg_element_stop_color_get(Edom_Tag *t, Esvg_Color *stop_color)
{
	Esvg_Element *thiz;

	if (!stop_color) return;
	thiz = _esvg_element_get(t);
	*stop_color = thiz->current_attr->stop_color;
}

static void _esvg_element_stop_opacity_set(Edom_Tag *t, double stop_opacity)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_presentation_stop_opacity_set(thiz->current_attr, stop_opacity);
}

static void _esvg_element_stop_opacity_get(Edom_Tag *t, double *stop_opacity)
{
	Esvg_Element *thiz;

	if (!stop_opacity) return;
	thiz = _esvg_element_get(t);
	*stop_opacity = thiz->current_attr->stop_opacity;
}

static void _esvg_element_visibility_set(Edom_Tag *t, Eina_Bool visibility)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(t);
	esvg_attribute_presentation_visibility_set(thiz->current_attr, visibility);
}

static void _esvg_element_visibility_get(Edom_Tag *t, Eina_Bool *visibility)
{
	Esvg_Element *thiz;

	if (!visibility) return;

	thiz = _esvg_element_get(t);
	*visibility = thiz->current_attr->visibility;
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
	if (thiz->descriptor.free)
		return thiz->descriptor.free(t);
	esvg_attribute_presentation_cleanup(&thiz->attr_xml);
	esvg_attribute_presentation_cleanup(&thiz->attr_css);
	esvg_attribute_presentation_cleanup(&thiz->attr_final);
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
		printf("already run\n");
		return EINA_TRUE;
	}
#endif

	thiz->last_run = c->run;

	/* the idea here is to call the setup interface of the element */
	/* note that on SVG every element must be part of a topmost SVG
	 * that way we need to always pass the upper svg/g element of this
	 * so relative properties can be calcualted correctly */
	/* TODO apply the style first */
	thiz->attr_final = thiz->attr_xml;
	thiz->state_final = thiz->state;
	/* FIXME avoid so many copies */
	if (rel_state)
	{
		_esvg_element_state_compose(&thiz->state, rel_state, &thiz->state_final);
	}


	/* TODO In theory it should be */
	/* first merge the css rel_attr with the xml rel_attr */
	/* then apply the style if present */
	/* add an ATTR_FINAL or something like that, so the inline style is applied there */
	/* is the inline style inherited on child elements ? yes! */

	/* FIXME check that the style has changed, if so revert it and start applying */
	/* FIXME should it have more priority than the properties? */
	if (thiz->style || rel_attr)
	{
		if (thiz->style)
		{
			/* swap where the css atrtibutes will be written before applying the style */
			esvg_element_attribute_type_set(t, ESVG_ATTR_CSS);
			ecss_context_inline_style_apply(&_esvg_element_css_context, thiz->style, t);
			esvg_element_attribute_type_set(t, ESVG_ATTR_XML);
			esvg_attribute_presentation_merge(&thiz->attr_css, &thiz->attr_xml, &thiz->attr_final);
			if (rel_attr)
			{
				esvg_attribute_presentation_merge(&thiz->attr_final, rel_attr, &thiz->attr_final);
			}
		}
		else
		{
			if (rel_attr)
			{
				esvg_attribute_presentation_merge(&thiz->attr_xml, rel_attr, &thiz->attr_final);
			}
		}
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
	printf("dpi %g %g\n", c->dpi_x, c->dpi_y);
	printf("viewbox %g %g %g %g\n", c->viewbox.min_x, c->viewbox.min_y, c->viewbox.width, c->viewbox.height);
	printf("transformation %" ENESIM_MATRIX_FORMAT "\n", ENESIM_MATRIX_ARGS (&c->transform.base));
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
	enesim_matrix_identity(&thiz->state.transform.base);

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

	t = edom_tag_new(&pdescriptor, thiz);

	esvg_attribute_presentation_setup(&thiz->attr_xml);
	esvg_attribute_presentation_setup(&thiz->attr_css);
	esvg_element_attribute_type_set(t, ESVG_ATTR_XML);

	return t;
}

/* The ender wrapper */
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
	Esvg_Animated_Transform at;

	if (!transform)
	{
		ender_element_property_value_set(e, ESVG_ELEMENT_TRANSFORM, NULL, NULL);
		return;
	}
	at.base = *transform;
	ender_element_property_value_set(e, ESVG_ELEMENT_TRANSFORM, &at, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_transform_get(Ender_Element *e, Enesim_Matrix *transform)
{
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
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_clip_path_set(Ender_Element *e, const char *id)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_CLIP_PATH, id, NULL);
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
	Esvg_Animated_Number ao;

	ao.base = opacity;
	ender_element_property_value_set(e, ESVG_ELEMENT_OPACITY, &ao, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_opacity_get(Ender_Element *e, double opacity)
{
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
	Edom_Tag *t;

	t = (Edom_Tag *)ender_element_object_get(e);
	_esvg_element_color_get(t, color);
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
	ender_element_property_value_set(e, ESVG_ELEMENT_FILL, fill, NULL);
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
	ender_element_property_value_set(e, ESVG_ELEMENT_FILL_OPACITY, fill_opacity, NULL);
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
	ender_element_property_value_set(e, ESVG_ELEMENT_STROKE, stroke, NULL);
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
	ender_element_property_value_set(e, ESVG_ELEMENT_STROKE_WIDTH, stroke_width, NULL);
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
	ender_element_property_value_set(e, ESVG_ELEMENT_STROKE_OPACITY, stroke_opacity, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_stroke_opacity_get(Ender_Element *e, double *stroke_opacity)
{
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
	ender_element_property_value_set(e, ESVG_ELEMENT_STOP_OPACITY, stop_opacity, NULL);
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
