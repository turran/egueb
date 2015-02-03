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
#ifndef _EGUEB_DOM_REGISTRY_H_
#define _EGUEB_DOM_REGISTRY_H_

EAPI void egueb_dom_registry_implementation_add(
		Egueb_Dom_Implementation *i);
EAPI Egueb_Dom_List * egueb_dom_registry_mime_get (void);
EAPI Egueb_Dom_Implementation * egueb_dom_registry_implementation_get_by_mime(
		Egueb_Dom_String *mime);

#endif

