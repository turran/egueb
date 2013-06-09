/* Egueb
 * Copyright (C) 2011 - 2013 Jorge Luis Zapata
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
#include "egueb_svg_main.h"
#include "egueb_svg_document.h"

#if 0
#include "egueb_svg_types.h"

/* main abstractions */
#include "egueb_svg_element.h"
#include "egueb_svg_renderable.h"

/* providers */
#include "egueb_svg_video_provider.h"
#include "egueb_svg_script_provider.h"

/* shapes */
#include "egueb_svg_element_path.h"
#include "egueb_svg_element_polygon.h"
#include "egueb_svg_element_polyline.h"
#include "egueb_svg_element_line.h"
#include "egueb_svg_element_circle.h"
#include "egueb_svg_element_ellipse.h"
#include "egueb_svg_element_rect.h"
#include "egueb_svg_element_image.h"
#include "egueb_svg_element_text.h"
#include "egueb_svg_element_use.h"
#include "egueb_svg_element_video.h"
/* containers */
#include "egueb_svg_element_svg.h"
#include "egueb_svg_element_g.h"

/* other */
#include "egueb_svg_element_a.h"
#include "egueb_svg_element_defs.h"
#include "egueb_svg_element_style.h"
#include "egueb_svg_element_clip_path.h"
#include "egueb_svg_element_script.h"
#include "egueb_svg_event.h"

/* paint servers */
#include "egueb_svg_element_gradient.h"
#include "egueb_svg_element_stop.h"
#include "egueb_svg_element_linear_gradient.h"
#include "egueb_svg_element_radial_gradient.h"

/* animation */
#include "egueb_svg_element_animation.h"
#include "egueb_svg_animate_base.h"
#include "egueb_svg_element_animate.h"
#include "egueb_svg_element_set.h"

/* parsing */
#include "egueb_svg_parser.h"

EAPI Eina_Bool egueb_svg_is_shape(Egueb_Dom_Tag *e);

EAPI Egueb_Dom_Tag * egueb_svg_pattern_new(void);
EAPI Eina_Bool egueb_svg_is_pattern(Egueb_Dom_Tag *e);
EAPI void egueb_svg_pattern_x_set(Egueb_Dom_Tag *e, const Egueb_Svg_Coord *x);
EAPI void egueb_svg_pattern_x_get(Egueb_Dom_Tag *e, Egueb_Svg_Coord *x);
EAPI void egueb_svg_pattern_y_set(Egueb_Dom_Tag *e, const Egueb_Svg_Coord *y);
EAPI void egueb_svg_pattern_y_get(Egueb_Dom_Tag *e, Egueb_Svg_Coord *y);
EAPI void egueb_svg_pattern_width_set(Egueb_Dom_Tag *e, const Egueb_Svg_Length *width);
EAPI void egueb_svg_pattern_width_get(Egueb_Dom_Tag *e, Egueb_Svg_Length *width);
EAPI void egueb_svg_pattern_height_set(Egueb_Dom_Tag *e, const Egueb_Svg_Length *height);
EAPI void egueb_svg_pattern_height_get(Egueb_Dom_Tag *e, Egueb_Svg_Length *height);
EAPI void egueb_svg_pattern_units_set(Egueb_Dom_Tag *e, Egueb_Svg_Pattern_Units units);
EAPI void egueb_svg_pattern_units_get(Egueb_Dom_Tag *e, Egueb_Svg_Pattern_Units *units);
EAPI Eina_Bool egueb_svg_pattern_units_is_set(Egueb_Dom_Tag *e);
EAPI void egueb_svg_pattern_transform_set(Egueb_Dom_Tag *e, const Enesim_Matrix *transform);
EAPI void egueb_svg_pattern_transform_get(Egueb_Dom_Tag *e, Enesim_Matrix *transform);
EAPI Eina_Bool egueb_svg_pattern_transform_is_set(Egueb_Dom_Tag *e);
EAPI void egueb_svg_pattern_content_set(Egueb_Dom_Tag *e, Egueb_Dom_Tag *content);
#endif

#endif /*_ESVG_H*/
