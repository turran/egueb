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
#include <config.h>
#endif

#include <Eina.h>
#include <Egueb_Css.h>
#include <Ender.h>

#include "egueb_svg_main_private.h"
#include "egueb_svg_private_attribute_presentation.h"
#include "egueb_svg_context_private.h"
#include "egueb_svg_element_private.h"
#include "egueb_svg_parser_private.h"

#include "egueb_svg_types.h"
#include "egueb_svg_main.h"
#include "egueb_svg_element_a.h"
#include "egueb_svg_element_clip_path.h"
#include "egueb_svg_element_defs.h"
#include "egueb_svg_element_animate.h"
#include "egueb_svg_element_animate_transform.h"
#include "egueb_svg_video_provider.h"
#include "egueb_svg_script_provider.h"
#include "egueb_svg_element_svg.h"
#include "egueb_svg_element_ellipse.h"
#include "egueb_svg_element_rect.h"
#include "egueb_svg_element_circle.h"
#include "egueb_svg_element_image.h"
#include "egueb_svg_element_path.h"
#include "egueb_svg_element_polygon.h"
#include "egueb_svg_element_polyline.h"
#include "egueb_svg_element_g.h"
#include "egueb_svg_element_line.h"
#include "egueb_svg_element_linear_gradient.h"
#include "egueb_svg_element_radial_gradient.h"
#include "egueb_svg_element_script.h"
#include "egueb_svg_element_set.h"
#include "egueb_svg_element_stop.h"
#include "egueb_svg_element_style.h"
#include "egueb_svg_element_text.h"
#include "egueb_svg_element_use.h"
#include "egueb_svg_element_video.h"

#include "egueb_svg_parser.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT _egueb_svg_parser_log

static int _egueb_svg_parser_log = -1;

typedef struct _Egueb_Svg_Parser
{
	Ender_Element *topmost;
} Egueb_Svg_Parser;

static void _egueb_svg_parser_tree_dump(Egueb_Dom_Tag *t, int level);

static Eina_Bool _egueb_svg_parser_tree_dump_cb(Egueb_Dom_Tag *t, Egueb_Dom_Tag *child, void *data)
{
	int *level = data;

	_egueb_svg_parser_tree_dump(child, *level);
	return EINA_TRUE;
}

static void _egueb_svg_parser_tree_dump(Egueb_Dom_Tag *t, int level)
{
	char out[PATH_MAX];
	const char *name;
	int i;

	for (i = 0; i < level; i++)
		out[i] = ' ';
	out[i] = '\0';
	name = egueb_dom_tag_name_get(t);
	strncat(out, name ? name : "(UNKNOWN)", PATH_MAX - i);
	INFO("%s", out);
	level += 1;
	egueb_dom_tag_child_foreach(t, _egueb_svg_parser_tree_dump_cb, &level);
}

