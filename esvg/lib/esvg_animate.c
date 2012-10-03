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
#include "esvg_private_attribute_animation.h"
#include "esvg_private_animation.h"
#include "esvg_private_animate_base.h"
#include "esvg_private_svg.h"

#include "esvg_animate.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Esvg_Animate
{
	/* properties */
	/* interface */
	/* private */
	/* only used for the path commands */
	/* here we store the value that is going to be set */
	Eina_List *cmds;
	/* here the animate_path_command to keep track of the animations
	 * on every command found and the destination command */
	Eina_List *animations;
} Esvg_Animate;

static Esvg_Animate * _esvg_animate_get(Edom_Tag *t)
{
	Esvg_Animate *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_ANIMATE)
		return NULL;
	thiz = esvg_animate_base_data_get(t);

	return thiz;
}

/*----------------------------------------------------------------------------*
 *                               generic helpers                              *
 *----------------------------------------------------------------------------*/
static void _esvg_animate_double_cb(Edom_Tag *t,
		Ender_Element *e,
		Ender_Property *p,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *kdata,
		void *data)
{
	double *d = kdata;
	*d = curr->data.d;
}

static void _esvg_animate_bool_cb(Edom_Tag *t,
		Ender_Element *e,
		Ender_Property *p,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *kdata,
		void *data)
{
	Eina_Bool *d = kdata;
	*d = curr->data.u32;
}

static void * _esvg_animate_value_get(const char *attr)
{
	printf("attr = %s\n", attr);
}
/*----------------------------------------------------------------------------*
 *                        The Esvg_Paint type descriptor                      *
 *----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*
 *                       The Esvg_Length type descriptor                      *
 *----------------------------------------------------------------------------*/
static void _esvg_animate_length_cb(Edom_Tag *t,
		Ender_Element *e,
		Ender_Property *p,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *kdata,
		void *data)
{
	Esvg_Length *length = kdata;
	Esvg_Animated_Length v;

	v.base.value = curr->data.d;
	v.base.unit = length->unit;

	ender_element_property_value_set(e, p, &v, NULL);
}

static Eina_Bool _esvg_animate_length_get(const char *attr, void **value)
{
	Esvg_Length *v;

	v = calloc(1, sizeof(Esvg_Length));
	esvg_length_string_from(v, attr);
	*value = v;
	return EINA_TRUE;
}

static void _esvg_animate_length_etch_data_to(void *d,
		Etch_Data *data)
{
	Esvg_Length *v = d;

	data->type = ETCH_DOUBLE;
	data->data.d = v->value;
}

static Eina_Bool _esvg_animate_length_animation_generate(Edom_Tag *t,
		Eina_List *values,
		Eina_List *times,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx)
{
	esvg_animate_base_animation_generate(t, values, times, actx, abctx, ETCH_DOUBLE,
			_esvg_animate_length_etch_data_to, _esvg_animate_length_cb, NULL);
	return EINA_TRUE;
}

static Esvg_Animate_Base_Type_Descriptor _length_descriptor = {
	/* .value_get 		= */ _esvg_animate_length_get,
	/* .value_free 		= */ free,
	/* .animation_generate 	= */ _esvg_animate_length_animation_generate,
};
/*----------------------------------------------------------------------------*
 *                        The number type descriptor                          *
 *----------------------------------------------------------------------------*/
static void _esvg_animate_number_cb(Edom_Tag *t,
		Ender_Element *e,
		Ender_Property *p,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *kdata,
		void *data)
{
	Esvg_Animated_Number v;

	v.base = curr->data.d;
	ender_element_property_value_set(e, p, &v, NULL);
}

static void _esvg_animate_number_etch_data_to(void *d,
		Etch_Data *data)
{
	double *v = d;

	data->type = ETCH_DOUBLE;
	data->data.d = *v;
}

static Eina_Bool _esvg_animate_number_get(const char *attr, void **value)
{
	double *v;

	v = calloc(1, sizeof(double));
	*v = esvg_number_string_from(attr, 1.0);
	*value = v;
	return EINA_TRUE;
}

