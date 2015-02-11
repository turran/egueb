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

#ifndef _EGUEB_DOM_MEDIA_NOTIFIER_H_
#define _EGUEB_DOM_MEDIA_NOTIFIER_H_

/**
 * @defgroup Egueb_Dom_Media_Notifier_Descriptor_Definition Media Notifier Descriptor Definition
 * @ingroup Egueb_Dom_Media_Notifier_Descriptor
 * @{
 */

typedef void (*Egueb_Dom_Media_Notifier_Descriptor_Destroy)(void *data);

/**
 * @}
 * @defgroup Egueb_Dom_Media_Notifier_Descriptor Media Notifier Descriptor
 * @ingroup Egueb_Dom_Media_Notifier
 * @{
 */

#define EGUEB_DOM_MEDIA_NOTIFIER_DESCRIPTOR_VERSION 0
typedef struct _Egueb_Dom_Media_Notifier_Descriptor
{
	int version;
	Egueb_Dom_Media_Notifier_Descriptor_Destroy destroy;
} Egueb_Dom_Media_Notifier_Descriptor;

/**
 * @}
 * @defgroup Egueb_Dom_Media_Notifier Media Notifier
 * @ingroup Egueb_Dom_Group
 * @{
 */

typedef struct _Egueb_Dom_Media_Notifier Egueb_Dom_Media_Notifier;

EAPI Egueb_Dom_Media_Notifier * egueb_dom_media_notifier_new(
		const Egueb_Dom_Media_Notifier_Descriptor *desc,
		void *data);

EAPI Egueb_Dom_Media_Notifier * egueb_dom_media_notifier_ref(Egueb_Dom_Media_Notifier *thiz);
EAPI void egueb_dom_media_notifier_unref(Egueb_Dom_Media_Notifier *thiz);
EAPI void * egueb_dom_media_notifier_data_get(Egueb_Dom_Media_Notifier *thiz);

/**
 * @}
 */

#endif
