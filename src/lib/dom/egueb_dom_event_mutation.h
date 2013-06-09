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
#ifndef _EGUEB_DOM_EVENT_MUTATION_H_
#define _EGUEB_DOM_EVENT_MUTATION_H_

#include "egueb_dom_string.h"
#include "egueb_dom_event.h"

typedef enum _Egueb_Dom_Event_Mutation_Attr_Type
{
	EGUEB_DOM_EVENT_MUTATION_ATTR_TYPE_ADDITION = 1,
	EGUEB_DOM_EVENT_MUTATION_ATTR_TYPE_MODIFICATION = 2,
	EGUEB_DOM_EVENT_MUTATION_ATTR_TYPE_REMOVAL = 3,
} Egueb_Dom_Event_Mutation_Attr_Type;

extern Egueb_Dom_String * EGUEB_DOM_EVENT_MUTATION_NODE_DESTROYED;
extern Egueb_Dom_String * EGUEB_DOM_EVENT_MUTATION_SUBTREE_MODIFIED;
extern Egueb_Dom_String * EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED;
extern Egueb_Dom_String * EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED;
extern Egueb_Dom_String * EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED_FROM_DOCUMENT;
extern Egueb_Dom_String * EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED_INTO_DOCUMENT;
extern Egueb_Dom_String * EGUEB_DOM_EVENT_MUTATION_ATTR_MODIFIED;
extern Egueb_Dom_String * EGUEB_DOM_EVENT_MUTATION_CHARACTER_DATA_MODIFIED;

EAPI Eina_Error egueb_dom_event_mutation_related_get(Egueb_Dom_Event *e, Egueb_Dom_Node **related);

EAPI void egueb_dom_event_mutation_value_prev_string_get(Egueb_Dom_Event *e,
		Egueb_Dom_String **s);
EAPI void egueb_dom_event_mutation_value_new_string_get(Egueb_Dom_Event *e,
		Egueb_Dom_String **s);
EAPI void egueb_dom_event_mutation_value_new_get(Egueb_Dom_Event *e,
		const Egueb_Dom_Value **v);
EAPI void egueb_dom_event_mutation_attr_name_get(Egueb_Dom_Event *e,
		Egueb_Dom_String **s);
EAPI void egueb_dom_event_mutation_attr_type_get(Egueb_Dom_Event *e,
		Egueb_Dom_Event_Mutation_Attr_Type *attr_type);
EAPI void egueb_dom_event_mutation_attr_modification_type_get(Egueb_Dom_Event *e,
		Egueb_Dom_Attr_Type *attr_modification_type);

EAPI Eina_Bool egueb_dom_event_mutation_is_attr_modified(
		const Egueb_Dom_String *str);
EAPI void egueb_dom_event_mutation_process_prevent(Egueb_Dom_Event *thiz);
EAPI Eina_Bool egueb_dom_event_mutation_process_prevented(Egueb_Dom_Event *thiz);


#endif
