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
#ifndef _EGUEB_DOM_ELEMENT_PRIVATE_H_

struct _Egueb_Dom_Element
{
	Egueb_Dom_Node parent;
	Egueb_Dom_Element_Descriptor *descriptor;
	Egueb_Dom_Element_Klass *klass;
	void *external_data;
};

#endif

