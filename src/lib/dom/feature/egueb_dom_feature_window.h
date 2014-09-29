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
#ifndef _EGUEB_DOM_FEATURE_WINDOW_H_
#define _EGUEB_DOM_FEATURE_WINDOW_H_

/**
 * @defgroup Egueb_Dom_Feature_Window Window
 * @brief Node window features
 *
 * For SVG we need to have a WINDOW_RELATIVE hint, that way the size
 * of the svg depends on size of the window
 * another case would be the CONTENT_RELATIVE hint, that way the size
 * of the window depends on the size of the content
 * @ingroup Egueb_Dom_Feature
 * @{
 */

/* get the hints? */
/* set the window size */
/* get the final size */

EAPI extern Egueb_Dom_String *EGUEB_DOM_FEATURE_WINDOW_NAME;

typedef enum _Egueb_Dom_Feature_Window_Type
{
	/** The window decides the size of the content, even if the content
	 * might have a geometry relative to the window
	 */
	EGUEB_DOM_FEATURE_WINDOW_TYPE_MASTER,
	/** The window size depends on the size of the content, for that
	 * request the size hints of the content
	 */
	EGUEB_DOM_FEATURE_WINDOW_TYPE_SLAVE
} Egueb_Dom_Feature_Window_Type;

typedef Eina_Bool (*Egueb_Dom_Feature_Window_Descriptor_Type_Get)(
		Egueb_Dom_Node *n, Egueb_Dom_Feature_Window_Type *type);

typedef Eina_Bool (*Egueb_Dom_Feature_Window_Descriptor_Content_Size_Set)(
		Egueb_Dom_Node *n, int w, int h);
typedef Eina_Bool (*Egueb_Dom_Feature_Window_Descriptor_Content_Size_Get)(
		Egueb_Dom_Node *n, int *w, int *h);

typedef struct _Egueb_Dom_Feature_Window_Descriptor
{
	Egueb_Dom_Feature_Window_Descriptor_Type_Get type_get;
	Egueb_Dom_Feature_Window_Descriptor_Content_Size_Set content_size_set;
	Egueb_Dom_Feature_Window_Descriptor_Content_Size_Get content_size_get;
} Egueb_Dom_Feature_Window_Descriptor;

EAPI Eina_Bool egueb_dom_feature_window_type_get(
		Egueb_Dom_Feature *f, Egueb_Dom_Feature_Window_Type *type);
EAPI Eina_Bool egueb_dom_feature_window_content_size_set(Egueb_Dom_Feature *f, int w, int h);
EAPI Eina_Bool egueb_dom_feature_window_content_size_get(Egueb_Dom_Feature *f, int *w, int *h);
EAPI Eina_Bool egueb_dom_feature_window_add(Egueb_Dom_Node *n,
		const Egueb_Dom_Feature_Window_Descriptor *d);

/**
 * @}
 */

#endif
