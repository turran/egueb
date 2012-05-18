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
#include "esvg_image.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Ender_Property *ESVG_IMAGE_X;
static Ender_Property *ESVG_IMAGE_Y;
static Ender_Property *ESVG_IMAGE_RX;
static Ender_Property *ESVG_IMAGE_RY;
static Ender_Property *ESVG_IMAGE_WIDTH;
static Ender_Property *ESVG_IMAGE_HEIGHT;
static Ender_Property *ESVG_IMAGE_HREF;

typedef struct _Esvg_Image_State
{
	Esvg_Coord x;
	Esvg_Coord y;
	Esvg_Length width;
	Esvg_Length height;
} Esvg_Image_State;

typedef struct _Esvg_Image
{
	EINA_MAGIC
	/* properties */
	Esvg_Image_State current;
	Esvg_Image_State past;
	char *href;
	char *real_href;
	/* private */
	Enesim_Renderer *proxy;
	Enesim_Renderer *rectangle;
	Enesim_Renderer *image;
	Enesim_Renderer *checker;
	Enesim_Surface *s;
	Eina_Bool changed : 1;
} Esvg_Image;

static Esvg_Image * _esvg_image_get(Edom_Tag *t)
{
	Esvg_Image *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_IMAGE)
		return NULL;
	thiz = esvg_instantiable_data_get(t);
	return thiz;
}

#if 0
static Eina_Bool _esvg_image_svg_load(Edom_Tag *t, Esvg_Image *thiz, double width, double height)
{
	Ender_Element *e;

	e = esvg_parser_load(thiz->real_href);
	if (!e) return EINA_FALSE;

	/* FIXME if it is not a svg, then just abort it? */
	/* we need the main svg element also process and add the etch to the list of etches, etc */
	/* when to render? whenever the renderable is being rendered? how to link both rendering process? */
	/* does the image element should inform the main svg that we need to render again? */
}
#endif

static void _esvg_image_load(Edom_Tag *t, Esvg_Image *thiz, double width, double height)
{
	Enesim_Surface *s = NULL;
	Ender_Element *topmost;
	char options[PATH_MAX];
	char *real;

	options[0] = '\0';
	/* set up the options */
	/* FIXME this width is without the viewbox transformation, we should transform
	 * and the assign the lengths
	 */
	if (width != 0 && height != 0)
	{
		sprintf(options, "width=%d;height=%d", (int)width, (int)height);
	}
	if (!thiz->href) goto cleanup;

	esvg_element_internal_topmost_get(t, &topmost);
	real = esvg_svg_uri_resolve (topmost, thiz->href);
	if (!real) goto cleanup;

	/* check that the href has actually changed */
	if (thiz->real_href)
	{
		if (!strcmp(thiz->real_href, real))
			return;
		free(thiz->real_href);
	}
	thiz->real_href = real;

	/* FIXME for svg files we should call the parser to create
	 * a new svg root, and our own root should process that one too
	 */
	
	/* FIXME handle async loading, for that we need someone to
	 * tick the emage thread
	 */

	esvg_svg_image_load(topmost, thiz->real_href, &s, options);
cleanup:
	if (thiz->s)
	{
		enesim_surface_unref(thiz->s);
		thiz->s = NULL;
	}
	enesim_renderer_image_src_set(thiz->image, s);
	thiz->s = s;
}

