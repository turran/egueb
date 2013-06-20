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
#include "egueb_svg_element_image.h"
#include "egueb_svg_document.h"
#include "egueb_svg_attr_string.h"
#include "egueb_svg_renderable_private.h"
/* TODO
 * to handle svg images we need to:
 * 1. make svg be able to load svgs. we need this to use the descriptor
 * functions set on the svg to actually load a local/remote file
 * 2. add on the svg functions to add external image svgs that will be
 * processed on the damages and on the draw
 * 3. the svg_image_add needs to also pass a callback to be called
 * whenever such svg is going to be processed for damages and for drawing
 * 4. whenever something changes on the image (file, size, etc) destroy
 * such external svg
 * 5. the damages are always relative to the svg, so we need to transform
 * them to the destination svg (apply the transformation matrix the object
 * currently has)
 */
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
	char *real_href;
	Enesim_Renderer *r;
	Enesim_Renderer *rectangle;
	Enesim_Renderer *image;
	Enesim_Renderer *checker;
	Enesim_Surface *s;
} Egueb_Svg_Element_Image;

typedef struct _Egueb_Svg_Element_Image_Class
{
	Egueb_Svg_Renderable_Class base;
} Egueb_Svg_Element_Image_Class;


/*----------------------------------------------------------------------------*
 *                           Image load descriptor                            *
 *----------------------------------------------------------------------------*/
static void _egueb_svg_element_image_loaded(Enesim_Surface *s,
		void *user_data)
{
	ERR("Image loaded");
}

static void _egueb_svg_element_image_failed(void *user_data)
{
	ERR("Image failed");
}

static Egueb_Svg_Document_Image_Load_Descriptor _image_data_load_descriptor = {
	/* .loaded = */ _egueb_svg_element_image_loaded,
	/* .failed = */ _egueb_svg_element_image_failed,
};
/*----------------------------------------------------------------------------*
 *                            Uri fetch descriptor                            *
 *----------------------------------------------------------------------------*/
static void _egueb_svg_element_uri_fetched(Enesim_Image_Data *data,
		void *user_data)
{
	ERR("Uri fetched");
}

static void _egueb_svg_element_uri_failed(void *user_data)
{
	ERR("Uri failed");
}

static Egueb_Svg_Document_Uri_Fetch_Descriptor _image_uri_fetch_descriptor = {
	/* .loaded = */ _egueb_svg_element_uri_fetched,
	/* .failed = */ _egueb_svg_element_uri_failed,
};

static void _egueb_svg_element_image_uri_load(Egueb_Svg_Element_Image *thiz,
		Egueb_Dom_Node *doc, Egueb_Dom_String * uri)
{
	const char *str;

	str = egueb_dom_string_string_get(uri);
	/* check if the data is inlined, if so, just let the document
	 * load it
	 */
	if (!strncmp(str, "data:image", 10))
	{
		Enesim_Image_Data *base_64;
		Enesim_Image_Data *data;

		base_64 = enesim_image_data_buffer_new(str, strlen(str));
		data = enesim_image_data_base64_new(base_64);
		egueb_svg_document_image_data_load(doc, data,
				&_image_data_load_descriptor,
				egueb_dom_node_ref(EGUEB_DOM_NODE(thiz)));
	}
	/* otherwise, let the document get the data and call our own callback */
	/* this callback should again make the document load the image for us */
	else
	{
		egueb_svg_document_uri_fetch(doc, egueb_dom_string_ref(uri),
				&_image_uri_fetch_descriptor,
				egueb_dom_node_ref(EGUEB_DOM_NODE(thiz)));
	}
}

#if 0
static void _egueb_svg_element_image_svg_load(void)
{
	if (_egueb_svg_element_svg_image_is_svg(name))
	{
		Egueb_Svg_Element_Svg_Image *image;
		Ender_Element *e;
		double aw, ah;
		int w, h;

		e = egueb_svg_parser_load(name, NULL, NULL);
		if (!e) return;

		/* set the container size */
		egueb_svg_element_svg_container_width_set(e, width);
		egueb_svg_element_svg_container_height_set(e, height);
		/* create a surface of the desired size */
		w = ceil(width);
		h = ceil(height);
		*s = enesim_surface_new(ENESIM_FORMAT_ARGB8888, w, h);

		/* FIXME when to destroy it? the image might change
		 * the uri and surface unreffed but not the ender element
		 */
		image = calloc(1, sizeof(Egueb_Svg_Element_Svg_Image));
		image->svg = e;
		image->s = s;

		/* add the svg to the list of svgs */
		thiz->image_svgs = eina_list_append(thiz->image_svgs, image);
	}
	else
}

