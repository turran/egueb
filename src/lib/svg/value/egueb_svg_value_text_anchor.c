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
#include "egueb_svg_text_anchor.h"

#include "egueb_svg_text_anchor_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/
EGUEB_DOM_VALUE_ENUM_BOILERPLATE(egueb_svg_text_anchor, Egueb_Svg_Text_Anchor);
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
const Egueb_Dom_Value_Descriptor * egueb_svg_value_text_anchor_descriptor_get(void)
{
	return &_egueb_svg_text_anchor_descriptor;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
