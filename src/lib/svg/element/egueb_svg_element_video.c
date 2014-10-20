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
#include "egueb_svg_element_video.h"
#include "egueb_svg_point.h"
#include "egueb_svg_element_svg.h"
#include "egueb_svg_document.h"
#include "egueb_svg_renderable.h"

#include "egueb_svg_attr_string_private.h"
#include "egueb_svg_renderable_private.h"

/* This object tries to follow the spec found on:
 * http://www.w3.org/TR/SVGTiny12/multimedia.html#VideoElement
 */

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_ELEMENT_VIDEO_DESCRIPTOR egueb_svg_element_video_descriptor_get()
#define EGUEB_SVG_ELEMENT_VIDEO_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Element_Video_Class, EGUEB_SVG_ELEMENT_VIDEO_DESCRIPTOR)
#define EGUEB_SVG_ELEMENT_VIDEO(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Element_Video, EGUEB_SVG_ELEMENT_VIDEO_DESCRIPTOR)

typedef struct _Egueb_Svg_Element_Video
{
	Egueb_Svg_Renderable base;
	/* properties */
	Egueb_Dom_Node *x;
	Egueb_Dom_Node *y;
	Egueb_Dom_Node *width;
	Egueb_Dom_Node *height;
	Egueb_Dom_Node *xlink_href;
	/* private */
	Egueb_Dom_Video_Provider *video_provider;
	double gx, gy, gw, gh;
	Egueb_Dom_String *last_xlink;
	Enesim_Renderer *r;
} Egueb_Svg_Element_Video;

typedef struct _Egueb_Svg_Element_Video_Class
{
	Egueb_Svg_Renderable_Class base;
} Egueb_Svg_Element_Video_Class;

/*----------------------------------------------------------------------------*
 *                           Video provider notifier                          *
 *----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*
 *                                  Helpers                                   *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_element_video_load(Egueb_Svg_Element_Video *thiz,
		Egueb_Dom_Node *doc)
{
	Egueb_Dom_String *xlink = NULL;
	Egueb_Dom_String *location;
	Egueb_Dom_Uri uri, final;
	Eina_Bool ret = EINA_TRUE;

	/* be sure to know if the xlink has changed */
	egueb_dom_attr_final_get(thiz->xlink_href, &xlink);
	if (egueb_dom_string_is_equal(xlink, thiz->last_xlink))
	{
		DBG("Nothing to process, everything's like before");
		goto done;
	}

	DBG("Loading href reference");
	if (!thiz->video_provider)
	{
		Egueb_Dom_Event *ev;

		ev = egueb_dom_event_multimedia_video_new(
			NULL,
			enesim_renderer_ref(thiz->r));
		egueb_dom_node_event_dispatch(EGUEB_DOM_NODE(thiz), egueb_dom_event_ref(ev), NULL, NULL);
		thiz->video_provider = egueb_dom_event_multimedia_video_provider_get(ev);
		egueb_dom_event_unref(ev);

		INFO("Requested video provider");
		if (!thiz->video_provider)
		{
			ERR("No video provider provided");
			goto wrong;
		}
	}

	if (!egueb_dom_uri_string_from(&uri, xlink))
	{
		ERR_ELEMENT(EGUEB_DOM_NODE(thiz), "Wrong URI");
		goto wrong;
	}

	location = egueb_dom_document_uri_get(doc);
	if (!egueb_dom_uri_resolve(&uri, location, &final))
	{
		ERR_ELEMENT(EGUEB_DOM_NODE(thiz), "Can not resolve URI");
		goto bad_uri;
	}

	if (final.fragment)
	{
		ERR_ELEMENT(EGUEB_DOM_NODE(thiz), "URI with fragment not supported");
		goto has_fragment;
	}
	/* set the uri */
	egueb_dom_video_provider_open(thiz->video_provider, final.location);
	/* go to play */
	egueb_dom_video_provider_play(thiz->video_provider);
has_fragment:
	egueb_dom_uri_cleanup(&final);
bad_uri:
	egueb_dom_string_unref(location);
	egueb_dom_uri_cleanup(&uri);
