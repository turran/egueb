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
#include "egueb_svg_private_attribute_presentation.h"
#include "egueb_svg_context_private.h"
#include "egueb_svg_element_private.h"
#include "egueb_svg_renderable_private.h"
#include "egueb_svg_element_svg_private.h"
#include "egueb_svg_element_video_private.h"

#include "egueb_svg_element_video.h"
#include "egueb_svg_video_provider.h"
#include "egueb_svg_video_provider_private.h"

/* This object tries to follow the spec found on:
 * http://www.w3.org/TR/SVGTiny12/multimedia.html#VideoElement
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT _egueb_svg_element_video_log

static int _egueb_svg_element_video_log = -1;

static Ender_Property *ESVG_ELEMENT_VIDEO_X;
static Ender_Property *ESVG_ELEMENT_VIDEO_Y;
static Ender_Property *ESVG_ELEMENT_VIDEO_WIDTH;
static Ender_Property *ESVG_ELEMENT_VIDEO_HEIGHT;
static Ender_Property *ESVG_ELEMENT_VIDEO_XLINK_HREF;

typedef struct _Egueb_Svg_Element_Video
{
	EINA_MAGIC
	/* properties */
	Egueb_Svg_Attribute_Animated_Coord x;
	Egueb_Svg_Attribute_Animated_Coord y;
	Egueb_Svg_Attribute_Animated_Length width;
	Egueb_Svg_Attribute_Animated_Length height;
	Egueb_Svg_Attribute_Animated_String href;
	/* generated at setup */
	double gx;
	double gy;
	char *ghref;
	/* private */
	Enesim_Renderer *image;
	Egueb_Svg_Video_Provider_Context ctx;
	Egueb_Svg_Video_Provider_Descriptor *descriptor;
	Egueb_Svg_Video_Provider *provider;
} Egueb_Svg_Element_Video;

static Egueb_Svg_Element_Video * _egueb_svg_element_video_get(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element_Video *thiz;

	if (egueb_svg_element_internal_type_get(t) != ESVG_TYPE_VIDEO)
		return NULL;
	thiz = egueb_svg_renderable_data_get(t);
	return thiz;
}
/*----------------------------------------------------------------------------*
 *                       The Esvg Renderable interface                        *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_element_video_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (strcmp(key, "x") == 0)
	{
		Egueb_Svg_Coord x;

		egueb_svg_length_string_from(&x, value);
		egueb_svg_element_video_x_set(e, &x);
	}
	else if (strcmp(key, "y") == 0)
	{
		Egueb_Svg_Coord y;

		egueb_svg_length_string_from(&y, value);
		egueb_svg_element_video_y_set(e, &y);
	}
	else if (strcmp(key, "width") == 0)
	{
		Egueb_Svg_Length width;

		egueb_svg_length_string_from(&width, value);
		egueb_svg_element_video_width_set(e, &width);
	}
	else if (strcmp(key, "height") == 0)
	{
		Egueb_Svg_Length height;

		egueb_svg_length_string_from(&height, value);
		egueb_svg_element_video_height_set(e, &height);
	}
	else if (strcmp(key, "xlink:href") == 0)
	{
		egueb_svg_element_video_xlink_href_set(e, value);
	}
	else
	{
		return EINA_FALSE;
	}

	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_element_video_attribute_get(Egueb_Dom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static int * _egueb_svg_element_video_attribute_animated_fetch(Egueb_Dom_Tag *t, const char *attr)
{
	Egueb_Svg_Element_Video *thiz;
	int *animated = NULL;

	thiz = _egueb_svg_element_video_get(t);
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

static Enesim_Renderer * _egueb_svg_element_video_renderer_get(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element_Video *thiz;

	thiz = _egueb_svg_element_video_get(t);
	return thiz->image;
}

static Egueb_Svg_Element_Setup_Return _egueb_svg_element_video_setup(Egueb_Dom_Tag *t,
		Egueb_Svg_Context *c,
		Egueb_Svg_Element_Context *ctx,
		Egueb_Svg_Attribute_Presentation *attr,
		Enesim_Log **error)
{
	Egueb_Svg_Element_Video *thiz;
	Egueb_Svg_Length lx, ly;
	Egueb_Svg_Length lwidth, lheight;

	thiz = _egueb_svg_element_video_get(t);
	/* set the position */
	egueb_svg_attribute_animated_length_final_get(&thiz->x, &lx);
	egueb_svg_attribute_animated_length_final_get(&thiz->y, &ly);
	thiz->gx = egueb_svg_coord_final_get(&lx, ctx->viewbox.w, ctx->font_size);
	thiz->gy = egueb_svg_coord_final_get(&ly, ctx->viewbox.h, ctx->font_size);
	/* set the size */
	egueb_svg_attribute_animated_length_final_get(&thiz->width, &lwidth);
	egueb_svg_attribute_animated_length_final_get(&thiz->height, &lheight);
	thiz->ctx.width = egueb_svg_coord_final_get(&lwidth, ctx->viewbox.w, ctx->font_size);
	thiz->ctx.height = egueb_svg_coord_final_get(&lheight, ctx->viewbox.h, ctx->font_size);
	/* set the href */
	egueb_svg_attribute_animated_string_final_get(&thiz->href, &thiz->ghref);

	return ESVG_SETUP_OK;
}

