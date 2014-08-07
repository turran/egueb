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
#include "egueb_svg_string.h"
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
	Egueb_Smil_Timeline *timeline;
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


static void _egueb_svg_element_svg_timeline_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Svg_Element_Svg *thiz = data;

	INFO("Requesting timeline");
	egueb_smil_event_timeline_set(e, egueb_smil_timeline_ref(thiz->timeline));
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
	svg_doc = egueb_dom_node_owner_document_get(EGUEB_DOM_NODE(r));
	relative = egueb_svg_element_geometry_relative_get(EGUEB_DOM_NODE(r));
	if (!relative)
	{
		egueb_svg_document_width_get(svg_doc, &relative_width);
		egueb_svg_document_height_get(svg_doc, &relative_height);
		relative_x = 0;
		relative_y = 0;

		enesim_matrix_identity(&relative_transform);
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

	if (!relative)
	{
		Enesim_Matrix m;

		/* Set our current scale/translate */
		enesim_matrix_scale(&m, thiz->current_scale, thiz->current_scale);
		enesim_matrix_translate(&relative_transform, thiz->current_translate.x, thiz->current_translate.y);
		enesim_matrix_compose(&relative_transform, &m, &relative_transform);
	}

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

static Ender_Item * _egueb_svg_element_svg_item_get(Egueb_Svg_Element *e)
{
	const Ender_Lib *lib;

	lib = ender_lib_find("egueb_svg");
	return ender_lib_item_find(lib, "egueb.svg.element.svg");
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
	Egueb_Svg_Element_Class *es_klass;
	Egueb_Dom_Element_Class *e_klass;

	klass = EGUEB_SVG_RENDERABLE_CLASS(k);
	klass->renderer_get = _egueb_svg_element_svg_renderer_get;
	klass->process = _egueb_svg_element_svg_process;
	klass->painter_get = _egueb_svg_element_svg_painter_get;
	klass->painter_apply = _egueb_svg_element_svg_painter_apply;

	es_klass= EGUEB_SVG_ELEMENT_CLASS(k);
	es_klass->item_get = _egueb_svg_element_svg_item_get;

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
	egueb_dom_node_event_listener_add(n,
			EGUEB_SMIL_EVENT_TIMELINE,
			_egueb_svg_element_svg_timeline_cb,
			EINA_TRUE, thiz);
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
	thiz->timeline = egueb_smil_timeline_new();
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
	/* remove timeline and all the animation system  */
	egueb_smil_timeline_unref(thiz->timeline);
	/* TODO free the scriptors */
}

#if 0
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
Egueb_Smil_Timeline * egueb_svg_element_svg_timeline_get(Egueb_Dom_Node *n)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = EGUEB_SVG_ELEMENT_SVG(n);
	return egueb_smil_timeline_ref(thiz->timeline);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_svg_element_svg_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_element_svg);
	return n;
}

EAPI Eina_Bool egueb_svg_element_is_svg(Egueb_Dom_Node *n)
{
	if (!n) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(n),
			EGUEB_SVG_ELEMENT_SVG_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}

#if 0
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

EAPI void egueb_svg_element_svg_animations_pause(Egueb_Dom_Node *n)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = EGUEB_SVG_ELEMENT_SVG(n);
	thiz->paused = EINA_TRUE;
}

EAPI void egueb_svg_element_svg_animations_unpause(Egueb_Dom_Node *n)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = EGUEB_SVG_ELEMENT_SVG(n);
	thiz->paused = EINA_FALSE;
}

EAPI Eina_Bool egueb_svg_element_svg_animations_paused(Egueb_Dom_Node *n)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = EGUEB_SVG_ELEMENT_SVG(n);
	return thiz->paused;
}

#if 0
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

/**
 * Sets the x coordinate of a svg element
 * @param[in] n The svg element to set the x coordinate @ender_transfer{none}
 * @param[in] x The x coordinate to set @ender_transfer{content}
 */
EAPI void egueb_svg_element_svg_x_set_simple(Egueb_Dom_Node *n, const Egueb_Svg_Coord *x)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = EGUEB_SVG_ELEMENT_SVG(n);
	egueb_dom_attr_set(thiz->x, EGUEB_DOM_ATTR_TYPE_BASE, x);
}

/**
 * Gets the x coordinate of a svg element
 * @ender_prop{x}
 * @param[in] n The svg element to get the x coordinate @ender_transfer{none}
 * @param[out] x The pointer to store the x coordinate @ender_transfer{content}
 */
