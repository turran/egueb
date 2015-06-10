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
#include "egueb_svg_string.h"
#include "egueb_svg_element.h"
#include "egueb_svg_element_g.h"
#include "egueb_svg_element_image.h"
#include "egueb_svg_point.h"
#include "egueb_svg_element_svg.h"
#include "egueb_svg_document.h"
#include "egueb_svg_renderable.h"

#include "egueb_svg_attr_string_private.h"
#include "egueb_svg_renderable_private.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_ELEMENT_IMAGE_DESCRIPTOR egueb_svg_element_image_descriptor_get()
#define EGUEB_SVG_ELEMENT_IMAGE_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Element_Image_Class, EGUEB_SVG_ELEMENT_IMAGE_DESCRIPTOR)
#define EGUEB_SVG_ELEMENT_IMAGE(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Element_Image, EGUEB_SVG_ELEMENT_IMAGE_DESCRIPTOR)

typedef struct _Egueb_Svg_Element_Image
{
	Egueb_Svg_Renderable base;
	/* properties */
	Egueb_Dom_Node *x;
	Egueb_Dom_Node *y;
	Egueb_Dom_Node *width;
	Egueb_Dom_Node *height;
	Egueb_Dom_Node *xlink_href;
	/* private */
	double gx, gy, gw, gh;
	Egueb_Dom_String *last_xlink;
	Enesim_Renderer *r;
	/* the renderer in case the image is invalid */
	Enesim_Renderer *rectangle;
	Enesim_Renderer *checker;
	/* the renderer in case the image is not a svg */
	Enesim_Renderer *image;
	/* the node in case we need to instantiate a new svg */
	Egueb_Dom_Node *g;
	/* a node in case the user provides the topmost element directly */
	Egueb_Dom_Node *svg;
	Eina_Bool svg_changed;
} Egueb_Svg_Element_Image;

typedef struct _Egueb_Svg_Element_Image_Class
{
	Egueb_Svg_Renderable_Class base;
} Egueb_Svg_Element_Image_Class;

/*----------------------------------------------------------------------------*
 *                               Event monitors                               *
 *----------------------------------------------------------------------------*/
static void _egueb_dom_element_image_g_node_monitor_cb(Egueb_Dom_Event *ev,
		void *data)
{
	Egueb_Svg_Element_Image *thiz = data;

	/* whenever we receive an event, just propagate it */
	egueb_dom_node_event_propagate(EGUEB_DOM_NODE(thiz), ev);
}
/*----------------------------------------------------------------------------*
 *                                  Helpers                                   *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_element_image_parse_data(const char **ostr,
		char *base, char *mime)
{
	const char *tmp;
	const char *str = *ostr;
	/* only use the real data, not the metadata */
	tmp = strchr(str, ';');
	if (!tmp)
	{
		ERR("Bad formed data");
		return EINA_FALSE;
	}

	/* mime */
	str += 5;
	strncpy(mime, str, tmp - str);
	mime[tmp - str] = '\0';
	str = tmp + 1;

	/* the base */
	tmp = strchr(str, ',');
	if (!tmp)
	{
		ERR("Bad formed data");
		return EINA_FALSE;
	}

	strncpy(base, str, tmp - str);
	if (strncmp(base, "base64", 6))
	{
		WARN("Unsupported base '%s'", base);
		return EINA_FALSE;
	}
	str += 7;
	*ostr = str;
	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_element_image_svg_load(Egueb_Dom_Node *n,
		Egueb_Dom_Node *doc, Egueb_Dom_Node *topmost)
{
	Egueb_Svg_Element_Image *thiz;
	Enesim_Renderer *r;
	Enesim_Matrix m;

	/* check that the node is a svg element */
	if (!egueb_svg_element_is_svg(topmost))
	{
		egueb_dom_node_unref(topmost);
		WARN("Topmost element is not a svg");
		return EINA_FALSE;
	}

	/* keep the node */
	thiz = EGUEB_SVG_ELEMENT_IMAGE(n);
	thiz->g = egueb_svg_element_g_new();
	thiz->g = egueb_dom_document_node_adopt(doc, thiz->g, NULL);
	/* add the events that we need to propagate upstream */ 
	egueb_dom_event_target_monitor_add(
			EGUEB_DOM_EVENT_TARGET_CAST(thiz->g),
			_egueb_dom_element_image_g_node_monitor_cb, thiz);
	topmost = egueb_dom_document_node_adopt(doc, topmost, NULL);
	/* add the svg topmost element on the g element */
	egueb_dom_node_child_append(thiz->g, topmost, NULL);
	/* set the transformation */
	enesim_matrix_translate(&m, thiz->gx, thiz->gy);
	egueb_svg_renderable_transform_set(thiz->g, &m);
	/* set the relative nodes, the presentation skipped
	 * because we don't inherit from the parent img
	 */
	egueb_svg_element_geometry_relative_set(thiz->g, n, NULL);

	/* set the proxy for the svg */
	r = egueb_svg_renderable_renderer_get(thiz->g);
	enesim_renderer_proxy_proxied_set(thiz->r, r);

	return EINA_TRUE;
}

