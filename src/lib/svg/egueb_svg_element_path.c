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
#include "egueb_svg_main.h"
#include "egueb_svg_point.h"
#include "egueb_svg_path_seg.h"
#include "egueb_svg_attr_path_seg_list.h"
#include "egueb_svg_element_path.h"
#include "egueb_svg_document.h"
#include "egueb_svg_shape_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_ELEMENT_PATH_DESCRIPTOR egueb_svg_element_path_descriptor_get()
#define EGUEB_SVG_ELEMENT_PATH_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Element_Path_Class, EGUEB_SVG_ELEMENT_PATH_DESCRIPTOR)
#define EGUEB_SVG_ELEMENT_PATH(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Element_Path, EGUEB_SVG_ELEMENT_PATH_DESCRIPTOR)

typedef struct _Egueb_Svg_Element_Path
{
	Egueb_Svg_Shape base;
	/* properties */
	Egueb_Dom_Node *d;
	/* private */
	Enesim_Renderer *r;
} Egueb_Svg_Element_Path;

typedef struct _Egueb_Svg_Element_Path_Class
{
	Egueb_Svg_Shape_Class base;
} Egueb_Svg_Element_Path_Class;

typedef struct _Egueb_Svg_Element_Path_D_Cb_Data
{
	Egueb_Svg_Element_Path *thiz;
	Egueb_Svg_Point cur;
	Eina_Bool first;
} Egueb_Svg_Element_Path_D_Cb_Data;

#if 0
static void _dump_paths(Enesim_Renderer *r)
{
	Eina_List *cmds = NULL;
	Enesim_Path_Command *cmd;

	enesim_renderer_path_command_get(r, &cmds);
	EINA_LIST_FREE (cmds, cmd)
	{
		switch (cmd->type)
		{
			case ENESIM_PATH_COMMAND_MOVE_TO:
			printf("M %g %g\n", cmd->definition.move_to.x, cmd->definition.move_to.y);
			break;

			case ENESIM_PATH_COMMAND_LINE_TO:
			case ENESIM_PATH_COMMAND_QUADRATIC_TO:
			case ENESIM_PATH_COMMAND_SQUADRATIC_TO:
			break;

			case ENESIM_PATH_COMMAND_CUBIC_TO:
			printf("C %g %g %g %g %g %g\n",
					cmd->definition.cubic_to.ctrl_x0,
					cmd->definition.cubic_to.ctrl_y0,
					cmd->definition.cubic_to.ctrl_x1,
					cmd->definition.cubic_to.ctrl_y1,
					cmd->definition.cubic_to.x,
					cmd->definition.cubic_to.y);
			break;

			case ENESIM_PATH_COMMAND_SCUBIC_TO:
			case ENESIM_PATH_COMMAND_ARC_TO:
			break;

			case ENESIM_PATH_COMMAND_CLOSE:
			printf("Z\n");
			break;
		}
	}
}
#endif