static void _egueb_svg_element_image_load(Egueb_Dom_Tag *t, Egueb_Svg_Element_Image *thiz, double width, double height)
{
	Enesim_Surface *s = NULL;
	Ender_Element *topmost;
	char *href;
	char *real;

	/* set up the options */
	/* FIXME this width is without the viewbox transformation, we should transform
	 * and the assign the lengths
	 */

	egueb_svg_attribute_animated_string_final_get(&thiz->href, &href);
	if (!href) goto cleanup;

	egueb_svg_element_internal_topmost_get(t, &topmost);
	if (!strncmp(href, "data:image", 10))
	{
		real = href;
	}
	else
	{
		real = egueb_svg_element_svg_uri_resolve(topmost, href);
		if (!real) goto cleanup;
	}
	/* check that the href has actually changed */
	DBG("Using real uri %s for %s", href, real);
	if (thiz->real_href)
	{
		if (!strcmp(thiz->real_href, real))
			return;
		free(thiz->real_href);
	}
	thiz->real_href = real;
	egueb_svg_element_svg_image_load(topmost, thiz->real_href, &s, width, height);
cleanup:
	if (thiz->s)
	{
		enesim_surface_unref(thiz->s);
		thiz->s = NULL;
	}
	DBG("Using the surface %p", s);
	enesim_renderer_image_src_set(thiz->image, s);
	thiz->s = s;
}

