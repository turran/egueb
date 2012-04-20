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

#include "esvg_svg.h"
#include "esvg_element.h"
#include "esvg_renderable.h"

/*
 * Given that a svg element can clip, we should use a clipper with a compound
 * inside as the renderer
 * TODO
 * Handle the following attributes
 * onunload
 * onabort
 * onerror
 * onresize
 * onscroll
 * onzoom
 *
 * void pauseAnimations();
 * void unpauseAnimations();
 * boolean animationsPaused();
 * float getCurrentTime();
 * void setCurrentTime(in float seconds);
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Ender_Property *ESVG_SVG_X;
static Ender_Property *ESVG_SVG_Y;
static Ender_Property *ESVG_SVG_WIDTH;
static Ender_Property *ESVG_SVG_HEIGHT;
static Ender_Property *ESVG_SVG_ACTUAL_WIDTH;
static Ender_Property *ESVG_SVG_ACTUAL_HEIGHT;
static Ender_Property *ESVG_SVG_VIEWBOX;

typedef struct _Esvg_Svg
{
	/* properties */
	double version;
	Esvg_View_Box view_box;
	Eina_Bool view_box_set;
	Esvg_Coord x;
	Esvg_Coord y;
	Esvg_Length width;
	Esvg_Length height;
	/* private */
	Eina_List *styles; /* the list of styles found on this svg scope */
	Eina_List *svgs; /* the list of svg documents found on the svg */
	Enesim_Renderer *clipper;
	Enesim_Renderer *compound;
	Eina_Hash *ids; /* the ids found */
} Esvg_Svg;

static Eina_Bool _esvg_svg_child_initialize(Edom_Tag *t, Edom_Tag *child_t, void *data);

static Esvg_Svg * _esvg_svg_get(Edom_Tag *t)
{
	Esvg_Svg *thiz;

	if (esvg_element_type_get_internal(t) != ESVG_SVG)
		return NULL;
	thiz = esvg_instantiable_data_get(t);

	return thiz;
}

/* FIXME the ender events just trigger once the id has changed so we dont know the old one */
static void _esvg_svg_child_id_changed(Ender_Element *e, const char *event_name, void *event_data, void *data)
{

}


static void _esvg_svg_child_child_cb(Ender_Element *e, const char *event_name, void *event_data, void *data)
{
	Ender_Event_Mutation_Property *ev = event_data;
	Edom_Tag *tag = data;
	Edom_Tag *child_child;
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(tag);
	switch (ev->type)
	{
		case ENDER_EVENT_MUTATION_ADD:
		child_child = ender_value_object_get(ev->value);
		_esvg_svg_child_initialize(tag, child_child, thiz);
		break;

		/* TODO remove the topmost */
		case ENDER_EVENT_MUTATION_REMOVE:
		printf("TODO\n");
		printf("child removed to one of our childs!\n");
		break;

		default:
		printf("TODO\n");
		break;
	}
}

static Eina_Bool _esvg_svg_child_initialize(Edom_Tag *t, Edom_Tag *child_t, void *data)
{
	Esvg_Svg *thiz;
	Esvg_Svg *child;
	Ender_Element *thiz_e;
	Ender_Element *child_e;
	const char *id;

 	thiz = data;

	/* set the topmost on every element */
	thiz_e = esvg_element_ender_get(t);
	esvg_element_topmost_set(child_t, thiz_e);

	/* add events for the child property */
	child_e = esvg_element_ender_get(child_t);
	ender_event_listener_add(child_e, "Mutation:child", _esvg_svg_child_child_cb, t);

	/* TODO add an event whenever the child changes the id */
	esvg_element_id_get(child_e, &id);
	if (id)
	{
		printf("adding id %s\n", id);
		eina_hash_add(thiz->ids, id, child_e);
	}
	/* iterate over the childs of the child and do the same initialization */
	edom_tag_child_foreach(child_t, _esvg_svg_child_initialize, thiz);
	return EINA_TRUE;
}

