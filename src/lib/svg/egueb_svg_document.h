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

#ifndef _EGUEB_SVG_DOCUMENT_H_
#define _EGUEB_SVG_DOCUMENT_H_

/**
 * @file
 * @ender_group{Egueb_Svg_Document}
 */

/**
 * @defgroup Egueb_Svg_Document Document
 * @brief Document @ender_inherits{Egueb_Dom_Document}
 * @ingroup Egueb_Svg_Group
 * @{
 */

EAPI Eina_Bool egueb_svg_is_document(Egueb_Dom_Node *n);
EAPI Egueb_Dom_Node * egueb_svg_document_new(void);
EAPI Egueb_Dom_String * egueb_svg_document_title_get(Egueb_Dom_Node *n);
EAPI Egueb_Dom_String * egueb_svg_document_referrer_get(Egueb_Dom_Node *n);
EAPI Egueb_Dom_String * egueb_svg_document_domain_get(Egueb_Dom_Node *n);

EAPI Egueb_Dom_Node * egueb_svg_document_root_element_get(Egueb_Dom_Node *n);

EAPI void egueb_svg_document_font_size_set(Egueb_Dom_Node *n,
		double font_size);
EAPI double egueb_svg_document_font_size_get(Egueb_Dom_Node *n);

/**
 * @}
 */

#endif

