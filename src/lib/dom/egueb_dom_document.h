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

#ifndef _EGUEB_DOM_DOCUMENT_H_
#define _EGUEB_DOM_DOCUMENT_H_

#include "egueb_dom_string.h"
#include "egueb_dom_node.h"

/**
 * @file
 * @ender_group{Egueb_Dom_Document}
 */

/**
 * @defgroup Egueb_Dom_Document Document
 * @brief Document @ender_inherits{Egueb_Dom_Node}
 * @ingroup Egueb_Dom_Group
 * @{
 */

EAPI Egueb_Dom_Node * egueb_dom_document_element_ns_create(Egueb_Dom_Node *n,
		Egueb_Dom_String *ns_uri, Egueb_Dom_String *qname,
		Eina_Error *err);
EAPI Egueb_Dom_Node * egueb_dom_document_element_create(Egueb_Dom_Node *n,
		Egueb_Dom_String *name, Eina_Error *err);

EAPI Egueb_Dom_Node * egueb_dom_document_document_element_get(Egueb_Dom_Node *n);
EAPI Egueb_Dom_String * egueb_dom_document_uri_get(Egueb_Dom_Node *n);
EAPI void egueb_dom_document_uri_set(Egueb_Dom_Node *n, Egueb_Dom_String *uri);

EAPI Egueb_Dom_Node * egueb_dom_document_document_fragment_create(
		Egueb_Dom_Node *n);
EAPI void egueb_dom_document_element_set(Egueb_Dom_Node *n,
		Egueb_Dom_Node *element);
EAPI Egueb_Dom_Node * egueb_dom_document_element_get_by_id(Egueb_Dom_Node *n,
		Egueb_Dom_String *id, Eina_Error *err);
EAPI Egueb_Dom_Node * egueb_dom_document_element_get_by_iri(Egueb_Dom_Node *n,
		Egueb_Dom_String *id, Eina_Error *err);
EAPI Egueb_Dom_Node * egueb_dom_document_node_adopt(Egueb_Dom_Node *n,
		Egueb_Dom_Node *adopted, Eina_Error *err);

EAPI void egueb_dom_document_process(Egueb_Dom_Node *n);
EAPI Eina_Bool egueb_dom_document_is_processing(Egueb_Dom_Node *n);
EAPI Eina_Bool egueb_dom_document_needs_process(Egueb_Dom_Node *n);
EAPI void egueb_dom_document_process_queue_clear(Egueb_Dom_Node *n);

/**
 * @}
 */

#endif
