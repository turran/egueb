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
#include "esvg_private_video.h"

#include "esvg_element_video.h"
#include "esvg_video_provider.h"
#include "esvg_private_video_provider.h"

/* This object tries to follow the spec found on:
 * http://www.w3.org/TR/SVGTiny12/multimedia.html#VideoElement
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT _esvg_element_video_log

static int _esvg_element_video_log = -1;

static Ender_Property *ESVG_ELEMENT_VIDEO_X;
static Ender_Property *ESVG_ELEMENT_VIDEO_Y;
static Ender_Property *ESVG_ELEMENT_VIDEO_WIDTH;
static Ender_Property *ESVG_ELEMENT_VIDEO_HEIGHT;
static Ender_Property *ESVG_ELEMENT_VIDEO_XLINK_HREF;

typedef struct _Esvg_Element_Video
{
	EINA_MAGIC
	/* properties */
	Esvg_Attribute_Animated_Coord x;
	Esvg_Attribute_Animated_Coord y;
	Esvg_Attribute_Animated_Length width;
	Esvg_Attribute_Animated_Length height;
	Esvg_Attribute_Animated_String href;
	/* generated at setup */
	double gx;
	double gy;
	char *ghref;
	/* private */
	Enesim_Renderer *image;
	Esvg_Video_Provider_Context ctx;
	Esvg_Video_Provider_Descriptor *descriptor;
	Esvg_Video_Provider *provider;
} Esvg_Element_Video;

static Esvg_Element_Video * _esvg_element_video_get(Edom_Tag *t)
{
	Esvg_Element_Video *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_ELEMENT_VIDEO)
		return NULL;
	thiz = esvg_renderable_data_get(t);
	return thiz;
}
/*----------------------------------------------------------------------------*
 *                       The Esvg Renderable interface                        *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_element_video_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (strcmp(key, "x") == 0)
	{
		Esvg_Coord x;

		esvg_length_string_from(&x, value);
		esvg_element_video_x_set(e, &x);
	}
	else if (strcmp(key, "y") == 0)
	{
		Esvg_Coord y;

		esvg_length_string_from(&y, value);
		esvg_element_video_y_set(e, &y);
	}
	else if (strcmp(key, "width") == 0)
	{
		Esvg_Length width;

		esvg_length_string_from(&width, value);
		esvg_element_video_width_set(e, &width);
	}
	else if (strcmp(key, "height") == 0)
	{
		Esvg_Length height;

		esvg_length_string_from(&height, value);
		esvg_element_video_height_set(e, &height);
	}
	else if (strcmp(key, "xlink:href") == 0)
	{
		esvg_element_video_xlink_href_set(e, value);
	}
	else
	{
		return EINA_FALSE;
	}

	return EINA_TRUE;
}

static Eina_Bool _esvg_element_video_attribute_get(Edom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static int * _esvg_element_video_attribute_animated_fetch(Edom_Tag *t, const char *attr)
{
	Esvg_Element_Video *thiz;
	int *animated = NULL;

	thiz = _esvg_element_video_get(t);
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

static Enesim_Renderer * _esvg_element_video_renderer_get(Edom_Tag *t)
{
	Esvg_Element_Video *thiz;

	thiz = _esvg_element_video_get(t);
	return thiz->image;
}

static Esvg_Element_Setup_Return _esvg_element_video_setup(Edom_Tag *t,
		Esvg_Context *c,
		Esvg_Element_Context *ctx,
		Esvg_Attribute_Presentation *attr,
		Enesim_Error **error)
{
	Esvg_Element_Video *thiz;
	Esvg_Length lx, ly;
	Esvg_Length lwidth, lheight;

	thiz = _esvg_element_video_get(t);
	/* set the position */
	esvg_attribute_animated_length_final_get(&thiz->x, &lx);
	esvg_attribute_animated_length_final_get(&thiz->y, &ly);
	thiz->gx = esvg_coord_final_get(&lx, ctx->viewbox.w, ctx->font_size);
	thiz->gy = esvg_coord_final_get(&ly, ctx->viewbox.h, ctx->font_size);
	/* set the size */
	esvg_attribute_animated_length_final_get(&thiz->width, &lwidth);
	esvg_attribute_animated_length_final_get(&thiz->height, &lheight);
	thiz->ctx.width = esvg_coord_final_get(&lwidth, ctx->viewbox.w, ctx->font_size);
	thiz->ctx.height = esvg_coord_final_get(&lheight, ctx->viewbox.h, ctx->font_size);
	/* set the href */
	esvg_attribute_animated_string_final_get(&thiz->href, &thiz->ghref);

	return ESVG_SETUP_OK;
}