static void _esvg_svg_child_deinitialize(Edom_Tag *t, Edom_Tag *child, void *data)
{
	/* remove from the ids */
}
/*----------------------------------------------------------------------------*
 *                       The Esvg Renderable interface                        *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_svg_attribute_set(Ender_Element *e, const char *key, const char *value)
{
	if (strcmp(key, "version") == 0)
	{
		double version = esvg_number_string_from(value, 0.0);
		esvg_svg_version_set(e, version);
	}
	else if (strcmp(key, "x") == 0)
	{
		Esvg_Coord x;

		esvg_length_string_from(&x, value, ESVG_COORD_0);
		esvg_svg_x_set(e, &x);
	}
	else if (strcmp(key, "y") == 0)
	{
		Esvg_Coord y;

		esvg_length_string_from(&y, value, ESVG_COORD_0);
		esvg_svg_y_set(e, &y);
	}
	else if (strcmp(key, "width") == 0)
	{
		Esvg_Length width;

		esvg_length_string_from(&width, value, ESVG_LENGTH_0);
		esvg_svg_width_set(e, &width);
	}
	else if (strcmp(key, "height") == 0)
	{
		Esvg_Length height;

		esvg_length_string_from(&height, value, ESVG_LENGTH_0);
		esvg_svg_height_set(e, &height);
	}
	else if (strcmp(key, "viewBox") == 0)
	{
		Esvg_View_Box vb = esvg_view_box_get(value);
		esvg_svg_viewbox_set(e, &vb);
	}

	return EINA_TRUE;
}

static Eina_Bool _esvg_svg_attribute_get(Edom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static Eina_Bool _esvg_svg_child_add(Edom_Tag *tag, Edom_Tag *child)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(tag);
	if (!esvg_is_element_internal(child))
		return;

	/* if renderable, add the renderer into the compound */
	if (esvg_is_instantiable_internal(child))
	{
		Enesim_Renderer *r = NULL;

		esvg_renderable_internal_renderer_get(child, &r);
		enesim_renderer_compound_layer_add(thiz->compound, r);
	}
	_esvg_svg_child_initialize(tag, child, thiz);

	return EINA_TRUE;
}

static Eina_Bool _esvg_svg_child_remove(Edom_Tag *t, Edom_Tag *child)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);

	if (esvg_is_instantiable_internal(child))
	{
		Enesim_Renderer *r = NULL;

		esvg_renderable_internal_renderer_get(child, &r);
		enesim_renderer_compound_layer_remove(thiz->compound, r);
	}
	return EINA_TRUE;
}

static Eina_Bool _esvg_svg_setup(Edom_Tag *t,
		Esvg_Element_Context *ctx,
		Esvg_Renderable_Context *rctx,
		Enesim_Error **error)
{
	Esvg_Svg *thiz;
	Eina_List *l;
	Enesim_Renderer *child;
	Enesim_Renderer *parent;
	double width, height;

	thiz = _esvg_svg_get(t);
	width = esvg_length_final_get(&thiz->width, ctx->viewbox.width);
	height = esvg_length_final_get(&thiz->height, ctx->viewbox.height);
	enesim_renderer_clipper_width_set(thiz->clipper, width);
	enesim_renderer_clipper_height_set(thiz->clipper, height);

	/* the viewbox will set a new user space coordinate */
	/* FIXME check zeros */
	if (thiz->view_box_set)
	{
		Enesim_Matrix scale;
		double new_vw;
		double new_vh;

		new_vw = thiz->view_box.width / width;
		new_vh = thiz->view_box.height / height;

		printf("setting scale %p to %g %g - %g %g (%g %g)\n",
				ctx,
				thiz->view_box.width,
				thiz->view_box.height,
				width,
				height,
				new_vw, new_vh);
		width = thiz->view_box.width;
		height = thiz->view_box.height;

		enesim_matrix_scale(&scale, 1.0/new_vw, 1.0/new_vh);
		enesim_matrix_compose(&scale, &ctx->transform, &ctx->transform);
		/* TODO handle current matrix */
	}
	ctx->viewbox.width = width;
	ctx->viewbox.height = height;

	return EINA_TRUE;
}