static Eina_Bool _esvg_animate_number_animation_generate(Edom_Tag *t,
		Eina_List *values,
		Eina_List *times,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx)
{
	esvg_animate_base_animation_generate(t, values, times, actx, abctx, ETCH_DOUBLE,
			_esvg_animate_number_etch_data_to, _esvg_animate_number_cb, NULL);
	return EINA_TRUE;
}

static Esvg_Animate_Base_Type_Descriptor _number_descriptor = {
	/* .value_get 		= */ _esvg_animate_number_get,
	/* .value_free 		= */ free,
	/* .animation_generate 	= */ _esvg_animate_number_animation_generate,
};
/*----------------------------------------------------------------------------*
 *                      The string type descriptor                            *
 *----------------------------------------------------------------------------*/
static void _esvg_animate_string_cb(Edom_Tag *t,
		Ender_Element *e,
		Ender_Property *p,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *kdata,
		void *data)
{
	Esvg_Animated_String v;

	v.base = curr->data.string;
	ender_element_property_value_set(e, p, &v, NULL);
}

static Eina_Bool _esvg_animate_string_get(const char *attr, void **value)
{
	char *v;

	v = strdup(attr);
	*value = v;
	return EINA_TRUE;
}

static void _esvg_animate_string_etch_data_to(void *d,
		Etch_Data *data)
{
	char *v = d;

	data->type = ETCH_STRING;
	data->data.string = v;
}

static Eina_Bool _esvg_animate_string_animation_generate(Edom_Tag *t,
		Eina_List *values,
		Eina_List *times,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx)
{
	esvg_animate_base_animation_generate(t, values, times, actx, abctx, ETCH_STRING,
			_esvg_animate_string_etch_data_to, _esvg_animate_string_cb, NULL);
	return EINA_TRUE;
}

static Esvg_Animate_Base_Type_Descriptor _string_descriptor = {
	/* .value_get 		= */ _esvg_animate_string_get,
	/* .value_free 		= */ free,
	/* .animation_generate 	= */ _esvg_animate_string_animation_generate,
};
/*----------------------------------------------------------------------------*
 *                   The path command type descriptor                         *
 *----------------------------------------------------------------------------*/
static void  _esvg_animate_path_command_get_cb(Esvg_Path_Command *cmd, void *data)
{
	Esvg_Path_Command *pcmd;
	Eina_List **dst = data;

	pcmd = calloc(1, sizeof(Esvg_Path_Command));
	*pcmd = *cmd;
	*dst = eina_list_append(*dst, pcmd);
}

static Eina_Bool _esvg_animate_path_command_get(const char *attr, void **value)
{
	Eina_List *l = NULL;
	esvg_path_string_from(attr, _esvg_animate_path_command_get_cb, &l);

	*value = l;
	return EINA_TRUE;
}

static void _esvg_animate_path_command_free(void *d)
{
	Esvg_Path_Command *pcmd;
	Eina_List *l = d;

	EINA_LIST_FREE(l, pcmd)
		free(pcmd);
}

static void _esvg_animate_path_command_double_final_cb(Edom_Tag *t,
		Ender_Element *e,
		Ender_Property *p,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *kdata,
		void *data)
{
	Esvg_Animated_List a;
	Edom_Tag *parent_t;
	double *d = kdata;

	parent_t = ender_element_object_get(e);
	*d = curr->data.d;
	a.base = data;
	a.anim = NULL;

	esvg_element_attribute_animate_set(parent_t, EINA_TRUE);
	ender_element_property_value_set(e, p, &a, NULL);
	esvg_element_attribute_animate_set(parent_t, EINA_FALSE);
}

