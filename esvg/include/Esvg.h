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
#ifndef _ESVG_H
#define _ESVG_H

#include <Eina.h>
#include <Edom.h>
#include <Enesim.h>
#include <Ender.h>

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef ESVG_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
# else
#  define EAPI __declspec(dllimport)
# endif /* ! EFL_ESVG_BUILD */
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif

/**
 * @mainpage Esvg
 * @section intro Introduction
 * @section dependencies Dependencies
 * - Eina
 * - Enesim
 * - Etex
 *
 * @todo
 */

/*
 * @defgroup Ender_Main_Group Main
 * @{
 */
EAPI int esvg_init(void);
EAPI int esvg_shutdown(void);

typedef enum _Esvg_Type {
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
	ESVG_SVG, /* 70 */
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
} Esvg_Type;


/* FIXME implement the unit type as the spec or just
 * ABSOLUTE/RELATIVE?
 */
typedef enum _Esvg_Unit_Length
{
	ESVG_UNIT_LENGTH_EM,
	ESVG_UNIT_LENGTH_EX,
	ESVG_UNIT_LENGTH_PX,
	ESVG_UNIT_LENGTH_PT,
	ESVG_UNIT_LENGTH_PC,
	ESVG_UNIT_LENGTH_CM,
	ESVG_UNIT_LENGTH_MM,
	ESVG_UNIT_LENGTH_IN,
	ESVG_UNIT_LENGTH_PERCENT,
	ESVG_UNIT_LENGTHS
} Esvg_Unit_Length;

typedef struct _Esvg_Length
{
	double value;
	Esvg_Unit_Length unit;
} Esvg_Length;

typedef Esvg_Length Esvg_Coord;

typedef struct _Esvg_Color
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} Esvg_Color;

/* FIXME when doing clipping or masking, we also use this
 * so better change the name
 */
typedef enum _Esvg_Gradient_Units
{
	ESVG_USER_SPACE_ON_USE,
	ESVG_OBJECT_BOUNDING_BOX,
	ESVG_GRADIENT_UNITS,
} Esvg_Gradient_Units, Esvg_Pattern_Units;

typedef Esvg_Gradient_Units Esvg_Clip_Path_Units;

typedef enum _Esvg_Paint_Server_Type
{
	ESVG_PAINT_SERVER_LINEAR_GRADIENT,
	ESVG_PAINT_SERVER_RADIAL_GRADIENT,
	ESVG_PAINT_SERVER_PATTERN,
	ESVG_PAINT_SERVER_TYPES
} Esvg_Paint_Server_Type;

/* FIXME the offset should not be a length
 * as it only accepts a number or percentage
 * no other units
 */
typedef struct _Esvg_Gradient_Stop
{
	Esvg_Length offset;
	Esvg_Color stop_color;
	double stop_opacity;
} Esvg_Gradient_Stop;

typedef enum _Esvg_Spread_Method
{
	ESVG_SPREAD_METHOD_PAD,
	ESVG_SPREAD_METHOD_REFLECT,
	ESVG_SPREAD_METHOD_REPEAT,
} Esvg_Spread_Method;

typedef enum _Esvg_Paint_Type
{
	ESVG_PAINT_NONE,
	ESVG_PAINT_CURRENT_COLOR,
	ESVG_PAINT_COLOR,
	ESVG_PAINT_SERVER,
	ESVG_PAINT_TYPES,
} Esvg_Paint_Type;

typedef struct _Esvg_Paint
{
	Esvg_Paint_Type type;
	union {
		Esvg_Color color;
		Enesim_Renderer *paint_server;
	} value;
} Esvg_Paint;

