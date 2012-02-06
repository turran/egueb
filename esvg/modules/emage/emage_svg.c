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
/* FIXME we should not include this */
//#include <esvg_private.h>
#include <Edom.h>

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

/* FIXME this code is shared with the parser, we better do something */
static char * _file_open(const char *filename, long *sz)
{
	FILE *f;
	char *buf;
	size_t res;
	long _sz;

	if (!filename || !(*filename))
		return NULL;

	f = fopen(filename, "rb");
	if (!f)
	{
		return NULL;
	}

	if (fseek(f, 0, SEEK_END) != 0)
		goto close_f;

	_sz = ftell(f);
	if (_sz < 0)
		goto close_f;

	if (fseek(f, 0, SEEK_SET) < 0)
		goto close_f;

	buf = (char *)malloc(_sz);
	if (!buf)
		goto close_f;

	res = fread(buf, 1, _sz, f);
	if (res != (size_t)_sz)
		goto free_buf;

	fclose(f);

	*sz = _sz;
	return buf;

free_buf:
	free(buf);
close_f:
	fclose(f);

	return NULL;
}
/*----------------------------------------------------------------------------*
 *                           The tag interface                                *
 *----------------------------------------------------------------------------*/
static Eina_Bool _emage_svg_tag_attribute_set(Edom_Tag *tag, const char *key, const char *value)
{
	Emage_Svg_Svg *thiz;
	Esvg_Length length_0 = { 0.0, ESVG_UNIT_LENGTH_PX }; 

	thiz = edom_tag_data_get(tag);
	if (strcmp(key, "width") == 0)
	{
		esvg_length_get(&thiz->width, value, length_0);
	}
	else if (strcmp(key, "height") == 0)
	{
		esvg_length_get(&thiz->height, value, length_0);
	}

	return EINA_TRUE;
}

static Edom_Tag_Descriptor _tag_descriptor = {
	/* .name_get 		= */ NULL,
	/* .attribute_set 	= */ _emage_svg_tag_attribute_set,
	/* .attribute_get 	= */ NULL,
};
/*----------------------------------------------------------------------------*
 *                         The context interface                               *
 *----------------------------------------------------------------------------*/
static Eina_Bool _emage_svg_context_tag_open(void *data, int tag_type, Eina_Array *contexts,
		const char *attributes, unsigned int length)
{
	Emage_Svg_Svg *thiz = data;
	Edom_Context *context;
	Edom_Tag *tag;

	/* the document context only supports SVG tags */
	if (tag_type != EMAGE_SVG_SVG)
		return EINA_FALSE;

	context = eina_array_data_get(contexts, eina_array_count_get(contexts) - 1);
	tag = edom_tag_new(context, &_tag_descriptor, EMAGE_SVG_SVG, NULL, thiz);
	edom_tag_attributes_from_xml(tag, attributes, length);

	return EINA_TRUE;
}

static Edom_Context_Descriptor _context_descriptor = {
	/* .tag_open = 	*/ _emage_svg_context_tag_open,
	/* .tag_close = */ NULL,
	/* .data =	*/ NULL,
	/* .cdata =	*/ NULL,
	/* .free = 	*/ NULL,
};
/*----------------------------------------------------------------------------*
 *                         The parser interface                               *
 *----------------------------------------------------------------------------*/
static Eina_Bool _emage_svg_parser_tag_get(Edom_Parser *parser, const char *content,
		 size_t sz, int *tag)
{
	if (sz != 3) return EINA_FALSE;
	if (!strncmp(content, "svg", 3))
	{
		*tag = EMAGE_SVG_SVG;
		return EINA_TRUE;
	}
	return EINA_FALSE;
}

static Edom_Parser_Descriptor _parser_descriptor = {
	/* .tag_get 	= */ _emage_svg_parser_tag_get,
};
/*----------------------------------------------------------------------------*
 *                          Emage Provider API                                *
 *----------------------------------------------------------------------------*/
typedef struct _Emage_Svg_Options
{
	int width;
	int height;
} Emage_Svg_Options;

static const int _default_width = 640;
static const int _default_height = 480;

static void _options_parse_cb(void *data, const char *key, const char *value)
{
	Emage_Svg_Options *thiz = data;

	if (!strcmp(key, "width"))
		thiz->width = atoi(value);
	else if (!strcmp(key, "height"))
		thiz->height = atoi(value);
}

static void * _emage_svg_options_parse(const char *options_str)
{
	Emage_Svg_Options *options;

	options = calloc(1, sizeof(Emage_Svg_Options));
	options->width = _default_width;
	options->height = _default_height;
	emage_options_parse(options_str, _options_parse_cb, options);
	printf("inside options! %d %d\n", options->width, options->height);
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
	Emage_Svg_Svg thiz;
	Edom_Parser *parser;
	Edom_Context *c;
	double svg_w;
	double svg_h;
	char *buf;
	long sz;
	int cw = _default_width;
	int ch = _default_height;
	Esvg_Length length_0 = { 0.0, ESVG_UNIT_LENGTH_PX }; 

	/* FIXME initialize the emage svg structure correctly */
	thiz.width = thiz.height = length_0; 

	buf = _file_open(file, &sz);
	if (!buf)
	{
		printf("error\n");
		return EMAGE_ERROR_EXIST;
	}
	parser = edom_parser_new(&_parser_descriptor, &thiz);
	c = edom_context_new(parser, &_context_descriptor, &thiz);
	edom_parser_parse(parser, c, buf, sz);
	edom_context_delete(c);
	edom_parser_delete(parser);

	/* get the final size */
	if (options)
	{
		Emage_Svg_Options *o = options;

		cw = o->width;
		ch = o->height;
	}

	/* FIXME this is not EAPI */
	svg_w = esvg_length_final_get(&thiz.width, cw);
	svg_h = esvg_length_final_get(&thiz.height, ch);

	*w = (int)svg_w;
	*h = (int)svg_h;
	*sfmt = ENESIM_BUFFER_FORMAT_ARGB8888_PRE;
	return 0;
}

static Eina_Error _emage_svg_load(const char *file, Enesim_Buffer *buffer, void *options)
{
	Enesim_Renderer *r;
	Enesim_Surface *s;
	Enesim_Error *err = NULL;
	Eina_Bool ret;
	double svg_w;
	double svg_h;
	int w = _default_width;
	int h = _default_height;

	r = esvg_parser_load(file);
	if (!r)
	{
		return EMAGE_ERROR_LOADING;
	}

	if (options)
	{
		Emage_Svg_Options *o = options;

		w = o->width;
		h = o->height;
	}
	/* we should render into the swdata? */
	esvg_element_container_width_set(r, w);
	esvg_element_container_height_set(r, h);
	esvg_svg_actual_width_get(r, &svg_w);
	esvg_svg_actual_height_get(r, &svg_h);

	w = (int)svg_w;
	h = (int)svg_h;

	s = enesim_surface_new_buffer_from(buffer);
	if (!s)
	{
		printf("no such surface\n");
		return 0;
	}
	printf("surface created of size %d %d\n", w, h);
	ret = enesim_renderer_draw(r, s, NULL, 0, 0, &err);
	if (!ret)
	{
		enesim_error_dump(err);
	}
	printf("ret = %d %p %p\n", ret, r, s);
	enesim_surface_unref(s);

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
	 * - Register svg spepcific errors
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