static Eina_Bool _egueb_svg_element_video_renderer_propagate(Egueb_Dom_Tag *t,
		Egueb_Svg_Context *c,
		const Egueb_Svg_Element_Context *ctx,
		const Egueb_Svg_Attribute_Presentation *attr,
		Egueb_Svg_Renderable_Context *rctx,
		Enesim_Log **error)
{
	Ender_Element *topmost;
	Egueb_Svg_Element_Video *thiz;
	Enesim_Matrix inv;
	char *real;

	thiz = _egueb_svg_element_video_get(t);

	/* set the image */
	enesim_renderer_image_x_set(thiz->image, thiz->gx);
	enesim_renderer_image_y_set(thiz->image, thiz->gy);
	enesim_renderer_image_width_set(thiz->image, thiz->ctx.width);
	enesim_renderer_image_height_set(thiz->image, thiz->ctx.height);
	enesim_matrix_inverse(&ctx->transform, &inv);
	enesim_renderer_transformation_set(thiz->image, &inv);

	/* set the pipeline attributes */
	if (!thiz->ghref) goto done;
	egueb_svg_element_internal_topmost_get(t, &topmost);
	real = egueb_svg_element_svg_uri_resolve(topmost, thiz->ghref);
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
		Egueb_Svg_Video_Provider_Descriptor *descriptor;

		descriptor = egueb_svg_element_svg_video_provider_descriptor_get(topmost);
		thiz->provider = egueb_svg_video_provider_new(descriptor,
			enesim_renderer_ref(thiz->image));
		if (!thiz->provider)
		{
			ERR("No video provider found");
			goto done;
		}
		thiz->descriptor = descriptor;
	}

	/* do the setup on the provider */
	egueb_svg_video_provider_setup(thiz->provider, &thiz->ctx);
	egueb_svg_video_provider_play(thiz->provider);
done:
	return EINA_TRUE;
}

static void _egueb_svg_element_video_free(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element_Video *thiz;

	thiz = _egueb_svg_element_video_get(t);
	if (thiz->provider)
	{
		egueb_svg_video_provider_free(thiz->provider);
		thiz->provider = NULL;
	}
	free(thiz);
}

static Egueb_Svg_Renderable_Descriptor _descriptor = {
	/* .child_add		     = */ NULL,
	/* .child_remove	     = */ NULL,
	/* .attribute_get 	     = */ _egueb_svg_element_video_attribute_get,
	/* .cdata_set 		     = */ NULL,
	/* .text_set 		     = */ NULL,
	/* .text_get 		     = */ NULL,
	/* .free 		     = */ _egueb_svg_element_video_free,
	/* .initialize 		     = */ NULL,
	/* .attribute_set 	     = */ _egueb_svg_element_video_attribute_set,
	/* .attribute_animated_fetch = */ _egueb_svg_element_video_attribute_animated_fetch,
	/* .setup		     = */ _egueb_svg_element_video_setup,
	/* .renderer_get	     = */ _egueb_svg_element_video_renderer_get,
	/* .renderer_propagate	     = */ _egueb_svg_element_video_renderer_propagate,
};