typedef enum _Esvg_Path_Command_Type
{
	ESVG_PATH_MOVE_TO,
	ESVG_PATH_LINE_TO,
	ESVG_PATH_HLINE_TO,
	ESVG_PATH_VLINE_TO,
	ESVG_PATH_CUBIC_TO,
	ESVG_PATH_SCUBIC_TO,
	ESVG_PATH_QUADRATIC_TO,
	ESVG_PATH_SQUADRATIC_TO,
	ESVG_PATH_ARC_TO,
	ESVG_PATH_CLOSE,
	ESVG_PATH_COMMAND_TYPES
} Esvg_Path_Command_Type;

typedef struct _Esvg_Path_Command
{
	Esvg_Path_Command_Type type;
	union {
		struct {
			double c;
		} hline_to, vline_to;
		struct {
			double x;
			double y;
		} move_to, line_to, squadratic_to;
		struct {
			double ctrl_x0;
			double ctrl_y0;
			double ctrl_x1;
			double ctrl_y1;
			double x;
			double y;
		} cubic_to;
		struct {
			double ctrl_x;
			double ctrl_y;
			double x;
			double y;
		} scubic_to, quadratic_to;
		struct {
			double rx;
			double ry;
			double angle;
			double large;
			double sweep;
			double x;
			double y;
		} arc_to;
 	} data;
	Eina_Bool relative : 1;
	Eina_Bool is_closed : 1;
} Esvg_Path_Command;

typedef struct _Esvg_Point
{
	double x;
	double y;
} Esvg_Point;

typedef struct _Esvg_View_Box
{
	double min_x;
	double min_y;
	double width;
	double height;
} Esvg_View_Box;


typedef enum _Esvg_Stroke_Line_Cap
{
	ESVG_LINE_CAP_BUTT,
	ESVG_LINE_CAP_ROUND,
	ESVG_LINE_CAP_SQUARE,
} Esvg_Stroke_Line_Cap;

typedef enum _Esvg_Stroke_Line_Join
{
	ESVG_LINE_JOIN_MITER,
	ESVG_LINE_JOIN_ROUND,
	ESVG_LINE_JOIN_BEVEL,
} Esvg_Stroke_Line_Join;

/* attributes */
typedef struct _Esvg_Attribute_Regular {
} Esvg_Attribute_Regular;

/* ‘alignment-baseline’,
 * ‘baseline-shift’,
 * ‘clip-rule’,
 * ‘clip’,
 * ‘color-interpolation-filters’,
 * ‘color-interpolation’,
 * ‘color-profile’,
 * ‘color-rendering’,
 * ‘cursor’,
 * ‘direction’,
 * ‘display’,
 * ‘dominant-baseline’,
 * ‘enable-background’,
 * ‘filter’,
 * ‘flood-color’,
 * ‘flood-opacity’,
 * ‘font-family’,
 * ‘font-size-adjust’,
 * ‘font-size’,
 * ‘font-stretch’,
 * ‘font-style’,
 * ‘font-variant’,
 * ‘font-weight’,
 * ‘glyph-orientation-horizontal’,
 * ‘glyph-orientation-vertical’,
 * ‘image-rendering’,
 * ‘kerning’,
 * ‘letter-spacing’,
 * ‘lighting-color’,
 * ‘marker-end’,
 * ‘marker-mid’,
 * ‘marker-start’,
 * ‘mask’,
 * ‘overflow’,
 * ‘pointer-events’,
 * ‘shape-rendering’,
 * ‘stroke-dasharray’,
 * ‘stroke-dashoffset’,
 * ‘stroke-miterlimit’,
 * ‘text-anchor’,
 * ‘text-decoration’,
 * ‘text-rendering’,
 * ‘unicode-bidi’,
 * ‘word-spacing’
 * ‘writing-mode’
 */

typedef enum _Esvg_Fill_Rule
{
	ESVG_NON_ZERO,
	ESVG_EVEN_ODD
} Esvg_Fill_Rule;

