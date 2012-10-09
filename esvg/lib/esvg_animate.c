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
/*----------------------------------------------------------------------------*
 *                        The Esvg_Paint type descriptor                      *
 *----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*
 *                       The Esvg_Length type descriptor                      *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_animate_length_get(const char *attr, void **value)
{
	Esvg_Length *v;

	v = calloc(1, sizeof(Esvg_Length));
	esvg_length_string_from(v, attr);
	*value = v;
	return EINA_TRUE;
}

static void * _esvg_animate_length_destination_new(void)
{
	Esvg_Animated_Length *v;
	v = calloc(1, sizeof(Esvg_Animated_Length));
	return v;
}

static void _esvg_animate_length_interpolate(void *a,
		void *b, double m, void *add, void *res)
{
	Esvg_Length *va = a;
	Esvg_Length *vb = b;
	Esvg_Animated_Length *vadd = add;
	Esvg_Animated_Length *r = res;

	r->base.unit = va->unit;
	etch_interpolate_double(va->value, vb->value, m, &r->base.value);
	if (vadd)
		r->base.value += vadd->anim.value;
}

static Esvg_Animate_Base_Type_Descriptor _length_descriptor = {
	/* .value_get 		= */ _esvg_animate_length_get,
	/* .value_free 		= */ free,
	/* .destination_new 	= */ _esvg_animate_length_destination_new,
	/* .destination_get 	= */ NULL,
	/* .destination_free 	= */ free,
	/* .interpolate 	= */ _esvg_animate_length_interpolate,
};
/*----------------------------------------------------------------------------*
 *                        The number type descriptor                          *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_animate_number_get(const char *attr, void **value)
{
	double *v;

	v = calloc(1, sizeof(double));
	*v = esvg_number_string_from(attr, 1.0);
	*value = v;
	return EINA_TRUE;
}

static void * _esvg_animate_number_destination_new(void)
{
	Esvg_Animated_Number *v;
	v = calloc(1, sizeof(Esvg_Animated_Number));
	return v;
}

static void _esvg_animate_number_interpolate(void *a,
		void *b, double m, void *add, void *res)
{
	Esvg_Number *va = a;
	Esvg_Number *vb = b;
	Esvg_Animated_Number *vadd = add;
	Esvg_Animated_Number *r = res;

	etch_interpolate_double(*va, *vb, m, &r->base);
	if (vadd)
		r->base += vadd->anim;
}

static Esvg_Animate_Base_Type_Descriptor _number_descriptor = {
	/* .value_get 		= */ _esvg_animate_number_get,
	/* .value_free 		= */ free,
	/* .destination_new 	= */ _esvg_animate_number_destination_new,
	/* .destination_get 	= */ NULL,
	/* .destination_free 	= */ free,
	/* .interpolate 	= */ _esvg_animate_number_interpolate,
};
/*----------------------------------------------------------------------------*
 *                      The string type descriptor                            *
 *----------------------------------------------------------------------------*/
static void * _esvg_animate_string_destination_new(void)
{
	Esvg_Animated_String *v;
	v = calloc(1, sizeof(Esvg_Animated_String));
	return v;
}

static void _esvg_animate_string_interpolate(void *a,
		void *b, double m, void *add, void *res)
{
	char *va = a;
	char *vb = b;
	Esvg_Animated_String *r = res;

	/* TODO
	etch_interpolate_string(va, vb, m, &r->base);
	*/
}

static Eina_Bool _esvg_animate_string_get(const char *attr, void **value)
{
	char *v;

	v = strdup(attr);
	*value = v;
	return EINA_TRUE;
}

