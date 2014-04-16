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
#include "egueb_svg_main_private.h"
#include "egueb_svg_main.h"
#include "egueb_svg_document.h"
#include "egueb_svg_element.h"
#include "egueb_svg_rect.h"
#include "egueb_svg_length.h"
#include "egueb_svg_overflow.h"
#include "egueb_svg_point.h"
#include "egueb_svg_element_svg.h"
#include "egueb_svg_element_use.h"
#include "egueb_svg_renderable_container.h"
#include "egueb_svg_shape.h"

#include "egueb_svg_element_svg_private.h"
#include "egueb_svg_shape_private.h"
#include "egueb_svg_document_private.h"
#include "egueb_svg_attr_rect_private.h"
#include "egueb_svg_attr_length_private.h"
#include "egueb_svg_event_request_painter_private.h"
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
#define EGUEB_SVG_ELEMENT_SVG_DESCRIPTOR egueb_svg_element_svg_descriptor_get()
#define EGUEB_SVG_ELEMENT_SVG_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Element_Svg_Class, EGUEB_SVG_ELEMENT_SVG_DESCRIPTOR)
#define EGUEB_SVG_ELEMENT_SVG(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Element_Svg, EGUEB_SVG_ELEMENT_SVG_DESCRIPTOR)

typedef struct _Egueb_Svg_Element_Svg
{
	Egueb_Svg_Renderable_Container base;
	/* properties */
	Egueb_Dom_Node *x;
	Egueb_Dom_Node *y;
	Egueb_Dom_Node *width;
	Egueb_Dom_Node *height;
	Egueb_Dom_Node *viewbox;
	/* not xml attributes */
	double current_scale;
	Egueb_Svg_Point current_translate;

	/* private */
	Enesim_Renderer *rectangle;
	Enesim_Renderer *proxy;

	/* animation */
	Etch *etch;
	Eina_Bool paused;

	/* painter */
	Egueb_Svg_Painter *painter;

	/* some state flags */
	/* keep track if the renderable tree has changed, includeing the <a> tag */
#if 0
	Eina_Bool renderable_tree_changed : 1;

	double version;
	double x_dpi;
	double y_dpi;
	char *content_script_type;
	/* user provded properties */
	double base_font_size;
	/* private */
	Eina_Bool relative_size : 1; /* to know if the width/height are relative */
	Eina_List *styles; /* the list of styles found on this svg scope */
	Eina_Bool styles_changed : 1;
	Eina_List *elements_changed;
	Enesim_Renderer *clipper;
	Eina_Hash *ids; /* the ids found */
	/* user provided callbacks */
	const Egueb_Svg_Element_Svg_Application_Descriptor *application_descriptor;
	void *application_data;
	/* input */
	Egueb_Svg_Renderable_Container *container;
	Egueb_Svg_Input *input;
	/* svg inclusion */
	Eina_List *svgs; /* the list of svg documents found on the svg */
	Eina_List *image_svgs; /* the list of svg images on the svg */
	/* scripts */
	Eina_Hash *scriptors;
#endif
} Egueb_Svg_Element_Svg;

typedef struct _Egueb_Svg_Element_Svg_Class
{
	Egueb_Svg_Renderable_Container_Class base;
} Egueb_Svg_Element_Svg_Class;


static void _egueb_svg_element_svg_event_request_painter_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Dom_Node *n;
	Egueb_Svg_Painter *painter;

	n = egueb_dom_event_target_get(e);
	DBG_ELEMENT(n, "Setting the generic painter on the renderable");
	painter = egueb_svg_renderable_class_painter_get(n);
	egueb_svg_event_request_painter_painter_set(e, painter);
	egueb_dom_node_unref(n);
}

#if 0
/* TODO add a function to unset the painter */
static void _egueb_svg_element_svg_set_generic_painter(Egueb_Dom_Node *n)
{
	if (egueb_svg_is_renderable(n))
	{
		Egueb_Svg_Painter *painter;

		painter = egueb_svg_renderable_painter_get(n);
		if (painter)
		{
			DBG("Renderable already has a painter nothing do");
			egueb_svg_painter_unref(painter);
			return;
		}

		DBG("Setting the generic painter on the shape");
		painter = egueb_svg_renderable_class_painter_get(n);
		egueb_svg_renderable_painter_set(n, painter);

		if (egueb_svg_is_renderable_container(n))
		{
			Egueb_Dom_Node *child;

			/* iterate over the shapes to set the painter too */
			child = egueb_dom_node_child_first_get(n);
			while (child)
			{
				Egueb_Dom_Node *tmp;

				_egueb_svg_element_svg_set_generic_painter(child);
				tmp = egueb_dom_node_sibling_next_get(child);
				egueb_dom_node_unref(child);
				child = tmp;
			}
		}
	}
}

/* whenever a child is added which can have a painter, check if it is set
 * if not, define the generic painter for it. Not that a node inserted
 * does not trigger the event for each of the children of the inserted
 * tree, so we need to iterate there
 */
static void _egueb_svg_element_svg_node_inserted_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Dom_Node *target = NULL;

	target = egueb_dom_event_target_get(e);
	_egueb_svg_element_svg_set_generic_painter(target);
	egueb_dom_node_unref(target);
}
#endif

static void _egueb_svg_element_svg_etch_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Svg_Element_Svg *thiz = data;

	INFO("Requesting etch");
	egueb_smil_event_etch_set(e, thiz->etch);
}
/*----------------------------------------------------------------------------*
 *                            Renderable interface                            *
 *----------------------------------------------------------------------------*/
static void _egueb_svg_element_svg_painter_apply(Egueb_Svg_Renderable *r,
		Egueb_Svg_Painter *painter)
{
	Enesim_Renderer *compound;
	Eina_Bool visibility;

	compound = egueb_svg_renderable_container_renderer_get(r);
	egueb_svg_painter_visibility_get(painter, &visibility);
	enesim_renderer_visibility_set(compound, visibility);
	enesim_renderer_unref(compound);
}

static Egueb_Svg_Painter * _egueb_svg_element_svg_painter_get(Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = EGUEB_SVG_ELEMENT_SVG(r);
	return egueb_svg_painter_ref(thiz->painter);
}

static Enesim_Renderer * _egueb_svg_element_svg_renderer_get(
		Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = EGUEB_SVG_ELEMENT_SVG(r);
	return enesim_renderer_ref(thiz->proxy);
}