typedef struct _Esvg_Attribute_Presentation {
	Enesim_Renderer *clip_path;
	Esvg_Color color;
	double opacity;
	Esvg_Paint fill;
	Esvg_Paint stroke;
	Esvg_Length stroke_width;
	Esvg_Stroke_Line_Cap stroke_line_cap;
	Esvg_Stroke_Line_Join stroke_line_join;
	double stroke_opacity;
	double fill_opacity;
	Esvg_Fill_Rule fill_rule;
	Eina_Bool visibility;
	Esvg_Color stop_color;
	double stop_opacity;
	/* are they set? */
	Eina_Bool clip_path_set;
	Eina_Bool color_set;
	Eina_Bool opacity_set;
	Eina_Bool fill_set;
	Eina_Bool stroke_set;
	Eina_Bool stroke_width_set;
	Eina_Bool stroke_line_cap_set;
	Eina_Bool stroke_line_join_set;
	Eina_Bool stroke_opacity_set;
	Eina_Bool fill_opacity_set;
	Eina_Bool fill_rule_set;
	Eina_Bool visibility_set;
	Eina_Bool stop_color_set;
	Eina_Bool stop_opacity_set;
} Esvg_Attribute_Presentation;

/**
 * @}
 */

EAPI Eina_Bool esvg_is_element(Ender_Element *e);
EAPI Esvg_Type esvg_element_type_get(Ender_Element *e);
EAPI void esvg_element_id_set(Ender_Element *e, const char *id);
EAPI void esvg_element_id_get(Ender_Element *e, const char **id);
EAPI void esvg_element_class_set(Ender_Element *e, const char *id);
EAPI void esvg_element_transform_set(Ender_Element *e, const Enesim_Matrix *transform);
EAPI void esvg_element_transform_get(Ender_Element *e, Enesim_Matrix *transform);
EAPI void esvg_element_style_set(Ender_Element *e, const Esvg_Attribute_Presentation *style);
EAPI void esvg_element_style_get(Ender_Element *e, Esvg_Attribute_Presentation *style);
EAPI Eina_Bool esvg_element_style_is_set(Ender_Element *e);
EAPI void esvg_element_clip_path_set(Ender_Element *e, const char *id);
EAPI void esvg_element_clip_path_unset(Ender_Element *e);
EAPI void esvg_element_opacity_set(Ender_Element *e, double opacity);
EAPI void esvg_element_opacity_unset(Ender_Element *e);
EAPI void esvg_element_x_dpi_set(Ender_Element *e, double x_dpi);
EAPI void esvg_element_x_dpi_get(Ender_Element *e, double *x_dpi);
EAPI void esvg_element_y_dpi_set(Ender_Element *e, double y_dpi);
EAPI void esvg_element_y_dpi_get(Ender_Element *e, double *y_dpi);
EAPI void esvg_element_container_width_set(Ender_Element *e, double container_width);
EAPI void esvg_element_container_width_get(Ender_Element *e, double *container_width);
EAPI void esvg_element_container_height_set(Ender_Element *e, double container_height);
EAPI void esvg_element_container_height_get(Ender_Element *e, double *container_height);

EAPI void esvg_element_color_set(Ender_Element *e, const Esvg_Color *color);
EAPI void esvg_element_color_unset(Ender_Element *e);
EAPI void esvg_element_fill_set(Ender_Element *e, const Esvg_Paint *fill);
EAPI void esvg_element_fill_unset(Ender_Element *e);
EAPI void esvg_element_fill_opacity_set(Ender_Element *e, double fill_opacity);
EAPI void esvg_element_fill_opacity_unset(Ender_Element *e);
EAPI void esvg_element_fill_rule_set(Ender_Element *e, Esvg_Fill_Rule fill_rule);
EAPI void esvg_element_stroke_set(Ender_Element *e, const Esvg_Paint *stroke);
EAPI void esvg_element_stroke_unset(Ender_Element *e);
EAPI void esvg_element_stroke_width_set(Ender_Element *e, const Esvg_Length *stroke_width);
EAPI void esvg_element_stroke_width_unset(Ender_Element *e);
EAPI void esvg_element_stroke_opacity_set(Ender_Element *e, double stroke_opacity);
EAPI void esvg_element_stroke_opacity_unset(Ender_Element *e);
EAPI void esvg_element_stroke_line_cap_set(Ender_Element *e, Esvg_Stroke_Line_Cap cap);
EAPI void esvg_element_stroke_line_cap_unset(Ender_Element *e);
EAPI void esvg_element_stroke_line_join_set(Ender_Element *e, Esvg_Stroke_Line_Join join);
EAPI void esvg_element_stroke_line_join_unset(Ender_Element *e);
EAPI void esvg_element_visibility_set(Ender_Element *e, Eina_Bool visibility);
EAPI void esvg_element_visibility_unset(Ender_Element *e);

