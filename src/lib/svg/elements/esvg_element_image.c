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
#include "esvg_private_svg.h"
#include "esvg_element_image.h"
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
#define ESVG_LOG_DEFAULT _esvg_element_image_log

static int _esvg_element_image_log = -1;

static Ender_Property *ESVG_ELEMENT_IMAGE_X;
static Ender_Property *ESVG_ELEMENT_IMAGE_Y;
static Ender_Property *ESVG_ELEMENT_IMAGE_WIDTH;
static Ender_Property *ESVG_ELEMENT_IMAGE_HEIGHT;
static Ender_Property *ESVG_ELEMENT_IMAGE_XLINK_HREF;

typedef struct _Esvg_Element_Image
{
	EINA_MAGIC
	/* properties */
	Esvg_Attribute_Animated_Coord x;
	Esvg_Attribute_Animated_Coord y;
	Esvg_Attribute_Animated_Length width;
	Esvg_Attribute_Animated_Length height;
	Esvg_Attribute_Animated_String href;
	char *real_href;
	/* private */
	Enesim_Renderer *proxy;
	Enesim_Renderer *rectangle;
	Enesim_Renderer *image;
	Enesim_Renderer *checker;
	Enesim_Surface *s;
	Eina_Bool changed : 1;
} Esvg_Element_Image;

static Esvg_Element_Image * _esvg_element_image_get(Edom_Tag *t)
{
	Esvg_Element_Image *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_TYPE_IMAGE)
		return NULL;
	thiz = esvg_renderable_data_get(t);
	return thiz;
}

static Eina_Bool _esvg_element_image_is_svg(const char *uri)
{
	char *last;

	last = strrchr(uri, '.');
	if (!last) return EINA_FALSE;
	if (!strcmp(last + 1, "svg"))
		return EINA_TRUE;
	return EINA_FALSE;
}

#if 0
static void _esvg_element_image_svg_load(void)
{
	if (_esvg_element_svg_image_is_svg(name))
	{
		Esvg_Element_Svg_Image *image;
		Ender_Element *e;
		double aw, ah;
		int w, h;

		e = esvg_parser_load(name, NULL, NULL);
		if (!e) return;

		/* set the container size */
		esvg_element_svg_container_width_set(e, width);
		esvg_element_svg_container_height_set(e, height);
		/* create a surface of the desired size */
		w = ceil(width);
		h = ceil(height);
		*s = enesim_surface_new(ENESIM_FORMAT_ARGB8888, w, h);

		/* FIXME when to destroy it? the image might change
		 * the uri and surface unreffed but not the ender element
		 */
		image = calloc(1, sizeof(Esvg_Element_Svg_Image));
		image->svg = e;
		image->s = s;

		/* add the svg to the list of svgs */
		thiz->image_svgs = eina_list_append(thiz->image_svgs, image);
	}
	else
}
#endif

