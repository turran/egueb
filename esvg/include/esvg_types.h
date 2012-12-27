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
#ifndef _ESVG_TYPES_H
#define _ESVG_TYPES_H

#include "esvg_length.h"
#include "esvg_color.h"
#include "esvg_paint.h"
#include "esvg_number.h"
#include "esvg_boolean.h"

#define ESVG_CLOCK_SECONDS (1000000000LL)
#define ESVG_CLOCK_MSECONDS (1000000LL)
#define ESVG_CLOCK_AS_SECONDS(t) ((t) / ESVG_CLOCK_SECONDS)

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
	ESVG_VIDEO,
	ESVG_VIEW,
	ESVG_VKERN,
	ESVG_PARSE_TAGS
} Esvg_Type;

typedef enum _Esvg_Attribute_Type
{
	ESVG_ATTR_CSS,
	ESVG_ATTR_XML,
	ESVG_ATTR_AUTO,
	ESVG_ATTR_TYPES,
} Esvg_Attribute_Type;

typedef struct _Esvg_Animated_Transform
{
	Enesim_Matrix base;
	Enesim_Matrix anim;
} __attribute__((packed)) Esvg_Animated_Transform;

typedef struct _Esvg_Animated_String
{
	char *base;
	char *anim;
} Esvg_Animated_String;

typedef struct _Esvg_Animated_Enum
{
	int base;
	int anim;
} Esvg_Animated_Enum;

typedef struct _Esvg_Animated_List
{
	Eina_List *base;
	Eina_List *anim;
} Esvg_Animated_List;

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

typedef enum _Esvg_Spread_Method
{
	ESVG_SPREAD_METHOD_PAD,
	ESVG_SPREAD_METHOD_REFLECT,
	ESVG_SPREAD_METHOD_REPEAT,
} Esvg_Spread_Method;

typedef enum _Esvg_Clip_Path_Type
{
	ESVG_CLIP_PATH_NONE,
	ESVG_CLIP_PATH_IRI,
	ESVG_CLIP_PATH_TYPES,
} Esvg_Clip_Path_Type;

typedef union _Esvg_Clip_Path_Value
{
	char *iri;
} __attribute__((packed)) Esvg_Clip_Path_Value;

typedef struct _Esvg_Clip_Path
{
	Esvg_Clip_Path_Type type;
	Esvg_Clip_Path_Value value;
} Esvg_Clip_Path;

typedef struct _Esvg_Animated_Clip_Path
{
	Esvg_Clip_Path base;
	Esvg_Clip_Path anim;
} Esvg_Animated_Clip_Path;

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
	Eina_Bool relative;
	Eina_Bool is_closed;
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

typedef enum _Esvg_Fill_Rule
{
	ESVG_NON_ZERO,
	ESVG_EVEN_ODD
} Esvg_Fill_Rule;

typedef struct _Esvg_Timing_Offset
{
	double offset;
} Esvg_Timing_Offset;

typedef struct _Esvg_Timing_Event
{
	char *id;
	char *event;
	double offset;
} Esvg_Timing_Event;

typedef struct _Esvg_Timing_Repeat
{
	char *id;
	double offset;
} Esvg_Timing_Repeat;

typedef enum _Esvg_Timing_Type
{
	ESVG_TIMING_OFFSET,
	ESVG_TIMING_EVENT,
	ESVG_TIMING_REPEAT,
	ESVG_TIMING_TYPES,
} Esvg_Timing_Type;

typedef struct _Esvg_Timing
{
	Esvg_Timing_Type type;
	union {
		Esvg_Timing_Offset offset;
		Esvg_Timing_Event event;
		Esvg_Timing_Repeat repeat;
		/*
		Esvg_Timing_Access_Key access_key;
		Esvg_Timing_Wall_Clock wall_clock;
		*/
	} data;
} Esvg_Timing;

typedef enum _Esvg_Calc_Mode
{
	ESVG_CALC_MODE_DISCRETE,
	ESVG_CALC_MODE_LINEAR,
	ESVG_CALC_MODE_PACED,
	ESVG_CALC_MODE_SPLINE,
	ESVG_CALC_MODES,
} Esvg_Calc_Mode;

typedef enum _Esvg_Duration_Type
{
	ESVG_DURATION_TYPE_CLOCK,
	ESVG_DURATION_TYPE_MEDIA,
	ESVG_DURATION_TYPE_INDEFINITE,
} Esvg_Duration_Type;

