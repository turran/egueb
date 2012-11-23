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
#include <Ecss.h>
#include <Ender.h>

#include "esvg_private_main.h"
#include "esvg_private_attribute_presentation.h"
#include "esvg_private_context.h"
#include "esvg_private_element.h"
#include "esvg_private_parser.h"

#include "esvg_types.h"
#include "esvg_main.h"
#include "esvg_a.h"
#include "esvg_clip_path.h"
#include "esvg_defs.h"
#include "esvg_animate.h"
#include "esvg_animate_transform.h"
#include "esvg_svg.h"
#include "esvg_ellipse.h"
#include "esvg_rect.h"
#include "esvg_circle.h"
#include "esvg_image.h"
#include "esvg_path.h"
#include "esvg_polygon.h"
#include "esvg_polyline.h"
#include "esvg_line.h"
#include "esvg_linear_gradient.h"
#include "esvg_radial_gradient.h"
#include "esvg_g.h"
#include "esvg_script.h"
#include "esvg_set.h"
#include "esvg_stop.h"
#include "esvg_style.h"
#include "esvg_text.h"
#include "esvg_use.h"
#include "esvg_video.h"

#include "esvg_parser.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT _esvg_parser_log

static int _esvg_parser_log = -1;

typedef struct _Esvg_Parser
{
	Ender_Element *topmost;
} Esvg_Parser;

static void _esvg_parser_tree_dump(Edom_Tag *t, int level);

static Eina_Bool _esvg_parser_tree_dump_cb(Edom_Tag *t, Edom_Tag *child, void *data)
{
	int *level = data;

	_esvg_parser_tree_dump(child, *level);
	return EINA_TRUE;
}

static void _esvg_parser_tree_dump(Edom_Tag *t, int level)
{
	char out[PATH_MAX];
	const char *name;
	int i;

	for (i = 0; i < level; i++)
		out[i] = ' ';
	out[i] = '\0';
	name = edom_tag_name_get(t);
	strncat(out, name ? name : "(UNKNOWN)", PATH_MAX - i);
	INFO("%s", out);
	level += 1;
	edom_tag_child_foreach(t, _esvg_parser_tree_dump_cb, &level);
}

static char * _esvg_parser_file_open(const char *filename, size_t *sz)
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

