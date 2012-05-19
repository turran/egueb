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
