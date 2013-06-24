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

#ifndef _EGUEB_SVG_CLIP_PATH_H
#define _EGUEB_SVG_CLIP_PATH_H

typedef enum _Egueb_Svg_Clip_Path_Type
{
	EGUEB_SVG_CLIP_PATH_TYPE_NONE,
	EGUEB_SVG_CLIP_PATH_TYPE_IRI,
	EGUEB_SVG_CLIP_PATH_TYPES,
} Egueb_Svg_Clip_Path_Type;

typedef union _Egueb_Svg_Clip_Path_Value
{
	char *iri;
} Egueb_Svg_Clip_Path_Value;

typedef struct _Egueb_Svg_Clip_Path
{
	Egueb_Svg_Clip_Path_Type type;
	Egueb_Svg_Clip_Path_Value value;
} Egueb_Svg_Clip_Path;

typedef struct _Egueb_Svg_Clip_Path_Animated
{
	Egueb_Svg_Clip_Path base;
	Egueb_Svg_Clip_Path anim;
} Egueb_Svg_Clip_Path_Animated;

#define EGUEB_SVG_CLIP_PATH_INIT { EGUEB_SVG_CLIP_PATH_TYPE_NONE, { NULL }}

EAPI extern const Egueb_Svg_Clip_Path EGUEB_SVG_CLIP_PATH_NONE;

EAPI const Egueb_Dom_Value_Descriptor * egueb_svg_clip_path_descriptor_get(void);
EAPI Eina_Bool egueb_svg_clip_path_is_equal(const Egueb_Svg_Clip_Path *p1,
		const Egueb_Svg_Clip_Path *p2);
EAPI Eina_Bool egueb_svg_clip_path_string_from(Egueb_Svg_Clip_Path *cp, const char *attr);
EAPI char * egueb_svg_clip_path_string_to(const Egueb_Svg_Clip_Path *thiz);
EAPI void egueb_svg_clip_path_copy(const Egueb_Svg_Clip_Path *thiz, Egueb_Svg_Clip_Path *copy);
EAPI void egueb_svg_clip_path_reset(Egueb_Svg_Clip_Path *thiz);

#endif
