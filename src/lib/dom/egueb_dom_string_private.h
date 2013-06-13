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

#ifndef _EGUEB_DOM_STRING_PRIVATE_H_
#define _EGUEB_DOM_STRING_PRIVATE_H_

struct _Egueb_Dom_String
{
	Eina_Bool owned;
	char *str;
	int ref;
};

#define EGUEB_DOM_STRING_ATTR_DESCRIPTOR egueb_dom_string_descriptor_get()
Enesim_Object_Descriptor * egueb_dom_string_descriptor_get(void);

#define EGUEB_DOM_STRING_STATIC(value) 			\
	{						\
		/* .owned */ EINA_FALSE,		\
		/* .str */ (char *)value,		\
		/* .ref */ 1				\
	}

#endif
