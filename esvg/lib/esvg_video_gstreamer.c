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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "esvg_private_video.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#ifdef BUILD_ESVG_VIDEO

typedef struct _Esvg_Video_Gstreamer
	GstElement *enesim_sink;
	GstElement *playbin2;
} Esvg_Video_Gstreamer;

static char * _esvg_video_path_urify(const char *s)
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

static void _esvg_video_buffer_display(GstElement *enesim_sink, Enesim_Buffer *b, gpointer user_data)
{
	Esvg_Video *thiz = user_data;

	if (thiz->b == b)
	{
		Eina_Rectangle area;
		/* damage the whole buffer */
		//enesim_renderer_image_damage_add
		eina_rectangle_coords_from(&area, 0, 0, ceil(thiz->gwidth), ceil(thiz->gheight));
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

static void _esvg_video_pipeline_setup(Esvg_Video *thiz)
{
	GstState current;
	GstState pending;
	GstStateChangeReturn ret;
	guint width;
	guint height;
	gchar *uri;

	width = ceil(thiz->gwidth);
	height = ceil(thiz->gheight);

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
	uri = _esvg_video_path_urify(thiz->real_href);
	g_object_set(thiz->playbin2, "uri", uri, NULL);
	free(uri);

	gst_element_set_state(thiz->playbin2, GST_STATE_PLAYING);
}

static void _esvg_video_pipeline_cleanup(Esvg_Video *thiz)
{
	gst_element_set_state(thiz->playbin2, GST_STATE_NULL);
	gst_object_unref(thiz->playbin2);
	gst_object_unref(thiz->enesim_sink);
}

void _esvg_video_gstreamer_new(void)
{
	enesim_sink = gst_element_factory_make("enesim_sink", NULL);
	/* TODO fallback to appsink in case enesim sink is not found */
	/* TODO playbin2 will overwrite our sink, we need
	 * to increase the priority
	 */
	if (!enesim_sink)
	{
		ERR("No enesim sink element found");
		enesim_renderer_unref(thiz->image);
		free(thiz);
		return NULL;
	}
	g_signal_connect (enesim_sink, "buffer-display",
          G_CALLBACK (_esvg_video_buffer_display), thiz);
	thiz->enesim_sink = gst_object_ref(enesim_sink);

	thiz->playbin2 = gst_element_factory_make("playbin2", "svg_video");
	g_object_set(thiz->playbin2, "video-sink", thiz->enesim_sink, NULL);

}

#endif
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