static Esvg_Animate_Base_Type_Descriptor _string_descriptor = {
	/* .value_get 		= */ _esvg_animate_string_get,
	/* .value_free 		= */ free,
	/* .destination_new 	= */ _esvg_animate_string_destination_new,
	/* .destination_get 	= */ NULL,
	/* .destination_free 	= */ free,
	/* .interpolate 	= */ _esvg_animate_string_interpolate,
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

static void * _esvg_animate_path_command_destination_new(void)
{
	Esvg_Animated_List *v;

	v = calloc(1, sizeof(Esvg_Animated_List));
	return v;
}

static void _esvg_animate_path_command_destination_get(void *d, Eina_List *values)
{
	Esvg_Animated_List *v = d;
	Esvg_Path_Command *cmd;
	Eina_List *value;
	Eina_List *l;

	value = values->data;
	EINA_LIST_FOREACH (value, l, cmd)
	{
		Esvg_Path_Command *ncmd;

		ncmd = calloc(1, sizeof(Esvg_Path_Command));
		*ncmd = *cmd;
		v->base = eina_list_append(v->base, ncmd);
	}
}

static void _esvg_animate_path_command_destination_free(void *data)
{
	Esvg_Animated_List *v = data;
	Esvg_Path_Command *cmd;

	EINA_LIST_FREE (v->base, cmd)
		free(cmd);
	free(v);
}

static void _esvg_animate_path_command_interpolate(void *a,
		void *b, double m, void *add, void *res)
{
	Esvg_Animated_List *r = res;
	Esvg_Path_Command *ca;
	Eina_List *va = a;
	Eina_List *vb = b;
	Eina_List *l1, *l2, *l3;
	
	l2 = vb;
	l3 = r->base;
	EINA_LIST_FOREACH (va, l1, ca)
	{
		Esvg_Path_Command *cb = l2->data;
		Esvg_Path_Command *cr = l3->data;

		switch (ca->type)
		{
			case ESVG_PATH_MOVE_TO:
			etch_interpolate_double(ca->data.move_to.x, cb->data.move_to.x, m, &cr->data.move_to.x);
			etch_interpolate_double(ca->data.move_to.y, cb->data.move_to.y, m, &cr->data.move_to.y);
			break;
			case ESVG_PATH_LINE_TO:
			etch_interpolate_double(ca->data.line_to.x, cb->data.line_to.x, m, &cr->data.line_to.x);
			etch_interpolate_double(ca->data.line_to.y, cb->data.line_to.y, m, &cr->data.line_to.y);
			break;
			case ESVG_PATH_HLINE_TO:
			etch_interpolate_double(ca->data.hline_to.c, cb->data.hline_to.c, m, &cr->data.hline_to.c);
			break;
			case ESVG_PATH_VLINE_TO:
			etch_interpolate_double(ca->data.vline_to.c, cb->data.vline_to.c, m, &cr->data.vline_to.c);
			break;
			case ESVG_PATH_CUBIC_TO:
			etch_interpolate_double(ca->data.cubic_to.ctrl_x1, cb->data.cubic_to.ctrl_x1, m, &cr->data.cubic_to.ctrl_x1);
			etch_interpolate_double(ca->data.cubic_to.ctrl_y1, cb->data.cubic_to.ctrl_y1, m, &cr->data.cubic_to.ctrl_y1);
			etch_interpolate_double(ca->data.cubic_to.ctrl_x0, cb->data.cubic_to.ctrl_x0, m, &cr->data.cubic_to.ctrl_x0);
			etch_interpolate_double(ca->data.cubic_to.ctrl_y0, cb->data.cubic_to.ctrl_y0, m, &cr->data.cubic_to.ctrl_y0);
			etch_interpolate_double(ca->data.cubic_to.x, cb->data.cubic_to.x, m, &cr->data.cubic_to.x);
			etch_interpolate_double(ca->data.cubic_to.y, cb->data.cubic_to.y, m, &cr->data.cubic_to.y);
			break;
			case ESVG_PATH_SCUBIC_TO:
			etch_interpolate_double(ca->data.scubic_to.ctrl_x, cb->data.scubic_to.ctrl_x, m, &cr->data.scubic_to.ctrl_x);
			etch_interpolate_double(ca->data.scubic_to.ctrl_y, cb->data.scubic_to.ctrl_y, m, &cr->data.scubic_to.ctrl_y);
			etch_interpolate_double(ca->data.scubic_to.x, cb->data.scubic_to.x, m, &cr->data.scubic_to.x);
			etch_interpolate_double(ca->data.scubic_to.y, cb->data.scubic_to.y, m, &cr->data.scubic_to.y);
			break;
			case ESVG_PATH_QUADRATIC_TO:
			etch_interpolate_double(ca->data.quadratic_to.ctrl_x, cb->data.quadratic_to.ctrl_x, m, &cr->data.quadratic_to.ctrl_x);
			etch_interpolate_double(ca->data.quadratic_to.ctrl_y, cb->data.quadratic_to.ctrl_y, m, &cr->data.quadratic_to.ctrl_y);
			etch_interpolate_double(ca->data.quadratic_to.x, cb->data.quadratic_to.x, m, &cr->data.quadratic_to.x);
			etch_interpolate_double(ca->data.quadratic_to.y, cb->data.quadratic_to.y, m, &cr->data.quadratic_to.y);
			break;
			case ESVG_PATH_SQUADRATIC_TO:
			etch_interpolate_double(ca->data.squadratic_to.x, cb->data.squadratic_to.x, m, &cr->data.squadratic_to.x);
			etch_interpolate_double(ca->data.squadratic_to.y, cb->data.squadratic_to.y, m, &cr->data.squadratic_to.y);
			break;
			case ESVG_PATH_ARC_TO:
			etch_interpolate_double(ca->data.arc_to.x, cb->data.arc_to.x, m, &cr->data.arc_to.x);
			etch_interpolate_double(ca->data.arc_to.y, cb->data.arc_to.y, m, &cr->data.arc_to.y);
			break;
			case ESVG_PATH_CLOSE:
			break;
		}
		l2 = l2->next;
		l3 = l3->next;
	}
}

static Esvg_Animate_Base_Type_Descriptor _path_command_descriptor = {
	/* .value_get 		= */ _esvg_animate_path_command_get,
	/* .value_free 		= */ _esvg_animate_path_command_free,
	/* .destination_new 	= */ _esvg_animate_path_command_destination_new,
	/* .destination_get 	= */ _esvg_animate_path_command_destination_get,
	/* .destination_free 	= */ _esvg_animate_path_command_destination_free,
	/* .interpolate 	= */ _esvg_animate_path_command_interpolate,
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