static Ender_Element * _esvg_parser_parse(Edom_Parser *parser, const char *buffer, size_t size)
{
	Ender_Element *e = NULL;
	Edom_Tag *t;

	if (!edom_parser_parse(parser, buffer, size))
	{
		DBG("Can not parse buffer");
		goto done;
	}

	/* check if we have a valid renderer */
	e = edom_parser_topmost_get(parser);
	/* useful for debugging */
	t = ender_element_object_get(e);
	_esvg_parser_tree_dump(t, 0);
done:
	edom_parser_delete(parser);
	return e;
}
/*----------------------------------------------------------------------------*
 *                            Edom parser interface                           *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_parser_tag_get(Edom_Parser *parser, const char *content,
		 size_t sz, int *tag)
{
	/* sz ==1 : 2 cases : a and g */
	if (sz == 1)
	{
		if (content[0] == 'a')
		{
			*tag = ESVG_A;
			return EINA_TRUE;
		}
		else if (content[0] == 'g')
		{
			*tag = ESVG_G;
			return EINA_TRUE;
		}
		else
		{
			DBG("tag <%s> not supported by SVG Tiny 1.1 spec", content);
		}
	}
	else if (sz == 2)
	{
		DBG("tag <%s> not supported by SVG Tiny 1.1 spec", content);
	}
	else if (sz == 3)
	{
		if (content[0] == 's')
		{
			if ((content[1] == 'e') && (content[2] == 't'))
			{
				*tag = ESVG_SET;
				return EINA_TRUE;
			}
			else if ((content[1] == 'v') && (content[2] == 'g'))
			{
				*tag = ESVG_SVG;
				return EINA_TRUE;
			}
			else
				DBG("tag <%s> not supported by SVG Tiny 1.1 spec", content);
		}
		else if (content[0] == 'u')
		{
			if ((content[1] == 's') && (content[2] == 'e'))
			{
				*tag = ESVG_USE;
				return EINA_TRUE;
			}
			else
				DBG("tag <%s> not supported by SVG Tiny 1.1 spec", content);
		}
		else
			DBG("tag <%s> not supported by SVG Tiny 1.1 spec", content);
	}
	else
	{
		if (content[0] == 'a')
		{
			if (strncmp("animate", content, sz) == 0)
			{
				*tag = ESVG_ANIMATE;
				return EINA_TRUE;
			}
			else if (strncmp("animateTransform", content, sz) == 0)
			{
				*tag = ESVG_ANIMATETRANSFORM;
				return EINA_TRUE;
			}

		}
		else if (content[0] == 'c')
		{
			if (strncmp("circle", content, sz) == 0)
			{
				*tag = ESVG_CIRCLE;
				return EINA_TRUE;
			}
			else if (strncmp("clipPath", content, sz) == 0)
			{
				*tag = ESVG_CLIPPATH;
				return EINA_TRUE;
			}
			else
				DBG("tag <%s> not supported by SVG Tiny 1.1 spec", content);
		}
		else if (content[0] == 'd')
		{
			if (strncmp("defs", content, sz) == 0)
			{
				*tag = ESVG_DEFS;
				return EINA_TRUE;
			}
			else if (strncmp("desc", content, sz) == 0)
			{
				*tag = ESVG_DESC;
				return EINA_TRUE;
			}
			else
				DBG("tag <%s> not supported by SVG Tiny 1.1 spec", content);
		}
		else if (content[0] == 'e')
		{
			if (strncmp("ellipse", content, sz) == 0)
			{
				*tag = ESVG_ELLIPSE;
				return EINA_TRUE;
			}
			else
				DBG("tag <%s> not supported by SVG Tiny 1.1 spec", content);
		}
		else if (content[0] == 'f')
		{
		}
		else if (content[0] == 'g')
		{
			/* <g> already managed */
			if (strncmp("glyph", content, sz) == 0)
			{
			}
			else if (strncmp("glyphRef", content, sz) == 0)
			{
			}
			else
				DBG("tag <%s> not supported by SVG Tiny 1.1 spec", content);
		}
		else if (content[0] == 'h')
		{
			if (strncmp("hkern", content, sz) == 0)
			{
			}
			else
				DBG("tag <%s> not supported by SVG Tiny 1.1 spec", content);
		}
		else if (content[0] == 'i')
		{
			if (strncmp("image", content, sz) == 0)
			{
				*tag = ESVG_IMAGE;
				return EINA_TRUE;
			}
			else
				DBG("tag <%s> not supported by SVG Tiny 1.1 spec", content);
		}
		else if (content[0] == 'l')
		{
			if (strncmp("line", content, sz) == 0)
			{
				*tag = ESVG_LINE;
				return EINA_TRUE;
			}
			else if (strncmp("linearGradient", content, sz) == 0)
			{
				*tag = ESVG_LINEARGRADIENT;
				return EINA_TRUE;
			}
			else
				DBG("tag <%s> not supported by SVG Tiny 1.1 spec", content);
		}
		else if (content[0] == 'm')
		{
		}
		else if (content[0] == 'p')
		{
			if (strncmp("path", content, sz) == 0)
			{
				*tag = ESVG_PATH;
				return EINA_TRUE;
			}
			else if (strncmp("pattern", content, sz) == 0)
			{
				*tag = ESVG_PATTERN;
				return EINA_TRUE;
			}
			else if (strncmp("polyline", content, sz) == 0)
			{
				*tag = ESVG_POLYLINE;
				return EINA_TRUE;
			}
			else if (strncmp("polygon", content, sz) == 0)
			{
				*tag = ESVG_POLYGON;
				return EINA_TRUE;
			}
		}
		else if (content[0] == 'r')
		{
			if (strncmp("radialGradient", content, sz) == 0)
			{
				*tag = ESVG_RADIALGRADIENT;
				return EINA_TRUE;
			}
			else if (strncmp("rect", content, sz) == 0)
			{
				*tag = ESVG_RECT;
				return EINA_TRUE;
			}
			else
				DBG("tag <%s> not supported by SVG Tiny 1.1 spec", content);
		}
		else if (content[0] == 's')
		{
			if (strncmp("script", content, sz) == 0)
			{
				*tag = ESVG_SCRIPT;
				return EINA_TRUE;
			}
			else if (strncmp("stop", content, sz) == 0)
			{
				*tag = ESVG_STOP;
				return EINA_TRUE;
			}
			else if (strncmp("style", content, sz) == 0)
			{
				*tag = ESVG_STYLE;
				return EINA_TRUE;
			}
		}
		else if (content[0] == 't')
		{
			if (strncmp("text", content, sz) == 0)
			{
				*tag = ESVG_TEXT;
				return EINA_TRUE;
			}
		}
		else if (content[0] == 'u')
		{
			/* <use> already managed */
			DBG("tag <%s> not supported by SVG Tiny 1.1 spec", content);
		}
		else if (content[0] == 'v')
		{
			if (strncmp("view", content, sz) == 0)
			{
			}
			else if (strncmp("vkern", content, sz) == 0)
			{
			}
			else if (strncmp("video", content, sz) == 0)
			{
				*tag = ESVG_VIDEO;
				return EINA_TRUE;
			}
			else
				DBG("tag <%s> not supported by SVG Tiny 1.1 spec", content);
		}
		else
			DBG("tag <%s> not supported by SVG Tiny 1.1 spec", content);
	}

	return EINA_FALSE;
}

