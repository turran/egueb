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
#include "esvg_private_svg.h"
#include "esvg_private_a.h"
#include "esvg_private_input.h"
#include "esvg_private_style.h"

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
 * Don't use a clipper, but a rectangle or we wont be able to also transform
 * a svg when it is included inside another
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT esvg_log_svg

static Ender_Property *ESVG_SVG_X;
static Ender_Property *ESVG_SVG_Y;
static Ender_Property *ESVG_SVG_WIDTH;
static Ender_Property *ESVG_SVG_HEIGHT;
static Ender_Property *ESVG_SVG_ACTUAL_WIDTH;
static Ender_Property *ESVG_SVG_ACTUAL_HEIGHT;
static Ender_Property *ESVG_SVG_VIEWBOX;

typedef struct _Esvg_Svg_User_Descriptor
{
	/* TODO what we really need here is not a way to retrive generic
	 * data relative or abolsute, but a way to get needed information
	 * for *known* data. For example, we need a way to load an image
	 * from http or load a fragment from other svg, so better
	 * "name" what we need to fetch externally
	 *
	 * helpful methods would be:
	 * _base_uri/dir_get() get the base uri/dir of the svg being loaded
	 * to handle relative files
	 * _image_load() to override the emage default loader (like the async one)
	 * _element_get() to get a fragment from another svg
	 */
	char *base_dir;
} Esvg_Svg_User_Descriptor;

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
	/* user provded properties */
	double base_font_size;
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
	/* user provided callbacks */
	Esvg_Svg_User_Descriptor user_descriptor;
	/* animation */
	Etch *etch;
	Eina_Bool paused;
} Esvg_Svg;

typedef struct _Esvg_Svg_Uri_Data
{
	Esvg_Svg *thiz;
	void *ret;
	void *data;
} Esvg_Svg_Uri_Data;

static Eina_Bool _esvg_svg_child_initialize(Edom_Tag *t, Edom_Tag *child_t, void *data);
static Eina_Bool _esvg_svg_child_deinitialize(Edom_Tag *t, Edom_Tag *child_t, void *data);

static Esvg_Svg * _esvg_svg_get(Edom_Tag *t)
{
	Esvg_Svg *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_SVG)
		return NULL;
	thiz = esvg_renderable_data_get(t);

	return thiz;
}

