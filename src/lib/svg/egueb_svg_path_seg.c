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
#include "egueb_svg_path_seg.h"
#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Value_Descriptor _egueb_svg_path_seg_descriptor;

static void _egueb_svg_path_seg_data_from(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_egueb_svg_path_seg_descriptor);
	egueb_dom_value_primitive_data_from(v, data);
}

static void _egueb_svg_path_seg_data_to(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_egueb_svg_path_seg_descriptor);
	egueb_dom_value_primitive_data_to(v, data);
}

static void _egueb_svg_path_seg_free(Egueb_Dom_Value *v)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_egueb_svg_path_seg_descriptor);
	if (v->owned)
	{
		free(v->data.ptr);
		v->data.ptr = NULL;
	}
}

static void _egueb_svg_path_seg_copy(const Egueb_Dom_Value *v, Egueb_Dom_Value *copy,
		Eina_Bool content)
{
	Egueb_Svg_Path_Seg *vl;
	Egueb_Svg_Path_Seg *cl;

	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_egueb_svg_path_seg_descriptor);
	if (!v->data.ptr)
		return;

	if (!copy->data.ptr)
	{
		copy->data.ptr = calloc(1, sizeof(Egueb_Svg_Path_Seg));
		copy->owned = EINA_TRUE;
	}
	cl = copy->data.ptr;
	vl = v->data.ptr;
	*cl = *vl;
}

static char * _egueb_svg_path_seg_string_to(const Egueb_Dom_Value *v)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_egueb_svg_path_seg_descriptor, NULL);
	return egueb_svg_path_seg_string_to(v->data.ptr);
}

#if 0
static Eina_Bool _egueb_svg_path_seg_string_from(Egueb_Dom_Value *v, const char *str)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_egueb_svg_path_seg_descriptor, EINA_FALSE);
	if (!v->data.ptr)
	{
		v->data.ptr = calloc(1, sizeof(Egueb_Svg_Path_Seg));
		v->owned = EINA_TRUE;
	}
	return egueb_svg_path_seg_string_from(v->data.ptr, str);
}
#endif

static void _egueb_svg_path_seg_interpolate(Egueb_Dom_Value *v,
		Egueb_Dom_Value *a, Egueb_Dom_Value *b, double m,
		Egueb_Dom_Value *add, Egueb_Dom_Value *acc, int mul)
{
	Egueb_Svg_Path_Seg *cv = v->data.ptr;
	Egueb_Svg_Path_Seg *cb = b->data.ptr;
	Egueb_Svg_Path_Seg *ca = a->data.ptr;

	if (m <= 0.5)
		cv->relative = ca->relative;
	else
		cv->relative = cb->relative;
	switch (ca->type)
	{
		case ESVG_PATH_MOVE_TO:
		etch_interpolate_double(ca->data.move_to.x, cb->data.move_to.x, m, &cv->data.move_to.x);
		etch_interpolate_double(ca->data.move_to.y, cb->data.move_to.y, m, &cv->data.move_to.y);
		break;

		case ESVG_PATH_LINE_TO:
		etch_interpolate_double(ca->data.line_to.x, cb->data.line_to.x, m, &cv->data.line_to.x);
		etch_interpolate_double(ca->data.line_to.y, cb->data.line_to.y, m, &cv->data.line_to.y);
		break;

		case ESVG_PATH_HLINE_TO:
		etch_interpolate_double(ca->data.hline_to.c, cb->data.hline_to.c, m, &cv->data.hline_to.c);
		break;

		case ESVG_PATH_VLINE_TO:
		etch_interpolate_double(ca->data.vline_to.c, cb->data.vline_to.c, m, &cv->data.vline_to.c);
		break;

		case ESVG_PATH_CUBIC_TO:
		etch_interpolate_double(ca->data.cubic_to.ctrl_x1, cb->data.cubic_to.ctrl_x1, m, &cv->data.cubic_to.ctrl_x1);
		etch_interpolate_double(ca->data.cubic_to.ctrl_y1, cb->data.cubic_to.ctrl_y1, m, &cv->data.cubic_to.ctrl_y1);
		etch_interpolate_double(ca->data.cubic_to.ctrl_x0, cb->data.cubic_to.ctrl_x0, m, &cv->data.cubic_to.ctrl_x0);
		etch_interpolate_double(ca->data.cubic_to.ctrl_y0, cb->data.cubic_to.ctrl_y0, m, &cv->data.cubic_to.ctrl_y0);
		etch_interpolate_double(ca->data.cubic_to.x, cb->data.cubic_to.x, m, &cv->data.cubic_to.x);
		etch_interpolate_double(ca->data.cubic_to.y, cb->data.cubic_to.y, m, &cv->data.cubic_to.y);
		break;

		case ESVG_PATH_SCUBIC_TO:
		etch_interpolate_double(ca->data.scubic_to.ctrl_x, cb->data.scubic_to.ctrl_x, m, &cv->data.scubic_to.ctrl_x);
		etch_interpolate_double(ca->data.scubic_to.ctrl_y, cb->data.scubic_to.ctrl_y, m, &cv->data.scubic_to.ctrl_y);
		etch_interpolate_double(ca->data.scubic_to.x, cb->data.scubic_to.x, m, &cv->data.scubic_to.x);
		etch_interpolate_double(ca->data.scubic_to.y, cb->data.scubic_to.y, m, &cv->data.scubic_to.y);
		break;

		case ESVG_PATH_QUADRATIC_TO:
		etch_interpolate_double(ca->data.quadratic_to.ctrl_x, cb->data.quadratic_to.ctrl_x, m, &cv->data.quadratic_to.ctrl_x);
		etch_interpolate_double(ca->data.quadratic_to.ctrl_y, cb->data.quadratic_to.ctrl_y, m, &cv->data.quadratic_to.ctrl_y);
		etch_interpolate_double(ca->data.quadratic_to.x, cb->data.quadratic_to.x, m, &cv->data.quadratic_to.x);
		etch_interpolate_double(ca->data.quadratic_to.y, cb->data.quadratic_to.y, m, &cv->data.quadratic_to.y);
		break;

		case ESVG_PATH_SQUADRATIC_TO:
		etch_interpolate_double(ca->data.squadratic_to.x, cb->data.squadratic_to.x, m, &cv->data.squadratic_to.x);
		etch_interpolate_double(ca->data.squadratic_to.y, cb->data.squadratic_to.y, m, &cv->data.squadratic_to.y);
		break;

		case ESVG_PATH_ARC_TO:
		etch_interpolate_double(ca->data.arc_to.x, cb->data.arc_to.x, m, &cv->data.arc_to.x);
		etch_interpolate_double(ca->data.arc_to.y, cb->data.arc_to.y, m, &cv->data.arc_to.y);
		break;

		case ESVG_PATH_CLOSE:
		default:
		break;
	}
}

