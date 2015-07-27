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
#ifndef _EGUEB_DOM_EVENT_MULTIMEDIA_H_
#define _EGUEB_DOM_EVENT_MULTIMEDIA_H_

/**
 * @file
 * @ender_group{Egueb_Dom_Event_Multimedia}
 */

/**
 * @defgroup Egueb_Dom_Event_Multimedia Multimedia Event
 * @brief Multimedia Event @ender_inherits{Egueb_Dom_Event}
 * @ingroup Egueb_Dom_Event
 * @{
 */

EAPI extern Egueb_Dom_String *EGUEB_DOM_EVENT_MULTIMEDIA_VIDEO;
EAPI extern Egueb_Dom_String *EGUEB_DOM_EVENT_MULTIMEDIA_AUDIO;

EAPI Egueb_Dom_Media_Notifier * egueb_dom_event_multimedia_notifier_get(Egueb_Dom_Event *e);
EAPI Egueb_Dom_Media_Provider * egueb_dom_event_multimedia_provider_get(Egueb_Dom_Event *e);
EAPI void egueb_dom_event_multimedia_provider_set(Egueb_Dom_Event *e, Egueb_Dom_Media_Provider *sc);

EAPI Egueb_Dom_Event * egueb_dom_event_multimedia_video_new(
		Egueb_Dom_Media_Notifier *notifier,
		Enesim_Renderer *image);
EAPI Enesim_Renderer * egueb_dom_event_multimedia_video_renderer_get(Egueb_Dom_Event *e);

EAPI Egueb_Dom_Event * egueb_dom_event_multimedia_audio_new(
		Egueb_Dom_Media_Notifier *notifier);


/**
 * @}
 */

#endif
