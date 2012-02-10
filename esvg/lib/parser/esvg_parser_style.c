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
#include "esvg_values.h"
/*
 * In order to support styling we need to keep track of every element
 * parsed, that is, generate the DOM tree. This way we can support Ecss
 * context to know whenver an element has to be applied with a specific
 * style
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/* FIXME share this */
#define ESVG_SPACE_SKIP(t) \
	while (*t) { if ((*t == ' ') || (*t == '\n') || (*t == '\t')) t++; else break; }


typedef struct _Esvg_Parser_Style_Inline
{
	Esvg_Attribute_Presentation *attr;
	Edom_Tag *tag;
	char *fill;
	char *stroke;
} Esvg_Parser_Style_Inline;

struct _Esvg_Parser_Style
{
	Ecss_Style *s;
};

static const char * _get_name(void *e)
{
	Edom_Tag *tag = e;
	Esvg_Parser_Tag_Type type;

	type = edom_tag_type_get(tag);
	switch (type)
	{
		case ESVG_A:
		return "a";

		case ESVG_ALTGLYPH:
		case ESVG_ALTGLYPHDEF:
		case ESVG_ALTGLYPHITEM:
		case ESVG_ANIMATE:
		case ESVG_ANIMATECOLOR:
		case ESVG_ANIMATEMOTION:
		case ESVG_ANIMATETRANSFORM:
		case ESVG_CIRCLE:
		return "circle";

		case ESVG_CLIPPATH:
		case ESVG_COLOR_PROFILE:
		case ESVG_CURSOR:
		case ESVG_DEFINITION_SRC:
		case ESVG_DEFS:
		return "defs";

		case ESVG_DESC:
		return "desc";

		case ESVG_ELLIPSE:
		return "ellipse";

		case ESVG_FEBLEND:
		case ESVG_FECOLORMATRIX:
		case ESVG_FECOMPONENTTRANSFER:
		case ESVG_FECOMPOSITE:
		case ESVG_FECONVOLVEMATRIX:
		case ESVG_FEDIFFUSELIGHTING:
		case ESVG_FEDISPLACEMENTMAP:
		case ESVG_FEDISTANTLIGHT:
		case ESVG_FEFLOOD:
		case ESVG_FEFUNCA:
		case ESVG_FEFUNCB:
		case ESVG_FEFUNCG:
		case ESVG_FEFUNCR:
		case ESVG_FEGAUSSIANBLUR:
		case ESVG_FEIMAGE:
		case ESVG_FEMERGE:
		case ESVG_FEMERGENODE:
		case ESVG_FEMORPHOLOGY:
		case ESVG_FEOFFSET:
		case ESVG_FEPOINTLIGHT:
		case ESVG_FESPECULARLIGHTING:
		case ESVG_FESPOTLIGHT:
		case ESVG_FETILE:
		case ESVG_FETURBULENCE:
		case ESVG_FILTER:
		case ESVG_FONT:
		case ESVG_FONT_FACE:
		case ESVG_FONT_FACE_FORMAT:
		case ESVG_FONT_FACE_NAME:
		case ESVG_FONT_FACE_SRC:
		case ESVG_FONT_FACE_URI:
		case ESVG_FOREIGNOBJECT:
		case ESVG_G:
		return "g";

		case ESVG_GLYPH:
		case ESVG_GLYPHREF:
		case ESVG_HKERN:
		case ESVG_IMAGE:
		return "image";
		case ESVG_LINE:
		return "line";

		case ESVG_LINEARGRADIENT:
		return "linearGradient";

		case ESVG_MARKER:
		case ESVG_MASK:
		case ESVG_METADATA:
		case ESVG_MISSING_GLYPH:
		case ESVG_MPATH:
		case ESVG_PATH:
		return "path";

		case ESVG_PATTERN:
		return "pattern";

		case ESVG_POLYGON:
		return "polygon";

		case ESVG_POLYLINE:
		return "polyline";

		case ESVG_RADIALGRADIENT:
		return "radialGradient";

		case ESVG_RECT:
		return "rect";

		case ESVG_SCRIPT:
		return "script";

		case ESVG_SET:
		return "set";

		case ESVG_STOP:
		return "stop";

		case ESVG_STYLE:
		return "style";

		case ESVG_SVG:
		return "svg";

		case ESVG_SWITCH:
		case ESVG_SYMBOL:
		case ESVG_TEXT:
		return "text";

		case ESVG_TEXTPATH:
		case ESVG_TITLE:
		case ESVG_TREF:
		case ESVG_TSPAN:
		case ESVG_USE:
		return "use";

		case ESVG_VIEW:
		case ESVG_VKERN:

		default:
		return "unknown";
	}
}