static Eina_Bool _egueb_svg_element_svg_process(Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Element_Svg *thiz;
	Egueb_Svg_Element *e;
	Egueb_Svg_Rect viewbox;
	Egueb_Svg_Length w, h, x, y;
	Egueb_Svg_Overflow overflow;
	Egueb_Dom_Node *svg_doc;
	Egueb_Dom_Node *relative = NULL;
	Enesim_Matrix relative_transform;
	double gw, gh, gx, gy;
	double relative_width, relative_height, relative_x, relative_y;
	double font_size;

	/* TODO First apply the local styles */

	thiz = EGUEB_SVG_ELEMENT_SVG(r);
	/* check if we are the topmost SVG, if so, use the document
	 * container size to generate the children viewbox (i.e our
	 * own bounds
	 */
	svg_doc = egueb_dom_node_document_get(EGUEB_DOM_NODE(r));
	relative = egueb_svg_element_geometry_relative_get(EGUEB_DOM_NODE(r));
	if (!relative)
	{
		Enesim_Matrix m;

		egueb_svg_document_width_get(svg_doc, &relative_width);
		egueb_svg_document_height_get(svg_doc, &relative_height);
		relative_x = 0;
		relative_y = 0;

		/* Set our current scale/translate */
		enesim_matrix_scale(&m, thiz->current_scale, thiz->current_scale);
		enesim_matrix_translate(&relative_transform, thiz->current_translate.x, thiz->current_translate.y);
		enesim_matrix_compose(&relative_transform, &m, &relative_transform);
	}
	else
	{
		Egueb_Svg_Element *svg_parent;

		svg_parent = EGUEB_SVG_ELEMENT(relative);
		relative_width = svg_parent->viewbox.w;
		relative_height = svg_parent->viewbox.h;
		relative_x = svg_parent->viewbox.x;
		relative_y = svg_parent->viewbox.y;
		relative_transform = svg_parent->transform;
		egueb_dom_node_unref(relative);
	}
	/* get the font size */
	font_size = egueb_svg_document_font_size_get(svg_doc);
	egueb_dom_node_unref(svg_doc);

	/* caluclate the new bounds */
	egueb_dom_attr_final_get(thiz->width, &w);
	egueb_dom_attr_final_get(thiz->height, &h);
	egueb_dom_attr_final_get(thiz->x, &x);
	egueb_dom_attr_final_get(thiz->y, &y);

	gw = egueb_svg_coord_final_get(&w, relative_width, font_size);
	gh = egueb_svg_coord_final_get(&h, relative_height, font_size);
	gx = egueb_svg_coord_final_get(&x, relative_x, font_size);
	gy = egueb_svg_coord_final_get(&y, relative_y, font_size);

	enesim_renderer_rectangle_position_set(thiz->rectangle, gx, gy);
	enesim_renderer_rectangle_size_set(thiz->rectangle, gw, gh);
	enesim_renderer_transformation_set(thiz->rectangle, &relative_transform);

	DBG_ELEMENT(EGUEB_DOM_NODE(r), "x: %g, y: %g, width: %g, height: %g", gx, gy, gw, gh);
	e = EGUEB_SVG_ELEMENT(r);
	/* the viewbox will set a new user space coordinate */
	/* FIXME check zeros */
	if (egueb_dom_attr_final_get(thiz->viewbox, &viewbox))
	{
		Enesim_Matrix scale;
		double new_vw;
		double new_vh;

		DBG_ELEMENT(EGUEB_DOM_NODE(r), "viewBox available '%g %g %g %g'", viewbox.x,
				viewbox.y, viewbox.w, viewbox.h);
		new_vw = viewbox.w / gw;
		new_vh = viewbox.h / gh;

		gw = viewbox.w;
		gh = viewbox.h;

		enesim_matrix_scale(&scale, 1.0/new_vw, 1.0/new_vh);
		enesim_matrix_compose(&scale, &relative_transform, &e->transform);
	}
	else
	{
		DBG_ELEMENT(EGUEB_DOM_NODE(r), "Not using any viewBox");
		e->transform = relative_transform;
	}
	/* set the new viewbox */
	e->viewbox.w = gw;
	e->viewbox.h = gh;

	/* decide what renderer to use based on the overflow attribute */
	egueb_dom_attr_final_get(e->overflow, &overflow);
	if (overflow == EGUEB_SVG_OVERFLOW_VISIBLE)
	{
		Enesim_Renderer *compound;

		compound = egueb_svg_renderable_container_renderer_get(
				EGUEB_SVG_RENDERABLE(thiz));
		enesim_renderer_proxy_proxied_set(thiz->proxy, compound);
	}
	else
	{
		enesim_renderer_proxy_proxied_set(thiz->proxy, enesim_renderer_ref(thiz->rectangle));
	}

	if (!egueb_svg_renderable_container_process(r))
		return EINA_FALSE;
	return EINA_TRUE;
}

/*----------------------------------------------------------------------------*
 *                              Element interface                             *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_String * _egueb_svg_element_svg_tag_name_get(
		Egueb_Dom_Element *e)
{
	return egueb_dom_string_ref(EGUEB_SVG_NAME_SVG);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_RENDERABLE_CONTAINER_DESCRIPTOR,
		Egueb_Svg_Element_Svg, Egueb_Svg_Element_Svg_Class,
		egueb_svg_element_svg);

static void _egueb_svg_element_svg_class_init(void *k)
{
	Egueb_Svg_Renderable_Class *klass;
	Egueb_Dom_Element_Class *e_klass;

	klass = EGUEB_SVG_RENDERABLE_CLASS(k);
	klass->renderer_get = _egueb_svg_element_svg_renderer_get;
	klass->process = _egueb_svg_element_svg_process;
	klass->painter_get = _egueb_svg_element_svg_painter_get;
	klass->painter_apply = _egueb_svg_element_svg_painter_apply;

	e_klass= EGUEB_DOM_ELEMENT_CLASS(k);
	e_klass->tag_name_get = _egueb_svg_element_svg_tag_name_get;
}

static void _egueb_svg_element_svg_instance_init(void *o)
{
	Egueb_Svg_Element_Svg *thiz;
	Egueb_Dom_Node *n;
	Enesim_Renderer *r;
	Enesim_Renderer *compound;

	thiz = EGUEB_SVG_ELEMENT_SVG(o);

	/* the properties */
	thiz->x = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_X),
			&EGUEB_SVG_LENGTH_0, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->y = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_Y),
			&EGUEB_SVG_LENGTH_0, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->width = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_WIDTH),
			&EGUEB_SVG_LENGTH_100_PERCENT,
			EINA_TRUE, EINA_FALSE, EINA_FALSE);
	thiz->height = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_HEIGHT),
			&EGUEB_SVG_LENGTH_100_PERCENT,
			EINA_TRUE, EINA_FALSE, EINA_FALSE);
	thiz->viewbox = egueb_svg_attr_rect_new(
			egueb_dom_string_ref(EGUEB_SVG_VIEWBOX),
			NULL);

	thiz->current_scale = 1.0;
	thiz->current_translate.x = 0;
	thiz->current_translate.y = 0;

	n = EGUEB_DOM_NODE(o);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->x), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->y), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->width), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->height), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->viewbox), NULL);

	/* add the event to set the painter */
#if 0
	egueb_dom_node_event_listener_add(n,
			EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED,
			_egueb_svg_element_svg_node_inserted_cb,
			EINA_FALSE, NULL);
#endif
	egueb_dom_node_event_listener_add(n,
			EGUEB_SMIL_EVENT_ETCH,
			_egueb_svg_element_svg_etch_cb,
			EINA_TRUE, thiz);
	egueb_dom_node_event_listener_add(n,
			EGUEB_SVG_EVENT_REQUEST_PAINTER,
			_egueb_svg_element_svg_event_request_painter_cb,
			EINA_FALSE, NULL);

	/* the rendering */
	r = enesim_renderer_proxy_new();
	thiz->proxy = r;

	compound = egueb_svg_renderable_container_renderer_get(
			EGUEB_SVG_RENDERABLE(thiz));
	r = enesim_renderer_rectangle_new();
	enesim_renderer_shape_fill_renderer_set(r, compound);
	enesim_renderer_shape_draw_mode_set(r, ENESIM_RENDERER_SHAPE_DRAW_MODE_FILL);
	enesim_renderer_rectangle_x_set(r, 0);
	enesim_renderer_rectangle_y_set(r, 0);
	thiz->rectangle = r;


	/* our own specific painter */
	thiz->painter = egueb_svg_painter_g_new();

	/* the animation system */
	thiz->etch = etch_new();
	etch_timer_fps_set(thiz->etch, 30);
}

static void _egueb_svg_element_svg_instance_deinit(void *o)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = EGUEB_SVG_ELEMENT_SVG(o);
	/* the properties */
	egueb_dom_node_unref(thiz->x);
	egueb_dom_node_unref(thiz->y);
	egueb_dom_node_unref(thiz->width);
	egueb_dom_node_unref(thiz->height);
	egueb_dom_node_unref(thiz->viewbox);
	/* the rendering */
	enesim_renderer_unref(thiz->rectangle);
	enesim_renderer_unref(thiz->proxy);
	/* the painter */
	egueb_svg_painter_unref(thiz->painter);
	/* remove etch and all the animation system  */
	etch_delete(thiz->etch);
	/* TODO free the scriptors */
}