static void _esvg_animate_path_command_move_to_keyframes_add(Esvg_Animate_Base_Context *abctx,
		Esvg_Path_Command *src,
		Esvg_Path_Command *dst,
		Etch_Animation **a,
		int64_t t)
{
	Etch_Data data;

	data.type = ETCH_DOUBLE;
	data.data.d = src->data.move_to.x;
	esvg_animate_base_animation_add_keyframe(a[0], abctx, &data, t, &dst->data.move_to.x);
	data.data.d = src->data.move_to.y;
	esvg_animate_base_animation_add_keyframe(a[1], abctx, &data, t, &dst->data.move_to.y);
}

static void _esvg_animate_path_command_hline_to_keyframes_add(Esvg_Animate_Base_Context *abctx,
		Esvg_Path_Command *src,
		Esvg_Path_Command *dst,
		Etch_Animation **a,
		int64_t t)
{
	Etch_Data data;

	data.type = ETCH_DOUBLE;
	data.data.d = src->data.hline_to.c;
	esvg_animate_base_animation_add_keyframe(a[0], abctx, &data, t, &dst->data.hline_to.c);
}

static void _esvg_animate_path_command_vline_to_keyframes_add(Esvg_Animate_Base_Context *abctx,
		Esvg_Path_Command *src,
		Esvg_Path_Command *dst,
		Etch_Animation **a,
		int64_t t)
{
	Etch_Data data;

	data.type = ETCH_DOUBLE;
	data.data.d = src->data.vline_to.c;
	esvg_animate_base_animation_add_keyframe(a[0], abctx, &data, t, &dst->data.vline_to.c);
}

static void _esvg_animate_path_command_line_to_keyframes_add(Esvg_Animate_Base_Context *abctx,
		Esvg_Path_Command *src,
		Esvg_Path_Command *dst,
		Etch_Animation **a,
		int64_t t)
{
	Etch_Data data;

	data.type = ETCH_DOUBLE;
	data.data.d = src->data.line_to.x;
	esvg_animate_base_animation_add_keyframe(a[0], abctx, &data, t, &dst->data.line_to.x);
	data.data.d = src->data.line_to.y;
	esvg_animate_base_animation_add_keyframe(a[1], abctx, &data, t, &dst->data.line_to.y);
}

static void _esvg_animate_path_command_squadratic_to_keyframes_add(Esvg_Animate_Base_Context *abctx,
		Esvg_Path_Command *src,
		Esvg_Path_Command *dst,
		Etch_Animation **a,
		int64_t t)
{
	Etch_Data data;

	data.type = ETCH_DOUBLE;
	data.data.d = src->data.squadratic_to.x;
	esvg_animate_base_animation_add_keyframe(a[0], abctx, &data, t, &dst->data.squadratic_to.x);
	data.data.d = src->data.squadratic_to.y;
	esvg_animate_base_animation_add_keyframe(a[1], abctx, &data, t, &dst->data.squadratic_to.y);
}

static void _esvg_animate_path_command_quadratic_to_keyframes_add(Esvg_Animate_Base_Context *abctx,
		Esvg_Path_Command *src,
		Esvg_Path_Command *dst,
		Etch_Animation **a,
		int64_t t)
{
	Etch_Data data;

	data.type = ETCH_DOUBLE;
	data.data.d = src->data.quadratic_to.x;
	esvg_animate_base_animation_add_keyframe(a[0], abctx, &data, t, &dst->data.quadratic_to.x);
	data.data.d = src->data.quadratic_to.y;
	esvg_animate_base_animation_add_keyframe(a[1], abctx, &data, t, &dst->data.quadratic_to.y);
	data.data.d = src->data.quadratic_to.ctrl_x;
	esvg_animate_base_animation_add_keyframe(a[2], abctx, &data, t, &dst->data.quadratic_to.ctrl_x);
	data.data.d = src->data.quadratic_to.ctrl_y;
	esvg_animate_base_animation_add_keyframe(a[3], abctx, &data, t, &dst->data.quadratic_to.ctrl_y);
}

