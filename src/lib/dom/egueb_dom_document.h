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

EAPI Eina_Error egueb_dom_document_element_create(Egueb_Dom_Node *n,
		Egueb_Dom_String *name, Egueb_Dom_Node **ret);
EAPI Eina_Error egueb_dom_document_element_get(Egueb_Dom_Node *n,
		Egueb_Dom_Node **element);
EAPI Eina_Error egueb_dom_document_element_set(Egueb_Dom_Node *n,
		Egueb_Dom_Node *element);
EAPI Eina_Error egueb_dom_document_element_get_by_id(Egueb_Dom_Node *n,
		Egueb_Dom_String *id, Egueb_Dom_Node **element);

EAPI void egueb_dom_document_process(Egueb_Dom_Node *n);

#endif
