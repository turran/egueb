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

#ifndef _EGUEB_SVG_PAINTER_H_
#define _EGUEB_SVG_PAINTER_H_

typedef struct _Egueb_Svg_Painter Egueb_Svg_Painter;

EAPI void egueb_svg_painter_visibility_get(const Egueb_Svg_Painter *thiz,
		Eina_Bool *visibility);
EAPI void egueb_svg_painter_color_get(const Egueb_Svg_Painter *thiz,
		Enesim_Color *color);
EAPI void egueb_svg_painter_draw_mode_get(const Egueb_Svg_Painter *thiz,
		Enesim_Renderer_Shape_Draw_Mode *draw_mode);
EAPI void egueb_svg_painter_stroke_dasharray_get(const Egueb_Svg_Painter *thiz,
		Eina_List **l);
EAPI void egueb_svg_painter_stroke_cap_get(const Egueb_Svg_Painter *thiz,
		Enesim_Renderer_Shape_Stroke_Cap *stroke_cap);
EAPI void egueb_svg_painter_stroke_join_get(const Egueb_Svg_Painter *thiz,
		Enesim_Renderer_Shape_Stroke_Join *stroke_join);
EAPI void egueb_svg_painter_stroke_color_get(const Egueb_Svg_Painter *thiz,
		Enesim_Color *stroke_color);
EAPI void egueb_svg_painter_stroke_weight_get(const Egueb_Svg_Painter *thiz,
		double *stroke_weight);
EAPI void egueb_svg_painter_stroke_renderer_get(const Egueb_Svg_Painter *thiz,
		Enesim_Renderer **stroke_renderer);
EAPI void egueb_svg_painter_fill_color_get(const Egueb_Svg_Painter *thiz,
		Enesim_Color *fill_color);
EAPI void egueb_svg_painter_fill_renderer_get(const Egueb_Svg_Painter *thiz,
		Enesim_Renderer **fill_renderer);
EAPI void egueb_svg_painter_fill_rule_get(const Egueb_Svg_Painter *thiz,
		Enesim_Renderer_Shape_Fill_Rule *fill_rule);

#endif