static Eina_Bool _esvg_svg_relative_to_absolute(Esvg_Svg *thiz, const char *relative, char *absolute, size_t len)
{
	if (!thiz->user_descriptor.base_dir)
	{
		printf("No base dir set\n");
		return EINA_FALSE;
	}

	/* get the base dir and concat with the relative path */
	strncpy(absolute, thiz->user_descriptor.base_dir, len);
	len -= strlen(thiz->user_descriptor.base_dir);
	if (len <= 0) return EINA_FALSE;
	strncat(absolute, relative, len);
	return EINA_TRUE;
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

	width = esvg_length_final_get(&thiz->width, ctx->viewbox.width, ctx->font_size);
	height = esvg_length_final_get(&thiz->height, ctx->viewbox.height, ctx->font_size);
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

	width = thiz->view_box.width;
	height = thiz->view_box.height;

	enesim_matrix_scale(&scale, 1.0/new_vw, 1.0/new_vh);
	enesim_matrix_compose(&scale, &ctx->transform, &ctx->transform);
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
 *                       The URI interface for uris                           *
 *----------------------------------------------------------------------------*/
static void _esvg_svg_resolve_uri_local_get(const char *name,
		const char *fragment, void *user_data)
{
	Esvg_Svg_Uri_Data *data = user_data;
	Esvg_Svg *thiz = data->thiz;
	char **real = data->ret;

	printf("TODO looking for %s\n", fragment);
	/* TODO we should concat the svg file + the fragment */
}

static void _esvg_svg_resolve_uri_absolute_get(const char *name,
		const char *fragment, void *user_data)
{
	Esvg_Svg_Uri_Data *data = user_data;
	Esvg_Svg *thiz = data->thiz;
	char **real = data->ret;
	size_t len;

	*real = strdup(name);
	/* FIXME what about the fragment? */
}

static void _esvg_svg_resolve_uri_relative_get(const char *name,
		const char *fragment, void *user_data)
{
	Esvg_Svg_Uri_Data *data = user_data;
	Esvg_Svg *thiz = data->thiz;
	char **real = data->ret;
	char absolute[PATH_MAX];

	if (!_esvg_svg_relative_to_absolute(thiz, name, absolute, PATH_MAX))
		return;
	*real = strdup(absolute);
	/* FIXME what about the fragment? */
	*real = strdup(absolute);
}

static Esvg_Uri_Descriptor _uri_resolve_descriptor = {
	/* .local_get 		= */ _esvg_svg_resolve_uri_local_get,
	/* .absolute_get 	= */ _esvg_svg_resolve_uri_absolute_get,
	/* .relative_get 	= */ _esvg_svg_resolve_uri_relative_get,
};
/*----------------------------------------------------------------------------*
 *                     The URI interface for elements                         *
 *----------------------------------------------------------------------------*/
static void _esvg_svg_element_uri_local_get(const char *name,
		const char *fragment, void *user_data)
{
	Esvg_Svg_Uri_Data *data = user_data;
	Esvg_Svg *thiz = data->thiz;
	Ender_Element **e = data->ret;

	*e = eina_hash_find(thiz->ids, fragment);
}

static void _esvg_svg_element_uri_absolute_get(const char *name,
		const char *fragment, void *user_data)
{
	printf("TODO fetching an absolute uri element\n");
}

static void _esvg_svg_element_uri_relative_get(const char *name,
		const char *fragment, void *user_data)
{
	printf("TODO fetching an relative uri element\n");
}

static Esvg_Uri_Descriptor _uri_element_descriptor = {
	/* .local_get 		= */ _esvg_svg_element_uri_local_get,
	/* .absolute_get 	= */ _esvg_svg_element_uri_absolute_get,
	/* .relative_get 	= */ _esvg_svg_element_uri_relative_get,
};
/*----------------------------------------------------------------------------*
 *                      The URI interface for images                          *
 *----------------------------------------------------------------------------*/
static void _esvg_svg_image_uri_local_get(const char *name,
		const char *fragment, void *user_data)
{
	printf("TODO fetching a local image?\n");
}

static void _esvg_svg_image_uri_absolute_get(const char *name,
		const char *fragment, void *user_data)
{
	Esvg_Svg_Uri_Data *data = user_data;
	Esvg_Svg *thiz = data->thiz;
	Enesim_Surface **s = data->ret;
	const char *options = data->data;
	Eina_Bool ret;

	printf("loading image, surface is %s\n", name);
	ret = emage_load(name, s, ENESIM_FORMAT_ARGB8888, NULL, options);
	if (!ret)
	{
		printf("some error?\n");
		return;
	}
	printf("everything went ok!\n");
}

static void _esvg_svg_image_uri_relative_get(const char *name,
		const char *fragment, void *user_data)
{
	Esvg_Svg_Uri_Data *data = user_data;
	Esvg_Svg *thiz = data->thiz;
	char absolute[PATH_MAX];
	size_t len;

	if (!_esvg_svg_relative_to_absolute(thiz, name, absolute, PATH_MAX))
		return;
	/* call the absolute one */
	_esvg_svg_image_uri_absolute_get(absolute, fragment, user_data);
}

static Esvg_Uri_Descriptor _uri_image_descriptor = {
	/* .local_get 		= */ _esvg_svg_image_uri_local_get,
	/* .absolute_get 	= */ _esvg_svg_image_uri_absolute_get,
	/* .relative_get 	= */ _esvg_svg_image_uri_relative_get,
};

static Eina_Bool _esvg_svg_child_topmost_set(Edom_Tag *t, Edom_Tag *child,
		void *data)
{
	Ender_Element *topmost = data;

	esvg_element_topmost_set(child, topmost);
	return EINA_TRUE;
}

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
	Ender_Element *thiz_e = data;
	Edom_Tag *child_child;

	child_child = ender_value_object_get(ev->value);
	switch (ev->type)
	{
		/* some child has been added to the whole svg tree */
		case ENDER_EVENT_MUTATION_ADD:
		/* set the topmost */
		esvg_element_topmost_set(child_child, thiz_e);
		break;

		/* some child has been removed from the whole svg tree */
		case ENDER_EVENT_MUTATION_REMOVE:
		esvg_element_topmost_set(child_child, NULL);
		break;

		default:
		printf("TODO\n");
		break;
	}
}

/* called whenever the svg has been set as a parent of another
 * element
 */
