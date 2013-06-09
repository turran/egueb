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

#ifndef _EGUEB_DOM_CHARACTER_DATA_PRIVATE_H_
#define _EGUEB_DOM_CHARACTER_DATA_PRIVATE_H_

#include "egueb_dom_string.h"
#include "egueb_dom_node_private.h"

#define EGUEB_DOM_CHARACTER_DATA_DESCRIPTOR egueb_dom_character_data_descriptor_get()
Enesim_Object_Descriptor *egueb_dom_character_data_descriptor_get(void);
#define EGUEB_DOM_CHARACTER_DATA_CLASS_GET(o) EGUEB_DOM_CHARACTER_DATA_CLASS(		\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_DOM_CHARACTER_DATA_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,		\
		Egueb_Dom_Character_Data_Class, EGUEB_DOM_CHARACTER_DATA_DESCRIPTOR)
#define EGUEB_DOM_CHARACTER_DATA(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Egueb_Dom_Character_Data, EGUEB_DOM_CHARACTER_DATA_DESCRIPTOR)

typedef struct _Egueb_Dom_Character_Data
{
	Egueb_Dom_Node parent;
	Enesim_Text_Buffer *buffer;
} Egueb_Dom_Character_Data;

typedef struct _Egueb_Dom_Character_Data_Class
{
	Egueb_Dom_Node_Class parent;
} Egueb_Dom_Character_Data_Class;

#endif

