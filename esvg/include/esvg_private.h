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
#ifndef _ESVG_PRIVATE_H
#define _ESVG_PRIVATE_H

#include <math.h>

#include <Ecss.h>
#include <Enesim.h>
#include <Etex.h>
#include <Emage.h>

#define ESVG_LOG_COLOR_DEFAULT EINA_COLOR_ORANGE

#ifdef ERR
# undef ERR
#endif
#define ERR(...) EINA_LOG_DOM_ERR(esvg_log_dom_global, __VA_ARGS__)

#ifdef DBG
# undef DBG
#endif
#define DBG(...) EINA_LOG_DOM_DBG(esvg_log_dom_global, __VA_ARGS__)

/* all the possible magic numbers */
#define ESVG_ELEMENT_MAGIC 0xe5500001
#define ESVG_CONTAINER_MAGIC 0xe5500002
#define ESVG_SHAPE_MAGIC 0xe5500003
#define ESVG_SVG_MAGIC 0xe5500004
#define ESVG_G_MAGIC 0xe5500005
#define ESVG_CIRCLE_MAGIC 0xe5500006
#define ESVG_ELLIPSE_MAGIC 0xe5500007
#define ESVG_LINE_MAGIC 0xe5500008
#define ESVG_RECT_MAGIC 0xe5500009
#define ESVG_POLYLINE_MAGIC 0xe550000a
#define ESVG_POLYGON_MAGIC 0xe550000b
#define ESVG_PATH_MAGIC 0xe550000c
#define ESVG_USE_MAGIC 0xe550000d

#define ESVG_PAINT_SERVER_MAGIC 0xe550000e
#define ESVG_GRADIENT_MAGIC 0xe550000f
#define ESVG_LINEAR_GRADIENT_MAGIC 0xe5500010
#define ESVG_RADIAL_GRADIENT_MAGIC 0xe5500011
#define ESVG_PATTERN_MAGIC 0xe5500012

#define ESVG_CLIP_PATH_MAGIC 0xe5500013
#define ESVG_IMAGE_MAGIC 0xe5500014

extern int esvg_log_dom_global;

void esvg_attribute_presentation_setup(Esvg_Attribute_Presentation *thiz);
void esvg_attribute_presentation_clip_path_set(Esvg_Attribute_Presentation *thiz, const Enesim_Renderer *clip_path);
void esvg_attribute_presentation_clip_path_unset(Esvg_Attribute_Presentation *thiz);
void esvg_attribute_presentation_color_set(Esvg_Attribute_Presentation *thiz, const Esvg_Color *color);
void esvg_attribute_presentation_color_unset(Esvg_Attribute_Presentation *thiz);
void esvg_attribute_presentation_opacity_set(Esvg_Attribute_Presentation *thiz, double opacity);
void esvg_attribute_presentation_fill_set(Esvg_Attribute_Presentation *thiz, const Esvg_Paint *fill);
void esvg_attribute_presentation_fill_unset(Esvg_Attribute_Presentation *thiz);
void esvg_attribute_presentation_fill_opacity_set(Esvg_Attribute_Presentation *thiz, double fill_opacity);
void esvg_attribute_presentation_fill_opacity_unset(Esvg_Attribute_Presentation *thiz);
void esvg_attribute_presentation_fill_rule_set(Esvg_Attribute_Presentation *thiz, Esvg_Fill_Rule rule);
void esvg_attribute_presentation_fill_rule_unset(Esvg_Attribute_Presentation *thiz);
void esvg_attribute_presentation_stroke_set(Esvg_Attribute_Presentation *thiz, const Esvg_Paint *stroke);
void esvg_attribute_presentation_stroke_opacity_unset(Esvg_Attribute_Presentation *thiz);
void esvg_attribute_presentation_stroke_width_set(Esvg_Attribute_Presentation *thiz, const Esvg_Length *stroke_width);
void esvg_attribute_presentation_stroke_opacity_set(Esvg_Attribute_Presentation *thiz, double stroke_opacity);
void esvg_attribute_presentation_stroke_line_cap_set(Esvg_Attribute_Presentation *thiz, Esvg_Stroke_Line_Cap cap);
void esvg_attribute_presentation_stroke_line_join_set(Esvg_Attribute_Presentation *thiz, Esvg_Stroke_Line_Join join);
void esvg_attribute_presentation_stroke_line_join_unset(Esvg_Attribute_Presentation *thiz);
void esvg_attribute_presentation_stroke_line_cap_unset(Esvg_Attribute_Presentation *thiz);
void esvg_attribute_presentation_visibility_set(Esvg_Attribute_Presentation *thiz, Eina_Bool visibility);
void esvg_attribute_presentation_visibility_unset(Esvg_Attribute_Presentation *thiz);