static void _esvg_animate_path_command_cubic_to_keyframes_add(Esvg_Animate_Base_Context *abctx,
		Esvg_Path_Command *src,
		Esvg_Path_Command *dst,
		Etch_Animation **a,
		int64_t t)
{
	Etch_Data data;

	data.type = ETCH_DOUBLE;
	data.data.d = src->data.cubic_to.x;
	esvg_animate_base_animation_add_keyframe(a[0], abctx, &data, t, &dst->data.cubic_to.x);
	data.data.d = src->data.cubic_to.y;
	esvg_animate_base_animation_add_keyframe(a[1], abctx, &data, t, &dst->data.cubic_to.y);
	data.data.d = src->data.cubic_to.ctrl_x0;
	esvg_animate_base_animation_add_keyframe(a[2], abctx, &data, t, &dst->data.cubic_to.ctrl_x0);
	data.data.d = src->data.cubic_to.ctrl_y0;
	esvg_animate_base_animation_add_keyframe(a[3], abctx, &data, t, &dst->data.cubic_to.ctrl_y0);
	data.data.d = src->data.cubic_to.ctrl_x1;
	esvg_animate_base_animation_add_keyframe(a[4], abctx, &data, t, &dst->data.cubic_to.ctrl_x1);
	data.data.d = src->data.cubic_to.ctrl_y1;
	esvg_animate_base_animation_add_keyframe(a[5], abctx, &data, t, &dst->data.cubic_to.ctrl_y1);
}

static void _esvg_animate_path_command_scubic_to_keyframes_add(Esvg_Animate_Base_Context *abctx,
		Esvg_Path_Command *src,
		Esvg_Path_Command *dst,
		Etch_Animation **a,
		int64_t t)
{
	Etch_Data data;

	data.type = ETCH_DOUBLE;
	data.data.d = src->data.scubic_to.x;
	esvg_animate_base_animation_add_keyframe(a[0], abctx, &data, t, &dst->data.scubic_to.x);
	data.data.d = src->data.scubic_to.y;
	esvg_animate_base_animation_add_keyframe(a[1], abctx, &data, t, &dst->data.scubic_to.y);
	data.data.d = src->data.scubic_to.ctrl_x;
	esvg_animate_base_animation_add_keyframe(a[2], abctx, &data, t, &dst->data.scubic_to.ctrl_x);
	data.data.d = src->data.scubic_to.ctrl_y;
	esvg_animate_base_animation_add_keyframe(a[3], abctx, &data, t, &dst->data.scubic_to.ctrl_y);
}

static void _esvg_animate_path_command_1doubles_animation_create(Edom_Tag *t,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx,
		Esvg_Path_Command *src,
		Esvg_Path_Command *dst,
		Etch_Animation **a,
		Eina_List *cmds,
		int64_t time,
		Eina_Bool final)
{
	if (final)
	{
		a[0] = esvg_animate_base_animation_empty_add(t, ETCH_DOUBLE, actx, abctx,
				_esvg_animate_path_command_double_final_cb, cmds);
	}
	else
	{
		a[0] = esvg_animate_base_animation_empty_add(t, ETCH_DOUBLE, actx, abctx,
				_esvg_animate_double_cb, cmds);
	}
}

static void _esvg_animate_path_command_2doubles_animation_create(Edom_Tag *t,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx,
		Esvg_Path_Command *src,
		Esvg_Path_Command *dst,
		Etch_Animation **a,
		Eina_List *cmds,
		int64_t time,
		Eina_Bool final)
{
	a[0] = esvg_animate_base_animation_empty_add(t, ETCH_DOUBLE, actx, abctx,
			_esvg_animate_double_cb, cmds);
	if (final)
	{
		a[1] = esvg_animate_base_animation_empty_add(t, ETCH_DOUBLE, actx, abctx,
				_esvg_animate_path_command_double_final_cb, cmds);
	}
	else
	{
		a[1] = esvg_animate_base_animation_empty_add(t, ETCH_DOUBLE, actx, abctx,
				_esvg_animate_double_cb, cmds);
	}
}