static void _egueb_svg_element_image_image_cb(Egueb_Dom_Node *n,
		Enesim_Surface *s)
{
	Egueb_Svg_Element_Image *thiz;

	thiz = EGUEB_SVG_ELEMENT_IMAGE(n);
	INFO_ELEMENT(n, "Image loaded");
	enesim_renderer_image_source_surface_set(thiz->image, s);
	enesim_renderer_proxy_proxied_set(thiz->r, enesim_renderer_ref(thiz->image));
}

static void _egueb_svg_element_image_data_cb(Egueb_Dom_Node *n,
		Enesim_Stream *data)
{
	Egueb_Svg_Element_Image *thiz;
	Egueb_Dom_Node *doc;
	const char *mime;

	thiz = EGUEB_SVG_ELEMENT_IMAGE(n);
	mime = enesim_image_mime_data_from(data);
	if (!mime)
	{
		ERR_ELEMENT(n, "Impossible to identify the image data");
		goto done;
	}

	INFO_ELEMENT(n, "Uri fetched with MIME '%s'", mime);
	doc = egueb_dom_node_owner_document_get(n);
	if (!doc)
	{
		WARN("No document set");
		goto done;
	}

	/* if is svg, then do create the node otherwise load it */
	if (!strcmp(mime, "image/svg+xml"))
	{
		Egueb_Dom_Node *new_doc;
		Egueb_Dom_Node *topmost = NULL;

		INFO("Parsing the svg file");

		/* parse the file */
		new_doc = egueb_svg_document_new();
		egueb_dom_parser_parse(enesim_stream_ref(data), &new_doc);
		topmost = egueb_dom_document_document_element_get(new_doc);
		egueb_dom_node_unref(new_doc);

		_egueb_svg_element_image_svg_load(n, doc, topmost);
	}
	else
	{
		Egueb_Dom_Event *e;

		e = egueb_dom_event_io_image_new(data, _egueb_svg_element_image_image_cb);
		egueb_dom_event_target_event_dispatch(
				EGUEB_DOM_EVENT_TARGET_CAST(thiz), e, NULL, NULL);
	}
	egueb_dom_node_unref(doc);
done:
	enesim_stream_unref(data);
}