EAPI Ender_Element * esvg_svg_new(void);
EAPI Eina_Bool esvg_is_svg(Ender_Element *e);
EAPI void esvg_svg_version_set(Ender_Element *e, double version);
EAPI void esvg_svg_version_get(Ender_Element *e, double *version);
EAPI void esvg_svg_x_set(Ender_Element *e, Esvg_Coord *x);
EAPI void esvg_svg_y_set(Ender_Element *e, Esvg_Coord *y);
EAPI void esvg_svg_width_set(Ender_Element *e, Esvg_Length *width);
EAPI void esvg_svg_height_set(Ender_Element *e, Esvg_Length *height);
EAPI void esvg_svg_viewbox_set(Ender_Element *e, Esvg_View_Box *vb);
EAPI void esvg_svg_actual_width_get(Ender_Element *e, double *actual_width);
EAPI void esvg_svg_actual_height_get(Ender_Element *e, double *actual_height);

EAPI Edom_Tag * esvg_g_new(void);
EAPI Eina_Bool esvg_is_g(Edom_Tag *e);

EAPI Eina_Bool esvg_is_shape(Edom_Tag *e);

EAPI Edom_Tag *esvg_rect_new(void);
EAPI Eina_Bool esvg_is_rect(Edom_Tag *e);
EAPI void esvg_rect_x_set(Edom_Tag *e, const Esvg_Coord *x);
EAPI void esvg_rect_x_get(Edom_Tag *e, Esvg_Coord *x);
EAPI void esvg_rect_y_set(Edom_Tag *e, const Esvg_Coord *y);
EAPI void esvg_rect_y_get(Edom_Tag *e, Esvg_Coord *y);
EAPI void esvg_rect_width_set(Edom_Tag *e, const Esvg_Length *width);
EAPI void esvg_rect_width_get(Edom_Tag *e, Esvg_Length *width);
EAPI void esvg_rect_height_set(Edom_Tag *e, const Esvg_Length *height);
EAPI void esvg_rect_height_get(Edom_Tag *e, Esvg_Length *height);
EAPI void esvg_rect_rx_set(Edom_Tag *e, const Esvg_Coord *rx);
EAPI void esvg_rect_rx_get(Edom_Tag *e, Esvg_Coord *rx);
EAPI void esvg_rect_ry_set(Edom_Tag *e, const Esvg_Coord *ry);
EAPI void esvg_rect_ry_get(Edom_Tag *e, Esvg_Coord *ry);

EAPI Edom_Tag * esvg_ellipse_new(void);
EAPI Eina_Bool esvg_is_ellipse(Edom_Tag *e);
EAPI void esvg_ellipse_cx_set(Edom_Tag *e, const Esvg_Coord *cx);
EAPI void esvg_ellipse_cx_get(Edom_Tag *e, Esvg_Coord *cx);
EAPI void esvg_ellipse_cy_set(Edom_Tag *e, const Esvg_Coord *cy);
EAPI void esvg_ellipse_cy_get(Edom_Tag *e, Esvg_Coord *cy);
EAPI void esvg_ellipse_rx_set(Edom_Tag *e, const Esvg_Length *rx);
EAPI void esvg_ellipse_rx_get(Edom_Tag *e, Esvg_Length *rx);
EAPI void esvg_ellipse_ry_set(Edom_Tag *e, const Esvg_Length *ry);
EAPI void esvg_ellipse_ry_get(Edom_Tag *e, Esvg_Length *ry);