static void _egueb_svg_element_path_d_cb(void *data, void *user_data)
{
	Egueb_Svg_Element_Path_D_Cb_Data *cb_data = user_data;
	Egueb_Svg_Element_Path *thiz = cb_data->thiz;
	Egueb_Svg_Path_Seg *pcmd = data;
	Enesim_Path_Command cmd;

	/* FIXME or we either use the generic command API
	 * of enesim or use the direct function call
	 */
	switch (pcmd->type)
	{
		case ESVG_PATH_MOVE_TO:
		DBG("move_to %c (%g, %g)", pcmd->relative ? 'R' : 'A',
				pcmd->data.move_to.x,  pcmd->data.move_to.y);
		cmd.type = ENESIM_PATH_COMMAND_MOVE_TO;
		if (!cb_data->first && pcmd->relative)
		{
			cmd.definition.move_to.x = cb_data->cur.x + pcmd->data.move_to.x;
			cmd.definition.move_to.y = cb_data->cur.y + pcmd->data.move_to.y;
		}
		else
		{
			cmd.definition.move_to.x = pcmd->data.move_to.x;
			cmd.definition.move_to.y = pcmd->data.move_to.y;
		}
		cb_data->cur.x = cmd.definition.move_to.x;
		cb_data->cur.y = cmd.definition.move_to.y;
		break;

		case ESVG_PATH_LINE_TO:
		DBG("line_to %c (%g, %g)", pcmd->relative ? 'R' : 'A',
				pcmd->data.line_to.x,  pcmd->data.line_to.y);
		cmd.type = ENESIM_PATH_COMMAND_LINE_TO;
		if (pcmd->relative)
		{
			cmd.definition.line_to.x = cb_data->cur.x + pcmd->data.line_to.x;
			cmd.definition.line_to.y = cb_data->cur.y + pcmd->data.line_to.y;
		}
		else
		{
			cmd.definition.line_to.x = pcmd->data.line_to.x;
			cmd.definition.line_to.y = pcmd->data.line_to.y;
		}
		cb_data->cur.x = cmd.definition.line_to.x;
		cb_data->cur.y = cmd.definition.line_to.y;
		break;

		case ESVG_PATH_HLINE_TO:
		DBG("hline_to %c (%g)", pcmd->relative ? 'R' : 'A',
				pcmd->data.hline_to.c);
		cmd.type = ENESIM_PATH_COMMAND_LINE_TO;
		if (pcmd->relative)
		{
			cmd.definition.line_to.x = cb_data->cur.x + pcmd->data.hline_to.c;
			cmd.definition.line_to.y = cb_data->cur.y;
		}
		else
		{
			cmd.definition.line_to.x = pcmd->data.hline_to.c;
			cmd.definition.line_to.y = cb_data->cur.y;
		}
		cb_data->cur.x = cmd.definition.line_to.x;
		cb_data->cur.y = cmd.definition.line_to.y;
		break;

		case ESVG_PATH_VLINE_TO:
		DBG("vline_to %c (%g)", pcmd->relative ? 'R' : 'A',
				pcmd->data.vline_to.c);
		cmd.type = ENESIM_PATH_COMMAND_LINE_TO;
		if (pcmd->relative)
		{
			cmd.definition.line_to.x = cb_data->cur.x;
			cmd.definition.line_to.y = cb_data->cur.y + pcmd->data.vline_to.c;
		}
		else
		{
			cmd.definition.line_to.x = cb_data->cur.x;
			cmd.definition.line_to.y = pcmd->data.vline_to.c;
		}
		cb_data->cur.x = cmd.definition.line_to.x;
		cb_data->cur.y = cmd.definition.line_to.y;
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
		cmd.type = ENESIM_PATH_COMMAND_CUBIC_TO;
		if (pcmd->relative)
		{
			cmd.definition.cubic_to.x = cb_data->cur.x + pcmd->data.cubic_to.x;
			cmd.definition.cubic_to.y = cb_data->cur.y + pcmd->data.cubic_to.y;
			cmd.definition.cubic_to.ctrl_x0 = cb_data->cur.x + pcmd->data.cubic_to.ctrl_x0;
			cmd.definition.cubic_to.ctrl_y0 = cb_data->cur.y + pcmd->data.cubic_to.ctrl_y0;
			cmd.definition.cubic_to.ctrl_x1 = cb_data->cur.x + pcmd->data.cubic_to.ctrl_x1;
			cmd.definition.cubic_to.ctrl_y1 = cb_data->cur.y + pcmd->data.cubic_to.ctrl_y1;

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
		cb_data->cur.x = cmd.definition.cubic_to.x;
		cb_data->cur.y = cmd.definition.cubic_to.y;
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
			cmd.definition.scubic_to.x = cb_data->cur.x + pcmd->data.scubic_to.x;
			cmd.definition.scubic_to.y = cb_data->cur.y + pcmd->data.scubic_to.y;
			cmd.definition.scubic_to.ctrl_x = cb_data->cur.x + pcmd->data.scubic_to.ctrl_x;
			cmd.definition.scubic_to.ctrl_y = cb_data->cur.y + pcmd->data.scubic_to.ctrl_y;
		}
		else
		{
			cmd.definition.scubic_to.x = pcmd->data.scubic_to.x;
			cmd.definition.scubic_to.y = pcmd->data.scubic_to.y;
			cmd.definition.scubic_to.ctrl_x = pcmd->data.scubic_to.ctrl_x;
			cmd.definition.scubic_to.ctrl_y = pcmd->data.scubic_to.ctrl_y;
		}
		cb_data->cur.x = cmd.definition.scubic_to.x;
		cb_data->cur.y = cmd.definition.scubic_to.y;
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
			cmd.definition.quadratic_to.x = cb_data->cur.x + pcmd->data.quadratic_to.x;
			cmd.definition.quadratic_to.y = cb_data->cur.y + pcmd->data.quadratic_to.y;
			cmd.definition.quadratic_to.ctrl_x = cb_data->cur.x + pcmd->data.quadratic_to.ctrl_x;
			cmd.definition.quadratic_to.ctrl_y = cb_data->cur.y + pcmd->data.quadratic_to.ctrl_y;

		}
		else
		{
			cmd.definition.quadratic_to.x = pcmd->data.quadratic_to.x;
			cmd.definition.quadratic_to.y = pcmd->data.quadratic_to.y;
			cmd.definition.quadratic_to.ctrl_x = pcmd->data.quadratic_to.ctrl_x;
			cmd.definition.quadratic_to.ctrl_y = pcmd->data.quadratic_to.ctrl_y;
		}
		cb_data->cur.x = cmd.definition.quadratic_to.x;
		cb_data->cur.y = cmd.definition.quadratic_to.y;
		break;

		case ESVG_PATH_SQUADRATIC_TO:
		DBG("squadratic_to %c (%g, %g)",
				pcmd->relative ? 'R' : 'A',
				pcmd->data.squadratic_to.x,
				pcmd->data.squadratic_to.y);
		cmd.type = ENESIM_PATH_COMMAND_SQUADRATIC_TO;
		if (pcmd->relative)
		{
			cmd.definition.squadratic_to.x = cb_data->cur.x + pcmd->data.squadratic_to.x;
			cmd.definition.squadratic_to.y = cb_data->cur.y + pcmd->data.squadratic_to.y;
		}
		else
		{
			cmd.definition.squadratic_to.x = pcmd->data.squadratic_to.x;
			cmd.definition.squadratic_to.y = pcmd->data.squadratic_to.y;
		}
		cb_data->cur.x = cmd.definition.squadratic_to.x;
		cb_data->cur.y = cmd.definition.squadratic_to.y;
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
			cmd.definition.arc_to.x = cb_data->cur.x + pcmd->data.arc_to.x;
			cmd.definition.arc_to.y = cb_data->cur.y + pcmd->data.arc_to.y;
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
		cb_data->cur.x = cmd.definition.arc_to.x;
		cb_data->cur.y = cmd.definition.arc_to.y;
		break;

		case ESVG_PATH_CLOSE:
		DBG("close");
		cmd.type = ENESIM_PATH_COMMAND_CLOSE;
		cmd.definition.close.close = EINA_TRUE;
		break;

		default:
		return;
		break;
	}
	cb_data->first = EINA_FALSE;
	enesim_renderer_path_command_add(thiz->r, &cmd);
}
/*----------------------------------------------------------------------------*
 *                               Shape interface                              *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_element_path_generate_geometry(Egueb_Svg_Shape *s,
		Egueb_Svg_Element *parent, Egueb_Dom_Node *doc)
{
	Egueb_Svg_Element_Path *thiz;
	Egueb_Svg_Element *e;
	Egueb_Dom_List *d;

	thiz = EGUEB_SVG_ELEMENT_PATH(s);

	/* TODO Be sure that we modified the points */
	egueb_dom_attr_final_get(thiz->d, &d);
	enesim_renderer_path_command_clear(thiz->r);
	if (d)
	{
		Egueb_Svg_Element_Path_D_Cb_Data data;
		data.cur.x = data.cur.y = 0;
		data.thiz = thiz;
		data.first = EINA_TRUE;

		egueb_dom_list_foreach(d, _egueb_svg_element_path_d_cb, &data);
		egueb_dom_list_unref(d);
	}

	e = EGUEB_SVG_ELEMENT(s);
	/* set the transformation */
	enesim_renderer_transformation_set(thiz->r, &e->transform);