static void _esvg_element_image_load(Edom_Tag *t, Esvg_Element_Image *thiz, double width, double height)
{
	Enesim_Surface *s = NULL;
	Ender_Element *topmost;
	char *href;
	char *real;

	/* set up the options */
	/* FIXME this width is without the viewbox transformation, we should transform
	 * and the assign the lengths
	 */

	esvg_attribute_animated_string_final_get(&thiz->href, &href);
	if (!href) goto cleanup;

	esvg_element_internal_topmost_get(t, &topmost);
	if (!strncmp(href, "data:image", 10))
	{
		real = href;
	}
	else
	{
		real = esvg_element_svg_uri_resolve(topmost, href);
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
	esvg_element_svg_image_load(topmost, thiz->real_href, &s, width, height);
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

/*----------------------------------------------------------------------------*
 *                       The Esvg Renderable interface                        *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_element_image_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (strcmp(key, "x") == 0)
	{
		Esvg_Coord x;

		esvg_length_string_from(&x, value);
		esvg_element_image_x_set(e, &x);
	}
	else if (strcmp(key, "y") == 0)
	{
		Esvg_Coord y;

		esvg_length_string_from(&y, value);
		esvg_element_image_y_set(e, &y);
	}
	else if (strcmp(key, "width") == 0)
	{
		Esvg_Length width;

		esvg_length_string_from(&width, value);
		esvg_element_image_width_set(e, &width);
	}
	else if (strcmp(key, "height") == 0)
	{
		Esvg_Length height;

		esvg_length_string_from(&height, value);
		esvg_element_image_height_set(e, &height);
	}
	else if (strcmp(key, "xlink:href") == 0)
	{
		esvg_element_image_xlink_href_set(e, value);
	}
	else
	{
		return EINA_FALSE;
	}

	return EINA_TRUE;
}

static Eina_Bool _esvg_element_image_attribute_get(Edom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static int * _esvg_element_image_attribute_animated_fetch(Edom_Tag *t, const char *attr)
{
	Esvg_Element_Image *thiz;
	int *animated = NULL;

	thiz = _esvg_element_image_get(t);
	if (!strcmp(attr, "x"))
	{
		animated = &thiz->x.animated;
	}
	else if (!strcmp(attr, "y"))
	{
		animated = &thiz->y.animated;
	}
	else if (!strcmp(attr, "width"))
	{
		animated = &thiz->width.animated;
	}
	else if (!strcmp(attr, "height"))
	{
		animated = &thiz->height.animated;
	}
	else if (!strcmp(attr, "xlink:href"))
		animated = &thiz->href.animated;
	return animated;
}

static Enesim_Renderer * _esvg_element_image_renderer_get(Edom_Tag *t)
{
	Esvg_Element_Image *thiz;

	thiz = _esvg_element_image_get(t);
	return thiz->proxy;
}

static Esvg_Element_Setup_Return _esvg_element_image_setup(Edom_Tag *t,
		Esvg_Context *c,
		Esvg_Element_Context *ctx,
		Esvg_Attribute_Presentation *attr,
		Enesim_Error **error)
{
	return ESVG_SETUP_OK;
}

static Eina_Bool _esvg_element_image_renderer_propagate(Edom_Tag *t,
		Esvg_Context *c,
		const Esvg_Element_Context *ctx,
		const Esvg_Attribute_Presentation *attr,
		Esvg_Renderable_Context *rctx,
		Enesim_Error **error)
{
	Esvg_Element_Image *thiz;
	Esvg_Length lx, ly;
	Esvg_Length lwidth, lheight;
	double x, y;
	double width, height;

	thiz = _esvg_element_image_get(t);
	/* set the position */
	esvg_attribute_animated_length_final_get(&thiz->x, &lx);
	esvg_attribute_animated_length_final_get(&thiz->y, &ly);
	x = esvg_coord_final_get(&lx, ctx->viewbox.w, ctx->font_size);
	y = esvg_coord_final_get(&ly, ctx->viewbox.h, ctx->font_size);
	/* set the size */
	esvg_attribute_animated_length_final_get(&thiz->width, &lwidth);
	esvg_attribute_animated_length_final_get(&thiz->height, &lheight);
	width = esvg_coord_final_get(&lwidth, ctx->viewbox.w, ctx->font_size);
	height = esvg_coord_final_get(&lheight, ctx->viewbox.h, ctx->font_size);

#if 0
	double iw;
	double ih;
	const Esvg_Element_Context *parent_ctx;
	Edom_Tag *parent_t;

	/* get the final transformation of the parent and apply it to the width/height to get the image size */
	parent_t = edom_tag_parent_get(t);
	parent_ctx = esvg_element_context_get(parent_t);
	/* load the image of that size */
	iw = width * parent_ctx->transform.base.xx;
	ih = height * parent_ctx->transform.base.yy;
	printf("iw = %g ih = %g\n", iw, ih);
#endif
	_esvg_element_image_load(t, thiz, width, height);

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

static void _esvg_element_image_free(Edom_Tag *t)
{
	Esvg_Element_Image *thiz;

	thiz = _esvg_element_image_get(t);
	free(thiz);
}

#if 0
static Eina_Bool _esvg_element_image_has_changed(Edom_Tag *t)
{
	Esvg_Element_Image *thiz;

	thiz = _esvg_element_image_get(t);
	if (!thiz->changed) return EINA_FALSE;

	if (esvg_length_is_equal(&thiz->x, &thiz->past.x))
		return EINA_TRUE;
	if (esvg_length_is_equal(&thiz->y, &thiz->past.y))
		return EINA_TRUE;
	if (esvg_length_is_equal(&thiz->width, &thiz->past.width))
		return EINA_TRUE;
	if (esvg_length_is_equal(&thiz->height, &thiz->past.height))
		return EINA_TRUE;

	return EINA_FALSE;
}
#endif

static Esvg_Renderable_Descriptor _descriptor = {
	/* .child_add		     = */ NULL,
	/* .child_remove	     = */ NULL,
	/* .attribute_get 	     = */ _esvg_element_image_attribute_get,
	/* .cdata_set 		     = */ NULL,
	/* .text_set 		     = */ NULL,
	/* .text_get 		     = */ NULL,
	/* .free 		     = */ _esvg_element_image_free,
	/* .initialize 		     = */ NULL,
	/* .attribute_set 	     = */ _esvg_element_image_attribute_set,
	/* .attribute_animated_fetch = */ _esvg_element_image_attribute_animated_fetch,
	/* .setup		     = */ _esvg_element_image_setup,
	/* .renderer_get	     = */ _esvg_element_image_renderer_get,
	/* .renderer_propagate	     = */ _esvg_element_image_renderer_propagate,
};

/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_element_image_new(void)
{
	Esvg_Element_Image *thiz;
	Edom_Tag *t;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Element_Image));
	if (!thiz) return NULL;
	EINA_MAGIC_SET(thiz, ESVG_ELEMENT_IMAGE_MAGIC);

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
	thiz->proxy = r;

	/* Default values */
	thiz->x.base.v = thiz->x.anim.v = ESVG_COORD_0;
	thiz->y.base.v = thiz->y.anim.v = ESVG_COORD_0;
	thiz->width.base.v = thiz->width.anim.v = ESVG_LENGTH_0;
	thiz->height.base.v = thiz->height.anim.v = ESVG_LENGTH_0;
	/* FIXME: href default value */

	t = esvg_renderable_new(&_descriptor, ESVG_TYPE_IMAGE, thiz);
	return t;
}

