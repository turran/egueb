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
#include "egueb_svg_point.h"
#include "egueb_svg_path_seg.h"
#include "egueb_svg_path_seg_list.h"
#include "egueb_dom_value_list.h"
#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                         Parsing related functions                          *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_path_number_get(char **attr, double *x)
{
	char *iter;
	char *endptr;

	iter = *attr;
	EGUEB_DOM_SPACE_COMMA_SKIP(iter);
	*x = eina_strtod(iter, &endptr);
	if (iter == endptr)
		return EINA_FALSE;

	*attr = endptr;

	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_path_flag_get(char **attr, Eina_Bool *b)
{
	char *iter;

	iter = *attr;
	EGUEB_DOM_SPACE_COMMA_SKIP(iter);
	if (*iter == '0')
	{
		*b = EINA_FALSE;
	}
	else if (*iter == '1')
	{
		*b = EINA_TRUE;
	}
	else
	{
		ERR("can not convert flag");
		return EINA_FALSE;
	}
	*attr = iter + 1;
	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_path_point_get(char **attr, Egueb_Svg_Point *p)
{
	if (!_egueb_svg_path_number_get(attr, &p->x))
	{
		ERR("can not convert number");
		return EINA_FALSE;
	}

	if (!_egueb_svg_path_number_get(attr, &p->y))
	{
		ERR("can not convert number");
		return EINA_FALSE;
	}

	return EINA_TRUE;
}

static Eina_Bool egueb_svg_parser_path_line_to(Eina_Bool relative,
		char **value, Egueb_Svg_Path_Seg *cmd)
{
	Egueb_Svg_Point p;

	if (!_egueb_svg_path_point_get(value, &p))
	{
		ERR("Can not get point");
		return EINA_FALSE;
	}

	cmd->type = ESVG_PATH_LINE_TO;
	cmd->relative = relative;
	cmd->data.line_to.x = p.x;
	cmd->data.line_to.y = p.y;

	return EINA_TRUE;
}

static Eina_Bool egueb_svg_parser_path_move_to(Eina_Bool relative,
		char **value, Egueb_Svg_Path_Seg *cmd)
{
	Egueb_Svg_Point p;

	if (!_egueb_svg_path_point_get(value, &p))
	{
		ERR("Can not get point");
		return EINA_FALSE;
	}
	cmd->type = ESVG_PATH_MOVE_TO;
	cmd->relative = relative;
	cmd->data.move_to.x = p.x;
	cmd->data.move_to.y = p.y;

	return EINA_TRUE;
}

static Eina_Bool egueb_svg_parser_path_hline_to(Eina_Bool relative,
		char **value, Egueb_Svg_Path_Seg *cmd)
{
	double c;

	if (!_egueb_svg_path_number_get(value, &c))
	{
		ERR("Can not get coord");
		return EINA_FALSE;
	}
	cmd->type = ESVG_PATH_HLINE_TO;
	cmd->relative = relative;
	cmd->data.hline_to.c = c;

	return EINA_TRUE;
}

static Eina_Bool egueb_svg_parser_path_vline_to(Eina_Bool relative,
		char **value, Egueb_Svg_Path_Seg *cmd)
{
	double c;

	if (!_egueb_svg_path_number_get(value, &c))
	{
		ERR("Can not get coord");
		return EINA_FALSE;
	}
	cmd->type = ESVG_PATH_VLINE_TO;
	cmd->relative = relative;
	cmd->data.hline_to.c = c;

	return EINA_TRUE;
}

static Eina_Bool egueb_svg_parser_path_cubic_to(Eina_Bool relative,
		char **value, Egueb_Svg_Path_Seg *cmd)
{
	Egueb_Svg_Point ctrl0, ctrl1, p;

	if (!_egueb_svg_path_point_get(value, &ctrl0))
	{
		ERR("Can not get control point %s", *value);
		return EINA_FALSE;
	}

	if (!_egueb_svg_path_point_get(value, &ctrl1))
	{
		ERR("Can not get control point");
		return EINA_FALSE;
	}

	if (!_egueb_svg_path_point_get(value, &p))
	{
		ERR("Can not get point");
		return EINA_FALSE;
	}

	cmd->type = ESVG_PATH_CUBIC_TO;
	cmd->relative = relative;
	cmd->data.cubic_to.ctrl_x0 = ctrl0.x;
	cmd->data.cubic_to.ctrl_y0 = ctrl0.y;
	cmd->data.cubic_to.ctrl_x1 = ctrl1.x;
	cmd->data.cubic_to.ctrl_y1 = ctrl1.y;
	cmd->data.cubic_to.x = p.x;
	cmd->data.cubic_to.y = p.y;

	return EINA_TRUE;
}

static Eina_Bool egueb_svg_parser_path_scubic_to(Eina_Bool relative,
		char **value, Egueb_Svg_Path_Seg *cmd)
{
	Egueb_Svg_Point ctrl, p;

	if (!_egueb_svg_path_point_get(value, &ctrl))
	{
		ERR("Can not get control point");
		return EINA_FALSE;
	}

	if (!_egueb_svg_path_point_get(value, &p))
	{
		ERR("Can not get point");
		return EINA_FALSE;
	}

	cmd->type = ESVG_PATH_SCUBIC_TO;
	cmd->relative = relative;
	cmd->data.scubic_to.ctrl_x = ctrl.x;
	cmd->data.scubic_to.ctrl_y = ctrl.y;
	cmd->data.scubic_to.x = p.x;
	cmd->data.scubic_to.y = p.y;

	return EINA_TRUE;
}

static Eina_Bool egueb_svg_parser_path_quadratic_to(	Eina_Bool relative,
		char **value, Egueb_Svg_Path_Seg *cmd)
{
	Egueb_Svg_Point ctrl, p;

	if (!_egueb_svg_path_point_get(value, &ctrl))
	{
		ERR("Can not get control point");
		return EINA_FALSE;
	}

	if (!_egueb_svg_path_point_get(value, &p))
	{
		ERR("Can not get point");
		return EINA_FALSE;
	}

	cmd->type = ESVG_PATH_QUADRATIC_TO;
	cmd->relative = relative;
	cmd->data.quadratic_to.ctrl_x = ctrl.x;
	cmd->data.quadratic_to.ctrl_y = ctrl.y;
	cmd->data.quadratic_to.x = p.x;
	cmd->data.quadratic_to.y = p.y;

	return EINA_TRUE;

}

static Eina_Bool egueb_svg_parser_path_squadratic_to(Eina_Bool relative,
		char **value, Egueb_Svg_Path_Seg *cmd)
{
	Egueb_Svg_Point p;

	if (!_egueb_svg_path_point_get(value, &p))
	{
		ERR("Can not get point");
		return EINA_FALSE;
	}
	cmd->type = ESVG_PATH_SQUADRATIC_TO;
	cmd->relative = relative;
	cmd->data.squadratic_to.x = p.x;
	cmd->data.squadratic_to.y = p.y;

	return EINA_TRUE;
}

static Eina_Bool egueb_svg_parser_path_arc_to(Eina_Bool relative,
		char **value, Egueb_Svg_Path_Seg *cmd)
{
	Egueb_Svg_Point p, radii;
	Eina_Bool large, sweep;
	double angle;

	if (!_egueb_svg_path_point_get(value, &radii))
	{
		ERR("can not get radii");
		return EINA_FALSE;
	}

	if (!_egueb_svg_path_number_get(value, &angle))
	{
		ERR("can not convert number");
		return EINA_FALSE;
	}

	if (!_egueb_svg_path_flag_get(value, &large))
	{
		ERR("can not convert the large flag");
		return EINA_FALSE;
	}

	if (!_egueb_svg_path_flag_get(value, &sweep))
	{
		ERR("can not convert the sweep flag");
		return EINA_FALSE;
	}

	if (!_egueb_svg_path_point_get(value, &p))
	{
		ERR("Can not get point");
		return EINA_FALSE;
	}

	cmd->type = ESVG_PATH_ARC_TO;
	cmd->relative = relative;
	cmd->data.arc_to.rx = radii.x;
	cmd->data.arc_to.ry = radii.y;
	cmd->data.arc_to.angle = angle;
	cmd->data.arc_to.large = large;
	cmd->data.arc_to.sweep = sweep;
	cmd->data.arc_to.x = p.x;
	cmd->data.arc_to.y = p.y;

	return EINA_TRUE;
}

static Eina_Bool egueb_svg_parser_path_close(char **value,
		Egueb_Svg_Path_Seg *cmd)
{
	cmd->type = ESVG_PATH_CLOSE;
	cmd->relative = EINA_FALSE;

	return EINA_TRUE;
}

static Eina_Bool egueb_svg_seg_list_parse(char command, char **value,
		Egueb_Svg_Path_Seg *cmd)
{
	Eina_Bool ret = EINA_TRUE;

	switch (command)
	{
		case 'L':
		ret = egueb_svg_parser_path_line_to(EINA_FALSE, value, cmd);
		break;

		case 'l':
		ret = egueb_svg_parser_path_line_to(EINA_TRUE, value, cmd);
		break;

		case 'M':
		ret = egueb_svg_parser_path_move_to(EINA_FALSE, value, cmd);
		break;

		case 'm':
		ret = egueb_svg_parser_path_move_to(EINA_TRUE, value, cmd);
		break;

		case 'H':
		ret = egueb_svg_parser_path_hline_to(EINA_FALSE, value, cmd);
		break;

		case 'h':
		ret = egueb_svg_parser_path_hline_to(EINA_TRUE, value, cmd);
		break;

		case 'V':
		ret = egueb_svg_parser_path_vline_to(EINA_FALSE, value, cmd);
		break;

		case 'v':
		ret = egueb_svg_parser_path_vline_to(EINA_TRUE, value, cmd);
		break;

		case 'C':
		ret = egueb_svg_parser_path_cubic_to(EINA_FALSE, value, cmd);
		break;

		case 'c':
		ret = egueb_svg_parser_path_cubic_to(EINA_TRUE, value, cmd);
		break;

		case 'S':
		ret = egueb_svg_parser_path_scubic_to(EINA_FALSE, value, cmd);
		break;

		case 's':
		ret = egueb_svg_parser_path_scubic_to(EINA_TRUE, value, cmd);
		break;

		case 'Q':
		ret = egueb_svg_parser_path_quadratic_to(EINA_FALSE, value, cmd);
		break;

		case 'q':
		ret = egueb_svg_parser_path_quadratic_to(EINA_TRUE, value, cmd);
		break;

		case 'T':
		ret = egueb_svg_parser_path_squadratic_to(EINA_FALSE, value, cmd);
		break;

		case 't':
		ret = egueb_svg_parser_path_squadratic_to(EINA_TRUE, value, cmd);
		break;

		case 'A':
		ret = egueb_svg_parser_path_arc_to(EINA_FALSE, value, cmd);
		break;

		case 'a':
		ret = egueb_svg_parser_path_arc_to(EINA_TRUE, value, cmd);
		break;

		case 'z':
		case 'Z':
		ret = egueb_svg_parser_path_close(value, cmd);
		break;

		default:
		ret = EINA_FALSE;
		break;
	}
	return ret;
}
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Value_Descriptor _descriptor;

static void _egueb_svg_path_seg_list_data_from(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	egueb_dom_value_list_data_from(v, data);
}

static void _egueb_svg_path_seg_list_data_to(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	egueb_dom_value_list_data_to(v, data);
}

static void _egueb_svg_path_seg_list_free(Egueb_Dom_Value *v)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	egueb_dom_value_list_free(v);
}

static void _egueb_svg_path_seg_list_copy(const Egueb_Dom_Value *v, Egueb_Dom_Value *copy)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	egueb_dom_value_list_copy(v, copy);
}