void esvg_attribute_presentation_stop_color_set(Esvg_Attribute_Presentation *thiz, const Esvg_Color *stop_color);
void esvg_attribute_presentation_stop_color_unset(Esvg_Attribute_Presentation *thiz);

void esvg_attribute_presentation_stop_opacity_set(Esvg_Attribute_Presentation *thiz, double stop_opacity);
void esvg_attribute_presentation_stop_opacity_unset(Esvg_Attribute_Presentation *thiz, double stop_opacity);

void esvg_attribute_presentation_dump(Esvg_Attribute_Presentation *thiz);

/* element */
typedef struct _Esvg_Element_State {
	double dpi_x;
	double dpi_y;
	double viewbox_w; /* FIXME remove this */
	double viewbox_h; /* FIXME remove this */
	Enesim_Rectangle viewbox;
	Enesim_Matrix transform;
	char *style;
	char *id;
} Esvg_Element_State;

typedef Enesim_Renderer * (*Esvg_Element_Renderer_Get)(Enesim_Renderer *, const Esvg_Element_State *state, const Esvg_Attribute_Presentation *attr);
typedef Eina_Bool (*Esvg_Element_Setup)(Enesim_Renderer *r,
		Esvg_Element_State *state,
		Esvg_Attribute_Presentation *attr,
		Enesim_Surface *s,
		Enesim_Error **error);

typedef void (*Esvg_Element_Cleanup)(Enesim_Renderer *r);
typedef void (*Esvg_Element_Clone)(Enesim_Renderer *r, Enesim_Renderer *dst);

typedef struct _Esvg_Element_Descriptor {
	/* the element interface */
	Enesim_Renderer_Name name_get;
	Esvg_Element_Renderer_Get renderer_get;
	Esvg_Element_Clone clone;
	Esvg_Element_Setup setup;
	Esvg_Element_Cleanup cleanup;
	Eina_Bool is_renderable;
} Esvg_Element_Descriptor;

void * esvg_element_data_get(Enesim_Renderer *r);
Enesim_Renderer * esvg_element_new(Esvg_Element_Descriptor *descriptor, void *data);
Eina_Bool esvg_element_setup(Enesim_Renderer *r, const Esvg_Element_State *state, const Esvg_Attribute_Presentation *attr, Enesim_Surface *s, Enesim_Error **error);
void esvg_element_parent_set(Enesim_Renderer *r, Enesim_Renderer *parent);
Enesim_Renderer * esvg_element_parent_get(Enesim_Renderer *r);
Enesim_Renderer * esvg_element_renderer_get(Enesim_Renderer *r);
void esvg_element_state_compose(Enesim_Renderer *r, const Esvg_Element_State *s, Esvg_Element_State *d);

/* shape */
typedef struct _Esvg_Shape_Enesim_State {
	/* common */
	Enesim_Color color;
	/* stroke */
	Enesim_Shape_Draw_Mode draw_mode;
	Enesim_Shape_Stroke_Cap stroke_cap;
	Enesim_Shape_Stroke_Join stroke_join;
	Enesim_Color stroke_color;
	Enesim_Renderer *stroke_renderer;
	double stroke_weight;
	/* fill */
	Enesim_Renderer *fill_renderer;
	Enesim_Color fill_color;
	Enesim_Shape_Fill_Rule fill_rule;
} Esvg_Shape_Enesim_State;

typedef Eina_Bool (*Esvg_Shape_Enesim_State_Calculate)(Enesim_Renderer *r,
		Esvg_Element_State *estate,
		const Esvg_Attribute_Presentation *attr,
		Esvg_Shape_Enesim_State *dstate,
		void *data);

typedef Enesim_Renderer * (*Esvg_Shape_Renderer_Get)(Enesim_Renderer *);
typedef Eina_Bool (*Esvg_Shape_Setup)(Enesim_Renderer *r, const Esvg_Element_State *estate,
		const Esvg_Shape_Enesim_State *dstate);

