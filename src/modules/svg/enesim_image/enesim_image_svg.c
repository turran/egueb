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

#include <Egueb_Svg.h>

/*
 * To implement the load_info interface we need to parse the svg tree, but
 * only the first svg tag and its width/height properties. So we need
 * to create an Egueb_Dom interface to retrieve such information
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                           Application Descriptor                           *
 *----------------------------------------------------------------------------*/
#if 0
/* given that we only support this callback, we pass the dir name as the data */
static const char * _enesim_image_svg_base_dir_get(Ender_Element *e, void *data)
{
	return data;
}

static Egueb_Svg_Element_Svg_Application_Descriptor _enesim_image_svg_descriptor = {
	/* .base_dir_get 	= */ _enesim_image_svg_base_dir_get,
	/* .go_to 		= */ NULL,
};
#endif
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

static void * _enesim_image_svg_options_parse(const char *options_str)
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

static void _enesim_image_svg_options_free(void *data)
{
	Enesim_Image_Svg_Options *options = data;
	free(options);
}

/* Here we should parse the first svg element only and return its
 * size, for now, we dont support this feature
 */
static Eina_Error _enesim_image_svg_info_load(Enesim_Image_Data *data, int *w, int *h, Enesim_Buffer_Format *sfmt, void *options)
{
	Egueb_Dom_Node *doc;
	Egueb_Dom_Node *topmost;
	Egueb_Svg_Length width;
	Egueb_Svg_Length height;
	Eina_Error ret = 0;
	double svg_w;
	double svg_h;
	int cw = _default_width;
	int ch = _default_height;

	doc = egueb_svg_document_new(NULL);
	egueb_dom_parser_parse(data, doc);
	egueb_dom_document_element_get(doc, &topmost);
	if (!topmost)
	{
		ret = ENESIM_IMAGE_ERROR_LOADING;
		goto err_parse;
	}
	/* get the final size */
	if (options)
	{
		Enesim_Image_Svg_Options *o = options;

		cw = o->container_width;
		ch = o->container_height;
	}
	egueb_svg_document_width_set(doc, cw);
	egueb_svg_document_height_set(doc, ch);
	egueb_svg_document_actual_width_get(doc, &svg_w);
	egueb_svg_document_actual_height_get(doc, &svg_h);

	*w = (int)ceil(svg_w);
	*h = (int)ceil(svg_h);
	*sfmt = ENESIM_BUFFER_FORMAT_ARGB8888_PRE;

err_parse:
	return ret;
}

static Eina_Error _enesim_image_svg_load(Enesim_Image_Data *data, Enesim_Buffer *buffer, void *options)
{
	Egueb_Dom_Node *doc;
	Egueb_Dom_Node *topmost;
	Enesim_Surface *s;
	Enesim_Log *err = NULL;
	Eina_Bool r;
	Eina_Error ret = 0;
	double svg_w;
	double svg_h;
	int w = _default_width;
	int h = _default_height;
	char *location;

	doc = egueb_svg_document_new(NULL);
	egueb_dom_parser_parse(data, doc);
	egueb_dom_document_element_get(doc, &topmost);
	if (!topmost)
	{
		ret = ENESIM_IMAGE_ERROR_LOADING;
		goto err_parse;
	}

	if (options)
	{
		Enesim_Image_Svg_Options *o = options;

		w = o->container_width;
		h = o->container_height;
	}
	/* set the application descriptor in case the svg needs it */
	location = enesim_image_data_location(data);
	if (location)
	{
#if 0
		char *base_dir;
		char *tmp;

		tmp = dirname(location);
		if (!strcmp(tmp, "."))
			tmp = "./";
		base_dir = strdup(tmp);
		egueb_svg_element_svg_application_descriptor_set(e, &_enesim_image_svg_descriptor, base_dir);
		free(location);
		location = base_dir;
#endif
	}
	/* we should render into the swdata? */
	egueb_svg_document_width_set(doc, w);
	egueb_svg_document_height_set(doc, h);
	s = enesim_surface_new_buffer_from(buffer);
	if (!s)
	{
		ret = ENESIM_IMAGE_ERROR_LOADING;
		goto err_surface;
	}
	egueb_dom_document_process(doc);
	if (!egueb_svg_element_svg_draw(topmost, s, NULL, 0, 0, NULL))
	{
		ret = ENESIM_IMAGE_ERROR_LOADING;
		enesim_log_dump(err);
	}

err_setup:
	enesim_surface_unref(s);
err_surface:
	//ender_element_unref(e);
err_parse:
	egueb_dom_node_unref(doc);
	if (location)
		free(location);
	return ret;
}

static Enesim_Image_Provider_Descriptor _provider = {
	/* .name = 		*/ "svg",
	/* .options_parse = 	*/ _enesim_image_svg_options_parse,
	/* .options_free = 	*/ _enesim_image_svg_options_free,
	/* .loadable = 		*/ NULL,
	/* .saveable = 		*/ NULL,
	/* .info_get = 		*/ _enesim_image_svg_info_load,
	/* .load = 		*/ _enesim_image_svg_load,
	/* .save = 		*/ NULL,
};

/*----------------------------------------------------------------------------*
 *                           Enesim Image Finder API                          *
 *----------------------------------------------------------------------------*/
static const char * _enesim_image_svg_data_from(Enesim_Image_Data *data)
{
	char buf[256];
	char *ret = NULL;
	int count;
	int i;

	/* only try to find the <svg tag in the first 256 bytes */
	count = enesim_image_data_read(data, buf, 256);
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

static const char * _enesim_image_svg_extension_from(const char *ext)
{
	if (!strcmp(ext, "svg"))
		return "image/svg+xml";
	return NULL;
}

static Enesim_Image_Finder _finder = {
	/* .data_from 		= */ _enesim_image_svg_data_from,
	/* .extension_from 	= */ _enesim_image_svg_extension_from,
};
/*----------------------------------------------------------------------------*
 *                             Module API                                     *
 *----------------------------------------------------------------------------*/
Eina_Bool svg_provider_init(void)
{
	/* @todo
	 * - Register svg specific errors
	 */
	egueb_svg_init();
	if (!enesim_image_provider_register(&_provider, ENESIM_PRIORITY_PRIMARY, "image/svg+xml"))
		return EINA_FALSE;
	if (!enesim_image_finder_register(&_finder))
	{
		enesim_image_provider_unregister(&_provider, "image/svg+xml");
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

void svg_provider_shutdown(void)
{
	enesim_image_finder_unregister(&_finder);
	enesim_image_provider_unregister(&_provider, "image/svg+xml");
	egueb_svg_shutdown();
}

EINA_MODULE_INIT(svg_provider_init);
EINA_MODULE_SHUTDOWN(svg_provider_shutdown);