static void * _esvg_parser_tag_new(Edom_Parser *parser, int tag_id)
{
	Esvg_Parser *thiz;
	Ender_Element *tag = NULL;

	thiz = edom_parser_data_get(parser);
	if (!thiz->topmost && tag_id != ESVG_SVG)
	{
		DBG("you need at least a topmost svg");
	}

	switch (tag_id)
	{
		case ESVG_LINEARGRADIENT:
                  tag = esvg_linear_gradient_new();
		break;

		case ESVG_RADIALGRADIENT:
		tag = esvg_radial_gradient_new();
		break;

#if 0
		case ESVG_PATTERN:
		tag = esvg_parser_pattern_new(parser);
		break;
#endif

		case ESVG_DEFS:
		tag = esvg_defs_new();
		break;

		case ESVG_USE:
		tag = esvg_use_new();
		break;

		case ESVG_SVG:
		tag = esvg_svg_new();
		if (!thiz->topmost)
			thiz->topmost = tag;
		break;

		case ESVG_CIRCLE:
		tag = esvg_circle_new();
		break;

		case ESVG_ELLIPSE:
		tag = esvg_ellipse_new();
		break;

		case ESVG_RECT:
		tag = esvg_rect_new();
		break;

		case ESVG_LINE:
		tag = esvg_line_new();
		break;

		case ESVG_PATH:
		tag = esvg_path_new();
		break;

		case ESVG_POLYLINE:
		tag = esvg_polyline_new();
		break;

		case ESVG_POLYGON:
		tag = esvg_polygon_new();
		break;

		case ESVG_TEXT:
		tag = esvg_text_new();
		break;

		case ESVG_G:
		tag = esvg_g_new();
		break;

		case ESVG_A:
		tag = esvg_a_new();
		break;

		case ESVG_SCRIPT:
		tag = esvg_script_new();
		break;

		case ESVG_STYLE:
		tag = esvg_style_new();
		break;

		case ESVG_IMAGE:
		tag = esvg_image_new();
		break;

		case ESVG_CLIPPATH:
		tag = esvg_clip_path_new();
		break;

		case ESVG_SET:
		tag = esvg_set_new();
		break;

		case ESVG_STOP:
		tag = esvg_stop_new();
		break;

		case ESVG_ANIMATE:
		tag = esvg_animate_new();
		break;

		case ESVG_ANIMATETRANSFORM:
		tag = esvg_animate_transform_new();
		break;

		case ESVG_VIDEO:
		tag = esvg_video_new();
		break;

		default:
		DBG("can't create the tag %s (%d)", esvg_type_string_to(tag_id), tag_id);
		break;
	}

	return tag;
}

static void * _esvg_parser_topmost_get(Edom_Parser *parser)
{
	Esvg_Parser *thiz;

	thiz = edom_parser_data_get(parser);
	return thiz->topmost;
}

static Eina_Bool _esvg_parser_tag_attribute_set(Edom_Parser *parser, void *tag, const char *name, const char *value)
{
	Ender_Element *e = tag;
	Edom_Tag *t;

	t = ender_element_object_get(e);
	edom_tag_attribute_set(t, name, value);
	return EINA_TRUE;
}

static Eina_Bool _esvg_parser_tag_child_add(Edom_Parser *parser, void *t, void *child)
{
	Ender_Element *tag = t;
	Edom_Tag *child_tag;

	/* add it */
	/* FIXME on add/remove functions ender should have a return value */
	child_tag = ender_element_object_get(child);
	ender_element_property_value_add(tag, EDOM_CHILD, child_tag, NULL);
	return EINA_TRUE;
}

static void _esvg_parser_tag_cdata_set(Edom_Parser *parser, void *t, const char *cdata, unsigned int length)
{
	Ender_Element *tag = t;
	Edom_String s;

	DBG("parser cdata %d", length);
	s.s = cdata;
	s.length = length;
	ender_element_property_value_set(tag, EDOM_CDATA, &s, NULL);
}

