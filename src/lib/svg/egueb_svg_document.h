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

EAPI Egueb_Dom_Node * egueb_svg_document_new(void *app);
EAPI Eina_Error egueb_svg_document_title_get(Egueb_Dom_Node *n,
		Egueb_Dom_String **title);
EAPI Eina_Error egueb_svg_document_referrer_get(Egueb_Dom_Node *n,
		Egueb_Dom_String **referrer);
EAPI Eina_Error egueb_svg_document_domain_get(Egueb_Dom_Node *n,
		Egueb_Dom_String **domain);
EAPI Eina_Error egueb_svg_document_url_get(Egueb_Dom_Node *n,
		Egueb_Dom_String **url);
EAPI Eina_Error egueb_svg_document_element_root_get(Egueb_Dom_Node *n,
		Egueb_Dom_Node **root);

EAPI Eina_Error egueb_svg_document_width_set(Egueb_Dom_Node *n,
		double width);
EAPI Eina_Error egueb_svg_document_width_get(Egueb_Dom_Node *n,
		double *width);
EAPI Eina_Error egueb_svg_document_height_set(Egueb_Dom_Node *n,
		double height);
EAPI Eina_Error egueb_svg_document_height_get(Egueb_Dom_Node *n,
		double *height);
EAPI Eina_Error egueb_svg_document_font_size_set(Egueb_Dom_Node *n,
		double font_size);
EAPI Eina_Error egueb_svg_document_font_size_get(Egueb_Dom_Node *n,
		double *font_size);

EAPI Eina_Error egueb_svg_document_element_get_by_iri(Egueb_Dom_Node *n,
		Egueb_Dom_String *iri, Egueb_Dom_Node **ref);
/* FIXME remove this as move it to the use element */
EAPI Eina_Error egueb_svg_document_iri_clone(Egueb_Dom_Node *n,
		Egueb_Dom_String *iri, Egueb_Dom_Node **cloned);

EAPI void egueb_svg_document_feed_mouse_move(Egueb_Dom_Node *n, int x, int y);
EAPI void egueb_svg_document_feed_mouse_down(Egueb_Dom_Node *n, int button);
EAPI void egueb_svg_document_feed_mouse_up(Egueb_Dom_Node *n, int button);


#endif

