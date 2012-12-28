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
#include "esvg_private_main.h"
#include "esvg_private_attribute_presentation.h"
#include "esvg_private_context.h"
#include "esvg_private_element.h"
#include "esvg_private_renderable.h"
#include "esvg_private_renderable_container.h"
#include "esvg_private_svg.h"
#include "esvg_private_a.h"
#include "esvg_private_input.h"
#include "esvg_private_script_provider.h"
#include "esvg_private_style.h"

#include "esvg_svg.h"
#include "esvg_element.h"
#include "esvg_renderable.h"
#include "esvg_event.h"
#include "esvg_parser.h"
#include "esvg_video_provider.h"

#include "esvg_private_video_provider.h"
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
#define ESVG_LOG_DEFAULT _esvg_svg_log

static int _esvg_svg_log = -1;

static Ender_Property *ESVG_SVG_X;
static Ender_Property *ESVG_SVG_Y;
static Ender_Property *ESVG_SVG_WIDTH;
static Ender_Property *ESVG_SVG_HEIGHT;
static Ender_Property *ESVG_SVG_VIEWBOX;
static Ender_Property *ESVG_SVG_X_DPI;
static Ender_Property *ESVG_SVG_Y_DPI;
static Ender_Property *ESVG_SVG_CONTENT_SCRIPT_TYPE;

static Ender_Property *ESVG_SVG_ACTUAL_WIDTH;
static Ender_Property *ESVG_SVG_ACTUAL_HEIGHT;
static Ender_Property *ESVG_SVG_CONTAINER_WIDTH;
static Ender_Property *ESVG_SVG_CONTAINER_HEIGHT;

static Ender_Function *ESVG_SVG_ELEMENT_GET_BY_ID;

typedef struct _Esvg_Svg
{
	/* properties */
	double version;
	Esvg_Rect view_box;
	Eina_Bool view_box_set;
	Esvg_Coord x;
	Esvg_Coord y;
	Esvg_Length width;
	Esvg_Length height;
	double container_width;
	double container_height;
	double x_dpi;
	double y_dpi;
	char *content_script_type;
	/* user provded properties */
	double base_font_size;
	/* private */
	Ender_Element *thiz_e;
	Eina_Bool relative_size : 1; /* to know if the width/height are relative */
	/* keep track if the renderable tree has changed, includeing the <a> tag */
	Eina_Bool renderable_tree_changed : 1;
	Eina_List *styles; /* the list of styles found on this svg scope */
	Eina_Bool styles_changed : 1;
	Eina_List *elements_changed;
	Enesim_Renderer *clipper;
	Enesim_Renderer *compound;
	/* FIXME we use a background because of the blend/fill thing on the redraws */
	Enesim_Renderer *background;
	Eina_Hash *ids; /* the ids found */
	/* user provided callbacks */
	const Esvg_Svg_Application_Descriptor *application_descriptor;
	void *application_data;
	/* animation */
	Etch *etch;
	Eina_Bool paused;
	/* input */
	Esvg_Renderable_Container *container;
	Esvg_Input *input;
	/* damages */
	Eina_Tiler *tiler;
	int tw;
	int th;
	/* svg inclusion */
	Eina_List *svgs; /* the list of svg documents found on the svg */
	Eina_List *image_svgs; /* the list of svg images on the svg */
	/* scripts */
	Eina_Hash *scriptors;
} Esvg_Svg;

typedef struct _Esvg_Svg_Uri_Data
{
	Esvg_Svg *thiz;
	void *ret;
	void *data;
} Esvg_Svg_Uri_Data;

typedef struct _Esvg_Svg_Uri_Image_Data
{
	Esvg_Svg *thiz;
	Enesim_Surface **s;
	double width;
	double height;
} Esvg_Svg_Uri_Image_Data;

typedef struct _Esvg_Svg_Image
{
	Ender_Element *svg;
	/* TODO the image tag that has a svg file set */
	Edom_Tag *referal;
	Enesim_Surface *s;
} Esvg_Svg_Image;

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

static const char * _esvg_svg_base_dir_get(Esvg_Svg *thiz)
{
	if (!thiz->application_descriptor)
		return NULL;
	if (thiz->application_descriptor->base_dir_get)
		return thiz->application_descriptor->base_dir_get(thiz->thiz_e, thiz->application_data);
	return NULL;
}