static Eina_Bool _esvg_element_video_renderer_propagate(Edom_Tag *t,
		Esvg_Context *c,
		const Esvg_Element_Context *ctx,
		const Esvg_Attribute_Presentation *attr,
		Esvg_Renderable_Context *rctx,
		Enesim_Error **error)
{
	Ender_Element *topmost;
	Esvg_Element_Video *thiz;
	Enesim_Matrix inv;
	char *real;

	thiz = _esvg_element_video_get(t);

	/* set the image */
	enesim_renderer_image_x_set(thiz->image, thiz->gx);
	enesim_renderer_image_y_set(thiz->image, thiz->gy);
	enesim_renderer_image_width_set(thiz->image, thiz->ctx.width);
	enesim_renderer_image_height_set(thiz->image, thiz->ctx.height);
	enesim_matrix_inverse(&ctx->transform, &inv);
	enesim_renderer_transformation_set(thiz->image, &inv);

	/* set the pipeline attributes */
	if (!thiz->ghref) goto done;
	esvg_element_internal_topmost_get(t, &topmost);
	real = esvg_element_svg_uri_resolve(topmost, thiz->ghref);
	if (!real) goto done;

	/* check that the href has actually changed */
	if (thiz->ctx.href)
	{
		free(thiz->ctx.href);
	}
	DBG("Using real uri '%s' for '%s'", real, thiz->ghref);
	thiz->ctx.href = real;

	/* create our own provider */
	if (!thiz->provider)
	{
		Esvg_Video_Provider_Descriptor *descriptor;

		descriptor = esvg_element_svg_video_provider_descriptor_get(topmost);
		thiz->provider = esvg_video_provider_new(descriptor,
			enesim_renderer_ref(thiz->image));
		if (!thiz->provider)
		{
			ERR("No video provider found");
			goto done;
		}
		thiz->descriptor = descriptor;
	}

	/* do the setup on the provider */
	esvg_video_provider_setup(thiz->provider, &thiz->ctx);
	esvg_video_provider_play(thiz->provider);
done:
	return EINA_TRUE;
}

static void _esvg_element_video_free(Edom_Tag *t)
{
	Esvg_Element_Video *thiz;

	thiz = _esvg_element_video_get(t);
	if (thiz->provider)
	{
		esvg_video_provider_free(thiz->provider);
		thiz->provider = NULL;
	}
	free(thiz);
}

static Esvg_Renderable_Descriptor _descriptor = {
	/* .child_add		     = */ NULL,
	/* .child_remove	     = */ NULL,
	/* .attribute_get 	     = */ _esvg_element_video_attribute_get,
	/* .cdata_set 		     = */ NULL,
	/* .text_set 		     = */ NULL,
	/* .text_get 		     = */ NULL,
	/* .free 		     = */ _esvg_element_video_free,
	/* .initialize 		     = */ NULL,
	/* .attribute_set 	     = */ _esvg_element_video_attribute_set,
	/* .attribute_animated_fetch = */ _esvg_element_video_attribute_animated_fetch,
	/* .setup		     = */ _esvg_element_video_setup,
	/* .renderer_get	     = */ _esvg_element_video_renderer_get,
	/* .renderer_propagate	     = */ _esvg_element_video_renderer_propagate,
};

/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_element_video_new(void)
{
	Esvg_Element_Video *thiz;
	Edom_Tag *t;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Element_Video));
	if (!thiz) return NULL;

	r = enesim_renderer_image_new();
	enesim_renderer_rop_set(r, ENESIM_BLEND);
	thiz->image = r;

	/* Default values */
	thiz->x.base.v = thiz->x.anim.v = ESVG_COORD_0;
	thiz->y.base.v = thiz->y.anim.v = ESVG_COORD_0;
	thiz->width.base.v = thiz->width.anim.v = ESVG_LENGTH_0;
	thiz->height.base.v = thiz->height.anim.v = ESVG_LENGTH_0;
	/* FIXME: href default value */

	t = esvg_renderable_new(&_descriptor, ESVG_ELEMENT_VIDEO, thiz);

	return t;
}

static void _esvg_element_video_x_set(Edom_Tag *t, const Esvg_Animated_Coord *x)
{
	Esvg_Element_Video *thiz;
	Esvg_Length def = { ESVG_UNIT_LENGTH_PX, 0 };
	Eina_Bool animating;

	thiz = _esvg_element_video_get(t);
	animating = esvg_element_attribute_animate_get(t);
	esvg_attribute_animated_length_set(&thiz->x, x, &def, animating);
}

static void _esvg_element_video_x_get(Edom_Tag *t, Esvg_Animated_Coord *x)
{
	Esvg_Element_Video *thiz;

	thiz = _esvg_element_video_get(t);
	esvg_attribute_animated_length_get(&thiz->x, x);
}

static void _esvg_element_video_y_set(Edom_Tag *t, const Esvg_Animated_Coord *y)
{
	Esvg_Element_Video *thiz;
	Esvg_Length def = { ESVG_UNIT_LENGTH_PX, 0 };
	Eina_Bool animating;

	thiz = _esvg_element_video_get(t);
	animating = esvg_element_attribute_animate_get(t);
	esvg_attribute_animated_length_set(&thiz->y, y, &def, animating);
}

static void _esvg_element_video_y_get(Edom_Tag *t, Esvg_Animated_Coord *y)
{
	Esvg_Element_Video *thiz;

	thiz = _esvg_element_video_get(t);
	esvg_attribute_animated_length_get(&thiz->y, y);
}

