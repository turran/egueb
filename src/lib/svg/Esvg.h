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
#include <Egueb_Dom.h>
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
#  endif
# else
#  define EAPI __declspec(dllimport)
# endif
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

/* basic data types */
#include "esvg_main.h"
#include "esvg_types.h"

/* main abstractions */
#include "esvg_element.h"
#include "esvg_renderable.h"

/* providers */
#include "esvg_video_provider.h"
#include "esvg_script_provider.h"

/* shapes */
#include "esvg_element_path.h"
#include "esvg_element_polygon.h"
#include "esvg_element_polyline.h"
#include "esvg_element_line.h"
#include "esvg_element_circle.h"
#include "esvg_element_ellipse.h"
#include "esvg_element_rect.h"
#include "esvg_element_image.h"
#include "esvg_element_text.h"
#include "esvg_element_use.h"
#include "esvg_element_video.h"
/* containers */
#include "esvg_element_svg.h"
#include "esvg_element_g.h"

/* other */
#include "esvg_element_a.h"
#include "esvg_element_defs.h"
#include "esvg_element_style.h"
#include "esvg_element_clip_path.h"
#include "esvg_element_script.h"
#include "esvg_event.h"

/* paint servers */
#include "esvg_element_gradient.h"
#include "esvg_element_stop.h"
#include "esvg_element_linear_gradient.h"
#include "esvg_element_radial_gradient.h"

/* animation */
#include "esvg_element_animation.h"
#include "esvg_animate_base.h"
#include "esvg_element_animate.h"
#include "esvg_element_set.h"

/* parsing */
#include "esvg_parser.h"

EAPI Eina_Bool esvg_is_shape(Edom_Tag *e);

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

#endif /*_ESVG_H*/
