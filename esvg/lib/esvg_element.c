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
#include "Esvg.h"
#include "esvg_private.h"
#include "esvg_values.h"
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
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_ELEMENT_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_ELEMENT_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_ELEMENT_MAGIC);\
	} while(0)

#if 0
typedef struct _Esvg_Parser_Element
{
	EINA_MAGIC
	Edom_Tag_Descriptor *descriptor;
	Enesim_Renderer *r;
	Edom_Tag *tag;
	char *fill;
	char *stroke;
	char *clip_path;
	void *data;
} Esvg_Parser_Element;

static Esvg_Parser_Element * _esvg_parser_element_get(Edom_Tag *tag)
{
	Esvg_Parser_Element *thiz;

	thiz = edom_tag_data_get(tag);
	ESVG_PARSER_ELEMENT_MAGIC_CHECK(thiz);

	return thiz;
}

static void _post_parse_fill_cb(Edom_Parser *parser, void *data)
{
	Esvg_Parser_Element *thiz = data;
	Esvg_Paint fill;
	Enesim_Renderer *r;

	r = esvg_parser_element_renderer_get(thiz->tag);
	esvg_paint_get(&fill, thiz->tag, thiz->fill);
	esvg_element_fill_set(r, &fill);
}

static void _post_parse_stroke_cb(Edom_Parser *parser, void *data)
{
	Esvg_Parser_Element *thiz = data;
	Esvg_Paint stroke;
	Enesim_Renderer *r;

	r = esvg_parser_element_renderer_get(thiz->tag);
	esvg_paint_get(&stroke, thiz->tag, thiz->stroke);
	esvg_element_stroke_set(r, &stroke);
}

static void _post_parse_clip_path_cb(Edom_Parser *parser, void *data)
{
	Esvg_Parser_Element *thiz = data;
	Edom_Tag *rel = NULL;
	Enesim_Renderer *r;
	Enesim_Renderer *r_rel;
	Eina_Bool ret;

	r = esvg_parser_element_renderer_get(thiz->tag);
	ret = esvg_uri_get(&rel, thiz->tag, thiz->clip_path);
	if (!rel) return;

	r_rel = esvg_parser_element_renderer_get(rel);
	esvg_element_clip_path_set(r, r_rel);
}

