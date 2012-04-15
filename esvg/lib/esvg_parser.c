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

#include "Esvg.h"
#include "Esvg_Parser.h"
#include "esvg_private_main.h"
#include "esvg_parser_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Esvg_Parser_Post_Data
{
	Esvg_Parser_Post cb;
	void *data;
} Esvg_Parser_Post_Data;

typedef struct _Esvg_Parser
{
	Eina_List *post_parsers;
	Esvg_Parser_Descriptor descriptor;
	Ender_Element *topmost;
	void *data;
} Esvg_Parser;

/**
 * @cond LOCAL
 */

static char * _esvg_parser_file_open(const char *filename, long *sz)
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

static Ender_Element * _esvg_parser_file_parse(const char *filename, Edom_Parser *parser)
{
	Ender_Element *tag = NULL;
	char *buf;
	long sz;

	edom_parser_location_set(parser, filename);
	buf = _esvg_parser_file_open(filename, &sz);
	if (!buf)
	{
		DBG("Can not open file %s\n", filename);
		return NULL;
	}

	if (!edom_parser_parse(parser, buf, sz))
	{
		DBG("Can not parse file %s\n", filename);
		goto parse_failed;
	}

	/* check if we have a valid renderer */
	tag = edom_parser_topmost_get(parser);

parse_failed:
	free(buf);
	return tag;
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
			if (strncmp("stop", content, sz) == 0)
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
		printf("you need at least a topmost svg\n");
	}

	switch (tag_id)
	{
#if 0
		case ESVG_LINEARGRADIENT:
		tag = esvg_parser_linear_gradient_new(parser);
		break;

		case ESVG_RADIALGRADIENT:
		tag = esvg_parser_radial_gradient_new(parser);
		break;

		case ESVG_PATTERN:
		tag = esvg_parser_pattern_new(parser);
		break;

		case ESVG_DEFS:
		tag = esvg_parser_defs_new(parser);
		break;

		case ESVG_USE:
		tag = esvg_parser_use_new(parser);
		break;
#endif
		case ESVG_SVG:
		tag = esvg_svg_new();
		if (!thiz->topmost)
			thiz->topmost = tag;
		break;
#if 0
		case ESVG_CIRCLE:
		tag = esvg_parser_circle_new(parser);
		break;

		case ESVG_ELLIPSE:
		tag = esvg_parser_ellipse_new(parser);
		break;

		case ESVG_RECT:
		tag = esvg_parser_rect_new(parser);
		break;

		case ESVG_LINE:
		tag = esvg_parser_line_new(parser);
		break;

		case ESVG_PATH:
		tag = esvg_parser_path_new(parser);
		break;

		case ESVG_POLYLINE:
		tag = esvg_parser_polyline_new(parser);
		break;

		case ESVG_POLYGON:
		tag = esvg_parser_polygon_new(parser);
		break;

		case ESVG_TEXT:
		tag = esvg_parser_text_new(parser);
		break;

		case ESVG_G:
		tag = esvg_parser_g_new(parser);
		break;

		case ESVG_A:
		tag = esvg_parser_a_new(parser);
		break;

		case ESVG_STYLE:
		//tag = esvg_parser_style_new(parser);
		break;

		case ESVG_IMAGE:
		tag = esvg_parser_image_new(parser);
		break;

		case ESVG_CLIPPATH:
		tag = esvg_parser_clip_path_new(parser);
		break;

		case ESVG_STOP:
		tag = esvg_parser_stop_new(parser);
		break;
#endif
		default:
		printf("can't create the tag %d\n", tag_id);
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

static Eina_Bool _esvg_parser_tag_attribute_set(Edom_Parser *parser, void *t, const char *name, const char *value)
{
	Ender_Element *tag = t;
	Edom_Attribute attribute;

	attribute.name = name;
	attribute.value = value;
	ender_element_property_value_set(tag, EDOM_ATTRIBUTE, &attribute, NULL);
	return EINA_TRUE;
}

static Eina_Bool _esvg_parser_tag_child_add(Edom_Parser *parser, void *t, void *child)
{
	Ender_Element *tag = t;

	/* add it */
	/* FIXME on add/remove functions ender should have a return value */
}

static void _esvg_parser_tag_cdata_set(Edom_Parser *parser, void *t, const char *cdata, unsigned int length)
{

}

static void _esvg_parser_tag_text_set(Edom_Parser *parser, void *t, const char *text, unsigned int length)
{

}

static Edom_Parser_Descriptor _descriptor = {
	/* .tag_get 	= */ _esvg_parser_tag_get,
	/* .tag_new 	= */ _esvg_parser_tag_new,
	/* .topmost_get = */ _esvg_parser_topmost_get,
	/* .tag_attribute_set = */ _esvg_parser_tag_attribute_set,
	/* .tag_child_add = */ _esvg_parser_tag_child_add,
	/* .tag_cdata_set = */ _esvg_parser_tag_cdata_set,
	/* .tag_text_set = */ _esvg_parser_tag_text_set,

};

/*----------------------------------------------------------------------------*
 *                            Edom parser interface                           *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_parser_info_tag_get(Edom_Parser *parser, const char *content,
		 size_t sz, int *tag)
{
	if (strncmp("esvg", content, sz) == 0)
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
	if (!thiz->topmost && tag_id != ESVG_SVG)
	{
		printf("you need at least a topmost svg\n");
	}
#if 0
	tag = esvg_parser_svg_new(parser);
	if (!thiz->topmost)
		thiz->topmost = tag;
#endif
	return tag;
}

static Edom_Parser_Descriptor _info_descriptor = {
	/* .tag_get 	= */ _esvg_parser_info_tag_get,
	/* .tag_new 	= */ _esvg_parser_info_tag_new,
	/* .topmost_get = */ _esvg_parser_topmost_get,
	/* .tag_attribute_set = */ NULL,
	/* .tag_child_add = */ NULL,
	/* .tag_cdata_set = */ NULL,
	/* .tag_text_set = */ NULL,
};