static void _post_parse_fill_cb(Edom_Parser *parser, void *data)
{
	Esvg_Parser_Style_Inline *thiz = data;
	Esvg_Paint fill;

	esvg_paint_get(&fill, thiz->tag, thiz->fill);
	esvg_attribute_presentation_fill_set(thiz->attr, &fill);
}

static void _post_parse_stroke_cb(Edom_Parser *parser, void *data)
{
	Esvg_Parser_Style_Inline *thiz = data;
	Esvg_Paint stroke;

	esvg_paint_get(&stroke, thiz->tag, thiz->stroke);
	esvg_attribute_presentation_stroke_set(thiz->attr, &stroke);
}

static void _attr_callback(Esvg_Parser_Style_Inline *thiz,
		const char *key, const char *value)
{
	/* common presentation attributes */
	if (strcmp(key, "color") == 0)
	{
		Esvg_Color color;

		esvg_color_get(&color, value);
		esvg_attribute_presentation_color_set(thiz->attr, &color);
	}
	else if (strcmp(key, "fill") == 0)
	{
		Esvg_Paint fill;

		if (!esvg_paint_get(&fill, thiz->tag, value))
		{
			/* we delay the parsing of fill for later in case
			 * the value has an uri
			 */
			if (esvg_parser_is_uri(value))
			{
				Edom_Parser *parser;

				parser = edom_tag_parser_get(thiz->tag);
				thiz->fill = strdup(value);
				esvg_parser_post_parse_add(parser, _post_parse_fill_cb, thiz);
			}
		}
		else
		{
			esvg_attribute_presentation_fill_set(thiz->attr, &fill);
		}
	}
	else if (strcmp(key, "fill-opacity") == 0)
	{
		double fill_opacity = esvg_number_get(value, 1.0);
		esvg_attribute_presentation_fill_opacity_set(thiz->attr, fill_opacity);
	}
	else if (strcmp(key, "stroke") == 0)
	{
		Esvg_Paint stroke;

		if (!esvg_paint_get(&stroke, thiz->tag, value))
		{
			/* we delay the parsing of stroke for later in case
			 * the value has an uri
			 */
			if (esvg_parser_is_uri(value))
			{
				Edom_Parser *parser;

				parser = edom_tag_parser_get(thiz->tag);
				thiz->stroke = strdup(value);
				esvg_parser_post_parse_add(parser, _post_parse_stroke_cb, thiz);
			}
		}
		else
		{
			esvg_attribute_presentation_stroke_set(thiz->attr, &stroke);
		}
	}
	else if (strcmp(key, "stroke-width") == 0)
	{
		Esvg_Length stroke_width;

		esvg_length_get(&stroke_width, value, ESVG_LENGTH_1);
		esvg_attribute_presentation_stroke_width_set(thiz->attr, &stroke_width);
	}
	else if (strcmp(key, "stroke-opacity") == 0)
	{
		double stroke_opacity = esvg_number_get(value, 1.0);
		esvg_attribute_presentation_stroke_opacity_set(thiz->attr, stroke_opacity);
	}
	else if (strcmp(key, "stroke-linecap") == 0)
	{
		Esvg_Stroke_Line_Cap stroke_line_cap;

		stroke_line_cap = esvg_stroke_line_cap_get(value);
		esvg_attribute_presentation_stroke_line_cap_set(thiz->attr, stroke_line_cap);
	}
	else if (strcmp(key, "stroke-linejoin") == 0)
	{
		Esvg_Stroke_Line_Join stroke_line_join;

		stroke_line_join = esvg_stroke_line_join_get(value);
		esvg_attribute_presentation_stroke_line_join_set(thiz->attr, stroke_line_join);
	}
	else if (strcmp(key, "stop-color") == 0)
	{
		Esvg_Color color;

		esvg_color_get(&color, value);
		esvg_attribute_presentation_stop_color_set(thiz->attr, &color);
	}
	else if (strcmp(key, "stop-opacity") == 0)
	{
		double stop_opacity = esvg_number_get(value, 1.0);
		esvg_attribute_presentation_stop_opacity_set(thiz->attr, stop_opacity);
	}
}
/*----------------------------------------------------------------------------*
 *                          Css context interface                             *
 *----------------------------------------------------------------------------*/