static Eina_Bool _parser_element_attribute_set(Edom_Tag *tag, const char *key, const char *value)
{
	Esvg_Parser_Element *thiz;
	Enesim_Renderer *r;

	thiz = _esvg_parser_element_get(tag);
	r = thiz->r;

	if (strcmp(key, "id") == 0)
	{
		printf("id = %s %p\n", value, tag);
		esvg_element_id_set(r, value);
		edom_tag_id_set(tag, value);
	}
	/* FIXME the class should not be here, but for now ... */
	else if (strcmp(key, "class") == 0)
	{
		edom_tag_class_set(tag, value);
	}
	else if (strcmp(key, "transform") == 0)
	{
		Enesim_Matrix matrix;

		esvg_transformation_get(&matrix, value);
		esvg_element_transform_set(r, &matrix);
	}
	else if (strcmp(key, "style") == 0)
	{
		Esvg_Attribute_Presentation attr;

		memset(&attr, 0, sizeof(Esvg_Attribute_Presentation));
		esvg_parser_style_inline_set(value, tag, &attr);
		esvg_element_style_set(r, &attr);
	}
	/* common presentation attributes */
	else if (strcmp(key, "clip-path") == 0)
	{
		Edom_Parser *parser;

		parser = edom_tag_parser_get(tag);
		thiz->clip_path = strdup(value);
		esvg_parser_post_parse_add(parser, _post_parse_clip_path_cb, thiz);
	}
	else if (strcmp(key, "opacity") == 0)
	{
		double opacity = esvg_number_get(value, 1.0);
		esvg_element_opacity_set(r, opacity);
	}
	else if (strcmp(key, "color") == 0)
	{
		Esvg_Color color;

		esvg_color_get(&color, value);
		esvg_element_color_set(r, &color);
	}
	else if (strcmp(key, "fill") == 0)
	{
		Esvg_Paint fill;

		if (!esvg_paint_get(&fill, tag, value))
		{
			/* we delay the parsing of fill for later in case
			 * the value has an uri
			 */
			if (esvg_parser_is_uri(value))
			{
				Edom_Parser *parser;

				parser = edom_tag_parser_get(tag);
				thiz->fill = strdup(value);
				esvg_parser_post_parse_add(parser, _post_parse_fill_cb, thiz);
			}
		}
		else
		{
			esvg_element_fill_set(r, &fill);
		}
	}
	else if (strcmp(key, "fill-rule") == 0)
	{
		Esvg_Fill_Rule fill_rule;

		esvg_parser_fill_rule_get(&fill_rule, value);
		esvg_element_fill_rule_set(r, fill_rule);
	}
	else if (strcmp(key, "fill-opacity") == 0)
	{
		double fill_opacity = esvg_number_get(value, 1.0);
		esvg_element_fill_opacity_set(r, fill_opacity);
	}
	else if (strcmp(key, "stroke") == 0)
	{
		Esvg_Paint stroke;

		if (!esvg_paint_get(&stroke, tag, value))
		{
			/* we delay the parsing of stroke for later in case
			 * the value has an uri
			 */
			if (esvg_parser_is_uri(value))
			{
				Edom_Parser *parser;

				parser = edom_tag_parser_get(tag);
				thiz->stroke = strdup(value);
				esvg_parser_post_parse_add(parser, _post_parse_stroke_cb, thiz);
			}
		}
		else
		{
			esvg_element_stroke_set(r, &stroke);
		}
	}
	else if (strcmp(key, "stroke-width") == 0)
	{
		Esvg_Length stroke_width;

		esvg_length_get(&stroke_width, value, ESVG_LENGTH_1);
		esvg_element_stroke_width_set(r, &stroke_width);
	}
	else if (strcmp(key, "stroke-opacity") == 0)
	{
		double stroke_opacity = esvg_number_get(value, 1.0);
		esvg_element_stroke_opacity_set(r, stroke_opacity);
	}
	else if (strcmp(key, "stroke-linecap") == 0)
	{
		Esvg_Stroke_Line_Cap stroke_line_cap;

		stroke_line_cap = esvg_stroke_line_cap_get(value);
		esvg_element_stroke_line_cap_set(r, stroke_line_cap);
	}
	else if (strcmp(key, "stroke-linejoin") == 0)
	{
		Esvg_Stroke_Line_Join stroke_line_join;

		stroke_line_join = esvg_stroke_line_join_get(value);
		esvg_element_stroke_line_join_set(r, stroke_line_join);
	}
	else if (strcmp(key, "stop-color") == 0)
	{
		Esvg_Color color;

		esvg_color_get(&color, value);
		/* TODO */
	}
	else if (strcmp(key, "stop-opacity") == 0)
	{
		double stop_opacity = esvg_number_get(value, 1.0);
		/* TODO */
	}
	else
	{
		if (thiz->descriptor)
		{
			if (thiz->descriptor->attribute_set)
				return thiz->descriptor->attribute_set(tag, key, value);
		}
	}

	return EINA_TRUE;
}

static const char * _parser_element_attribute_get(Edom_Tag *tag, const char *attribute)
{
	Esvg_Parser_Element *thiz;

	thiz = _esvg_parser_element_get(tag);
	/* FIXME handle common properties */
	if (thiz->descriptor)
	{
		if (thiz->descriptor->attribute_get)
			return thiz->descriptor->attribute_get(tag, attribute);
	}
	return NULL;
}

static const char * _parser_element_name_get(Edom_Tag *tag)
{
	Esvg_Parser_Element *thiz;

	thiz = _esvg_parser_element_get(tag);
	return thiz->descriptor->name_get(tag);
}

static Eina_Bool _parser_element_child_supported(Edom_Tag *tag, int tag_id)
{
	Esvg_Parser_Element *thiz;

	thiz = _esvg_parser_element_get(tag);
	if (thiz->descriptor->child_supported)
		return thiz->descriptor->child_supported(tag, tag_id);
	return EINA_FALSE;
}

