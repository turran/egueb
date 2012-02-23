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
#include "esvg_parser_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Esvg_Parser_Context_Simple
{
	int tag;
	Edom_Tag *svg;
	Edom_Tag *parent;
	Enesim_Renderer *parent_r;
	Esvg_Parser_Context_Simple_Descriptor *descriptor;
	void *data;
} Esvg_Parser_Context_Simple;

/*----------------------------------------------------------------------------*
 *                            Clip path parsing                               *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_simple_clip_path(Edom_Context *context,
		Esvg_Parser_Context_Simple *thiz,
		Edom_Parser *parser)
{
	Edom_Context *new_context;
	Edom_Tag *tag;
	Enesim_Renderer *r;

	tag = esvg_parser_clip_path_new(context, thiz->svg);
	r = esvg_parser_element_renderer_get(tag);
	if (thiz->parent_r)
		esvg_container_element_add(thiz->parent_r, r);

	new_context = esvg_parser_context_clip_path_new(thiz->svg,
			tag, r);
	edom_parser_context_push(parser, new_context);

	return tag;
}
/*----------------------------------------------------------------------------*
 *                              Style parsing                                 *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_simple_style(Edom_Context *context,
		Esvg_Parser_Context_Simple *thiz,
		Edom_Parser *parser)
{
	Edom_Context *new_context;
	Edom_Tag *tag;
	Esvg_Parser_Style *style;

	style = esvg_parser_style_new();

	tag = edom_tag_new(context, NULL, ESVG_STYLE, thiz->svg, style);
	esvg_parser_svg_style_add(thiz->svg, style);

	new_context = esvg_parser_context_style_new(tag);
	edom_parser_context_push(parser, new_context);

	return tag;
}
/*----------------------------------------------------------------------------*
 *                                A parsing                                   *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_simple_a(Edom_Context *context,
		Esvg_Parser_Context_Simple *thiz,
		Edom_Parser *parser)
{
	Edom_Context *new_context;
	Edom_Tag *tag;

	tag = esvg_parser_a_new(context, thiz->svg);
	new_context = esvg_parser_context_a_new(thiz->svg,
			tag, thiz->parent_r);
	edom_parser_context_push(parser, new_context);

	return tag;
}
/*----------------------------------------------------------------------------*
 *                                G parsing                                   *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_simple_g(Edom_Context *context,
		Esvg_Parser_Context_Simple *thiz,
		Edom_Parser *parser)
{
	Edom_Context *new_context;
	Edom_Tag *tag;
	Enesim_Renderer *r;

	tag = esvg_parser_g_new(context, thiz->svg);
	r = esvg_parser_element_renderer_get(tag);
	if (thiz->parent_r)
	{
		esvg_container_element_add(thiz->parent_r, r);
	}

	new_context = esvg_parser_context_g_new(thiz->svg,
			tag, r);
	edom_parser_context_push(parser, new_context);

	return tag;
}
/*----------------------------------------------------------------------------*
 *                               Rect parsing                                 *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_simple_rect(Edom_Context *context,
		Esvg_Parser_Context_Simple *thiz,
		Edom_Parser *parser)
{
	Edom_Tag *tag;
	Enesim_Renderer *r;

	tag = esvg_parser_rect_new(context, thiz->svg);
	if (thiz->parent_r)
	{
		r = esvg_parser_element_renderer_get(tag);
		esvg_container_element_add(thiz->parent_r, r);
	}

	return tag;
}
/*----------------------------------------------------------------------------*
 *                               Line parsing                                 *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_simple_line(Edom_Context *context,
		Esvg_Parser_Context_Simple *thiz,
		Edom_Parser *parser)
{
	Edom_Tag *tag;

	tag = esvg_parser_line_new(context, thiz->svg);
	if (thiz->parent_r)
	{
		Enesim_Renderer *r;

		r = esvg_parser_element_renderer_get(tag);
		esvg_container_element_add(thiz->parent_r, r);
	}

	return tag;
}
/*----------------------------------------------------------------------------*
 *                               Ellipse parsing                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_simple_ellipse(Edom_Context *context,
		Esvg_Parser_Context_Simple *thiz,
		Edom_Parser *parser)
{
	Edom_Tag *tag;

	tag = esvg_parser_ellipse_new(context, thiz->svg);
	if (thiz->parent_r)
	{
		Enesim_Renderer *r;

		r = esvg_parser_element_renderer_get(tag);
		esvg_container_element_add(thiz->parent_r, r);
	}

	return tag;
}
/*----------------------------------------------------------------------------*
 *                               Circle parsing                               *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_simple_circle(Edom_Context *context,
		Esvg_Parser_Context_Simple *thiz,
		Edom_Parser *parser)
{
	Edom_Tag *tag;

	tag = esvg_parser_circle_new(context, thiz->svg);
	if (thiz->parent_r)
	{
		Enesim_Renderer *r;

		r = esvg_parser_element_renderer_get(tag);
		esvg_container_element_add(thiz->parent_r, r);
	}

	return tag;
}
/*----------------------------------------------------------------------------*
 *                                Path parsing                                *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_simple_path(Edom_Context *context,
		Esvg_Parser_Context_Simple *thiz,
		Edom_Parser *parser)
{
	Edom_Tag *tag;
	Enesim_Renderer *r;

	tag = esvg_parser_path_new(context, thiz->svg);
	if (thiz->parent_r)
	{
		r = esvg_parser_element_renderer_get(tag);
		esvg_container_element_add(thiz->parent_r, r);
	}

	return tag;
}
/*----------------------------------------------------------------------------*
 *                              Polygon parsing                               *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_simple_polygon(Edom_Context *context,
		Esvg_Parser_Context_Simple *thiz,
		Edom_Parser *parser)
{
	Edom_Tag *tag;

	tag = esvg_parser_polygon_new(context, thiz->svg);
	if (thiz->parent_r)
	{
		Enesim_Renderer *r;

		r = esvg_parser_element_renderer_get(tag);
		esvg_container_element_add(thiz->parent_r, r);
	}

	return tag;
}
/*----------------------------------------------------------------------------*
 *                             Polyline parsing                               *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_simple_polyline(Edom_Context *context,
		Esvg_Parser_Context_Simple *thiz,
		Edom_Parser *parser)
{
	Edom_Tag *tag;

	tag = esvg_parser_polyline_new(context, thiz->svg);
	if (thiz->parent_r)
	{
		Enesim_Renderer *r;

		r = esvg_parser_element_renderer_get(tag);
		esvg_container_element_add(thiz->parent_r, r);
	}

	return tag;
}
/*----------------------------------------------------------------------------*
 *                                Text parsing                                *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_simple_text(Edom_Context *context,
		Esvg_Parser_Context_Simple *thiz,
		Edom_Parser *parser)
{
	Edom_Tag *tag;
	Edom_Context *new_context;

	tag = esvg_parser_text_new(context, thiz->svg);
	if (thiz->parent_r)
	{
		Enesim_Renderer *r;

		r = esvg_parser_element_renderer_get(tag);
		esvg_container_element_add(thiz->parent_r, r);
	}

	new_context = esvg_parser_context_text_new(tag);
	edom_parser_context_push(parser, new_context);

	return tag;
}
/*----------------------------------------------------------------------------*
 *                                Svg parsing                                 *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_simple_svg(Edom_Context *context,
		Esvg_Parser_Context_Simple *thiz,
		Edom_Parser *parser)
{
	Edom_Tag *tag;
	Edom_Context *new_context;
	Enesim_Renderer *r;

	tag = esvg_parser_svg_new(context, thiz->svg);
	r = esvg_parser_element_renderer_get(tag);
	if (thiz->parent_r)
		esvg_container_element_add(thiz->parent_r, r);

	new_context = esvg_parser_context_svg_new(tag, r);
	edom_parser_context_push(parser, new_context);

	return tag;
}
/*----------------------------------------------------------------------------*
 *                              Image parsing                                 *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_simple_image(Edom_Context *context,
		Esvg_Parser_Context_Simple *thiz,
		Edom_Parser *parser)
{
	Edom_Tag *tag;

	tag = esvg_parser_image_new(context, thiz->svg);
	if (thiz->parent_r)
	{
		Enesim_Renderer *r;

		r = esvg_parser_element_renderer_get(tag);
		esvg_container_element_add(thiz->parent_r, r);
	}

	return tag;
}
/*----------------------------------------------------------------------------*
 *                               Defs parsing                                 *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_simple_defs(Edom_Context *context,
		Esvg_Parser_Context_Simple *thiz,
		Edom_Parser *parser)
{
	Edom_Context *new_context;
	Edom_Tag *tag;

	tag = edom_tag_new(context, NULL, ESVG_DEFS, thiz->svg, NULL);

	new_context = esvg_parser_defs_new(tag);
	edom_parser_context_push(parser, new_context);

	return tag;
}
/*----------------------------------------------------------------------------*
 *                               Use parsing                                  *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_simple_use(Edom_Context *context,
		Esvg_Parser_Context_Simple *thiz,
		Edom_Parser *parser)
{
	Edom_Tag *tag;

	tag = esvg_parser_use_new(context, thiz->svg);
	if (thiz->parent_r)
	{
		Enesim_Renderer *r;

		r = esvg_parser_element_renderer_get(tag);
		esvg_container_element_add(thiz->parent_r, r);
	}

	return tag;
}
/*----------------------------------------------------------------------------*
 *                              Pattern parsing                               *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_container_pattern(Edom_Context *context,
		Esvg_Parser_Context_Simple *thiz,
		Edom_Parser *parser)
{
	Edom_Tag *tag;
	Edom_Context *new_context;

	tag = esvg_parser_pattern_new(context, thiz->svg);
	if (thiz->parent_r)
	{
		Enesim_Renderer *r;

		r = esvg_parser_element_renderer_get(tag);
		esvg_container_element_add(thiz->parent_r, r);
	}

	return tag;
}
/*----------------------------------------------------------------------------*
 *                         Linear gradient parsing                            *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_simple_linear_gradient(Edom_Context *context,
		Esvg_Parser_Context_Simple *thiz,
		Edom_Parser *parser)
{
	Edom_Tag *tag;
	Edom_Context *new_context;
	Enesim_Renderer *r;

	tag = esvg_parser_linear_gradient_new(context, thiz->svg);
	r = esvg_parser_element_renderer_get(tag);

	new_context = esvg_parser_context_gradient_new(tag, r);
	edom_parser_context_push(parser, new_context);

	return tag;
}
/*----------------------------------------------------------------------------*
 *                         Radial gradient parsing                            *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_simple_radial_gradient(Edom_Context *context,
		Esvg_Parser_Context_Simple *thiz,
		Edom_Parser *parser)
{
	Edom_Tag *tag;
	Edom_Context *new_context;
	Enesim_Renderer *r;

	tag = esvg_parser_radial_gradient_new(context, thiz->svg);
	r = esvg_parser_element_renderer_get(tag);

	new_context = esvg_parser_context_gradient_new(tag, r);
	edom_parser_context_push(parser, new_context);

	return tag;
}

static Edom_Tag * _context_tag_new(Esvg_Parser_Context_Simple *thiz,
		int tag, Edom_Context *context, Edom_Parser *parser)
{
	Edom_Tag *new_tag = NULL;

	switch (tag)
	{
		case ESVG_LINEARGRADIENT:
		new_tag = _esvg_parser_context_simple_linear_gradient(context, thiz, parser);
		break;

		case ESVG_RADIALGRADIENT:
		new_tag = _esvg_parser_context_simple_radial_gradient(context, thiz, parser);
		break;

		case ESVG_PATTERN:
		new_tag = _esvg_parser_container_pattern(context, thiz, parser);
		break;

		case ESVG_DEFS:
		new_tag = _esvg_parser_context_simple_defs(context, thiz, parser);
		break;

		case ESVG_USE:
		new_tag = _esvg_parser_context_simple_use(context, thiz, parser);
		break;

		case ESVG_SVG:
		new_tag = _esvg_parser_context_simple_svg(context, thiz, parser);
		break;

		case ESVG_CIRCLE:
		new_tag = _esvg_parser_context_simple_circle(context, thiz, parser);
		break;

		case ESVG_ELLIPSE:
		new_tag = _esvg_parser_context_simple_ellipse(context, thiz, parser);
		break;

		case ESVG_RECT:
		new_tag = _esvg_parser_context_simple_rect(context, thiz, parser);
		break;

		case ESVG_LINE:
		new_tag = _esvg_parser_context_simple_line(context, thiz, parser);
		break;

		case ESVG_PATH:
		new_tag = _esvg_parser_context_simple_path(context, thiz, parser);
		break;

		case ESVG_POLYLINE:
		new_tag = _esvg_parser_context_simple_polyline(context, thiz, parser);
		break;

		case ESVG_POLYGON:
		new_tag = _esvg_parser_context_simple_polygon(context, thiz, parser);
		break;

		case ESVG_TEXT:
		new_tag = _esvg_parser_context_simple_text(context, thiz, parser);
		break;

		case ESVG_G:
		new_tag = _esvg_parser_context_simple_g(context, thiz, parser);
		break;

		case ESVG_A:
		new_tag = _esvg_parser_context_simple_a(context, thiz, parser);
		break;

		case ESVG_STYLE:
		/* FIXME store the style? */
		new_tag = _esvg_parser_context_simple_style(context, thiz, parser);
		break;

		case ESVG_IMAGE:
		new_tag = _esvg_parser_context_simple_image(context, thiz, parser);
		break;

		case ESVG_CLIPPATH:
		new_tag = _esvg_parser_context_simple_clip_path(context, thiz, parser);
		break;

		default:
		break;
	}
	return new_tag;
}
/*----------------------------------------------------------------------------*
 *                         The context interface                               *
 *----------------------------------------------------------------------------*/
