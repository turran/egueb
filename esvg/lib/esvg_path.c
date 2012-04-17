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
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_PATH_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_PATH_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_PATH_MAGIC);\
	} while(0)

typedef struct _Esvg_Path
{
	EINA_MAGIC
	/* properties */
	Eina_List *commands;
	/* private */
	Enesim_Renderer *r;
} Esvg_Path;

static Esvg_Path * _esvg_path_get(Enesim_Renderer *r)
{
	Esvg_Path *thiz;

	thiz = esvg_shape_data_get(r);
	ESVG_PATH_MAGIC_CHECK(thiz);

	return thiz;
}

/*----------------------------------------------------------------------------*
 *                         The ESVG element interface                         *
 *----------------------------------------------------------------------------*/
static const char * _esvg_path_name_get(Enesim_Renderer *r)
{
	return "esvg_path";
}

static Enesim_Renderer * _esvg_path_renderer_get(Enesim_Renderer *r)
{
	Esvg_Path *thiz;

	thiz = _esvg_path_get(r);
	return thiz->r;
}

static Eina_Bool _esvg_path_setup(Enesim_Renderer *r, const Esvg_Element_Context *estate,
		const Esvg_Shape_Enesim_State *dstate)
{
	Esvg_Path *thiz;
	Esvg_Path_Command *c;
	Eina_List *l;
	Esvg_Point cur = { 0, 0 };
	Eina_Bool first; /* TODO handle correctly the first flag */

	thiz = _esvg_path_get(r);

	//printf("path setup\n");
	/* shape properties */
	if (!dstate->fill_renderer)
		enesim_renderer_shape_fill_color_set(thiz->r, dstate->fill_color);
	else
		enesim_renderer_shape_fill_renderer_set(thiz->r, dstate->fill_renderer);
	
	if (!dstate->stroke_renderer)
		enesim_renderer_shape_stroke_color_set(thiz->r, dstate->stroke_color);
	else
		enesim_renderer_shape_stroke_renderer_set(thiz->r, dstate->stroke_renderer);

	enesim_renderer_shape_fill_rule_set(thiz->r, dstate->fill_rule);
	enesim_renderer_shape_stroke_weight_set(thiz->r, dstate->stroke_weight);
	enesim_renderer_shape_stroke_location_set(thiz->r, ENESIM_SHAPE_STROKE_CENTER);
	enesim_renderer_shape_stroke_cap_set(thiz->r, dstate->stroke_cap);
	enesim_renderer_shape_stroke_join_set(thiz->r, dstate->stroke_join);
	enesim_renderer_shape_draw_mode_set(thiz->r, dstate->draw_mode);
	/* base properties */
	enesim_renderer_geometry_transformation_set(thiz->r, &estate->transform);
	enesim_renderer_color_set(thiz->r, dstate->color);

	EINA_LIST_FOREACH(thiz->commands, l, c)
	{
		Enesim_Renderer_Path_Command cmd;
		const char *name;
		/* FIXME or we either use the generic command API
		 * of enesim or use the direct function call
		 */
		enesim_renderer_name_get(thiz->r, &name);
		//printf("calling the setup on the path %s ", name);
		switch (c->type)
		{
			case ESVG_PATH_MOVE_TO:
			cmd.type = ENESIM_COMMAND_MOVE_TO;
			if (!first && c->relative)
			{
				cmd.definition.move_to.x = cur.x + c->data.move_to.x;
				cmd.definition.move_to.y = cur.y + c->data.move_to.y;
			}
			else
			{
				cmd.definition.move_to.x = c->data.move_to.x;
				cmd.definition.move_to.y = c->data.move_to.y;
			}
			cur.x = cmd.definition.move_to.x;
			cur.y = cmd.definition.move_to.y;
			break;

			case ESVG_PATH_LINE_TO:
			//printf(" line_to (%f, %f)\n", c->data.line_to.x,  c->data.line_to.y);
			cmd.type = ENESIM_COMMAND_LINE_TO;
			if (c->relative)
			{
				cmd.definition.line_to.x = cur.x + c->data.line_to.x;
				cmd.definition.line_to.y = cur.y + c->data.line_to.y;
			}
			else
			{
				cmd.definition.line_to.x = c->data.line_to.x;
				cmd.definition.line_to.y = c->data.line_to.y;
			}
			cur.x = cmd.definition.line_to.x;
			cur.y = cmd.definition.line_to.y;
			break;

			case ESVG_PATH_HLINE_TO:
			cmd.type = ENESIM_COMMAND_LINE_TO;
			if (c->relative)
			{
				cmd.definition.line_to.x = cur.x + c->data.hline_to.c;
				cmd.definition.line_to.y = cur.y;
			}
			else
			{
				cmd.definition.line_to.x = c->data.hline_to.c;
				cmd.definition.line_to.y = cur.y;
			}
			cur.x = cmd.definition.line_to.x;
			cur.y = cmd.definition.line_to.y;
			break;

			case ESVG_PATH_VLINE_TO:
			cmd.type = ENESIM_COMMAND_LINE_TO;
			if (c->relative)
			{
				cmd.definition.line_to.x = cur.x;
				cmd.definition.line_to.y = cur.y + c->data.vline_to.c;
			}
			else
			{
				cmd.definition.line_to.x = cur.x;
				cmd.definition.line_to.y = c->data.vline_to.c;
			}
			cur.x = cmd.definition.line_to.x;
			cur.y = cmd.definition.line_to.y;
			break;

			case ESVG_PATH_CUBIC_TO:
			//printf(" cubic_to ((%f, %f) (%f, %f) (%f, %f))\n", c->data.cubic_to.ctrl_x0,  c->data.cubic_to.ctrl_y0, c->data.cubic_to.ctrl_x1,  c->data.cubic_to.ctrl_y1, c->data.cubic_to.x,  c->data.cubic_to.y);
			cmd.type = ENESIM_COMMAND_CUBIC_TO;
			if (c->relative)
			{
				cmd.definition.cubic_to.x = cur.x + c->data.cubic_to.x;
				cmd.definition.cubic_to.y = cur.y + c->data.cubic_to.y;
				cmd.definition.cubic_to.ctrl_x0 = cur.x + c->data.cubic_to.ctrl_x0;
				cmd.definition.cubic_to.ctrl_y0 = cur.y + c->data.cubic_to.ctrl_y0;
				cmd.definition.cubic_to.ctrl_x1 = cur.x + c->data.cubic_to.ctrl_x1;
				cmd.definition.cubic_to.ctrl_y1 = cur.y + c->data.cubic_to.ctrl_y1;

			}
			else
			{
				cmd.definition.cubic_to.x = c->data.cubic_to.x;
				cmd.definition.cubic_to.y = c->data.cubic_to.y;
				cmd.definition.cubic_to.ctrl_x0 = c->data.cubic_to.ctrl_x0;
				cmd.definition.cubic_to.ctrl_y0 = c->data.cubic_to.ctrl_y0;
				cmd.definition.cubic_to.ctrl_x1 = c->data.cubic_to.ctrl_x1;
				cmd.definition.cubic_to.ctrl_y1 = c->data.cubic_to.ctrl_y1;
			}
			cur.x = cmd.definition.cubic_to.x;
			cur.y = cmd.definition.cubic_to.y;
			break;

			case ESVG_PATH_SCUBIC_TO:
			//printf(" scubic_to ((%f, %f) (%f, %f))\n", c->data.scubic_to.ctrl_x,  c->data.scubic_to.ctrl_y, c->data.scubic_to.x,  c->data.scubic_to.y);
			cmd.type = ENESIM_COMMAND_SCUBIC_TO;
			if (c->relative)
			{
				cmd.definition.scubic_to.x = cur.x + c->data.scubic_to.x;
				cmd.definition.scubic_to.y = cur.y + c->data.scubic_to.y;
				cmd.definition.scubic_to.ctrl_x = cur.x + c->data.scubic_to.ctrl_x;
				cmd.definition.scubic_to.ctrl_y = cur.y + c->data.scubic_to.ctrl_y;
			}
			else
			{
				cmd.definition.scubic_to.x = c->data.scubic_to.x;
				cmd.definition.scubic_to.y = c->data.scubic_to.y;
				cmd.definition.scubic_to.ctrl_x = c->data.scubic_to.ctrl_x;
				cmd.definition.scubic_to.ctrl_y = c->data.scubic_to.ctrl_y;
			}
			cur.x = cmd.definition.scubic_to.x;
			cur.y = cmd.definition.scubic_to.y;
			break;

			case ESVG_PATH_QUADRATIC_TO:
			//printf(" quadratic_to ((%f, %f) (%f, %f))\n", c->data.quadratic_to.ctrl_x,  c->data.quadratic_to.ctrl_y, c->data.quadratic_to.x,  c->data.quadratic_to.y);
			cmd.type = ENESIM_COMMAND_QUADRATIC_TO;
			if (c->relative)
			{
				cmd.definition.quadratic_to.x = cur.x + c->data.quadratic_to.x;
				cmd.definition.quadratic_to.y = cur.y + c->data.quadratic_to.y;
				cmd.definition.quadratic_to.ctrl_x = cur.x + c->data.quadratic_to.ctrl_x;
				cmd.definition.quadratic_to.ctrl_y = cur.y + c->data.quadratic_to.ctrl_y;

			}
			else
			{
				cmd.definition.quadratic_to.x = c->data.quadratic_to.x;
				cmd.definition.quadratic_to.y = c->data.quadratic_to.y;
				cmd.definition.quadratic_to.ctrl_x = c->data.quadratic_to.ctrl_x;
				cmd.definition.quadratic_to.ctrl_y = c->data.quadratic_to.ctrl_y;
			}
			cur.x = cmd.definition.quadratic_to.x;
			cur.y = cmd.definition.quadratic_to.y;
			break;

			case ESVG_PATH_SQUADRATIC_TO:
			//printf(" squadratic_to (%f, %f)\n", c->data.squadratic_to.x,  c->data.squadratic_to.y);
			cmd.type = ENESIM_COMMAND_SQUADRATIC_TO;
			if (c->relative)
			{
				cmd.definition.squadratic_to.x = cur.x + c->data.squadratic_to.x;
				cmd.definition.squadratic_to.y = cur.y + c->data.squadratic_to.y;
			}
			else
			{
				cmd.definition.squadratic_to.x = c->data.squadratic_to.x;
				cmd.definition.squadratic_to.y = c->data.squadratic_to.y;
			}
			cur.x = cmd.definition.squadratic_to.x;
			cur.y = cmd.definition.squadratic_to.y;
			break;

			case ESVG_PATH_ARC_TO:
			//printf(" arc_to ((%f, %f) %f %f %f (%f, %f))\n", c->data.arc_to.rx,  c->data.arc_to.ry, c->data.arc_to.angle,  c->data.arc_to.large,  c->data.arc_to.sweep,  c->data.arc_to.x,  c->data.arc_to.y);
			cmd.type = ENESIM_COMMAND_ARC_TO;
			if (c->relative)
			{
				cmd.definition.arc_to.x = cur.x + c->data.arc_to.x;
				cmd.definition.arc_to.y = cur.y + c->data.arc_to.y;
			}
			else
			{
				cmd.definition.arc_to.x = c->data.arc_to.x;
				cmd.definition.arc_to.y = c->data.arc_to.y;
			}
			cmd.definition.arc_to.rx = c->data.arc_to.rx;
			cmd.definition.arc_to.ry = c->data.arc_to.ry;
			cmd.definition.arc_to.angle = c->data.arc_to.angle;
			cmd.definition.arc_to.large = c->data.arc_to.large;
			cmd.definition.arc_to.sweep = c->data.arc_to.sweep;
			cur.x = cmd.definition.arc_to.x;
			cur.y = cmd.definition.arc_to.y;
			break;

			case ESVG_PATH_CLOSE:
			//printf(" close\n");
			cmd.type = ENESIM_COMMAND_CLOSE;
			cmd.definition.close.close = EINA_TRUE;
			break;

			default:
			continue;
			break;
		}
		first = EINA_FALSE;
		enesim_renderer_path_command_add(thiz->r, &cmd);
	}

	return EINA_TRUE;
}