static Eina_Bool _egueb_svg_element_image_renderer_propagate(Egueb_Dom_Tag *t,
		Egueb_Svg_Context *c,
		const Egueb_Svg_Element_Context *ctx,
		const Egueb_Svg_Attribute_Presentation *attr,
		Egueb_Svg_Renderable_Context *rctx,
		Enesim_Log **error)
{
	Egueb_Svg_Element_Image *thiz;
	Egueb_Svg_Length lx, ly;
	Egueb_Svg_Length lwidth, lheight;
	double x, y;
	double width, height;

	thiz = _egueb_svg_element_image_get(t);
	/* set the position */
	egueb_svg_attribute_animated_length_final_get(&thiz->x, &lx);
	egueb_svg_attribute_animated_length_final_get(&thiz->y, &ly);
	x = egueb_svg_coord_final_get(&lx, ctx->viewbox.w, ctx->font_size);
	y = egueb_svg_coord_final_get(&ly, ctx->viewbox.h, ctx->font_size);
	/* set the size */
	egueb_svg_attribute_animated_length_final_get(&thiz->width, &lwidth);
	egueb_svg_attribute_animated_length_final_get(&thiz->height, &lheight);
	width = egueb_svg_coord_final_get(&lwidth, ctx->viewbox.w, ctx->font_size);
	height = egueb_svg_coord_final_get(&lheight, ctx->viewbox.h, ctx->font_size);

#if 0
	double iw;
	double ih;
	const Egueb_Svg_Element_Context *parent_ctx;
	Egueb_Dom_Tag *parent_t;

	/* get the final transformation of the parent and apply it to the width/height to get the image size */
	parent_t = egueb_dom_tag_parent_get(t);
	parent_ctx = egueb_svg_element_context_get(parent_t);
	/* load the image of that size */
	iw = width * parent_ctx->transform.base.xx;
	ih = height * parent_ctx->transform.base.yy;
	printf("iw = %g ih = %g\n", iw, ih);
#endif
	_egueb_svg_element_image_load(t, thiz, width, height);

	/* set the image */
	if (!thiz->s)
	{
		enesim_renderer_rectangle_x_set(thiz->rectangle, x);
		enesim_renderer_rectangle_y_set(thiz->rectangle, y);
		enesim_renderer_rectangle_width_set(thiz->rectangle, width);
		enesim_renderer_rectangle_height_set(thiz->rectangle, height);
		enesim_renderer_transformation_set(thiz->rectangle, &ctx->transform);
		enesim_renderer_proxy_proxied_set(thiz->proxy, thiz->rectangle);
	}
	else
	{
		enesim_renderer_image_x_set(thiz->image, x);
		enesim_renderer_image_y_set(thiz->image, y);
		enesim_renderer_image_width_set(thiz->image, width);
		enesim_renderer_image_height_set(thiz->image, height);
		enesim_renderer_transformation_set(thiz->image, &ctx->transform);
		enesim_renderer_proxy_proxied_set(thiz->proxy, thiz->image);
	}

	return EINA_TRUE;
}
#endif
/*----------------------------------------------------------------------------*
 *                            Renderable interface                            *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_element_image_process(
		Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Element_Image *thiz;
	Egueb_Svg_Element *e, *e_parent;
	Egueb_Svg_Length x, y, rx, ry, w, h;
	Egueb_Dom_String *uri;
	Egueb_Dom_Node *relative, *doc;
	Eina_Bool rx_set, ry_set;
	double font_size;

	thiz = EGUEB_SVG_ELEMENT_IMAGE(r);
	egueb_dom_attr_final_get(thiz->x, &x);
	egueb_dom_attr_final_get(thiz->y, &y);
	egueb_dom_attr_final_get(thiz->width, &w);
	egueb_dom_attr_final_get(thiz->height, &h);
	egueb_dom_attr_final_get(thiz->xlink_href, &uri);

	if (!egueb_dom_string_is_valid(uri)) {
		WARN("No uri set, nothing to do");
		return EINA_FALSE;
	}

	/* calculate the real size */
	egueb_svg_element_geometry_relative_get(EGUEB_DOM_NODE(r), &relative);
	if (!relative)
	{
		WARN("No relative available");
		egueb_dom_string_unref(uri);
		return EINA_FALSE;
	}
	egueb_dom_node_document_get(EGUEB_DOM_NODE(r), &doc);
	if (!doc)
	{
		WARN("No document set");
		egueb_dom_node_unref(relative);
		egueb_dom_string_unref(uri);
		return EINA_FALSE;
	}

	e_parent = EGUEB_SVG_ELEMENT(relative);
	egueb_svg_document_font_size_get(doc, &font_size);

	thiz->gx = egueb_svg_coord_final_get(&x, e_parent->viewbox.w, font_size);
	thiz->gy = egueb_svg_coord_final_get(&y, e_parent->viewbox.h, font_size);
	thiz->gw = egueb_svg_coord_final_get(&w, e_parent->viewbox.w, font_size);
	thiz->gh = egueb_svg_coord_final_get(&h, e_parent->viewbox.h, font_size);
	/* TODO be sure to know if the xlink has changed */
	_egueb_svg_element_image_uri_load(thiz, doc, uri);

	egueb_dom_node_unref(relative);
	egueb_dom_node_unref(doc);
	egueb_dom_string_unref(uri);

	e = EGUEB_SVG_ELEMENT(r);
	/* set the image */
	if (!thiz->s)
	{
		enesim_renderer_rectangle_x_set(thiz->rectangle, thiz->gx);
		enesim_renderer_rectangle_y_set(thiz->rectangle, thiz->gy);
		enesim_renderer_rectangle_width_set(thiz->rectangle, thiz->gw);
		enesim_renderer_rectangle_height_set(thiz->rectangle, thiz->gh);
		enesim_renderer_transformation_set(thiz->rectangle, &e->transform);
		enesim_renderer_proxy_proxied_set(thiz->r, thiz->rectangle);
	}
	else
	{
		enesim_renderer_image_x_set(thiz->image, thiz->gx);
		enesim_renderer_image_y_set(thiz->image, thiz->gy);
		enesim_renderer_image_width_set(thiz->image, thiz->gw);
		enesim_renderer_image_height_set(thiz->image, thiz->gh);
		enesim_renderer_transformation_set(thiz->image, &e->transform);
		enesim_renderer_proxy_proxied_set(thiz->r, thiz->image);
	}
	return EINA_TRUE;
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
	return egueb_dom_string_ref(EGUEB_SVG_NAME_RECT);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_image, Egueb_Svg_Element_Image, x);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_image, Egueb_Svg_Element_Image, y);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_image, Egueb_Svg_Element_Image, width);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_image, Egueb_Svg_Element_Image, height);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_image, Egueb_Svg_Element_Image, xlink_href);

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

static void _egueb_svg_element_image_class_deinit(void *k)
{
}

