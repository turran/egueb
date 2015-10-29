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
#ifndef _EGUEB_DOM_ATTR_STRINGLIST_H_
#define _EGUEB_DOM_ATTR_STRING_LIST_H_

EAPI Egueb_Dom_Node * egueb_dom_attr_string_list_new(Egueb_Dom_String *name,
		Egueb_Dom_String *ns, Egueb_Dom_List *def,
		Eina_Bool animatable, Eina_Bool stylable,
		Eina_Bool inheritable);

EAPI void egueb_dom_attr_string_list_prepend(Egueb_Dom_Node *attr,
		Egueb_Dom_Attr_Type type, Egueb_Dom_String *str);
EAPI void egueb_dom_attr_string_list_append(Egueb_Dom_Node *attr,
		Egueb_Dom_Attr_Type type, Egueb_Dom_String *str);
EAPI void egueb_dom_attr_string_list_clear(Egueb_Dom_Node *attr,
		Egueb_Dom_Attr_Type type);

#endif
