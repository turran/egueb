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
} Esvg_Animated_Transform;

typedef double Esvg_Number;

typedef struct _Esvg_Animated_Number
{
	Esvg_Number base;
	Esvg_Number anim;
} Esvg_Animated_Number;

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
} __attribute__((packed)) Esvg_Length;

typedef Esvg_Length Esvg_Coord;

typedef struct _Esvg_Animated_Length
{
	Esvg_Length base;
	Esvg_Length anim;
} Esvg_Animated_Length;

typedef Esvg_Animated_Length Esvg_Animated_Coord;

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

/* We need to use uint32_t here because we can not pass
 * data < 32bits through varargs
 */
typedef struct _Esvg_Color
{
	uint32_t r;
	uint32_t g;
	uint32_t b;
} Esvg_Color;

typedef struct _Esvg_Animated_Color
{
	Esvg_Color base;
	Esvg_Color anim;
} Esvg_Animated_Color;

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

typedef enum _Esvg_Paint_Type
{
	ESVG_PAINT_NONE,
	ESVG_PAINT_CURRENT_COLOR,
	ESVG_PAINT_COLOR,
	ESVG_PAINT_SERVER,
	ESVG_PAINT_TYPES,
} Esvg_Paint_Type;

typedef union _Esvg_Paint_Value
{
	Esvg_Color color;
	char *paint_server;
} __attribute__((packed)) Esvg_Paint_Value;

typedef struct _Esvg_Paint
{
	Esvg_Paint_Type type;
	Esvg_Paint_Value value;
} Esvg_Paint;

typedef struct _Esvg_Animated_Paint
{
	Esvg_Paint base;
	Esvg_Paint anim;
} Esvg_Animated_Paint;

typedef struct _Esvg_Animated_Bool
{
	Eina_Bool base;
	Eina_Bool anim;
} Esvg_Animated_Bool;

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

typedef void (*Esvg_Uri_Get)(const char *uri, const char *fragment, void *data);

typedef struct _Esvg_Uri_Descriptor
{
	Esvg_Uri_Get local_get;
	Esvg_Uri_Get absolute_get;
	Esvg_Uri_Get relative_get;
} Esvg_Uri_Descriptor;

EAPI Eina_Bool esvg_iri_string_from(const char *attr, Esvg_Uri_Descriptor *descriptor, void *data);
EAPI Eina_Bool esvg_uri_string_from(const char *attr, Esvg_Uri_Descriptor *descriptor, void *data);

extern Esvg_Length ESVG_LENGTH_0;
extern Esvg_Length ESVG_LENGTH_1;
extern Esvg_Length ESVG_LENGTH_100_PERCENT;
#define ESVG_COORD_0 ESVG_LENGTH_0

static inline Eina_Bool esvg_length_is_relative(Esvg_Length *length)
{
	return (length->unit == ESVG_UNIT_LENGTH_PERCENT) ? EINA_TRUE : EINA_FALSE;
}

static inline void esvg_length_set(Esvg_Length *length, double value, Esvg_Unit_Length unit)
{
	length->value = value;
	length->unit = unit;
}

typedef void (*Esvg_Free_Cb)(void *data);
typedef void (*Esvg_Command_Cb)(Esvg_Path_Command *cmd, void *data);
typedef void (*Esvg_Timing_Cb)(Esvg_Timing *t, void *data);
typedef void (*Esvg_Points_Cb)(Esvg_Point *p, void *data);
typedef void (*Esvg_List_Cb)(const char *item, void *data);
typedef Eina_Bool (*Esvg_Number_List_Cb)(double v, void *data);

EAPI const char * esvg_type_string_to(Esvg_Type type);

EAPI Eina_Bool esvg_string_is_equal(const char *s1, const char *s2);

EAPI double esvg_number_string_from(const char *attr_val, double default_nbr);
/* FIXME rename this: remove the _parser_ */
EAPI Eina_Bool esvg_parser_gradient_units_string_from(Esvg_Gradient_Units *gu, const char *value);
EAPI Eina_Bool esvg_parser_fill_rule_get(Esvg_Fill_Rule *rule, const char *attr);
EAPI Eina_Bool esvg_parser_spread_method_get(Esvg_Spread_Method *smethod, const char *attr);

EAPI Eina_Bool esvg_color_string_from(Esvg_Color *color, const char *attr_val);
EAPI Eina_Bool esvg_color_is_equal(const Esvg_Color *c1, const Esvg_Color *c2);

EAPI Eina_Bool esvg_path_string_from(const char *value, Esvg_Command_Cb cb, void *data);
EAPI Eina_Bool esvg_transformation_string_from(Enesim_Matrix *matrix, const char *attr);
EAPI Eina_Bool esvg_length_string_from(Esvg_Length *length, const char *attr_val);
EAPI Eina_Bool esvg_length_is_equal(Esvg_Length *length1, Esvg_Length *length2);
EAPI double esvg_length_final_get(const Esvg_Length *l, double parent_length, double font_size);
EAPI double esvg_length_full_final_get(const Esvg_Length *l, double width, double height, double font_size);

EAPI Eina_Bool esvg_paint_string_from(Esvg_Paint *paint, const char *attr);
EAPI Eina_Bool esvg_paint_is_equal(const Esvg_Paint *p1,
		const Esvg_Paint *p2);
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

EAPI Eina_Bool esvg_type_is_paint_server(Esvg_Type type);
EAPI Eina_Bool esvg_type_is_shape(Esvg_Type type);
EAPI Eina_Bool esvg_type_is_renderable(Esvg_Type type);

/* animation parsing */
EAPI Eina_Bool esvg_animation_event_list_string_from(Eina_List **l, const char *s);
EAPI Eina_Bool esvg_animation_event_string_from(Esvg_Animation_Event *e, const char *s);


#endif