static void _egueb_svg_element_image_instance_init(void *o)
{
	Egueb_Svg_Element_Image *thiz;
	Enesim_Renderer *r;

	thiz = EGUEB_SVG_ELEMENT_IMAGE(o);

	r = enesim_renderer_image_new();
	enesim_renderer_rop_set(r, ENESIM_BLEND);
	thiz->image = r;

	r = enesim_renderer_checker_new();
	enesim_renderer_checker_width_set(r, 10);
	enesim_renderer_checker_height_set(r, 10);
	enesim_renderer_checker_even_color_set(r, 0xff00ffff);
	enesim_renderer_checker_odd_color_set(r, 0xff00ff00);
	thiz->checker = r;

	r = enesim_renderer_rectangle_new();
	enesim_renderer_shape_fill_renderer_set(r, thiz->checker);
	enesim_renderer_rop_set(r, ENESIM_BLEND);
	thiz->rectangle = r;

	r = enesim_renderer_proxy_new();
	thiz->r = r;

	/* Default values */
	enesim_renderer_rop_set(thiz->r, ENESIM_BLEND);

	/* create the properties */
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
			&EGUEB_SVG_LENGTH_0,
			EINA_TRUE, EINA_FALSE, EINA_FALSE);
	thiz->height = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_HEIGHT),
			&EGUEB_SVG_LENGTH_0,
			EINA_TRUE, EINA_FALSE, EINA_FALSE);
	thiz->xlink_href = egueb_svg_attr_string_new(
			egueb_dom_string_ref(EGUEB_SVG_XLINK_HREF),
			NULL, EINA_TRUE, EINA_FALSE, EINA_FALSE);

	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_image, x);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_image, y);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_image, width);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_image, height);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_image, xlink_href);
}

static void _egueb_svg_element_image_instance_deinit(void *o)
{
	Egueb_Svg_Element_Image *thiz;

	thiz = EGUEB_SVG_ELEMENT_IMAGE(o);
	enesim_renderer_unref(thiz->r);
	/* destroy the properties */
	egueb_dom_node_unref(thiz->x);
	egueb_dom_node_unref(thiz->y);
	egueb_dom_node_unref(thiz->width);
	egueb_dom_node_unref(thiz->height);
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
#if 0
void egueb_svg_element_image_damage_add(Egueb_Dom_Tag *t, Eina_Rectangle *area)
{
	Egueb_Svg_Element_Image *thiz;

	thiz = _egueb_svg_element_image_get(t);
	enesim_renderer_image_damage_add(thiz->image, area);
}
#endif
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_svg_element_image_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_element_image);
	return n;
}

#if 0
EAPI Ender_Element * egueb_svg_element_image_new(void)
{
	return ESVG_ELEMENT_NEW("SVGImageElement");
}

EAPI Eina_Bool egueb_svg_is_image(Ender_Element *e)
{
	Egueb_Dom_Tag *t;
	Egueb_Svg_Type type;

	t = (Egueb_Dom_Tag *)ender_element_object_get(e);
	type = egueb_svg_element_internal_type_get(t);
	return (type == ESVG_TYPE_IMAGE) ? EINA_TRUE : EINA_FALSE;
}

EAPI void egueb_svg_element_image_x_set(Ender_Element *e, const Egueb_Svg_Coord *x)
{
	egueb_svg_element_property_length_set(e, ESVG_ELEMENT_IMAGE_X, x);
}

EAPI void egueb_svg_element_image_x_get(Ender_Element *e, Egueb_Svg_Coord *x)
{
}

EAPI void egueb_svg_element_image_y_set(Ender_Element *e, const Egueb_Svg_Coord *y)
{
	egueb_svg_element_property_length_set(e, ESVG_ELEMENT_IMAGE_Y, y);
}

EAPI void egueb_svg_element_image_y_get(Ender_Element *e, Egueb_Svg_Coord *y)
{
}

EAPI void egueb_svg_element_image_width_set(Ender_Element *e, const Egueb_Svg_Length *width)
{
	egueb_svg_element_property_length_set(e, ESVG_ELEMENT_IMAGE_WIDTH, width);
}

EAPI void egueb_svg_element_image_width_get(Ender_Element *e, Egueb_Svg_Length *width)
{
}

EAPI void egueb_svg_element_image_height_set(Ender_Element *e, const Egueb_Svg_Length *height)
{
	egueb_svg_element_property_length_set(e, ESVG_ELEMENT_IMAGE_HEIGHT, height);
}

EAPI void egueb_svg_element_image_height_get(Ender_Element *e, Egueb_Svg_Length *height)
{
}

EAPI void egueb_svg_element_image_xlink_href_set(Ender_Element *e, const char *href)
{
	egueb_svg_element_property_string_set(e, ESVG_ELEMENT_IMAGE_XLINK_HREF, href);
}

EAPI void egueb_svg_element_image_xlink_href_get(Ender_Element *e, const char **href)
{
}
#endif