static void _esvg_element_video_width_set(Edom_Tag *t, const Esvg_Length_Animated *width)
{
	Esvg_Element_Video *thiz;
	Esvg_Length def = { ESVG_UNIT_LENGTH_PX, 0 };
	Eina_Bool animating;

	thiz = _esvg_element_video_get(t);
	animating = esvg_element_attribute_animate_get(t);
	esvg_attribute_animated_length_set(&thiz->width, width, &def, animating);
}

static void _esvg_element_video_width_get(Edom_Tag *t, Esvg_Length_Animated *width)
{
	Esvg_Element_Video *thiz;

	thiz = _esvg_element_video_get(t);
	esvg_attribute_animated_length_get(&thiz->width, width);
}

static void _esvg_element_video_height_set(Edom_Tag *t, const Esvg_Length_Animated *height)
{
	Esvg_Element_Video *thiz;
	Esvg_Length def = { ESVG_UNIT_LENGTH_PX, 0 };
	Eina_Bool animating;

	thiz = _esvg_element_video_get(t);
	animating = esvg_element_attribute_animate_get(t);
	esvg_attribute_animated_length_set(&thiz->height, height, &def, animating);
}

static void _esvg_element_video_height_get(Edom_Tag *t, Esvg_Length_Animated *height)
{
	Esvg_Element_Video *thiz;

	thiz = _esvg_element_video_get(t);
	esvg_attribute_animated_length_get(&thiz->height, height);
}

static void _esvg_element_video_xlink_href_set(Edom_Tag *t, Esvg_String_Animated *href)
{
	Esvg_Element_Video *thiz;
	Eina_Bool animating;

	thiz = _esvg_element_video_get(t);
	animating = esvg_element_attribute_animate_get(t);
	esvg_attribute_animated_string_set(&thiz->href,
		href, animating);
}

static void _esvg_element_video_xlink_href_get(Edom_Tag *t, Esvg_String_Animated *href)
{
	Esvg_Element_Video *thiz;

	thiz = _esvg_element_video_get(t);
	esvg_attribute_animated_string_get(&thiz->href,
		href);
}

/* The ender wrapper */
#define _esvg_element_video_delete NULL
#define _esvg_element_video_x_is_set NULL
#define _esvg_element_video_y_is_set NULL
#define _esvg_element_video_width_is_set NULL
#define _esvg_element_video_height_is_set NULL
#define _esvg_element_video_xlink_href_is_set NULL
#include "generated/esvg_generated_video.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_element_video_init(void)
{
	_esvg_element_video_log = eina_log_domain_register("esvg_element_video", ESVG_LOG_COLOR_DEFAULT);
	if (_esvg_element_video_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_esvg_element_video_init();
}

void esvg_element_video_shutdown(void)
{
	if (_esvg_element_video_log < 0)
		return;
	_esvg_element_video_shutdown();
	eina_log_domain_unregister(_esvg_element_video_log);
	_esvg_element_video_log = -1;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * esvg_element_video_new(void)
{
	return ESVG_ELEMENT_NEW("video");
}

EAPI Eina_Bool esvg_is_video(Ender_Element *e)
{
	Edom_Tag *t;
	Esvg_Type type;

	t = (Edom_Tag *)ender_element_object_get(e);
	type = esvg_element_internal_type_get(t);
	return (type == ESVG_ELEMENT_VIDEO) ? EINA_TRUE : EINA_FALSE;
}

EAPI void esvg_element_video_x_set(Ender_Element *e, const Esvg_Coord *x)
{
	esvg_element_property_length_set(e, ESVG_ELEMENT_VIDEO_X, x);
}

EAPI void esvg_element_video_x_get(Ender_Element *e, Esvg_Coord *x)
{
}

EAPI void esvg_element_video_y_set(Ender_Element *e, const Esvg_Coord *y)
{
	esvg_element_property_length_set(e, ESVG_ELEMENT_VIDEO_Y, y);
}

EAPI void esvg_element_video_y_get(Ender_Element *e, Esvg_Coord *y)
{
}

EAPI void esvg_element_video_width_set(Ender_Element *e, const Esvg_Length *width)
{
	esvg_element_property_length_set(e, ESVG_ELEMENT_VIDEO_WIDTH, width);
}

EAPI void esvg_element_video_width_get(Ender_Element *e, Esvg_Length *width)
{
}

EAPI void esvg_element_video_height_set(Ender_Element *e, const Esvg_Length *height)
{
	esvg_element_property_length_set(e, ESVG_ELEMENT_VIDEO_HEIGHT, height);
}

EAPI void esvg_element_video_height_get(Ender_Element *e, Esvg_Length *height)
{
}

EAPI void esvg_element_video_xlink_href_set(Ender_Element *e, const char *href)
{
	esvg_element_property_string_set(e, ESVG_ELEMENT_VIDEO_XLINK_HREF, href);
}

EAPI void esvg_element_video_xlink_href_get(Ender_Element *e, const char **href)
{
}