static Eina_Bool _context_tag_open(void *data, int tag,
		Edom_Context *context, const char *attributes,
		unsigned int length)
{
	Esvg_Parser_Context_Simple *thiz = data;
	Edom_Parser *parser;
	Edom_Tag *new_tag = NULL;
	Eina_Bool ret = EINA_TRUE;

	/* first check if the tag is supported */
	if (!thiz->descriptor->tag_is_supported(thiz->data, tag))
	{
		printf("parsing error. tag is not suppoted %d\n", tag);
		return EINA_FALSE;
	}
	/* if so, create it */
	parser = edom_context_parser_get(context);
	new_tag = _context_tag_new(thiz, tag, context, parser);
	/* FIXME this is wrong. this new tag should be passed to the context tag */
	if (new_tag)
	{
		edom_tag_attributes_from_xml(new_tag, attributes, length);
		edom_tag_child_add(thiz->parent, new_tag);
		esvg_parser_svg_tag_add(thiz->svg, new_tag);
		if (thiz->descriptor->tag_added)
		{
			thiz->descriptor->tag_added(thiz->data, new_tag);
		}
	}

	return ret;
}

static void _context_tag_close(void *data, int tag,
		Edom_Context *context)
{
	Esvg_Parser_Context_Simple *thiz = data;

	/* destroy ourselves */
	if (thiz->tag == tag)
	{
		Edom_Parser *parser;

		parser = edom_context_parser_get(context);
		context = edom_parser_context_pop(parser);
		edom_context_delete(context);
		return;
	}
}

static void _context_free(void *data)
{
	Esvg_Parser_Context_Simple *thiz = data;
	free(thiz);
}

static Edom_Context_Descriptor _descriptor = {
	/* .tag_open = 	*/ _context_tag_open,
	/* .tag_close =	*/ _context_tag_close,
	/* .data =	*/ NULL,
	/* .cdata =	*/ NULL,
	/* .free = 	*/ _context_free,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Edom_Context * esvg_parser_context_simple_new(int tag, Edom_Tag *svg, Edom_Tag *parent,
		Enesim_Renderer *parent_r,
		Esvg_Parser_Context_Simple_Descriptor *descriptor,
		void *data)
{
	Esvg_Parser_Context_Simple *thiz;
	Edom_Parser *parser;

	if (!svg) return NULL;
	if (!descriptor) return NULL;

	thiz = calloc(1, sizeof(Esvg_Parser_Context_Simple));
	thiz->tag = tag;
	thiz->svg = svg;
	thiz->parent = parent;
	thiz->parent_r = parent_r;
	thiz->descriptor = descriptor;
	thiz->data = data;

	parser = edom_tag_parser_get(svg);
	return esvg_parser_context_new(parser, &_descriptor, thiz);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
