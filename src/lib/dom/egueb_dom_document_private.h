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

#ifndef _EGUEB_DOM_DOCUMENT_PRIVATE_H_
#define _EGUEB_DOM_DOCUMENT_PRIVATE_H_

#include "egueb_dom_node_private.h"
#include "egueb_dom_implementation.h"

typedef struct _Egueb_Dom_Document
{
	Egueb_Dom_Node base;
	/* private */
	Eina_Hash *ids;
	Eina_List *current_enqueued;
	/* TODO use this
	Eina_List *later_enqueued;
	*/
	int current_run;
	Eina_Bool processing;
	Egueb_Dom_Implementation *i;
} Egueb_Dom_Document;

typedef Egueb_Dom_Node * (*Egueb_Dom_Document_Element_Create)(
		Egueb_Dom_Document *thiz, const char *name);
typedef Eina_Bool (*Egueb_Dom_Document_Child_Appendable)(Egueb_Dom_Document *thiz,
		Egueb_Dom_Node *child);
typedef struct _Egueb_Dom_Document_Class
{
	Egueb_Dom_Node_Class base;
	Egueb_Dom_Document_Element_Create element_create;
	Egueb_Dom_Document_Child_Appendable child_appendable;
} Egueb_Dom_Document_Class;

#define EGUEB_DOM_DOCUMENT_DESCRIPTOR egueb_dom_document_descriptor_get()

#define EGUEB_DOM_DOCUMENT_CLASS_GET(o) EGUEB_DOM_DOCUMENT_CLASS(		\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_DOM_DOCUMENT_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,		\
		Egueb_Dom_Document_Class, EGUEB_DOM_DOCUMENT_DESCRIPTOR)
#define EGUEB_DOM_DOCUMENT(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Egueb_Dom_Document, EGUEB_DOM_DOCUMENT_DESCRIPTOR)

int egueb_dom_document_current_run_get(Egueb_Dom_Node *n);
void egueb_dom_document_element_enqueue(Egueb_Dom_Node *n,
		Egueb_Dom_Node *node);
void egueb_dom_document_element_dequeue(Egueb_Dom_Node *n,
		Egueb_Dom_Node *node);

#endif

