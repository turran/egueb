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
#ifndef _EGUEB_DOM_IMPLEMENTATION_SOURCE_H_
#define _EGUEB_DOM_IMPLEMENTATION_SOURCE_H_

typedef struct _Egueb_Dom_Implementation_Source Egueb_Dom_Implementation_Source;

typedef Egueb_Dom_Implementation * (*Egueb_Dom_Implementation_Source_Descriptor_Implementation_Get)(void);
typedef Egueb_Dom_String * (*Egueb_Dom_Implementation_Source_Descriptor_Mime_Get)(void);

typedef struct _Egueb_Dom_Implementation_Source_Descriptor
{
	Egueb_Dom_Implementation_Source_Descriptor_Implementation_Get implementation_get;
	Egueb_Dom_Implementation_Source_Descriptor_Mime_Get mime_get;
} Egueb_Dom_Implementation_Source_Descriptor;

EAPI Egueb_Dom_Implementation_Source * egueb_dom_implementation_source_new(
		const Egueb_Dom_Implementation_Source_Descriptor *d);
EAPI void egueb_dom_implementation_source_free(
		Egueb_Dom_Implementation_Source *thiz);

#endif
