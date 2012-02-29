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
#include <Ecss.h>

/* parser */
typedef void (*Esvg_Parser_Post)(Edom_Parser *p, void *data);

void esvg_parser_post_parse_add(Edom_Parser *thiz, Esvg_Parser_Post cb, void *data);
void esvg_parser_href_set(Edom_Parser *p, Enesim_Renderer *r, const char *href);

/* style */
typedef struct _Esvg_Parser_Style Esvg_Parser_Style;

const char * esvg_parser_tag_type_string_to(Esvg_Parser_Tag_Type type);

Esvg_Parser_Style * esvg_parser_style_new(void);
void esvg_parser_style_style_set(Esvg_Parser_Style *thiz, Ecss_Style *style);
void esvg_parser_style_apply(Esvg_Parser_Style *thiz, Edom_Tag *root);
void esvg_parser_style_inline_set(const char *value, Edom_Tag *tag,
		Esvg_Attribute_Presentation *attr);

/* doc context */
Edom_Context * esvg_parser_context_doc_new(Edom_Parser *parser);
Edom_Tag * esvg_parser_context_doc_topmost_get(Edom_Context *c);

/* simple context */
typedef Eina_Bool (*Esvg_Parser_Context_Simple_Tag_Is_Supported)(void *data, int tag);
typedef void (*Esvg_Parser_Context_Simple_Tag_Added)(void *data, Edom_Tag *tag);
typedef void (*Esvg_Parser_Context_Simple_Free)(void *data);

typedef struct _Esvg_Parser_Context_Simple_Descriptor {
	Esvg_Parser_Context_Simple_Tag_Is_Supported tag_is_supported;
	Esvg_Parser_Context_Simple_Tag_Added tag_added;
	Esvg_Parser_Context_Simple_Free tag_free;
} Esvg_Parser_Context_Simple_Descriptor;

Edom_Context * esvg_parser_context_simple_new(int tag, Edom_Tag *svg, Edom_Tag *parent,
		Enesim_Renderer *parent_r,
		Esvg_Parser_Context_Simple_Descriptor *descriptor,
		void *data);

Edom_Context * esvg_parser_context_svg_new(Edom_Tag *topmost, Enesim_Renderer *topmost_r);

Edom_Context * esvg_parser_context_g_new(Edom_Tag *topmost, Edom_Tag *root, Enesim_Renderer *parent_r);

Edom_Context * esvg_parser_context_gradient_new(Edom_Tag *tag, Enesim_Renderer *r);

Edom_Context * esvg_parser_defs_new(Edom_Parser *parser);

Edom_Context * esvg_parser_context_style_new(Edom_Tag *tag);

Edom_Context * esvg_parser_context_text_new(Edom_Tag *tag);

Eina_Bool esvg_parser_shape_attributes_cb(void *data, const char *key,
		const char *value);

/* shape */
Edom_Tag * esvg_parser_shape_new(Edom_Parser *parser,
		Edom_Tag_Descriptor *descriptor,
		Esvg_Parser_Tag_Type type,
		Enesim_Renderer *r, void *data);

/* svg */
Edom_Tag * esvg_parser_svg_new(Edom_Parser *parser);
void esvg_parser_svg_tag_add(Edom_Tag *tag, Edom_Tag *child_tag);
void esvg_parser_svg_style_add(Edom_Tag *thiz, Esvg_Parser_Style *s);
void esvg_parser_svg_style_apply(Edom_Tag *thiz);
void esvg_parser_svg_svg_add(Edom_Tag *thiz, Edom_Tag *svg);
Edom_Tag * esvg_parser_svg_tag_find(Edom_Tag *thiz, const char *id);

