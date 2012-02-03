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
#include "esvg_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Esvg_Parser_Context
{
	int tag;
	Edom_Tag *svg;
	Edom_Tag *parent;
	Enesim_Renderer *parent_r;
	Esvg_Parser_Context_Tag_Is_Supported tag_is_supported;
} Esvg_Parser_Context;

/*----------------------------------------------------------------------------*
 *                            Clip path parsing                               *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_clip_path(Edom_Context *context,
		Esvg_Parser_Context *thiz,
		Eina_Array *contexts)
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
	eina_array_push(contexts, new_context);

	return tag;
}
/*----------------------------------------------------------------------------*
 *                              Style parsing                                 *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_style(Edom_Context *context,
		Esvg_Parser_Context *thiz,
		Eina_Array *contexts)
{
	Edom_Context *new_context;
	Edom_Tag *tag;
	Esvg_Parser_Style *style;

	style = esvg_parser_style_new();

	tag = edom_tag_new(context, NULL, ESVG_STYLE, thiz->svg, style);
	esvg_parser_svg_style_add(thiz->svg, style);

	new_context = esvg_parser_context_style_new(tag);
	eina_array_push(contexts, new_context);

	return tag;
}
/*----------------------------------------------------------------------------*
 *                                G parsing                                   *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_g(Edom_Context *context,
		Esvg_Parser_Context *thiz,
		Eina_Array *contexts)
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
	eina_array_push(contexts, new_context);

	return tag;
}
/*----------------------------------------------------------------------------*
 *                               Rect parsing                                 *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_rect(Edom_Context *context,
		Esvg_Parser_Context *thiz,
		Eina_Array *contexts)
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
static Edom_Tag * _esvg_parser_context_line(Edom_Context *context,
		Esvg_Parser_Context *thiz,
		Eina_Array *contexts)
{
	Edom_Tag *tag;
	Enesim_Renderer *r;

	tag = esvg_parser_line_new(context, thiz->svg);
	if (thiz->parent_r)
	{
		r = esvg_parser_element_renderer_get(tag);
		esvg_container_element_add(thiz->parent_r, r);
	}

	return tag;
}
/*----------------------------------------------------------------------------*
 *                               Ellipse parsing                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_ellipse(Edom_Context *context,
		Esvg_Parser_Context *thiz,
		Eina_Array *contexts)
{
	Edom_Tag *tag;
	Enesim_Renderer *r;

	tag = esvg_parser_ellipse_new(context, thiz->svg);
	if (thiz->parent_r)
	{
		r = esvg_parser_element_renderer_get(tag);
		esvg_container_element_add(thiz->parent_r, r);
	}

	return tag;
}
/*----------------------------------------------------------------------------*
 *                               Circle parsing                               *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_circle(Edom_Context *context,
		Esvg_Parser_Context *thiz,
		Eina_Array *contexts)
{
	Edom_Tag *tag;
	Enesim_Renderer *r;

	tag = esvg_parser_circle_new(context, thiz->svg);
	if (thiz->parent_r)
	{
		r = esvg_parser_element_renderer_get(tag);
		esvg_container_element_add(thiz->parent_r, r);
	}

	return tag;
}
/*----------------------------------------------------------------------------*
 *                                Path parsing                                *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_path(Edom_Context *context,
		Esvg_Parser_Context *thiz,
		Eina_Array *contexts)
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
static Edom_Tag * _esvg_parser_context_polygon(Edom_Context *context,
		Esvg_Parser_Context *thiz,
		Eina_Array *contexts)
{
	Edom_Tag *tag;
	Enesim_Renderer *r;

	tag = esvg_parser_polygon_new(context, thiz->svg);
	if (thiz->parent_r)
	{
		r = esvg_parser_element_renderer_get(tag);
		esvg_container_element_add(thiz->parent_r, r);
	}

	return tag;
}
/*----------------------------------------------------------------------------*
 *                             Polyline parsing                               *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_polyline(Edom_Context *context,
		Esvg_Parser_Context *thiz,
		Eina_Array *contexts)
{
	Edom_Tag *tag;
	Enesim_Renderer *r;

	tag = esvg_parser_polyline_new(context, thiz->svg);
	if (thiz->parent_r)
	{
		r = esvg_parser_element_renderer_get(tag);
		esvg_container_element_add(thiz->parent_r, r);
	}

	return tag;
}
/*----------------------------------------------------------------------------*
 *                                Text parsing                                *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_text(Edom_Context *context,
		Esvg_Parser_Context *thiz,
		Eina_Array *contexts)
{
	Edom_Tag *tag;
	Edom_Context *new_context;
	Enesim_Renderer *r;

	tag = esvg_parser_text_new(context, thiz->svg);
	if (thiz->parent_r)
	{
		r = esvg_parser_element_renderer_get(tag);
		esvg_container_element_add(thiz->parent_r, r);
	}

	new_context = esvg_parser_context_text_new(tag);
	eina_array_push(contexts, new_context);

	return tag;
}
/*----------------------------------------------------------------------------*
 *                                Svg parsing                                 *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_svg(Edom_Context *context,
		Esvg_Parser_Context *thiz,
		Eina_Array *contexts)
{
	Edom_Tag *tag;
	Edom_Context *new_context;
	Enesim_Renderer *r;

	tag = esvg_parser_svg_new(context, thiz->svg);
	r = esvg_parser_element_renderer_get(tag);
	if (thiz->parent_r)
		esvg_container_element_add(thiz->parent_r, r);

	new_context = esvg_parser_context_svg_new(tag, r);
	eina_array_push(contexts, new_context);

	return tag;
}
/*----------------------------------------------------------------------------*
 *                              Image parsing                                 *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_image(Edom_Context *context,
		Esvg_Parser_Context *thiz,
		Eina_Array *contexts)
{
	Edom_Tag *tag;
	Enesim_Renderer *r;

	tag = esvg_parser_image_new(context, thiz->svg);
	r = esvg_parser_element_renderer_get(tag);
	if (thiz->parent_r)
		esvg_container_element_add(thiz->parent_r, r);

	return tag;
}
/*----------------------------------------------------------------------------*
 *                               Defs parsing                                 *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_defs(Edom_Context *context,
		Esvg_Parser_Context *thiz,
		Eina_Array *contexts)
{
	Edom_Context *new_context;
	Edom_Tag *tag;

	tag = edom_tag_new(context, NULL, ESVG_DEFS, thiz->svg, NULL);

	new_context = esvg_parser_defs_new(tag);
	eina_array_push(contexts, new_context);

	return tag;
}
/*----------------------------------------------------------------------------*
 *                               Use parsing                                  *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_use(Edom_Context *context,
		Esvg_Parser_Context *thiz,
		Eina_Array *contexts)
{
	Edom_Tag *tag;
	Enesim_Renderer *r;

	tag = esvg_parser_use_new(context, thiz->svg);
	r = esvg_parser_element_renderer_get(tag);
	if (thiz->parent_r)
		esvg_container_element_add(thiz->parent_r, r);

	return tag;
}
/*----------------------------------------------------------------------------*
 *                              Pattern parsing                               *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_container_pattern(Edom_Context *context,
		Esvg_Parser_Context *thiz,
		Eina_Array *contexts)
{
	Edom_Tag *tag = NULL;

	printf("TODO\n");
	return tag;
}
/*----------------------------------------------------------------------------*
 *                         Linear gradient parsing                            *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_linear_gradient(Edom_Context *context,
		Esvg_Parser_Context *thiz,
		Eina_Array *contexts)
{
	Edom_Tag *tag;
	Edom_Context *new_context;
	Enesim_Renderer *r;

	tag = esvg_parser_linear_gradient_new(context, thiz->svg);
	r = esvg_parser_element_renderer_get(tag);

	new_context = esvg_parser_context_gradient_new(tag, r);
	eina_array_push(contexts, new_context);

	return tag;
}
/*----------------------------------------------------------------------------*
 *                         Radial gradient parsing                            *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_parser_context_radial_gradient(Edom_Context *context,
		Esvg_Parser_Context *thiz,
		Eina_Array *contexts)
{
	Edom_Tag *tag;
	Edom_Context *new_context;
	Enesim_Renderer *r;

	tag = esvg_parser_radial_gradient_new(context, thiz->svg);
	r = esvg_parser_element_renderer_get(tag);

	new_context = esvg_parser_context_gradient_new(tag, r);
	eina_array_push(contexts, new_context);

	return tag;
}

static Edom_Tag * _context_tag_new(Esvg_Parser_Context *thiz,
		int tag, Edom_Context *context, Eina_Array *contexts)
{
	Edom_Tag *new_tag = NULL;

	switch (tag)
	{
		case ESVG_LINEARGRADIENT:
		new_tag = _esvg_parser_context_linear_gradient(context, thiz, contexts);
		break;

		case ESVG_RADIALGRADIENT:
		new_tag = _esvg_parser_context_radial_gradient(context, thiz, contexts);
		break;

		case ESVG_PATTERN:
		new_tag = _esvg_parser_container_pattern(context, thiz, contexts);
		break;

		case ESVG_DEFS:
		new_tag = _esvg_parser_context_defs(context, thiz, contexts);
		break;

		case ESVG_USE:
		new_tag = _esvg_parser_context_use(context, thiz, contexts);
		break;

		case ESVG_SVG:
		new_tag = _esvg_parser_context_svg(context, thiz, contexts);
		break;

		case ESVG_CIRCLE:
		new_tag = _esvg_parser_context_circle(context, thiz, contexts);
		break;

		case ESVG_ELLIPSE:
		new_tag = _esvg_parser_context_ellipse(context, thiz, contexts);
		break;

		case ESVG_RECT:
		new_tag = _esvg_parser_context_rect(context, thiz, contexts);
		break;

		case ESVG_LINE:
		new_tag = _esvg_parser_context_line(context, thiz, contexts);
		break;

		case ESVG_PATH:
		new_tag = _esvg_parser_context_path(context, thiz, contexts);
		break;

		case ESVG_POLYLINE:
		new_tag = _esvg_parser_context_polyline(context, thiz, contexts);
		break;

		case ESVG_POLYGON:
		new_tag = _esvg_parser_context_polygon(context, thiz, contexts);
		break;

		case ESVG_TEXT:
		new_tag = _esvg_parser_context_text(context, thiz, contexts);
		break;

		case ESVG_G:
		new_tag = _esvg_parser_context_g(context, thiz, contexts);
		break;

		case ESVG_STYLE:
		/* FIXME store the style? */
		new_tag = _esvg_parser_context_style(context, thiz, contexts);
		break;

		case ESVG_IMAGE:
		new_tag = _esvg_parser_context_image(context, thiz, contexts);
		break;

		case ESVG_CLIPPATH:
		new_tag = _esvg_parser_context_clip_path(context, thiz, contexts);
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
		Eina_Array *contexts, const char *attributes,
		unsigned int length)
{
	Esvg_Parser_Context *thiz = data;
	Edom_Context *context;
	Edom_Tag *new_tag = NULL;
	Eina_Bool ret = EINA_TRUE;

	context = eina_array_data_get(contexts, eina_array_count_get(contexts) - 1);
	/* TODO first check if the tag is supported */
	if (!thiz->tag_is_supported(tag))
	{
		printf("parsing error. tag is not suppoted %d\n", tag);
		return;
	}
	/* if so, create it */
	new_tag = _context_tag_new(thiz, tag, context, contexts);
	if (new_tag)
	{
		edom_tag_attributes_from_xml(new_tag, attributes, length);
		edom_tag_child_add(thiz->parent, new_tag);
		esvg_parser_svg_tag_add(thiz->svg, new_tag);
	}
	/* if there's a tag open callback call it with the new tag passed */

	return ret;
}

static void _context_tag_close(void *data, int tag,
		Eina_Array *contexts)
{
	Esvg_Parser_Context *thiz = data;
	Edom_Context *context;

	/* destroy ourselves */
	if (thiz->tag == tag)
	{
		context = eina_array_pop(contexts);
		edom_context_delete(context);
		return;
	}
}

static void _context_free(void *data)
{
	Esvg_Parser_Context *thiz = data;
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
Edom_Context * esvg_parser_context_new(Esvg_Parser_Context_Tag_Is_Supported
		tag_is_supported, int tag, Edom_Tag *svg, Edom_Tag *parent,
		Enesim_Renderer *parent_r)
{
	Esvg_Parser_Context *thiz;
	Edom_Parser *parser;

	if (!svg) return NULL;

	thiz = calloc(1, sizeof(Esvg_Parser_Context));
	thiz->tag_is_supported = tag_is_supported;
	thiz->tag = tag;
	thiz->svg = svg;
	thiz->parent = parent;
	thiz->parent_r = parent_r;

	parser = edom_tag_parser_get(svg);
	return edom_context_new(parser, &_descriptor, thiz);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