wrong:
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
static Eina_Bool _egueb_svg_element_video_process(
		Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Element_Video *thiz;
	Egueb_Svg_Element *e, *e_parent;
	Egueb_Svg_Length x, y, w, h;
	Egueb_Dom_Node *relative, *doc;
	Eina_Bool ret;
	double font_size;

	thiz = EGUEB_SVG_ELEMENT_VIDEO(r);
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

	ret = _egueb_svg_element_video_load(thiz, doc);

	egueb_dom_node_unref(relative);
	egueb_dom_node_unref(doc);

	DBG_ELEMENT(EGUEB_DOM_NODE(r), "x: %g, y: %g, width: %g, height: %g",
			thiz->gx, thiz->gy, thiz->gw, thiz->gh);
	/* setup our own renderers */
	enesim_renderer_image_x_set(thiz->r, thiz->gx);
	enesim_renderer_image_y_set(thiz->r, thiz->gy);
	enesim_renderer_image_width_set(thiz->r, thiz->gw);
	enesim_renderer_image_height_set(thiz->r, thiz->gh);
	enesim_renderer_transformation_set(thiz->r, &e->transform);

	return ret;
}

static Enesim_Renderer * _egueb_svg_element_video_renderer_get(
		Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Element_Video *thiz;

	thiz = EGUEB_SVG_ELEMENT_VIDEO(r);
	return enesim_renderer_ref(thiz->r);
}

static void _egueb_svg_element_video_bounds_get(Egueb_Svg_Renderable *r,
		Enesim_Rectangle *bounds)
{
	Egueb_Svg_Element_Video *thiz;

	thiz = EGUEB_SVG_ELEMENT_VIDEO(r);
	enesim_rectangle_coords_from(bounds, thiz->gx, thiz->gy,
			thiz->gw, thiz->gh);
}
/*----------------------------------------------------------------------------*
 *                              Element interface                             *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_String * _egueb_svg_element_video_tag_name_get(
		Egueb_Dom_Element *e)
{
	return egueb_dom_string_ref(EGUEB_SVG_NAME_VIDEO);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_RENDERABLE_DESCRIPTOR,
		Egueb_Svg_Element_Video, Egueb_Svg_Element_Video_Class,
		egueb_svg_element_video);

static void _egueb_svg_element_video_class_init(void *k)
{
	Egueb_Svg_Renderable_Class *r_klass;
	Egueb_Dom_Element_Class *e_klass;

	r_klass = EGUEB_SVG_RENDERABLE_CLASS(k);
	r_klass->bounds_get = _egueb_svg_element_video_bounds_get;
	r_klass->renderer_get = _egueb_svg_element_video_renderer_get;
	r_klass->process = _egueb_svg_element_video_process;

	e_klass= EGUEB_DOM_ELEMENT_CLASS(k);
	e_klass->tag_name_get = _egueb_svg_element_video_tag_name_get;
}

static void _egueb_svg_element_video_instance_init(void *o)
{
	Egueb_Svg_Element_Video *thiz;
	Egueb_Dom_Node *n;
	Enesim_Renderer *r;

	thiz = EGUEB_SVG_ELEMENT_VIDEO(o);

	r = enesim_renderer_image_new();
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

static void _egueb_svg_element_video_instance_deinit(void *o)
{
	Egueb_Svg_Element_Video *thiz;

	thiz = EGUEB_SVG_ELEMENT_VIDEO(o);

	if (thiz->last_xlink)
	{
		egueb_dom_string_unref(thiz->last_xlink);
		thiz->last_xlink = NULL;
	}

	if (thiz->video_provider)
	{
		egueb_dom_video_provider_unref(thiz->video_provider);
		thiz->video_provider = NULL;
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
EAPI Egueb_Dom_Node * egueb_svg_element_video_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_element_video);
	return n;
}

/**
 * Sets the x coordinate of a video element
 * @param[in] n The image element to set the x coordinate @ender_transfer{none}
 * @param[in] x The x coordinate to set @ender_transfer{content}
 */
EAPI void egueb_svg_element_video_x_set_simple(Egueb_Dom_Node *n, const Egueb_Svg_Coord *x)
{
	Egueb_Svg_Element_Video *thiz;

	thiz = EGUEB_SVG_ELEMENT_VIDEO(n);
	egueb_dom_attr_set(thiz->x, EGUEB_DOM_ATTR_TYPE_BASE, x);
}

/**
 * Gets the x coordinate of a video element
 * @ender_prop{x}
 * @param[in] n The image element to get the x coordinate @ender_transfer{none}
 * @param[out] x The pointer to store the x coordinate @ender_transfer{content}
 */