static Egueb_Dom_Value_Descriptor _egueb_svg_path_seg_descriptor = {
	/* .data_from 		= */ _egueb_svg_path_seg_data_from,
	/* .data_from_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .data_to 		= */ _egueb_svg_path_seg_data_to,
	/* .data_to_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .init 		= */ NULL,
	/* .free 		= */ _egueb_svg_path_seg_free,
	/* .copy 		= */ _egueb_svg_path_seg_copy,
	/* .string_to 		= */ _egueb_svg_path_seg_string_to,
	/* .string_from 	= */ NULL,
	/* .interpolate 	= */ _egueb_svg_path_seg_interpolate,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI const Egueb_Dom_Value_Descriptor * egueb_svg_path_seg_descriptor_get(void)
{
	return &_egueb_svg_path_seg_descriptor;
}

EAPI char * egueb_svg_path_seg_string_to(Egueb_Svg_Path_Seg *thiz)
{
	char *ret = NULL;
	char cmd;

	switch (thiz->type)
	{
		case ESVG_PATH_MOVE_TO:
		cmd = 'M';
		if (thiz->relative)
			cmd = 'm';
		ret = eina_extra_str_dup_printf("%c %g %g", cmd, thiz->data.move_to.x, thiz->data.move_to.y);
		break;

		case ESVG_PATH_LINE_TO:
		cmd = 'L';
		if (thiz->relative)
			cmd = 'l';
		ret = eina_extra_str_dup_printf("%c %g %g", cmd, thiz->data.line_to.x, thiz->data.line_to.y);
		break;

		case ESVG_PATH_HLINE_TO:
		cmd = 'H';
		if (thiz->relative)
			cmd = 'h';
		ret = eina_extra_str_dup_printf("%c %g", cmd, thiz->data.hline_to.c);
		break;

		case ESVG_PATH_VLINE_TO:
		cmd = 'V';
		if (thiz->relative)
			cmd = 'v';
		ret = eina_extra_str_dup_printf("%c %g", cmd, thiz->data.vline_to.c);
		break;

		case ESVG_PATH_CUBIC_TO:
		cmd = 'C';
		if (thiz->relative)
			cmd = 'c';
		ret = eina_extra_str_dup_printf("%c %g %g %g %g %g %g", cmd,
				thiz->data.cubic_to.ctrl_x0,
				thiz->data.cubic_to.ctrl_y0,
				thiz->data.cubic_to.ctrl_x1,
				thiz->data.cubic_to.ctrl_y1,
				thiz->data.cubic_to.x,
				thiz->data.cubic_to.y);
		break;

		case ESVG_PATH_SCUBIC_TO:
		cmd = 'S';
		if (thiz->relative)
			cmd = 's';
		ret = eina_extra_str_dup_printf("%c %g %g %g %g", cmd,
				thiz->data.scubic_to.ctrl_x,
				thiz->data.scubic_to.ctrl_y,
				thiz->data.scubic_to.x,
				thiz->data.scubic_to.y);
		break;

#if 0
		case ESVG_PATH_QUADRATIC_TO:
		DBG("quadratic_to %c ((%g, %g) (%g, %g))",
				pcmd->relative ? 'R' : 'A',
				pcmd->data.quadratic_to.ctrl_x,
				pcmd->data.quadratic_to.ctrl_y,
				pcmd->data.quadratic_to.x,
				pcmd->data.quadratic_to.y);
		cmd.type = ENESIM_PATH_COMMAND_QUADRATIC_TO;
		if (pcmd->relative)
		{
			cmd.definition.quadratic_to.x = cur.x + pcmd->data.quadratic_to.x;
			cmd.definition.quadratic_to.y = cur.y + pcmd->data.quadratic_to.y;
			cmd.definition.quadratic_to.ctrl_x = cur.x + pcmd->data.quadratic_to.ctrl_x;
			cmd.definition.quadratic_to.ctrl_y = cur.y + pcmd->data.quadratic_to.ctrl_y;

		}
		else
		{
			cmd.definition.quadratic_to.x = pcmd->data.quadratic_to.x;
			cmd.definition.quadratic_to.y = pcmd->data.quadratic_to.y;
			cmd.definition.quadratic_to.ctrl_x = pcmd->data.quadratic_to.ctrl_x;
			cmd.definition.quadratic_to.ctrl_y = pcmd->data.quadratic_to.ctrl_y;
		}
		cur.x = cmd.definition.quadratic_to.x;
		cur.y = cmd.definition.quadratic_to.y;
		break;

		case ESVG_PATH_SQUADRATIC_TO:
		DBG("squadratic_to %c (%g, %g)",
				pcmd->relative ? 'R' : 'A',
				pcmd->data.squadratic_to.x,
				pcmd->data.squadratic_to.y);
		cmd.type = ENESIM_PATH_COMMAND_SQUADRATIC_TO;
		if (pcmd->relative)
		{
			cmd.definition.squadratic_to.x = cur.x + pcmd->data.squadratic_to.x;
			cmd.definition.squadratic_to.y = cur.y + pcmd->data.squadratic_to.y;
		}
		else
		{
			cmd.definition.squadratic_to.x = pcmd->data.squadratic_to.x;
			cmd.definition.squadratic_to.y = pcmd->data.squadratic_to.y;
		}
		cur.x = cmd.definition.squadratic_to.x;
		cur.y = cmd.definition.squadratic_to.y;
		break;

		case ESVG_PATH_ARC_TO:
		DBG("arc_to %c ((%g, %g) %g %g %g (%g, %g))",
				pcmd->relative ? 'R' : 'A',
				pcmd->data.arc_to.rx,
				pcmd->data.arc_to.ry,
				pcmd->data.arc_to.angle,
				pcmd->data.arc_to.large,
				pcmd->data.arc_to.sweep,
				pcmd->data.arc_to.x,
				pcmd->data.arc_to.y);
		cmd.type = ENESIM_PATH_COMMAND_ARC_TO;
		if (pcmd->relative)
		{
			cmd.definition.arc_to.x = cur.x + pcmd->data.arc_to.x;
			cmd.definition.arc_to.y = cur.y + pcmd->data.arc_to.y;
		}
		else
		{
			cmd.definition.arc_to.x = pcmd->data.arc_to.x;
			cmd.definition.arc_to.y = pcmd->data.arc_to.y;
		}
		cmd.definition.arc_to.rx = pcmd->data.arc_to.rx;
		cmd.definition.arc_to.ry = pcmd->data.arc_to.ry;
		cmd.definition.arc_to.angle = pcmd->data.arc_to.angle;
		cmd.definition.arc_to.large = pcmd->data.arc_to.large;
		cmd.definition.arc_to.sweep = pcmd->data.arc_to.sweep;
		cur.x = cmd.definition.arc_to.x;
		cur.y = cmd.definition.arc_to.y;
		break;

		case ESVG_PATH_CLOSE:
		DBG("close");
		cmd.type = ENESIM_PATH_COMMAND_CLOSE;
		cmd.definition.close.close = EINA_TRUE;
		break;
#endif
		default:
		break;

	}
	return ret;
}