typedef struct _Esvg_Duration
{
	Esvg_Duration_Type type;
	union {
		int64_t clock;
	} data;
} Esvg_Duration;

typedef enum _Esvg_Additive
{
	ESVG_ADDITIVE_SUM,
	ESVG_ADDITIVE_REPLACE,
} Esvg_Additive;

typedef enum _Esvg_Accumulate
{
	ESVG_ACCUMULATE_SUM,
	ESVG_ACCUMULATE_NONE,
} Esvg_Accumulate;

typedef enum _Esvg_Fill
{
	ESVG_FILL_FREEZE,
	ESVG_FILL_REMOVE,
} Esvg_Fill;

typedef enum _Esvg_Animate_Transform_Type
{
	ESVG_ANIMATE_TRANSFORM_TYPE_TRANSLATE,
	ESVG_ANIMATE_TRANSFORM_TYPE_SCALE,
	ESVG_ANIMATE_TRANSFORM_TYPE_ROTATE,
	ESVG_ANIMATE_TRANSFORM_TYPE_SKEWX,
	ESVG_ANIMATE_TRANSFORM_TYPE_SKEWY,
	ESVG_ANIMATE_TRANSFORM_TYPES
} Esvg_Animate_Transform_Type;

typedef struct _Esvg_Animate_Key_Spline
{
	double x0;
	double y0;
	double x1;
	double y1;
} Esvg_Animate_Key_Spline;

typedef enum _Esvg_Animation_Event_Type
{
	ESVG_ANIMATION_EVENT_OFFSET,
	ESVG_ANIMATION_EVENT_SYNCBASE,
	ESVG_ANIMATION_EVENT_EVENT,
	ESVG_ANIMATION_EVENT_REPEAT,
	ESVG_ANIMATION_EVENT_ACCESS_KEY,
	ESVG_ANIMATION_EVENT_WALLCLOCK,
	ESVG_ANIMATION_EVENT_INDEFINITE,
	ESVG_ANIMATION_EVENT_TYPES,
} Esvg_Animation_Event_Type;

typedef struct _Esvg_Animation_Event
{
	Esvg_Animation_Event_Type type;
	char *id;
	char *event;
	int repeat;
	int key;
	int64_t offset;
} Esvg_Animation_Event;

typedef enum _Esvg_Visibility
{
	ESVG_VISIBILITY_VISIBLE,
	ESVG_VISIBILITY_HIDDEN,
	ESVG_VISIBILITY_COLLAPSE
} Esvg_Visibility;

typedef enum _Esvg_Display
{
	ESVG_DISPLAY_INLINE,
	ESVG_DISPLAY_BLOCK,
	ESVG_DISPLAY_LIST_ITEM,
	ESVG_DISPLAY_RUN_IN,
	ESVG_DISPLAY_COMPACT,
	ESVG_DISPLAY_MARKER,
	ESVG_DISPLAY_TABLE,
	ESVG_DISPLAY_INLINE_TABLE,
	ESVG_DISPLAY_TABLE_ROW_GROUP,
	ESVG_DISPLAY_TABLE_HEADER_GROUP,
	ESVG_DISPLAY_TABLE_FOOTER_GROUP,
	ESVG_DISPLAY_TABLE_ROW,
	ESVG_DISPLAY_TABLE_COLUMN_GROUP,
	ESVG_DISPLAY_TABLE_COLUMN,
	ESVG_DISPLAY_TABLE_CELL,
	ESVG_DISPLAY_TABLE_CAPTION,
	ESVG_DISPLAY_NONE,
} Esvg_Display;

typedef void (*Esvg_Uri_Get)(const char *uri, const char *fragment, void *data);

typedef struct _Esvg_Uri_Descriptor
{
	Esvg_Uri_Get local_get;
	Esvg_Uri_Get absolute_get;
	Esvg_Uri_Get relative_get;
} Esvg_Uri_Descriptor;

EAPI Eina_Bool esvg_iri_string_from(const char *attr, Esvg_Uri_Descriptor *descriptor, void *data);
EAPI Eina_Bool esvg_uri_string_from(const char *attr, Esvg_Uri_Descriptor *descriptor, void *data);