static Eina_Bool _esvg_svg_damage_cb(Enesim_Renderer *r,
		const Eina_Rectangle *area, Eina_Bool past,
		void *data)
{
	Eina_Tiler *tiler = data;
	const char *name;

	eina_tiler_rect_add(tiler, area);
	enesim_renderer_name_get(r, &name);
	DBG("renderer %s has changed at area %d %d %d %d", name, area->x, area->y, area->w, area->h);
	return EINA_TRUE;
}

static Eina_Bool _esvg_svg_relative_to_absolute(Esvg_Svg *thiz, const char *relative, char *absolute, size_t len)
{
	const char *base_dir;
	size_t base_dir_len;

	base_dir = _esvg_svg_base_dir_get(thiz);
	if (!base_dir)
	{
		WARN("No base dir available");
		return EINA_FALSE;
	}

	/* get the base dir and concat with the relative path */
	strncpy(absolute, base_dir, len);
	base_dir_len = strlen(base_dir);
	len -= base_dir_len;
	if (len <= 0) return EINA_FALSE;

	/* add the separator */
	if (absolute[base_dir_len - 1] != '/')
	{
		strncat(absolute, "/", len);
		len--;
		if (len <= 0) return EINA_FALSE;
	}

	strncat(absolute, relative, len);
	return EINA_TRUE;
}

static Eina_Bool _esvg_svg_setup_pre(Edom_Tag *t,
		Edom_Tag *child,
		Esvg_Context *c,
		Enesim_Error **error,
		void *data)
{
	Esvg_Type type;
	Esvg_Svg *thiz = data;

	type = esvg_element_internal_type_get(child);
	if (esvg_type_is_renderable(type) && (thiz->renderable_tree_changed || thiz->relative_size))
	{
		return EINA_TRUE;
	}
 	else if (type == ESVG_A)
	{
		return EINA_TRUE;
	}
	else
	{
		return EINA_TRUE;
	}

}

static Eina_Bool _esvg_svg_setup_post(Edom_Tag *t,
		Edom_Tag *child,
		Esvg_Context *c,
		Enesim_Error **error,
		void *data)
{
	Esvg_Type type;
	Esvg_Svg *thiz = data;

	type = esvg_element_internal_type_get(child);
	/* check if it is a renderable */
	if (esvg_type_is_renderable(type) && thiz->renderable_tree_changed)
	{
		Enesim_Renderer *r = NULL;

		/* add to the compound */
		esvg_renderable_internal_renderer_get(child, &r);
		enesim_renderer_compound_layer_add(thiz->compound, r);
		/* add it to the container */
		esvg_renderable_container_renderable_add(thiz->container, child);
	}
 	else if (type == ESVG_A)
	{
		return esvg_element_internal_child_setup(child, c, error,
				_esvg_svg_setup_pre, _esvg_svg_setup_post, thiz);
	}
	return EINA_TRUE;
}

static inline void _esvg_svg_size_apply(Esvg_Svg *thiz, Esvg_Element_Context *ctx)
{
	double width;
	double height;

	width = esvg_coord_final_get(&thiz->width, ctx->viewbox.w, ctx->font_size);
	height = esvg_coord_final_get(&thiz->height, ctx->viewbox.h, ctx->font_size);
	enesim_renderer_clipper_width_set(thiz->clipper, width);
	enesim_renderer_clipper_height_set(thiz->clipper, height);
	ctx->viewbox.w = width;
	ctx->viewbox.h = height;
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

	new_vw = thiz->view_box.w / ctx->viewbox.w;
	new_vh = thiz->view_box.h / ctx->viewbox.h;

	width = thiz->view_box.w;
	height = thiz->view_box.h;

	enesim_matrix_scale(&scale, 1.0/new_vw, 1.0/new_vh);
	enesim_matrix_compose(&scale, &ctx->transform, &ctx->transform);
	ctx->viewbox.w = width;
	ctx->viewbox.h = height;
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
		DBG("Tag '%s' found on the changed list", edom_tag_name_get(changed_t));
		if (!esvg_element_has_setup(changed_t, c))
		{
			DBG("Tag '%s' marked as changed is going to be setup", edom_tag_name_get(changed_t));
			esvg_element_internal_setup(changed_t, c, error);
		}
		thiz->elements_changed = eina_list_remove_list(thiz->elements_changed, l);
	}
}

static void _esvg_svg_element_changed_add(Esvg_Svg *thiz, Ender_Element *e)
{
	DBG("Tag '%s' has been added to the list of changes", esvg_element_name_get(e));
	thiz->elements_changed = eina_list_append(thiz->elements_changed, e);
}

