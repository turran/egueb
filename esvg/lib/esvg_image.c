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
	char *href;
} Esvg_Image_State;

typedef struct _Esvg_Image
{
	EINA_MAGIC
	/* properties */
	Esvg_Image_State current;
	Esvg_Image_State past;
	/* private */
	Enesim_Renderer *r;
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

static void _esvg_image_load(Esvg_Image *thiz, double width, double height)
{
	Enesim_Surface *s = NULL;
	Eina_Bool ret;
	char options[PATH_MAX];

	options[0] = '\0';
	/* set up the options */
	if (width != 0 && height != 0)
	{
		sprintf(options, "width=%d;height=%d", (int)width, (int)height);
	}
	/* FIXME handle async loading, for that we need someone to
	 * tick the emage thread
	 */
	if (thiz->s)
	{
		enesim_surface_unref(thiz->s);
		thiz->s = NULL;
	}

	printf("loading!!\n");
	ret = emage_load(thiz->current.href, &s, ENESIM_FORMAT_ARGB8888, NULL, options);
	if (!ret)
	{
		printf("some error?\n");
	}
	thiz->s = s;
	enesim_renderer_image_src_set(thiz->image, s);
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
	else if (strcmp(key, "href") == 0)
	{
		esvg_image_href_set(e, value);
	}
#if 0
	else if (strcmp(key, "xlink:href") == 0)
	{
		/* absolute */
		if (*value == '/')
		{
			esvg_image_href_set(r, value);
		}
		/* relative */
		else
		{
			Edom_Parser *parser;
			char real[PATH_MAX];
			const char *root;

			parser = edom_tag_parser_get(tag);
			root = edom_parser_root_get(parser);
			strcpy(real, root);
			strcat(real, value);
			esvg_image_href_set(r, real);
		}
	}
#endif

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
	return thiz->r;
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

	/* load the image of that size */
	_esvg_image_load(thiz, width, height);

	printf("calling the setup on the image (%g %g %g %g)\n", x, y, width, height);
	/* set the image */
	if (!thiz->s)
	{
		enesim_renderer_clipper_content_set(thiz->r, thiz->checker);
	}
	else
	{
		int sw, sh;

		enesim_surface_size_get(thiz->s, &sw, &sh);
		enesim_renderer_image_width_set(thiz->image, sw);
		enesim_renderer_image_height_set(thiz->image, sh);
		enesim_renderer_clipper_content_set(thiz->r, thiz->image);
	}
	/* we set the origin later as it depends on the content flags */
	enesim_renderer_origin_set(thiz->r, x, y);
	enesim_renderer_clipper_width_set(thiz->r, width);
	enesim_renderer_clipper_height_set(thiz->r, height);

	return EINA_TRUE;
}

static void _esvg_image_clone(Enesim_Renderer *r, Enesim_Renderer *dr)
{

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
	if (strcmp(thiz->current.href, thiz->past.href) == 0)
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
	/* .clone		= */ _esvg_image_clone,
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
	thiz->image = r;

	r = enesim_renderer_checker_new();
	enesim_renderer_checker_width_set(r, 10);
	enesim_renderer_checker_height_set(r, 10);
	enesim_renderer_checker_even_color_set(r, 0xff00ffff);
	enesim_renderer_checker_odd_color_set(r, 0xff00ff00);
	thiz->checker = r;

	r = enesim_renderer_clipper_new();
	/* FIXME for now */
	enesim_renderer_rop_set(r, ENESIM_BLEND);
	thiz->r = r;

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
			thiz->current.href = strdup(href);
			thiz->changed = EINA_TRUE;
		}
	}
}

static void _esvg_image_href_get(Edom_Tag *t, const char **href)
{
	Esvg_Image *thiz;

	thiz = _esvg_image_get(t);
	if (href && *href) *href = thiz->current.href;
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