static void _esvg_svg_topmost_changed_cb(Ender_Element *e, const char *event_name,
		void *event_data, void *data)
{
	Esvg_Element_Event_Topmost_Changed *ev = event_data;
	Edom_Tag *child_t;
	Ender_Element *child_e;

	child_t = ev->child;
	child_e = esvg_element_ender_get(child_t);
	if (ev->previous == e)
	{
		Esvg_Svg *thiz;
		Edom_Tag *topmost_p;

		topmost_p = ender_element_object_get(ev->previous);
		thiz = _esvg_svg_get(topmost_p);
		/* remove previous callbacks on the element */
		_esvg_svg_child_deinitialize(topmost_p, child_t, thiz);
	}
	if (ev->current == e)
	{
		Esvg_Svg *thiz;
		Edom_Tag *topmost_c;

		DBG("Setting topmost on %s", esvg_type_string_to(esvg_element_internal_type_get(child_t)));
		topmost_c = ender_element_object_get(ev->current);
		thiz = _esvg_svg_get(topmost_c);
		/* setup all the needed callbacks on the element */
		_esvg_svg_child_initialize(topmost_c, child_t, thiz);
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

	thiz_e = esvg_element_ender_get(t);
	//printf("initializing %s\n", esvg_type_string_to(esvg_element_internal_type_get(child_t)));
	/* add a callback whenever the child property has changed
	 * to initialize that child too */
	child_e = esvg_element_ender_get(child_t);
	ender_event_listener_add(child_e, "Mutation:child", _esvg_svg_child_mutation_child_cb, thiz_e);
	/* add a callback whenever any property has changed to add it the list
	 * of changed elements */
	ender_event_listener_add(child_e, "Mutation", _esvg_svg_child_mutation_cb, thiz);
	/* add an event whenever the child changes the id */
	ender_event_listener_add(child_e, "Mutation:id", _esvg_svg_child_id_cb, thiz);
	/* add the id to the hash of ids */
	esvg_element_id_get(child_e, &id);
	if (id) eina_hash_add(thiz->ids, id, child_e);

	/* add the style to the list of styles */
	if (esvg_style_is_internal(child_t))
	{
		thiz->styles = eina_list_append(thiz->styles, child_t);
		thiz->styles_changed = EINA_TRUE;
	}
	/* mark it for processing */
	_esvg_svg_element_changed_add(thiz, child_e);
	/* set the topmost on every element */
	edom_tag_child_foreach(child_t, _esvg_svg_child_topmost_set, thiz_e);

	return EINA_TRUE;
}

static Eina_Bool _esvg_svg_child_deinitialize(Edom_Tag *t, Edom_Tag *child_t, void *data)
{
	Esvg_Svg *thiz;
	Ender_Element *thiz_e;
	Ender_Element *child_e;
	const char *id;

 	thiz = data;

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
	if (esvg_style_is_internal(child_t))
	{
		thiz->styles = eina_list_remove(thiz->styles, child_t);
		thiz->styles_changed = EINA_TRUE;
	}
	/* mark it for processing */
	_esvg_svg_element_changed_remove(thiz, child_e);
	/* set the topmost on every element */
	edom_tag_child_foreach(child_t, _esvg_svg_child_topmost_set, NULL);

	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                       The Esvg Renderable interface                        *
 *----------------------------------------------------------------------------*/
static void _esvg_svg_initialize(Ender_Element *e)
{
	/* called whenever the topmost changes */
	ender_event_listener_add(e, "TopmostChanged", _esvg_svg_topmost_changed_cb, NULL);
}

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
	Ender_Element *e;

	if (!esvg_is_element_internal(child))
		return EINA_FALSE;

	thiz = _esvg_svg_get(t);
	type = esvg_element_internal_type_get(child);
	if (esvg_type_is_renderable(type) || type == ESVG_A)
	{
		thiz->renderable_tree_changed = EINA_TRUE;
		enesim_renderer_compound_layer_clear(thiz->compound);
		enesim_renderer_compound_layer_add(thiz->compound, thiz->background);
	}

	e = esvg_element_ender_get(t);
	esvg_element_topmost_set(child, e);

	return EINA_TRUE;
}

static Eina_Bool _esvg_svg_child_remove(Edom_Tag *t, Edom_Tag *child)
{
	Esvg_Svg *thiz;
	Esvg_Type type;

	if (!esvg_is_element_internal(child))
		return EINA_FALSE;

	thiz = _esvg_svg_get(t);
	type = esvg_element_internal_type_get(child);
	if (esvg_type_is_renderable(type) || type == ESVG_A)
	{
		thiz->renderable_tree_changed = EINA_TRUE;
		enesim_renderer_compound_layer_clear(thiz->compound);
		enesim_renderer_compound_layer_add(thiz->compound, thiz->background);
	}
	esvg_element_topmost_set(child, NULL);

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
		width = esvg_length_final_get(&thiz->width, ctx->viewbox.width, thiz->base_font_size);
		height = esvg_length_final_get(&thiz->height, ctx->viewbox.height, thiz->base_font_size);
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

			width = thiz->view_box.width;
			height = thiz->view_box.height;

			enesim_matrix_scale(&scale, 1.0/new_vw, 1.0/new_vh);
			enesim_matrix_compose(&scale, &ctx->transform, &ctx->transform);
			/* TODO handle current matrix */
		}
		ctx->viewbox.width = width;
		ctx->viewbox.height = height;
		/* FIXME for now, it should really be base_font_size * attr.font-size */
		ctx->font_size = thiz->base_font_size;

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

static void _esvg_svg_free(Edom_Tag *t)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	free(thiz);
}