EAPI Edom_Tag * esvg_circle_new(void);
EAPI Eina_Bool esvg_is_circle(Edom_Tag *e);
EAPI void esvg_circle_cx_set(Edom_Tag *e, const Esvg_Coord *cx);
EAPI void esvg_circle_cx_get(Edom_Tag *e, Esvg_Coord *cx);
EAPI void esvg_circle_cy_set(Edom_Tag *e, const Esvg_Coord *cy);
EAPI void esvg_circle_cy_get(Edom_Tag *e, Esvg_Coord *cy);
EAPI void esvg_circle_radius_set(Edom_Tag *e, const Esvg_Length *radius);
EAPI void esvg_circle_radius_get(Edom_Tag *e, Esvg_Length *radius);

EAPI Edom_Tag * esvg_path_new(void);
EAPI Eina_Bool esvg_is_path(Edom_Tag *e);
EAPI void esvg_path_command_add(Edom_Tag *e, const Esvg_Path_Command *cmd);

EAPI Edom_Tag * esvg_line_new(void);
EAPI Eina_Bool esvg_is_line(Edom_Tag *e);
EAPI void esvg_line_x1_set(Edom_Tag *e, const Esvg_Coord *x1);
EAPI void esvg_line_x1_get(Edom_Tag *e, Esvg_Coord *x1);
EAPI void esvg_line_y1_set(Edom_Tag *e, const Esvg_Coord *y1);
EAPI void esvg_line_y1_get(Edom_Tag *e, Esvg_Coord *y1);
EAPI void esvg_line_x2_set(Edom_Tag *e, const Esvg_Coord *x2);
EAPI void esvg_line_x2_get(Edom_Tag *e, Esvg_Coord *x2);
EAPI void esvg_line_y2_set(Edom_Tag *e, const Esvg_Coord *y2);
EAPI void esvg_line_y2_get(Edom_Tag *e, Esvg_Coord *y2);

EAPI Edom_Tag * esvg_polyline_new(void);
EAPI void esvg_polyline_point_add(Edom_Tag *e, Esvg_Point *p);

EAPI Edom_Tag * esvg_polygon_new(void);
EAPI Eina_Bool esvg_is_polygon(Edom_Tag *e);
EAPI void esvg_polygon_point_add(Edom_Tag *e, Esvg_Point *p);

EAPI Edom_Tag * esvg_use_new(void);
EAPI Eina_Bool esvg_is_use(Edom_Tag *e);
EAPI void esvg_use_x_set(Edom_Tag *e, const Esvg_Coord *x);
EAPI void esvg_use_x_get(Edom_Tag *e, Esvg_Coord *x);
EAPI void esvg_use_y_set(Edom_Tag *e, const Esvg_Coord *y);
EAPI void esvg_use_y_get(Edom_Tag *e, Esvg_Coord *y);
EAPI void esvg_use_width_set(Edom_Tag *e, const Esvg_Length *width);
EAPI void esvg_use_width_get(Edom_Tag *e, Esvg_Length *width);
EAPI void esvg_use_height_set(Edom_Tag *e, const Esvg_Length *height);
EAPI void esvg_use_height_get(Edom_Tag *e, Esvg_Length *height);
EAPI void esvg_use_link_set(Edom_Tag *e, const Edom_Tag *link);
EAPI void esvg_use_link_get(Edom_Tag *e, Edom_Tag **link);

