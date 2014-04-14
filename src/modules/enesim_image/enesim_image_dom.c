/* Egueb_Svg - SVG
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
#include "config.h"
#endif

#include <math.h>
#include <libgen.h>

#include <Egueb_Dom.h>

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ENESIM_IMAGE_LOG_COLOR_DEFAULT EINA_COLOR_GREEN

#ifdef ERR
# undef ERR
#endif
#define ERR(...) EINA_LOG_DOM_ERR(_enesim_image_dom_log, __VA_ARGS__)

#ifdef WRN
# undef WRN
#endif
#define WRN(...) EINA_LOG_DOM_WARN(_enesim_image_dom_log, __VA_ARGS__)

#ifdef DBG
# undef DBG
#endif
#define DBG(...) EINA_LOG_DOM_DBG(_enesim_image_dom_log, __VA_ARGS__)

static int _enesim_image_dom_log = -1;

/*----------------------------------------------------------------------------*
 *                         Enesim Image Provider API                          *
 *----------------------------------------------------------------------------*/
typedef struct _Enesim_Image_Svg_Options
{
	int container_width;
	int container_height;
} Enesim_Image_Svg_Options;

static const int _default_width = 640;
static const int _default_height = 480;

static void _options_parse_cb(void *data, const char *key, const char *value)
{
	Enesim_Image_Svg_Options *thiz = data;

	if (!strcmp(key, "width"))
		thiz->container_width = atoi(value);
	else if (!strcmp(key, "height"))
		thiz->container_height = atoi(value);
}

static void * _enesim_image_dom_options_parse(const char *options_str)
{
	Enesim_Image_Svg_Options *options;

	options = calloc(1, sizeof(Enesim_Image_Svg_Options));
	options->container_width = _default_width;
	options->container_height = _default_height;
	enesim_image_options_parse(options_str, _options_parse_cb, options);
	/* the options we support are:
	 * container_size=wxh
	 */

	return options;
}

static void _enesim_image_dom_options_free(void *data)
{
	Enesim_Image_Svg_Options *options = data;
	free(options);
}

/* Here we should parse the first svg element only and return its
 * size, for now, we dont support this feature
 */
static Eina_Bool _enesim_image_dom_info_load(Enesim_Stream *data, int *w, int *h,
		Enesim_Buffer_Format *sfmt, void *options, Eina_Error *err)
{
	Egueb_Dom_Node *doc = NULL;
	Egueb_Dom_Feature *window;
	Egueb_Dom_Feature *io;
	Eina_Bool ret = EINA_FALSE;
	int cw = _default_width;
	int ch = _default_height;

	if (!egueb_dom_parser_parse(data, &doc))
	{
		*err = ENESIM_IMAGE_ERROR_LOADING;
		goto err_parse;
	}
	/* get the final size */
	if (options)
	{
		Enesim_Image_Svg_Options *o = options;

		cw = o->container_width;
		ch = o->container_height;
	}

	window = egueb_dom_node_feature_get(doc, EGUEB_DOM_FEATURE_WINDOW_NAME, NULL);
	if (!window)
	{
		ret = ENESIM_IMAGE_ERROR_LOADING;
		goto err_feature;
	}

	io = egueb_dom_node_feature_get(doc, EGUEB_DOM_FEATURE_IO_NAME, NULL);
	if (io)
	{
		egueb_dom_feature_io_default_enable(io, EINA_TRUE);
		egueb_dom_feature_unref(io);
	}

	egueb_dom_feature_window_content_size_set(window, cw, ch);
	egueb_dom_document_process(doc);
	egueb_dom_feature_window_content_size_get(window, w, h);
	*sfmt = ENESIM_BUFFER_FORMAT_ARGB8888_PRE;
	egueb_dom_feature_unref(window);
	ret = EINA_TRUE;

err_feature:
	egueb_dom_node_unref(doc);
err_parse:
	return ret;
}