static void _esvg_svg_element_changed_remove(Esvg_Svg *thiz, Ender_Element *e)
{
	DBG("Tag '%s' has been removed from the list of changes", esvg_element_name_get(e));

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
	printf("TODO fetching an absolute uri element '%s'\n", name);
}

static void _esvg_svg_element_uri_relative_get(const char *name,
		const char *fragment, void *user_data)
{
	Esvg_Svg_Uri_Data *data = user_data;
	Esvg_Svg *thiz = data->thiz;
	Ender_Element **e = data->ret;

	*e = eina_hash_find(thiz->ids, name);
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
	Esvg_Svg_Uri_Image_Data *data = user_data;
	Esvg_Svg *thiz = data->thiz;
	Enesim_Surface **s = data->s;
	double width = data->width;
	double height = data->height;
	Eina_Bool ret;

	{
		char options[PATH_MAX];

		options[0] = '\0';
		if (width != 0 && height != 0)
			sprintf(options, "width=%d;height=%d", (int)width, (int)height);

		ret = emage_file_load(name, s, ENESIM_FORMAT_ARGB8888, NULL, options);
		if (!ret)
		{
			Eina_Error err;

			err = eina_error_get();
			ERR("Image '%s' failed to load with error: %s", name, eina_error_msg_get(err));
			return;
		}
	}
}

static void _esvg_svg_image_uri_relative_get(const char *name,
		const char *fragment, void *user_data)
{
	Esvg_Svg_Uri_Image_Data *data = user_data;
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
	Ender_Element *child_e;
	const char *id;

 	thiz = data;

	DBG("Initializing '%s'", esvg_type_string_to(esvg_element_internal_type_get(child_t)));
	/* add a callback whenever the child property has changed
	 * to initialize that child too */
	child_e = esvg_element_ender_get(child_t);
	ender_event_listener_add(child_e, "Mutation:child", _esvg_svg_child_mutation_child_cb, thiz->thiz_e);
	/* add a callback whenever any property has changed to add it the list
	 * of changed elements */
	ender_event_listener_add(child_e, "Mutation", _esvg_svg_child_mutation_cb, thiz);
	/* add an event whenever the child changes the id */
	ender_event_listener_add(child_e, "Mutation:id", _esvg_svg_child_id_cb, thiz);
	/* add the id to the hash of ids */
	esvg_element_id_get(child_e, &id);
	if (id)
	{
		DBG("Adding '%s' to the list of ids", id);
		eina_hash_add(thiz->ids, id, child_e);
	}

	/* add the style to the list of styles */
	if (esvg_style_is_internal(child_t))
	{
		thiz->styles = eina_list_append(thiz->styles, child_t);
		thiz->styles_changed = EINA_TRUE;
	}
	/* mark it for processing */
	_esvg_svg_element_changed_add(thiz, child_e);
	/* set the topmost on every element */
	edom_tag_child_foreach(child_t, _esvg_svg_child_topmost_set, thiz->thiz_e);

	return EINA_TRUE;
}

static Eina_Bool _esvg_svg_child_deinitialize(Edom_Tag *t, Edom_Tag *child_t, void *data)
{
	Esvg_Svg *thiz;
	Ender_Element *child_e;
	const char *id;

 	thiz = data;

	/* add a callback whenever the child property has changed
	 * to initialize that child too */
	child_e = esvg_element_ender_get(child_t);
	ender_event_listener_remove_full(child_e, "Mutation:child", _esvg_svg_child_mutation_child_cb, thiz->thiz_e);
	/* add a callback whenever any property has changed to add it the list
	 * of changed elements */
	ender_event_listener_remove_full(child_e, "Mutation", _esvg_svg_child_mutation_cb, thiz);
	/* add an event whenever the child changes the id */
	ender_event_listener_remove_full(child_e, "Mutation:id", _esvg_svg_child_id_cb, thiz);
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
 *                           The Esvg Input interface                         *
 *----------------------------------------------------------------------------*/
static Ender_Element * _esvg_svg_element_at(void *data, int x, int y)
{
	Edom_Tag *t = data;
	Esvg_Svg *thiz;
	Eina_Rectangle in;
	Eina_Rectangle bounds;
	Ender_Element *e;

	thiz = _esvg_svg_get(t);
	eina_rectangle_coords_from(&in, x, y, 1, 1);
	enesim_renderer_destination_boundings(thiz->clipper, &bounds, 0, 0);
	//printf("%s: %d %d - %d %d %d %d\n", edom_tag_name_get(t), x, y, bounds.x, bounds.y, bounds.w, bounds.h);
	if (!eina_rectangles_intersect(&bounds, &in))
		return NULL;

	e = esvg_element_ender_get(t);
	return e;
}

static Esvg_Input_Descriptor _esvg_svg_input_descriptor = {
	/* .element_at 	= */ _esvg_svg_element_at,
};
/*----------------------------------------------------------------------------*
 *                       The Esvg Renderable interface                        *
 *----------------------------------------------------------------------------*/
static void _esvg_svg_initialize(Ender_Element *e)
{
	Esvg_Svg *thiz;
	Edom_Tag *t;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);

	thiz->thiz_e = e;
	thiz->container = esvg_renderable_container_new(e);
	/* called whenever the topmost changes */
	ender_event_listener_add(e, "TopmostChanged", _esvg_svg_topmost_changed_cb, NULL);
	/* own input system */
	thiz->input = esvg_input_new(&_esvg_svg_input_descriptor, t);
}

