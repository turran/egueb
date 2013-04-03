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
#include "esvg_main_private.h"
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
		case ESVG_PAINT_TYPE_CURRENT_COLOR:
		printf("current color");
		break;

		case ESVG_PAINT_TYPE_COLOR:
		_color_dump(&p->color);
		break;

		case ESVG_PAINT_TYPE_SERVER:
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
		case ESVG_STROKE_LINE_CAP_BUTT:
		printf("butt");
		break;

		case ESVG_STROKE_LINE_CAP_ROUND:
		printf("round");
		break;

		case ESVG_STROKE_LINE_CAP_SQUARE:
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
		case ESVG_STROKE_LINE_JOIN_MITER:
		printf("miter");
		break;

		case ESVG_STROKE_LINE_JOIN_ROUND:
		printf("round");
		break;

		case ESVG_STROKE_LINE_JOIN_BEVEL:
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
	Esvg_Number onen = { 1.0 };

	/* now the default values */
	thiz->color.v = black;
	thiz->stroke_width.v = ESVG_LENGTH_1;
	thiz->stroke_opacity.v = onen;
	thiz->fill_opacity.v = onen;
	thiz->opacity.v = onen;
	thiz->fill_rule.v = ESVG_NON_ZERO;
	thiz->fill.v.type = ESVG_PAINT_TYPE_COLOR;
	thiz->fill.v.color = black;
	thiz->stroke.v.type = ESVG_PAINT_TYPE_NONE;
	thiz->stroke_width.v = one;
	thiz->stroke_line_cap.v = ESVG_STROKE_LINE_CAP_BUTT;
	thiz->stroke_line_join.v = ESVG_STROKE_LINE_JOIN_MITER;
	thiz->stop_opacity.v = onen;
	thiz->visibility.v = ESVG_VISIBILITY_VISIBLE;
	thiz->clip_path.v.type = ESVG_CLIP_PATH_NONE;
}

void esvg_attribute_presentation_merge_rel(const Esvg_Attribute_Presentation *rel,
		const Esvg_Attribute_Presentation *s,
		Esvg_Attribute_Presentation *d)
{
	/* FIXME check if actually something has changed */
	/* clip_path */
	esvg_attribute_clip_path_merge_rel(&rel->clip_path, &s->clip_path, &d->clip_path);
	/* color */
	esvg_attribute_color_merge_rel(&rel->color, &s->color, &d->color);
	/* opacity */
	esvg_attribute_number_merge_rel(&rel->opacity, &s->opacity, &d->opacity);
	/* FIXME do the real merge_rel (multiply, etc, etc) */
	/* fill */
	esvg_attribute_paint_merge_rel(&rel->fill, &s->fill, &d->fill);
	/* fill opacity */
	esvg_attribute_number_merge_rel(&rel->fill_opacity, &s->fill_opacity, &d->fill_opacity);
	/* fill rule */
	esvg_attribute_enum_merge_rel(&rel->fill_rule, &s->fill_rule, &d->fill_rule);
	/* stroke */
	esvg_attribute_paint_merge_rel(&rel->stroke, &s->stroke, &d->stroke);
	/* stroke width */
	esvg_attribute_length_merge_rel(&rel->stroke_width, &s->stroke_width, &d->stroke_width);
	/* stroke line cap */
	esvg_attribute_enum_merge_rel(&rel->stroke_line_cap, &s->stroke_line_cap, &d->stroke_line_cap);
	/* stroke line join */
	esvg_attribute_enum_merge_rel(&rel->stroke_line_join, &s->stroke_line_join, &d->stroke_line_join);
	/* stroke opacity */
	esvg_attribute_number_merge_rel(&rel->stroke_opacity, &s->stroke_opacity, &d->stroke_opacity);
	/* visibility */
	esvg_attribute_enum_merge_rel(&rel->visibility, &s->visibility, &d->visibility);
	/* stop opacity */
	esvg_attribute_number_merge_rel(&rel->stop_opacity, &s->stop_opacity, &d->stop_opacity);
	/* stop color */
	esvg_attribute_color_merge_rel(&rel->stop_color, &s->stop_color, &d->stop_color);
	/* display */
	esvg_attribute_enum_merge_rel(&rel->display, &s->display, &d->display);
}

void esvg_attribute_presentation_dump(Esvg_Attribute_Presentation *thiz)
{
#if 0
	/* disabled for now */

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
#endif
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