#if 0
typedef struct _Egueb_Svg_Element_Svg_Uri_Data
{
	Egueb_Svg_Element_Svg *thiz;
	void *ret;
	void *data;
} Egueb_Svg_Element_Svg_Uri_Data;

typedef struct _Egueb_Svg_Element_Svg_Uri_Image_Data
{
	Egueb_Svg_Element_Svg *thiz;
	Enesim_Surface **s;
	double width;
	double height;
} Egueb_Svg_Element_Svg_Uri_Image_Data;

typedef struct _Egueb_Svg_Element_Svg_Image
{
	Ender_Element *svg;
	/* TODO the image tag that has a svg file set */
	Egueb_Dom_Tag *referal;
	Enesim_Surface *s;
} Egueb_Svg_Element_Svg_Image;

static Eina_Bool _egueb_svg_element_svg_child_initialize(Egueb_Dom_Tag *t, Egueb_Dom_Tag *child_t, void *data);
static Eina_Bool _egueb_svg_element_svg_child_deinitialize(Egueb_Dom_Tag *t, Egueb_Dom_Tag *child_t, void *data);

static Egueb_Svg_Element_Svg * _egueb_svg_element_svg_get(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element_Svg *thiz;

	if (egueb_svg_element_internal_type_get(t) != ESVG_TYPE_SVG)
		return NULL;
	thiz = egueb_svg_renderable_data_get(t);

	return thiz;
}

static const char * _egueb_svg_element_svg_base_dir_get(Egueb_Svg_Element_Svg *thiz)
{
	if (!thiz->application_descriptor)
		return NULL;
	if (thiz->application_descriptor->base_dir_get)
		return thiz->application_descriptor->base_dir_get(thiz->thiz_e, thiz->application_data);
	return NULL;
}