static Eina_Bool _parser_element_child_add(Edom_Tag *tag, Edom_Tag *child)
{
	Esvg_Parser_Element *thiz;

	thiz = _esvg_parser_element_get(tag);
	if (thiz->descriptor->child_add)
		return thiz->descriptor->child_add(tag, child);
	return EINA_TRUE;
}

static Edom_Tag_Descriptor _descriptor = {
	/* .name_get 		= */ _parser_element_name_get,
	/* .attribute_set 	= */ _parser_element_attribute_set,
	/* .attribute_get 	= */ _parser_element_attribute_get,
	/* .child_supported	= */ _parser_element_child_supported,
	/* .child_add		= */ _parser_element_child_add,
	/* .child_remove	= */ NULL,
};

Edom_Tag * esvg_parser_element_new(Edom_Parser *parser,
		Edom_Tag_Descriptor *descriptor,
		Esvg_Parser_Tag_Type type,
		Enesim_Renderer *r,
		void *data)
{
	Esvg_Parser_Element *thiz;
	Edom_Tag *tag;

	thiz = calloc(1, sizeof(Esvg_Parser_Element));
	EINA_MAGIC_SET(thiz, ESVG_PARSER_ELEMENT_MAGIC);
	thiz->r = r;
	thiz->descriptor = descriptor;
	thiz->data = data;

	tag = edom_tag_new(parser, &_descriptor, type, thiz);
	thiz->tag = tag;

	return tag;
}

void * esvg_parser_element_data_get(Edom_Tag *tag)
{
	Esvg_Parser_Element *thiz;

	thiz = _esvg_parser_element_get(tag);
	return thiz->data;
}

Enesim_Renderer * esvg_parser_element_renderer_get(Edom_Tag *tag)
{
	Esvg_Parser_Element *thiz;

	thiz = _esvg_parser_element_get(tag);
	return thiz->r;
}
#endif

typedef struct _Esvg_Element
{
	EINA_MAGIC
	/* properties like the id, whatever */
	Esvg_Element_State state;
	Esvg_Attribute_Presentation attr;
	Esvg_Attribute_Presentation style;
	double container_width;
	double container_height;
	double x_dpi;
	double y_dpi;
	/* the descriptor interface */
	Enesim_Renderer_Name name_get;
	Esvg_Element_Renderer_Get renderer_get;
	Esvg_Element_Clone clone;
	Esvg_Element_Setup setup;
	Esvg_Element_Cleanup cleanup;
	Esvg_Element_Has_Changed has_changed;
	Eina_Bool is_renderable;
	/* private */
	Esvg_Attribute_Presentation *attr_p;
	Esvg_Element_State *state_p;
	Enesim_Renderer *parent;
	Enesim_Renderer *real_r;
	Esvg_Attribute_Presentation attr_final;
	Esvg_Element_State state_final;
	Eina_Bool changed : 1;
	Eina_Bool style_set : 1;
	void *data;
} Esvg_Element;

static Esvg_Element * _esvg_element_get(Edom_Tag *e)
{
	Esvg_Element *thiz;

	thiz = enesim_renderer_data_get(e);
	ESVG_ELEMENT_MAGIC_CHECK(thiz);

	return thiz;
}

static Enesim_Renderer * _esvg_element_renderer_get(Esvg_Element *thiz, Enesim_Renderer *r)
{
	if (thiz->renderer_get)
		return thiz->renderer_get(r, &thiz->state_final, &thiz->attr_final);
	return NULL;
}

static void _esvg_element_draw(Enesim_Renderer *r,
		const Enesim_Renderer_State *state,
		int x, int y, unsigned int len, void *dst)
{
	Esvg_Element *thiz;
	Enesim_Renderer *real_r;

	thiz = _esvg_element_get(r);
	/* the real renderer should be set at the setup */
	enesim_renderer_sw_draw(thiz->real_r, x, y, len, dst);
}

