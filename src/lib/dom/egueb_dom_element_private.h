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

#ifndef _EGUEB_DOM_ELEMENT_PRIVATE_H_
#define _EGUEB_DOM_ELEMENT_PRIVATE_H_

typedef struct _Egueb_Dom_Element
{
	Egueb_Dom_Node parent;
	void *external_data;
} Egueb_Dom_Element;

typedef struct _Egueb_Dom_Element_Class
{
	Egueb_Dom_Node_Class parent;
} Egueb_Dom_Element_Class;

#define EGUEB_DOM_ELEMENT_DESCRIPTOR egueb_dom_element_descriptor_get()
#define EGUEB_DOM_ELEMENT_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,			\
		Egueb_Dom_Element_Class, EGUEB_DOM_ELEMENT_DESCRIPTOR)
#define EGUEB_DOM_ELEMENT(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Egueb_Dom_Element, EGUEB_DOM_ELEMENT_DESCRIPTOR)

#endif
