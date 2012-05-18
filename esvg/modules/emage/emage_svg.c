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

#include <Esvg.h>
#include <math.h>

#if HAVE_EMAGE
#include <Emage.h>

/*
 * To implement the load_info interface we need to parse the svg tree, but
 * only the first svg tag and its width/height properties. So we need
 * to create an Edom interface to retrieve such information
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef enum _Emage_Svg_Tag_Type
{
	EMAGE_SVG_SVG,
} Emage_Svg_Tag_Type;

typedef struct _Emage_Svg_Svg
{
	Esvg_Length width;
	Esvg_Length height;
} Emage_Svg_Svg;

static Eina_Bool _check_extension(const char *file)
{
	char *d;

	d = strrchr(file, '.');
	if (!d) return EINA_FALSE;

	d++;
	if (!strcasecmp(d, "svg"))
		return EINA_TRUE;
	return EINA_FALSE;
}
/*----------------------------------------------------------------------------*
 *                          Emage Provider API                                *
 *----------------------------------------------------------------------------*/
typedef struct _Emage_Svg_Options
{
	int container_width;
	int container_height;
} Emage_Svg_Options;

static const int _default_width = 640;
static const int _default_height = 480;

static void _options_parse_cb(void *data, const char *key, const char *value)
{
	Emage_Svg_Options *thiz = data;

	if (!strcmp(key, "width"))
		thiz->container_width = atoi(value);
	else if (!strcmp(key, "height"))
		thiz->container_height = atoi(value);
}

static void * _emage_svg_options_parse(const char *options_str)
{
	Emage_Svg_Options *options;

	options = calloc(1, sizeof(Emage_Svg_Options));
	options->container_width = _default_width;
	options->container_height = _default_height;
	emage_options_parse(options_str, _options_parse_cb, options);
	/* the options we support are:
	 * container_size=wxh
	 */

	return options;
}

static void _emage_svg_options_free(void *data)
{
	Emage_Svg_Options *options = data;
	free(options);
}

static Eina_Bool _emage_svg_loadable(const char *file)
{
	return _check_extension(file);
}

/* Here we should parse the first svg element only and return its
 * size, for now, we dont support this feature
 */
static Eina_Error _emage_svg_info_load(const char *file, int *w, int *h, Enesim_Buffer_Format *sfmt, void *options)
{
	double svg_w;
	double svg_h;
	int cw = _default_width;
	int ch = _default_height;
	Esvg_Length width;
	Esvg_Length height;

	if (!esvg_parser_info_load(file, &width, &height))
	{
		return EMAGE_ERROR_LOADING;
	}
	/* get the final size */
	if (options)
	{
		Emage_Svg_Options *o = options;

		cw = o->container_width;
		ch = o->container_height;
	}
	/* FIXME this is not EAPI */
	svg_w = esvg_length_final_get(&width, cw);
	svg_h = esvg_length_final_get(&height, ch);

	*w = (int)ceil(svg_w);
	*h = (int)ceil(svg_h);
	*sfmt = ENESIM_BUFFER_FORMAT_ARGB8888_PRE;

	return 0;
}

static Eina_Error _emage_svg_load(const char *file, Enesim_Buffer *buffer, void *options)
{
	Ender_Element *e;
	Enesim_Surface *s;
	Enesim_Error *err = NULL;
	Eina_Bool ret;
	double svg_w;
	double svg_h;
	int w = _default_width;
	int h = _default_height;

	e = esvg_parser_load(file, NULL, NULL);
	if (!e)
	{
		return EMAGE_ERROR_LOADING;
	}

	if (options)
	{
		Emage_Svg_Options *o = options;

		w = o->container_width;
		h = o->container_height;
	}
	/* we should render into the swdata? */
	esvg_renderable_container_width_set(e, w);
	esvg_renderable_container_height_set(e, h);
	esvg_svg_actual_width_get(e, &svg_w);
	esvg_svg_actual_height_get(e, &svg_h);

	w = (int)ceil(svg_w);
	h = (int)ceil(svg_h);

	s = enesim_surface_new_buffer_from(buffer);
	if (!s)
	{
		printf("no such surface\n");
		return 0;
	}
	ret = esvg_element_setup(e, &err);
	if (!ret)
	{
		enesim_error_dump(err);
	}
	ret = esvg_renderable_draw(e, s, NULL, 0, 0, NULL);
	if (!ret)
	{
		enesim_error_dump(err);
	}
	enesim_surface_unref(s);
	// ender_element_unref(e);

	return 0;
}

static Emage_Provider _provider = {
	/* .name = 		*/ "svg",
	/* .type = 		*/ EMAGE_PROVIDER_SW,
	/* .options_parse = 	*/ _emage_svg_options_parse,
	/* .options_free = 	*/ _emage_svg_options_free,
	/* .loadable = 		*/ _emage_svg_loadable,
	/* .saveable = 		*/ NULL,
	/* .info_get = 		*/ _emage_svg_info_load,
	/* .load = 		*/ _emage_svg_load,
	/* .save = 		*/ NULL,
};
/*----------------------------------------------------------------------------*
 *                             Module API                                     *
 *----------------------------------------------------------------------------*/
Eina_Bool svg_provider_init(void)
{
	/* @todo
	 * - Register svg specific errors
	 */
	return emage_provider_register(&_provider);
}

void svg_provider_shutdown(void)
{
	emage_provider_unregister(&_provider);
}

EINA_MODULE_INIT(svg_provider_init);
EINA_MODULE_SHUTDOWN(svg_provider_shutdown);

#endif