static void _esvg_animate_path_command_4doubles_animation_create(Edom_Tag *t,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx,
		Esvg_Path_Command *src,
		Esvg_Path_Command *dst,
		Etch_Animation **a,
		Eina_List *cmds,
		int64_t time,
		Eina_Bool final)
{
	a[0] = esvg_animate_base_animation_empty_add(t, ETCH_DOUBLE, actx, abctx,
			_esvg_animate_double_cb, cmds);
	a[1] = esvg_animate_base_animation_empty_add(t, ETCH_DOUBLE, actx, abctx,
			_esvg_animate_double_cb, cmds);
	a[2] = esvg_animate_base_animation_empty_add(t, ETCH_DOUBLE, actx, abctx,
			_esvg_animate_double_cb, cmds);
	if (final)
	{
		a[3] = esvg_animate_base_animation_empty_add(t, ETCH_DOUBLE, actx, abctx,
				_esvg_animate_path_command_double_final_cb, cmds);
	}
	else
	{
		a[3] = esvg_animate_base_animation_empty_add(t, ETCH_DOUBLE, actx, abctx,
				_esvg_animate_double_cb, cmds);
	}
}

static void _esvg_animate_path_command_6doubles_animation_create(Edom_Tag *t,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx,
		Esvg_Path_Command *src,
		Esvg_Path_Command *dst,
		Etch_Animation **a,
		Eina_List *cmds,
		int64_t time,
		Eina_Bool final)
{
	a[0] = esvg_animate_base_animation_empty_add(t, ETCH_DOUBLE, actx, abctx,
			_esvg_animate_double_cb, cmds);
	a[1] = esvg_animate_base_animation_empty_add(t, ETCH_DOUBLE, actx, abctx,
			_esvg_animate_double_cb, cmds);
	a[2] = esvg_animate_base_animation_empty_add(t, ETCH_DOUBLE, actx, abctx,
			_esvg_animate_double_cb, cmds);
	a[3] = esvg_animate_base_animation_empty_add(t, ETCH_DOUBLE, actx, abctx,
			_esvg_animate_double_cb, cmds);
	a[4] = esvg_animate_base_animation_empty_add(t, ETCH_DOUBLE, actx, abctx,
			_esvg_animate_double_cb, cmds);
	if (final)
	{
		a[5] = esvg_animate_base_animation_empty_add(t, ETCH_DOUBLE, actx, abctx,
				_esvg_animate_path_command_double_final_cb, cmds);
	}
	else
	{
		a[5] = esvg_animate_base_animation_empty_add(t, ETCH_DOUBLE, actx, abctx,
				_esvg_animate_double_cb, cmds);
	}
}

static void _esvg_animate_path_command_move_to_animation_create(Edom_Tag *t,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx,
		Esvg_Path_Command *src,
		Esvg_Path_Command *dst,
		Etch_Animation **a,
		Eina_List *cmds,
		int64_t time,
		Eina_Bool final)
{
	_esvg_animate_path_command_2doubles_animation_create(t, actx, abctx, src, dst, a, cmds, time, final);
	_esvg_animate_path_command_move_to_keyframes_add(abctx, src, dst, a, time);
}

static void _esvg_animate_path_command_line_to_animation_create(Edom_Tag *t,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx,
		Esvg_Path_Command *src,
		Esvg_Path_Command *dst,
		Etch_Animation **a,
		Eina_List *cmds,
		int64_t time,
		Eina_Bool final)
{
	_esvg_animate_path_command_2doubles_animation_create(t, actx, abctx, src, dst, a, cmds, time, final);
	_esvg_animate_path_command_line_to_keyframes_add(abctx, src, dst, a, time);
}

static void _esvg_animate_path_command_squadratic_to_animation_create(Edom_Tag *t,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx,
		Esvg_Path_Command *src,
		Esvg_Path_Command *dst,
		Etch_Animation **a,
		Eina_List *cmds,
		int64_t time,
		Eina_Bool final)
{
	_esvg_animate_path_command_2doubles_animation_create(t, actx, abctx, src, dst, a, cmds, time, final);
	_esvg_animate_path_command_squadratic_to_keyframes_add(abctx, src, dst, a, time);
}

