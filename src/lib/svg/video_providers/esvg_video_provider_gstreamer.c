/* Esvg - SVG
 * Copyright (C) 2012 Jorge Luis Zapata
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
#include <gst/gst.h>
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_COLOR_DEFAULT EINA_COLOR_ORANGE
/* Whenever a file needs to generate a log, it must declare this first */

#ifdef ERR
# undef ERR
#endif
#define ERR(...) EINA_LOG_DOM_ERR(_egueb_svg_video_provider_gstreamer_log, __VA_ARGS__)

#ifdef WARN
# undef WARN
#endif
#define WARN(...) EINA_LOG_DOM_WARN(_egueb_svg_video_provider_gstreamer_log, __VA_ARGS__)

#ifdef INFO
# undef INFO
#endif
#define INFO(...) EINA_LOG_DOM_INFO(_egueb_svg_video_provider_gstreamer_log, __VA_ARGS__)

#ifdef DBG
# undef DBG
#endif
#define DBG(...) EINA_LOG_DOM_DBG(_egueb_svg_video_provider_gstreamer_log, __VA_ARGS__)

/* FIXME we still need to register the log domain */
static int _egueb_svg_video_provider_gstreamer_log = -1;

typedef struct _Egueb_Svg_Element_Video_Gstreamer
{
	GstElement *enesim_sink;
	GstElement *playbin2;

	Enesim_Renderer *image;

	Enesim_Buffer *b;
	Enesim_Surface *s;

	uint32_t width;
	uint32_t height;
} Egueb_Svg_Element_Video_Gstreamer;

static char * _egueb_svg_element_video_path_urify(const char *s)
{
	/* FIXME we need a way to check if the path stats with an uri */
	if (*s == '/')
	{
		char *uri;
		uri = malloc(strlen(s) + 7 + 1); /* 7 = file:// */
		strcpy(uri, "file://");
		strcat(uri, s);
		return uri;
	}
	else
	{
		return strdup(s);
	}
}

static void _egueb_svg_element_video_buffer_display(GstElement *enesim_sink, Enesim_Buffer *b, gpointer user_data)
{
	Egueb_Svg_Element_Video_Gstreamer *thiz = user_data;

	if (thiz->b == b)
	{
		Eina_Rectangle area;
		/* damage the whole buffer */
		//enesim_renderer_image_damage_add
		eina_rectangle_coords_from(&area, 0, 0, thiz->width, thiz->height);
		enesim_renderer_image_damage_add(thiz->image, &area);
	}
	else
	{
		/* TODO handle correctly the refcounting */
		Enesim_Surface *s;

		if (thiz->s)
			enesim_surface_unref(thiz->s);
		if (thiz->b)
			enesim_buffer_unref(thiz->b);

		s = enesim_surface_new_buffer_from(b);
		thiz->s = enesim_surface_ref(s);
		thiz->b = enesim_buffer_ref(b);
		enesim_renderer_lock(thiz->image);
		enesim_renderer_image_src_set(thiz->image, thiz->s);
		enesim_renderer_unlock(thiz->image);
	}
}
/*----------------------------------------------------------------------------*
 *                           The Video interface                              *
 *----------------------------------------------------------------------------*/
static void * _egueb_svg_element_video_gstreamer_create(Enesim_Renderer *image)
{
	Egueb_Svg_Element_Video_Gstreamer *thiz;
	GstElement *enesim_sink;
	static Eina_Bool _gst_init = EINA_FALSE;

	if (!_gst_init)
	{
		gst_init(NULL, NULL);
		_gst_init = EINA_TRUE;
	}

	enesim_sink = gst_element_factory_make("enesim_sink", NULL);
	if (!enesim_sink)
	{
		ERR("No enesim sink element found");
		enesim_renderer_unref(image);
		return NULL;
	}

	thiz = calloc(1, sizeof(Egueb_Svg_Element_Video_Gstreamer));
	thiz->image = image;

	/* TODO fallback to appsink in case enesim sink is not found */
	/* TODO playbin2 will overwrite our sink, we need
	 * to increase the priority
	 */
	g_signal_connect (enesim_sink, "buffer-display",
          G_CALLBACK (_egueb_svg_element_video_buffer_display), thiz);
	thiz->enesim_sink = gst_object_ref(enesim_sink);

	thiz->playbin2 = gst_element_factory_make("playbin2", "svg_video");
	g_object_set(thiz->playbin2, "video-sink", thiz->enesim_sink, NULL);

	return thiz;
}

static void _egueb_svg_element_video_gstreamer_free(void *data)
{
	Egueb_Svg_Element_Video_Gstreamer *thiz = data;

	enesim_renderer_unref(thiz->image);
	gst_element_set_state(thiz->playbin2, GST_STATE_NULL);
	gst_object_unref(thiz->playbin2);
	gst_object_unref(thiz->enesim_sink);
	free(thiz);
}

static void _egueb_svg_element_video_gstreamer_setup(void *data, const Egueb_Svg_Video_Provider_Context *ctx)
{
	Egueb_Svg_Element_Video_Gstreamer *thiz = data;
	GstState current;
	GstState pending;
	GstStateChangeReturn ret;
	guint width;
	guint height;
	gchar *uri;

	width = ceil(ctx->width);
	height = ceil(ctx->height);

	/* FIXME later the pool */
	/* set the element properties */
	g_object_set(thiz->enesim_sink, "width", width,
			"height", height,
			"format", ENESIM_BUFFER_FORMAT_ARGB8888_PRE,
			NULL);
	/* set the state to READY in case the pipeline is already playing */
	ret = gst_element_get_state(thiz->playbin2, &current, &pending, GST_CLOCK_TIME_NONE);
	if (current != GST_STATE_READY)
		gst_element_set_state(thiz->playbin2, GST_STATE_READY);

	/* we need to transform the real href into a playbin2 uri */
	uri = _egueb_svg_element_video_path_urify(ctx->href);
	g_object_set(thiz->playbin2, "uri", uri, NULL);
	free(uri);

	gst_element_set_state(thiz->playbin2, GST_STATE_PLAYING);
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Svg_Video_Provider_Descriptor egueb_svg_video_provider_gstreamer_descriptor = {
	/* .create 	= */ _egueb_svg_element_video_gstreamer_create,
	/* .free 	= */ _egueb_svg_element_video_gstreamer_free,
	/* .setup 	= */ _egueb_svg_element_video_gstreamer_setup,
};
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