static Eina_Bool _egueb_svg_element_image_uri_load(Egueb_Svg_Element_Image *thiz,
		Egueb_Dom_Node *doc, Egueb_Dom_String *uri)
{
	const char *str;

	if (!egueb_dom_string_is_valid(uri)) {
		WARN("No uri set, nothing to do");
		return EINA_FALSE;
	}

	str = egueb_dom_string_string_get(uri);
	/* check if the data is inlined, if so, just let the document
	 * load it
	 */
	if (!strncmp(str, "data:image", 10))
	{
		Egueb_Dom_Event *e;
		Enesim_Stream *base64;
		Enesim_Stream *data;
		char mime[1024];
		char base[1024];
		char *base64_data;

		if (!_egueb_svg_element_image_parse_data(&str, base, mime))
		{
			ERR_ELEMENT(EGUEB_DOM_NODE(thiz), "Failed to parse the image data");
			return EINA_FALSE;
		}

		/* TODO if it is a svg, then just parse it */
		/* TODO Be sure to keep a reference of the string, so we can safely
		 * delegate the image loading
		 */
		DBG_ELEMENT(EGUEB_DOM_NODE(thiz), "Loading a base64 based image with MIME '%s'", mime);

		base64_data = strdup(str);
		base64 = enesim_stream_buffer_new(base64_data, strlen(base64_data), free);
		data = enesim_stream_base64_new(base64);

		e = egueb_dom_event_io_image_new(data, _egueb_svg_element_image_image_cb);
		egueb_dom_event_target_event_dispatch(
				EGUEB_DOM_EVENT_TARGET_CAST(thiz), e, NULL, NULL);
		enesim_stream_unref(data);
	}
	/* otherwise, let the document get the data and call our own callback */
	/* this callback should again make the document load the image for us */
	else
	{
		Egueb_Dom_Event *e;
		Egueb_Dom_Uri u;

		if (!egueb_dom_uri_string_from(&u, uri))
		{
			ERR_ELEMENT(EGUEB_DOM_NODE(thiz), "Wrong URI");
			return EINA_FALSE;
		}

		e = egueb_dom_event_io_data_new(&u, _egueb_svg_element_image_data_cb);
		egueb_dom_event_target_event_dispatch(
				EGUEB_DOM_EVENT_TARGET_CAST(thiz), e, NULL, NULL);
		egueb_dom_uri_cleanup(&u);
	}
	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_element_image_load(Egueb_Svg_Element_Image *thiz,
		Egueb_Dom_Node *doc)
{
	Egueb_Dom_String *xlink = NULL;
	Eina_Bool xlink_changed = EINA_FALSE;
	Eina_Bool ret = EINA_TRUE;

	/* be sure to know if the xlink has changed */
	egueb_dom_attr_final_get(thiz->xlink_href, &xlink);
	if (!egueb_dom_string_is_equal(xlink, thiz->last_xlink))
		xlink_changed = EINA_TRUE;

	if (!xlink_changed && !thiz->svg_changed)
	{
		DBG("Nothing to process, everything's like before");
		goto done;
	}

	/* in case we had an element, be sure to remove it */
	if (thiz->g)
	{
		egueb_dom_node_unref(thiz->g);
		thiz->g = NULL;
	}

	/* check if we either use the xlink attribute or the user provided svg */
	if (thiz->svg)
	{
		DBG("Loading user provided element");
		ret = _egueb_svg_element_image_svg_load(EGUEB_DOM_NODE(thiz), doc, egueb_dom_node_ref(thiz->svg));
	}
	else
	{
		DBG("Loading href reference");
		ret = _egueb_svg_element_image_uri_load(thiz, doc, xlink);
	}

	thiz->svg_changed = EINA_FALSE;

	/* finally swap the last xlink */
	if (thiz->last_xlink)
	{
		egueb_dom_string_unref(thiz->last_xlink);
		thiz->last_xlink = NULL;
	}
	thiz->last_xlink = egueb_dom_string_dup(xlink);
done:
	egueb_dom_string_unref(xlink);
	return ret;
}
/*----------------------------------------------------------------------------*
 *                            Renderable interface                            *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_element_image_process(
		Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Element_Image *thiz;
	Egueb_Svg_Element *e, *e_parent;
	Egueb_Svg_Length x, y, w, h;
	Egueb_Dom_Node *relative, *doc;
	Eina_Bool ret;
	double font_size;

	thiz = EGUEB_SVG_ELEMENT_IMAGE(r);
	egueb_dom_attr_final_get(thiz->x, &x);
	egueb_dom_attr_final_get(thiz->y, &y);
	egueb_dom_attr_final_get(thiz->width, &w);
	egueb_dom_attr_final_get(thiz->height, &h);

	/* calculate the real size */
	relative = egueb_svg_element_geometry_relative_get(EGUEB_DOM_NODE(r));
	if (!relative)
	{
		WARN("No relative available");
		return EINA_FALSE;
	}
	doc = egueb_dom_node_owner_document_get(EGUEB_DOM_NODE(r));
	if (!doc)
	{
		WARN("No document set");
		egueb_dom_node_unref(relative);
		return EINA_FALSE;
	}

	e_parent = EGUEB_SVG_ELEMENT(relative);
	e = EGUEB_SVG_ELEMENT(r);
	font_size = egueb_svg_document_font_size_get(doc);

	thiz->gx = egueb_svg_coord_final_get(&x, e_parent->viewbox.w, font_size);
	thiz->gy = egueb_svg_coord_final_get(&y, e_parent->viewbox.h, font_size);
	thiz->gw = egueb_svg_coord_final_get(&w, e_parent->viewbox.w, font_size);
	thiz->gh = egueb_svg_coord_final_get(&h, e_parent->viewbox.h, font_size);
	/* set the viewbox in case we have a svg image */
	e->viewbox.x = thiz->gx;
	e->viewbox.y = thiz->gy;
	e->viewbox.w = thiz->gw;
	e->viewbox.h = thiz->gh;

	ret = _egueb_svg_element_image_load(thiz, doc);

	egueb_dom_node_unref(relative);
	egueb_dom_node_unref(doc);

	DBG_ELEMENT(EGUEB_DOM_NODE(r), "x: %g, y: %g, width: %g, height: %g",
			thiz->gx, thiz->gy, thiz->gw, thiz->gh);
	/* setup our own renderers */
	enesim_renderer_rectangle_x_set(thiz->rectangle, thiz->gx);
	enesim_renderer_rectangle_y_set(thiz->rectangle, thiz->gy);
	enesim_renderer_rectangle_width_set(thiz->rectangle, thiz->gw);
	enesim_renderer_rectangle_height_set(thiz->rectangle, thiz->gh);
	enesim_renderer_transformation_set(thiz->rectangle, &e->transform);

	enesim_renderer_image_x_set(thiz->image, thiz->gx);
	enesim_renderer_image_y_set(thiz->image, thiz->gy);
	enesim_renderer_image_width_set(thiz->image, thiz->gw);
	enesim_renderer_image_height_set(thiz->image, thiz->gh);
	enesim_renderer_transformation_set(thiz->image, &e->transform);

	/* in case we do have created a <g> (the svg case) make sure to process it */
	if (thiz->g)
	{
		egueb_dom_element_process(thiz->g);
	}

	return ret;
}

