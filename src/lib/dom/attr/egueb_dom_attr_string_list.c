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
#include "egueb_dom_private.h"

#include "egueb_dom_string.h"
#include "egueb_dom_main.h"
#include "egueb_dom_attr.h"
#include "egueb_dom_value.h"
#include "egueb_dom_value_dom_string.h"
#include "egueb_dom_list.h"
#include "egueb_dom_string_list.h"
#include "egueb_dom_attr_string_list.h"
#include "egueb_dom_event_mutation.h"

#include "egueb_dom_attr_private.h"
#include "egueb_dom_attr_object_private.h"
#include "egueb_dom_event_mutation_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
EGUEB_DOM_ATTR_OBJECT_BOILERPLATE(Egueb_Dom_Attr_String_List,
		egueb_dom_string_list, egueb_dom_attr_string_list)
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_dom_attr_string_list_new(Egueb_Dom_String *name,
		Egueb_Dom_String *ns, Egueb_Dom_List *def, Eina_Bool animatable,
		Eina_Bool stylable, Eina_Bool inheritable)
{
	Egueb_Dom_Node *n;
	
	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_attr_string_list);
	egueb_dom_attr_init(n, name, NULL, animatable, stylable, inheritable);
	if (def)
		egueb_dom_attr_set(n, EGUEB_DOM_ATTR_TYPE_DEFAULT, def);
	return n;
}

EAPI void egueb_dom_attr_string_list_prepend(Egueb_Dom_Node *attr,
		Egueb_Dom_Attr_Type type, Egueb_Dom_String *str)
{
	Egueb_Dom_List *l;

	egueb_dom_attr_get(attr, type, &l);
	if (!l) l = egueb_dom_list_new(egueb_dom_value_dom_string_descriptor_get());
	egueb_dom_list_item_prepend(l, str);
	egueb_dom_attr_set(attr, type, l);
}

EAPI void egueb_dom_attr_string_list_append(Egueb_Dom_Node *attr,
		Egueb_Dom_Attr_Type type, Egueb_Dom_String *str)
{
	Egueb_Dom_List *l;

	egueb_dom_attr_get(attr, type, &l);
	if (!l) l = egueb_dom_list_new(egueb_dom_value_dom_string_descriptor_get());
	egueb_dom_list_item_append(l, str);
	egueb_dom_attr_set(attr, type, l);
}

EAPI void egueb_dom_attr_string_list_clear(Egueb_Dom_Node *attr,
		Egueb_Dom_Attr_Type type)
{
	Egueb_Dom_List *l;

	egueb_dom_attr_get(attr, type, &l);
	if (l)
	{
		egueb_dom_list_clear(l);
		egueb_dom_attr_set(attr, type, l);
	}
}
