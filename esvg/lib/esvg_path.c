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
#include "esvg_private_element.h"
#include "esvg_private_renderable.h"
#include "esvg_private_instantiable.h"
#include "esvg_path.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Ender_Property *ESVG_PATH_D;

typedef struct _Esvg_Path
{
	EINA_MAGIC
	/* properties */
	Eina_List *commands;
	/* private */
	Enesim_Renderer *r;
} Esvg_Path;

static Esvg_Path * _esvg_path_get(Edom_Tag *t)
{
	Esvg_Path *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_PATH)
		return NULL;
	thiz = esvg_instantiable_data_get(t);

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
		if (!esvg_parser_path(value, _esvg_path_command_cb, e))
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

static Eina_Bool _esvg_path_setup(Edom_Tag *t,
		Esvg_Element_Context *ctx,
		Esvg_Attribute_Presentation *attr,
		Esvg_Renderable_Context *rctx,
		Enesim_Error **error)
{
	Esvg_Path *thiz;
	Esvg_Path_Command *c;
	Eina_List *l;
	Esvg_Point cur = { 0, 0 };
	Eina_Bool first; /* TODO handle correctly the first flag */

	thiz = _esvg_path_get(t);

	//printf("path setup\n");
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

static void _esvg_path_clone(Edom_Tag *r, Edom_Tag *dr)
{
#if 0
	Esvg_Path *thiz;
	Esvg_Path_Command *cmd;
	Eina_List *l;

	thiz = _esvg_path_get(r);

	EINA_LIST_FOREACH(thiz->commands, l, cmd)
	{
		Esvg_Path_Command ocmd;

		ocmd = *cmd;
		esvg_path_d_add(dr, &ocmd);
	}
#endif
}

void _esvg_path_free(Edom_Tag *t)
{
	Esvg_Path *thiz;

	thiz = _esvg_path_get(t);
	free(thiz);
}

static Esvg_Instantiable_Descriptor _descriptor = {
	/* .child_add		= */ NULL,
	/* .child_remove	= */ NULL,
	/* .attribute_get 	= */ _esvg_path_attribute_get,
	/* .cdata_set 		= */ NULL,
	/* .text_set 		= */ NULL,
	/* .free 		= */ _esvg_path_free,
	/* .initialize 		= */ NULL,
	/* .attribute_set 	= */ _esvg_path_attribute_set,
	/* .clone		= */ _esvg_path_clone,
	/* .setup		= */ _esvg_path_setup,
	/* .renderer_get	= */ _esvg_path_renderer_get,
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

	t = esvg_instantiable_new(&_descriptor, ESVG_PATH, thiz);
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
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* The ender wrapper */
#define _esvg_path_d_set NULL
#define _esvg_path_d_get NULL
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

EAPI void esvg_path_d_add(Ender_Element *e, const Esvg_Path_Command *cmd)
{
	ender_element_property_value_add(e, ESVG_PATH_D, cmd, NULL);
}