EAPI void egueb_svg_element_svg_x_get(Egueb_Dom_Node *n, Egueb_Svg_Coord_Animated *x)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = EGUEB_SVG_ELEMENT_SVG(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->x, x);
}

/**
 * Sets the y coordinate of a svg element
 * @param[in] n The svg element to set the y coordinate @ender_transfer{none}
 * @param[in] y The y coordinate to set @ender_transfer{content}
 */
EAPI void egueb_svg_element_svg_y_set_simple(Egueb_Dom_Node *n, const Egueb_Svg_Coord *y)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = EGUEB_SVG_ELEMENT_SVG(n);
	egueb_dom_attr_set(thiz->y, EGUEB_DOM_ATTR_TYPE_BASE, y);
}

/**
 * Gets the y coordinate of a svg element
 * @ender_prop{y}
 * @param[in] n The svg element to get the y coordinate @ender_transfer{none}
 * @param[out] y The pointer to store the y coordinate @ender_transfer{content}
 */
EAPI void egueb_svg_element_svg_y_get(Egueb_Dom_Node *n, Egueb_Svg_Coord_Animated *y)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = EGUEB_SVG_ELEMENT_SVG(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->y, y);
}

/**
 * Sets the width of a svg element
 * @param[in] n The svg element to set the width @ender_transfer{none}
 * @param[in] width The width to set @ender_transfer{content}
 */
EAPI void egueb_svg_element_svg_width_set_simple(Egueb_Dom_Node *n, const Egueb_Svg_Length *width)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = EGUEB_SVG_ELEMENT_SVG(n);
	egueb_dom_attr_set(thiz->width, EGUEB_DOM_ATTR_TYPE_BASE, width);
}

/**
 * Gets the width of a svg element
 * @ender_prop{width}
 * @param[in] n The svg element to get the width @ender_transfer{none}
 * @param[out] width The pointer to store the width @ender_transfer{content}
 */
EAPI void egueb_svg_element_svg_width_get(Egueb_Dom_Node *n, Egueb_Svg_Length_Animated *width)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = EGUEB_SVG_ELEMENT_SVG(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->width, width);
}

/**
 * Sets the height of a svg element
 * @param[in] n The svg element to set the height @ender_transfer{none}
 * @param[in] height The height to set @ender_transfer{content}
 */
EAPI void egueb_svg_element_svg_height_set_simple(Egueb_Dom_Node *n, const Egueb_Svg_Length *height)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = EGUEB_SVG_ELEMENT_SVG(n);
	egueb_dom_attr_set(thiz->height, EGUEB_DOM_ATTR_TYPE_BASE, height);
}

/**
 * Gets the height of a svg element
 * @ender_prop{height}
 * @param[in] n The svg element to get the height @ender_transfer{none}
 * @param[out] height The pointer to store the height @ender_transfer{content}
 */
EAPI void egueb_svg_element_svg_height_get(Egueb_Dom_Node *n, Egueb_Svg_Length_Animated *height)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = EGUEB_SVG_ELEMENT_SVG(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->height, height);
}

/**
 * @ender_prop{current_scale}
 */
EAPI void egueb_svg_element_svg_current_scale_set(Egueb_Dom_Node *n,
		double scale)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = EGUEB_SVG_ELEMENT_SVG(n);
	thiz->current_scale = scale;
	egueb_dom_element_request_process(n);
}

/**
 * @ender_prop{current_scale}
 */
EAPI double egueb_svg_element_svg_current_scale_get(Egueb_Dom_Node *n)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = EGUEB_SVG_ELEMENT_SVG(n);
	return thiz->current_scale;
}

/**
 * Sets the current translation on a svg element
 * @ender_prop{current_translate}
 * @param[in] n The SVG element to set the current translation @ender_transfer{none}
 * @param[in] p The transaltion to set @ender_transfer{content}
 */
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

/**
 * Gets the current translation of the svg element
 * @ender_prop{current_translate}
 * @param[in] n The SVG element to get the current translation from @ender_transfer{none}
 * @param[out] p The point to store the current translation @ender_transfer{content}
 */
EAPI void egueb_svg_element_svg_current_translate_get(Egueb_Dom_Node *n,
		Egueb_Svg_Point *p)
{
	Egueb_Svg_Element_Svg *thiz;

	thiz = EGUEB_SVG_ELEMENT_SVG(n);
	*p = thiz->current_translate;
}

EAPI Egueb_Svg_Point * egueb_svg_element_svg_svg_point_create(Egueb_Dom_Node *n)
{
	Egueb_Svg_Point *ret;

	ret = calloc(1, sizeof(Egueb_Svg_Point));
	return ret;
}