static void _esvg_animate_path_command_quadratic_to_animation_create(Edom_Tag *t,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx,
		Esvg_Path_Command *src,
		Esvg_Path_Command *dst,
		Etch_Animation **a,
		Eina_List *cmds,
		int64_t time,
		Eina_Bool final)
{
	_esvg_animate_path_command_4doubles_animation_create(t, actx, abctx, src, dst, a, cmds, time, final);
	_esvg_animate_path_command_quadratic_to_keyframes_add(abctx, src, dst, a, time);
}

static void _esvg_animate_path_command_scubic_to_animation_create(Edom_Tag *t,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx,
		Esvg_Path_Command *src,
		Esvg_Path_Command *dst,
		Etch_Animation **a,
		Eina_List *cmds,
		int64_t time,
		Eina_Bool final)
{
	_esvg_animate_path_command_4doubles_animation_create(t, actx, abctx, src, dst, a, cmds, time, final);
	_esvg_animate_path_command_scubic_to_keyframes_add(abctx, src, dst, a, time);
}

static void _esvg_animate_path_command_hline_to_animation_create(Edom_Tag *t,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx,
		Esvg_Path_Command *src,
		Esvg_Path_Command *dst,
		Etch_Animation **a,
		Eina_List *cmds,
		int64_t time,
		Eina_Bool final)
{
	_esvg_animate_path_command_1doubles_animation_create(t, actx, abctx, src, dst, a, cmds, time, final);
	_esvg_animate_path_command_hline_to_keyframes_add(abctx, src, dst, a, time);
}

static void _esvg_animate_path_command_vline_to_animation_create(Edom_Tag *t,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx,
		Esvg_Path_Command *src,
		Esvg_Path_Command *dst,
		Etch_Animation **a,
		Eina_List *cmds,
		int64_t time,
		Eina_Bool final)
{
	_esvg_animate_path_command_1doubles_animation_create(t, actx, abctx, src, dst, a, cmds, time, final);
	_esvg_animate_path_command_vline_to_keyframes_add(abctx, src, dst, a, time);
}

static void _esvg_animate_path_command_cubic_to_animation_create(Edom_Tag *t,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx,
		Esvg_Path_Command *src,
		Esvg_Path_Command *dst,
		Etch_Animation **a,
		Eina_List *cmds,
		int64_t time,
		Eina_Bool final)
{
	_esvg_animate_path_command_6doubles_animation_create(t, actx, abctx, src, dst, a, cmds, time, final);
	_esvg_animate_path_command_cubic_to_keyframes_add(abctx, src, dst, a, time);
}

static Etch_Animation ** _esvg_animate_path_command_animation_alloc(Esvg_Path_Command_Type type)
{
	int count[ESVG_PATH_COMMAND_TYPES] = {
		2, /* ESVG_PATH_MOVE_TO */
		2, /* ESVG_PATH_LINE_TO */
		1, /* ESVG_PATH_HLINE_TO */
		1, /* ESVG_PATH_VLINE_TO */
		6, /* ESVG_PATH_CUBIC_TO */
		4, /* ESVG_PATH_SCUBIC_TO */
		4, /* ESVG_PATH_QUADRATIC_TO */
		2, /* ESVG_PATH_SQUADRATIC_TO */
		7, /* ESVG_PATH_ARC_TO */
		0, /* ESVG_PATH_CLOSE */
	};
	return calloc(count[type], sizeof(Etch_Animation *));
}