typedef void (*Esvg_Free_Cb)(void *data);
typedef void (*Esvg_Command_Cb)(Esvg_Path_Command *cmd, void *data);
typedef void (*Esvg_Timing_Cb)(Esvg_Timing *t, void *data);
typedef void (*Esvg_Points_Cb)(Esvg_Point *p, void *data);
typedef void (*Esvg_List_Cb)(const char *item, void *data);
typedef Eina_Bool (*Esvg_Number_List_Cb)(double v, void *data);

EAPI const char * esvg_type_string_to(Esvg_Type type);

EAPI Eina_Bool esvg_string_is_equal(const char *s1, const char *s2);

/* FIXME rename this: remove the _parser_ */
EAPI Eina_Bool esvg_parser_gradient_units_string_from(Esvg_Gradient_Units *gu, const char *value);
EAPI Eina_Bool esvg_parser_fill_rule_get(Esvg_Fill_Rule *rule, const char *attr);
EAPI Eina_Bool esvg_parser_spread_method_get(Esvg_Spread_Method *smethod, const char *attr);

EAPI Eina_Bool esvg_path_string_from(const char *value, Esvg_Command_Cb cb, void *data);
EAPI Eina_Bool esvg_transformation_string_from(Enesim_Matrix *matrix, const char *attr);

/* FIXME rename this from _get to _string_from */
Esvg_View_Box esvg_view_box_get(const char *attr_val);
EAPI Esvg_Stroke_Line_Cap esvg_stroke_line_cap_string_from(const char *value);
EAPI Esvg_Stroke_Line_Join esvg_stroke_line_join_string_from(const char *value);
EAPI Eina_Bool esvg_attribute_type_string_from(Esvg_Attribute_Type *type, const char *value);
EAPI void esvg_timing_string_from(const char *attr, Esvg_Timing_Cb cb, void *data);
EAPI void esvg_points_string_from(const char *value, Esvg_Points_Cb cb, void *data);
EAPI Eina_Bool esvg_animate_transform_type_string_from(Esvg_Animate_Transform_Type *type, const char *s);
EAPI Eina_Bool esvg_animate_key_spline_string_from(Esvg_Animate_Key_Spline *spline, const char *s);
EAPI Eina_Bool esvg_list_string_from(const char *attr, char sep, Esvg_List_Cb cb, void *data);
EAPI Eina_Bool esvg_number_list_string_from(const char *attr, Esvg_Number_List_Cb cb, void *data);
EAPI Eina_Bool esvg_clock_string_from(int64_t *clock, const char *attr);
EAPI Eina_Bool esvg_calc_mode_string_from(Esvg_Calc_Mode *c, const char *attr);
EAPI Eina_Bool esvg_duration_string_from(Esvg_Duration *d, const char *attr);
EAPI Eina_Bool esvg_additive_string_from(Esvg_Additive *add, const char *attr);
EAPI Eina_Bool esvg_accumulate_string_from(Esvg_Accumulate *acc, const char *attr);
EAPI Eina_Bool esvg_repeat_count_string_from(int *rc, const char *attr);
EAPI Eina_Bool esvg_visibility_string_from(Esvg_Visibility *v, const char *attr);
EAPI Eina_Bool esvg_display_string_from(Esvg_Display *d, const char *attr);
EAPI Eina_Bool esvg_clip_path_is_equal(const Esvg_Clip_Path *p1,
		const Esvg_Clip_Path *p2);
EAPI Eina_Bool esvg_clip_path_string_from(Esvg_Clip_Path *cp, const char *attr);
EAPI Eina_Bool esvg_type_is_animation(Esvg_Type type);
EAPI Eina_Bool esvg_type_is_paint_server(Esvg_Type type);
EAPI Eina_Bool esvg_type_is_shape(Esvg_Type type);
EAPI Eina_Bool esvg_type_is_renderable(Esvg_Type type);

/* animation parsing */
EAPI Eina_Bool esvg_animation_event_list_string_from(Eina_List **l, const char *s);
EAPI Eina_Bool esvg_animation_event_string_from(Esvg_Animation_Event *e, const char *s);

EAPI void esvg_animation_event_offset_set(Esvg_Animation_Event *a, int64_t offset);
EAPI Eina_Bool esvg_fill_string_from(Esvg_Fill *fill, const char *s);
#endif