static Enesim_Renderer * _esvg_svg_renderer_get(Edom_Tag *t)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	return thiz->clipper;
}

static void _esvg_svg_clone(Edom_Tag *t, Edom_Tag *dt)
{

}

static void _esvg_svg_free(Edom_Tag *t)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	free(thiz);
}

static Esvg_Instantiable_Descriptor _descriptor = {
	/* .child_add		= */ _esvg_svg_child_add,
	/* .child_remove	= */ _esvg_svg_child_remove,
	/* .attribute_get 	= */ _esvg_svg_attribute_get,
	/* .cdata_set 		= */ NULL,
	/* .text_set 		= */ NULL,
	/* .free 		= */ _esvg_svg_free,
	/* .initialize 		= */ NULL,
	/* .attribute_set 	= */ _esvg_svg_attribute_set,
	/* .clone		= */ _esvg_svg_clone,
	/* .setup		= */ _esvg_svg_setup,
	/* .renderer_get	= */ _esvg_svg_renderer_get,
};

/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_svg_new(void)
{
	Esvg_Svg *thiz;
	Edom_Tag *t;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Svg));
	if (!thiz) return NULL;

	r = enesim_renderer_compound_new();
	enesim_renderer_rop_set(r, ENESIM_BLEND);
	thiz->compound = r;

	r = enesim_renderer_clipper_new();
	enesim_renderer_clipper_content_set(r, thiz->compound);
	thiz->clipper = r;

	thiz->ids = eina_hash_string_superfast_new(NULL);

	/* Default values */
	thiz->version = 1.0;
	thiz->x = ESVG_COORD_0;
	thiz->y = ESVG_COORD_0;
	thiz->width = ESVG_LENGTH_100_PERCENT;
	thiz->height = ESVG_LENGTH_100_PERCENT;

	/* no default value for the view_box */

	t = esvg_instantiable_new(&_descriptor, ESVG_SVG, thiz);
	return t;
}

Edom_Tag * _esvg_svg_element_find(Edom_Tag *tag, const char *id)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(tag);
	return eina_hash_find(thiz->ids, id);
}

static void _esvg_svg_version_set(Edom_Tag *t, double version)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	thiz->version = version;
}

static void _esvg_svg_version_get(Edom_Tag *t, double *version)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	if (version) *version = thiz->version;
}

static void _esvg_svg_x_set(Edom_Tag *t, Esvg_Coord *x)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	thiz->x = *x;
}

static void _esvg_svg_x_get(Edom_Tag *t, Esvg_Coord *x)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	if (x) *x = thiz->x;
}


static void _esvg_svg_y_set(Edom_Tag *t, Esvg_Coord *y)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	thiz->y = *y;
}

static void _esvg_svg_y_get(Edom_Tag *t, Esvg_Coord *y)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	if (y) *y = thiz->y;
}

static void _esvg_svg_width_set(Edom_Tag *t, Esvg_Length *width)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	thiz->width = *width;
}

static void _esvg_svg_width_get(Edom_Tag *t, Esvg_Length *width)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	if (width) *width = thiz->width;
}

static void _esvg_svg_height_set(Edom_Tag *t, Esvg_Length *height)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	thiz->height = *height;
}

static void _esvg_svg_height_get(Edom_Tag *t, Esvg_Length *height)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	if (height) *height = thiz->height;
}

static void _esvg_svg_viewbox_set(Edom_Tag *t, Esvg_View_Box *vb)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	if (!vb)
	{
		thiz->view_box_set = EINA_FALSE;
	}
	else
	{
		thiz->view_box = *vb;
		thiz->view_box_set = EINA_TRUE;
	}
}

/* FIXME the below two functions should return the actual width/height based
 * on the width and height properties, if it is relative it should use the
 * the container width/height to compute them, this is are helper functions
 * to know the real size of the svg, this way we'll know on the upper libraries
 * the preferred area
 */