EAPI void egueb_svg_element_video_x_get(Egueb_Dom_Node *n, Egueb_Svg_Coord_Animated *x)
{
	Egueb_Svg_Element_Video *thiz;

	thiz = EGUEB_SVG_ELEMENT_VIDEO(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->x, x);
}

/**
 * Sets the y coordinate of a video element
 * @param[in] n The image element to set the y coordinate @ender_transfer{none}
 * @param[in] y The y coordinate to set @ender_transfer{content}
 */
EAPI void egueb_svg_element_video_y_set_simple(Egueb_Dom_Node *n, const Egueb_Svg_Coord *y)
{
	Egueb_Svg_Element_Video *thiz;

	thiz = EGUEB_SVG_ELEMENT_VIDEO(n);
	egueb_dom_attr_set(thiz->y, EGUEB_DOM_ATTR_TYPE_BASE, y);
}

/**
 * Gets the y coordinate of a video element
 * @ender_prop{y}
 * @param[in] n The image element to get the y coordinate @ender_transfer{none}
 * @param[out] y The pointer to store the y coordinate @ender_transfer{content}
 */
EAPI void egueb_svg_element_video_y_get(Egueb_Dom_Node *n, Egueb_Svg_Coord_Animated *y)
{
	Egueb_Svg_Element_Video *thiz;

	thiz = EGUEB_SVG_ELEMENT_VIDEO(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->y, y);
}

/**
 * Sets the width of a video element
 * @param[in] n The image element to set the width @ender_transfer{none}
 * @param[in] width The width to set @ender_transfer{content}
 */
EAPI void egueb_svg_element_video_width_set_simple(Egueb_Dom_Node *n, const Egueb_Svg_Length *width)
{
	Egueb_Svg_Element_Video *thiz;

	thiz = EGUEB_SVG_ELEMENT_VIDEO(n);
	egueb_dom_attr_set(thiz->width, EGUEB_DOM_ATTR_TYPE_BASE, width);
}

/**
 * Gets the width of a video element
 * @ender_prop{width}
 * @param[in] n The image element to get the width @ender_transfer{none}
 * @param[out] width The pointer to store the width @ender_transfer{content}
 */
EAPI void egueb_svg_element_video_width_get(Egueb_Dom_Node *n, Egueb_Svg_Length_Animated *width)
{
	Egueb_Svg_Element_Video *thiz;

	thiz = EGUEB_SVG_ELEMENT_VIDEO(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->width, width);
}

/**
 * Sets the height of a video element
 * @param[in] n The image element to set the height @ender_transfer{none}
 * @param[in] height The height to set @ender_transfer{content}
 */
EAPI void egueb_svg_element_video_height_set_simple(Egueb_Dom_Node *n, const Egueb_Svg_Length *height)
{
	Egueb_Svg_Element_Video *thiz;

	thiz = EGUEB_SVG_ELEMENT_VIDEO(n);
	egueb_dom_attr_set(thiz->height, EGUEB_DOM_ATTR_TYPE_BASE, height);
}

/**
 * Gets the height of a video element
 * @ender_prop{height}
 * @param[in] n The image element to get the height @ender_transfer{none}
 * @param[out] height The pointer to store the height @ender_transfer{content}
 */
EAPI void egueb_svg_element_video_height_get(Egueb_Dom_Node *n, Egueb_Svg_Length_Animated *height)
{
	Egueb_Svg_Element_Video *thiz;

	thiz = EGUEB_SVG_ELEMENT_VIDEO(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->height, height);
}

/**
 * Sets the href of a video element
 * @param[in] n The image element to get the href @ender_transfer{none}
 * @param[out] v The href to set @ender_transfer{full}
 */
EAPI void egueb_svg_element_video_href_set_simple(Egueb_Dom_Node *n, Egueb_Dom_String *v)
{
	Egueb_Svg_Element_Video *thiz;

	thiz = EGUEB_SVG_ELEMENT_VIDEO(n);
	egueb_dom_attr_set(thiz->xlink_href, EGUEB_DOM_ATTR_TYPE_BASE, v);
}

/**
 * Gets the href of a video element
 * @ender_prop{href}
 * @param[in] n The image element to get the href @ender_transfer{none}
 * @param[out] v The pointer to store the href @ender_transfer{content}
 */
EAPI void egueb_svg_element_video_href_get(Egueb_Dom_Node *n, Egueb_Svg_String_Animated *v)
{
	Egueb_Svg_Element_Video *thiz;

	thiz = EGUEB_SVG_ELEMENT_VIDEO(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->xlink_href, v);
}