static char * _egueb_svg_path_seg_list_string_to(const Egueb_Dom_Value *v)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_descriptor, NULL);
	return egueb_svg_path_seg_list_string_to(v->data.ptr);
}

static Eina_Bool _egueb_svg_path_seg_list_string_from(Egueb_Dom_Value *v, const char *str)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_descriptor, EINA_FALSE);
	if (!v->data.ptr)
	{
		v->data.ptr = egueb_dom_list_new(egueb_smil_timing_descriptor_get());
		v->owned = EINA_TRUE;
	}
	return egueb_svg_path_seg_list_string_from(v->data.ptr, str);
}

static Egueb_Dom_Value_Descriptor _descriptor = {
	/* .data_from 		= */ _egueb_svg_path_seg_list_data_from,
	/* .data_from_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .data_to 		= */ _egueb_svg_path_seg_list_data_to,
	/* .data_to_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .init 		= */ NULL,
	/* .free 		= */ _egueb_svg_path_seg_list_free,
	/* .copy 		= */ _egueb_svg_path_seg_list_copy,
	/* .string_to 		= */ _egueb_svg_path_seg_list_string_to,
	/* .string_from 	= */ _egueb_svg_path_seg_list_string_from,
	/* .interpolate 	= */ NULL,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI const Egueb_Dom_Value_Descriptor * egueb_svg_path_seg_list_descriptor_get(void)
{
	return &_descriptor;
}

EAPI Eina_Bool egueb_svg_path_seg_list_string_from(Egueb_Dom_List *l,
		const char *value)
{
	Eina_Bool ret = EINA_TRUE;
	Eina_Bool first = EINA_TRUE;
	char last_command = 0;
	char *iter = (char *)value;

	egueb_dom_list_clear(l);

	EGUEB_DOM_SPACE_SKIP(iter);
	/* empty path data */
	if (!*iter) return EINA_FALSE;
	/* First char must be 'M' or 'm' */
	if ((*iter != 'M') &&
	    (*iter != 'm'))
	{
		ERR("First char not 'M' or 'm' but '%c'", *iter);
		return EINA_FALSE;
	}
	while (*iter)
	{
		Egueb_Svg_Path_Seg cmd;
		char command;

 		command = *iter;
		iter++;
		ret = egueb_svg_seg_list_parse(command, &iter, &cmd);
		if (!ret)
		{
			/* try with the last command */
			iter--;
			ret = egueb_svg_seg_list_parse(last_command, &iter, &cmd);
			if (ret)
			{
				Egueb_Svg_Path_Seg *seg;

				seg = calloc(1, sizeof(Egueb_Svg_Path_Seg));
				*seg = cmd;
				egueb_dom_list_item_append(l, seg);
			}
		}
		else
		{
			Egueb_Svg_Path_Seg *seg;

			seg = calloc(1, sizeof(Egueb_Svg_Path_Seg));
			*seg = cmd;
			/* everything went ok, update the last command */
			last_command = command;
			egueb_dom_list_item_append(l, seg);
		}

		if (!ret)
		{
			ERR("Unsupported path data instruction (%c) %s", command, iter);
			break;
		}
		/* for the 'move' case the next elements should be lines */
		if ((command == 'm' || command == 'M'))
		{
			/* the next commands should be lines */
			if (command == 'm')
				last_command = 'l';
			else
				last_command = 'L';
		}
		first = EINA_FALSE;
		EGUEB_DOM_SPACE_COMMA_SKIP(iter);
	}
	return ret;
}

EAPI char * egueb_svg_path_seg_list_string_to(Egueb_Dom_List *l)
{
	ERR("Not supported");
	return NULL;
}

