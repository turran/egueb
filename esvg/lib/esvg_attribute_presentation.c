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
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static void _color_dump(Esvg_Color *c)
{
	printf("%02x%02x%02x", c->r, c->g, c->b);
}

static void _paint_dump(Esvg_Paint *p)
{
	switch (p->type)
	{
		case ESVG_PAINT_CURRENT_COLOR:
		printf("current color");
		break;

		case ESVG_PAINT_COLOR:
		_color_dump(&p->value.color);
		break;

		case ESVG_PAINT_SERVER:
		printf("paint server");
		break;

		default:
		break;
	}
}

static void _length_dump(Esvg_Length *l)
{
	printf("%g", l->value);
	switch (l->unit)
	{
		case ESVG_UNIT_LENGTH_EM:
		printf("em");
		break;

		case ESVG_UNIT_LENGTH_EX:
		printf("ex");
		break;

		case ESVG_UNIT_LENGTH_PX:
		printf("px");
		break;

		case ESVG_UNIT_LENGTH_PT:
		printf("pt");
		break;

		case ESVG_UNIT_LENGTH_PC:
		printf("pc");
		break;

		case ESVG_UNIT_LENGTH_CM:
		printf("cm");
		break;

		case ESVG_UNIT_LENGTH_MM:
		printf("mm");
		break;

		case ESVG_UNIT_LENGTH_IN:
		printf("in");
		break;

		case ESVG_UNIT_LENGTH_PERCENT:
		printf("%%");
		break;

		default:
		break;
	}
}

static void _line_cap_dump(Esvg_Stroke_Line_Cap cap)
{
	switch (cap)
	{
		case ESVG_LINE_CAP_BUTT:
		printf("butt");
		break;

		case ESVG_LINE_CAP_ROUND:
		printf("round");
		break;

		case ESVG_LINE_CAP_SQUARE:
		printf("square");
		break;

		default:
		break;
	}
}