static void _esvg_parser_tag_text_set(Edom_Parser *parser, void *t, const char *text, unsigned int length)
{
	Ender_Element *tag = t;
	Edom_String s;

	s.s = text;
	s.length = length;
	ender_element_property_value_set(tag, EDOM_TEXT, &s, NULL);
}

static Edom_Parser_Descriptor _descriptor = {
	/* .tag_get 		= */ _esvg_parser_tag_get,
	/* .tag_new 		= */ _esvg_parser_tag_new,
	/* .topmost_get 	= */ _esvg_parser_topmost_get,
	/* .tag_attribute_set 	= */ _esvg_parser_tag_attribute_set,
	/* .tag_child_add 	= */ _esvg_parser_tag_child_add,
	/* .tag_cdata_set 	= */ _esvg_parser_tag_cdata_set,
	/* .tag_text_set 	= */ _esvg_parser_tag_text_set,

};
/*----------------------------------------------------------------------------*
 *                            Edom parser interface                           *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_parser_info_tag_get(Edom_Parser *parser, const char *content,
		 size_t sz, int *tag)
{
	if (strncmp("svg", content, sz) == 0)
	{
		*tag = ESVG_SVG;
		return EINA_TRUE;
	}
	return EINA_FALSE;
}

static void * _esvg_parser_info_tag_new(Edom_Parser *parser, int tag_id)
{
	Esvg_Parser *thiz;
	Ender_Element *tag = NULL;

	thiz = edom_parser_data_get(parser);
	tag = esvg_svg_new();
	if (!thiz->topmost)
		thiz->topmost = tag;
	return tag;
}

static Edom_Parser_Descriptor _info_descriptor = {
	/* .tag_get 		= */ _esvg_parser_info_tag_get,
	/* .tag_new 		= */ _esvg_parser_info_tag_new,
	/* .topmost_get 	= */ _esvg_parser_topmost_get,
	/* .tag_attribute_set 	= */ _esvg_parser_tag_attribute_set,
	/* .tag_child_add 	= */ NULL,
	/* .tag_cdata_set 	= */ NULL,
	/* .tag_text_set 	= */ NULL,
};

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_parser_init(void)
{
	_esvg_parser_log = eina_log_domain_register("esvg_parser", ESVG_LOG_COLOR_DEFAULT);
	if (_esvg_parser_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
}

void esvg_parser_shutdown(void)
{
	if (_esvg_parser_log < 0)
		return;
	eina_log_domain_unregister(_esvg_parser_log);
	_esvg_parser_log = -1;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * @param width The parsed svg's width attribute
 * @param height The parsed svg's height attribute
 */
EAPI Eina_Bool esvg_parser_info_load(const char *filename,
		Esvg_Length *width, Esvg_Length *height)
{
	Ender_Element *e;
	Eina_Bool ret;
	char *buffer;
	size_t size;

	buffer = _esvg_parser_file_open(filename, &size);
	if (!buffer) return EINA_FALSE;

	ret = esvg_parser_info_load_from_buffer(buffer, size, width, height);
	free(buffer);
	return ret;
}

EAPI Eina_Bool esvg_parser_info_load_from_buffer(const char *buffer,
		size_t size, Esvg_Length *width, Esvg_Length *height)
{
	Esvg_Parser *thiz;
	Edom_Parser *info_parser;
	Ender_Element *e;

	thiz = calloc(1, sizeof(Esvg_Parser));
	info_parser = edom_parser_new(&_info_descriptor, thiz);
	e = _esvg_parser_parse(info_parser, buffer, size);
	if (!e) return EINA_FALSE;

	esvg_svg_width_get(e, width);
	esvg_svg_height_get(e, height);
	//ender_element_unref(e);

	return EINA_TRUE;
}

/**
 *
 */
EAPI Ender_Element * esvg_parser_load(const char *filename)
{
	Ender_Element *e;
	char *buffer;
	size_t size;

	buffer = _esvg_parser_file_open(filename, &size);
	if (!buffer) return NULL;

	e = esvg_parser_load_from_buffer(buffer, size);
	free(buffer);

	return e;
}

EAPI Ender_Element * esvg_parser_load_from_buffer(const char *buffer, size_t size)
{
	Esvg_Parser *thiz;
	Edom_Parser *parser;
	Ender_Element *e;

	thiz = calloc(1, sizeof(Esvg_Parser));
	parser = edom_parser_new(&_descriptor, thiz);
	return _esvg_parser_parse(parser, buffer, size);
}

/**
 * FIXME for later
 */
EAPI void esvg_parser_save(Enesim_Renderer *r, const char *filename)
{

}