static Enesim_Renderer * _egueb_svg_element_image_renderer_get(
		Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Element_Image *thiz;

	thiz = EGUEB_SVG_ELEMENT_IMAGE(r);
	return enesim_renderer_ref(thiz->r);
}

static void _egueb_svg_element_image_bounds_get(Egueb_Svg_Renderable *r,
		Enesim_Rectangle *bounds)
{
	Egueb_Svg_Element_Image *thiz;

	thiz = EGUEB_SVG_ELEMENT_IMAGE(r);
	enesim_rectangle_coords_from(bounds, thiz->gx, thiz->gy,
			thiz->gw, thiz->gh);
}
/*----------------------------------------------------------------------------*
 *                              Element interface                             *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_String * _egueb_svg_element_image_tag_name_get(
		Egueb_Dom_Element *e)
{
	return egueb_dom_string_ref(EGUEB_SVG_NAME_IMAGE);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_RENDERABLE_DESCRIPTOR,
		Egueb_Svg_Element_Image, Egueb_Svg_Element_Image_Class,
		egueb_svg_element_image);

static void _egueb_svg_element_image_class_init(void *k)
{
	Egueb_Svg_Renderable_Class *r_klass;
	Egueb_Dom_Element_Class *e_klass;

	r_klass = EGUEB_SVG_RENDERABLE_CLASS(k);
	r_klass->bounds_get = _egueb_svg_element_image_bounds_get;
	r_klass->renderer_get = _egueb_svg_element_image_renderer_get;
	r_klass->process = _egueb_svg_element_image_process;

	e_klass= EGUEB_DOM_ELEMENT_CLASS(k);
	e_klass->tag_name_get = _egueb_svg_element_image_tag_name_get;
}

static void _egueb_svg_element_image_instance_init(void *o)
{
	Egueb_Svg_Element_Image *thiz;
	Egueb_Dom_Node *n;
	Enesim_Renderer *r;

	thiz = EGUEB_SVG_ELEMENT_IMAGE(o);

	r = enesim_renderer_image_new();
	thiz->image = r;

	r = enesim_renderer_checker_new();
	enesim_renderer_checker_width_set(r, 10);
	enesim_renderer_checker_height_set(r, 10);
	enesim_renderer_checker_even_color_set(r, 0xff00ffff);
	enesim_renderer_checker_odd_color_set(r, 0xff00ff00);
	thiz->checker = r;

	r = enesim_renderer_rectangle_new();
	enesim_renderer_shape_fill_renderer_set(r, thiz->checker);
	thiz->rectangle = r;

	r = enesim_renderer_proxy_new();
	thiz->r = r;

	/* Default values */

	/* create the properties */
	thiz->x = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_NAME_X),
			&EGUEB_SVG_LENGTH_0, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->y = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_NAME_Y),
			&EGUEB_SVG_LENGTH_0, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->width = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_NAME_WIDTH),
			&EGUEB_SVG_LENGTH_0,
			EINA_TRUE, EINA_FALSE, EINA_FALSE);
	thiz->height = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_NAME_HEIGHT),
			&EGUEB_SVG_LENGTH_0,
			EINA_TRUE, EINA_FALSE, EINA_FALSE);
	thiz->xlink_href = egueb_svg_attr_string_new(
			egueb_dom_string_ref(EGUEB_DOM_NAME_XLINK_HREF),
			NULL);

	n = EGUEB_DOM_NODE(o);
	egueb_dom_element_attribute_node_set(n, egueb_dom_node_ref(thiz->x), NULL);
	egueb_dom_element_attribute_node_set(n, egueb_dom_node_ref(thiz->y), NULL);
	egueb_dom_element_attribute_node_set(n, egueb_dom_node_ref(thiz->width), NULL);
	egueb_dom_element_attribute_node_set(n, egueb_dom_node_ref(thiz->height), NULL);
	egueb_dom_element_attribute_node_set(n, egueb_dom_node_ref(thiz->xlink_href), NULL);
}