/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Tag * _egueb_svg_element_video_new(void)
{
	Egueb_Svg_Element_Video *thiz;
	Egueb_Dom_Tag *t;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Egueb_Svg_Element_Video));
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

	t = egueb_svg_renderable_new(&_descriptor, ESVG_TYPE_VIDEO, thiz);

	return t;
}

static void _egueb_svg_element_video_x_set(Egueb_Dom_Tag *t, const Egueb_Svg_Animated_Coord *x)
{
	Egueb_Svg_Element_Video *thiz;
	Egueb_Svg_Length def = { ESVG_UNIT_LENGTH_PX, 0 };
	Eina_Bool animating;

	thiz = _egueb_svg_element_video_get(t);
	animating = egueb_svg_element_attribute_animate_get(t);
	egueb_svg_attribute_animated_length_set(&thiz->x, x, &def, animating);
}

static void _egueb_svg_element_video_x_get(Egueb_Dom_Tag *t, Egueb_Svg_Animated_Coord *x)
{
	Egueb_Svg_Element_Video *thiz;

	thiz = _egueb_svg_element_video_get(t);
	egueb_svg_attribute_animated_length_get(&thiz->x, x);
}

static void _egueb_svg_element_video_y_set(Egueb_Dom_Tag *t, const Egueb_Svg_Animated_Coord *y)
{
	Egueb_Svg_Element_Video *thiz;
	Egueb_Svg_Length def = { ESVG_UNIT_LENGTH_PX, 0 };
	Eina_Bool animating;

	thiz = _egueb_svg_element_video_get(t);
	animating = egueb_svg_element_attribute_animate_get(t);
	egueb_svg_attribute_animated_length_set(&thiz->y, y, &def, animating);
}

static void _egueb_svg_element_video_y_get(Egueb_Dom_Tag *t, Egueb_Svg_Animated_Coord *y)
{
	Egueb_Svg_Element_Video *thiz;

	thiz = _egueb_svg_element_video_get(t);
	egueb_svg_attribute_animated_length_get(&thiz->y, y);
}

static void _egueb_svg_element_video_width_set(Egueb_Dom_Tag *t, const Egueb_Svg_Length_Animated *width)
{
	Egueb_Svg_Element_Video *thiz;
	Egueb_Svg_Length def = { ESVG_UNIT_LENGTH_PX, 0 };
	Eina_Bool animating;

	thiz = _egueb_svg_element_video_get(t);
	animating = egueb_svg_element_attribute_animate_get(t);
	egueb_svg_attribute_animated_length_set(&thiz->width, width, &def, animating);
}

static void _egueb_svg_element_video_width_get(Egueb_Dom_Tag *t, Egueb_Svg_Length_Animated *width)
{
	Egueb_Svg_Element_Video *thiz;

	thiz = _egueb_svg_element_video_get(t);
	egueb_svg_attribute_animated_length_get(&thiz->width, width);
}

static void _egueb_svg_element_video_height_set(Egueb_Dom_Tag *t, const Egueb_Svg_Length_Animated *height)
{
	Egueb_Svg_Element_Video *thiz;
	Egueb_Svg_Length def = { ESVG_UNIT_LENGTH_PX, 0 };
	Eina_Bool animating;

	thiz = _egueb_svg_element_video_get(t);
	animating = egueb_svg_element_attribute_animate_get(t);
	egueb_svg_attribute_animated_length_set(&thiz->height, height, &def, animating);
}

static void _egueb_svg_element_video_height_get(Egueb_Dom_Tag *t, Egueb_Svg_Length_Animated *height)
{
	Egueb_Svg_Element_Video *thiz;

	thiz = _egueb_svg_element_video_get(t);
	egueb_svg_attribute_animated_length_get(&thiz->height, height);
}