static Esvg_Renderable_Descriptor _descriptor = {
	/* .child_add		= */ _esvg_svg_child_add,
	/* .child_remove	= */ _esvg_svg_child_remove,
	/* .attribute_get 	= */ _esvg_svg_attribute_get,
	/* .cdata_set 		= */ NULL,
	/* .text_set 		= */ NULL,
	/* .free 		= */ _esvg_svg_free,
	/* .initialize 		= */ _esvg_svg_initialize,
	/* .attribute_set 	= */ _esvg_svg_attribute_set,
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
	/* FIXME we are using 16px here, this 16px refer to the 'normal' font-size */
	thiz->base_font_size = 16;

	/* no default value for the view_box */
	/* the animation system */
	thiz->etch = etch_new();
	etch_timer_fps_set(thiz->etch, 30);

	t = esvg_renderable_new(&_descriptor, ESVG_SVG, thiz);
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
	aw = esvg_length_final_get(&thiz->width, cw, thiz->base_font_size);
	*actual_width = aw;
}

static void _esvg_svg_actual_height_get(Edom_Tag *t, double *actual_height)
{
	Esvg_Svg *thiz;
	double ah;
	double ch;

	thiz = _esvg_svg_get(t);
	esvg_renderable_internal_container_height_get(t, &ch);
	ah = esvg_length_final_get(&thiz->height, ch, thiz->base_font_size);
	*actual_height = ah;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_svg_element_get(Ender_Element *e, const char *uri, Ender_Element **el)
{
	Esvg_Svg_Uri_Data data;
	Edom_Tag *t;
	Esvg_Svg *thiz;

	if (!el) return;
	if (!uri) return;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);

	data.thiz = thiz;
	data.ret = el;
	/* resolve the uri for relative/absolute */
	DBG("Looking for %s", uri);
	esvg_iri_string_from(uri, &_uri_element_descriptor, &data);
}

void esvg_svg_image_load(Ender_Element *e, const char *uri, Enesim_Surface **s, const char *options)
{
	Esvg_Svg_Uri_Data data;
	Edom_Tag *t;
	Esvg_Svg *thiz;

	if (!s) return;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);

	data.thiz = thiz;
	data.ret = (void *)s;
	data.data = (void *)options;
	/* resolve the uri for relative/absolute */
	esvg_iri_string_from(uri, &_uri_image_descriptor, &data);
}