/* paint server */
Edom_Tag * esvg_parser_paint_server_new(Edom_Parser *parser,
		Edom_Tag_Descriptor *descriptor, int type,
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

Edom_Tag * esvg_parser_gradient_new(Edom_Parser *parser,
		Esvg_Parser_Gradient_Descriptor *descriptor,
		int type,
		Enesim_Renderer *r,
		void *data);
void * esvg_parser_gradient_data_get(Edom_Tag *tag);
/* linear gradient */
Edom_Tag * esvg_parser_linear_gradient_new(Edom_Parser *parser);
/* radial gradient */
Edom_Tag * esvg_parser_radial_gradient_new(Edom_Parser *parser);

/* a */
Edom_Tag * esvg_parser_a_new(Edom_Parser *p);
const char * esvg_parser_a_href_get(Edom_Tag *t);

/* circle */
Edom_Tag * esvg_parser_circle_new(Edom_Parser *parser);

/* ellipse */
Edom_Tag * esvg_parser_ellipse_new(Edom_Parser *parser);

/* g */
Edom_Tag * esvg_parser_g_new(Edom_Parser *parser);

/* line */
Edom_Tag * esvg_parser_line_new(Edom_Parser *parser);

/* path */
Edom_Tag * esvg_parser_path_new(Edom_Parser *parser);

/* polygon */
Edom_Tag * esvg_parser_polygon_new(Edom_Parser *parser);

/* polyline */
Edom_Tag * esvg_parser_polyline_new(Edom_Parser *parser);

/* rect */
Edom_Tag * esvg_parser_rect_new(Edom_Parser *parser);

/* text */
Edom_Tag * esvg_parser_text_new(Edom_Parser *parser);

/* use */
Edom_Tag * esvg_parser_use_new(Edom_Parser *parser);

/* stop */
Edom_Tag * esvg_parser_stop_new(Edom_Parser *parser);

/* image */
Edom_Tag * esvg_parser_image_new(Edom_Parser *parser);

/* pattern */
Edom_Tag * esvg_parser_pattern_new(Edom_Parser *parser);

/* clip path */
Edom_Tag * esvg_parser_clip_path_new(Edom_Parser *parser);

/* element */
Edom_Tag * esvg_parser_element_new(Edom_Parser *parser, Edom_Tag_Descriptor *descriptor, Esvg_Parser_Tag_Type type, Enesim_Renderer *r, void *data);
Enesim_Renderer * esvg_parser_element_renderer_get(Edom_Tag *tag);
void * esvg_parser_element_data_get(Edom_Tag *tag);

typedef void (*Esvg_Parser_Points_Cb)(Esvg_Point *p, void *data);
void esvg_parser_points(const char *value, Esvg_Parser_Points_Cb cb, void *data);

Eina_Bool esvg_parser_path(const char *value, Enesim_Renderer *r);

Eina_Bool esvg_uri_get(Edom_Tag **tag, Edom_Tag *rel, const char *attr);
Eina_Bool esvg_href_get(Edom_Tag **tag, Edom_Tag *rel, const char *href);

Eina_Bool esvg_paint_get(Esvg_Paint *paint, Edom_Tag *tag, const char *attr);
Esvg_Stroke_Line_Cap esvg_stroke_line_cap_get(const char *value);
Esvg_Stroke_Line_Join esvg_stroke_line_join_get(const char *value);

Eina_Bool esvg_parser_is_uri(const char *attr);

Eina_Bool esvg_parser_gradient_units_get(Esvg_Gradient_Units *gu, const char *value);
Eina_Bool esvg_parser_fill_rule_get(Esvg_Fill_Rule *rule, const char *attr);
Eina_Bool esvg_parser_spread_method_get(Esvg_Spread_Method *smethod, const char *attr);

Eina_Bool esvg_length_get(Esvg_Length *l, const char *attr_val, Esvg_Length default_length);

Esvg_View_Box esvg_view_box_get(const char *attr_val);

Eina_Bool esvg_color_get(Esvg_Color *color, const char *attr_val);

Eina_Bool esvg_transformation_get(Enesim_Matrix *matrix, const char *attr);

double esvg_number_get(const char *attr_val, double default_nbr);


#endif
