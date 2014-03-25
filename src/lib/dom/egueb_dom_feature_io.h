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
#ifndef _EGUEB_DOM_FEATURE_IO_H_
#define _EGUEB_DOM_FEATURE_IO_H_

/**
 * The IO feature informs the application that file operations
 * will be requested. This includes the image loading support.
 * If a document implements this feature, the application
 * must listen to the IO_DATA_LOAD and IO_IMAGE_LOAD events.
 */

EAPI extern Egueb_Dom_String *EGUEB_DOM_FEATURE_IO_NAME;
EAPI Eina_Bool egueb_dom_feature_io_add(Egueb_Dom_Node *n);

#endif