/*----------------------------------------------------------------------------*
 *                       The Esvg Renderable interface                        *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_image_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (strcmp(key, "x") == 0)
	{
		Esvg_Coord x;

		esvg_length_string_from(&x, value);
		esvg_image_x_set(e, &x);
	}
	else if (strcmp(key, "y") == 0)
	{
		Esvg_Coord y;

		esvg_length_string_from(&y, value);
		esvg_image_y_set(e, &y);
	}
	else if (strcmp(key, "width") == 0)
	{
		Esvg_Length width;

		esvg_length_string_from(&width, value);
		esvg_image_width_set(e, &width);
	}
	else if (strcmp(key, "height") == 0)
	{
		Esvg_Length height;

		esvg_length_string_from(&height, value);
		esvg_image_height_set(e, &height);
	}
	else if (strcmp(key, "xlink:href") == 0)
	{
		esvg_image_href_set(e, value);
	}

	return EINA_TRUE;
}

static Eina_Bool _esvg_image_attribute_get(Edom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static Enesim_Renderer * _esvg_image_renderer_get(Edom_Tag *t)
{
	Esvg_Image *thiz;

	thiz = _esvg_image_get(t);
	return thiz->proxy;
}

static Esvg_Element_Setup_Return _esvg_image_setup(Edom_Tag *t,
		Esvg_Context *c,
		Esvg_Element_Context *ctx,
		Esvg_Attribute_Presentation *attr,
		Enesim_Error **error)
{
	return ESVG_SETUP_OK;
}

static Eina_Bool _esvg_image_renderer_propagate(Edom_Tag *t,
		Esvg_Context *c,
		const Esvg_Element_Context *ctx,
		const Esvg_Attribute_Presentation *attr,
		Esvg_Renderable_Context *rctx,
		Enesim_Error **error)
{
	Esvg_Image *thiz;
	double x, y;
	double width, height;

	thiz = _esvg_image_get(t);
	/* set the position */
	x = esvg_length_final_get(&thiz->current.x, ctx->viewbox.width);
	y = esvg_length_final_get(&thiz->current.y, ctx->viewbox.height);
	/* set the size */
	width = esvg_length_final_get(&thiz->current.width, ctx->viewbox.width);
	height = esvg_length_final_get(&thiz->current.height, ctx->viewbox.height);

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
	_esvg_image_load(t, thiz, width, height);

	printf("calling the setup on the image (%g %g %g %g %p)\n", x, y, width, height, thiz->s);
	/* set the image */
	if (!thiz->s)
	{
		enesim_renderer_rectangle_x_set(thiz->rectangle, x);
		enesim_renderer_rectangle_y_set(thiz->rectangle, y);
		enesim_renderer_rectangle_width_set(thiz->rectangle, width);
		enesim_renderer_rectangle_height_set(thiz->rectangle, height);
		enesim_renderer_geometry_transformation_set(thiz->rectangle, &ctx->transform.base);
		enesim_renderer_proxy_proxied_set(thiz->proxy, thiz->rectangle);
	}
	else
	{
		enesim_renderer_image_x_set(thiz->image, x);
		enesim_renderer_image_y_set(thiz->image, y);
		enesim_renderer_image_width_set(thiz->image, width);
		enesim_renderer_image_height_set(thiz->image, height);
		enesim_renderer_geometry_transformation_set(thiz->image, &ctx->transform.base);
		enesim_renderer_proxy_proxied_set(thiz->proxy, thiz->image);
	}

	return EINA_TRUE;
}

static Eina_Bool _esvg_image_has_changed(Edom_Tag *t)
{
	Esvg_Image *thiz;

	thiz = _esvg_image_get(t);
	if (!thiz->changed) return EINA_FALSE;

	if (esvg_length_is_equal(&thiz->current.x, &thiz->past.x))
		return EINA_TRUE;
	if (esvg_length_is_equal(&thiz->current.y, &thiz->past.y))
		return EINA_TRUE;
	if (esvg_length_is_equal(&thiz->current.width, &thiz->past.width))
		return EINA_TRUE;
	if (esvg_length_is_equal(&thiz->current.height, &thiz->past.height))
		return EINA_TRUE;

	return EINA_FALSE;
}

static void _esvg_image_free(Edom_Tag *t)
{
	Esvg_Image *thiz;

	thiz = _esvg_image_get(t);
	free(thiz);
}


static Esvg_Instantiable_Descriptor _descriptor = {
	/* .child_add		= */ NULL,
	/* .child_remove	= */ NULL,
	/* .attribute_get 	= */ _esvg_image_attribute_get,
	/* .cdata_set 		= */ NULL,
	/* .text_set 		= */ NULL,
	/* .free 		= */ _esvg_image_free,
	/* .initialize 		= */ NULL,
	/* .attribute_set 	= */ _esvg_image_attribute_set,
	/* .clone		= */ NULL,
	/* .setup		= */ _esvg_image_setup,
	/* .renderer_get	= */ _esvg_image_renderer_get,
	/* .renderer_propagate	= */ _esvg_image_renderer_propagate,
};

/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_image_new(void)
{
	Esvg_Image *thiz;
	Edom_Tag *t;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Image));
	if (!thiz) return NULL;
	EINA_MAGIC_SET(thiz, ESVG_IMAGE_MAGIC);

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
	thiz->current.x = ESVG_COORD_0;
	thiz->current.y = ESVG_COORD_0;
	thiz->current.width = ESVG_LENGTH_0;
	thiz->current.height = ESVG_LENGTH_0;
	/* FIXME: href default value */

	t = esvg_instantiable_new(&_descriptor, ESVG_IMAGE, thiz);
	return t;
}

