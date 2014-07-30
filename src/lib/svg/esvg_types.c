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
#include "egueb_svg_main_private.h"
#include "egueb_svg_private_attribute_presentation.h"
#include "egueb_svg_types_private.h"

#include "egueb_svg_types.h"

#include <stdlib.h>
#include <errno.h>
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                               Generic helpers                              *
 *----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*
 *                         Timing related functions                           *
 *----------------------------------------------------------------------------*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool egueb_svg_attribute_type_string_from(Egueb_Svg_Attribute_Type *type, const char *value)
{
	Eina_Bool ret = EINA_TRUE;

	if (!strcmp(value, "CSS"))
		*type = ESVG_ATTR_CSS;
	else if (!strcmp(value, "XML"))
		*type = ESVG_ATTR_XML;
	else if (!strcmp(value, "auto"))
		*type = ESVG_ATTR_AUTO;
	else
		ret = EINA_FALSE;
	return ret;
}

/* x1,y1 x2,y2 ... */
EAPI void egueb_svg_points_string_from(const char *value, Egueb_Svg_Points_Cb cb, void *data)
{
	const char *tmp;
	char *endptr;

	EGUEB_DOM_SPACE_SKIP(value);
	tmp = value;
	while (*tmp)
	{
		Egueb_Svg_Point p;

		p.x = egueb_dom_double_get(tmp, &endptr);
		tmp = endptr;
		EGUEB_DOM_SPACE_COMMA_SKIP(tmp);
		p.y = egueb_dom_double_get(tmp, &endptr);
		tmp = endptr;
		EGUEB_DOM_SPACE_COMMA_SKIP(tmp);

		cb(&p, data);
	}
}


EAPI void egueb_svg_timing_string_from(const char *attr, Egueb_Svg_Timing_Cb cb, void *data)
{

}


