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
#include "esvg_private_instantiable.h"
#include "esvg_private_svg.h"
#include "esvg_private_a.h"

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
	/* keep track if the renderable tree has changed, includeing the <a> tag */
	Eina_Bool renderable_tree_changed : 1;
	Eina_List *styles; /* the list of styles found on this svg scope */
	Eina_Bool styles_changed : 1;
	Eina_List *svgs; /* the list of svg documents found on the svg */
	Eina_List *elements_changed;
	Enesim_Renderer *clipper;
	Enesim_Renderer *compound;
	/* FIXME we use a background because of the blend/fill thing on the redraws */
	Enesim_Renderer *background;
	Eina_Hash *ids; /* the ids found */
	/* animation */
	Etch *etch;
	Eina_Bool paused;
} Esvg_Svg;

static Eina_Bool _esvg_svg_child_initialize(Edom_Tag *t, Edom_Tag *child_t, void *data);
static Eina_Bool _esvg_svg_child_deinitialize(Edom_Tag *t, Edom_Tag *child_t, void *data);

static Esvg_Svg * _esvg_svg_get(Edom_Tag *t)
{
	Esvg_Svg *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_SVG)
		return NULL;
	thiz = esvg_instantiable_data_get(t);

	return thiz;
}

static Eina_Bool _esvg_svg_setup_interceptor(Edom_Tag *t,
		Edom_Tag *child,
		Esvg_Context *c,
		Enesim_Error **error,
		void *data)
{
	Esvg_Type type;
	Esvg_Svg *thiz = data;

	type = esvg_element_internal_type_get(child);
	if (esvg_type_is_renderable(type) && thiz->renderable_tree_changed)
	{
		/* if renderable, add the renderer into the compound */
		Enesim_Renderer *r = NULL;

		esvg_renderable_internal_renderer_get(child, &r);
		enesim_renderer_compound_layer_add(thiz->compound, r);
	}
 	else if (type == ESVG_A)
	{
		return esvg_element_internal_child_setup(child, c, error, _esvg_svg_setup_interceptor, thiz);
	}
	return EINA_TRUE;
}

static inline void _esvg_svg_size_apply(Esvg_Svg *thiz, Esvg_Element_Context *ctx)
{
	double width;
	double height;

	width = esvg_length_final_get(&thiz->width, ctx->viewbox.width);
	height = esvg_length_final_get(&thiz->height, ctx->viewbox.height);
	enesim_renderer_clipper_width_set(thiz->clipper, width);
	enesim_renderer_clipper_height_set(thiz->clipper, height);
	ctx->viewbox.width = width;
	ctx->viewbox.height = height;
}

static inline void _esvg_svg_viewbox_apply(Esvg_Svg *thiz, Esvg_Element_Context *ctx)
{
	Enesim_Matrix scale;
	double width;
	double height;
	double new_vw;
	double new_vh;

	/* the viewbox will set a new user space coordinate */
	/* FIXME check zeros */
	if (!thiz->view_box_set)
		return;

	new_vw = thiz->view_box.width / ctx->viewbox.width;
	new_vh = thiz->view_box.height / ctx->viewbox.height;

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
	enesim_matrix_compose(&scale, &ctx->transform.base, &ctx->transform.base);
	ctx->viewbox.width = width;
	ctx->viewbox.height = height;
}

/* call the setup on every element of the list of changed elements */
/* FIXME fix the return value */
static void _esvg_svg_element_changed_setup(Esvg_Svg *thiz,
		Esvg_Context *c,
		Enesim_Error **error)
{
	Ender_Element *e;
	Eina_List *l, *l_next;

	EINA_LIST_FOREACH_SAFE(thiz->elements_changed, l, l_next, e)
	{
		Edom_Tag *changed_t;

		changed_t = ender_element_object_get(e);
		if (!esvg_element_has_setup(changed_t, c))
		{
			esvg_element_internal_setup(changed_t, c, error);
		}
		thiz->elements_changed = eina_list_remove_list(thiz->elements_changed, l);
	}
}

static void _esvg_svg_element_changed_add(Esvg_Svg *thiz, Ender_Element *e)
{
#if 0
	Edom_Tag *t;

	t = ender_element_object_get(e);
	printf("adding element %s to the list of changes\n", esvg_type_string_to(esvg_element_internal_type_get(t)));
#endif

	thiz->elements_changed = eina_list_append(thiz->elements_changed, e);
}

