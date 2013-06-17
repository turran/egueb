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
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
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
		ret = eina_str_dup_printf("%c %g %g", cmd, thiz->data.move_to.x, thiz->data.move_to.y);
		break;

		case ESVG_PATH_LINE_TO:
		cmd = 'L';
		if (thiz->relative)
			cmd = 'l';
		ret = eina_str_dup_printf("%c %g %g", cmd, thiz->data.line_to.x, thiz->data.line_to.y);
		break;

		case ESVG_PATH_HLINE_TO:
		cmd = 'H';
		if (thiz->relative)
			cmd = 'h';
		ret = eina_str_dup_printf("%c %g", cmd, thiz->data.hline_to.c);
		break;

		case ESVG_PATH_VLINE_TO:
		cmd = 'V';
		if (thiz->relative)
			cmd = 'v';
		ret = eina_str_dup_printf("%c %g", cmd, thiz->data.vline_to.c);
		break;

#if 0
		case ESVG_PATH_CUBIC_TO:
		DBG("cubic_to %c ((%g, %g) (%g, %g) (%g, %g))",
				pcmd->relative ? 'R' : 'A',
				pcmd->data.cubic_to.ctrl_x0,
				pcmd->data.cubic_to.ctrl_y0,
				pcmd->data.cubic_to.ctrl_x1,
				pcmd->data.cubic_to.ctrl_y1,
				pcmd->data.cubic_to.x,
				pcmd->data.cubic_to.y);
		cmd.type = ENESIM_PATH_COMMAND_CUBIC_TO;
		if (pcmd->relative)
		{
			cmd.definition.cubic_to.x = cur.x + pcmd->data.cubic_to.x;
			cmd.definition.cubic_to.y = cur.y + pcmd->data.cubic_to.y;
			cmd.definition.cubic_to.ctrl_x0 = cur.x + pcmd->data.cubic_to.ctrl_x0;
			cmd.definition.cubic_to.ctrl_y0 = cur.y + pcmd->data.cubic_to.ctrl_y0;
			cmd.definition.cubic_to.ctrl_x1 = cur.x + pcmd->data.cubic_to.ctrl_x1;
			cmd.definition.cubic_to.ctrl_y1 = cur.y + pcmd->data.cubic_to.ctrl_y1;

		}
		else
		{
			cmd.definition.cubic_to.x = pcmd->data.cubic_to.x;
			cmd.definition.cubic_to.y = pcmd->data.cubic_to.y;
			cmd.definition.cubic_to.ctrl_x0 = pcmd->data.cubic_to.ctrl_x0;
			cmd.definition.cubic_to.ctrl_y0 = pcmd->data.cubic_to.ctrl_y0;
			cmd.definition.cubic_to.ctrl_x1 = pcmd->data.cubic_to.ctrl_x1;
			cmd.definition.cubic_to.ctrl_y1 = pcmd->data.cubic_to.ctrl_y1;
		}
		cur.x = cmd.definition.cubic_to.x;
		cur.y = cmd.definition.cubic_to.y;
		break;

		case ESVG_PATH_SCUBIC_TO:
		DBG("scubic_to %c ((%g, %g) (%g, %g))",
				pcmd->relative ? 'R' : 'A',
				pcmd->data.scubic_to.ctrl_x,
				pcmd->data.scubic_to.ctrl_y,
				pcmd->data.scubic_to.x,
				pcmd->data.scubic_to.y);
		cmd.type = ENESIM_PATH_COMMAND_SCUBIC_TO;
		if (pcmd->relative)
		{
			cmd.definition.scubic_to.x = cur.x + pcmd->data.scubic_to.x;
			cmd.definition.scubic_to.y = cur.y + pcmd->data.scubic_to.y;
			cmd.definition.scubic_to.ctrl_x = cur.x + pcmd->data.scubic_to.ctrl_x;
			cmd.definition.scubic_to.ctrl_y = cur.y + pcmd->data.scubic_to.ctrl_y;
		}
		else
		{
			cmd.definition.scubic_to.x = pcmd->data.scubic_to.x;
			cmd.definition.scubic_to.y = pcmd->data.scubic_to.y;
			cmd.definition.scubic_to.ctrl_x = pcmd->data.scubic_to.ctrl_x;
			cmd.definition.scubic_to.ctrl_y = pcmd->data.scubic_to.ctrl_y;
		}
		cur.x = cmd.definition.scubic_to.x;
		cur.y = cmd.definition.scubic_to.y;
		break;

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
