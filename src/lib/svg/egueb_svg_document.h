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
 * @defgroup Egueb_Svg_Document_Group Document
 * @brief Document
 * @ingroup Egueb_Svg_Group
 * @{
 */

typedef void (*Egueb_Svg_Document_Failed_Cb)(void *user_data);
typedef void (*Egueb_Svg_Document_Image_Loaded)(Enesim_Surface *s, void *user_data);

typedef struct _Egueb_Svg_Document_Image_Load_Descriptor
{
	Egueb_Svg_Document_Image_Loaded loaded;
	Egueb_Svg_Document_Failed_Cb failed;
} Egueb_Svg_Document_Image_Load_Descriptor;

typedef void (*Egueb_Svg_Document_Uri_Fetched_Cb)(Enesim_Stream *data, void *user_data);

typedef struct _Egueb_Svg_Document_Uri_Fetch_Descriptor
{
	Egueb_Svg_Document_Uri_Fetched_Cb fetched;
	Egueb_Svg_Document_Failed_Cb failed;
} Egueb_Svg_Document_Uri_Fetch_Descriptor;

typedef const char * (*Egueb_Svg_Document_String_Get_Cb)(void *user_data);

EAPI Egueb_Dom_Node * egueb_svg_document_new(void);
EAPI Egueb_Dom_String * egueb_svg_document_title_get(Egueb_Dom_Node *n);
EAPI Egueb_Dom_String * egueb_svg_document_referrer_get(Egueb_Dom_Node *n);
EAPI Egueb_Dom_String * egueb_svg_document_domain_get(Egueb_Dom_Node *n);
EAPI Egueb_Dom_String * egueb_svg_document_url_get(Egueb_Dom_Node *n);

EAPI Egueb_Dom_Node * egueb_svg_document_element_root_get(Egueb_Dom_Node *n);

EAPI void egueb_svg_document_font_size_set(Egueb_Dom_Node *n,
		double font_size);
EAPI double egueb_svg_document_font_size_get(Egueb_Dom_Node *n);

EAPI Egueb_Dom_Node * egueb_svg_document_element_get_by_iri(Egueb_Dom_Node *n,
		Egueb_Dom_String *iri);

EAPI void egueb_svg_document_feed_mouse_move(Egueb_Dom_Node *n, int x, int y);
EAPI void egueb_svg_document_feed_mouse_down(Egueb_Dom_Node *n, int button);
EAPI void egueb_svg_document_feed_mouse_up(Egueb_Dom_Node *n, int button);

EAPI void egueb_svg_document_image_load(Egueb_Dom_Node *n,
		Egueb_Dom_String *uri,
		Egueb_Svg_Document_Image_Load_Descriptor *d,
		void *user_data);
EAPI void egueb_svg_document_image_data_load(Egueb_Dom_Node *n,
		Enesim_Stream *data,
		Egueb_Svg_Document_Image_Load_Descriptor *d,
		void *user_data);
EAPI void egueb_svg_document_uri_fetch(Egueb_Dom_Node *n,
		Egueb_Dom_String *uri,
		Egueb_Svg_Document_Uri_Fetch_Descriptor *d,
		void *user_data);
EAPI void egueb_svg_document_location_get_cb_set(Egueb_Dom_Node *n,
		Egueb_Svg_Document_String_Get_Cb cb, void *user_data);
EAPI void egueb_svg_document_filename_get_cb_set(Egueb_Dom_Node *n,
		Egueb_Svg_Document_String_Get_Cb cb, void *user_data);

/* FIXME remove this as move it to the use element */
EAPI Egueb_Dom_Node * egueb_svg_document_iri_clone(Egueb_Dom_Node *n,
		Egueb_Dom_String *iri, Eina_Error *err);

/**
 * @}
 */

#endif