EAPI Edom_Tag * esvg_text_new(void);
EAPI Eina_Bool esvg_is_text(Edom_Tag *e);
EAPI void esvg_text_x_set(Edom_Tag *e, const Esvg_Coord *x);
EAPI void esvg_text_x_get(Edom_Tag *e, Esvg_Coord *x);
EAPI void esvg_text_y_set(Edom_Tag *e, const Esvg_Coord *y);
EAPI void esvg_text_y_get(Edom_Tag *e, Esvg_Coord *y);
EAPI void esvg_text_font_size_set(Edom_Tag *e, const Esvg_Length *size);
EAPI void esvg_text_font_family_set(Edom_Tag *e, const char *family);
EAPI void esvg_text_text_set(Edom_Tag *e, const char *text);
EAPI void esvg_text_text_get(Edom_Tag *e, const char **text);

EAPI Edom_Tag * esvg_image_new(void);
EAPI void esvg_image_x_set(Edom_Tag *e,  const Esvg_Coord *x);
EAPI void esvg_image_x_get(Edom_Tag *e, Esvg_Coord *x);
EAPI void esvg_image_y_set(Edom_Tag *e,  const Esvg_Coord *y);
EAPI void esvg_image_y_get(Edom_Tag *e, Esvg_Coord *y);
EAPI void esvg_image_width_set(Edom_Tag *e,  const Esvg_Length *width);
EAPI void esvg_image_width_get(Edom_Tag *e, Esvg_Length *width);
EAPI void esvg_image_height_set(Edom_Tag *e,  const Esvg_Length *height);
EAPI void esvg_image_height_get(Edom_Tag *e, Esvg_Length *height);
EAPI void esvg_image_href_set(Edom_Tag *e, const char *href);
EAPI void esvg_image_href_get(Edom_Tag *e, const char **href);

EAPI Eina_Bool esvg_is_gradient(Edom_Tag *e);
EAPI void esvg_gradient_stop_add(Edom_Tag *e, Esvg_Gradient_Stop *s);
EAPI void esvg_gradient_stop_get(Edom_Tag *e, const Eina_List **l);
EAPI void esvg_gradient_units_set(Edom_Tag *e, Esvg_Gradient_Units units);
EAPI void esvg_gradient_units_get(Edom_Tag *e, Esvg_Gradient_Units *units);
EAPI Eina_Bool esvg_gradient_units_is_set(Edom_Tag *e);
EAPI void esvg_gradient_transform_set(Edom_Tag *e, const Enesim_Matrix *transform);
EAPI void esvg_gradient_transform_get(Edom_Tag *e, Enesim_Matrix *transform);
EAPI Eina_Bool esvg_gradient_transform_is_set(Edom_Tag *e);
EAPI void esvg_gradient_spread_method_set(Edom_Tag *e, Esvg_Spread_Method spread_method);
EAPI void esvg_gradient_spread_method_get(Edom_Tag *e, Esvg_Spread_Method *spread_method);

EAPI Edom_Tag * esvg_radial_gradient_new(void);
EAPI Eina_Bool esvg_is_radial_gradient(Edom_Tag *e);
EAPI void esvg_radial_gradient_cx_set(Edom_Tag *e, const Esvg_Coord *cx);
EAPI void esvg_radial_gradient_cx_get(Edom_Tag *e, Esvg_Coord *cx);
EAPI Eina_Bool esvg_radial_gradient_cx_is_set(Edom_Tag *e);
EAPI void esvg_radial_gradient_cy_set(Edom_Tag *e, const Esvg_Coord *cy);
EAPI void esvg_radial_gradient_cy_get(Edom_Tag *e, Esvg_Coord *cy);
EAPI Eina_Bool esvg_radial_gradient_cy_is_set(Edom_Tag *e);
EAPI void esvg_radial_gradient_fx_set(Edom_Tag *e, const Esvg_Coord *fx);
EAPI void esvg_radial_gradient_fx_get(Edom_Tag *e, Esvg_Coord *fx);
EAPI Eina_Bool esvg_radial_gradient_fx_is_set(Edom_Tag *e);
EAPI void esvg_radial_gradient_fy_set(Edom_Tag *e, const Esvg_Coord *fy);
EAPI void esvg_radial_gradient_fy_get(Edom_Tag *e, Esvg_Coord *fy);
EAPI Eina_Bool esvg_radial_gradient_fy_is_set(Edom_Tag *e);
EAPI void esvg_radial_gradient_r_set(Edom_Tag *e, const Esvg_Length *rad);
EAPI void esvg_radial_gradient_r_get(Edom_Tag *e, Esvg_Length *rad);
EAPI Eina_Bool esvg_radial_gradient_r_is_set(Edom_Tag *e);