static Eina_Bool _enesim_image_dom_load(Enesim_Stream *data,
		Enesim_Buffer *buffer, void *options, Eina_Error *err)
{
	Egueb_Dom_Node *doc = NULL;
	Egueb_Dom_Feature *window, *render, *io;
	Enesim_Surface *s;
	Eina_Bool ret = EINA_FALSE;
	int w = _default_width;
	int h = _default_height;

	if (!egueb_dom_parser_parse(data, &doc))
	{
		*err = ENESIM_IMAGE_ERROR_LOADING;
		goto err_parse;
	}

	if (options)
	{
		Enesim_Image_Svg_Options *o = options;

		w = o->container_width;
		h = o->container_height;
	}
	window = egueb_dom_node_feature_get(doc, EGUEB_DOM_FEATURE_WINDOW_NAME, NULL);
	if (!window)
	{
		*err = ENESIM_IMAGE_ERROR_LOADING;
		goto err_feature;
	}
	egueb_dom_feature_window_content_size_set(window, w, h);
	egueb_dom_feature_unref(window);

	io = egueb_dom_node_feature_get(doc, EGUEB_DOM_FEATURE_IO_NAME, NULL);
	if (io)
	{
		egueb_dom_feature_io_default_enable(io, EINA_TRUE);
		egueb_dom_feature_unref(io);
	}

	render = egueb_dom_node_feature_get(doc, EGUEB_DOM_FEATURE_RENDER_NAME, NULL);
	if (!render)
	{
		*err = ENESIM_IMAGE_ERROR_LOADING;
		goto err_feature;
	}

	s = enesim_surface_new_buffer_from(buffer);
	if (!s)
	{
		*err = ENESIM_IMAGE_ERROR_LOADING;
		goto err_surface;
	}

	egueb_dom_document_process(doc);
	if (!egueb_dom_feature_render_draw(render, s, ENESIM_ROP_FILL, NULL, 0, 0, NULL))
	{
		*err = ENESIM_IMAGE_ERROR_LOADING;
		goto err_draw;
	}
	ret = EINA_TRUE;

err_draw:
	enesim_surface_unref(s);
err_surface:
	egueb_dom_feature_unref(render);
err_feature:
	egueb_dom_node_unref(doc);
err_parse:
	return ret;
}

static Enesim_Image_Provider_Descriptor _provider = {
	/* .name = 		*/ "svg",
	/* .options_parse = 	*/ _enesim_image_dom_options_parse,
	/* .options_free = 	*/ _enesim_image_dom_options_free,
	/* .loadable = 		*/ NULL,
	/* .saveable = 		*/ NULL,
	/* .info_get = 		*/ _enesim_image_dom_info_load,
	/* .load = 		*/ _enesim_image_dom_load,
	/* .save = 		*/ NULL,
};

/*----------------------------------------------------------------------------*
 *                           Enesim Image Finder API                          *
 *----------------------------------------------------------------------------*/
static const char * _enesim_image_dom_data_from(Enesim_Stream *data)
{
	char buf[4096];
	char *ret = NULL;
	int count;
	int i;

	/* TODO we should find the first tag and skip all the comments */
	/* only try to find the <svg tag in the first 4096 bytes */
	count = enesim_stream_read(data, buf, 4096);
	for (i = 0; i < count; i++)
	{
		if (buf[i] == '<' && i + 4 < count)
		{
			/* check for the <svg tag */
			if (!strncmp(&buf[i], "<svg", 4))
			{
				ret = "image/svg+xml";
				break;
			}
		}
	}
	return ret;
}

static const char * _enesim_image_dom_extension_from(const char *ext)
{
	if (!strcmp(ext, "svg"))
		return "image/svg+xml";
	return NULL;
}

static Enesim_Image_Finder _finder = {
	/* .data_from 		= */ _enesim_image_dom_data_from,
	/* .extension_from 	= */ _enesim_image_dom_extension_from,
};
/*----------------------------------------------------------------------------*
 *                             Module API                                     *
 *----------------------------------------------------------------------------*/
static Eina_Bool _enesim_image_dom_init(void)
{
	/* @todo
	 * - Register svg specific errors
	 */
	egueb_dom_init();
	_enesim_image_dom_log = eina_log_domain_register("enesim_image_dom", ENESIM_IMAGE_LOG_COLOR_DEFAULT);
	if (_enesim_image_dom_log < 0)
	{
		EINA_LOG_ERR("Enesim: Can not create a general log domain.");
		return EINA_FALSE;
	}
	/* TODO later we need to register the same provider with every mimetype found on the lib
	 * that supports the render/window interface
	 */
	if (!enesim_image_provider_register(&_provider, ENESIM_PRIORITY_PRIMARY, "image/svg+xml"))
		return EINA_FALSE;
	if (!enesim_image_finder_register(&_finder))
	{
		enesim_image_provider_unregister(&_provider, "image/svg+xml");
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

static void _enesim_image_dom_shutdown(void)
{
	enesim_image_finder_unregister(&_finder);
	enesim_image_provider_unregister(&_provider, "image/svg+xml");
	eina_log_domain_unregister(_enesim_image_dom_log);
	_enesim_image_dom_log = -1;
	egueb_dom_shutdown();
}

EINA_MODULE_INIT(_enesim_image_dom_init);
EINA_MODULE_SHUTDOWN(_enesim_image_dom_shutdown);
