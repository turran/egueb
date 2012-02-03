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
	/* TODO set default values */
}

void esvg_attribute_presentation_clip_path_set(Esvg_Attribute_Presentation *thiz, const Enesim_Renderer *clip_path)
{
	if (thiz->clip_path == clip_path)
		return;

	if (!clip_path)
	{
		esvg_attribute_presentation_clip_path_unset(thiz);
	}
	else
	{
		if (!esvg_is_clip_path(clip_path))
			return;

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
