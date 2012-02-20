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

#include <stdlib.h>
#include <stdio.h>

#include <Eina.h>

#include "Esvg.h"
#include "Esvg_Parser.h"
#include "esvg_private.h"
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
	void *data;
} Esvg_Parser;

/**
 * @cond LOCAL
 */

static char * _esvg_file_open(const char *filename, long *sz)
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

/*----------------------------------------------------------------------------*
 *                            Edom parser interface                           *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_parser_tag_get(Edom_Parser *parser, const char *content,
		 size_t sz, int *tag)
{
	Esvg_Parser *thiz;

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

static Edom_Parser_Descriptor _descriptor = {
	/* .tag_get 	= */ _esvg_parser_tag_get,
};

#define TAGNAME(t) 								\
	case t:									\
	return #t;

/* TODO this has to go away */
const char * esvg_parser_tag_type_string_to(Esvg_Parser_Tag_Type type)
{
	switch (type)
	{
		TAGNAME(ESVG_A)
		TAGNAME(ESVG_ALTGLYPH)
		TAGNAME(ESVG_ALTGLYPHDEF)
		TAGNAME(ESVG_ALTGLYPHITEM)
		TAGNAME(ESVG_ANIMATE)
		TAGNAME(ESVG_ANIMATECOLOR)
		TAGNAME(ESVG_ANIMATEMOTION)
		TAGNAME(ESVG_ANIMATETRANSFORM)
		TAGNAME(ESVG_CIRCLE)
		TAGNAME(ESVG_CLIPPATH)
		TAGNAME(ESVG_COLOR_PROFILE)
		TAGNAME(ESVG_CURSOR)
		TAGNAME(ESVG_DEFINITION_SRC)
		TAGNAME(ESVG_DEFS)
		TAGNAME(ESVG_DESC)
		TAGNAME(ESVG_ELLIPSE)
		TAGNAME(ESVG_FEBLEND)
		TAGNAME(ESVG_FECOLORMATRIX)
		TAGNAME(ESVG_FECOMPONENTTRANSFER)
		TAGNAME(ESVG_FECOMPOSITE)
		TAGNAME(ESVG_FECONVOLVEMATRIX)
		TAGNAME(ESVG_FEDIFFUSELIGHTING)
		TAGNAME(ESVG_FEDISPLACEMENTMAP)
		TAGNAME(ESVG_FEDISTANTLIGHT)
		TAGNAME(ESVG_FEFLOOD)
		TAGNAME(ESVG_FEFUNCA)
		TAGNAME(ESVG_FEFUNCB)
		TAGNAME(ESVG_FEFUNCG)
		TAGNAME(ESVG_FEFUNCR)
		TAGNAME(ESVG_FEGAUSSIANBLUR)
		TAGNAME(ESVG_FEIMAGE)
		TAGNAME(ESVG_FEMERGE)
		TAGNAME(ESVG_FEMERGENODE)
		TAGNAME(ESVG_FEMORPHOLOGY)
		TAGNAME(ESVG_FEOFFSET)
		TAGNAME(ESVG_FEPOINTLIGHT)
		TAGNAME(ESVG_FESPECULARLIGHTING)
		TAGNAME(ESVG_FESPOTLIGHT)
		TAGNAME(ESVG_FETILE)
		TAGNAME(ESVG_FETURBULENCE)
		TAGNAME(ESVG_FILTER)
		TAGNAME(ESVG_FONT)
		TAGNAME(ESVG_FONT_FACE)
		TAGNAME(ESVG_FONT_FACE_FORMAT)
		TAGNAME(ESVG_FONT_FACE_NAME)
		TAGNAME(ESVG_FONT_FACE_SRC)
		TAGNAME(ESVG_FONT_FACE_URI)
		TAGNAME(ESVG_FOREIGNOBJECT)
		TAGNAME(ESVG_G)
		TAGNAME(ESVG_GLYPH)
		TAGNAME(ESVG_GLYPHREF)
		TAGNAME(ESVG_HKERN)
		TAGNAME(ESVG_IMAGE)
		TAGNAME(ESVG_LINE)
		TAGNAME(ESVG_LINEARGRADIENT)
		TAGNAME(ESVG_MARKER)
		TAGNAME(ESVG_MASK)
		TAGNAME(ESVG_METADATA)
		TAGNAME(ESVG_MISSING_GLYPH)
		TAGNAME(ESVG_MPATH)
		TAGNAME(ESVG_PATH)
		TAGNAME(ESVG_PATTERN)
		TAGNAME(ESVG_POLYGON)
		TAGNAME(ESVG_POLYLINE)
		TAGNAME(ESVG_RADIALGRADIENT)
		TAGNAME(ESVG_RECT)
		TAGNAME(ESVG_SCRIPT)
		TAGNAME(ESVG_SET)
		TAGNAME(ESVG_STOP)
		TAGNAME(ESVG_STYLE)
		TAGNAME(ESVG_SVG)
		TAGNAME(ESVG_SWITCH)
		TAGNAME(ESVG_SYMBOL)
		TAGNAME(ESVG_TEXT)
		TAGNAME(ESVG_TEXTPATH)
		TAGNAME(ESVG_TITLE)
		TAGNAME(ESVG_TREF)
		TAGNAME(ESVG_TSPAN)
		TAGNAME(ESVG_USE)
		TAGNAME(ESVG_VIEW)
		TAGNAME(ESVG_VKERN)
		default:
		return "ESVG_UNKNOWN";
	}
}


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
EAPI Enesim_Renderer * esvg_parser_load(const char *filename,
		Esvg_Parser_Descriptor *descriptor, void *data)
{
	Esvg_Parser *thiz;
	Esvg_Parser_Post_Data *pdata;
	Edom_Parser *parser;
	Edom_Context *c;
	Edom_Tag *tag;
	Enesim_Renderer *r = NULL;
	Eina_List *l;
	char *buf;
	long sz;

	thiz = calloc(1, sizeof(Esvg_Parser));
	thiz->data = data;
	if (descriptor)
		thiz->descriptor = *descriptor;

	parser = edom_parser_new(&_descriptor, thiz);

	edom_parser_location_set(parser, filename);
	/* create the doc context */
	c = esvg_parser_context_doc_new(parser);

	buf = _esvg_file_open(filename, &sz);
	if (!buf)
	{
		DBG("Can not open file %s\n", filename);
		goto open_failed;
	}

	if (!edom_parser_parse(parser, c, buf, sz))
	{
		DBG("Can not parse file %s\n", filename);
		goto parse_failed;
	}

	/* check if we have a valid renderer */
	tag = esvg_parser_context_doc_topmost_get(c);
	if (!tag)
		goto end;

	esvg_parser_svg_style_apply(tag);
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

	r = esvg_parser_element_renderer_get(tag);
end:
	edom_context_delete(c);
parse_failed:
	free(buf);
open_failed:
	edom_parser_delete(parser);

	return r;
}

/**
 * FIXME for later
 */
EAPI void esvg_parser_save(Enesim_Renderer *r, const char *filename)
{

}

