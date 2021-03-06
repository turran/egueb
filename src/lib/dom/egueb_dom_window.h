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
 * @file
 * @ender_group_proto{Egueb_Dom_Event_Target}
 * @ender_group{Egueb_Dom_Window_Definitions}
 * @ender_group{Egueb_Dom_Window_Descriptor}
 * @ender_group{Egueb_Dom_Window_Descriptor_Definition}
 * @ender_group{Egueb_Dom_Window}
 */

/**
 * @defgroup Egueb_Dom_Window_Definitions Definitions
 * @ingroup Egueb_Dom_Window
 * @{
 */

/**
 * The window handle
 */
typedef struct _Egueb_Dom_Window Egueb_Dom_Window;

/**
 * @ender_name{egueb.dom.window.timeout_cb}
 */
typedef void (*Egueb_Dom_Window_Timeout_Cb)(Egueb_Dom_Window *thiz,
		void *user_data);
/**
 * @}
 * @defgroup Egueb_Dom_Window_Descriptor_Definition Window Descriptor Definition
 * @ingroup Egueb_Dom_Window_Descriptor
 * @{
 */

/**
 * @ender_name{egueb.dom.window.descriptor.destroy_cb}
 */
typedef void (*Egueb_Dom_Window_Descriptor_Destroy_Cb)(void *data);

/**
 * @ender_name{egueb.dom.window.descriptor.timeout_set_cb}
 */
typedef void * (*Egueb_Dom_Window_Descriptor_Timeout_Set_Cb)(void *data,
		Egueb_Dom_Window_Timeout_Cb cb,
		int64_t delay, void *user_data);

/**
 * @ender_name{egueb.dom.window.descriptor.timeout_clear_cb}
 */
typedef void (*Egueb_Dom_Window_Descriptor_Timeout_Clear_Cb)(void *data, void *timeout);

/**
 * @ender_name{egueb.dom.window.descriptor.size_get_cb}
 */
typedef int (*Egueb_Dom_Window_Descriptor_Size_Get_Cb)(void *data);

/**
 * @}
 * @defgroup Egueb_Dom_Window_Descriptor Window Descriptor
 * @ingroup Egueb_Dom_Window
 * @{
 */

#define EGUEB_DOM_WINDOW_DESCRIPTOR_VERSION 0

typedef struct _Egueb_Dom_Window_Descriptor
{
	int version;
	Egueb_Dom_Window_Descriptor_Destroy_Cb destroy;
	/* size */
	Egueb_Dom_Window_Descriptor_Size_Get_Cb outer_width_get;
	Egueb_Dom_Window_Descriptor_Size_Get_Cb outer_height_get;
	Egueb_Dom_Window_Descriptor_Size_Get_Cb inner_width_get;
	Egueb_Dom_Window_Descriptor_Size_Get_Cb inner_height_get;
	/* one-shot timer */
	Egueb_Dom_Window_Descriptor_Timeout_Set_Cb timeout_set;
	Egueb_Dom_Window_Descriptor_Timeout_Clear_Cb timeout_clear;
	/* repeating timer */
} Egueb_Dom_Window_Descriptor;

/**
 * @}
 * @defgroup Egueb_Dom_Window Window
 * @brief Window @ender_inherits{Egueb_Dom_Event_Target}
 * @ingroup Egueb_Dom_Group
 * @{
 */

#define EGUEB_DOM_WINDOW_DESCRIPTOR egueb_dom_window_descriptor_get()
EAPI Enesim_Object_Descriptor * egueb_dom_window_descriptor_get(void);
#define EGUEB_DOM_WINDOW(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Egueb_Dom_Window, EGUEB_DOM_WINDOW_DESCRIPTOR)

EAPI Egueb_Dom_Window * egueb_dom_window_new(
		const Egueb_Dom_Window_Descriptor *desc, void *data);

EAPI Egueb_Dom_Window * egueb_dom_window_ref(Egueb_Dom_Window *thiz);
EAPI void egueb_dom_window_unref(Egueb_Dom_Window *thiz);
EAPI void * egueb_dom_window_data_get(Egueb_Dom_Window *thiz);

EAPI void egueb_dom_window_document_set(Egueb_Dom_Window *thiz,
		Egueb_Dom_Node *doc);
EAPI Egueb_Dom_Node * egueb_dom_window_document_get(Egueb_Dom_Window *thiz);

EAPI void * egueb_dom_window_timeout_set(Egueb_Dom_Window *thiz,
		Egueb_Dom_Window_Timeout_Cb cb,
		int64_t delay, void *user_data);
EAPI void egueb_dom_window_timeout_clear(Egueb_Dom_Window *thiz,
		void *timeout);

EAPI int egueb_dom_window_outter_width_get(Egueb_Dom_Window *thiz);
EAPI int egueb_dom_window_outter_height_get(Egueb_Dom_Window *thiz);
EAPI int egueb_dom_window_inner_width_get(Egueb_Dom_Window *thiz);
EAPI int egueb_dom_window_inner_height_get(Egueb_Dom_Window *thiz);
EAPI void egueb_dom_window_resize_notify(Egueb_Dom_Window *thiz);
EAPI void egueb_dom_window_close_notify(Egueb_Dom_Window *thiz);

/**
 * @}
 */

#endif