static Eina_Bool _esvg_svg_attribute_set(Ender_Element *e, const char *key, const char *value)
{
	if (strcmp(key, "version") == 0)
	{
		Esvg_Number n;
		esvg_number_string_from(&n, value, 0.0);
		esvg_svg_version_set(e, n.value);
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
		Esvg_Rect viewbox;
		esvg_rect_string_from(&viewbox, value);
		esvg_svg_viewbox_set(e, &viewbox);
	}
	else if (strcmp(key, "contentScriptType") == 0)
	{
		esvg_svg_content_script_type_set(e, value);
	}
	else
	{
		return EINA_FALSE;
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

	thiz = _esvg_svg_get(t);
	type = esvg_element_internal_type_get(child);
	if (esvg_type_is_renderable(type) || type == ESVG_A)
	{
		thiz->renderable_tree_changed = EINA_TRUE;
	}

	e = esvg_element_ender_get(t);
	esvg_element_topmost_set(child, e);

	return EINA_TRUE;
}

static Eina_Bool _esvg_svg_child_remove(Edom_Tag *t, Edom_Tag *child)
{
	Esvg_Svg *thiz;
	Esvg_Type type;

	thiz = _esvg_svg_get(t);
	type = esvg_element_internal_type_get(child);
	if (esvg_type_is_renderable(type) || type == ESVG_A)
	{
		thiz->renderable_tree_changed = EINA_TRUE;
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
	if (!changed && !thiz->elements_changed && !thiz->renderable_tree_changed)
		return EINA_TRUE;

	ctx->viewbox.x = 0;
	ctx->viewbox.y = 0;
	ctx->viewbox.w = thiz->container_width;
	ctx->viewbox.h = thiz->container_height;

	ctx->dpi_y = thiz->x_dpi;
	ctx->dpi_x = thiz->y_dpi;
	if (thiz->renderable_tree_changed)
	{
		esvg_renderable_container_clear(thiz->container);
		enesim_renderer_compound_layer_clear(thiz->compound);
		enesim_renderer_compound_layer_add(thiz->compound, thiz->background);
	}
	/* 1. if the attr or the context have changed, then we need to propagate on the whole tree again
	 * that means that if for some reason some renderable direct child or the a tag has been added/removed
	 * (renderable_tree_changed flag) on the setup process we also need to add the renderers
	 * to our compound
	 */
	//if (changed)
	{
		width = esvg_coord_final_get(&thiz->width, ctx->viewbox.w, thiz->base_font_size);
		height = esvg_coord_final_get(&thiz->height, ctx->viewbox.h, thiz->base_font_size);
		enesim_renderer_clipper_width_set(thiz->clipper, width);
		enesim_renderer_clipper_height_set(thiz->clipper, height);
		/* the viewbox will set a new user space coordinate */
		/* FIXME check zeros */
		if (thiz->view_box_set)
		{
			Enesim_Matrix scale;
			double new_vw;
			double new_vh;

			new_vw = thiz->view_box.w / width;
			new_vh = thiz->view_box.h / height;

			width = thiz->view_box.w;
			height = thiz->view_box.h;

			enesim_matrix_scale(&scale, 1.0/new_vw, 1.0/new_vh);
			enesim_matrix_compose(&scale, &ctx->transform, &ctx->transform);
			/* TODO handle current matrix */
		}
		ctx->viewbox.w = width;
		ctx->viewbox.h = height;
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
	ret = esvg_element_internal_child_setup(t, c, error,
		_esvg_svg_setup_pre, _esvg_svg_setup_post, thiz);
	thiz->renderable_tree_changed = EINA_FALSE;

	/* 4. now process the list of changed elements, it will automatically do the setup on its
	 * parent
	 */
	/* FIXME here we are also doing the setup on every referenceable child,
	 * for example a clipPath with a rect children, the rect and its childs will
	 * also do the setup, which is something not desirable
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
	enesim_renderer_unref(thiz->clipper);
	enesim_renderer_unref(thiz->background);
	enesim_renderer_unref(thiz->compound);
	/* TODO remove etch and all the animation system  */
	/* TODO free the scriptors */
	free(thiz);
}

static Esvg_Renderable_Descriptor _descriptor = {
	/* .child_add		= */ _esvg_svg_child_add,
	/* .child_remove	= */ _esvg_svg_child_remove,
	/* .attribute_get 	= */ _esvg_svg_attribute_get,
	/* .cdata_set 		= */ NULL,
	/* .text_set 		= */ NULL,
	/* .text_get 		= */ NULL,
	/* .free 		= */ _esvg_svg_free,
	/* .initialize 		= */ _esvg_svg_initialize,
	/* .attribute_set 	= */ _esvg_svg_attribute_set,
	/* .attribute_animated_set = */ NULL,
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

	thiz->container_width = 640;
	thiz->container_height = 480;
	thiz->x_dpi = 96.0;
	thiz->y_dpi = 96.0;

	thiz->content_script_type = strdup("application/ecmascript");
	/* no default value for the view_box */

	/* the animation system */
	thiz->etch = etch_new();
	etch_timer_fps_set(thiz->etch, 30);
	t = esvg_renderable_new(&_descriptor, ESVG_SVG, thiz);
	/* the script system */
	thiz->scriptors = eina_hash_string_superfast_new(NULL);
	return t;
}

static Edom_Tag * _esvg_svg_element_find(Edom_Tag *tag, const char *id)
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
	if (esvg_length_is_relative(width))
		thiz->relative_size = EINA_TRUE;
	else
		thiz->relative_size = EINA_FALSE;
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
	if (esvg_length_is_relative(height))
		thiz->relative_size = EINA_TRUE;
	else
		thiz->relative_size = EINA_FALSE;
}

static void _esvg_svg_height_get(Edom_Tag *t, Esvg_Length *height)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	if (height) *height = thiz->height;
}

static void _esvg_svg_viewbox_set(Edom_Tag *t, Esvg_Rect *vb)
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
	esvg_svg_internal_container_width_get(t, &cw);
	aw = esvg_coord_final_get(&thiz->width, cw, thiz->base_font_size);
	*actual_width = aw;
}

