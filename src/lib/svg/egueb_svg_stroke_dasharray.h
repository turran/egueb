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
#ifndef _EGUEB_SVG_STROKE_DASHARRAY_H
#define _EGUEB_SVG_STROKE_DASHARRAY_H

typedef enum _Egueb_Svg_Stroke_Dasharray_Type
{
	EGUEB_SVG_STROKE_DASHARRAY_TYPE_NONE,
	EGUEB_SVG_STROKE_DASHARRAY_TYPE_LIST,
} Egueb_Svg_Stroke_Dasharray_Type;

typedef struct _Egueb_Svg_Stroke_Dasharray
{
	Egueb_Svg_Stroke_Dasharray_Type type;
	Egueb_Dom_List *list;
} Egueb_Svg_Stroke_Dasharray;

EAPI void egueb_svg_stroke_dasharray_reset(Egueb_Svg_Stroke_Dasharray *thiz);

#endif