static void _esvg_element_state_merge(const Esvg_Attribute_Presentation *state, const Esvg_Attribute_Presentation *parent, Esvg_Attribute_Presentation *d)
{
	/* clip_path */
	if (!state->clip_path_set)
	{
		d->clip_path = parent->clip_path;
		d->clip_path_set = parent->clip_path_set;
	}
	else
	{
		d->clip_path = state->clip_path;
		d->clip_path_set = EINA_TRUE;
	}

	/* color */
	if (!state->color_set)
	{
		d->color = parent->color;
		d->color_set = parent->color_set;
	}
	else
	{
		d->color = state->color;
		d->color_set = EINA_TRUE;
	}

	/* opacity */
	if (!state->opacity_set)
	{
		d->opacity = parent->opacity;
		d->opacity_set = parent->opacity_set;
	}
	else
	{
		d->opacity = state->opacity;
		d->opacity_set = EINA_TRUE;
	}

	/* FIXME do the real merge (multiply, etc, etc) */
	/* fill */
	if (!state->fill_set)
	{
		d->fill = parent->fill;
		d->fill_set = parent->fill_set;
	}
	else
	{
		d->fill = state->fill;
		d->fill_set = EINA_TRUE;
	}

	/* fill opacity */
	if (!state->fill_opacity_set)
	{
		d->fill_opacity = parent->fill_opacity;
		d->fill_opacity_set = parent->fill_opacity_set;
	}
	else
	{
		d->fill_opacity = state->fill_opacity;
		d->fill_opacity_set = EINA_TRUE;
	}

	/* fill rule */
	if (!state->fill_rule_set)
	{
		d->fill_rule = parent->fill_rule;
		d->fill_rule_set = parent->fill_rule_set;
	}
	else
	{
		d->fill_rule = state->fill_rule;
		d->fill_rule_set = EINA_TRUE;
	}

	/* stroke */
	if (!state->stroke_set)
	{
		d->stroke = parent->stroke;
		d->stroke_set = parent->stroke_set;
	}
	else
	{
		d->stroke = state->stroke;
		d->stroke_set = EINA_TRUE;
	}

	/* stroke width */
	if (!state->stroke_width_set)
	{
		d->stroke_width = parent->stroke_width;
		d->stroke_width_set = parent->stroke_width_set;
	}
	else
	{
		d->stroke_width = state->stroke_width;
		d->stroke_width_set = EINA_TRUE;
	}

	/* stroke line cap */
	if (!state->stroke_line_cap_set)
	{
		d->stroke_line_cap = parent->stroke_line_cap;
		d->stroke_line_cap_set = parent->stroke_line_cap_set;
	}
	else
	{
		d->stroke_line_cap = state->stroke_line_cap;
		d->stroke_line_cap_set = EINA_TRUE;

	}

	/* stroke line join */
	if (!state->stroke_line_join_set)
	{
		d->stroke_line_join = parent->stroke_line_join;
		d->stroke_line_join_set = parent->stroke_line_join_set;
	}
	else
	{
		d->stroke_line_join = state->stroke_line_join;
		d->stroke_line_join_set = EINA_TRUE;

	}

	/* stroke opacity */
	if (!state->stroke_opacity_set)
	{
		d->stroke_opacity = parent->stroke_opacity;
		d->stroke_opacity_set = parent->stroke_opacity_set;
	}
	else
	{
		d->stroke_opacity = state->stroke_opacity;
		d->stroke_opacity_set = EINA_TRUE;
	}

	/* visibility */
	if (!state->visibility_set)
	{
		d->visibility = parent->visibility;
		d->visibility_set = parent->visibility_set;
	}
	else
	{
		d->visibility = state->visibility;
		d->visibility_set = EINA_TRUE;

	}
}

static void _esvg_element_state_compose(const Esvg_Element_State *s, const Esvg_Element_State *parent, Esvg_Element_State *d)
{
	/* only set */
	d->dpi_x = parent->dpi_x;
	d->dpi_y = parent->dpi_y;
	d->viewbox_w = parent->viewbox_w;
	d->viewbox_h = parent->viewbox_h;
	/* actually compose */
	enesim_matrix_compose(&parent->transform, &s->transform, &d->transform);
}
/*----------------------------------------------------------------------------*
 *                      The Enesim's renderer interface                       *
 *----------------------------------------------------------------------------*/
