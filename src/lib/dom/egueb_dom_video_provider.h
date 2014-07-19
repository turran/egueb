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

#ifndef _EGUEB_DOM_VIDEO_PROVIDER_H_
#define _EGUEB_DOM_VIDEO_PROVIDER_H_

/**
 * @defgroup Egueb_Dom_Video_Provider_Descriptor_Definition Video Provider Descriptor Definition
 * @ingroup Egueb_Dom_Video_Provider_Descriptor
 * @{
 */

typedef void * (*Egueb_Dom_Video_Provider_Descriptor_Create)(void);
typedef void (*Egueb_Dom_Video_Provider_Descriptor_Destroy)(void *data);

typedef void (*Egueb_Dom_Video_Provider_Descriptor_Open)(void *data, Egueb_Dom_String *uri);
typedef void (*Egueb_Dom_Video_Provider_Descriptor_Close)(void *data);

typedef void (*Egueb_Dom_Video_Provider_Descriptor_Play)(void *data);
typedef void (*Egueb_Dom_Video_Provider_Descriptor_Pause)(void *data);

/**
 * @}
 * @defgroup Egueb_Dom_Video_Provider_Descriptor Video Provider Descriptor
 * @ingroup Egueb_Dom_Video_Provider
 * @{
 */

typedef struct _Egueb_Dom_Video_Provider_Descriptor
{
	Egueb_Dom_Video_Provider_Descriptor_Create create;
	Egueb_Dom_Video_Provider_Descriptor_Destroy destroy;
	Egueb_Dom_Video_Provider_Descriptor_Open open;
	Egueb_Dom_Video_Provider_Descriptor_Close close;
	Egueb_Dom_Video_Provider_Descriptor_Play play;
	Egueb_Dom_Video_Provider_Descriptor_Pause pause;
} Egueb_Dom_Video_Provider_Descriptor;

/**
 * @}
 * @defgroup Egueb_Dom_Video_Provider_Notifier Video Provider Notifier
 * @ingroup Egueb_Dom_Video_Provider
 * @{
 */

typedef struct _Egueb_Dom_Video_Provider_Notifier
{
} Egueb_Dom_Video_Provider_Notifier;


/**
 * @}
 * @defgroup Egueb_Dom_Video_Provider Video Provider
 * @{
 */

typedef struct _Egueb_Dom_Video_Provider Egueb_Dom_Video_Provider;

EAPI Egueb_Dom_Video_Provider * egueb_dom_video_provider_new(
		const Egueb_Dom_Video_Provider_Descriptor *desc,
		const Egueb_Dom_Video_Provider_Notifier *notifier,
		Enesim_Renderer *image, Egueb_Dom_Node *n);
EAPI Egueb_Dom_Video_Provider * egueb_dom_video_provider_ref(Egueb_Dom_Video_Provider *thiz);
EAPI void egueb_dom_video_provider_unref(Egueb_Dom_Video_Provider *thiz);
EAPI void * egueb_dom_video_provider_data_get(Egueb_Dom_Video_Provider *thiz);
EAPI Enesim_Renderer * egueb_dom_video_provider_renderer_get(Egueb_Dom_Video_Provider *thiz);

EAPI void egueb_dom_video_provider_open(Egueb_Dom_Video_Provider *thiz, Egueb_Dom_String *uri);
EAPI void egueb_dom_video_provider_close(Egueb_Dom_Video_Provider *thiz);
EAPI void egueb_dom_video_provider_play(Egueb_Dom_Video_Provider *thiz);
EAPI void egueb_dom_video_provider_pause(Egueb_Dom_Video_Provider *thiz);

/**
 * @}
 */

#endif