static Eina_Bool _egueb_svg_element_svg_relative_to_absolute(Egueb_Svg_Element_Svg *thiz, const char *relative, char *absolute, size_t len)
{
	const char *base_dir;
	size_t base_dir_len;

	base_dir = _egueb_svg_element_svg_base_dir_get(thiz);
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

static Eina_Bool _egueb_svg_element_svg_setup_pre(Egueb_Dom_Tag *t,
		Egueb_Dom_Tag *child,
		Egueb_Svg_Context *c,
		Enesim_Log **error,
		void *data)
{
	Egueb_Svg_Type type;
	Egueb_Svg_Element_Svg *thiz = data;

	type = egueb_svg_element_internal_type_get(child);
	if (egueb_svg_type_is_renderable(type) && (thiz->renderable_tree_changed || thiz->relative_size))
	{
		return EINA_TRUE;
	}
 	else if (type == ESVG_TYPE_A)
	{
		return EINA_TRUE;
	}
	else
	{
		return EINA_TRUE;
	}

}

static Eina_Bool _egueb_svg_element_svg_setup_post(Egueb_Dom_Tag *t,
		Egueb_Dom_Tag *child,
		Egueb_Svg_Context *c,
		Enesim_Log **error,
		void *data)
{
	Egueb_Svg_Type type;
	Egueb_Svg_Element_Svg *thiz = data;

	type = egueb_svg_element_internal_type_get(child);
	/* check if it is a renderable */
	if (egueb_svg_type_is_renderable(type) && thiz->renderable_tree_changed)
	{
		Enesim_Renderer *r = NULL;

		/* add to the compound */
		egueb_svg_renderable_internal_renderer_get(child, &r);
		enesim_renderer_compound_layer_add(thiz->compound, r);
		/* add it to the container */
		egueb_svg_renderable_container_renderable_add(thiz->container, child);
	}
 	else if (type == ESVG_TYPE_A)
	{
		return egueb_svg_element_internal_child_setup(child, c, error,
				_egueb_svg_element_svg_setup_pre, _egueb_svg_element_svg_setup_post, thiz);
	}
	return EINA_TRUE;
}

static inline void _egueb_svg_element_svg_size_apply(Egueb_Svg_Element_Svg *thiz, Egueb_Svg_Element_Context *ctx)
{
	double width;
	double height;

	width = egueb_svg_coord_final_get(&thiz->width, ctx->viewbox.w, ctx->font_size);
	height = egueb_svg_coord_final_get(&thiz->height, ctx->viewbox.h, ctx->font_size);
	enesim_renderer_clipper_width_set(thiz->clipper, width);
	enesim_renderer_clipper_height_set(thiz->clipper, height);
	ctx->viewbox.w = width;
	ctx->viewbox.h = height;
}

static inline void _egueb_svg_element_svg_viewbox_apply(Egueb_Svg_Element_Svg *thiz, Egueb_Svg_Element_Context *ctx)
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
static void _egueb_svg_element_svg_element_changed_setup(Egueb_Svg_Element_Svg *thiz,
		Egueb_Svg_Context *c,
		Enesim_Log **error)
{
	Ender_Element *e;
	Eina_List *l, *l_next;

	EINA_LIST_FOREACH_SAFE(thiz->elements_changed, l, l_next, e)
	{
		Egueb_Dom_Tag *changed_t;

		changed_t = ender_element_object_get(e);
		DBG("Tag '%s' found on the changed list", egueb_dom_tag_name_get(changed_t));
		if (!egueb_svg_element_has_setup(changed_t, c))
		{
			DBG("Tag '%s' marked as changed is going to be setup", egueb_dom_tag_name_get(changed_t));
			egueb_svg_element_internal_setup(changed_t, c, error);
		}
		thiz->elements_changed = eina_list_remove_list(thiz->elements_changed, l);
	}
}

static void _egueb_svg_element_svg_element_changed_add(Egueb_Svg_Element_Svg *thiz, Ender_Element *e)
{
	DBG("Tag '%s' has been added to the list of changes", egueb_svg_element_name_get(e));
	thiz->elements_changed = eina_list_append(thiz->elements_changed, e);
}

static void _egueb_svg_element_svg_element_changed_remove(Egueb_Svg_Element_Svg *thiz, Ender_Element *e)
{
	DBG("Tag '%s' has been removed from the list of changes", egueb_svg_element_name_get(e));

}
/*----------------------------------------------------------------------------*
 *                       The URI interface for uris                           *
 *----------------------------------------------------------------------------*/
static void _egueb_svg_element_svg_resolve_uri_local_get(const char *name,
		const char *fragment, void *user_data)
{
	Egueb_Svg_Element_Svg_Uri_Data *data = user_data;
	Egueb_Svg_Element_Svg *thiz = data->thiz;
	char **real = data->ret;

	printf("TODO looking for %s\n", fragment);
	/* TODO we should concat the svg file + the fragment */
}

static void _egueb_svg_element_svg_resolve_uri_absolute_get(const char *name,
		const char *fragment, void *user_data)
{
	Egueb_Svg_Element_Svg_Uri_Data *data = user_data;
	Egueb_Svg_Element_Svg *thiz = data->thiz;
	char **real = data->ret;
	size_t len;

	*real = strdup(name);
	/* FIXME what about the fragment? */
}

static void _egueb_svg_element_svg_resolve_uri_relative_get(const char *name,
		const char *fragment, void *user_data)
{
	Egueb_Svg_Element_Svg_Uri_Data *data = user_data;
	Egueb_Svg_Element_Svg *thiz = data->thiz;
	char **real = data->ret;
	char absolute[PATH_MAX];

	if (!_egueb_svg_element_svg_relative_to_absolute(thiz, name, absolute, PATH_MAX))
		return;
	*real = strdup(absolute);
	/* FIXME what about the fragment? */
}

static Egueb_Svg_Uri_Descriptor _uri_resolve_descriptor = {
	/* .local_get 		= */ _egueb_svg_element_svg_resolve_uri_local_get,
	/* .absolute_get 	= */ _egueb_svg_element_svg_resolve_uri_absolute_get,
	/* .relative_get 	= */ _egueb_svg_element_svg_resolve_uri_relative_get,
};
/*----------------------------------------------------------------------------*
 *                      The URI interface for images                          *
 *----------------------------------------------------------------------------*/
static void _egueb_svg_element_svg_image_uri_local_get(const char *name,
		const char *fragment, void *user_data)
{
	printf("TODO fetching a local image?\n");
}

static void _egueb_svg_element_svg_image_uri_absolute_get(const char *name,
		const char *fragment, void *user_data)
{
	Egueb_Svg_Element_Svg_Uri_Image_Data *data = user_data;
	Egueb_Svg_Element_Svg *thiz = data->thiz;
	Enesim_Surface **s = data->s;
	double width = data->width;
	double height = data->height;
	Eina_Bool ret;

	{
		char options[PATH_MAX];

		options[0] = '\0';
		if (width != 0 && height != 0)
			sprintf(options, "width=%d;height=%d", (int)width, (int)height);

		ret = enesim_image_file_load(name, s, ENESIM_FORMAT_ARGB8888, NULL, options);
		if (!ret)
		{
			Eina_Error err;

			err = eina_error_get();
			ERR("Image '%s' failed to load with error: %s", name, eina_error_msg_get(err));
			return;
		}
	}
}

static void _egueb_svg_element_svg_image_uri_relative_get(const char *name,
		const char *fragment, void *user_data)
{
	Egueb_Svg_Element_Svg_Uri_Image_Data *data = user_data;
	Egueb_Svg_Element_Svg *thiz = data->thiz;
	char absolute[PATH_MAX];
	size_t len;

	if (!_egueb_svg_element_svg_relative_to_absolute(thiz, name, absolute, PATH_MAX))
		return;
	/* call the absolute one */
	_egueb_svg_element_svg_image_uri_absolute_get(absolute, fragment, user_data);
}

static Egueb_Svg_Uri_Descriptor _uri_image_descriptor = {
	/* .local_get 		= */ _egueb_svg_element_svg_image_uri_local_get,
	/* .absolute_get 	= */ _egueb_svg_element_svg_image_uri_absolute_get,
	/* .relative_get 	= */ _egueb_svg_element_svg_image_uri_relative_get,
};

static Eina_Bool _egueb_svg_element_svg_child_topmost_set(Egueb_Dom_Tag *t, Egueb_Dom_Tag *child,
		void *data)
{
	Ender_Element *topmost = data;

	egueb_svg_element_topmost_set(child, topmost);
	return EINA_TRUE;
}

/* FIXME the ender events just trigger once the id has changed so we dont know the old one */
static void _egueb_svg_element_svg_child_id_cb(Ender_Element *e, const char *event_name, void *event_data, void *data)
{
	Egueb_Svg_Element_Svg *thiz = data;
	Ender_Event_Mutation_Property *ev = event_data;
	const char *id;

	id = ender_value_string_get(ev->value);
	eina_hash_del_by_data(thiz->ids, e);
	if (id) eina_hash_add(thiz->ids, id, e);
}

static void _egueb_svg_element_svg_child_mutation_child_cb(Ender_Element *e, const char *event_name, void *event_data, void *data)
{
	Ender_Event_Mutation_Property *ev = event_data;
	Ender_Element *thiz_e = data;
	Egueb_Dom_Tag *child_child;

	child_child = ender_value_object_get(ev->value);
	switch (ev->type)
	{
		/* some child has been added to the whole svg tree */
		case ENDER_EVENT_MUTATION_ADD:
		/* set the topmost */
		egueb_svg_element_topmost_set(child_child, thiz_e);
		break;

		/* some child has been removed from the whole svg tree */
		case ENDER_EVENT_MUTATION_REMOVE:
		egueb_svg_element_topmost_set(child_child, NULL);
		break;

		default:
		printf("TODO\n");
		break;
	}
}

/* called whenever the svg has been set as a parent of another
 * element
 */
static void _egueb_svg_element_svg_topmost_changed_cb(Ender_Element *e, const char *event_name,
		void *event_data, void *data)
{
	Egueb_Svg_Element_Event_Topmost_Changed *ev = event_data;
	Egueb_Dom_Tag *child_t;
	Ender_Element *child_e;

	child_t = ev->child;
	child_e = egueb_svg_element_ender_get(child_t);
	if (ev->previous == e)
	{
		Egueb_Svg_Element_Svg *thiz;
		Egueb_Dom_Tag *topmost_p;

		topmost_p = ender_element_object_get(ev->previous);
		thiz = _egueb_svg_element_svg_get(topmost_p);
		/* remove previous callbacks on the element */
		_egueb_svg_element_svg_child_deinitialize(topmost_p, child_t, thiz);
	}
	if (ev->current == e)
	{
		Egueb_Svg_Element_Svg *thiz;
		Egueb_Dom_Tag *topmost_c;

		DBG("Setting topmost on %s", egueb_svg_type_string_to(egueb_svg_element_internal_type_get(child_t)));
		topmost_c = ender_element_object_get(ev->current);
		thiz = _egueb_svg_element_svg_get(topmost_c);
		/* setup all the needed callbacks on the element */
		_egueb_svg_element_svg_child_initialize(topmost_c, child_t, thiz);
	}
}

static void _egueb_svg_element_svg_child_mutation_cb(Ender_Element *e, const char *event_name, void *event_data, void *data)
{
	Egueb_Svg_Element_Svg *thiz = data;

	/* add the element to the list of changed elements */
	/* TODO if the property is the child, dont do nothing */
	_egueb_svg_element_svg_element_changed_add(thiz, e);
}

static Eina_Bool _egueb_svg_element_svg_child_initialize(Egueb_Dom_Tag *t, Egueb_Dom_Tag *child_t, void *data)
{
	Egueb_Svg_Element_Svg *thiz;
	Ender_Element *child_e;
	const char *id;

 	thiz = data;

	DBG("Initializing '%s'", egueb_svg_type_string_to(egueb_svg_element_internal_type_get(child_t)));
	/* add a callback whenever the child property has changed
	 * to initialize that child too */
	child_e = egueb_svg_element_ender_get(child_t);
	ender_event_listener_add(child_e, "Mutation:child", _egueb_svg_element_svg_child_mutation_child_cb, thiz->thiz_e);
	/* add a callback whenever any property has changed to add it the list
	 * of changed elements */
	ender_event_listener_add(child_e, "Mutation", _egueb_svg_element_svg_child_mutation_cb, thiz);
	/* add an event whenever the child changes the id */
	ender_event_listener_add(child_e, "Mutation:id", _egueb_svg_element_svg_child_id_cb, thiz);
	/* add the id to the hash of ids */
	egueb_svg_element_id_get(child_e, &id);
	if (id)
	{
		DBG("Adding '%s' to the list of ids", id);
		eina_hash_add(thiz->ids, id, child_e);
	}

	/* add the style to the list of styles */
	if (egueb_svg_element_style_is_internal(child_t))
	{
		thiz->styles = eina_list_append(thiz->styles, child_t);
		thiz->styles_changed = EINA_TRUE;
	}
	/* mark it for processing */
	_egueb_svg_element_svg_element_changed_add(thiz, child_e);
	/* set the topmost on every element */
	egueb_dom_tag_child_foreach(child_t, _egueb_svg_element_svg_child_topmost_set, thiz->thiz_e);

	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_element_svg_child_deinitialize(Egueb_Dom_Tag *t, Egueb_Dom_Tag *child_t, void *data)
{
	Egueb_Svg_Element_Svg *thiz;
	Ender_Element *child_e;
	const char *id;

 	thiz = data;

	/* add a callback whenever the child property has changed
	 * to initialize that child too */
	child_e = egueb_svg_element_ender_get(child_t);
	ender_event_listener_remove_full(child_e, "Mutation:child", _egueb_svg_element_svg_child_mutation_child_cb, thiz->thiz_e);
	/* add a callback whenever any property has changed to add it the list
	 * of changed elements */
	ender_event_listener_remove_full(child_e, "Mutation", _egueb_svg_element_svg_child_mutation_cb, thiz);
	/* add an event whenever the child changes the id */
	ender_event_listener_remove_full(child_e, "Mutation:id", _egueb_svg_element_svg_child_id_cb, thiz);
	/* add the id to the hash of ids */
	egueb_svg_element_id_get(child_e, &id);
	if (id) eina_hash_del(thiz->ids, id, child_e);

	/* add the style to the list of styles */
	if (egueb_svg_element_style_is_internal(child_t))
	{
		thiz->styles = eina_list_remove(thiz->styles, child_t);
		thiz->styles_changed = EINA_TRUE;
	}
	/* mark it for processing */
	_egueb_svg_element_svg_element_changed_remove(thiz, child_e);
	/* set the topmost on every element */
	egueb_dom_tag_child_foreach(child_t, _egueb_svg_element_svg_child_topmost_set, NULL);

	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                           The Esvg Input interface                         *
 *----------------------------------------------------------------------------*/
static Ender_Element * _egueb_svg_element_svg_element_at(void *data, int x, int y)
{
	Egueb_Dom_Tag *t = data;
	Egueb_Svg_Element_Svg *thiz;
	Eina_Rectangle in;
	Eina_Rectangle bounds;
	Ender_Element *e;

	thiz = _egueb_svg_element_svg_get(t);
	eina_rectangle_coords_from(&in, x, y, 1, 1);
	enesim_renderer_destination_bounds_get(thiz->clipper, &bounds, 0, 0);
	//printf("%s: %d %d - %d %d %d %d\n", egueb_dom_tag_name_get(t), x, y, bounds.x, bounds.y, bounds.w, bounds.h);
	if (!eina_rectangles_intersect(&bounds, &in))
		return NULL;

	e = egueb_svg_element_ender_get(t);
	return e;
}

static Egueb_Svg_Input_Descriptor _egueb_svg_element_svg_input_descriptor = {
	/* .element_at 	= */ _egueb_svg_element_svg_element_at,
};
/*----------------------------------------------------------------------------*
 *                       The Esvg Renderable interface                        *
 *----------------------------------------------------------------------------*/
static void _egueb_svg_element_svg_initialize(Ender_Element *e)
{
	Egueb_Svg_Element_Svg *thiz;
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	thiz = _egueb_svg_element_svg_get(t);

	thiz->thiz_e = e;
	thiz->container = egueb_svg_renderable_container_new(e);
	/* called whenever the topmost changes */
	ender_event_listener_add(e, "TopmostChanged", _egueb_svg_element_svg_topmost_changed_cb, NULL);
	/* own input system */
	thiz->input = egueb_svg_input_new(&_egueb_svg_element_svg_input_descriptor, t);
}

static Eina_Bool _egueb_svg_element_svg_attribute_set(Ender_Element *e, const char *key, const char *value)
{
	if (strcmp(key, "version") == 0)
	{
		Egueb_Svg_Number n;
		egueb_svg_number_string_from(&n, value, 0.0);
		egueb_svg_element_svg_version_set(e, n.value);
	}
	else if (strcmp(key, "x") == 0)
	{
		Egueb_Svg_Coord x;

		egueb_svg_length_string_from(&x, value);
		egueb_svg_element_svg_x_set(e, &x);
	}
	else if (strcmp(key, "y") == 0)
	{
		Egueb_Svg_Coord y;

		egueb_svg_length_string_from(&y, value);
		egueb_svg_element_svg_y_set(e, &y);
	}
	else if (strcmp(key, "width") == 0)
	{
		Egueb_Svg_Length width;

		egueb_svg_length_string_from(&width, value);
		egueb_svg_element_svg_width_set(e, &width);
	}
	else if (strcmp(key, "height") == 0)
	{
		Egueb_Svg_Length height;

		egueb_svg_length_string_from(&height, value);
		egueb_svg_element_svg_height_set(e, &height);
	}
	else if (strcmp(key, "viewBox") == 0)
	{
		Egueb_Svg_Rect viewbox;
		egueb_svg_rect_string_from(&viewbox, value);
		egueb_svg_element_svg_viewbox_set(e, &viewbox);
	}
	else if (strcmp(key, "contentScriptType") == 0)
	{
		egueb_svg_element_svg_content_script_type_set(e, value);
	}
	else
	{
		return EINA_FALSE;
	}

	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_element_svg_attribute_get(Egueb_Dom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static Eina_Bool _egueb_svg_element_svg_child_add(Egueb_Dom_Tag *t, Egueb_Dom_Tag *child)
{
	Egueb_Svg_Element_Svg *thiz;
	Egueb_Svg_Type type;
	Ender_Element *e;

	thiz = _egueb_svg_element_svg_get(t);
	type = egueb_svg_element_internal_type_get(child);
	if (egueb_svg_type_is_renderable(type) || type == ESVG_TYPE_A)
	{
		thiz->renderable_tree_changed = EINA_TRUE;
	}

	e = egueb_svg_element_ender_get(t);
	egueb_svg_element_topmost_set(child, e);

	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_element_svg_child_remove(Egueb_Dom_Tag *t, Egueb_Dom_Tag *child)
{
	Egueb_Svg_Element_Svg *thiz;
	Egueb_Svg_Type type;

	thiz = _egueb_svg_element_svg_get(t);
	type = egueb_svg_element_internal_type_get(child);
	if (egueb_svg_type_is_renderable(type) || type == ESVG_TYPE_A)
	{
		thiz->renderable_tree_changed = EINA_TRUE;
	}
	egueb_svg_element_topmost_set(child, NULL);

	return EINA_TRUE;
}

static Egueb_Svg_Element_Setup_Return _egueb_svg_element_svg_setup(Egueb_Dom_Tag *t,
		Egueb_Svg_Context *c,
		Egueb_Svg_Element_Context *ctx,
		Egueb_Svg_Attribute_Presentation *attr,
		Enesim_Log **error)
{
	Egueb_Svg_Element_Svg *thiz;
	Egueb_Dom_Tag *style;
	Enesim_Renderer *child;
	Enesim_Renderer *parent;
	Egueb_Svg_Element_Setup_Return ret;
	int changed;
	Eina_Bool full_process = EINA_FALSE;
	double width, height;

	thiz = _egueb_svg_element_svg_get(t);

	/* check if we have changed or some element has changed */
	changed = egueb_svg_element_changed(t);
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
		egueb_svg_renderable_container_clear(thiz->container);
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
		width = egueb_svg_coord_final_get(&thiz->width, ctx->viewbox.w, thiz->base_font_size);
		height = egueb_svg_coord_final_get(&thiz->height, ctx->viewbox.h, thiz->base_font_size);
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
				egueb_svg_element_style_apply(style, t);
			}
			thiz->styles_changed = EINA_FALSE;
		}
	}
	/* 2. if no attr or context changed, but some property that changes the context have
	 * changed (viewbox) then do a full tree process
	 */

	/* 3. ok no need to process the whole tree. check if we do need to add the renderables back */
	ret = egueb_svg_element_internal_child_setup(t, c, error,
		_egueb_svg_element_svg_setup_pre, _egueb_svg_element_svg_setup_post, thiz);
	thiz->renderable_tree_changed = EINA_FALSE;

	/* 4. now process the list of changed elements, it will automatically do the setup on its
	 * parent
	 */
	/* FIXME here we are also doing the setup on every referenceable child,
	 * for example a clipPath with a rect children, the rect and its childs will
	 * also do the setup, which is something not desirable
	 */
	_egueb_svg_element_svg_element_changed_setup(thiz, c, error);

	return ret;
}

static Egueb_Svg_Renderable_Descriptor _descriptor = {
	/* .child_add		= */ _egueb_svg_element_svg_child_add,
	/* .child_remove	= */ _egueb_svg_element_svg_child_remove,
	/* .attribute_get 	= */ _egueb_svg_element_svg_attribute_get,
	/* .cdata_set 		= */ NULL,
	/* .text_set 		= */ NULL,
	/* .text_get 		= */ NULL,
	/* .free 		= */ _egueb_svg_element_svg_free,
	/* .initialize 		= */ _egueb_svg_element_svg_initialize,
	/* .attribute_set 	= */ _egueb_svg_element_svg_attribute_set,
	/* .attribute_animated_set = */ NULL,
	/* .setup		= */ _egueb_svg_element_svg_setup,
	/* .renderer_get	= */ _egueb_svg_element_svg_renderer_get,
	/* .renderer_propagate	= */ NULL,
};

/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Tag * _egueb_svg_element_svg_new(void)
{
	Egueb_Svg_Element_Svg *thiz;
	Egueb_Dom_Tag *t;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Egueb_Svg_Element_Svg));
	if (!thiz) return NULL;


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

	/* the script system */
	thiz->scriptors = eina_hash_string_superfast_new(NULL);
	return t;
}

static Egueb_Dom_Tag * _egueb_svg_element_svg_element_find(Egueb_Dom_Tag *tag, const char *id)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = _egueb_svg_element_svg_get(tag);
	return eina_hash_find(thiz->ids, id);
}