static void _esvg_element_image_x_set(Edom_Tag *t, const Esvg_Animated_Coord *x)
{
	Esvg_Element_Image *thiz;
	Esvg_Length def = { ESVG_UNIT_LENGTH_PX, 0 };
	Eina_Bool animating;

	thiz = _esvg_element_image_get(t);
	animating = esvg_element_attribute_animate_get(t);
	esvg_attribute_animated_length_set(&thiz->x, x, &def, animating);
	thiz->changed = EINA_TRUE;
}

static void _esvg_element_image_x_get(Edom_Tag *t, Esvg_Animated_Coord *x)
{
	Esvg_Element_Image *thiz;

	thiz = _esvg_element_image_get(t);
	esvg_attribute_animated_length_get(&thiz->x, x);
}

static void _esvg_element_image_y_set(Edom_Tag *t, const Esvg_Animated_Coord *y)
{
	Esvg_Element_Image *thiz;
	Esvg_Length def = { ESVG_UNIT_LENGTH_PX, 0 };
	Eina_Bool animating;

	thiz = _esvg_element_image_get(t);
	animating = esvg_element_attribute_animate_get(t);
	esvg_attribute_animated_length_set(&thiz->y, y, &def, animating);
	thiz->changed = EINA_TRUE;
}

static void _esvg_element_image_y_get(Edom_Tag *t, Esvg_Animated_Coord *y)
{
	Esvg_Element_Image *thiz;

	thiz = _esvg_element_image_get(t);
	esvg_attribute_animated_length_get(&thiz->y, y);
}

static void _esvg_element_image_width_set(Edom_Tag *t, const Esvg_Length_Animated *width)
{
	Esvg_Element_Image *thiz;
	Esvg_Length def = { ESVG_UNIT_LENGTH_PX, 0 };
	Eina_Bool animating;

	thiz = _esvg_element_image_get(t);
	animating = esvg_element_attribute_animate_get(t);
	esvg_attribute_animated_length_set(&thiz->width, width, &def, animating);
	thiz->changed = EINA_TRUE;
}

static void _esvg_element_image_width_get(Edom_Tag *t, Esvg_Length_Animated *width)
{
	Esvg_Element_Image *thiz;

	thiz = _esvg_element_image_get(t);
	esvg_attribute_animated_length_get(&thiz->width, width);
}

static void _esvg_element_image_height_set(Edom_Tag *t, const Esvg_Length_Animated *height)
{
	Esvg_Element_Image *thiz;
	Esvg_Length def = { ESVG_UNIT_LENGTH_PX, 0 };
	Eina_Bool animating;

	thiz = _esvg_element_image_get(t);
	animating = esvg_element_attribute_animate_get(t);
	esvg_attribute_animated_length_set(&thiz->height, height, &def, animating);
	thiz->changed = EINA_TRUE;
}

