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

#include "esvg_main.h"
#include "esvg_types.h"
#include "esvg_element.h"
#include "esvg_renderable.h"
#include "esvg_ellipse.h"
#include "esvg_svg.h"
#include "esvg_rect.h"
#include "esvg_path.h"
#include "esvg_polygon.h"
#include "esvg_polyline.h"
#include "esvg_line.h"
#include "esvg_circle.h"
#include "esvg_g.h"

EAPI Eina_Bool esvg_is_shape(Edom_Tag *e);

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