static void _egueb_svg_element_svg_version_set(Egueb_Dom_Tag *t, double version)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = _egueb_svg_element_svg_get(t);
	thiz->version = version;
}

static void _egueb_svg_element_svg_version_get(Egueb_Dom_Tag *t, double *version)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = _egueb_svg_element_svg_get(t);
	if (version) *version = thiz->version;
}

static void _egueb_svg_element_svg_x_set(Egueb_Dom_Tag *t, Egueb_Svg_Coord *x)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = _egueb_svg_element_svg_get(t);
	thiz->x = *x;
}

static void _egueb_svg_element_svg_x_get(Egueb_Dom_Tag *t, Egueb_Svg_Coord *x)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = _egueb_svg_element_svg_get(t);
	if (x) *x = thiz->x;
}


static void _egueb_svg_element_svg_y_set(Egueb_Dom_Tag *t, Egueb_Svg_Coord *y)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = _egueb_svg_element_svg_get(t);
	thiz->y = *y;
}

static void _egueb_svg_element_svg_y_get(Egueb_Dom_Tag *t, Egueb_Svg_Coord *y)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = _egueb_svg_element_svg_get(t);
	if (y) *y = thiz->y;
}

static void _egueb_svg_element_svg_width_set(Egueb_Dom_Tag *t, Egueb_Svg_Length *width)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = _egueb_svg_element_svg_get(t);
	thiz->width = *width;
	if (egueb_svg_length_is_relative(width))
		thiz->relative_size = EINA_TRUE;
	else
		thiz->relative_size = EINA_FALSE;
}

