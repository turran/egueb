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

#ifndef _EGUEB_DOM_ELEMENT_EXTERNAL_H_
#define _EGUEB_DOM_ELEMENT_EXTERNAL_H_

typedef Egueb_Dom_Node * (*Egueb_Dom_Element_External_Descriptor_Ctor)(void *data);
typedef void (*Egueb_Dom_Element_External_Descriptor_Init)(Egueb_Dom_Node *node, void *data);
typedef void (*Egueb_Dom_Element_External_Descriptor_Deinit)(
		Egueb_Dom_Node *node, void *data);
typedef Egueb_Dom_String * (*Egueb_Dom_Element_External_Descriptor_Tag_Name_Get)(
		Egueb_Dom_Node *node, void *data);
typedef Eina_Bool (*Egueb_Dom_Element_External_Descriptor_Process)(
		Egueb_Dom_Node *node, void *data);
typedef Eina_Bool (*Egueb_Dom_Element_External_Descriptor_Child_Appendable)(
		Egueb_Dom_Node *node, void *data, Egueb_Dom_Node *child);

#define EGUEB_DOM_ELEMENT_EXTERNAL_DESCRIPTOR_VERSION 0
typedef struct _Egueb_Dom_Element_External_Descriptor
{
	int version;
	Egueb_Dom_Element_External_Descriptor_Ctor ctor;
	Egueb_Dom_Element_External_Descriptor_Init init;
	Egueb_Dom_Element_External_Descriptor_Deinit deinit;
	Egueb_Dom_Element_External_Descriptor_Tag_Name_Get tag_name_get;
	Egueb_Dom_Element_External_Descriptor_Child_Appendable child_appendable;
	Egueb_Dom_Element_External_Descriptor_Process process;
} Egueb_Dom_Element_External_Descriptor;

EAPI Eina_Bool egueb_dom_element_is_external(Egueb_Dom_Node *n);
EAPI void * egueb_dom_element_external_data_get(Egueb_Dom_Node *n);
EAPI Egueb_Dom_Node * egueb_dom_element_external_new(
		const Egueb_Dom_Element_External_Descriptor *descriptor,
		void *data);

#endif