done:
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                            Renderable interface                            *
 *----------------------------------------------------------------------------*/
static Enesim_Renderer * _egueb_svg_element_path_renderer_get(
		Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Element_Path *thiz;

	thiz = EGUEB_SVG_ELEMENT_PATH(r);
	return enesim_renderer_ref(thiz->r);
}

static void _egueb_svg_element_path_bounds_get(Egueb_Svg_Renderable *r,
		Enesim_Rectangle *bounds)
{
	Egueb_Svg_Element_Path *thiz;

	thiz = EGUEB_SVG_ELEMENT_PATH(r);
}
/*----------------------------------------------------------------------------*
 *                              Element interface                             *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_String * _egueb_svg_element_path_tag_name_get(
		Egueb_Dom_Element *e)
{
	return egueb_dom_string_ref(EGUEB_SVG_NAME_PATH);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_path, Egueb_Svg_Element_Path, d);

ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_SHAPE_DESCRIPTOR,
		Egueb_Svg_Element_Path, Egueb_Svg_Element_Path_Class,
		egueb_svg_element_path);

static void _egueb_svg_element_path_class_init(void *k)
{
	Egueb_Svg_Shape_Class *klass;
	Egueb_Svg_Renderable_Class *r_klass;
	Egueb_Dom_Element_Class *e_klass;

	klass = EGUEB_SVG_SHAPE_CLASS(k);
	klass->generate_geometry = _egueb_svg_element_path_generate_geometry;

	r_klass = EGUEB_SVG_RENDERABLE_CLASS(k);
	r_klass->bounds_get = _egueb_svg_element_path_bounds_get;
	r_klass->renderer_get = _egueb_svg_element_path_renderer_get;

	e_klass= EGUEB_DOM_ELEMENT_CLASS(k);
	e_klass->tag_name_get = _egueb_svg_element_path_tag_name_get;
}

static void _egueb_svg_element_path_class_deinit(void *k)
{
}

static void _egueb_svg_element_path_instance_init(void *o)
{
	Egueb_Svg_Element_Path *thiz;
	Enesim_Renderer *r;

	thiz = EGUEB_SVG_ELEMENT_PATH(o);
	r = enesim_renderer_path_new();
	thiz->r = r;

	/* Default values */
	enesim_renderer_rop_set(thiz->r, ENESIM_BLEND);

	/* create the properties */
	thiz->d = egueb_svg_attr_path_seg_list_new(
			egueb_dom_string_ref(EGUEB_SVG_D),
			NULL, EINA_TRUE, EINA_FALSE, EINA_FALSE);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_path, d);
}

static void _egueb_svg_element_path_instance_deinit(void *o)
{
	Egueb_Svg_Element_Path *thiz;

	thiz = EGUEB_SVG_ELEMENT_PATH(o);
	enesim_renderer_unref(thiz->r);
	/* destroy the properties */
	egueb_dom_node_unref(thiz->d);
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_svg_element_path_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_element_path);
	return n;
}

