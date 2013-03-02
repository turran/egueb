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
#include "config.h"
#endif

#include <math.h>
#include <libgen.h>

#include <Esvg.h>

/*
 * To implement the load_info interface we need to parse the svg tree, but
 * only the first svg tag and its width/height properties. So we need
 * to create an Edom interface to retrieve such information
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef enum _Enesim_Image_Svg_Tag_Type
{
	ENESIM_IMAGE_SVG_SVG,
} Enesim_Image_Svg_Tag_Type;

typedef struct _Enesim_Image_Svg_Svg
{
	Esvg_Length width;
	Esvg_Length height;
} Enesim_Image_Svg_Svg;

/*----------------------------------------------------------------------------*
 *                        Esvg Application Descriptor                         *
 *----------------------------------------------------------------------------*/
/* given that we only support this callback, we pass the dir name as the data */
static const char * _enesim_image_svg_base_dir_get(Ender_Element *e, void *data)
{
	return data;
}

static Esvg_Element_Svg_Application_Descriptor _enesim_image_svg_descriptor = {
	/* .base_dir_get 	= */ _enesim_image_svg_base_dir_get,
	/* .go_to 		= */ NULL,
};
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
	Esvg_Length width;
	Esvg_Length height;
	Eina_Bool mmaped = EINA_FALSE;
	Eina_Error ret = 0;
	double svg_w;
	double svg_h;
	int cw = _default_width;
	int ch = _default_height;
	char *mmap;
	size_t size;

	/* first try to map the data */
	mmap = enesim_image_data_mmap(data, &size);
	/* in case it fails, we should try to read the whole svg */
	if (!mmap)
	{
		size = enesim_image_data_length(data);
		if (!size) return ENESIM_IMAGE_ERROR_LOADING;

		mmap = malloc(size);
		enesim_image_data_read(data, mmap, size);
		mmaped = EINA_FALSE;
	}
	if (!esvg_parser_info_load_from_buffer(mmap, size, &width, &height))
	{
		ret = ENESIM_IMAGE_ERROR_LOADING;
		goto err_load;
	}
	/* get the final size */
	if (options)
	{
		Enesim_Image_Svg_Options *o = options;

		cw = o->container_width;
		ch = o->container_height;
	}
	/* FIXME this is not EAPI */
	/* FIXME the 16 should be part of the options, same as container width */
	svg_w = esvg_coord_final_get(&width, cw, 16);
	svg_h = esvg_coord_final_get(&height, ch, 16);

	*w = (int)ceil(svg_w);
	*h = (int)ceil(svg_h);
	*sfmt = ENESIM_BUFFER_FORMAT_ARGB8888_PRE;

err_load:
	if (mmaped)
		free(mmap);
	return ret;
}

static Eina_Error _enesim_image_svg_load(Enesim_Image_Data *data, Enesim_Buffer *buffer, void *options)
{
	Ender_Element *e;
	Enesim_Surface *s;
	Enesim_Log *err = NULL;
	Eina_Bool r;
	Eina_Bool mmaped = EINA_TRUE;
	Eina_Error ret = 0;
	double svg_w;
	double svg_h;
	int w = _default_width;
	int h = _default_height;
	char *location;
	char *mmap;
	size_t size;

	/* first try to map the data */
	mmap = enesim_image_data_mmap(data, &size);
	/* in case it fails, we should try to read the whole svg */
	if (!mmap)
	{
		size = enesim_image_data_length(data);
		if (!size) return ENESIM_IMAGE_ERROR_LOADING;

		mmap = malloc(size);
		enesim_image_data_read(data, mmap, size);
		mmaped = EINA_FALSE;
	}

	e = esvg_parser_load_from_buffer(mmap, size);
	if (!e)
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
		char *base_dir;
		char *tmp;

		tmp = dirname(location);
		if (!strcmp(tmp, "."))
			tmp = "./";
		base_dir = strdup(tmp);
		esvg_element_svg_application_descriptor_set(e, &_enesim_image_svg_descriptor, base_dir);
		free(location);
		location = base_dir;
	}
	/* we should render into the swdata? */
	esvg_element_svg_container_width_set(e, w);
	esvg_element_svg_container_height_set(e, h);
	esvg_element_svg_actual_width_get(e, &svg_w);
	esvg_element_svg_actual_height_get(e, &svg_h);

	w = (int)ceil(svg_w);
	h = (int)ceil(svg_h);

	s = enesim_surface_new_buffer_from(buffer);
	if (!s)
	{
		ret = ENESIM_IMAGE_ERROR_LOADING;
		goto err_surface;
	}
	if (!esvg_element_svg_setup(e, &err))
	{
		enesim_log_dump(err);
		ret = ENESIM_IMAGE_ERROR_LOADING;
		goto err_setup;
	}
	if (!esvg_element_svg_draw(e, s, NULL, 0, 0, NULL))
	{
		ret = ENESIM_IMAGE_ERROR_LOADING;
		enesim_log_dump(err);
	}

err_setup:
	enesim_surface_unref(s);
err_surface:
	//ender_element_unref(e);
err_parse:
	if (mmaped)
		free(mmap);
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
}

EINA_MODULE_INIT(svg_provider_init);
EINA_MODULE_SHUTDOWN(svg_provider_shutdown);
