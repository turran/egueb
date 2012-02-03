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
#ifndef _ESVG_PARSER_PRIVATE_H
#define _ESVG_PARSER_PRIVATE_H

#include <Edom.h>

typedef void (*Esvg_Parser_Post)(Edom_Parser *p, void *data);

typedef enum _Esvg_Parser_Tag_Type {
	ESVG_A,
	ESVG_ALTGLYPH,
	ESVG_ALTGLYPHDEF,
	ESVG_ALTGLYPHITEM,
	ESVG_ANIMATE,
	ESVG_ANIMATECOLOR,
	ESVG_ANIMATEMOTION,
	ESVG_ANIMATETRANSFORM,
	ESVG_CIRCLE,
	ESVG_CLIPPATH,
	ESVG_COLOR_PROFILE, /* 10 */
	ESVG_CURSOR,
	ESVG_DEFINITION_SRC,
	ESVG_DEFS,
	ESVG_DESC,
	ESVG_ELLIPSE,
	ESVG_FEBLEND,
	ESVG_FECOLORMATRIX,
	ESVG_FECOMPONENTTRANSFER,
	ESVG_FECOMPOSITE,
	ESVG_FECONVOLVEMATRIX, /* 20 */
	ESVG_FEDIFFUSELIGHTING,
	ESVG_FEDISPLACEMENTMAP,
	ESVG_FEDISTANTLIGHT,
	ESVG_FEFLOOD,
	ESVG_FEFUNCA,
	ESVG_FEFUNCB,
	ESVG_FEFUNCG,
	ESVG_FEFUNCR,
	ESVG_FEGAUSSIANBLUR,
	ESVG_FEIMAGE, /* 30 */
	ESVG_FEMERGE,
	ESVG_FEMERGENODE,
	ESVG_FEMORPHOLOGY,
	ESVG_FEOFFSET,
	ESVG_FEPOINTLIGHT,
	ESVG_FESPECULARLIGHTING,
	ESVG_FESPOTLIGHT,
	ESVG_FETILE,
	ESVG_FETURBULENCE,
	ESVG_FILTER, /* 40 */
	ESVG_FONT,
	ESVG_FONT_FACE,
	ESVG_FONT_FACE_FORMAT,
	ESVG_FONT_FACE_NAME,
	ESVG_FONT_FACE_SRC,
	ESVG_FONT_FACE_URI,
	ESVG_FOREIGNOBJECT,
	ESVG_G,
	ESVG_GLYPH,
	ESVG_GLYPHREF, /* 50 */
	ESVG_HKERN,
	ESVG_IMAGE,
	ESVG_LINE,
	ESVG_LINEARGRADIENT,
	ESVG_MARKER,
	ESVG_MASK,
	ESVG_METADATA,
	ESVG_MISSING_GLYPH,
	ESVG_MPATH,
	ESVG_PATH, /* 60 */
	ESVG_PATTERN,
	ESVG_POLYGON,
	ESVG_POLYLINE,
	ESVG_RADIALGRADIENT,
	ESVG_RECT,
	ESVG_SCRIPT,
	ESVG_SET,
	ESVG_STOP,
	ESVG_STYLE,
	ESVG_SVG,
	ESVG_SWITCH,
	ESVG_SYMBOL,
	ESVG_TEXT,
	ESVG_TEXTPATH,
	ESVG_TITLE,
	ESVG_TREF,
	ESVG_TSPAN,
	ESVG_USE,
	ESVG_VIEW,
	ESVG_VKERN,
	ESVG_PARSE_TAGS
} Esvg_Parser_Tag_Type;

void esvg_parser_post_parse_add(Edom_Parser *thiz, Esvg_Parser_Post cb, void *data);

typedef struct _Esvg_Parser_Style Esvg_Parser_Style;

const char * esvg_parser_tag_type_string_to(Esvg_Parser_Tag_Type type);

Esvg_Parser_Style * esvg_parser_style_new(void);
void esvg_parser_style_style_set(Esvg_Parser_Style *thiz, Ecss_Style *style);
void esvg_parser_style_apply(Esvg_Parser_Style *thiz, Edom_Tag *root);
void esvg_parser_style_inline_set(const char *value, Edom_Tag *tag,
		Esvg_Attribute_Presentation *attr);

Edom_Context * esvg_parser_context_doc_new(Edom_Parser *parser);
Edom_Tag * esvg_parser_context_doc_topmost_get(Edom_Context *c);

typedef Eina_Bool (*Esvg_Parser_Context_Tag_Is_Supported)(int tag);
Edom_Context * esvg_parser_context_new(Esvg_Parser_Context_Tag_Is_Supported is_supported,
		int tag, Edom_Tag *svg, Edom_Tag *parent,
		Enesim_Renderer *parent_r);

Edom_Context * esvg_parser_context_svg_new(Edom_Tag *topmost, Enesim_Renderer *topmost_r);

Edom_Context * esvg_parser_context_g_new(Edom_Tag *topmost, Edom_Tag *root, Enesim_Renderer *parent_r);

Edom_Context * esvg_parser_context_gradient_new(Edom_Tag *tag, Enesim_Renderer *r);

Edom_Context * esvg_parser_defs_new(Edom_Tag *tag);

Edom_Context * esvg_parser_context_style_new(Edom_Tag *tag);

