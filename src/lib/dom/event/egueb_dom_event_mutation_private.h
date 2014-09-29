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
#ifndef _EGUEB_DOM_EVENT_MUTATION_PRIVATE_H_
#define _EGUEB_DOM_EVENT_MUTATION_PRIVATE_H_

#include "egueb_dom_string.h"
#include "egueb_dom_event.h"
#include "egueb_dom_event_private.h"

typedef struct _Egueb_Dom_Event_Mutation
{
	Egueb_Dom_Event parent;
	Egueb_Dom_Node *related;
	const Egueb_Dom_Value *prev_value;
	const Egueb_Dom_Value *new_value;
	Egueb_Dom_String *attr;
	Egueb_Dom_Event_Mutation_Attr_Type attr_type;
	/* to know if we set an animatable value, the
	 * default, the styled, the base, etc
	 */
	Egueb_Dom_Attr_Type attr_modification_type;
	/* if this flag is set, the event should not be taken
	 * into account on whenever the document wants
	 * to process again such node
	 */
	Eina_Bool process_prevent;
} Egueb_Dom_Event_Mutation;

typedef struct _Egueb_Dom_Event_Mutation_Class
{
	Egueb_Dom_Event parent;
} Egueb_Dom_Event_Mutation_Class;

#define EGUEB_DOM_EVENT_MUTATION_DESCRIPTOR egueb_dom_event_mutation_descriptor_get()
Enesim_Object_Descriptor * egueb_dom_event_mutation_descriptor_get(void);

#define EGUEB_DOM_EVENT_MUTATION_CLASS_GET(o) EGUEB_DOM_EVENT_MUTATION_CLASS(	\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_DOM_EVENT_MUTATION_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,		\
		Egueb_Dom_Event_Mutation_Class,					\
		EGUEB_DOM_EVENT_MUTATION_DESCRIPTOR)
#define EGUEB_DOM_EVENT_MUTATION(o) ENESIM_OBJECT_INSTANCE_CHECK(o,		\
		Egueb_Dom_Event_Mutation, EGUEB_DOM_EVENT_MUTATION_DESCRIPTOR)


Egueb_Dom_Event * egueb_dom_event_mutation_node_inserted_new(Egueb_Dom_Node *parent);
Egueb_Dom_Event * egueb_dom_event_mutation_node_inserted_into_document_new(
		void);
Egueb_Dom_Event * egueb_dom_event_mutation_node_removed_new(Egueb_Dom_Node *parent);
Egueb_Dom_Event * egueb_dom_event_mutation_node_removed_from_document_new(void);
Egueb_Dom_Event * egueb_dom_event_mutation_character_data_modified_new(void);
Egueb_Dom_Event * egueb_dom_event_mutation_node_destroyed_new(void);
Egueb_Dom_Event * egueb_dom_event_mutation_attr_modified_new(
		Egueb_Dom_Node *related,
		const Egueb_Dom_Value *prev_value,
		const Egueb_Dom_Value *new_value,
		Egueb_Dom_String *attr,
		Egueb_Dom_Event_Mutation_Attr_Type attr_type,
		Egueb_Dom_Attr_Type attr_modification_type);

#endif