static void _esvg_svg_actual_height_get(Edom_Tag *t, double *actual_height)
{
	Esvg_Svg *thiz;
	double ah;
	double ch;

	thiz = _esvg_svg_get(t);
	esvg_svg_internal_container_height_get(t, &ch);
	ah = esvg_coord_final_get(&thiz->height, ch, thiz->base_font_size);
	*actual_height = ah;
}

static void _esvg_svg_container_width_set(Edom_Tag *t, double container_width)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	thiz->container_width = container_width;
}

static void _esvg_svg_container_height_set(Edom_Tag *t, double container_height)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	thiz->container_height = container_height;
}

static void _esvg_svg_x_dpi_set(Edom_Tag *t, double x_dpi)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	thiz->x_dpi = x_dpi;
}

static void _esvg_svg_x_dpi_get(Edom_Tag *t, double *x_dpi)
{
	Esvg_Svg *thiz;

	if (!x_dpi)
		return;
	thiz = _esvg_svg_get(t);
	*x_dpi = thiz->x_dpi;
}

static void _esvg_svg_y_dpi_set(Edom_Tag *t, double y_dpi)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	thiz->y_dpi = y_dpi;
}

static void _esvg_svg_y_dpi_get(Edom_Tag *t, double *y_dpi)
{
	Esvg_Svg *thiz;

	if (!y_dpi)
		return;
	thiz = _esvg_svg_get(t);
	*y_dpi = thiz->y_dpi;
}

static void _esvg_svg_content_script_type_set(Edom_Tag *t, const char *v)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(t);
	if (thiz->content_script_type)
	{
		free(thiz->content_script_type);
		thiz->content_script_type = NULL;
	}

	if (v)
	{
		thiz->content_script_type = strdup(v);
	}
}

static void _esvg_svg_content_script_type_get(Edom_Tag *t, const char **v)
{
	Esvg_Svg *thiz;

	if (!v) return;
	thiz = _esvg_svg_get(t);
	*v = thiz->content_script_type;
}