static Eina_Bool _esvg_animate_path_command_animation_generate(Edom_Tag *t,
		Eina_List *values,
		Eina_List *times,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx)
{
	Esvg_Animate *thiz;
	Esvg_Path_Command *cmd;
	Esvg_Path_Command *last;
	Eina_List *cmds;
	Eina_List *l;
	int64_t *time;

	thiz = _esvg_animate_get(t);

	/* first create the animations for every command value */
	time = eina_list_data_get(times);
	times = eina_list_next(times);

	cmds = eina_list_data_get(values);
	values = eina_list_next(values);

	EINA_LIST_REVERSE_FOREACH (cmds, l, cmd)
	{
		if (cmd->type == ESVG_PATH_CLOSE)
			continue;
		last = cmd;
		break;
	}

	EINA_LIST_FOREACH (cmds, l, cmd)
	{
		Esvg_Path_Command *ncmd;
		Etch_Animation **a;
		Etch_Data data;
		Eina_Bool final = EINA_FALSE;

		/* add a new destination command */
		ncmd = calloc(1, sizeof(Esvg_Path_Command));
		*ncmd = *cmd;
		thiz->cmds = eina_list_append(thiz->cmds, ncmd);

		/* the final cmd should set the property */
		if (last == cmd)
			final = EINA_TRUE;

		/* no need for animations on the close command */
		if (cmd->type == ESVG_PATH_CLOSE)
			continue;

		/* create the animations holder */
		a = _esvg_animate_path_command_animation_alloc(cmd->type);
		thiz->animations = eina_list_append(thiz->animations, a);

		switch (cmd->type)
		{
			case ESVG_PATH_MOVE_TO:
			_esvg_animate_path_command_move_to_animation_create(t,
				actx, abctx, cmd, ncmd, a, thiz->cmds, *time, final);
	
			break;

			case ESVG_PATH_LINE_TO:
			_esvg_animate_path_command_line_to_animation_create(t,
				actx, abctx, cmd, ncmd, a, thiz->cmds, *time, final);
			break;

			case ESVG_PATH_HLINE_TO:
			_esvg_animate_path_command_hline_to_animation_create(t,
				actx, abctx, cmd, ncmd, a, thiz->cmds, *time, final);
			break;

			case ESVG_PATH_VLINE_TO:
			_esvg_animate_path_command_vline_to_animation_create(t,
				actx, abctx, cmd, ncmd, a, thiz->cmds, *time, final);
			break;

			case ESVG_PATH_CUBIC_TO:
			_esvg_animate_path_command_cubic_to_animation_create(t,
				actx, abctx, cmd, ncmd, a, thiz->cmds, *time, final);
			break;

			case ESVG_PATH_SCUBIC_TO:
			_esvg_animate_path_command_scubic_to_animation_create(t,
				actx, abctx, cmd, ncmd, a, thiz->cmds, *time, final);
			break;

			case ESVG_PATH_QUADRATIC_TO:
			_esvg_animate_path_command_quadratic_to_animation_create(t,
				actx, abctx, cmd, ncmd, a, thiz->cmds, *time, final);
			break;

			case ESVG_PATH_SQUADRATIC_TO:
			_esvg_animate_path_command_squadratic_to_animation_create(t,
				actx, abctx, cmd, ncmd, a, thiz->cmds, *time, final);
			break;

			case ESVG_PATH_ARC_TO:
			printf("TODO\n");
			default:
			break;
		}
	}

	/* once the animations are created we need to iterate over the next
	 * list of commands, iterate over the values and fetch the needed
	 * animation from the animate_base abstract */
	EINA_LIST_FOREACH (values, l, cmds)
	{
		Eina_List *l2;
		Eina_List *l3;
		Eina_List *l4;

		l2 = thiz->cmds;
		l3 = thiz->animations;
		time = eina_list_data_get(times);
		printf("time = %lld\n", *time);
		EINA_LIST_FOREACH (cmds, l4, cmd)
		{
			Esvg_Path_Command *ncmd;
			Etch_Animation **a;

			ncmd = eina_list_data_get(l2);
			a = eina_list_data_get(l3);
			/* the value command is not the same as the first value command */
			if (ncmd->type != cmd->type)
			{
				printf("wrong!\n");
			}
			if (ncmd->type == ESVG_PATH_CLOSE)
				goto done;

			switch (cmd->type)
			{
				case ESVG_PATH_MOVE_TO:
				_esvg_animate_path_command_move_to_keyframes_add(abctx, cmd, ncmd, a, *time);
				break;

				case ESVG_PATH_LINE_TO:
				_esvg_animate_path_command_line_to_keyframes_add(abctx, cmd, ncmd, a, *time);
				break;

				case ESVG_PATH_HLINE_TO:
				_esvg_animate_path_command_hline_to_keyframes_add(abctx, cmd, ncmd, a, *time);
				break;

				case ESVG_PATH_VLINE_TO:
				_esvg_animate_path_command_vline_to_keyframes_add(abctx, cmd, ncmd, a, *time);
				break;

				case ESVG_PATH_QUADRATIC_TO:
				_esvg_animate_path_command_quadratic_to_keyframes_add(abctx, cmd, ncmd, a, *time);
				break;

				case ESVG_PATH_SQUADRATIC_TO:
				_esvg_animate_path_command_squadratic_to_keyframes_add(abctx, cmd, ncmd, a, *time);
				break;

				case ESVG_PATH_SCUBIC_TO:
				_esvg_animate_path_command_scubic_to_keyframes_add(abctx, cmd, ncmd, a, *time);
				break;

				case ESVG_PATH_CUBIC_TO:
				_esvg_animate_path_command_cubic_to_keyframes_add(abctx, cmd, ncmd, a, *time);
				break;

				case ESVG_PATH_ARC_TO:
				default:
				printf("TODO\n");
				break;
			}
done:
			l2 = eina_list_next(l2);
			l3 = eina_list_next(l3);
		}
		/* just add the needed keyframes for every command, using the current time
		 * on each value, pick up the next time */
		times = eina_list_next(times);
	}
	printf("animation generated!!!\n");

	return EINA_TRUE;
}