/**
 * @endcond
 */

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_parser_post_parse_add(Edom_Parser *p, Esvg_Parser_Post cb, void *data)
{
	Esvg_Parser *thiz;
	Esvg_Parser_Post_Data *pdata;

	thiz = edom_parser_data_get(p);
	pdata = malloc(sizeof(Esvg_Parser_Post_Data));
	pdata->cb = cb;
	pdata->data = data;

	thiz->post_parsers = eina_list_append(thiz->post_parsers, pdata);
}

/* functions to call the descriptor functions */
void esvg_parser_href_set(Edom_Parser *p, Enesim_Renderer *r, const char *href)
{
	Esvg_Parser *thiz;

	thiz = edom_parser_data_get(p);
	if (thiz->descriptor.href_set)
		return thiz->descriptor.href_set(thiz->data, r, href);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 *
 */
EAPI Eina_Bool esvg_parser_info_load(const char *filename,
		double *width, double *height)
{
	Esvg_Parser *thiz;
	Edom_Parser *info_parser;
	Ender_Element *tag;
	Enesim_Renderer *r;
	Eina_Bool ret = EINA_FALSE;

	thiz = calloc(1, sizeof(Esvg_Parser));
	info_parser = edom_parser_new(&_info_descriptor, thiz);
	tag = _esvg_parser_file_parse(filename, info_parser);
	edom_parser_delete(info_parser);

	if (tag)
	{
		//edom_tag_unref(tag);
		ret = EINA_TRUE;
	}

	return ret;
}

/**
 *
 */
EAPI Ender_Element * esvg_parser_load(const char *filename,
		Esvg_Parser_Descriptor *descriptor, void *data)
{
	Esvg_Parser *thiz;
	Esvg_Parser_Post_Data *pdata;
	Edom_Parser *parser;
	Ender_Element *tag;
	Eina_List *l;

	thiz = calloc(1, sizeof(Esvg_Parser));
	thiz->data = data;
	if (descriptor)
		thiz->descriptor = *descriptor;

	parser = edom_parser_new(&_descriptor, thiz);
	tag = _esvg_parser_file_parse(filename, parser);
	if (!tag) goto parse_failed;

	//edom_tag_dump(tag);
	/* FIXME we should handle the style thing correctly
	 * so far we were applying it once the document is loaded
	 * this is fine, but what will happen whenever some style
	 * is being modified *after* rendering?
	 * we need to handle that case now that we start supporting
	 * the whole tree on the lib
	 */

	//esvg_parser_svg_style_apply(tag);
	/* FIXME all the link property of the <use> tags
	 * are created at parse time, in case we apply a style
	 * for the linked element, it wont be propagated to
	 * the clone
	 */
	/* call all the post parse functions */
	EINA_LIST_FOREACH(thiz->post_parsers, l, pdata)
	{
		pdata->cb(parser, pdata->data);
	}

parse_failed:
	edom_parser_delete(parser);

	return tag;
}

/**
 * FIXME for later
 */
EAPI void esvg_parser_save(Enesim_Renderer *r, const char *filename)
{

}