static void _line_join_dump(Esvg_Stroke_Line_Join join)
{
	switch (join)
	{
		case ESVG_LINE_JOIN_MITER:
		printf("miter");
		break;

		case ESVG_LINE_JOIN_ROUND:
		printf("round");
		break;

		case ESVG_LINE_JOIN_BEVEL:
		printf("bevel");
		break;

		default:
		break;
	}
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_attribute_presentation_setup(Esvg_Attribute_Presentation *thiz)
{
	Esvg_Color black = { 0, 0, 0 };
	Esvg_Length one = { ESVG_UNIT_LENGTH_PX, 1 }; 

	thiz->color = black;
	thiz->stroke_width = ESVG_LENGTH_1;
	thiz->stroke_opacity = 1.0;
	thiz->fill_opacity = 1.0;
	thiz->opacity = 1.0;
	thiz->fill_rule = ESVG_NON_ZERO;
	thiz->fill.type = ESVG_PAINT_COLOR;
	thiz->fill.value.color = black;
	thiz->stroke.type = ESVG_PAINT_NONE;
	thiz->stroke_width = one;
	thiz->stroke_line_cap = ESVG_LINE_CAP_BUTT;
	thiz->stroke_line_join = ESVG_LINE_JOIN_MITER;
	thiz->stop_opacity = 1.0;
}

void esvg_attribute_presentation_merge(const Esvg_Attribute_Presentation *state,
		const Esvg_Attribute_Presentation *parent,
		Esvg_Attribute_Presentation *d)
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


void esvg_attribute_presentation_clip_path_set(Esvg_Attribute_Presentation *thiz, const Edom_Tag *clip_path)
{
	if (thiz->clip_path == clip_path)
		return;

	if (!clip_path)
	{
		esvg_attribute_presentation_clip_path_unset(thiz);
	}
	else
	{
		/* FIXME fis this */
#if 0
		if (!esvg_is_clip_path(clip_path))
			return;
#endif
		if (thiz->clip_path)
			enesim_renderer_unref(thiz->clip_path);

		thiz->clip_path = enesim_renderer_ref(clip_path);
		thiz->clip_path_set = EINA_TRUE;
	}
}

void esvg_attribute_presentation_clip_path_unset(Esvg_Attribute_Presentation *thiz)
{
	if (thiz->clip_path)
		enesim_renderer_unref(thiz->clip_path);
	thiz->clip_path = NULL;
	thiz->clip_path_set = EINA_FALSE;
}

void esvg_attribute_presentation_color_set(Esvg_Attribute_Presentation *thiz, const Esvg_Color *color)
{
	if (!color)
	{
		esvg_attribute_presentation_color_unset(thiz);
	}
	else
	{
		thiz->color = *color;
		thiz->color_set = EINA_TRUE;
	}
}

void esvg_attribute_presentation_color_unset(Esvg_Attribute_Presentation *thiz)
{
	Esvg_Color black = { 0, 0, 0 };

	thiz->color = black;
	thiz->color_set = EINA_FALSE;
}

void esvg_attribute_presentation_opacity_set(Esvg_Attribute_Presentation *thiz, double opacity)
{
	thiz->opacity = opacity;
	thiz->opacity_set = EINA_TRUE;
}

void esvg_attribute_presentation_opacity_unset(Esvg_Attribute_Presentation *thiz, double opacity)
{
	thiz->opacity = 1.0;
	thiz->opacity_set = EINA_FALSE;
}

void esvg_attribute_presentation_fill_set(Esvg_Attribute_Presentation *thiz, const Esvg_Paint *fill)
{
	if (!fill)
	{
		esvg_attribute_presentation_fill_unset(thiz);
	}
	else
	{
		thiz->fill = *fill;
		thiz->fill_set = EINA_TRUE;
	}
}

void esvg_attribute_presentation_fill_unset(Esvg_Attribute_Presentation *thiz)
{
	thiz->fill_set = EINA_FALSE;
}

void esvg_attribute_presentation_fill_opacity_set(Esvg_Attribute_Presentation *thiz, double fill_opacity)
{
	if (fill_opacity > 1)
		fill_opacity = 1;
	else if (fill_opacity < 0)
		fill_opacity = 0;

	thiz->fill_opacity = fill_opacity;
	thiz->fill_opacity_set = EINA_TRUE;
}

void esvg_attribute_presentation_fill_opacity_unset(Esvg_Attribute_Presentation *thiz)
{
	thiz->fill_opacity_set = EINA_FALSE;
}

void esvg_attribute_presentation_fill_rule_set(Esvg_Attribute_Presentation *thiz, Esvg_Fill_Rule rule)
{
	thiz->fill_rule = rule;
	thiz->fill_rule_set = EINA_TRUE;
}

void esvg_attribute_presentation_fill_rule_unset(Esvg_Attribute_Presentation *thiz)
{
	thiz->fill_rule_set = EINA_FALSE;
}

void esvg_attribute_presentation_stroke_set(Esvg_Attribute_Presentation *thiz, const Esvg_Paint *stroke)
{
	if (!stroke)
	{
		thiz->stroke_set = EINA_FALSE;
	}
	else
	{
		thiz->stroke = *stroke;
		thiz->stroke_set = EINA_TRUE;
	}
}

void esvg_attribute_presentation_stroke_width_set(Esvg_Attribute_Presentation *thiz, const Esvg_Length *stroke_width)
{
	if (!stroke_width) return;
	thiz->stroke_width = *stroke_width;
	thiz->stroke_width_set = EINA_TRUE;
}

void esvg_attribute_presentation_stroke_opacity_set(Esvg_Attribute_Presentation *thiz, double stroke_opacity)
{
	if (stroke_opacity > 1)
		stroke_opacity = 1;
	else if (stroke_opacity < 0)
		stroke_opacity = 0;

	thiz->stroke_opacity = stroke_opacity;
	thiz->stroke_opacity_set = EINA_TRUE;
}

void esvg_attribute_presentation_stroke_opacity_unset(Esvg_Attribute_Presentation *thiz)
{
	thiz->stroke_opacity_set = EINA_FALSE;
}

void esvg_attribute_presentation_stroke_line_cap_set(Esvg_Attribute_Presentation *thiz, Esvg_Stroke_Line_Cap cap)
{
	thiz->stroke_line_cap = cap;
	thiz->stroke_line_cap_set = EINA_TRUE;
}

void esvg_attribute_presentation_stroke_line_cap_unset(Esvg_Attribute_Presentation *thiz)
{
	thiz->stroke_line_cap_set = EINA_FALSE;
}

void esvg_attribute_presentation_stroke_line_join_set(Esvg_Attribute_Presentation *thiz, Esvg_Stroke_Line_Join join)
{
	thiz->stroke_line_join = join;
	thiz->stroke_line_join_set = EINA_TRUE;
}

void esvg_attribute_presentation_stroke_line_join_unset(Esvg_Attribute_Presentation *thiz)
{
	thiz->stroke_line_join_set = EINA_FALSE;
}

void esvg_attribute_presentation_visibility_set(Esvg_Attribute_Presentation *thiz, Eina_Bool visibility)
{
	thiz->visibility = visibility;
	thiz->visibility_set = EINA_TRUE;
}

void esvg_attribute_presentation_visibility_unset(Esvg_Attribute_Presentation *thiz)
{
	thiz->visibility_set = EINA_FALSE;
}

void esvg_attribute_presentation_stop_color_set(Esvg_Attribute_Presentation *thiz, const Esvg_Color *stop_color)
{
	if (!stop_color)
	{
		esvg_attribute_presentation_stop_color_unset(thiz);
	}
	else
	{
		thiz->stop_color = *stop_color;
		thiz->stop_color_set = EINA_TRUE;
	}
}

void esvg_attribute_presentation_stop_color_unset(Esvg_Attribute_Presentation *thiz)
{
	Esvg_Color black = { 0, 0, 0 };

	thiz->stop_color = black;
	thiz->stop_color_set = EINA_FALSE;
}

void esvg_attribute_presentation_stop_opacity_set(Esvg_Attribute_Presentation *thiz, double stop_opacity)
{
	thiz->stop_opacity = stop_opacity;
	thiz->stop_opacity_set = EINA_TRUE;
}

void esvg_attribute_presentation_stop_opacity_unset(Esvg_Attribute_Presentation *thiz, double stop_opacity)
{
	thiz->stop_opacity = 1.0;
	thiz->stop_opacity_set = EINA_FALSE;
}

void esvg_attribute_presentation_dump(Esvg_Attribute_Presentation *thiz)
{
	printf("fill: %d ", thiz->fill_set);
	_paint_dump(&thiz->fill);
	printf("\n");

	printf("stroke: %d ", thiz->stroke_set);
	_paint_dump(&thiz->stroke);
	printf("\n");

	printf("color: %d ", thiz->color_set);
	_color_dump(&thiz->color);
	printf("\n");

	printf("stroke width: %d ", thiz->stroke_width_set);
	_length_dump(&thiz->stroke_width);
	printf("\n");

	printf("stroke line cap: %d ", thiz->stroke_line_cap_set);
	_line_cap_dump(thiz->stroke_line_cap);
	printf("\n");

	printf("stroke line join: %d ", thiz->stroke_line_join_set);
	_line_join_dump(thiz->stroke_line_join);
	printf("\n");
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