static void _esvg_image_x_set(Edom_Tag *t, const Esvg_Coord *x)
{
	Esvg_Image *thiz;

	thiz = _esvg_image_get(t);
	if (x)
	{
		thiz->current.x = *x;
		thiz->changed = EINA_TRUE;
	}
}

static void _esvg_image_x_get(Edom_Tag *t, Esvg_Coord *x)
{
	Esvg_Image *thiz;

	thiz = _esvg_image_get(t);
	if (x) *x = thiz->current.x;
}

static void _esvg_image_y_set(Edom_Tag *t, const Esvg_Coord *y)
{
	Esvg_Image *thiz;

	thiz = _esvg_image_get(t);
	if (y)
	{
		thiz->current.y = *y;
		thiz->changed = EINA_TRUE;
	}
}

static void _esvg_image_y_get(Edom_Tag *t, Esvg_Coord *y)
{
	Esvg_Image *thiz;

	thiz = _esvg_image_get(t);
	if (y) *y = thiz->current.y;
}

static void _esvg_image_width_set(Edom_Tag *t, const Esvg_Length *width)
{
	Esvg_Image *thiz;

	thiz = _esvg_image_get(t);
	if (width)
	{
		thiz->current.width = *width;
		thiz->changed = EINA_TRUE;
	}
}

static void _esvg_image_width_get(Edom_Tag *t, Esvg_Length *width)
{
	Esvg_Image *thiz;

	thiz = _esvg_image_get(t);
	if (width) *width = thiz->current.width;
}

static void _esvg_image_height_set(Edom_Tag *t, const Esvg_Length *height)
{
	Esvg_Image *thiz;

	thiz = _esvg_image_get(t);
	if (height)
	{
		thiz->current.height = *height;
		thiz->changed = EINA_TRUE;
	}
}

static void _esvg_image_height_get(Edom_Tag *t, Esvg_Length *height)
{
	Esvg_Image *thiz;

	thiz = _esvg_image_get(t);
	if (height) *height = thiz->current.height;
}

static void _esvg_image_href_set(Edom_Tag *t, const char *href)
{
	Esvg_Image *thiz;

	thiz = _esvg_image_get(t);
	if (href)
	{
		char *h;
		h = strdup(href);
		if (h)
		{
			thiz->href = strdup(href);
			thiz->changed = EINA_TRUE;
		}
	}
}

static void _esvg_image_href_get(Edom_Tag *t, const char **href)
{
	Esvg_Image *thiz;

	thiz = _esvg_image_get(t);
	if (href && *href) *href = thiz->href;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* The ender wrapper */
#define _esvg_image_x_is_set NULL
#define _esvg_image_y_is_set NULL
#define _esvg_image_width_is_set NULL
#define _esvg_image_height_is_set NULL
#define _esvg_image_href_is_set NULL
#include "generated/esvg_generated_image.c"
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Element * esvg_image_new(void)
{
	return ender_element_new_with_namespace("image", "esvg");
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_image_x_set(Ender_Element *e, const Esvg_Coord *x)
{
	ender_element_property_value_set(e, ESVG_IMAGE_X, x, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_image_x_get(Ender_Element *e, Esvg_Coord *x)
{

}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_image_y_set(Ender_Element *e, const Esvg_Coord *y)
{
	ender_element_property_value_set(e, ESVG_IMAGE_Y, y, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_image_y_get(Ender_Element *e, Esvg_Coord *y)
{

}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_image_width_set(Ender_Element *e, const Esvg_Length *width)
{
	ender_element_property_value_set(e, ESVG_IMAGE_WIDTH, width, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_image_width_get(Ender_Element *e, Esvg_Length *width)
{

}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_image_height_set(Ender_Element *e, const Esvg_Length *height)
{
	ender_element_property_value_set(e, ESVG_IMAGE_HEIGHT, height, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_image_height_get(Ender_Element *e, Esvg_Length *height)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_image_href_set(Ender_Element *e, const char *href)
{
	ender_element_property_value_set(e, ESVG_IMAGE_HREF, href, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_image_href_get(Ender_Element *e, const char **href)
{
}