static void _egueb_svg_element_video_xlink_href_set(Egueb_Dom_Tag *t, Egueb_Svg_String_Animated *href)
{
	Egueb_Svg_Element_Video *thiz;
	Eina_Bool animating;

	thiz = _egueb_svg_element_video_get(t);
	animating = egueb_svg_element_attribute_animate_get(t);
	egueb_svg_attribute_animated_string_set(&thiz->href,
		href, animating);
}

static void _egueb_svg_element_video_xlink_href_get(Egueb_Dom_Tag *t, Egueb_Svg_String_Animated *href)
{
	Egueb_Svg_Element_Video *thiz;

	thiz = _egueb_svg_element_video_get(t);
	egueb_svg_attribute_animated_string_get(&thiz->href,
		href);
}

/* The ender wrapper */
#define _egueb_svg_element_video_delete NULL
#define _egueb_svg_element_video_x_is_set NULL
#define _egueb_svg_element_video_y_is_set NULL
#define _egueb_svg_element_video_width_is_set NULL
#define _egueb_svg_element_video_height_is_set NULL
#define _egueb_svg_element_video_xlink_href_is_set NULL
#include "egueb_svg_generated_element_video.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_svg_element_video_init(void)
{
	_egueb_svg_element_video_log = eina_log_domain_register("egueb_svg_element_video", ESVG_LOG_COLOR_DEFAULT);
	if (_egueb_svg_element_video_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_egueb_svg_element_video_init();
}

void egueb_svg_element_video_shutdown(void)
{
	if (_egueb_svg_element_video_log < 0)
		return;
	_egueb_svg_element_video_shutdown();
	eina_log_domain_unregister(_egueb_svg_element_video_log);
	_egueb_svg_element_video_log = -1;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * egueb_svg_element_video_new(void)
{
	return ESVG_ELEMENT_NEW("SVGVideoElement");
}

EAPI Eina_Bool egueb_svg_is_video(Ender_Element *e)
{
	Egueb_Dom_Tag *t;
	Egueb_Svg_Type type;

	t = (Egueb_Dom_Tag *)ender_element_object_get(e);
	type = egueb_svg_element_internal_type_get(t);
	return (type == ESVG_TYPE_VIDEO) ? EINA_TRUE : EINA_FALSE;
}

EAPI void egueb_svg_element_video_x_set(Ender_Element *e, const Egueb_Svg_Coord *x)
{
	egueb_svg_element_property_length_set(e, ESVG_ELEMENT_VIDEO_X, x);
}

EAPI void egueb_svg_element_video_x_get(Ender_Element *e, Egueb_Svg_Coord *x)
{
}

EAPI void egueb_svg_element_video_y_set(Ender_Element *e, const Egueb_Svg_Coord *y)
{
	egueb_svg_element_property_length_set(e, ESVG_ELEMENT_VIDEO_Y, y);
}

EAPI void egueb_svg_element_video_y_get(Ender_Element *e, Egueb_Svg_Coord *y)
{
}

EAPI void egueb_svg_element_video_width_set(Ender_Element *e, const Egueb_Svg_Length *width)
{
	egueb_svg_element_property_length_set(e, ESVG_ELEMENT_VIDEO_WIDTH, width);
}

EAPI void egueb_svg_element_video_width_get(Ender_Element *e, Egueb_Svg_Length *width)
{
}

EAPI void egueb_svg_element_video_height_set(Ender_Element *e, const Egueb_Svg_Length *height)
{
	egueb_svg_element_property_length_set(e, ESVG_ELEMENT_VIDEO_HEIGHT, height);
}

EAPI void egueb_svg_element_video_height_get(Ender_Element *e, Egueb_Svg_Length *height)
{
}

EAPI void egueb_svg_element_video_xlink_href_set(Ender_Element *e, const char *href)
{
	egueb_svg_element_property_string_set(e, ESVG_ELEMENT_VIDEO_XLINK_HREF, href);
}

EAPI void egueb_svg_element_video_xlink_href_get(Ender_Element *e, const char **href)
{
}

