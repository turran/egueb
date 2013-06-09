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

#ifndef _EGUEB_DOM_VALUE_DOM_STRING_H_
#define _EGUEB_DOM_VALUE_DOM_STRING_H_

EAPI const Egueb_Dom_Value_Descriptor *
		egueb_dom_value_dom_string_descriptor_get(void);
EAPI Egueb_Dom_String * egueb_dom_value_dom_string_dup(Egueb_Dom_Value *v);
EAPI const Egueb_Dom_String * egueb_dom_value_dom_string_get(Egueb_Dom_Value *v);
EAPI void egueb_dom_value_dom_string_set(Egueb_Dom_Value *v, Egueb_Dom_String *str);

#endif