static void _esvg_element_image_height_get(Edom_Tag *t, Esvg_Length_Animated *height)
{
	Esvg_Element_Image *thiz;

	thiz = _esvg_element_image_get(t);
	esvg_attribute_animated_length_get(&thiz->height, height);
}

static void _esvg_element_image_xlink_href_set(Edom_Tag *t, Esvg_String_Animated *href)
{
	Esvg_Element_Image *thiz;
	Eina_Bool animating;

	thiz = _esvg_element_image_get(t);
	animating = esvg_element_attribute_animate_get(t);
	esvg_attribute_animated_string_set(&thiz->href,
		href, animating);
}

static void _esvg_element_image_xlink_href_get(Edom_Tag *t, Esvg_String_Animated *href)
{
	Esvg_Element_Image *thiz;

	thiz = _esvg_element_image_get(t);
	esvg_attribute_animated_string_get(&thiz->href,
		href);
}

/* The ender wrapper */
#define _esvg_element_image_delete NULL
#define _esvg_element_image_x_is_set NULL
#define _esvg_element_image_y_is_set NULL
#define _esvg_element_image_width_is_set NULL
#define _esvg_element_image_height_is_set NULL
#define _esvg_element_image_xlink_href_is_set NULL
#include "esvg_generated_element_image.c"

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_element_image_init(void)
{
	_esvg_element_image_log = eina_log_domain_register("esvg_element_image", ESVG_LOG_COLOR_DEFAULT);
	if (_esvg_element_image_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_esvg_element_image_init();
}

void esvg_element_image_shutdown(void)
{
	if (_esvg_element_image_log < 0)
		return;
	_esvg_element_image_shutdown();
	eina_log_domain_unregister(_esvg_element_image_log);
	_esvg_element_image_log = -1;
}

#if 0
void esvg_element_image_damage_add(Edom_Tag *t, Eina_Rectangle *area)
{
	Esvg_Element_Image *thiz;

	thiz = _esvg_element_image_get(t);
	enesim_renderer_image_damage_add(thiz->image, area);
}
#endif
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * esvg_element_image_new(void)
{
	return ESVG_ELEMENT_NEW("SVGImageElement");
}

EAPI Eina_Bool esvg_is_image(Ender_Element *e)
{
	Edom_Tag *t;
	Esvg_Type type;

	t = (Edom_Tag *)ender_element_object_get(e);
	type = esvg_element_internal_type_get(t);
	return (type == ESVG_TYPE_IMAGE) ? EINA_TRUE : EINA_FALSE;
}

EAPI void esvg_element_image_x_set(Ender_Element *e, const Esvg_Coord *x)
{
	esvg_element_property_length_set(e, ESVG_ELEMENT_IMAGE_X, x);
}

EAPI void esvg_element_image_x_get(Ender_Element *e, Esvg_Coord *x)
{
}

EAPI void esvg_element_image_y_set(Ender_Element *e, const Esvg_Coord *y)
{
	esvg_element_property_length_set(e, ESVG_ELEMENT_IMAGE_Y, y);
}

EAPI void esvg_element_image_y_get(Ender_Element *e, Esvg_Coord *y)
{
}

EAPI void esvg_element_image_width_set(Ender_Element *e, const Esvg_Length *width)
{
	esvg_element_property_length_set(e, ESVG_ELEMENT_IMAGE_WIDTH, width);
}

EAPI void esvg_element_image_width_get(Ender_Element *e, Esvg_Length *width)
{
}

EAPI void esvg_element_image_height_set(Ender_Element *e, const Esvg_Length *height)
{
	esvg_element_property_length_set(e, ESVG_ELEMENT_IMAGE_HEIGHT, height);
}

EAPI void esvg_element_image_height_get(Ender_Element *e, Esvg_Length *height)
{
}

EAPI void esvg_element_image_xlink_href_set(Ender_Element *e, const char *href)
{
	esvg_element_property_string_set(e, ESVG_ELEMENT_IMAGE_XLINK_HREF, href);
}

EAPI void esvg_element_image_xlink_href_get(Ender_Element *e, const char **href)
{
}