static void _egueb_svg_element_image_instance_deinit(void *o)
{
	Egueb_Svg_Element_Image *thiz;

	thiz = EGUEB_SVG_ELEMENT_IMAGE(o);

	if (thiz->last_xlink)
	{
		egueb_dom_string_unref(thiz->last_xlink);
		thiz->last_xlink = NULL;
	}

	if (thiz->svg)
	{
		egueb_dom_node_unref(thiz->svg);
		thiz->svg = NULL;
	}

	if (thiz->g)
	{
		egueb_dom_node_unref(thiz->g);
		thiz->g = NULL;
	}
	enesim_renderer_unref(thiz->r);
	/* destroy the properties */
	egueb_dom_node_unref(thiz->x);
	egueb_dom_node_unref(thiz->y);
	egueb_dom_node_unref(thiz->width);
	egueb_dom_node_unref(thiz->height);
	egueb_dom_node_unref(thiz->xlink_href);
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_svg_element_image_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_element_image);
	return n;
}

/**
 * Sets the x coordinate of an image element
 * @param[in] n The image element to set the x coordinate @ender_transfer{none}
 * @param[in] x The x coordinate to set @ender_transfer{content}
 */
EAPI void egueb_svg_element_image_x_set_simple(Egueb_Dom_Node *n, const Egueb_Svg_Coord *x)
{
	Egueb_Svg_Element_Image *thiz;

	thiz = EGUEB_SVG_ELEMENT_IMAGE(n);
	egueb_dom_attr_set(thiz->x, EGUEB_DOM_ATTR_TYPE_BASE, x);
}

/**
 * Gets the x coordinate of an image element
 * @ender_prop{x}
 * @param[in] n The image element to get the x coordinate @ender_transfer{none}
 * @param[out] x The pointer to store the x coordinate @ender_transfer{content}
 */
EAPI void egueb_svg_element_image_x_get(Egueb_Dom_Node *n, Egueb_Svg_Coord_Animated *x)
{
	Egueb_Svg_Element_Image *thiz;

	thiz = EGUEB_SVG_ELEMENT_IMAGE(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->x, x);
}

/**
 * Sets the y coordinate of an image element
 * @param[in] n The image element to set the y coordinate @ender_transfer{none}
 * @param[in] y The y coordinate to set @ender_transfer{content}
 */
EAPI void egueb_svg_element_image_y_set_simple(Egueb_Dom_Node *n, const Egueb_Svg_Coord *y)
{
	Egueb_Svg_Element_Image *thiz;

	thiz = EGUEB_SVG_ELEMENT_IMAGE(n);
	egueb_dom_attr_set(thiz->y, EGUEB_DOM_ATTR_TYPE_BASE, y);
}

