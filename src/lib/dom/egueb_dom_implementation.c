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
#include "egueb_dom_node.h"
#include "egueb_dom_implementation.h"

#include "egueb_dom_document_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Egueb_Dom_Implementation
{
	int ref;
	const Egueb_Dom_Implementation_Descriptor *d;
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Implementation * egueb_dom_implementation_new(
		const Egueb_Dom_Implementation_Descriptor *d)
{
	Egueb_Dom_Implementation *thiz;

	if (!d) return NULL;

	thiz = calloc(1, sizeof(Egueb_Dom_Implementation));
	thiz->ref = 1;
	thiz->d = d;

	return thiz;
}

EAPI Egueb_Dom_Implementation * egueb_dom_implementation_ref(
		Egueb_Dom_Implementation *thiz)
{
	if (!thiz) return NULL;
	thiz->ref++;
	return thiz;
}

EAPI void egueb_dom_implementation_unref(Egueb_Dom_Implementation *thiz)
{
	if (!thiz) return;
	thiz->ref--;
	if (!thiz->ref)
		free(thiz);
}

EAPI Egueb_Dom_Node * egueb_dom_implementation_document_create(
		Egueb_Dom_Implementation *thiz)
{
	Egueb_Dom_Document *doc;
	Egueb_Dom_Node *ret;

	if (!thiz) return NULL;
	if (!thiz->d->document_create) return NULL;

	ret = thiz->d->document_create();
	if (!ret) return NULL;

	if (egueb_dom_node_type_get(ret) != EGUEB_DOM_NODE_TYPE_DOCUMENT)
	{
		egueb_dom_node_unref(ret);
		return NULL;
	}

	doc = EGUEB_DOM_DOCUMENT(ret);
	doc->i = egueb_dom_implementation_ref(thiz);

	return ret;
}


/*
boolean            hasFeature(in DOMString feature, 
                                in DOMString version);
  // Introduced in DOM Level 2:
  DocumentType       createDocumentType(in DOMString qualifiedName, 
                                        in DOMString publicId, 
                                        in DOMString systemId)
                                        raises(DOMException);
  // Introduced in DOM Level 2:
  Document           createDocument(in DOMString namespaceURI, 
                                    in DOMString qualifiedName, 
                                    in DocumentType doctype)
                                        raises(DOMException);
  // Introduced in DOM Level 3:
  DOMObject          getFeature(in DOMString feature, 
                                in DOMString version);
*/