static void _esvg_svg_actual_width_get(Edom_Tag *t, double *actual_width)
{
	Esvg_Svg *thiz;
	double aw;
	double cw;

	thiz = _esvg_svg_get(t);
	esvg_renderable_internal_container_width_get(t, &cw);
	aw = esvg_length_final_get(&thiz->width, cw);
	*actual_width = aw;
}

static void _esvg_svg_actual_height_get(Edom_Tag *t, double *actual_height)
{
	Esvg_Svg *thiz;
	double ah;
	double ch;

	thiz = _esvg_svg_get(t);
	esvg_renderable_internal_container_height_get(t, &ch);
	ah = esvg_length_final_get(&thiz->height, ch);
	*actual_height = ah;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ender_Element * esvg_svg_internal_element_find(Edom_Tag *t, const char *id)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	return eina_hash_find(thiz->ids, id);
}

/* The ender wrapper */
#define _esvg_svg_actual_width_set NULL
#define _esvg_svg_actual_height_set NULL
#define _esvg_svg_viewbox_get NULL
#include "generated/esvg_generated_svg.c"

#if 0
void esvg_svg_style_add(Edom_Tag *tag, Esvg_Parser_Style *s)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(tag);
	thiz->styles = eina_list_append(thiz->styles, s);
}

void esvg_svg_style_apply(Edom_Tag *tag)
{
	Esvg_Svg *thiz;
	Esvg_Parser_Style *s;
	Eina_List *l;

	/* FIXME  we are iterating over every style and going through the
	 * tag tree on ecss, we better merge the styles to only parse the tree
	 * once
	 */
	thiz = _esvg_svg_get(tag);
	EINA_LIST_FOREACH(thiz->styles, l, s)
	{
		printf("applying style %p\n", s);
		esvg_style_apply(s, thiz->tag);
	}
}
#endif

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * esvg_svg_new(void)
{
	return ender_element_new_with_namespace("svg", "esvg");
}

EAPI Ender_Element * esvg_svg_element_find(Ender_Element *e, const char *id)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	return esvg_svg_internal_element_find(t, id);
}

EAPI Eina_Bool esvg_is_svg(Ender_Element *e)
{
	if (esvg_element_type_get(e) != ESVG_SVG)
		return EINA_FALSE;
	return EINA_TRUE;
}

EAPI void esvg_svg_version_set(Ender_Element *e, double version)
{
}

EAPI void esvg_svg_version_get(Ender_Element *e, double *version)
{
}

EAPI void esvg_svg_x_set(Ender_Element *e, Esvg_Coord *x)
{
	ender_element_property_value_set(e, ESVG_SVG_X, x, NULL);
}

EAPI void esvg_svg_y_set(Ender_Element *e, Esvg_Coord *y)
{
	ender_element_property_value_set(e, ESVG_SVG_Y, y, NULL);
}

EAPI void esvg_svg_width_set(Ender_Element *e, Esvg_Length *width)
{
	ender_element_property_value_set(e, ESVG_SVG_WIDTH, width, NULL);
}

EAPI void esvg_svg_height_set(Ender_Element *e, Esvg_Length *height)
{
	ender_element_property_value_set(e, ESVG_SVG_HEIGHT, height, NULL);
}

EAPI void esvg_svg_viewbox_set(Ender_Element *e, Esvg_View_Box *vb)
{
	ender_element_property_value_set(e, ESVG_SVG_VIEWBOX, vb, NULL);
}

/* FIXME the below two functions should return the actual width/height based
 * on the width and height properties, if it is relative it should use the
 * the container width/height to compute them, this is are helper functions
 * to know the real size of the svg, this way we'll know on the upper libraries
 * the preferred area
 */
EAPI void esvg_svg_actual_width_get(Ender_Element *e, double *actual_width)
{
	Edom_Tag *t;
	Esvg_Svg *thiz;

	t = ender_element_object_get(e);
	_esvg_svg_actual_width_get(t, actual_width);
}

EAPI void esvg_svg_actual_height_get(Ender_Element *e, double *actual_height)
{
	Edom_Tag *t;
	Esvg_Svg *thiz;

	t = ender_element_object_get(e);
	_esvg_svg_actual_height_get(t, actual_height);
}