static Ender_Element *_esvg_svg_element_get_by_id(Edom_Tag *t, const char *id)
{
	Esvg_Svg *thiz;
	Ender_Element *found;

	thiz = _esvg_svg_get(t);
	found = eina_hash_find(thiz->ids, id);
	return found;
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
#define _esvg_svg_container_width_get esvg_svg_internal_container_width_get
#define _esvg_svg_container_width_is_set NULL
#define _esvg_svg_container_height_get esvg_svg_internal_container_height_get
#define _esvg_svg_container_height_is_set NULL
#define _esvg_svg_x_dpi_is_set NULL
#define _esvg_svg_y_dpi_is_set NULL
#define _esvg_svg_content_script_type_is_set NULL
#include "generated/esvg_generated_svg.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_svg_init(void)
{
	_esvg_svg_log = eina_log_domain_register("esvg_svg", ESVG_LOG_COLOR_DEFAULT);
	if (_esvg_svg_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	/* FIXME: initialize the ender interface */
	_esvg_svg_init();
}

void esvg_svg_shutdown(void)
{
	if (_esvg_svg_log < 0)
		return;
	_esvg_svg_shutdown();
	eina_log_domain_unregister(_esvg_svg_log);
	_esvg_svg_log = -1;
}
/*----------------------------------------------------------------------------*
 *                      The script provider functions                         *
 *----------------------------------------------------------------------------*/
Esvg_Script_Provider * esvg_svg_script_provider_get(Ender_Element *e, const char *type)
{
	Esvg_Svg *thiz;
	Esvg_Script_Provider *scriptor;
	Esvg_Script_Provider_Descriptor *descriptor;
	Edom_Tag *t;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);

	/* return the created scriptor in case it exists on the svg */
	scriptor = eina_hash_find(thiz->scriptors, type);
	if (scriptor) return scriptor;

	descriptor = esvg_script_provider_descriptor_find(type);
	if (!descriptor) return NULL;

	scriptor = esvg_script_provider_new(descriptor, e);
	return scriptor;
}
/*----------------------------------------------------------------------------*
 *                       The video provider functions                         *
 *----------------------------------------------------------------------------*/
Esvg_Video_Provider_Descriptor * esvg_svg_video_provider_descriptor_get(
		Ender_Element *e)
{
	Esvg_Svg *thiz;
	const Esvg_Svg_Application_Descriptor *descriptor;
	Esvg_Video_Provider_Descriptor *ret = NULL;
	Edom_Tag *t;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);

	descriptor = thiz->application_descriptor;
	if (descriptor)
	{
		if (descriptor->video_provider_descriptor_get)
		{
			ret = descriptor->video_provider_descriptor_get(e,
					thiz->application_data);
		}
	}
	if (!ret)
	{
		/* get the default video descriptor */
		/* FIXME for now only gstreamer */
#if BUILD_ESVG_VIDEO_GSTREAMER
		ret = &esvg_video_provider_gstreamer_descriptor;
#endif

	}
	return ret;
}

void esvg_svg_element_get(Ender_Element *e, const char *uri, Ender_Element **el)
{
	Esvg_Svg *thiz;
	Esvg_Svg_Uri_Data data;
	Edom_Tag *t;

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

void esvg_svg_image_load(Ender_Element *e, const char *uri, Enesim_Surface **s, double width, double height)
{
	Esvg_Svg *thiz;
	Esvg_Svg_Uri_Image_Data data;
	Edom_Tag *t;

	if (!s) return;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);

	data.thiz = thiz;
	data.s = s;
	data.width = width;
	data.height = height;

	/* check if the uri starts with image:data (embedded image) first */
	if (!strncmp(uri, "data:image", 10))
	{
		Emage_Data *edata_buffer;
		Emage_Data *edata_base64;
		Eina_Bool ret;
		char mime[PATH_MAX];
		char format[7];
		char *tmp;

		/* first the mime */
		uri = uri + 5;
		tmp = strchr(uri, ';');
		if (!tmp) return;
		strncpy(mime, uri, tmp - uri);
		mime[tmp - uri] = '\0';
		uri = tmp + 1;

		/* the base */
		if (strncmp(uri, "base64", 6))
			return;
		uri += 7;

		edata_buffer = emage_data_buffer_new((char *)uri, strlen(uri));
		edata_base64 = emage_data_base64_new(edata_buffer);
		if (!emage_load(edata_base64, mime, s, ENESIM_FORMAT_ARGB8888, NULL, NULL))
		{
			Eina_Error err;

			err = eina_error_get();
			ERR("Embedded Image with mime '%s' failed to load with error: %s", mime, eina_error_msg_get(err));
		}
		emage_data_free(edata_base64);
	}
	else
	{
		/* resolve the uri for relative/absolute */
		esvg_iri_string_from(uri, &_uri_image_descriptor, &data);
	}
}

