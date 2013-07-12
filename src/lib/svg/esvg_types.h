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

#include "egueb_svg_boolean.h"
#include "egueb_svg_color.h"
#include "egueb_svg_length.h"
#include "egueb_svg_matrix.h"
#include "egueb_svg_number.h"
#include "egueb_svg_paint.h"
#include "egueb_svg_rect.h"
#include "egueb_svg_string.h"
#include "egueb_svg_path.h"

typedef enum _Egueb_Svg_Attribute_Type
{
	ESVG_ATTR_CSS,
	ESVG_ATTR_XML,
	ESVG_ATTR_AUTO,
	ESVG_ATTR_TYPES,
} Egueb_Svg_Attribute_Type;

typedef struct _Egueb_Svg_Animated_Enum
{
	int base;
	int anim;
} Egueb_Svg_Animated_Enum;

typedef struct _Egueb_Svg_Animated_List
{
	Eina_List *base;
	Eina_List *anim;
} Egueb_Svg_Animated_List;

/* FIXME when doing clipping or masking, we also use this
 * so better change the name
 */
typedef enum _Egueb_Svg_Element_Gradient_Units
{
	ESVG_USER_SPACE_ON_USE,
	ESVG_OBJECT_BOUNDING_BOX,
	ESVG_ELEMENT_GRADIENT_UNITS,
} Egueb_Svg_Element_Gradient_Units, Egueb_Svg_Pattern_Units;

typedef Egueb_Svg_Element_Gradient_Units Egueb_Svg_Clip_Path_Units;

typedef enum _Egueb_Svg_Paint_Server_Type
{
	ESVG_PAINT_SERVER_LINEAR_GRADIENT,
	ESVG_PAINT_SERVER_RADIAL_GRADIENT,
	ESVG_PAINT_SERVER_PATTERN,
	ESVG_PAINT_SERVER_TYPES
} Egueb_Svg_Paint_Server_Type;

/* attributes */
typedef struct _Egueb_Svg_Attribute_Regular {
} Egueb_Svg_Attribute_Regular;

typedef struct _Egueb_Svg_Timing_Offset
{
	double offset;
} Egueb_Svg_Timing_Offset;

typedef struct _Egueb_Svg_Timing_Event
{
	char *id;
	char *event;
	double offset;
} Egueb_Svg_Timing_Event;

typedef struct _Egueb_Svg_Timing_Repeat
{
	char *id;
	double offset;
} Egueb_Svg_Timing_Repeat;

typedef enum _Egueb_Svg_Timing_Type
{
	ESVG_TIMING_OFFSET,
	ESVG_TIMING_EVENT,
	ESVG_TIMING_REPEAT,
	ESVG_TIMING_TYPES,
} Egueb_Svg_Timing_Type;

typedef struct _Egueb_Svg_Timing
{
	Egueb_Svg_Timing_Type type;
	union {
		Egueb_Svg_Timing_Offset offset;
		Egueb_Svg_Timing_Event event;
		Egueb_Svg_Timing_Repeat repeat;
		/*
		Egueb_Svg_Timing_Access_Key access_key;
		Egueb_Svg_Timing_Wall_Clock wall_clock;
		*/
	} data;
} Egueb_Svg_Timing;

typedef enum _Egueb_Svg_Calc_Mode
{
	ESVG_CALC_MODE_DISCRETE,
	ESVG_CALC_MODE_LINEAR,
	ESVG_CALC_MODE_PACED,
	ESVG_CALC_MODE_SPLINE,
	ESVG_CALC_MODES,
} Egueb_Svg_Calc_Mode;

typedef enum _Egueb_Svg_Animate_Transform_Type
{
	ESVG_ANIMATE_TRANSFORM_TYPE_TRANSLATE,
	ESVG_ANIMATE_TRANSFORM_TYPE_SCALE,
	ESVG_ANIMATE_TRANSFORM_TYPE_ROTATE,
	ESVG_ANIMATE_TRANSFORM_TYPE_SKEWX,
	ESVG_ANIMATE_TRANSFORM_TYPE_SKEWY,
	ESVG_ANIMATE_TRANSFORM_TYPES
} Egueb_Svg_Animate_Transform_Type;

typedef struct _Egueb_Svg_Animate_Key_Spline
{
	double x0;
	double y0;
	double x1;
	double y1;
} Egueb_Svg_Animate_Key_Spline;

typedef void (*Egueb_Svg_Free_Cb)(void *data);
typedef void (*Egueb_Svg_Timing_Cb)(Egueb_Svg_Timing *t, void *data);
typedef void (*Egueb_Svg_Points_Cb)(Egueb_Svg_Point *p, void *data);
typedef void (*Egueb_Svg_List_Cb)(const char *item, void *data);
typedef Eina_Bool (*Egueb_Svg_Number_List_Cb)(double v, void *data);

EAPI const char * egueb_svg_type_string_to(Egueb_Svg_Type type);

EAPI Eina_Bool egueb_svg_string_is_equal(const char *s1, const char *s2);

/* FIXME rename this: remove the _parser_ */
EAPI Eina_Bool egueb_svg_parser_gradient_units_string_from(Egueb_Svg_Element_Gradient_Units *gu, const char *value);
EAPI Eina_Bool egueb_svg_parser_spread_method_get(Egueb_Svg_Spread_Method *smethod, const char *attr);

EAPI Eina_Bool egueb_svg_transformation_string_from(Enesim_Matrix *matrix, const char *attr);

EAPI Eina_Bool egueb_svg_attribute_type_string_from(Egueb_Svg_Attribute_Type *type, const char *value);
EAPI void egueb_svg_timing_string_from(const char *attr, Egueb_Svg_Timing_Cb cb, void *data);
EAPI void egueb_svg_points_string_from(const char *value, Egueb_Svg_Points_Cb cb, void *data);
EAPI Eina_Bool egueb_svg_animate_transform_type_string_from(Egueb_Svg_Animate_Transform_Type *type, const char *s);
EAPI Eina_Bool egueb_svg_animate_key_spline_string_from(Egueb_Svg_Animate_Key_Spline *spline, const char *s);
EAPI Eina_Bool egueb_svg_list_string_from(const char *attr, char sep, Egueb_Svg_List_Cb cb, void *data);
EAPI Eina_Bool egueb_svg_number_list_string_from(const char *attr, Egueb_Svg_Number_List_Cb cb, void *data);
EAPI Eina_Bool egueb_svg_clock_string_from(int64_t *clock, const char *attr);
EAPI Eina_Bool egueb_svg_calc_mode_string_from(Egueb_Svg_Calc_Mode *c, const char *attr);
EAPI Eina_Bool egueb_svg_repeat_count_string_from(int *rc, const char *attr);
EAPI Eina_Bool egueb_svg_type_is_animation(Egueb_Svg_Type type);
EAPI Eina_Bool egueb_svg_type_is_paint_server(Egueb_Svg_Type type);
EAPI Eina_Bool egueb_svg_type_is_shape(Egueb_Svg_Type type);
EAPI Eina_Bool egueb_svg_type_is_renderable(Egueb_Svg_Type type);

/* animation parsing */
EAPI Eina_Bool egueb_svg_animation_event_list_string_from(Eina_List **l, const char *s);

EAPI void egueb_svg_animation_event_offset_set(Egueb_Svg_Animation_Event *a, int64_t offset);
EAPI Eina_Bool egueb_svg_fill_string_from(Egueb_Svg_Fill *fill, const char *s);
#endif