static void _esvg_svg_element_changed_remove(Esvg_Svg *thiz, Ender_Element *e)
{

}
/*----------------------------------------------------------------------------*
 *                             The URI interface                              *
 *----------------------------------------------------------------------------*/
static void * _esvg_svg_uri_local_get(const char *name, void *data)
{
	Ender_Element *topmost = data;
	Ender_Element *relative;

	relative = esvg_svg_element_find(topmost, name);
	return relative;
}

static Esvg_Uri_Descriptor _uri_descriptor = {
	/* .local_get 		= */ _esvg_svg_uri_local_get,
	/* .absolute_get 	= */ NULL
};


/* FIXME the ender events just trigger once the id has changed so we dont know the old one */
static void _esvg_svg_child_id_cb(Ender_Element *e, const char *event_name, void *event_data, void *data)
{
	Esvg_Svg *thiz = data;
	Ender_Event_Mutation_Property *ev = event_data;
	const char *id;

	id = ender_value_string_get(ev->value);
	eina_hash_del_by_data(thiz->ids, e);
	if (id) eina_hash_add(thiz->ids, id, e);
}

static void _esvg_svg_child_mutation_child_cb(Ender_Element *e, const char *event_name, void *event_data, void *data)
{
	Ender_Event_Mutation_Property *ev = event_data;
	Ender_Element *child_child_e;
	Edom_Tag *tag = data;
	Edom_Tag *child_child;
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(tag);
	child_child = ender_value_object_get(ev->value);
	child_child_e = esvg_element_ender_get(child_child);
	switch (ev->type)
	{
		/* some child has been added to the whole svg tree */
		case ENDER_EVENT_MUTATION_ADD:
		_esvg_svg_child_initialize(tag, child_child, thiz);
		_esvg_svg_element_changed_add(thiz, child_child_e);
		break;

		/* some child has been removed from the whole svg tree */
		case ENDER_EVENT_MUTATION_REMOVE:
		_esvg_svg_child_deinitialize(tag, child_child, thiz);
		_esvg_svg_element_changed_remove(thiz, child_child_e);
		/* TODO in case the child removed is found, it will get removed */
		break;

		default:
		printf("TODO\n");
		break;
	}
}

static void _esvg_svg_child_mutation_cb(Ender_Element *e, const char *event_name, void *event_data, void *data)
{
	Esvg_Svg *thiz = data;

	/* add the element to the list of changed elements */
	/* TODO if the property is the child, dont do nothing */
	_esvg_svg_element_changed_add(thiz, e);
}

static Eina_Bool _esvg_svg_child_initialize(Edom_Tag *t, Edom_Tag *child_t, void *data)
{
	Esvg_Svg *thiz;
	Ender_Element *thiz_e;
	Ender_Element *child_e;
	const char *id;

 	thiz = data;

	/* set the topmost on every element */
	thiz_e = esvg_element_ender_get(t);
	esvg_element_topmost_set(child_t, thiz_e);

	//printf("initializing %s\n", esvg_type_string_to(esvg_element_internal_type_get(child_t)));
	/* add a callback whenever the child property has changed
	 * to initialize that child too */
	child_e = esvg_element_ender_get(child_t);
	ender_event_listener_add(child_e, "Mutation:child", _esvg_svg_child_mutation_child_cb, t);
	/* add a callback whenever any property has changed to add it the list
	 * of changed elements */
	ender_event_listener_add(child_e, "Mutation", _esvg_svg_child_mutation_cb, thiz);
	/* add an event whenever the child changes the id */
	ender_event_listener_add(child_e, "Mutation:id", _esvg_svg_child_id_cb, thiz);
	/* add the id to the hash of ids */
	esvg_element_id_get(child_e, &id);
	if (id) eina_hash_add(thiz->ids, id, child_e);

	/* add the style to the list of styles */
	if (esvg_is_style_internal(child_t))
	{
		thiz->styles = eina_list_append(thiz->styles, child_t);
		thiz->styles_changed = EINA_TRUE;
	}
	/* iterate over the childs of the child and do the same initialization */
	edom_tag_child_foreach(child_t, _esvg_svg_child_initialize, thiz);

	return EINA_TRUE;
}

