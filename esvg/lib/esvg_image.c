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
#include "Esvg.h"
#include "esvg_private.h"
#include "esvg_values.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_IMAGE_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_IMAGE_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_IMAGE_MAGIC);\
	} while(0)

typedef struct _Esvg_Image
{
	EINA_MAGIC
	/* properties */
	Esvg_Coord x;
	Esvg_Coord y;
	Esvg_Length width;
	Esvg_Length height;
	char *href;
	/* private */
	Enesim_Renderer *r;
	Enesim_Renderer *image;
	Enesim_Renderer *checker;
	Enesim_Surface *s;
} Esvg_Image;

static Esvg_Image * _esvg_image_get(Enesim_Renderer *r)
{
	Esvg_Image *thiz;

	thiz = esvg_element_data_get(r);
	ESVG_IMAGE_MAGIC_CHECK(thiz);
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
	ret = emage_load(thiz->href, &s, ENESIM_FORMAT_ARGB8888, NULL, options);
	if (!ret)
	{
		printf("some error?\n");
	}

	thiz->s = s;
	enesim_renderer_image_src_set(thiz->image, s);
}

/*----------------------------------------------------------------------------*
 *                         Esvg Element interface                             *
 *----------------------------------------------------------------------------*/
static const char * _esvg_image_name_get(Enesim_Renderer *r)
{
	return "image";
}

static Enesim_Renderer * _esvg_image_renderer_get(Enesim_Renderer *r,
		const Esvg_Element_State *state,
		const Esvg_Attribute_Presentation *attr)
{
	Esvg_Image *thiz;

	thiz = _esvg_image_get(r);
	return thiz->r;
}

static Eina_Bool _esvg_image_setup(Enesim_Renderer *r, Esvg_Element_State *estate,
		Esvg_Attribute_Presentation *attr,
		Enesim_Surface *s,
		Enesim_Error **error)
{
	Esvg_Image *thiz;
	double x, y;
	double width, height;

	thiz = _esvg_image_get(r);
	/* set the position */
	x = esvg_length_final_get(&thiz->x, estate->viewbox_w);
	y = esvg_length_final_get(&thiz->y, estate->viewbox_h);
	/* set the size */
	width = esvg_length_final_get(&thiz->width, estate->viewbox_w);
	height = esvg_length_final_get(&thiz->height, estate->viewbox_h);

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
		enesim_renderer_image_w_set(thiz->image, sw);
		enesim_renderer_image_h_set(thiz->image, sh);
		enesim_renderer_clipper_content_set(thiz->r, thiz->image);
	}
	/* we set the origin later as it depends on the content flags */
	enesim_renderer_origin_set(thiz->r, x, y);
	enesim_renderer_clipper_width_set(thiz->r, width);
	enesim_renderer_clipper_height_set(thiz->r, height);

	return EINA_TRUE;
}

static void _esvg_image_cleanup(Enesim_Renderer *r)
{

}

static void _esvg_image_clone(Enesim_Renderer *r, Enesim_Renderer *dr)
{

}

static Esvg_Element_Descriptor _descriptor = {
	/* .name_get =		*/ _esvg_image_name_get,
	/* .renderer_get =	*/ _esvg_image_renderer_get,
	/* .clone =		*/ _esvg_image_clone,
	/* .setup =		*/ _esvg_image_setup,
	/* .cleanup =		*/ _esvg_image_cleanup,
	/* .is_renderable = 	*/ EINA_TRUE,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Enesim_Renderer * esvg_image_new(void)
{
	Esvg_Image *thiz;
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
	thiz->x = ESVG_COORD_0;
	thiz->y = ESVG_COORD_0;
	thiz->width = ESVG_LENGTH_0;
	thiz->height = ESVG_LENGTH_0;

	r = esvg_element_new(&_descriptor, thiz);
	return r;
}

EAPI void esvg_image_x_set(Enesim_Renderer *r, const Esvg_Coord *x)
{
	Esvg_Image *thiz;

	thiz = _esvg_image_get(r);
	if (x) thiz->x = *x;
}

EAPI void esvg_image_x_get(Enesim_Renderer *r, Esvg_Coord *x)
{
	Esvg_Image *thiz;

	thiz = _esvg_image_get(r);
	if (x) *x = thiz->x;
}

EAPI void esvg_image_y_set(Enesim_Renderer *r, const Esvg_Coord *y)
{
	Esvg_Image *thiz;

	thiz = _esvg_image_get(r);
	if (y) thiz->y = *y;
}

EAPI void esvg_image_y_get(Enesim_Renderer *r, Esvg_Coord *y)
{
	Esvg_Image *thiz;

	thiz = _esvg_image_get(r);
	if (y) *y = thiz->y;
}

EAPI void esvg_image_width_set(Enesim_Renderer *r, const Esvg_Length *width)
{
	Esvg_Image *thiz;

	thiz = _esvg_image_get(r);
	if (width) thiz->width = *width;
}

EAPI void esvg_image_width_get(Enesim_Renderer *r, Esvg_Length *width)
{
	Esvg_Image *thiz;

	thiz = _esvg_image_get(r);
	if (width) *width = thiz->width;
}

EAPI void esvg_image_height_set(Enesim_Renderer *r, const Esvg_Length *height)
{
	Esvg_Image *thiz;

	thiz = _esvg_image_get(r);
	if (height) thiz->height = *height;
}

EAPI void esvg_image_height_get(Enesim_Renderer *r, Esvg_Length *height)
{
	Esvg_Image *thiz;

	thiz = _esvg_image_get(r);
	if (height) *height = thiz->height;
}

EAPI void esvg_image_href_set(Enesim_Renderer *r, const char *href)
{
	Esvg_Image *thiz;

	thiz = _esvg_image_get(r);
	if (href) thiz->href = strdup(href);
}

EAPI void esvg_image_href_get(Enesim_Renderer *r, const char **href)
{
	Esvg_Image *thiz;

	thiz = _esvg_image_get(r);
	if (href && *href) *href = thiz->href;
}