typedef struct _Esvg_Shape_Descriptor {
	/* the shape interface */
	Esvg_Shape_Setup setup;
	Esvg_Shape_Renderer_Get renderer_get;
	/* the element interface */
	Enesim_Renderer_Name name_get;
	Esvg_Element_Clone clone;
	Esvg_Element_Cleanup cleanup;
} Esvg_Shape_Descriptor;

void * esvg_shape_data_get(Enesim_Renderer *r);
Enesim_Renderer * esvg_shape_new(Esvg_Shape_Descriptor *descriptor, void *data);

/* container */
typedef Eina_Bool (*Esvg_Container_Element_Add)(Enesim_Renderer *r, Enesim_Renderer *child);
typedef void (*Esvg_Container_Element_Remove)(Enesim_Renderer *r, Enesim_Renderer *child);
typedef Enesim_Renderer * (*Esvg_Container_Element_At)(Enesim_Renderer *r, double x, double y);
typedef struct _Esvg_Container_Descriptor {
	/* the container interface */
	Esvg_Container_Element_Add element_add;
	Esvg_Container_Element_Remove element_remove;
	Esvg_Container_Element_At element_at;
	/* the element interface */
	Enesim_Renderer_Name name_get;
	Esvg_Element_Renderer_Get renderer_get;
	Esvg_Element_Clone clone;
	Esvg_Element_Setup setup;
	Esvg_Element_Cleanup cleanup;
	Eina_Bool is_renderable;
} Esvg_Container_Descriptor;

void * esvg_container_data_get(Enesim_Renderer *r);
Enesim_Renderer * esvg_container_new(Esvg_Container_Descriptor *descriptor, void *data);

/* clip path */
void esvg_clip_path_relative_set(Enesim_Renderer *r, Enesim_Renderer *rel, Enesim_Matrix *rel_m);

/* svg */

/* circle */

/* ellipse */

/* rect */

/* g */

/* paint server */
typedef Eina_Bool (*Esvg_Paint_Server_Setup)(Enesim_Renderer *r, const Esvg_Element_State *state, Enesim_Renderer *rel);

typedef struct _Evg_Paint_Server_Descriptor
{
	/* the paint server interface */
	Esvg_Paint_Server_Setup setup;
	/* the element interface */
	Enesim_Renderer_Name name_get;
	Esvg_Element_Renderer_Get renderer_get;
	Esvg_Element_Clone clone;
} Esvg_Paint_Server_Descriptor;

void esvg_paint_server_renderer_setup(Enesim_Renderer *r, const Esvg_Element_State *state, Enesim_Renderer *rel);
Enesim_Renderer * esvg_paint_server_new(Esvg_Paint_Server_Descriptor *descriptor,
		void *data);
void * esvg_paint_server_data_get(Enesim_Renderer *r);

/* gradient */
typedef struct _Esvg_Gradient_State
{
	Esvg_Gradient_Units units;
	Esvg_Spread_Method spread_method;
	Enesim_Matrix transform;
	Eina_List *stops;
} Esvg_Gradient_State;

typedef Eina_Bool (*Esvg_Gradient_Setup)(Enesim_Renderer *r, const Esvg_Element_State *state, Enesim_Renderer *rel, const Esvg_Gradient_State *gstate);

typedef struct _Evg_Gradient_Descriptor
{
	/* the gradient interface */
	Esvg_Gradient_Setup setup;
	/* the paint server interface */
	Enesim_Renderer_Name name_get;
	Esvg_Element_Renderer_Get renderer_get;
	Esvg_Element_Clone clone;
} Esvg_Gradient_Descriptor;

Enesim_Renderer * esvg_gradient_new(Esvg_Gradient_Descriptor *descriptor,
		void *data);
void * esvg_gradient_data_get(Enesim_Renderer *r);

/* parse */
/* units */

/* number */

double esvg_number_get(const char *attr_val, double default_nbr);

/* length */

Eina_Bool esvg_length_get(Esvg_Length *l, const char *attr_val, Esvg_Length default_length);
double esvg_length_final_get(const Esvg_Length *l, double parent_length);

Esvg_View_Box esvg_view_box_get(const char *attr_val);

Eina_Bool esvg_color_get(Esvg_Color *color, const char *attr_val);

Eina_Bool esvg_transformation_get(Enesim_Matrix *matrix, const char *attr);

#endif