static const char * _property_get(void *e, const char *property)
{
	Edom_Tag *tag = e;

	if (!strcmp(property, "class"))
	{
		return edom_tag_class_get(e);
	}
	else if (!strcmp(property, "id"))
	{
		return edom_tag_id_get(e);
	}
	else
	{
		return NULL;
	}
}

static void _property_set(void *e, const char *property, const char *value)
{
	Edom_Tag *tag = e;

	printf("setting %s %s\n", property, value);
	edom_tag_attribute_set(tag, property, value);
}

static void * _get_child(void *e)
{
	Edom_Tag *tag = e;
	return edom_tag_child_get(tag);
}

static void * _get_next_sibling(void *e)
{
	Edom_Tag *tag = e;
	return edom_tag_next_get(tag);
}

static Ecss_Context _context = {
	/* .property_set 	= */ _property_set,
	/* .property_get 	= */ _property_get,
	/* .get_name 		= */ _get_name,
	/* .get_child 		= */ _get_child,
	/* .get_next_sibling 	= */ _get_next_sibling,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_parser_style_inline_set(const char *value, Edom_Tag *tag,
		Esvg_Attribute_Presentation *attr)
{
	/* FIXME for now is not a pointer, the deferred callback will segv, but is ok for now */
	Esvg_Parser_Style_Inline thiz;
	char *orig;
	char *v;
	char *sc;
	char *c;


	thiz.fill = NULL;
	thiz.stroke = NULL;
	thiz.tag = tag;
	thiz.attr = attr;

	orig = v = strdup(value);
	ESVG_SPACE_SKIP(v);
	/* split the value by ';' */
	while ((sc = index(v, ';')))
	{
		*sc = '\0';
		/* split it by ':' */
		c = index(v, ':');
		if (c)
		{
			char *vv;

			*c = '\0';
			vv = c + 1;
			ESVG_SPACE_SKIP(vv);
			/* and call the attr_cb */
			_attr_callback(&thiz, v, vv);
		}
		v = sc + 1;
		ESVG_SPACE_SKIP(v);
	}
	/* do the last one */
	c = index(v, ':');
	if (c)
	{
		char *vv;

		*c = '\0';
		vv = c + 1;
		ESVG_SPACE_SKIP(vv);
		/* and call the attr_cb */
		_attr_callback(&thiz, v, vv);
	}

	free(orig);
}

Esvg_Parser_Style * esvg_parser_style_new(void)
{
	Esvg_Parser_Style *thiz;

	thiz = calloc(1, sizeof(Esvg_Parser_Style));
	return thiz;
}

void esvg_parser_style_style_set(Esvg_Parser_Style *thiz, Ecss_Style *style)
{
	thiz->s = style;
}

void esvg_parser_style_apply(Esvg_Parser_Style *thiz, Edom_Tag *root)
{
	ecss_context_style_apply(&_context, thiz->s, root);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
