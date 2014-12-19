/* Esvg - SVG
 * Copyright (C) 2012 Jorge Luis Zapata
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
#include "egueb_svg_main_private.h"
#include "egueb_svg_animate_transform_type.h"

#include "egueb_svg_animate_transform_type_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_svg_animate_transform_type_string_from(
		Egueb_Svg_Animate_Transform_Type *thiz, const char *value)
{
	Eina_Bool ret = EINA_TRUE;

	if (!strcmp(value, "translate"))
		*thiz = EGUEB_SVG_ANIMATE_TRANSFORM_TYPE_TRANSLATE;
	else if (!strcmp(value, "scale"))
		*thiz = EGUEB_SVG_ANIMATE_TRANSFORM_TYPE_SCALE;
	else if (!strcmp(value, "rotate"))
		*thiz = EGUEB_SVG_ANIMATE_TRANSFORM_TYPE_ROTATE;
	else if (!strcmp(value, "skewX"))
		*thiz = EGUEB_SVG_ANIMATE_TRANSFORM_TYPE_SKEW_X;
	else if (!strcmp(value, "skewY"))
		*thiz = EGUEB_SVG_ANIMATE_TRANSFORM_TYPE_SKEW_Y;
	else
		ret = EINA_FALSE;

	return ret;
}

char * egueb_svg_animate_transform_type_string_to(Egueb_Svg_Animate_Transform_Type thiz)
{
	switch (thiz)
	{
		case EGUEB_SVG_ANIMATE_TRANSFORM_TYPE_TRANSLATE:
		return strdup("translate");

		case EGUEB_SVG_ANIMATE_TRANSFORM_TYPE_SCALE:
		return strdup("scale");

		case EGUEB_SVG_ANIMATE_TRANSFORM_TYPE_ROTATE:
		return strdup("rotate");

		case EGUEB_SVG_ANIMATE_TRANSFORM_TYPE_SKEW_X:
		return strdup("skewX");

		case EGUEB_SVG_ANIMATE_TRANSFORM_TYPE_SKEW_Y:
		return strdup("skewY");

		default:
		return NULL;
	}
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