static char * _egueb_svg_parser_file_open(const char *filename, size_t *sz)
{
	FILE *f;
	char *buf;
	size_t res;
	size_t _sz;

	if (!filename || !(*filename))
		return NULL;

	f = fopen(filename, "rb");
	if (!f)
	{
		ERR("Can not open file %s", filename);
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
	if (res != _sz)
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

static Ender_Element * _egueb_svg_parser_parse(Egueb_Dom_Parser *parser, const char *buffer, size_t size)
{
	Ender_Element *e = NULL;
	Egueb_Dom_Tag *t;

	if (!egueb_dom_parser_parse(parser, buffer, size))
	{
		DBG("Can not parse buffer");
		goto done;
	}

	/* check if we have a valid renderer */
	e = egueb_dom_parser_topmost_get(parser);
	/* useful for debugging */
	t = ender_element_object_get(e);
	_egueb_svg_parser_tree_dump(t, 0);
done:
	egueb_dom_parser_delete(parser);
	return e;
}
/*----------------------------------------------------------------------------*
 *                            Egueb_Dom parser interface                           *
 *----------------------------------------------------------------------------*/
static void * _egueb_svg_parser_topmost_get(Egueb_Dom_Parser *parser)
{
	Egueb_Svg_Parser *thiz;

	thiz = egueb_dom_parser_data_get(parser);
	return thiz->topmost;
}

static Eina_Bool _egueb_svg_parser_tag_attribute_set(Egueb_Dom_Parser *parser, void *tag, const char *name, const char *value)
{
	Ender_Element *e = tag;
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	egueb_dom_tag_attribute_set(t, name, value);
	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_parser_tag_child_add(Egueb_Dom_Parser *parser, void *t, void *child)
{
	Ender_Element *tag = t;
	Egueb_Dom_Tag *child_tag;

	/* add it */
	/* FIXME on add/remove functions ender should have a return value */
	child_tag = ender_element_object_get(child);
	ender_element_property_value_add(tag, EGUEB_DOM_CHILD, child_tag, NULL);
	return EINA_TRUE;
}

static void _egueb_svg_parser_tag_cdata_set(Egueb_Dom_Parser *parser, void *t, const char *cdata, unsigned int length)
{
	Ender_Element *tag = t;
	Egueb_Dom_String s;

	DBG("parser cdata %d", length);
	s.s = cdata;
	s.length = length;
	ender_element_property_value_set(tag, EGUEB_DOM_CDATA, &s, NULL);
}

static void _egueb_svg_parser_tag_text_set(Egueb_Dom_Parser *parser, void *t, const char *text, unsigned int length)
{
	Ender_Element *tag = t;
	Egueb_Dom_String s;

	s.s = text;
	s.length = length;
	ender_element_property_value_set(tag, EGUEB_DOM_TEXT, &s, NULL);
}

static Egueb_Dom_Parser_Descriptor _descriptor = {
	/* .tag_get 		= */ _egueb_svg_parser_tag_get,
	/* .tag_new 		= */ _egueb_svg_parser_tag_new,
	/* .topmost_get 	= */ _egueb_svg_parser_topmost_get,
	/* .tag_attribute_set 	= */ _egueb_svg_parser_tag_attribute_set,
	/* .tag_child_add 	= */ _egueb_svg_parser_tag_child_add,
	/* .tag_cdata_set 	= */ _egueb_svg_parser_tag_cdata_set,
	/* .tag_text_set 	= */ _egueb_svg_parser_tag_text_set,

};
/*----------------------------------------------------------------------------*
 *                            Egueb_Dom parser interface                           *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_parser_info_tag_get(Egueb_Dom_Parser *parser, const char *content,
		 size_t sz, int *tag)
{
	if (strncmp("svg", content, sz) == 0)
	{
		*tag = ESVG_TYPE_SVG;
		return EINA_TRUE;
	}
	return EINA_FALSE;
}

static void * _egueb_svg_parser_info_tag_new(Egueb_Dom_Parser *parser, int tag_id)
{
	Egueb_Svg_Parser *thiz;
	Ender_Element *tag = NULL;

	thiz = egueb_dom_parser_data_get(parser);
	tag = egueb_svg_element_svg_new();
	if (!thiz->topmost)
		thiz->topmost = tag;
	return tag;
}

static Egueb_Dom_Parser_Descriptor _info_descriptor = {
	/* .tag_get 		= */ _egueb_svg_parser_info_tag_get,
	/* .tag_new 		= */ _egueb_svg_parser_info_tag_new,
	/* .topmost_get 	= */ _egueb_svg_parser_topmost_get,
	/* .tag_attribute_set 	= */ _egueb_svg_parser_tag_attribute_set,
	/* .tag_child_add 	= */ NULL,
	/* .tag_cdata_set 	= */ NULL,
	/* .tag_text_set 	= */ NULL,
};

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_svg_parser_init(void)
{
	_egueb_svg_parser_log = eina_log_domain_register("egueb_svg_parser", ESVG_LOG_COLOR_DEFAULT);
	if (_egueb_svg_parser_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
}

void egueb_svg_parser_shutdown(void)
{
	if (_egueb_svg_parser_log < 0)
		return;
	eina_log_domain_unregister(_egueb_svg_parser_log);
	_egueb_svg_parser_log = -1;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * @param width The parsed svg's width attribute
 * @param height The parsed svg's height attribute
 */
EAPI Eina_Bool egueb_svg_parser_info_load(const char *filename,
		Egueb_Svg_Length *width, Egueb_Svg_Length *height)
{
	Ender_Element *e;
	Eina_Bool ret;
	char *buffer;
	size_t size;

	buffer = _egueb_svg_parser_file_open(filename, &size);
	if (!buffer) return EINA_FALSE;

	ret = egueb_svg_parser_info_load_from_buffer(buffer, size, width, height);
	free(buffer);
	return ret;
}

EAPI Eina_Bool egueb_svg_parser_info_load_from_buffer(const char *buffer,
		size_t size, Egueb_Svg_Length *width, Egueb_Svg_Length *height)
{
	Egueb_Svg_Parser *thiz;
	Egueb_Dom_Parser *info_parser;
	Ender_Element *e;

	thiz = calloc(1, sizeof(Egueb_Svg_Parser));
	info_parser = egueb_dom_parser_new(&_info_descriptor, thiz);
	e = _egueb_svg_parser_parse(info_parser, buffer, size);
	if (!e) return EINA_FALSE;

	egueb_svg_element_svg_width_get(e, width);
	egueb_svg_element_svg_height_get(e, height);
	//ender_element_unref(e);

	return EINA_TRUE;
}

/**
 *
 */
EAPI Ender_Element * egueb_svg_parser_load(const char *filename)
{
	Ender_Element *e;
	char *buffer;
	size_t size;

	buffer = _egueb_svg_parser_file_open(filename, &size);
	if (!buffer) return NULL;

	e = egueb_svg_parser_load_from_buffer(buffer, size);
	free(buffer);

	return e;
}

EAPI Ender_Element * egueb_svg_parser_load_from_buffer(const char *buffer, size_t size)
{
	Egueb_Svg_Parser *thiz;
	Egueb_Dom_Parser *parser;
	Ender_Element *e;

	thiz = calloc(1, sizeof(Egueb_Svg_Parser));
	parser = egueb_dom_parser_new(&_descriptor, thiz);
	return _egueb_svg_parser_parse(parser, buffer, size);
}

/**
 * FIXME for later
 */
EAPI void egueb_svg_parser_save(Enesim_Renderer *r, const char *filename)
{

}

