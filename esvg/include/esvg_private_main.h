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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <math.h>

#include <Eina.h>
#include <Ecss.h>
#include <Enesim.h>
#include <Etex.h>
#include <Emage.h>
#include <Ender.h>
#include <Edom.h>
#include <Etch.h>

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

#define ESVG_LOG_COLOR_DEFAULT EINA_COLOR_ORANGE
/* Whenever a file needs to generate a log, it must declare this first */

/* The log domaings */
extern int esvg_log_a;
extern int esvg_log_animate;
extern int esvg_log_animate_base;
extern int esvg_log_animate_transform;
extern int esvg_log_animation;
extern int esvg_log_clone;
extern int esvg_log_circle;
extern int esvg_log_clip_path;
extern int esvg_log_element;
extern int esvg_log_ellipse;
extern int esvg_log_gradient;
extern int esvg_log_image;
extern int esvg_log_line;
extern int esvg_log_linear_gradient;
extern int esvg_log_parser;
extern int esvg_log_path;
extern int esvg_log_polygon;
extern int esvg_log_polyline;
extern int esvg_log_radial_gradient;
extern int esvg_log_rect;
extern int esvg_log_renderable;
extern int esvg_log_set;
extern int esvg_log_stop;
extern int esvg_log_svg;
extern int esvg_log_text;
extern int esvg_log_type;
extern int esvg_log_use;
extern int esvg_log_video;

#ifdef ERR
# undef ERR
#endif
#define ERR(...) EINA_LOG_DOM_ERR(ESVG_LOG_DEFAULT, __VA_ARGS__)

#ifdef WARN
# undef WARN
#endif
#define WARN(...) EINA_LOG_DOM_WARN(ESVG_LOG_DEFAULT, __VA_ARGS__)

#ifdef INFO
# undef INFO
#endif
#define INFO(...) EINA_LOG_DOM_INFO(ESVG_LOG_DEFAULT, __VA_ARGS__)

#ifdef DBG
# undef DBG
#endif
#define DBG(...) EINA_LOG_DOM_DBG(ESVG_LOG_DEFAULT, __VA_ARGS__)

/* all the possible magic numbers */
#define ESVG_ELEMENT_MAGIC 0xe5500001
/* basic abstractions */
#define ESVG_RENDERABLE_MAGIC 0xe5500002
#define ESVG_REFERENCEABLE_MAGIC 0xe5500003
#define ESVG_INSTANTIABLE_MAGIC 0xe5500004
#define ESVG_ANIMATION_MAGIC 0xe5500005
#define ESVG_PAINT_SERVER_MAGIC 0xe5500006

#define ESVG_GRADIENT_MAGIC 0xe5500010
#define ESVG_PATTERN_MAGIC 0xe5500012

#define ESVG_CLIP_PATH_MAGIC 0xe5500013
#define ESVG_IMAGE_MAGIC 0xe5500014

#define ESVG_ANIMATE_BASE_MAGIC 0xe5500015

#define ESVG_ELEMENT_NEW(name) ender_element_new_namespace_from(name, esvg_namespace_get());

extern int esvg_log_dom_global;
Ender_Namespace * esvg_namespace_get(void);

void esvg_element_init(void);
void esvg_renderable_init(void);
void esvg_svg_init(void);
void esvg_rect_init(void);

#if 0

typedef struct _Esvg_Shape_Descriptor {
	/* the shape interface */
	Esvg_Shape_Setup setup;
	Esvg_Shape_Renderer_Get renderer_get;
	/* the element interface */
	Enesim_Renderer_Name name_get;
	Esvg_Element_Clone clone;
} Esvg_Shape_Descriptor;

void * esvg_shape_data_get(Enesim_Renderer *r);
Enesim_Renderer * esvg_shape_new(Esvg_Shape_Descriptor *descriptor, void *data);

/* clip path */
void esvg_clip_path_relative_set(Edom_Tag *e, Enesim_Renderer *rel, Enesim_Matrix *rel_m);

/* svg */

/* circle */

/* ellipse */

/* rect */

/* g */

/* paint server */
typedef Eina_Bool (*Esvg_Paint_Server_Setup)(Enesim_Renderer *r, const Esvg_Element_Context *state, Enesim_Renderer *rel);

typedef struct _Evg_Paint_Server_Descriptor
{
	/* the paint server interface */
	Esvg_Paint_Server_Setup setup;
	/* the element interface */
	Enesim_Renderer_Name name_get;
	Esvg_Renderable_Renderer_Get renderer_get;
	Esvg_Element_Clone clone;
} Esvg_Paint_Server_Descriptor;

void esvg_paint_server_renderer_setup(Enesim_Renderer *r, const Esvg_Element_Context *state, Enesim_Renderer *rel);
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

typedef Eina_Bool (*Esvg_Gradient_Setup)(Enesim_Renderer *r, const Esvg_Element_Context *state, Enesim_Renderer *rel, const Esvg_Gradient_State *gstate);

typedef struct _Evg_Gradient_Descriptor
{
	/* the gradient interface */
	Esvg_Gradient_Setup setup;
	/* the paint server interface */
	Enesim_Renderer_Name name_get;
	Esvg_Renderable_Renderer_Get renderer_get;
	Esvg_Element_Clone clone;
} Esvg_Gradient_Descriptor;

Enesim_Renderer * esvg_gradient_new(Esvg_Gradient_Descriptor *descriptor,
		void *data);
void * esvg_gradient_data_get(Enesim_Renderer *r);

/* length */

Eina_Bool esvg_length_is_equal(Esvg_Length *length1, Esvg_Length *length2);
double esvg_length_final_get(const Esvg_Length *l, double parent_length);

#endif
#endif