Enesim_Surface * esvg_svg_surface_new(Ender_Element *e, int w, int h)
{
	Enesim_Surface *s;

	/* TODO here use the desired pool */
	s = enesim_surface_new(ENESIM_FORMAT_ARGB8888, w, h);
	return s;
}

/*----------------------------------------------------------------------------*
 *                        The application interface                           *
 *----------------------------------------------------------------------------*/
void esvg_svg_go_to(Ender_Element *e, const char *uri)
{
	Edom_Tag *t;
	Esvg_Svg *thiz;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);
	if (!thiz->application_descriptor)
		return;
	if (thiz->application_descriptor->go_to)
		thiz->application_descriptor->go_to(thiz->thiz_e, thiz->application_data, uri);
}

#if 0
Esvg_Svg_External * esvg_svg_external_new(Ender_Element *svg, Edom_Tag *t, Enesim_Surface *s)
{
	Esvg_Svg_External *external;

	external = calloc(1, sizeof(Esvg_Svg_External));
	external->svg = svg;
	external->owner = t;
	external->s = s;
}

void esvg_svg_external_add(Ender_Element *e, Esvg_Svg_External *external)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);
	thiz->externals = eina_list_append(thiz->externals, external);
}

void esvg_svg_external_remove(Ender_Element *e, Esvg_Svg_External *external)
{
	thiz->externals = eina_list_remove(thiz->externals, external);
}

void esvg_svg_external_free(Esvg_Svg_External *external)
{
	free(external);
}
#endif