static const char * _esvg_element_name(Enesim_Renderer *r)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(r);
	if (thiz->name_get) return thiz->name_get(r);
	return "element";
}

static void _esvg_element_boundings(Enesim_Renderer *r,
		const Enesim_Renderer_State *states[ENESIM_RENDERER_STATES],
		Enesim_Rectangle *rect)
{
	Esvg_Element *thiz;
	Enesim_Renderer *real_r;

	thiz = _esvg_element_get(r);
	/* TODO in case we have changed call the setup again */
	real_r = _esvg_element_renderer_get(thiz, r);
	enesim_renderer_boundings(real_r, rect);
}

static void _esvg_element_destination_boundings(Enesim_Renderer *r,
		const Enesim_Renderer_State *states[ENESIM_RENDERER_STATES],
		Eina_Rectangle *rect)
{
	Esvg_Element *thiz;
	Enesim_Renderer *real_r;

	thiz = _esvg_element_get(r);
	/* TODO in case we have changed call the setup again */
	real_r = _esvg_element_renderer_get(thiz, r);
	enesim_renderer_destination_boundings(real_r, rect, 0, 0);
}

static void _esvg_element_free(Enesim_Renderer *r)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(r);
	/* TODO */
}

static Eina_Bool _esvg_element_sw_setup(Enesim_Renderer *r,
		const Enesim_Renderer_State *states[ENESIM_RENDERER_STATES],
		Enesim_Surface *s,
		Enesim_Renderer_Sw_Fill *fill, Enesim_Error **error)
{
	Esvg_Element *thiz;
	Enesim_Renderer *parent;
	Esvg_Attribute_Presentation *parent_attr = NULL;
	Esvg_Element_State *parent_state = NULL;
	Enesim_Renderer *real_r;

	thiz = _esvg_element_get(r);
	parent = thiz->parent;
	if (parent)
	{
		Esvg_Element *parent_thiz;

		parent_thiz = _esvg_element_get(parent);
		parent_attr = &parent_thiz->attr_final;
		parent_state = &parent_thiz->state_final;
	}

	esvg_element_setup(r, parent_state, parent_attr, s, error);

	real_r = _esvg_element_renderer_get(thiz, r);
	if (!real_r)
		return EINA_FALSE;

	if (!enesim_renderer_setup(real_r, s, error))
		return EINA_FALSE;

	thiz->real_r = real_r;
	*fill = _esvg_element_draw;

	return EINA_TRUE;
}

static void _esvg_element_sw_cleanup(Enesim_Renderer *r, Enesim_Surface *s)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(r);
}

static void _esvg_element_flags(Enesim_Renderer *r, const Enesim_Renderer_State *state,
		Enesim_Renderer_Flag *flags)
{
	Esvg_Element *thiz;
	Enesim_Renderer *real_r;

	thiz = _esvg_element_get(r);
	real_r = _esvg_element_renderer_get(thiz, r);
	enesim_renderer_flags(real_r, flags);
}

static void _esvg_element_hints(Enesim_Renderer *r, const Enesim_Renderer_State *state,
		Enesim_Renderer_Hint *hints)
{
	Esvg_Element *thiz;
	Enesim_Renderer *real_r;

	thiz = _esvg_element_get(r);
	real_r = _esvg_element_renderer_get(thiz, r);
	enesim_renderer_hints_get(real_r, hints);
}


static Eina_Bool _esvg_element_has_changed(Enesim_Renderer *r,
		const Enesim_Renderer_State *states[ENESIM_RENDERER_STATES])
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(r);
	if (thiz->has_changed)
		return thiz->has_changed(r);
	return EINA_FALSE;
}

static void _esvg_element_damage(Enesim_Renderer *r, Enesim_Renderer_Damage_Cb cb, void *data)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(r);
}