/**
 * Gets the y coordinate of an image element
 * @ender_prop{y}
 * @param[in] n The image element to get the y coordinate @ender_transfer{none}
 * @param[out] y The pointer to store the y coordinate @ender_transfer{content}
 */
EAPI void egueb_svg_element_image_y_get(Egueb_Dom_Node *n, Egueb_Svg_Coord_Animated *y)
{
	Egueb_Svg_Element_Image *thiz;

	thiz = EGUEB_SVG_ELEMENT_IMAGE(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->y, y);
}

/**
 * Sets the width of an image element
 * @param[in] n The image element to set the width @ender_transfer{none}
 * @param[in] width The width to set @ender_transfer{content}
 */
EAPI void egueb_svg_element_image_width_set_simple(Egueb_Dom_Node *n, const Egueb_Svg_Length *width)
{
	Egueb_Svg_Element_Image *thiz;

	thiz = EGUEB_SVG_ELEMENT_IMAGE(n);
	egueb_dom_attr_set(thiz->width, EGUEB_DOM_ATTR_TYPE_BASE, width);
}

/**
 * Gets the width of an image element
 * @ender_prop{width}
 * @param[in] n The image element to get the width @ender_transfer{none}
 * @param[out] width The pointer to store the width @ender_transfer{content}
 */
EAPI void egueb_svg_element_image_width_get(Egueb_Dom_Node *n, Egueb_Svg_Length_Animated *width)
{
	Egueb_Svg_Element_Image *thiz;

	thiz = EGUEB_SVG_ELEMENT_IMAGE(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->width, width);
}

/**
 * Sets the height of an image element
 * @param[in] n The image element to set the height @ender_transfer{none}
 * @param[in] height The height to set @ender_transfer{content}
 */
EAPI void egueb_svg_element_image_height_set_simple(Egueb_Dom_Node *n, const Egueb_Svg_Length *height)
{
	Egueb_Svg_Element_Image *thiz;

	thiz = EGUEB_SVG_ELEMENT_IMAGE(n);
	egueb_dom_attr_set(thiz->height, EGUEB_DOM_ATTR_TYPE_BASE, height);
}

/**
 * Gets the height of an image element
 * @ender_prop{height}
 * @param[in] n The image element to get the height @ender_transfer{none}
 * @param[out] height The pointer to store the height @ender_transfer{content}
 */
EAPI void egueb_svg_element_image_height_get(Egueb_Dom_Node *n, Egueb_Svg_Length_Animated *height)
{
	Egueb_Svg_Element_Image *thiz;

	thiz = EGUEB_SVG_ELEMENT_IMAGE(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->height, height);
}

/**
 * Sets the href of an image element
 * @param[in] n The image element to get the href @ender_transfer{none}
 * @param[out] v The href to set @ender_transfer{full}
 */
EAPI void egueb_svg_element_image_href_set_simple(Egueb_Dom_Node *n, Egueb_Dom_String *v)
{
	Egueb_Svg_Element_Image *thiz;

	thiz = EGUEB_SVG_ELEMENT_IMAGE(n);
	egueb_dom_attr_set(thiz->xlink_href, EGUEB_DOM_ATTR_TYPE_BASE, v);
}

/**
 * Gets the href of an image element
 * @ender_prop{href}
 * @param[in] n The image element to get the href @ender_transfer{none}
 * @param[out] v The pointer to store the href @ender_transfer{content}
 */
EAPI void egueb_svg_element_image_href_get(Egueb_Dom_Node *n, Egueb_Svg_String_Animated *v)
{
	Egueb_Svg_Element_Image *thiz;

	thiz = EGUEB_SVG_ELEMENT_IMAGE(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->xlink_href, v);
}

EAPI void egueb_svg_element_image_svg_set(Egueb_Dom_Node *n, Egueb_Dom_Node *svg)
{
	Egueb_Svg_Element_Image *thiz;

	thiz = EGUEB_SVG_ELEMENT_IMAGE(n);
	thiz->svg_changed = EINA_TRUE;
	if (!svg)
	{
		egueb_dom_node_unref(thiz->svg);
		thiz->svg = NULL;
	}
	else
	{
		if (!egueb_svg_element_is_svg(svg))
			return;
		thiz->svg = svg;
	}
}
