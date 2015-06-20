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

#ifndef _EGUEB_DOM_WINDOW_H_
#define _EGUEB_DOM_WINDOW_H_

/**
 * @defgroup Egueb_Dom_Window Window
 * @ingroup Egueb_Dom_Group
 * @{
 */

typedef struct _Egueb_Dom_Window Egueb_Dom_Window;

/**
 * @}
 * @defgroup Egueb_Dom_Window_Descriptor_Definition Window Descriptor Definition
 * @ingroup Egueb_Dom_Window_Descriptor
 * @{
 */

typedef void (*Egueb_Dom_Window_Descriptor_Destroy)(void *data);

typedef void (*Egueb_Dom_Window_Descriptor_Timeout_Cb)(Egueb_Dom_Window *thiz,
		void *user_data);
typedef void * (*Egueb_Dom_Window_Descriptor_Timeout_Set)(void *data,
		Egueb_Dom_Window_Descriptor_Timeout_Cb cb,
		int64_t delay, void *user_data);
typedef void (*Egueb_Dom_Window_Descriptor_Timeout_Clear)(void *data, void *timeout);

typedef struct _Egueb_Dom_Window_Descriptor
{
	Egueb_Dom_Window_Descriptor_Destroy destroy;
	/* one-shot timer */
	Egueb_Dom_Window_Descriptor_Timeout_Set timeout_set;
	Egueb_Dom_Window_Descriptor_Timeout_Clear timeout_clear;
	/* repeating timer */
} Egueb_Dom_Window_Descriptor;


/**
 * @}
 * @defgroup Egueb_Dom_Window_Descriptor Window Descriptor
 * @ingroup Egueb_Dom_Window
 * @{
 */

EAPI Egueb_Dom_Window * egueb_dom_window_new(
		const Egueb_Dom_Window_Descriptor *desc,
		Egueb_Dom_Node *doc, void *data);
EAPI Egueb_Dom_Window * egueb_dom_window_ref(Egueb_Dom_Window *thiz);
EAPI void egueb_dom_window_unref(Egueb_Dom_Window *thiz);
EAPI void * egueb_dom_window_data_get(Egueb_Dom_Window *thiz);
EAPI Egueb_Dom_Node * egueb_dom_window_document_get(Egueb_Dom_Window *thiz);
EAPI void * egueb_dom_window_timeout_set(Egueb_Dom_Window *thiz,
		Egueb_Dom_Window_Descriptor_Timeout_Cb cb,
		int64_t delay, void *user_data);
EAPI void egueb_dom_window_timeout_clear(Egueb_Dom_Window *thiz,
		void *timeout);

/**
 * @}
 */

#endif