char * esvg_svg_uri_resolve(Ender_Element *e, const char *uri)
{
	Esvg_Svg_Uri_Data data;
	Edom_Tag *t;
	Esvg_Svg *thiz;
	char *ret = NULL;

	if (!uri) return NULL;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);

	data.thiz = thiz;
	data.ret = &ret;
	/* resolve the uri for relative/absolute */
	esvg_iri_string_from(uri, &_uri_resolve_descriptor, &data);

	return ret;
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
#define _esvg_svg_delete NULL
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
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Element * esvg_svg_new(void)
{
	return ender_element_new_with_namespace("svg", "esvg");
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Element * esvg_svg_element_find(Ender_Element *e, const char *id)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	return esvg_svg_internal_element_find(t, id);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool esvg_is_svg(Ender_Element *e)
{
	if (esvg_element_type_get(e) != ESVG_SVG)
		return EINA_FALSE;
	return EINA_TRUE;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_version_set(Ender_Element *e, double version)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_version_get(Ender_Element *e, double *version)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_x_set(Ender_Element *e, Esvg_Coord *x)
{
	ender_element_property_value_set(e, ESVG_SVG_X, x, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_y_set(Ender_Element *e, Esvg_Coord *y)
{
	ender_element_property_value_set(e, ESVG_SVG_Y, y, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_width_set(Ender_Element *e, Esvg_Length *width)
{
	ender_element_property_value_set(e, ESVG_SVG_WIDTH, width, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_width_get(Ender_Element *e, Esvg_Length *width)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	_esvg_svg_width_get(t, width);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_height_set(Ender_Element *e, Esvg_Length *height)
{
	ender_element_property_value_set(e, ESVG_SVG_HEIGHT, height, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_height_get(Ender_Element *e, Esvg_Length *height)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	_esvg_svg_height_get(t, height);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
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

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_actual_height_get(Ender_Element *e, double *actual_height)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	_esvg_svg_actual_height_get(t, actual_height);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_animations_pause(Ender_Element *e)
{
	Edom_Tag *t;
	Esvg_Svg *thiz;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);
	thiz->paused = EINA_TRUE;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_animations_unpause(Ender_Element *e)
{
	Edom_Tag *t;
	Esvg_Svg *thiz;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);
	thiz->paused = EINA_FALSE;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool esvg_svg_animations_paused(Ender_Element *e)
{
	Edom_Tag *t;
	Esvg_Svg *thiz;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);
	return thiz->paused;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_time_tick(Ender_Element *e)
{
	Edom_Tag *t;
	Esvg_Svg *thiz;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);
	etch_timer_tick(thiz->etch);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI double esvg_svg_time_get(Ender_Element *e)
{
	Edom_Tag *t;
	Esvg_Svg *thiz;
	Etch_Time et;
	double time;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);
	etch_timer_get(thiz->etch, &et);

	/* FIXME fix this */
	time = et / 10000000;

	return time;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_time_set(Ender_Element *e, double secs)
{
	Edom_Tag *t;
	Esvg_Svg *thiz;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);
	//etch_timer_goto(Etch *e, unsigned long frame);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_feed_mouse_move(Ender_Element *e, int x, int y)
{
	double dx;
	double dy;

	/* always double coordinates */
	dx = x;
	dy = y;
	/* send the mouse move to the input system */
	esvg_input_feed_mouse_move(e, dx, dy);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_feed_mouse_down(Ender_Element *e)
{

}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_feed_mouse_up(Ender_Element *e)
{

}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_List * esvg_svg_intersection_list_get(Ender_Element *e, Enesim_Rectangle *rect)
{
	return NULL;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI const char * esvg_svg_base_dir_get(Ender_Element *e)
{
	Edom_Tag *t;
	Esvg_Svg *thiz;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);
	return thiz->user_descriptor.base_dir;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_base_dir_set(Ender_Element *e, const char *base_dir)
{
	Edom_Tag *t;
	Esvg_Svg *thiz;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);

	if (thiz->user_descriptor.base_dir)
	{
		free(thiz->user_descriptor.base_dir);
		thiz->user_descriptor.base_dir = NULL;
	}

	if (base_dir)
		thiz->user_descriptor.base_dir = strdup(base_dir);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI double esvg_svg_base_font_size_get(Ender_Element *e)
{
	Edom_Tag *t;
	Esvg_Svg *thiz;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);
	return thiz->base_font_size;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_base_font_size_set(Ender_Element *e, double base_font_size)
{
	Edom_Tag *t;
	Esvg_Svg *thiz;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);

	thiz->base_font_size = base_font_size;
}

/* FIXME this should be the correct entry point to process
 * the whole svg tree, instead of the setup we have on
 * esvg_element.c this one should call the global symbol
 * to do the process which will do the setup/clanup
 * in case of a referenceable for example, it will do
 * also the propagate
 */
EAPI void esvg_svg_process(Ender_Element *e, Enesim_Error **error)
{
	/* in case we are the topmost use NULL as the parent's state */
	esvg_element_setup(e, error);
}
