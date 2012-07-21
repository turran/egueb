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
#include "esvg_private_renderable.h"
#include "esvg_path.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT esvg_log_path

static Ender_Property *ESVG_PATH_D;

typedef struct _Esvg_Path
{
	EINA_MAGIC
	/* properties */
	Eina_List *commands;
	/* private */
	Eina_Bool d_changed : 1;
	Enesim_Renderer *r;
} Esvg_Path;

static Esvg_Path * _esvg_path_get(Edom_Tag *t)
{
	Esvg_Path *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_PATH)
		return NULL;
	thiz = esvg_renderable_data_get(t);

	return thiz;
}

static void _esvg_path_command_cb(Esvg_Path_Command *cmd, void *data)
{
	Ender_Element *e = data;

	esvg_path_d_add(e, cmd);
}
/*----------------------------------------------------------------------------*
 *                       The Esvg Renderable interface                        *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_path_attribute_set(Ender_Element *e, const char *key, const char *value)
{
	if (strcmp(key, "d") == 0)
	{
		if (!esvg_path_string_from(value, _esvg_path_command_cb, e))
			return EINA_FALSE;
	}

	return EINA_TRUE;
}

static Eina_Bool _esvg_path_attribute_get(Edom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static Enesim_Renderer * _esvg_path_renderer_get(Edom_Tag *t)
{
	Esvg_Path *thiz;

	thiz = _esvg_path_get(t);
	return thiz->r;
}

static Esvg_Element_Setup_Return _esvg_path_setup(Edom_Tag *t,
		Esvg_Context *c,
		Esvg_Element_Context *ctx,
		Esvg_Attribute_Presentation *attr,
		Enesim_Error **error)
{
	return ESVG_SETUP_OK;
}

static Eina_Bool _esvg_path_renderer_propagate(Edom_Tag *t,
		Esvg_Context *c,
		const Esvg_Element_Context *ctx,
		const Esvg_Attribute_Presentation *attr,
		Esvg_Renderable_Context *rctx,
		Enesim_Error **error)
{
	Esvg_Path *thiz;
	Esvg_Path_Command *pcmd;
	Eina_List *l;
	Esvg_Point cur = { 0, 0 };
	Eina_Bool first; /* TODO handle correctly the first flag */

	thiz = _esvg_path_get(t);

	//DBG("path setup");
	/* shape properties */
	if (!rctx->fill_renderer)
		enesim_renderer_shape_fill_color_set(thiz->r, rctx->fill_color);
	else
		enesim_renderer_shape_fill_renderer_set(thiz->r, rctx->fill_renderer);

	if (!rctx->stroke_renderer)
		enesim_renderer_shape_stroke_color_set(thiz->r, rctx->stroke_color);
	else
		enesim_renderer_shape_stroke_renderer_set(thiz->r, rctx->stroke_renderer);

	enesim_renderer_shape_fill_rule_set(thiz->r, rctx->fill_rule);
	enesim_renderer_shape_stroke_weight_set(thiz->r, rctx->stroke_weight);
	enesim_renderer_shape_stroke_location_set(thiz->r, ENESIM_SHAPE_STROKE_CENTER);
	enesim_renderer_shape_stroke_cap_set(thiz->r, rctx->stroke_cap);
	enesim_renderer_shape_stroke_join_set(thiz->r, rctx->stroke_join);
	enesim_renderer_shape_draw_mode_set(thiz->r, rctx->draw_mode);
	/* base properties */
	enesim_renderer_geometry_transformation_set(thiz->r, &ctx->transform);
	enesim_renderer_color_set(thiz->r, rctx->color);

	if (!thiz->d_changed)
		return EINA_TRUE;

	/* we need to generate again the commands */
	enesim_renderer_path_command_clear(thiz->r);
	EINA_LIST_FOREACH(thiz->commands, l, pcmd)
	{
		Enesim_Renderer_Path_Command cmd;
		/* FIXME or we either use the generic command API
		 * of enesim or use the direct function call
		 */
		switch (pcmd->type)
		{
			case ESVG_PATH_MOVE_TO:
			DBG("move_to %c (%g, %g)", pcmd->relative ? 'R' : 'A',
					pcmd->data.move_to.x,  pcmd->data.move_to.y);
			cmd.type = ENESIM_COMMAND_MOVE_TO;
			if (!first && pcmd->relative)
			{
				cmd.definition.move_to.x = cur.x + pcmd->data.move_to.x;
				cmd.definition.move_to.y = cur.y + pcmd->data.move_to.y;
			}
			else
			{
				cmd.definition.move_to.x = pcmd->data.move_to.x;
				cmd.definition.move_to.y = pcmd->data.move_to.y;
			}
			cur.x = cmd.definition.move_to.x;
			cur.y = cmd.definition.move_to.y;
			break;

			case ESVG_PATH_LINE_TO:
			DBG("line_to %c (%g, %g)", pcmd->relative ? 'R' : 'A',
					pcmd->data.line_to.x,  pcmd->data.line_to.y);
			cmd.type = ENESIM_COMMAND_LINE_TO;
			if (pcmd->relative)
			{
				cmd.definition.line_to.x = cur.x + pcmd->data.line_to.x;
				cmd.definition.line_to.y = cur.y + pcmd->data.line_to.y;
			}
			else
			{
				cmd.definition.line_to.x = pcmd->data.line_to.x;
				cmd.definition.line_to.y = pcmd->data.line_to.y;
			}
			cur.x = cmd.definition.line_to.x;
			cur.y = cmd.definition.line_to.y;
			break;

			case ESVG_PATH_HLINE_TO:
			DBG("hline_to %c (%g)", pcmd->relative ? 'R' : 'A',
					pcmd->data.hline_to.c);
			cmd.type = ENESIM_COMMAND_LINE_TO;
			if (pcmd->relative)
			{
				cmd.definition.line_to.x = cur.x + pcmd->data.hline_to.c;
				cmd.definition.line_to.y = cur.y;
			}
			else
			{
				cmd.definition.line_to.x = pcmd->data.hline_to.c;
				cmd.definition.line_to.y = cur.y;
			}
			cur.x = cmd.definition.line_to.x;
			cur.y = cmd.definition.line_to.y;
			break;

			case ESVG_PATH_VLINE_TO:
			DBG("vline_to %c (%g)", pcmd->relative ? 'R' : 'A',
					pcmd->data.vline_to.c);
			cmd.type = ENESIM_COMMAND_LINE_TO;
			if (pcmd->relative)
			{
				cmd.definition.line_to.x = cur.x;
				cmd.definition.line_to.y = cur.y + pcmd->data.vline_to.c;
			}
			else
			{
				cmd.definition.line_to.x = cur.x;
				cmd.definition.line_to.y = pcmd->data.vline_to.c;
			}
			cur.x = cmd.definition.line_to.x;
			cur.y = cmd.definition.line_to.y;
			break;

			case ESVG_PATH_CUBIC_TO:
			DBG("cubic_to %c ((%g, %g) (%g, %g) (%g, %g))",
					pcmd->relative ? 'R' : 'A',
					pcmd->data.cubic_to.ctrl_x0,
					pcmd->data.cubic_to.ctrl_y0,
					pcmd->data.cubic_to.ctrl_x1,
					pcmd->data.cubic_to.ctrl_y1,
					pcmd->data.cubic_to.x,
					pcmd->data.cubic_to.y);
			cmd.type = ENESIM_COMMAND_CUBIC_TO;
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
			cmd.type = ENESIM_COMMAND_SCUBIC_TO;
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
			cmd.type = ENESIM_COMMAND_QUADRATIC_TO;
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
			cmd.type = ENESIM_COMMAND_SQUADRATIC_TO;
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
			cmd.type = ENESIM_COMMAND_ARC_TO;
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
	thiz->d_changed = EINA_FALSE;

	return EINA_TRUE;
}

void _esvg_path_free(Edom_Tag *t)
{
	Esvg_Path *thiz;
	Esvg_Path_Command *cmd;

	thiz = _esvg_path_get(t);
	EINA_LIST_FREE(thiz->commands, cmd)
	{
		free(cmd);
	}
	enesim_renderer_unref(thiz->r);
	free(thiz);
}

static Esvg_Renderable_Descriptor _descriptor = {
	/* .child_add		= */ NULL,
	/* .child_remove	= */ NULL,
	/* .attribute_get 	= */ _esvg_path_attribute_get,
	/* .cdata_set 		= */ NULL,
	/* .text_set 		= */ NULL,
	/* .free 		= */ _esvg_path_free,
	/* .initialize 		= */ NULL,
	/* .attribute_set 	= */ _esvg_path_attribute_set,
	/* .setup		= */ _esvg_path_setup,
	/* .renderer_get	= */ _esvg_path_renderer_get,
	/* .renderer_propagate	= */ _esvg_path_renderer_propagate,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_path_new(void)
{
	Esvg_Path *thiz;
	Edom_Tag *t;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Path));
	if (!thiz) return NULL;

	r = enesim_renderer_path_new();
	thiz->r = r;
	enesim_renderer_rop_set(r, ENESIM_BLEND);
	/* default values */

	t = esvg_renderable_new(&_descriptor, ESVG_PATH, thiz);
	return t;
}