Ender_Element * esvg_svg_svg_load(Ender_Element *e, const char *uri)
{
	Ender_Element *svg;
	char *final;

	final = esvg_svg_uri_resolve(e, uri);
	if (!final) return NULL;
	/* TODO use the correct descriptor */
	svg = esvg_parser_load(final);
	free(final);

	return svg;
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

void esvg_svg_internal_container_width_get(Edom_Tag *t, double *container_width)
{
	Esvg_Svg *thiz;

	if (!container_width)
		return;
	thiz = _esvg_svg_get(t);
	*container_width = thiz->container_width;
}

void esvg_svg_internal_container_height_get(Edom_Tag *t, double *container_height)
{
	Esvg_Svg *thiz;

	if (!container_height)
		return;
	thiz = _esvg_svg_get(t);
	*container_height = thiz->container_height;
}


Etch * esvg_svg_etch_get(Ender_Element *e)
{
	Esvg_Svg *thiz;
	Edom_Tag *t;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);
	return thiz->etch;
}

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
	return ESVG_ELEMENT_NEW("svg");
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Element * esvg_svg_element_find(Ender_Element *e, const char *id)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	return _esvg_svg_element_get_by_id(t, id);
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
EAPI void esvg_svg_viewbox_set(Ender_Element *e, Esvg_Rect *v)
{
	ender_element_property_value_set(e, ESVG_SVG_VIEWBOX, v, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_content_script_type_set(Ender_Element *e, const char *v)
{
	ender_element_property_value_set(e, ESVG_SVG_CONTENT_SCRIPT_TYPE, v, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_content_script_type_get(Ender_Element *e, const char **v)
{
	Edom_Tag *t;
	t = ender_element_object_get(e);
	_esvg_svg_content_script_type_get(t, v);
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
EAPI void esvg_svg_animations_fps_set(Ender_Element *e, int fps)
{
	Edom_Tag *t;
	Esvg_Svg *thiz;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);
	etch_timer_fps_set(thiz->etch, fps);
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

	time = ESVG_CLOCK_AS_SECONDS(et);
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
EAPI void esvg_svg_script_alert(Ender_Element *e, const char *msg)
{
	Edom_Tag *t;
	Esvg_Svg *thiz;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);
	ERR("script alert msg '%s'", msg);
	if (!thiz->application_descriptor)
		return;
	if (thiz->application_descriptor->script_alert)
		thiz->application_descriptor->script_alert(thiz->thiz_e, thiz->application_data, msg);

}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_feed_mouse_move(Ender_Element *e, int x, int y)
{
	Edom_Tag *t;
	Esvg_Svg *thiz;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);
	esvg_input_feed_mouse_move(thiz->input, x, y);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_feed_mouse_down(Ender_Element *e, int button)
{
	Edom_Tag *t;
	Esvg_Svg *thiz;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);
	esvg_input_feed_mouse_down(thiz->input, button);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_feed_mouse_up(Ender_Element *e, int button)
{
	Edom_Tag *t;
	Esvg_Svg *thiz;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);
	esvg_input_feed_mouse_up(thiz->input, button);
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
EAPI Eina_Bool esvg_svg_setup(Ender_Element *e, Enesim_Error **error)
{
	Esvg_Context context;
	Edom_Tag *t;

	t = ender_element_object_get(e);
	esvg_context_init(&context);

	if (esvg_element_internal_setup(t, &context, error) == ESVG_SETUP_FAILED)
	{
		/* clean the context */
		esvg_context_shutdown(&context);
		return EINA_FALSE;
	}
	esvg_context_setup_dequeue(&context);
	return EINA_TRUE;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_container_width_set(Ender_Element *e, double container_width)
{
	ender_element_property_value_set(e, ESVG_SVG_CONTAINER_WIDTH, container_width, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_container_width_get(Ender_Element *e, double *container_width)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_container_height_set(Ender_Element *e, double container_height)
{
	ender_element_property_value_set(e, ESVG_SVG_CONTAINER_HEIGHT, container_height, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_container_height_get(Ender_Element *e, double *container_height)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_x_dpi_set(Ender_Element *e, double x_dpi)
{
	ender_element_property_value_set(e, ESVG_SVG_X_DPI, x_dpi, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_x_dpi_get(Ender_Element *e, double *x_dpi)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_y_dpi_set(Ender_Element *e, double y_dpi)
{
	ender_element_property_value_set(e, ESVG_SVG_Y_DPI, y_dpi, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_y_dpi_get(Ender_Element *e, double *y_dpi)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool esvg_svg_draw(Ender_Element *e, Enesim_Surface *s,
		Eina_Rectangle *clip, int x, int y, Enesim_Error **error)
{
	Edom_Tag *t;
	Enesim_Renderer *r = NULL;

	t = ender_element_object_get(e);
	/* TODO first render every svg image into its own surface */
	esvg_renderable_internal_renderer_get(t, &r);
	if (!r) return EINA_FALSE;

	return enesim_renderer_draw(r, s, clip, x, y, error);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool esvg_svg_draw_list(Ender_Element *e, Enesim_Surface *s,
		Eina_List *clips, int x, int y, Enesim_Error **error)
{
	Edom_Tag *t;
	Enesim_Renderer *r = NULL;

	t = ender_element_object_get(e);
	/* TODO first render every svg image we have but first interesecting
	 * the damage with the surface area (transformed)
	 */
	esvg_renderable_internal_renderer_get(t, &r);
	if (!r) return EINA_FALSE;

	return enesim_renderer_draw_list(r, s, clips, x, y, error);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_damages_get(Ender_Element *e, Esvg_Svg_Damage_Cb cb, void *data)
{
	Edom_Tag *t;
	Esvg_Svg *thiz;
	Enesim_Renderer *r;
	Eina_Iterator *iter;
	Eina_Rectangle *rect;
	int cw;
	int ch;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);

	cw = ceil(thiz->container_width);
	ch = ceil(thiz->container_height);

	if (!thiz->tiler || thiz->tw != cw || thiz->th != ch)
	{
		Eina_Rectangle full;

		if (thiz->tiler)
			eina_tiler_free(thiz->tiler);
		thiz->tiler = eina_tiler_new(cw, ch);
		thiz->tw = cw;
		thiz->th = ch;

		eina_rectangle_coords_from(&full, 0, 0, cw, ch);
		cb(e, &full, data);
		return;
	}

	esvg_renderable_internal_renderer_get(t, &r);
	/* TODO first generate the damages on every svg image we have */
	enesim_renderer_damages_get(r, _esvg_svg_damage_cb, thiz->tiler);

	iter = eina_tiler_iterator_new(thiz->tiler);
	EINA_ITERATOR_FOREACH(iter, rect)
	{
		cb(e, rect, data);
	}
	eina_iterator_free(iter);
	eina_tiler_clear(thiz->tiler);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_svg_application_descriptor_set(Ender_Element *e, const Esvg_Svg_Application_Descriptor *descriptor, void *data)
{
	Esvg_Svg *thiz;
	Edom_Tag *t;

	t = ender_element_object_get(e);
	thiz = _esvg_svg_get(t);
	thiz->application_descriptor = descriptor;
	thiz->application_data = data;
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
	return _esvg_svg_base_dir_get(thiz);
}