static void _egueb_svg_element_svg_width_get(Egueb_Dom_Tag *t, Egueb_Svg_Length *width)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = _egueb_svg_element_svg_get(t);
	if (width) *width = thiz->width;
}

static void _egueb_svg_element_svg_height_set(Egueb_Dom_Tag *t, Egueb_Svg_Length *height)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = _egueb_svg_element_svg_get(t);
	thiz->height = *height;
	if (egueb_svg_length_is_relative(height))
		thiz->relative_size = EINA_TRUE;
	else
		thiz->relative_size = EINA_FALSE;
}

static void _egueb_svg_element_svg_height_get(Egueb_Dom_Tag *t, Egueb_Svg_Length *height)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = _egueb_svg_element_svg_get(t);
	if (height) *height = thiz->height;
}

static void _egueb_svg_element_svg_viewbox_set(Egueb_Dom_Tag *t, Egueb_Svg_Rect *vb)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = _egueb_svg_element_svg_get(t);
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

static void _egueb_svg_element_svg_x_dpi_set(Egueb_Dom_Tag *t, double x_dpi)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = _egueb_svg_element_svg_get(t);
	thiz->x_dpi = x_dpi;
}

static void _egueb_svg_element_svg_x_dpi_get(Egueb_Dom_Tag *t, double *x_dpi)
{
	Egueb_Svg_Element_Svg *thiz;

	if (!x_dpi)
		return;
	thiz = _egueb_svg_element_svg_get(t);
	*x_dpi = thiz->x_dpi;
}

static void _egueb_svg_element_svg_y_dpi_set(Egueb_Dom_Tag *t, double y_dpi)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = _egueb_svg_element_svg_get(t);
	thiz->y_dpi = y_dpi;
}

static void _egueb_svg_element_svg_y_dpi_get(Egueb_Dom_Tag *t, double *y_dpi)
{
	Egueb_Svg_Element_Svg *thiz;

	if (!y_dpi)
		return;
	thiz = _egueb_svg_element_svg_get(t);
	*y_dpi = thiz->y_dpi;
}

static void _egueb_svg_element_svg_content_script_type_set(Egueb_Dom_Tag *t, const char *v)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = _egueb_svg_element_svg_get(t);
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

static void _egueb_svg_element_svg_content_script_type_get(Egueb_Dom_Tag *t, const char **v)
{
	Egueb_Svg_Element_Svg *thiz;

	if (!v) return;
	thiz = _egueb_svg_element_svg_get(t);
	*v = thiz->content_script_type;
}

static Ender_Element *_egueb_svg_element_svg_element_get_by_id(Egueb_Dom_Tag *t, const char *id)
{
	Egueb_Svg_Element_Svg *thiz;
	Ender_Element *found;

	thiz = _egueb_svg_element_svg_get(t);
	found = eina_hash_find(thiz->ids, id);
	return found;
}

/* The ender wrapper */
#define _egueb_svg_element_svg_delete NULL
#define _egueb_svg_element_svg_x_is_set NULL
#define _egueb_svg_element_svg_y_is_set NULL
#define _egueb_svg_element_svg_width_is_set NULL
#define _egueb_svg_element_svg_height_is_set NULL
#define _egueb_svg_element_svg_viewbox_get NULL
#define _egueb_svg_element_svg_viewbox_is_set NULL
#define _egueb_svg_element_svg_x_dpi_is_set NULL
#define _egueb_svg_element_svg_y_dpi_is_set NULL
#define _egueb_svg_element_svg_content_script_type_is_set NULL
#include "egueb_svg_generated_element_svg.c"
void egueb_svg_element_svg_init(void)
{
	_egueb_svg_element_svg_log = eina_log_domain_register("egueb_svg_element_svg", ESVG_LOG_COLOR_DEFAULT);
	if (_egueb_svg_element_svg_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	/* FIXME: initialize the ender interface */
	_egueb_svg_element_svg_init();
}

void egueb_svg_element_svg_shutdown(void)
{
	if (_egueb_svg_element_svg_log < 0)
		return;
	_egueb_svg_element_svg_shutdown();
	eina_log_domain_unregister(_egueb_svg_element_svg_log);
	_egueb_svg_element_svg_log = -1;
}
/*----------------------------------------------------------------------------*
 *                      The script provider functions                         *
 *----------------------------------------------------------------------------*/
Egueb_Svg_Script_Provider * egueb_svg_element_svg_script_provider_get(Ender_Element *e, const char *type)
{
	Egueb_Svg_Element_Svg *thiz;
	Egueb_Svg_Script_Provider *scriptor;
	Egueb_Svg_Script_Provider_Descriptor *descriptor;
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	thiz = _egueb_svg_element_svg_get(t);

	/* return the created scriptor in case it exists on the svg */
	scriptor = eina_hash_find(thiz->scriptors, type);
	if (scriptor) return scriptor;

	descriptor = egueb_svg_script_provider_descriptor_find(type);
	if (!descriptor) return NULL;

	scriptor = egueb_svg_script_provider_new(descriptor, e);
	return scriptor;
}
/*----------------------------------------------------------------------------*
 *                       The video provider functions                         *
 *----------------------------------------------------------------------------*/
Egueb_Svg_Video_Provider_Descriptor * egueb_svg_element_svg_video_provider_descriptor_get(
		Ender_Element *e)
{
	Egueb_Svg_Element_Svg *thiz;
	const Egueb_Svg_Element_Svg_Application_Descriptor *descriptor;
	Egueb_Svg_Video_Provider_Descriptor *ret = NULL;
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	thiz = _egueb_svg_element_svg_get(t);

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
		ret = &egueb_svg_video_provider_gstreamer_descriptor;
#endif

	}
	return ret;
}