static Esvg_Animate_Base_Type_Descriptor _path_command_descriptor = {
	/* .value_get 		= */ _esvg_animate_path_command_get,
	/* .value_free 		= */ _esvg_animate_path_command_free,
	/* .animation_generate 	= */ _esvg_animate_path_command_animation_generate,
};
/*----------------------------------------------------------------------------*
 *                         The Esvg Element interface                         *
 *----------------------------------------------------------------------------*/
static void _esvg_animate_free(Edom_Tag *t)
{
	Esvg_Animate *thiz;

	thiz = _esvg_animate_get(t);
	free(thiz);
}

static Eina_Bool _esvg_animate_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	return EINA_TRUE;
}

static Eina_Bool _esvg_animate_attribute_get(Edom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static Eina_Bool _esvg_animate_type_descriptor_get(Edom_Tag *t,
		const char *name, Esvg_Animate_Base_Type_Descriptor **d)
{
	/* based on the property name get the correct descriptor */
	if (!strcmp(name, "esvg_animated_length") || !strcmp(name, "esvg_animated_coord"))
	{
		*d = &_length_descriptor;
	}
	else if (!strcmp(name, "esvg_animated_number"))
	{
		*d = &_number_descriptor;
	}
	else if (!strcmp(name, "esvg_animated_string"))
	{
		*d = &_string_descriptor;
	}
	else if (!strcmp(name, "esvg_animated_path_command"))
	{
		*d = &_path_command_descriptor;
	}
	else
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

static Esvg_Animate_Base_Descriptor _descriptor = {
	/* .attribute_get 	= */ _esvg_animate_attribute_get,
	/* .free 		= */ _esvg_animate_free,
	/* .initialize 		= */ NULL,
	/* .attribute_set 	= */ _esvg_animate_attribute_set,
	/* .type_descriptor_get	= */ _esvg_animate_type_descriptor_get,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_animate_new(void)
{
	Esvg_Animate *thiz;
	Edom_Tag *t;

	thiz = calloc(1, sizeof(Esvg_Animate));

	t = esvg_animate_base_new(&_descriptor, ESVG_ANIMATE, thiz);
	return t;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* The ender wrapper */
#define _esvg_animate_delete NULL
#include "generated/esvg_generated_animate.c"
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Element * esvg_animate_new(void)
{
	return ESVG_ELEMENT_NEW("animate");
}