EAPI Edom_Tag * esvg_linear_gradient_new(void);
EAPI Eina_Bool esvg_is_linear_gradient(Edom_Tag *e);
EAPI void esvg_linear_gradient_x1_set(Edom_Tag *e, const Esvg_Coord *x1);
EAPI void esvg_linear_gradient_x1_get(Edom_Tag *e, Esvg_Coord *x1);
EAPI Eina_Bool esvg_linear_gradient_x1_is_set(Edom_Tag *e);
EAPI void esvg_linear_gradient_y1_set(Edom_Tag *e, const Esvg_Coord *y1);
EAPI void esvg_linear_gradient_y1_get(Edom_Tag *e, Esvg_Coord *y1);
EAPI Eina_Bool esvg_linear_gradient_y1_is_set(Edom_Tag *e);
EAPI void esvg_linear_gradient_x2_set(Edom_Tag *e, const Esvg_Coord *x2);
EAPI void esvg_linear_gradient_x2_get(Edom_Tag *e, Esvg_Coord *x2);
EAPI Eina_Bool esvg_linear_gradient_x2_is_set(Edom_Tag *e);
EAPI void esvg_linear_gradient_y2_set(Edom_Tag *e, const Esvg_Coord *y2);
EAPI void esvg_linear_gradient_y2_get(Edom_Tag *e, Esvg_Coord *y2);
EAPI Eina_Bool esvg_linear_gradient_y2_is_set(Edom_Tag *e);

EAPI Eina_Bool esvg_is_paint_server(Edom_Tag *e);

EAPI Edom_Tag * esvg_pattern_new(void);
EAPI Eina_Bool esvg_is_pattern(Edom_Tag *e);
EAPI void esvg_pattern_x_set(Edom_Tag *e, const Esvg_Coord *x);
EAPI void esvg_pattern_x_get(Edom_Tag *e, Esvg_Coord *x);
EAPI void esvg_pattern_y_set(Edom_Tag *e, const Esvg_Coord *y);
EAPI void esvg_pattern_y_get(Edom_Tag *e, Esvg_Coord *y);
EAPI void esvg_pattern_width_set(Edom_Tag *e, const Esvg_Length *width);
EAPI void esvg_pattern_width_get(Edom_Tag *e, Esvg_Length *width);
EAPI void esvg_pattern_height_set(Edom_Tag *e, const Esvg_Length *height);
EAPI void esvg_pattern_height_get(Edom_Tag *e, Esvg_Length *height);
EAPI void esvg_pattern_units_set(Edom_Tag *e, Esvg_Pattern_Units units);
EAPI void esvg_pattern_units_get(Edom_Tag *e, Esvg_Pattern_Units *units);
EAPI Eina_Bool esvg_pattern_units_is_set(Edom_Tag *e);
EAPI void esvg_pattern_transform_set(Edom_Tag *e, const Enesim_Matrix *transform);
EAPI void esvg_pattern_transform_get(Edom_Tag *e, Enesim_Matrix *transform);
EAPI Eina_Bool esvg_pattern_transform_is_set(Edom_Tag *e);
EAPI void esvg_pattern_content_set(Edom_Tag *e, Edom_Tag *content);

EAPI Edom_Tag * esvg_clip_path_new(void);
EAPI Eina_Bool esvg_is_clip_path(Edom_Tag *thiz);

/* helper functions */
EAPI double esvg_length_final_get(const Esvg_Length *l, double parent_length);


#endif /*_ESVG_H*/