void egueb_svg_element_svg_image_load(Ender_Element *e, const char *uri, Enesim_Surface **s, double width, double height)
{
	Egueb_Svg_Element_Svg *thiz;
	Egueb_Svg_Element_Svg_Uri_Image_Data data;
	Egueb_Dom_Tag *t;

	if (!s) return;

	t = ender_element_object_get(e);
	thiz = _egueb_svg_element_svg_get(t);

	data.thiz = thiz;
	data.s = s;
	data.width = width;
	data.height = height;

	/* check if the uri starts with image:data (embedded image) first */
	if (!strncmp(uri, "data:image", 10))
	{
		Enesim_Stream *edata_buffer;
		Enesim_Stream *edata_base64;
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

		edata_buffer = enesim_stream_buffer_new((char *)uri, strlen(uri));
		edata_base64 = enesim_stream_base64_new(edata_buffer);
		if (!enesim_image_load(edata_base64, mime, s, ENESIM_FORMAT_ARGB8888, NULL, NULL))
		{
			Eina_Error err;

			err = eina_error_get();
			ERR("Embedded Image with mime '%s' failed to load with error: %s", mime, eina_error_msg_get(err));
		}
		enesim_stream_unref(edata_base64);
	}
	else
	{
		/* resolve the uri for relative/absolute */
		egueb_svg_iri_string_from(uri, &_uri_image_descriptor, &data);
	}
}

Enesim_Surface * egueb_svg_element_svg_surface_new(Ender_Element *e, int w, int h)
{
	Enesim_Surface *s;

	/* TODO here use the desired pool */
	s = enesim_surface_new(ENESIM_FORMAT_ARGB8888, w, h);
	return s;
}

/*----------------------------------------------------------------------------*
 *                        The application interface                           *
 *----------------------------------------------------------------------------*/
void egueb_svg_element_svg_go_to(Ender_Element *e, const char *uri)
{
	Egueb_Dom_Tag *t;
	Egueb_Svg_Element_Svg *thiz;

	t = ender_element_object_get(e);
	thiz = _egueb_svg_element_svg_get(t);
	if (!thiz->application_descriptor)
		return;
	if (thiz->application_descriptor->go_to)
		thiz->application_descriptor->go_to(thiz->thiz_e, thiz->application_data, uri);
}

#if 0
Egueb_Svg_Element_Svg_External * egueb_svg_element_svg_external_new(Ender_Element *svg, Egueb_Dom_Tag *t, Enesim_Surface *s)
{
	Egueb_Svg_Element_Svg_External *external;

	external = calloc(1, sizeof(Egueb_Svg_Element_Svg_External));
	external->svg = svg;
	external->owner = t;
	external->s = s;
}

void egueb_svg_element_svg_external_add(Ender_Element *e, Egueb_Svg_Element_Svg_External *external)
{
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	thiz = _egueb_svg_element_svg_get(t);
	thiz->externals = eina_list_append(thiz->externals, external);
}

void egueb_svg_element_svg_external_remove(Ender_Element *e, Egueb_Svg_Element_Svg_External *external)
{
	thiz->externals = eina_list_remove(thiz->externals, external);
}

void egueb_svg_element_svg_external_free(Egueb_Svg_Element_Svg_External *external)
{
	free(external);
}
#endif

Ender_Element * egueb_svg_element_svg_svg_load(Ender_Element *e, const char *uri)
{
	Ender_Element *svg;
	char *final;

	final = egueb_svg_element_svg_uri_resolve(e, uri);
	if (!final) return NULL;
	/* TODO use the correct descriptor */
	svg = egueb_svg_parser_load(final);
	free(final);

	return svg;
}

char * egueb_svg_element_svg_uri_resolve(Ender_Element *e, const char *uri)
{
	Egueb_Svg_Element_Svg_Uri_Data data;
	Egueb_Dom_Tag *t;
	Egueb_Svg_Element_Svg *thiz;
	char *ret = NULL;

	if (!uri) return NULL;

	t = ender_element_object_get(e);
	thiz = _egueb_svg_element_svg_get(t);

	data.thiz = thiz;
	data.ret = &ret;
	/* resolve the uri for relative/absolute */
	egueb_svg_iri_string_from(uri, &_uri_resolve_descriptor, &data);

	return ret;
}

#if 0
void egueb_svg_element_svg_style_add(Egueb_Dom_Tag *tag, Egueb_Svg_Parser_Style *s)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = _egueb_svg_element_svg_get(tag);
	thiz->styles = eina_list_append(thiz->styles, s);
}

void egueb_svg_element_svg_style_apply(Egueb_Dom_Tag *tag)
{
	Egueb_Svg_Element_Svg *thiz;
	Egueb_Svg_Parser_Style *s;
	Eina_List *l;

	/* FIXME  we are iterating over every style and going through the
	 * tag tree on egueb_css, we better merge the styles to only parse the tree
	 * once
	 */
	thiz = _egueb_svg_element_svg_get(tag);
	EINA_LIST_FOREACH(thiz->styles, l, s)
	{
		printf("applying style %p\n", s);
		egueb_svg_element_style_apply(s, thiz->tag);
	}
}
#endif
#endif
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Etch * egueb_svg_element_svg_etch_get(Egueb_Dom_Node *n)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = EGUEB_SVG_ELEMENT_SVG(n);
	return thiz->etch;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Egueb_Dom_Node * egueb_svg_element_svg_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_element_svg);
	return n;
}