static void _esvg_path_d_add(Edom_Tag *t, const Esvg_Path_Command *cmd)
{
	Esvg_Path *thiz;
	Esvg_Path_Command *new_cmd;

	thiz = _esvg_path_get(t);

	new_cmd = calloc(1, sizeof(Esvg_Path_Command));
	*new_cmd = *cmd;
	thiz->commands = eina_list_append(thiz->commands, new_cmd);
	thiz->d_changed = EINA_TRUE;
}

static void _esvg_path_d_set(Edom_Tag *t, const Eina_List *cmds)
{
	Esvg_Path *thiz;
	Esvg_Path_Command *cmd;
	const Eina_List *l;

	thiz = _esvg_path_get(t);
	/* FIXME remove what we had */
	EINA_LIST_FOREACH (cmds, l, cmd)
	{
		_esvg_path_d_add(t, cmd);
	}
}

static void _esvg_path_d_get(Edom_Tag *t, const Eina_List **cmds)
{
	Esvg_Path *thiz;

	thiz = _esvg_path_get(t);
	*cmds = thiz->commands;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* The ender wrapper */
#define _esvg_path_delete NULL
#define _esvg_path_d_clear NULL
#define _esvg_path_d_remove NULL
#define _esvg_path_d_is_set NULL
#include "generated/esvg_generated_path.c"
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * esvg_path_new(void)
{
	return ender_element_new_with_namespace("path", "esvg");
}

EAPI Eina_Bool esvg_is_path(Ender_Element *e)
{
	Eina_Bool ret = EINA_TRUE;

	return ret;
}

EAPI void esvg_path_d_set(Ender_Element *e, const Eina_List *cmds)
{
	ender_element_property_value_set(e, ESVG_PATH_D, cmds, NULL);
}

EAPI void esvg_path_d_get(Ender_Element *e, const Eina_List **cmds)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	_esvg_path_d_get(t, cmds);
}

EAPI void esvg_path_d_add(Ender_Element *e, const Esvg_Path_Command *cmd)
{
	ender_element_property_value_add(e, ESVG_PATH_D, cmd, NULL);
}