static Eina_Bool _esvg_element_is_inside(Enesim_Renderer *r, double x, double y)
{
	/* FIXME */
	return EINA_TRUE;
}

static Enesim_Renderer_Descriptor _descriptor = {
	/* .version =               */ ENESIM_RENDERER_API,
	/* .name =                  */ _esvg_element_name,
	/* .free =                  */ _esvg_element_free,
	/* .boundings =             */ _esvg_element_boundings,
	/* .destination_boundings = */ _esvg_element_destination_boundings,
	/* .flags =                 */ _esvg_element_flags,
	/* .hints_get =             */ _esvg_element_hints,
	/* .is_inside =             */ _esvg_element_is_inside,
	/* .damage =                */ _esvg_element_damage,
	/* .has_changed =           */ _esvg_element_has_changed,
	/* .sw_setup =              */ _esvg_element_sw_setup,
	/* .sw_cleanup =            */ _esvg_element_sw_cleanup,
	/* .ocl_setup =             */ NULL,
	/* .ocl_kernel_cleanup =    */ NULL,
	/* .ocl_cleanup =           */ NULL
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void * esvg_element_data_get(Edom_Tag *e)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(r);
	return thiz->data;
}

Enesim_Renderer * esvg_element_new(Esvg_Element_Descriptor *descriptor, void *data)
{
	Esvg_Element *thiz;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Element));
	if (!thiz) return NULL;

	EINA_MAGIC_SET(thiz, ESVG_ELEMENT_MAGIC);
	thiz->data = data;
	enesim_matrix_identity(&thiz->state.transform);

	thiz->name_get = descriptor->name_get;
	thiz->renderer_get = descriptor->renderer_get;
	thiz->clone = descriptor->clone;
	thiz->setup = descriptor->setup;
	thiz->cleanup = descriptor->cleanup;
	thiz->has_changed = descriptor->has_changed;
	thiz->is_renderable = descriptor->is_renderable;

	r = enesim_renderer_new(&_descriptor, thiz);
	/* default values */
	thiz->container_width = 640;
	thiz->container_height = 480;
	thiz->x_dpi = 96.0;
	thiz->y_dpi = 96.0;

	esvg_attribute_presentation_setup(&thiz->attr);
	esvg_attribute_presentation_setup(&thiz->style);
	/* default enesim properties */
	enesim_renderer_rop_set(r, ENESIM_BLEND);

	return r;
}

/* FIXME handle correctly the ref counting */
Enesim_Renderer * esvg_element_parent_get(Edom_Tag *e)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(r);
	return thiz->parent;
}

/* FIXME handle correctly the ref counting */
void esvg_element_parent_set(Edom_Tag *e, Enesim_Renderer *parent)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	thiz->parent = parent;
}

/* state and attr are the parents one */
Eina_Bool esvg_element_setup(Edom_Tag *e, const Esvg_Element_State *state,
		const Esvg_Attribute_Presentation *attr,
		Enesim_Surface *s,
		Enesim_Error **error)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	/* the idea here is to call the setup interface of the element */
	/* note that on SVG every element must be part of a topmost SVG
	 * that way we need to always pass the upper svg/g element of this
	 * so relative properties can be calcualted correctly */
	if (!thiz->setup)
		return EINA_FALSE;
	/* TODO apply the style first */
	thiz->attr_final = thiz->attr;
	thiz->state_final = thiz->state;
	/* FIXME avoid so many copies */
	if (state)
	{
		_esvg_element_state_compose(&thiz->state, state, &thiz->state_final);
	}
	else
	{
		thiz->state_final.viewbox_w = thiz->container_width;
		thiz->state_final.viewbox_h = thiz->container_height;
	}

	/* in case we have set the style also merge it */
	/* FIXME should it have more priority than the properties? */
	if (thiz->style_set || attr)
	{
		if (thiz->style_set)
		{
			_esvg_element_state_merge(&thiz->style, &thiz->attr, &thiz->attr_final);
			if (attr)
			{
				_esvg_element_state_merge(&thiz->attr_final, attr, &thiz->attr_final);
			}
		}
		else
		{
			if (attr)
			{
				_esvg_element_state_merge(&thiz->attr, attr, &thiz->attr_final);
			}
		}
	}

	//esvg_attribute_presentation_dump(new_attr);

	if (!thiz->setup(e, &thiz->state_final, &thiz->attr_final, s, error))
		return EINA_FALSE;
	return EINA_TRUE;
}