static Eina_Bool _esvg_svg_child_deinitialize(Edom_Tag *t, Edom_Tag *child_t, void *data)
{
	Esvg_Svg *thiz;
	Ender_Element *thiz_e;
	Ender_Element *child_e;
	const char *id;

 	thiz = data;

	/* set the topmost on every element */
	thiz_e = esvg_element_ender_get(t);
	esvg_element_topmost_set(child_t, NULL);

	/* add a callback whenever the child property has changed
	 * to initialize that child too */
	child_e = esvg_element_ender_get(child_t);
	ender_event_listener_remove(child_e, "Mutation:child", _esvg_svg_child_mutation_child_cb);
	/* add a callback whenever any property has changed to add it the list
	 * of changed elements */
	ender_event_listener_remove(child_e, "Mutation", _esvg_svg_child_mutation_cb);
	/* add an event whenever the child changes the id */
	ender_event_listener_remove(child_e, "Mutation:id", _esvg_svg_child_id_cb);
	/* add the id to the hash of ids */
	esvg_element_id_get(child_e, &id);
	if (id) eina_hash_del(thiz->ids, id, child_e);

	/* add the style to the list of styles */
	if (esvg_is_style_internal(child_t))
	{
		thiz->styles = eina_list_remove(thiz->styles, child_t);
		thiz->styles_changed = EINA_TRUE;
	}
	/* iterate over the childs of the child and do the same initialization */
	edom_tag_child_foreach(child_t, _esvg_svg_child_deinitialize, thiz);

	return EINA_TRUE;
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

		esvg_length_string_from(&x, value);
		esvg_svg_x_set(e, &x);
	}
	else if (strcmp(key, "y") == 0)
	{
		Esvg_Coord y;

		esvg_length_string_from(&y, value);
		esvg_svg_y_set(e, &y);
	}
	else if (strcmp(key, "width") == 0)
	{
		Esvg_Length width;

		esvg_length_string_from(&width, value);
		esvg_svg_width_set(e, &width);
	}
	else if (strcmp(key, "height") == 0)
	{
		Esvg_Length height;

		esvg_length_string_from(&height, value);
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

static Eina_Bool _esvg_svg_child_add(Edom_Tag *t, Edom_Tag *child)
{
	Esvg_Svg *thiz;
	Esvg_Type type;

	thiz = _esvg_svg_get(t);
	if (!esvg_is_element_internal(child))
		return EINA_FALSE;

	type = esvg_element_internal_type_get(child);
	if (esvg_type_is_renderable(type) || type == ESVG_A)
	{
		thiz->renderable_tree_changed = EINA_TRUE;
		enesim_renderer_compound_layer_clear(thiz->compound);
		enesim_renderer_compound_layer_add(thiz->compound, thiz->background);
	}
	_esvg_svg_child_initialize(t, child, thiz);

	return EINA_TRUE;
}

static Eina_Bool _esvg_svg_child_remove(Edom_Tag *t, Edom_Tag *child)
{
	Esvg_Svg *thiz;
	Esvg_Type type;

	thiz = _esvg_svg_get(t);
	if (!esvg_is_element_internal(child))
		return EINA_FALSE;

	type = esvg_element_internal_type_get(child);
	if (esvg_type_is_renderable(type) || type == ESVG_A)
	{
		thiz->renderable_tree_changed = EINA_TRUE;
		enesim_renderer_compound_layer_clear(thiz->compound);
		enesim_renderer_compound_layer_add(thiz->compound, thiz->background);
	}
	_esvg_svg_child_deinitialize(t, child, thiz);

	return EINA_TRUE;
}

static Esvg_Element_Setup_Return _esvg_svg_setup(Edom_Tag *t,
		Esvg_Context *c,
		Esvg_Element_Context *ctx,
		Esvg_Attribute_Presentation *attr,
		Enesim_Error **error)
{
	Esvg_Svg *thiz;
	Edom_Tag *style;
	Enesim_Renderer *child;
	Enesim_Renderer *parent;
	Esvg_Element_Setup_Return ret;
	int changed;
	Eina_Bool full_process = EINA_FALSE;
	double width, height;

	thiz = _esvg_svg_get(t);

	/* check if we have changed or some element has changed */
	changed = esvg_element_changed(t);
	if (!changed && !thiz->elements_changed)
		return EINA_TRUE;

	/* 1. if the attr or the context have changed, then we need to propagate on the whole tree again
	 * that means that if for some reason some renderable direct child or the a tag has been added/removed
	 * (renderable_tree_changed flag) on the setup process we also need to add the renderers
	 * to our compound
	 */
	//if (changed)
	{
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
			enesim_matrix_compose(&scale, &ctx->transform.base, &ctx->transform.base);
			/* TODO handle current matrix */
		}
		ctx->viewbox.width = width;
		ctx->viewbox.height = height;

		/* if the styles have changed apply them */
		if (thiz->styles_changed)
		{
			printf("styles changed!!!\n");
			/* for every style apply it */
			/* TODO what if we have a sub svg with its own styles? */
			EINA_LIST_FREE(thiz->styles, style)
			{
				esvg_style_apply(style, t);
			}
			thiz->styles_changed = EINA_FALSE;
		}
	}
	/* 2. if no attr or context changed, but some property that changes the context have
	 * changed (viewbox) then do a full tree process
	 */

	/* 3. ok no need to process the whole tree. check if we do need to add the renderables back */
	ret = esvg_element_internal_child_setup(t, c, error, _esvg_svg_setup_interceptor, thiz);
	thiz->renderable_tree_changed = EINA_FALSE;

	/* 4. now process the list of changed elements, it will automatically do the setup on its
	 * parent
	 */
	_esvg_svg_element_changed_setup(thiz, c, error);

	return ret;
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
	/* .renderer_propagate	= */ NULL,
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
	enesim_renderer_rop_set(r, ENESIM_FILL);
	thiz->compound = r;

	r = enesim_renderer_background_new();
	enesim_renderer_rop_set(r, ENESIM_FILL);
	enesim_renderer_background_color_set(r, 0x00000000);
	enesim_renderer_compound_layer_add(thiz->compound, r);
	thiz->background = r;

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
	/* the animation system */
	thiz->etch = etch_new();
	etch_timer_fps_set(thiz->etch, 30);

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
Ender_Element * esvg_svg_uri_get(Ender_Element *e, const char *uri)
{
	Ender_Element *found;

	/* just get the renderer here, dont do the setup */
	found = esvg_uri_string_from(uri, &_uri_descriptor, e);
	return found;
}

Ender_Element * esvg_svg_internal_element_find(Edom_Tag *t, const char *id)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	return eina_hash_find(thiz->ids, id);
}