static void _esvg_path_clone(Enesim_Renderer *r, Enesim_Renderer *dr)
{
	Esvg_Path *thiz;
	Esvg_Path_Command *cmd;
	Eina_List *l;

	thiz = _esvg_path_get(r);

	EINA_LIST_FOREACH(thiz->commands, l, cmd)
	{
		Esvg_Path_Command ocmd;

		ocmd = *cmd;
		esvg_path_command_add(dr, &ocmd);
	}
}


static void _esvg_path_cleanup(Enesim_Renderer *r)
{
}

static Esvg_Shape_Descriptor _descriptor = {
	/* .setup =		*/ _esvg_path_setup,
	/* .renderer_get =	*/ _esvg_path_renderer_get,
	/* .name_get =		*/ _esvg_path_name_get,
	/* .clone =		*/ _esvg_path_clone,
	/* .cleanup =		*/ _esvg_path_cleanup,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Enesim_Renderer * esvg_path_new(void)
{
	Esvg_Path *thiz;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Path));
	if (!thiz) return NULL;
	EINA_MAGIC_SET(thiz, ESVG_PATH_MAGIC);

	r = enesim_renderer_path_new();
	thiz->r = r;
	enesim_renderer_rop_set(r, ENESIM_BLEND);
	/* default values */

	r = esvg_shape_new(&_descriptor, thiz);
	return r;
}

EAPI Eina_Bool esvg_is_path(Enesim_Renderer *r)
{
	Esvg_Path *thiz;
	Eina_Bool ret;

	if (!esvg_is_shape(r))
		return EINA_FALSE;
	thiz = esvg_shape_data_get(r);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_PATH_MAGIC);

	return ret;
}

EAPI void esvg_path_command_add(Enesim_Renderer *r, const Esvg_Path_Command *cmd)
{
	Esvg_Path *thiz;
	Esvg_Path_Command *new_cmd;

	thiz = _esvg_path_get(r);

	new_cmd = calloc(1, sizeof(Esvg_Path_Command));
	*new_cmd = *cmd;
	thiz->commands = eina_list_append(thiz->commands, new_cmd);
}
