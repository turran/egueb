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
#ifndef _EGUEB_DOM_IMPLEMENTATION_H_
#define _EGUEB_DOM_IMPLEMENTATION_H_

typedef Egueb_Dom_Node * (*Egueb_Dom_Implementation_Descriptor_Document_Create)(void);

#define EGUEB_DOM_IMPLEMENTATION_DESCRIPTOR_VERSION 0
typedef struct _Egueb_Dom_Implementation_Descriptor
{
	int version;
	Egueb_Dom_Implementation_Descriptor_Document_Create document_create;
} Egueb_Dom_Implementation_Descriptor;

typedef struct _Egueb_Dom_Implementation Egueb_Dom_Implementation;

EAPI Egueb_Dom_Implementation * egueb_dom_implementation_new(
		const Egueb_Dom_Implementation_Descriptor *d);
EAPI Egueb_Dom_Implementation * egueb_dom_implementation_ref(
		Egueb_Dom_Implementation *thiz);
EAPI void egueb_dom_implementation_unref(Egueb_Dom_Implementation *thiz);
EAPI Egueb_Dom_Node * egueb_dom_implementation_document_create(
		Egueb_Dom_Implementation *thiz);

#endif