Edom_Context * esvg_parser_context_text_new(Edom_Tag *tag);

Eina_Bool esvg_parser_shape_attributes_cb(void *data, const char *key,
		const char *value);

/* shape */
Edom_Tag * esvg_parser_shape_new(Edom_Context *c, Edom_Tag_Descriptor *descriptor, Esvg_Parser_Tag_Type type, Edom_Tag *topmost, Enesim_Renderer *r, void *data);

/* svg */
Edom_Tag * esvg_parser_svg_new(Edom_Context *c, Edom_Tag *tag);
void esvg_parser_svg_tag_add(Edom_Tag *tag, Edom_Tag *child_tag);
void esvg_parser_svg_style_add(Edom_Tag *thiz, Esvg_Parser_Style *s);
void esvg_parser_svg_style_apply(Edom_Tag *thiz);
void esvg_parser_svg_svg_add(Edom_Tag *thiz, Edom_Tag *svg);
Edom_Tag * esvg_parser_svg_tag_find(Edom_Tag *thiz, const char *id);

/* paint server */
Edom_Tag * esvg_parser_paint_server_new(Edom_Context *c,
		Edom_Tag_Descriptor *descriptor, int type,
		Edom_Tag *topmost,
		Enesim_Renderer *r,
		void *data);
void * esvg_parser_paint_server_data_get(Edom_Tag *tag);

/* gradient */
typedef struct _Esvg_Parser_Gradient_Descriptor {
	
	void (*merge)(Enesim_Renderer *r,
			Enesim_Renderer *rel);
	const char * (*name_get)(Edom_Tag *t);
	Eina_Bool (*attribute_set)(Edom_Tag *t, const char *attribute, const char *value);
	const char * (*attribute_get)(Edom_Tag *t, const char *attribute);
} Esvg_Parser_Gradient_Descriptor;

Edom_Tag * esvg_parser_gradient_new(Edom_Context *c,
		Esvg_Parser_Gradient_Descriptor *descriptor,
		int type,
		Edom_Tag *topmost,
		Enesim_Renderer *r,
		void *data);
void * esvg_parser_gradient_data_get(Edom_Tag *tag);
/* linear gradient */
Edom_Tag * esvg_parser_linear_gradient_new(Edom_Context *c, Edom_Tag *topmost);
/* radial gradient */
Edom_Tag * esvg_parser_radial_gradient_new(Edom_Context *c, Edom_Tag *topmost);


/* circle */
Edom_Tag * esvg_parser_circle_new(Edom_Context *c, Edom_Tag *topmost);

/* ellipse */
Edom_Tag * esvg_parser_ellipse_new(Edom_Context *c, Edom_Tag *topmost);

/* g */
Edom_Tag * esvg_parser_g_new(Edom_Context *c, Edom_Tag *topmost);

/* line */
Edom_Tag * esvg_parser_line_new(Edom_Context *c, Edom_Tag *topmost);

/* path */
Edom_Tag * esvg_parser_path_new(Edom_Context *c, Edom_Tag *topmost);

/* polygon */
Edom_Tag * esvg_parser_polygon_new(Edom_Context *c, Edom_Tag *topmost);

/* polyline */
Edom_Tag * esvg_parser_polyline_new(Edom_Context *c, Edom_Tag *topmost);

/* rect */
Edom_Tag * esvg_parser_rect_new(Edom_Context *c, Edom_Tag *topmost);

/* text */
Edom_Tag * esvg_parser_text_new(Edom_Context *c, Edom_Tag *topmost);

/* use */
Edom_Tag * esvg_parser_use_new(Edom_Context *c, Edom_Tag *topmost);

/* stop */
Edom_Tag * esvg_parser_stop_new(Edom_Context *c, Edom_Tag *topmost);

/* image */
Edom_Tag * esvg_parser_image_new(Edom_Context *c, Edom_Tag *topmost);

/* element */
Edom_Tag * esvg_parser_element_new(Edom_Context *context, Edom_Tag_Descriptor *descriptor, Esvg_Parser_Tag_Type type, Edom_Tag *topmost, Enesim_Renderer *r, void *data);
Enesim_Renderer * esvg_parser_element_renderer_get(Edom_Tag *tag);
void * esvg_parser_element_data_get(Edom_Tag *tag);

typedef void (*Esvg_Parser_Points_Cb)(Esvg_Point *p, void *data);
void esvg_parser_points(const char *value, Esvg_Parser_Points_Cb cb, void *data);

Eina_Bool esvg_parser_path(const char *value, Enesim_Renderer *r);

Eina_Bool esvg_uri_get(Edom_Tag **tag, Edom_Tag *rel, const char *attr);
Eina_Bool esvg_href_get(Edom_Tag **tag, Edom_Tag *rel, const char *href);

Eina_Bool esvg_paint_get(Esvg_Paint *paint, Edom_Tag *tag, const char *attr);
Esvg_Stroke_Line_Cap esvg_stroke_line_cap_get(const char *value);

Eina_Bool esvg_parser_is_uri(const char *attr);

Eina_Bool esvg_parser_gradient_units_get(Esvg_Gradient_Units *gu, const char *value);
Eina_Bool esvg_parser_fill_rule_get(Esvg_Fill_Rule *rule, const char *attr);

#endif