Enesim_Renderer * esvg_element_renderer_get(Edom_Tag *e)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	return _esvg_element_renderer_get(thiz, e);
}

Eina_Bool esvg_element_is_renderable(Edom_Tag *e)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	return thiz->is_renderable;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool esvg_is_element(Edom_Tag *e)
{
	Esvg_Element *thiz;
	Eina_Bool ret;

	thiz = enesim_renderer_data_get(e);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_ELEMENT_MAGIC);

	return ret;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_id_get(Edom_Tag *e, const char **id)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	if (!id) return;
	*id = thiz->state.id;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_id_set(Edom_Tag *e, const char *id)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	if (thiz->state.id)
	{
		free(thiz->state.id);
		thiz->state.id = NULL;
	}
	if (id)
	{
		thiz->state.id = strdup(id);
	}
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_transform_set(Edom_Tag *e, const Enesim_Matrix *transform)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	if (!transform) return;
	thiz->state.transform = *transform;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_transform_get(Edom_Tag *e, Enesim_Matrix *transform)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	if (transform) *transform = thiz->state.transform;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_style_set(Edom_Tag *e, const Esvg_Attribute_Presentation *style)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
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
	if (!style)
	{
		thiz->style_set = EINA_FALSE;
	}
	else
	{
		thiz->style = *style;
		thiz->style_set = EINA_TRUE;
	}
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_style_get(Edom_Tag *e, Esvg_Attribute_Presentation *style)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	if (style) *style = thiz->style;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool esvg_element_style_is_set(Edom_Tag *e)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	return thiz->style_set;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_clip_path_set(Edom_Tag *e, const Edom_Tag *clip_path)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	if (clip_path) esvg_attribute_presentation_clip_path_set(&thiz->attr, clip_path);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_clip_path_unset(Edom_Tag *e)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	esvg_attribute_presentation_clip_path_unset(&thiz->attr);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_opacity_set(Edom_Tag *e, double opacity)
{
	Esvg_Element *thiz;
	thiz = _esvg_element_get(e);

	esvg_attribute_presentation_opacity_set(&thiz->attr, opacity);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_opacity_unset(Edom_Tag *e)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	esvg_attribute_presentation_opacity_unset(&thiz->attr);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_color_set(Edom_Tag *e, const Esvg_Color *color)
{
	Esvg_Element *thiz;
	thiz = _esvg_element_get(e);

	esvg_attribute_presentation_color_set(&thiz->attr, color);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_color_unset(Edom_Tag *e)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	esvg_attribute_presentation_color_unset(&thiz->attr);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_fill_set(Edom_Tag *e, const Esvg_Paint *fill)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	esvg_attribute_presentation_fill_set(&thiz->attr, fill);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_fill_unset(Edom_Tag *e)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	esvg_attribute_presentation_fill_unset(&thiz->attr);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_fill_opacity_set(Edom_Tag *e, double fill_opacity)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	esvg_attribute_presentation_fill_opacity_set(&thiz->attr, fill_opacity);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_fill_opacity_unset(Edom_Tag *e)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	esvg_attribute_presentation_fill_opacity_unset(&thiz->attr);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_fill_rule_set(Edom_Tag *e, Esvg_Fill_Rule fill_rule)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	esvg_attribute_presentation_fill_rule_set(&thiz->attr, fill_rule);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_fill_rule_unset(Edom_Tag *e)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	esvg_attribute_presentation_fill_rule_unset(&thiz->attr);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_stroke_set(Edom_Tag *e, const Esvg_Paint *stroke)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	esvg_attribute_presentation_stroke_set(&thiz->attr, stroke);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
/* EAPI void esvg_element_stroke_unset(Edom_Tag *e) */
/* { */
/* 	Esvg_Element *thiz; */

/* 	thiz = _esvg_element_get(e); */
/* 	esvg_attribute_presentation_stroke_unset(&thiz->attr); */
/* } */

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_stroke_width_set(Edom_Tag *e, const Esvg_Length *stroke_width)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	esvg_attribute_presentation_stroke_width_set(&thiz->attr, stroke_width);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
/* EAPI void esvg_element_stroke_width_unset(Edom_Tag *e) */
/* { */
/* 	Esvg_Element *thiz; */

/* 	thiz = _esvg_element_get(e); */
/* 	esvg_attribute_presentation_stroke_width_unset(&thiz->attr); */
/* } */

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_stroke_opacity_set(Edom_Tag *e, double stroke_opacity)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	esvg_attribute_presentation_stroke_opacity_set(&thiz->attr, stroke_opacity);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_stroke_opacity_unset(Edom_Tag *e)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	esvg_attribute_presentation_stroke_opacity_unset(&thiz->attr);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_stroke_line_cap_set(Edom_Tag *e, Esvg_Stroke_Line_Cap cap)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	esvg_attribute_presentation_stroke_line_cap_set(&thiz->attr, cap);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_stroke_line_cap_unset(Edom_Tag *e)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	esvg_attribute_presentation_stroke_line_cap_unset(&thiz->attr);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_stroke_line_join_set(Edom_Tag *e, Esvg_Stroke_Line_Join join)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	esvg_attribute_presentation_stroke_line_join_set(&thiz->attr, join);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_stroke_line_join_unset(Edom_Tag *e)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	esvg_attribute_presentation_stroke_line_join_unset(&thiz->attr);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_visibility_set(Edom_Tag *e, Eina_Bool visibility)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	esvg_attribute_presentation_visibility_set(&thiz->attr, visibility);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_visibility_unset(Edom_Tag *e)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	esvg_attribute_presentation_visibility_unset(&thiz->attr);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_container_width_set(Edom_Tag *e, double container_width)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	thiz->container_width = container_width;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_container_width_get(Edom_Tag *e, double *container_width)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	if (container_width) *container_width = thiz->container_width;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_container_height_set(Edom_Tag *e, double container_height)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	thiz->container_height = container_height;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_container_height_get(Edom_Tag *e, double *container_height)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	if (container_height) *container_height = thiz->container_height;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_x_dpi_set(Edom_Tag *e, double x_dpi)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	thiz->x_dpi = x_dpi;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_x_dpi_get(Edom_Tag *e, double *x_dpi)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	if (x_dpi) *x_dpi = thiz->x_dpi;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_y_dpi_set(Edom_Tag *e, double y_dpi)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	thiz->y_dpi = y_dpi;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_y_dpi_get(Edom_Tag *e, double *y_dpi)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(e);
	if (y_dpi) *y_dpi = thiz->y_dpi;
}

EAPI Edom_Tag * esvg_element_clone(Edom_Tag *e)
{
	Enesim_Renderer *new_r = NULL;

	/* check every final type */
	if (esvg_is_svg(e))
		new_r = esvg_svg_new();
	else if (esvg_is_g(e))
		new_r = esvg_g_new();
	else if (esvg_is_line(e))
		new_r = esvg_line_new();
	else if (esvg_is_circle(e))
		new_r = esvg_circle_new();
	else if (esvg_is_ellipse(e))
		new_r = esvg_ellipse_new();
	else if (esvg_is_path(e))
		new_r = esvg_path_new();
	else if (esvg_is_rect(e))
		new_r = esvg_rect_new();
	else if (esvg_is_polygon(e))
		new_r = esvg_polygon_new();

	if (new_r)
	{
		Esvg_Element *thiz;
		Esvg_Element *other;

		thiz = _esvg_element_get(e);
		other = _esvg_element_get(new_r);
		other->state = thiz->state;
		other->attr = thiz->attr;
		if (thiz->clone)
			thiz->clone(e, new_r);
	}

	return new_r;
}
