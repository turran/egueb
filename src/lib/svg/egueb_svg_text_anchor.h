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
#ifndef _EGUEB_SVG_TEXT_ANCHOR_H
#define _EGUEB_SVG_TEXT_ANCHOR_H

typedef enum _Egueb_Svg_Text_Anchor
{
	EGUEB_SVG_TEXT_ANCHOR_START,
	EGUEB_SVG_TEXT_ANCHOR_MIDDLE,
	EGUEB_SVG_TEXT_ANCHOR_END,
} Egueb_Svg_Text_Anchor;

EAPI const Egueb_Dom_Value_Descriptor * egueb_svg_text_anchor_descriptor_get(void);
EAPI Eina_Bool egueb_svg_text_anchor_string_from(Egueb_Svg_Text_Anchor *thiz, const char *value);
EAPI char * egueb_svg_text_anchor_string_to(Egueb_Svg_Text_Anchor thiz);

#endif