#if 0
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Element * egueb_svg_element_svg_new(void)
{
	return ESVG_ELEMENT_NEW("SVGSVGElement");
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Element * egueb_svg_element_svg_element_find(Ender_Element *e, const char *id)
{
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	return _egueb_svg_element_svg_element_get_by_id(t, id);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool egueb_svg_is_svg(Ender_Element *e)
{
	if (egueb_svg_element_type_get(e) != ESVG_TYPE_SVG)
		return EINA_FALSE;
	return EINA_TRUE;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_svg_version_set(Ender_Element *e, double version)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_svg_version_get(Ender_Element *e, double *version)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_svg_x_set(Ender_Element *e, Egueb_Svg_Coord *x)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_SVG_X, x, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_svg_y_set(Ender_Element *e, Egueb_Svg_Coord *y)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_SVG_Y, y, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_svg_width_set(Ender_Element *e, Egueb_Svg_Length *width)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_SVG_WIDTH, width, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_svg_width_get(Ender_Element *e, Egueb_Svg_Length *width)
{
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	_egueb_svg_element_svg_width_get(t, width);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_svg_height_set(Ender_Element *e, Egueb_Svg_Length *height)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_SVG_HEIGHT, height, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_svg_height_get(Ender_Element *e, Egueb_Svg_Length *height)
{
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	_egueb_svg_element_svg_height_get(t, height);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_svg_viewbox_set(Ender_Element *e, Egueb_Svg_Rect *v)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_SVG_VIEWBOX, v, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_svg_content_script_type_set(Ender_Element *e, const char *v)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_SVG_CONTENT_SCRIPT_TYPE, v, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_svg_content_script_type_get(Ender_Element *e, const char **v)
{
	Egueb_Dom_Tag *t;
	t = ender_element_object_get(e);
	_egueb_svg_element_svg_content_script_type_get(t, v);
}

#endif

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_svg_animations_pause(Egueb_Dom_Node *n)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = EGUEB_SVG_ELEMENT_SVG(n);
	thiz->paused = EINA_TRUE;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_svg_animations_unpause(Egueb_Dom_Node *n)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = EGUEB_SVG_ELEMENT_SVG(n);
	thiz->paused = EINA_FALSE;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool egueb_svg_element_svg_animations_paused(Egueb_Dom_Node *n)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = EGUEB_SVG_ELEMENT_SVG(n);
	return thiz->paused;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI double egueb_svg_element_svg_time_get(Egueb_Dom_Node *n)
{
	Egueb_Svg_Element_Svg *thiz;
	Etch_Time et;
	double time;

	thiz = EGUEB_SVG_ELEMENT_SVG(n);
	etch_timer_get(thiz->etch, &et);

	time = EGUEB_SMIL_CLOCK_AS_SECONDS(et);
	return time;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_svg_time_set(Egueb_Dom_Node *n, double secs)
{
	Egueb_Svg_Element_Svg *thiz;
	Etch_Time t;

	thiz = EGUEB_SVG_ELEMENT_SVG(n);
	t = secs * EGUEB_SMIL_CLOCK_SECONDS;
	etch_timer_set(thiz->etch, t);
}

#if 0
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_svg_script_alert(Ender_Element *e, const char *msg)
{
	Egueb_Dom_Tag *t;
	Egueb_Svg_Element_Svg *thiz;

	t = ender_element_object_get(e);
	thiz = _egueb_svg_element_svg_get(t);
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
EAPI Eina_List * egueb_svg_element_svg_intersection_list_get(Ender_Element *e, Enesim_Rectangle *rect)
{
	return NULL;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI double egueb_svg_element_svg_base_font_size_get(Ender_Element *e)
{
	Egueb_Dom_Tag *t;
	Egueb_Svg_Element_Svg *thiz;

	t = ender_element_object_get(e);
	thiz = _egueb_svg_element_svg_get(t);
	return thiz->base_font_size;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_svg_base_font_size_set(Ender_Element *e, double base_font_size)
{
	Egueb_Dom_Tag *t;
	Egueb_Svg_Element_Svg *thiz;

	t = ender_element_object_get(e);
	thiz = _egueb_svg_element_svg_get(t);

	thiz->base_font_size = base_font_size;
}

/* FIXME this should be the correct entry point to process
 * the whole svg tree, instead of the setup we have on
 * egueb_svg_element.c this one should call the global symbol
 * to do the process which will do the setup/clanup
 * in case of a referenceable for example, it will do
 * also the propagate
 */
EAPI Eina_Bool egueb_svg_element_svg_setup(Ender_Element *e, Enesim_Log **error)
{
	Egueb_Svg_Context context;
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	egueb_svg_context_init(&context);

	if (egueb_svg_element_internal_setup(t, &context, error) == ESVG_SETUP_FAILED)
	{
		/* clean the context */
		egueb_svg_context_shutdown(&context);
		return EINA_FALSE;
	}
	egueb_svg_context_setup_dequeue(&context);
	return EINA_TRUE;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_svg_container_width_set(Ender_Element *e, double container_width)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_SVG_CONTAINER_WIDTH, container_width, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_svg_container_width_get(Ender_Element *e, double *container_width)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_svg_container_height_set(Ender_Element *e, double container_height)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_SVG_CONTAINER_HEIGHT, container_height, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_svg_container_height_get(Ender_Element *e, double *container_height)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_svg_x_dpi_set(Ender_Element *e, double x_dpi)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_SVG_X_DPI, x_dpi, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_svg_x_dpi_get(Ender_Element *e, double *x_dpi)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_svg_y_dpi_set(Ender_Element *e, double y_dpi)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_SVG_Y_DPI, y_dpi, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_svg_y_dpi_get(Ender_Element *e, double *y_dpi)
{
}
#endif

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool egueb_svg_element_svg_draw(Egueb_Dom_Node *n,
		Enesim_Surface *s, Enesim_Rop rop, Eina_Rectangle *clip,
		int x, int y, Enesim_Log **error)
{
	Enesim_Renderer *r;
	Eina_Bool ret;

	/* TODO first render every svg image into its own surface */
	r = egueb_svg_renderable_renderer_get(n);
	if (!r) return EINA_FALSE;

	ret = enesim_renderer_draw(r, s, rop, clip, x, y, error);
	enesim_renderer_unref(r);
	return ret;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool egueb_svg_element_svg_draw_list(Egueb_Dom_Node *n,
		Enesim_Surface *s, Enesim_Rop rop, Eina_List *clips,
		int x, int y, Enesim_Log **error)
{
	Enesim_Renderer *r;
	Eina_Bool ret;

	/* TODO first render every svg image we have but first interesecting
	 * the damage with the surface area (transformed)
	 */
	r = egueb_svg_renderable_renderer_get(n);
	if (!r) return EINA_FALSE;

	ret = enesim_renderer_draw_list(r, s, rop, clips, x, y, error);
	enesim_renderer_unref(r);
	return ret;
}

EAPI void egueb_svg_element_svg_width_get(Egueb_Dom_Node *n,
		Egueb_Svg_Length_Animated *width)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = EGUEB_SVG_ELEMENT_SVG(n);
	EGUEB_SVG_ELEMENT_ATTR_SIMPLE_GET(thiz->width, width);
}

EAPI void egueb_svg_element_svg_height_get(Egueb_Dom_Node *n,
		Egueb_Svg_Length_Animated *height)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = EGUEB_SVG_ELEMENT_SVG(n);
	EGUEB_SVG_ELEMENT_ATTR_SIMPLE_GET(thiz->height, height);
}

EAPI void egueb_svg_element_svg_current_scale_set(Egueb_Dom_Node *n,
		double scale)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = EGUEB_SVG_ELEMENT_SVG(n);
	thiz->current_scale = scale;
	egueb_dom_element_request_process(n);
}

EAPI void egueb_svg_element_svg_current_translate_set(Egueb_Dom_Node *n,
		Egueb_Svg_Point *p)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = EGUEB_SVG_ELEMENT_SVG(n);
	if (!p) 
	{
		thiz->current_translate.x = 0;
		thiz->current_translate.y = 0;
	}
	else
	{
		thiz->current_translate = *p;
	}
	egueb_dom_element_request_process(n);
}

#if 0
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_svg_element_svg_application_descriptor_set(Ender_Element *e, const Egueb_Svg_Element_Svg_Application_Descriptor *descriptor, void *data)
{
	Egueb_Svg_Element_Svg *thiz;
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	thiz = _egueb_svg_element_svg_get(t);
	thiz->application_descriptor = descriptor;
	thiz->application_data = data;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI const char * egueb_svg_element_svg_base_dir_get(Ender_Element *e)
{
	Egueb_Dom_Tag *t;
	Egueb_Svg_Element_Svg *thiz;

	t = ender_element_object_get(e);
	thiz = _egueb_svg_element_svg_get(t);
	return _egueb_svg_element_svg_base_dir_get(thiz);
}
#endif