Etch * esvg_svg_etch_get(Ender_Element *e)
{
	Esvg_Svg *thiz;
	Edom_Tag *t;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);
	return thiz->etch;
}

/* The ender wrapper */
#define _esvg_svg_x_is_set NULL
#define _esvg_svg_y_is_set NULL
#define _esvg_svg_width_is_set NULL
#define _esvg_svg_height_is_set NULL
#define _esvg_svg_actual_width_set NULL
#define _esvg_svg_actual_width_is_set NULL
#define _esvg_svg_actual_height_set NULL
#define _esvg_svg_actual_height_is_set NULL
#define _esvg_svg_viewbox_get NULL
#define _esvg_svg_viewbox_is_set NULL
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

	t = ender_element_object_get(e);
	_esvg_svg_actual_width_get(t, actual_width);
}

EAPI void esvg_svg_actual_height_get(Ender_Element *e, double *actual_height)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	_esvg_svg_actual_height_get(t, actual_height);
}

EAPI void esvg_svg_animations_pause(Ender_Element *e)
{
	Edom_Tag *t;
	Esvg_Svg *thiz;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);
	thiz->paused = EINA_TRUE;
}

EAPI void esvg_svg_animations_unpause(Ender_Element *e)
{
	Edom_Tag *t;
	Esvg_Svg *thiz;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);
	thiz->paused = EINA_FALSE;
}

EAPI Eina_Bool esvg_svg_animations_paused(Ender_Element *e)
{
	Edom_Tag *t;
	Esvg_Svg *thiz;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);
	return thiz->paused;
}

EAPI void esvg_svg_time_tick(Ender_Element *e)
{
	Edom_Tag *t;
	Esvg_Svg *thiz;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);
	etch_timer_tick(thiz->etch);
}

EAPI double esvg_svg_time_get(Ender_Element *e)
{
	Edom_Tag *t;
	Esvg_Svg *thiz;
	double time;
	unsigned long secs;
	unsigned long usecs;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);
	etch_timer_get(thiz->etch, &secs, &usecs);

	time = secs + (double)(usecs / 10000000);

	return time;
}

EAPI void esvg_svg_time_set(Ender_Element *e, double secs)
{
	Edom_Tag *t;
	Esvg_Svg *thiz;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);
	//etch_timer_goto(Etch *e, unsigned long frame);
}
